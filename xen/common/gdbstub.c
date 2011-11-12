/*
 * Copyright (C) 2005 Jimi Xenidis <jimix@watson.ibm.com>, IBM Corporation
 * Copyright (C) 2006 Isaku Yamahata <yamahata at valinux co jp>
 *                    VA Linux Systems Japan. K.K.
 * 
 * gdbstub arch neutral part
 * Based on x86 cdb (xen/arch/x86/cdb.c) and ppc gdbstub(xen/common/gdbstub.c)
 * But extensively modified.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/*
 * gdbstub: implements the architecture independant parts of the
 * gdb remote protocol.
 */

/* We try to avoid assuming much about what the rest of the system is
   doing.  In particular, dynamic memory allocation is out of the
   question. */

/* Resuming after we've stopped used to work, but more through luck
   than any actual intention.  It doesn't at the moment. */

#include <xen/lib.h>
#include <xen/spinlock.h>
#include <xen/serial.h>
#include <xen/irq.h>
#include <asm/debugger.h>
#include <xen/init.h>
#include <xen/smp.h>
#include <xen/console.h>

/* Printk isn't particularly safe just after we've trapped to the
   debugger. so avoid it. */
#define dbg_printk(...)
/*#define dbg_printk(...)   printk(__VA_ARGS__)*/

#define GDB_RETRY_MAX   10

static char opt_gdb[30] = "none";
string_param("gdb", opt_gdb);

/* value <-> char (de)serialzers */
char
hex2char(unsigned long x)
{
    const char array[] = "0123456789abcdef";

    return array[x & 15];
}

int
char2hex(unsigned char c)
{
    if ( (c >= '0') && (c <= '9') )
        return c - '0';
    else if ( (c >= 'a') && (c <= 'f') )
        return c - 'a' + 10;
    else if ( (c >= 'A') && (c <= 'F') )
        return c - 'A' + 10;
    else
        BUG();
    return -1;
}

char
str2hex(const char *str)
{
    return (char2hex(str[0]) << 4) | char2hex(str[1]);
}

unsigned long
str2ulong(const char *str, unsigned long bytes)
{
    unsigned long x = 0;
    unsigned long i = 0;

    while ( *str && (i < (bytes * 2)) )
    {
        x <<= 4;
        x += char2hex(*str);
        ++str;
        ++i;
    }

    return x;
}

/* gdb io wrappers */
static signed long
gdb_io_write(const char *buf, unsigned long len, struct gdb_context *ctx)
{
    int i;
    for ( i = 0; i < len; i++ )
        serial_putc(ctx->serhnd, buf[i]);
    return i;
}

static int
gdb_io_write_char(u8 data, struct gdb_context *ctx)
{
    return gdb_io_write((char*)&data, 1, ctx);
}

static unsigned char
gdb_io_read(struct gdb_context *ctx)
{
    return serial_getc(ctx->serhnd);
}

/* Receive a command.  Returns -1 on csum error, 0 otherwise. */
/* Does not acknowledge. */
static int 
attempt_receive_packet(struct gdb_context *ctx)
{
    u8 csum;
    u8 received_csum;
    u8 ch;

    /* Skip over everything up to the first '$' */
    while ( (ch = gdb_io_read(ctx)) != '$' )
        continue;

    csum = 0;
    for ( ctx->in_bytes = 0;
          ctx->in_bytes < sizeof(ctx->in_buf);
          ctx->in_bytes++ )
    {
        ch = gdb_io_read(ctx);
        if ( ch == '#' )
            break;
        ctx->in_buf[ctx->in_bytes] = ch;
        csum += ch;
    }

    if ( ctx->in_bytes == sizeof(ctx->in_buf) )
    {
        dbg_printk("WARNING: GDB sent a stupidly big packet.\n");
        return -1;
    }

    ctx->in_buf[ctx->in_bytes] = '\0';
    received_csum = char2hex(gdb_io_read(ctx)) * 16 +
        char2hex(gdb_io_read(ctx));

    return (received_csum == csum) ? 0 : -1;
}

