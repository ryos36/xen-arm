/*
 * symbols.c: in-kernel printing of symbolic oopses and stack traces.
 *
 * Copyright 2002 Rusty Russell <rusty@rustcorp.com.au> IBM Corporation
 *
 * ChangeLog:
 *
 * (25/Aug/2004) Paulo Marques <pmarques@grupopie.com>
 *      Changed the compression method from stem compression to "table lookup"
 *      compression (see tools/symbols.c for a more complete description)
 */

#include <xen/config.h>
#include <xen/symbols.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/string.h>

/* These will be re-linked against their real values during the second link stage */
extern unsigned long symbols_addresses[] __attribute__((weak));
extern unsigned long symbols_num_syms __attribute__((weak,section("data")));
extern u8 symbols_names[] __attribute__((weak));

extern u8 symbols_token_table[] __attribute__((weak));
extern u16 symbols_token_index[] __attribute__((weak));

extern unsigned long symbols_markers[] __attribute__((weak));

/* expand a compressed symbol data into the resulting uncompressed string,
   given the offset to where the symbol is in the compressed stream */
static unsigned int symbols_expand_symbol(unsigned int off, char *result)
{
    int len, skipped_first = 0;
    u8 *tptr, *data;

    /* get the compressed symbol length from the first symbol byte */
    data = &symbols_names[off];
    len = *data;
    data++;

    /* update the offset to return the offset for the next symbol on
     * the compressed stream */
    off += len + 1;

    /* for every byte on the compressed symbol data, copy the table
       entry for that byte */
    while(len) {
        tptr = &symbols_token_table[ symbols_token_index[*data] ];
        data++;
        len--;

        while (*tptr) {
            if(skipped_first) {
                *result = *tptr;
                result++;
            } else
                skipped_first = 1;
            tptr++;
        }
    }

    *result = '\0';

    /* return to offset to the next symbol */
    return off;
}

/* find the offset on the compressed stream given and index in the
 * symbols array */
static unsigned int get_symbol_offset(unsigned long pos)
{
    u8 *name;
    int i;

    /* use the closest marker we have. We have markers every 256 positions,
     * so that should be close enough */
    name = &symbols_names[ symbols_markers[pos>>8] ];

    /* sequentially scan all the symbols up to the point we're searching for.
     * Every symbol is stored in a [<len>][<len> bytes of data] format, so we
     * just need to add the len to the current pointer for every symbol we
     * wish to skip */
    for(i = 0; i < (pos&0xFF); i++)
        name = name + (*name) + 1;

    return name - symbols_names;
}

const char *symbols_lookup(unsigned long addr,
                           unsigned long *symbolsize,
                           unsigned long *offset,
                           char *namebuf)
{
    unsigned long i, low, high, mid;
    unsigned long symbol_end = 0;

    /* This kernel should never had been booted. */
    BUG_ON(!symbols_addresses);

    namebuf[KSYM_NAME_LEN] = 0;
    namebuf[0] = 0;

    if (!is_kernel_text(addr))
        return NULL;

        /* do a binary search on the sorted symbols_addresses array */
    low = 0;
    high = symbols_num_syms;

    while (high-low > 1) {
        mid = (low + high) / 2;
        if (symbols_addresses[mid] <= addr) low = mid;
        else high = mid;
    }

    /* search for the first aliased symbol. Aliased symbols are
           symbols with the same address */
    while (low && symbols_addresses[low - 1] == symbols_addresses[low])
        --low;

        /* Grab name */
    symbols_expand_symbol(get_symbol_offset(low), namebuf);

    /* Search for next non-aliased symbol */
    for (i = low + 1; i < symbols_num_syms; i++) {
        if (symbols_addresses[i] > symbols_addresses[low]) {
            symbol_end = symbols_addresses[i];
            break;
        }
    }

    /* if we found no next symbol, we use the end of the section */
    if (!symbol_end)
        symbol_end = kernel_text_end();

    *symbolsize = symbol_end - symbols_addresses[low];
    *offset = addr - symbols_addresses[low];
    return namebuf;
}

/* Replace "%s" in format with address, or returns -errno. */
void __print_symbol(const char *fmt, unsigned long address)
{
    const char *name;
    unsigned long offset, size;
    char namebuf[KSYM_NAME_LEN+1];
    char buffer[sizeof("%s+%#lx/%#lx [%s]") + KSYM_NAME_LEN +
               2*(BITS_PER_LONG*3/10) + 1];

    name = symbols_lookup(address, &size, &offset, namebuf);

    if (!name)
        sprintf(buffer, "???");
    else
        sprintf(buffer, "%s+%#lx/%#lx", name, offset, size);

    printk(fmt, buffer);
}
