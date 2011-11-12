/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#define MBI_MEMLIMITS  (1<<0)
#define MBI_DRIVES     (1<<1)
#define MBI_CMDLINE    (1<<2)
#define MBI_MODULES    (1<<3)
#define MBI_MEMMAP     (1<<6)
#define MBI_LOADERNAME (1<<9)

/* The symbol table for a.out.  */
typedef struct {
    u32 tabsize;
    u32 strsize;
    u32 addr;
    u32 reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct {
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    union {
        aout_symbol_table_t aout_sym;
        elf_section_header_table_t elf_sec;
    } u;
    u32 mmap_length;
    u32 mmap_addr;
} multiboot_info_t;

/* The module structure.  */
typedef struct {
    u32 mod_start;
    u32 mod_end;
    u32 string;
    u32 reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct {
    u32 size;
    u32 base_addr_low;
    u32 base_addr_high;
    u32 length_low;
    u32 length_high;
    u32 type;
} memory_map_t;

#endif /* __MULTIBOOT_H__ */