/* Receive a command, discarding up to ten packets with csum
 * errors.  Acknowledges all received packets. */
static int 
receive_command(struct gdb_context *ctx)
{
    int r, count = 0;

    count = 0;
    do {
        r = attempt_receive_packet(ctx);
        gdb_io_write_char((r < 0) ? '-' : '+', ctx);
        count++;
    } while ( (r < 0) && (count < GDB_RETRY_MAX) );

    return r;
}

/* routines to send reply packets */

static void 
gdb_start_packet(struct gdb_context *ctx)
{
    ctx->out_buf[0] = '$';
    ctx->out_offset = 1;
    ctx->out_csum = 0;
}

static void 
gdb_write_to_packet_char(u8 data, struct gdb_context *ctx)
{
    ctx->out_csum += data;
    ctx->out_buf[ctx->out_offset] = data;
    ctx->out_offset++;
}

void 
gdb_write_to_packet(const char *buf, int count, struct gdb_context *ctx)
{
    int x;
    for ( x = 0; x < count; x++ )
        gdb_write_to_packet_char(buf[x], ctx);
}

void 
gdb_write_to_packet_str(const char *buf, struct gdb_context *ctx)
{
    gdb_write_to_packet(buf, strlen(buf), ctx);
}

void
gdb_write_to_packet_hex(unsigned long x, int int_size, struct gdb_context *ctx)
{
    char buf[sizeof(unsigned long) * 2 + 1];
    int i = sizeof(unsigned long) * 2;
    int width = int_size * 2;

    buf[sizeof(unsigned long) * 2] = 0;

    switch ( int_size )
    {
    case sizeof(u8):
    case sizeof(u16):
    case sizeof(u32):
    case sizeof(u64):
        break;
    default:
        dbg_printk("WARNING: %s x: 0x%lx int_size: %d\n",
                   __func__, x, int_size);
        break;
    }

    do {
        buf[--i] = hex2char(x & 15);
        x >>= 4;
    } while ( x );

    while ( (i + width) > (sizeof(unsigned long) * 2) )
        buf[--i] = '0';

    gdb_write_to_packet(&buf[i], width, ctx);
}

static int
gdb_check_ack(struct gdb_context *ctx)
{
    u8 c = gdb_io_read(ctx);

    switch ( c )
    {
    case '+':
        return 1;
    case '-':
        return 0;
    default:
        printk("Bad ack: %c\n", c);
        return 0;
    }
}

/* Return 0 if the reply was successfully received, !0 otherwise. */
void 
gdb_send_packet(struct gdb_context *ctx)
{
    char buf[3];
    int count;

    sprintf(buf, "%.02x\n", ctx->out_csum);

    gdb_write_to_packet_char('#', ctx);
    gdb_write_to_packet(buf, 2, ctx);

    count = 0;
    do {
        gdb_io_write(ctx->out_buf, ctx->out_offset, ctx);
    } while ( !gdb_check_ack(ctx) && (count++ < GDB_RETRY_MAX) );

    if ( count == GDB_RETRY_MAX )
        dbg_printk("WARNING: %s reached max retry %d\n",
                   __func__, GDB_RETRY_MAX);
}

void 
gdb_send_reply(const char *buf, struct gdb_context *ctx)
{
    gdb_start_packet(ctx);
    gdb_write_to_packet_str(buf, ctx);
    gdb_send_packet(ctx);
}

/* arch neutral command handlers */

static void 
gdb_cmd_signum(struct gdb_context *ctx)
{
    gdb_write_to_packet_char('S', ctx);
    gdb_write_to_packet_hex(ctx->signum, sizeof(ctx->signum), ctx);
    gdb_send_packet(ctx);
}

