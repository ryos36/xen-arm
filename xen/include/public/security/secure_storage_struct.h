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

#ifndef _SECURE_STORAGE_STRUCT_H_
#define _SECURE_STORAGE_STRUCT_H_

#define __SSS_VERSION__ 08102310 /* YYMMDDHH */

#ifdef __XEN__
#include <xen/types.h>
#else
#include <linux/types.h>
#endif


/* MBB = Master Boot Block; flash location for partition and kernel image information */

#define MAGIC_MBB 0x5A4D4242
#define MAGIC_SP1 0x5A535031
#define MAGIC_SP2 0x5A325350
#define MAGIC_SP3 0x5A533350
#define MAGIC_OSIP 0x4F534950

#define MAX_MTD_PARTITION 10
#define MAX_IMAGE_STRUCT_SIZE 10

/* in image type enumeration value 
 * binary 32bits: MSB --> aaaa bbbb cccc cccc dddd eeee eeee eeee <-- LSB
 *
 * in the upper 16 bits
 1) MSB 4 bits (aaaa): reserved (must be 0)
 2) MSB 4 bits (bbbb) in remaining 12 bits: SP1, SP2, SP3
 * 0x01: SP1
 * 0x02: SP2
 * 0x03: SP3
 * 0x04: OS image partition
 * 0x05 ~ 0x0f: not defined
 * 
 3) Remaining 8 bits (cccc cccc): categories
 * 0000 0001: images with signatures
 * 0000 0010: signatures
 * 0000 0100: policies
 * 0000 1000: etc.
 * 0001 0000: not defined
 * 0010 0000: not defined
 * 0100 0000: not defined
 * 1000 0000: not defined
*/

#define VERIFY_RESERVED(x) ((x) & 0xF0000000)
#define VERIFY_SP1(x)  (!VERIFY_RESERVED(x) && (((x)>>24) == 1))
#define VERIFY_SP2(x)  (!VERIFY_RESERVED(x) && (((x)>>24) == 2))
#define VERIFY_SP3(x)  (!VERIFY_RESERVED(x) && (((x)>>24) == 3))
#define VERIFY_OSIP(x) (!VERIFY_RESERVED(x) && (((x)>>24) == 4))

#define VERIFY_IMAGES(x)      (!VERIFY_RESERVED(x) && ((x) & 0x00010000))
#define VERIFY_SIGNATURES(x)  (!VERIFY_RESERVED(x) && ((x) & 0x00020000))
#define VERIFY_POLICIES(x)    (!VERIFY_RESERVED(x) && ((x) & 0x00040000))

/* 
 * in the lower 16bits
 4) upper 4bits (dddd) in lower 16bits of image or signature categories
 * 0000: reserved
 * 0001: certm or XEN_ARM, ...
 * 0010: kernel images
 * 0100: not defined
 * 1000: not defined
 */
/* 
 5) in lower 12bits (eeee eeee eeee), 
 * image value and corresponding signature(hash) value must be the same 
 */
#define VERIFY_KERNEL_AND_SIG(x) (!VERIFY_RESERVED(x) && ((x) & 0x00002000))

#define PARTITION_INFO(x) (((x) >> 24) & 0x0F)

typedef enum {
  CERTM_IMG=0x01011000, XEN_ARM_IMG=0X01011001,
  DRIVER_DOM_IMG=0x04012000, SECURE_DOM_IMG, 
  NORMAL_DOM1_IMG, NORMAL_DOM2_IMG, 
  NORMAL_DOM3_IMG, NORMAL_DOM4_IMG, RESERVE_FOR_IMG_POS,
  CERTM_SIGNED_HASH=0x01021000, XEN_ARM_SIGNED_HASH=0x01021001,
  DRIVER_DOM_SIGNED_HASH=0x01022000, SECURE_DOM_SIGNED_HASH, 
  NORMAL_DOM1_SIGNED_HASH, NORMAL_DOM2_SIGNED_HASH,
  NORMAL_DOM3_SIGNED_HASH, NORMAL_DOM4_SIGNED_HASH,
  XEN_AC_POLICY_1=0x02040000,
  XEN_AC_POLICY_2,
  XEN_AC_POLICY_3, XEN_AC_POLICY_4,
  XEN_AC_POLICY_5, XEN_AC_POLICY_6,
  XEN_AC_POLICY_7, XEN_AC_POLICY_8,
  DRIVER_DOM_AC_POLICY,
  SECURE_DOM_KEY=0x03000000, 
  IMAGE_END=0xFFFFFFFF
} image_type_t;

typedef enum {
  PART_MBB, PART_SP1=1, PART_SP2=2, PART_SP3=3, PART_OS_IMAGE=4, 
  PART_DRV_RFS, PART_DOM0_RFS, PART_DOM1_RFS, PART_DOM2_RFS,
  TRANSFER_MASTER_KEY, PART_SUB_VMM_IMAGE,
  PART_END
} partition_type_t;

typedef struct {
  image_type_t type;
  u_int32_t size;
  union {
    char *ptr;
    u_int32_t pos;
  } u;
} default_struct_t;

typedef struct {
  u_int32_t magic;
  u_int32_t size;
} ssb_master_boot_block_header_t;

typedef struct {
  partition_type_t ptype;             /* identifier string */
  u_int32_t mtd_tb_num;		/* MTD table number <mtd_table[xx]> */
  u_int32_t part_offset;	/* partition offset in the master MTD space */
  u_int32_t data_offset;	/* image offset in the partition */
  u_int32_t size;		/* partition image size */
  u_int32_t max_size;          /* partition space size */
  u_int32_t mask_flags;	       /* master MTD flags to mask out for this partition */
} ssb_mtd_partition_t;

typedef struct {
  u_int32_t psize;
  ssb_mtd_partition_t mtd_part[MAX_MTD_PARTITION];
} ssb_master_boot_block_t;

typedef struct {
  u_int32_t images_size;
  default_struct_t image[MAX_IMAGE_STRUCT_SIZE];
} ssb_image_container_t;

typedef struct {
  partition_type_t type;
  u_int32_t size;
  char *ptr;
} transfer_struct_t;

typedef struct {
  u_int32_t images_size;
  transfer_struct_t image[MAX_IMAGE_STRUCT_SIZE];
} ssb_transfer_container_t;


typedef enum {CRYPTO_HASH=1, CRYPTO_ASYM, CRYPTO_SYM} crypto_kind_of_t;
typedef enum {CRYPTO_ASYM_RSA_PUBLIC=1, CRYPTO_ASYM_RSA_PRIVATE} crypto_asym_t;

typedef struct {
  crypto_kind_of_t kind_of;
  union {
    u_int32_t hash;
    u_int32_t sym;
    crypto_asym_t asym;
  } u;
} crypto_type_t;

typedef struct {
  crypto_type_t type;
  u_int8_t *key;
  u_int16_t key_size;
  u_int8_t *init_vec;
  u_int16_t init_vec_size;
} sym_key_t;


#endif /* _SECURE_STORAGE_STRUCT_H_ */
