/*
 * (C) Copyright 2007
 * Samsung Software Labs., sd.mo@samsung.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SRA_FUNC_H_
#define _SRA_FUNC_H_

#include <xen/types.h>
#include <public/security/secure_storage_struct.h>

#define MAX_ENC_DEC_LEN (0x8000)
#define MAX_PART_SIZE (128*1024)

#define SRA_DEBUG 1

#define SECURE_DOM_IMG_POS	(unsigned char*)0xC1C00000
#define NORMAL_DOM1_IMG_POS	(unsigned char*)0xC1F00000
#define NORMAL_DOM2_IMG_POS	(unsigned char*)0xC2200000
#define NORMAL_DOM3_IMG_POS	(unsigned char*)0xC2500000
#define NORMAL_DOM4_IMG_POS	(unsigned char*)0xC2800000


/**********************************************
Public interface
**********************************************/

/* secure storage intialization (symmetric key, Master Boot Block, Secure Partition loading) */
int sra_init(void);

/* make encrypted MBB, SP, or OS Image partition for flashing to flash memory */
int sra_make_enc_part(void *to_mem, u_int32_t to_size, void *from_bin, u_int32_t bin_size, partition_type_t ptype);

/* appending or changing image data */
int sra_set_image(default_struct_t *img, partition_type_t ptype);

/* read the img->type data */
default_struct_t *sra_get_image(partition_type_t ptype, image_type_t itype);

/* get MBB information pointer */
ssb_master_boot_block_t *sra_get_mbb(void);

/* write MBB to local DB */
int sra_set_mbb(ssb_master_boot_block_t *pmbb);

/* get SP information */
ssb_image_container_t *sra_get_sp(partition_type_t ptype);

/* write SP information to */
int sra_set_sp(ssb_image_container_t *sp, partition_type_t ptype);

/* crc32 function */
unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

/* get binary pointer for ptype partition */
transfer_struct_t sra_get_bin(partition_type_t ptype);



/**********************************************
Private interface
**********************************************/
void init_part_var(partition_type_t ptype);

#define ERROR_EXIT(_errno)                                          \
    do {                                                            \
        DPRINTK(3,"Failure in %s: domain %d, error %d, line %d\n", \
                __func__, current->domain->domain_id, (_errno), __LINE__); \
        rc = (_errno);                                              \
        goto out;                                                   \
    } while ( 0 )

/* find image structure */
transfer_struct_t *_find_transfer_image(ssb_transfer_container_t *tc, partition_type_t ttype);

#if 0
default_struct_t *_find_default_image(ssb_image_container_t *ic, image_type_t itype);
#endif

/* calculate partition size */
u_int32_t _get_part_size(ssb_image_container_t *ic);

/* memory free and/or allocation */
void *_page_alloc(void *ptr, u_int32_t bytes);

/* MBB & SP data unserialization */
#define COPY 1
#define NOCOPY 0
int _sra_unserialize(void *to_con, u_int32_t to_size, void *from_bin, u_int32_t from_size, 
		     partition_type_t ptype, int copy);

/* MBB & SP data serialization */
int _sra_serialize(void *to_bin, u_int32_t to_size, void *from_con, u_int32_t from_size, 
		   partition_type_t ptype, int copy);


/* load MBB, SP, or OS Image partition : input parition type */
int _load_part(ssb_transfer_container_t *tc, partition_type_t ptype);

/* get ssb_mtd_partition_t data for ptype */
int _get_mtd_partition(ssb_mtd_partition_t *psmp);

/* set ssb_mtd_partition_t data for ptype */
int _set_mtd_partition(ssb_mtd_partition_t *psmp);

#endif /* _SRA_FUNC_H_ */