static void 
gdb_cmd_read_mem(unsigned long addr, unsigned long length,
                 struct gdb_context *ctx)
{
    int x, r;
    unsigned char val;

    dbg_printk("Memory read starting at %lx, length %lx.\n", addr,
               length);

    for ( x = 0; x < length; x++ )
    {
        r = gdb_arch_copy_from_user(&val, (void *)(addr + x), 1);
        if ( r != 0 )
        {
            dbg_printk("Error reading from %lx.\n", addr + x);
            break;
        }
        gdb_write_to_packet_hex(val, sizeof(val), ctx);
    }

    if ( x == 0 )
        gdb_write_to_packet_str("E05", ctx);

    dbg_printk("Read done.\n");

    gdb_send_packet(ctx);
}

static void 
gdb_cmd_write_mem(unsigned long addr, unsigned long length,
                  const char *buf, struct gdb_context *ctx)
{
    int x, r;
    unsigned char val;

    dbg_printk("Memory write starting at %lx, length %lx.\n", addr, length);

    for ( x = 0; x < length; x++, addr++, buf += 2 )
    {
        val = str2ulong(buf, sizeof(val));
        r = gdb_arch_copy_to_user((void*)addr, (void*)&val, 1);
        if ( r != 0 )
        {
            dbg_printk("Error writing to %lx.\n", addr);
            break;
        }
    }

    if (x == length)
        gdb_write_to_packet_str("OK", ctx);
    else
        gdb_write_to_packet_str("E11", ctx);

    dbg_printk("Write done.\n");

    gdb_send_packet(ctx);
}

/* command dispatcher */
static int 
process_command(struct cpu_user_regs *regs, struct gdb_context *ctx)
{
    char *ptr;
    unsigned long addr, length;
    int resume = 0;

    /* XXX check ctx->in_bytes >= 2 or similar. */

    gdb_start_packet(ctx);
    switch ( ctx->in_buf[0] )
    {
    case '?':    /* query signal number */
        gdb_cmd_signum(ctx);
        break;
    case 'H':    /* thread operations */
        gdb_send_reply("OK", ctx);
        break;
    case 'g': /* Read registers */
        gdb_arch_read_reg_array(regs, ctx);
        break;
    case 'G': /* Write registers */
        gdb_arch_write_reg_array(regs, ctx->in_buf + 1, ctx);
        break;
    case 'm': /* Read memory */
        addr = simple_strtoul(ctx->in_buf + 1, &ptr, 16);
        if ( (ptr == (ctx->in_buf + 1)) || (ptr[0] != ',') )
        {
            gdb_send_reply("E03", ctx);
            return 0;
        }
        length = simple_strtoul(ptr + 1, &ptr, 16);
        if ( ptr[0] != 0 )
        {
            gdb_send_reply("E04", ctx);
            return 0;
        }
        gdb_cmd_read_mem(addr, length, ctx);
        break;
    case 'M': /* Write memory */
        addr = simple_strtoul(ctx->in_buf + 1, &ptr, 16);
        if ( (ptr == (ctx->in_buf + 1)) || (ptr[0] != ',') )
        {
            gdb_send_reply("E03", ctx);
            return 0;
        }
        length = simple_strtoul(ptr + 1, &ptr, 16);
        if ( ptr[0] != ':')
        {
            gdb_send_reply("E04", ctx);
            return 0;
        }
        gdb_cmd_write_mem(addr, length, ptr + 1, ctx);
        break;
    case 'p': /* read register */
        addr = simple_strtoul(ctx->in_buf + 1, &ptr, 16);
        if ( ptr == (ctx->in_buf + 1) )
        {
            gdb_send_reply("E03", ctx);
            return 0;
        }
        if ( ptr[0] != 0 )
        {
            gdb_send_reply("E04", ctx);
            return 0;
        }
        gdb_arch_read_reg(addr, regs, ctx);
        break;
    case 'D':
        ctx->currently_attached = 0;
        gdb_send_reply("OK", ctx);
        /* fall through */
    case 'k':
        ctx->connected = 0;
        /* fall through */
    case 's': /* Single step */
    case 'c': /* Resume at current address */
    {
        unsigned long addr = ~((unsigned long)0);
        unsigned long type = GDB_CONTINUE;
        if ( ctx->in_buf[0] == 's' )
            type = GDB_STEP;
        if ( ((ctx->in_buf[0] == 's') || (ctx->in_buf[0] == 'c')) &&
             ctx->in_buf[1] )
            addr = str2ulong(&ctx->in_buf[1], sizeof(unsigned long));
        if ( ctx->in_buf[0] != 'D' )
            ctx->currently_attached = 1;
        resume = 1;
        gdb_arch_resume(regs, addr, type, ctx);
        break;
    }

    default:
        gdb_send_reply("", ctx);
        break;
    }
    return resume;
}

