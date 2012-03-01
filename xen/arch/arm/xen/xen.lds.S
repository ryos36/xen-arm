/*
 * xen.lds.S
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          ChanJu Park  <bestworld@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <xen/config.h>

OUTPUT_ARCH(arm)
ENTRY(start)
ENTRY(text)

jiffies = jiffies64;

PHDRS
{
	text PT_LOAD ;
}

SECTIONS
{
	/* . = 0xFF008000; */
	. = XEN_START_ADDR; 

	_start = .;
	_stext = .;			/* Text and read-only data */
	.text : {
		*(.text)
		*(.fixup)
		*(.gnu.warning)
	} :text =0x9090

	.rodata : {
		*(.rodata)
		*(.rodata.*) 
	} :text
	_etext = .;			/* End of text section */

	. = ALIGN(32);
	_sextable = .;
	.extable : { 
		*(.extable) 
	} : text
	_eextable = .;

	. = ALIGN(32);
	_spreextable = .;
	.preextable : {
		*(.preextable)
	} : text
	_epreextable = .;

	_sdata = .;
	.data : {			/* Data */
		*(.data)
	} :text
	_edata = .;

	__init_begin = .;
	.text.init : {
		*(.text.init) 
	} :text

	.data.init : { 
		*(.data.init)
	} :text

	__setup_start = .;
	.setup.init : { 
		*(.setup.init) 
	} :text
	__setup_end = .;

	. = ALIGN(4);

	__initcall_start = .;
	.initcall.init : {
		*(.initcall.init) 
	} :text
	__initcall_end = .;
	__init_end = .;
	. = ALIGN(4096);
	_sbss = .;		/* BSS */
	.bss : {
		*(.bss.stack_aligned)
		*(.bss.page_aligned);
		*(.bss)
	} :text
	_ebss = .;
	_end = . ;
	/* Sections to be discarded */

  	/DISCARD/ : {
  		*(.text.exit)
		*(.data.exit)
		*(.exitcall.exit)
	}
	/* Stabs debugging sections.  */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	
}