static struct gdb_context
__gdb_ctx = {
    .serhnd             = -1,
    .currently_attached = 0,
    .running            = ATOMIC_INIT(1),
    .connected          = 0,
    .signum             = 1,
    .in_bytes           = 0,
    .out_offset         = 0,
    .out_csum           = 0,
};
static struct gdb_context *gdb_ctx = &__gdb_ctx;

/* trap handler: main entry point */
int 
__trap_to_gdb(struct cpu_user_regs *regs, unsigned long cookie)
{
    int resume = 0;
    int r;
    unsigned long flags;

    if ( gdb_ctx->serhnd < 0 )
    {
        dbg_printk("Debugger not ready yet.\n");
        return 0;
    }

    /* We rely on our caller to ensure we're only on one processor
     * at a time... We should probably panic here, but given that
     * we're a debugger we should probably be a little tolerant of
     * things going wrong. */
    /* We don't want to use a spin lock here, because we're doing
       two distinct things:

       1 -- we don't want to run on more than one processor at a time,
            and
       2 -- we want to do something sensible if we re-enter ourselves.

       Spin locks are good for 1, but useless for 2. */
    if ( !atomic_dec_and_test(&gdb_ctx->running) )
    {
        printk("WARNING WARNING WARNING: Avoiding recursive gdb.\n");
        atomic_inc(&gdb_ctx->running);
        return 0;
    }

    if ( !gdb_ctx->connected )
    {
        printk("GDB connection activated.\n");
        gdb_arch_print_state(regs);
        gdb_ctx->connected = 1;
    }

    smp_send_stop();

    /* Try to make things a little more stable by disabling
       interrupts while we're here. */
    local_irq_save(flags);

    watchdog_disable();
    console_start_sync();

    /* Shouldn't really do this, but otherwise we stop for no
       obvious reason, which is Bad */
    printk("Waiting for GDB to attach...\n");

    gdb_arch_enter(regs);
    gdb_ctx->signum = gdb_arch_signal_num(regs, cookie);
    /* If gdb is already attached, tell it we've stopped again. */
    if ( gdb_ctx->currently_attached )
    {
        gdb_start_packet(gdb_ctx);
        gdb_cmd_signum(gdb_ctx);
    }

    while ( resume == 0 )
    {
        r = receive_command(gdb_ctx);
        if ( r < 0 )
        {
            dbg_printk("GDB disappeared, trying to resume Xen...\n");
            resume = 1;
        }
        else
        {
            resume = process_command(regs, gdb_ctx);
        }
    }

    gdb_arch_exit(regs);
    console_end_sync();
    watchdog_enable();
    atomic_inc(&gdb_ctx->running);

    local_irq_restore(flags);

    return 0;
}

void
initialise_gdb(void)
{
    gdb_ctx->serhnd = serial_parse_handle(opt_gdb);
    if ( gdb_ctx->serhnd != -1 )
        printk("GDB stub initialised.\n");
    serial_start_sync(gdb_ctx->serhnd);
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 */
