#ifndef __XEN_PUBLIC_SRA_OPS_H__
#define __XEN_PUBLIC_SRA_OPS_H__

#include "secure_storage_struct.h"

#define SRA_INTERFACE_VERSION   0x00090000

#define SRA_OP_READ 1
typedef struct sra_op_read {
  /* IN/OUT parameters */
  default_struct_t image;
  /* OUT parameters */
  long rc;
} sra_op_read_t;

#define SRA_OP_READ_ENC_PART 2
typedef struct sra_op_read_enc {
  /* IN parameter */
  u_int32_t size;
  /* IN/OUT parameter */
  transfer_struct_t part[MAX_MTD_PARTITION];
  /* OUT parameters. */
  ssb_master_boot_block_t part_pos;
  long rc;
} sra_op_read_enc_part_t;

#define SRA_OP_WRITE 3
typedef struct sra_op_write {
  /* IN parameters. */
  default_struct_t image;
  default_struct_t sig;
  /* OUT parameters. */
  long rc;
} sra_op_write_t;

#define SRA_OP_UPDATE 4
typedef struct sra_op_update {
  /* IN parameters. */
  default_struct_t image;
  default_struct_t sig;
  /* IN/OUT parameters. */
  default_struct_t vmm;
  default_struct_t access_code;
  /* OUT parameters. */
  long rc;
} sra_op_update_t;

#define SRA_OP_VERIFY 5
typedef struct sra_op_verify {
  /* IN parameters. */
  default_struct_t image;
  default_struct_t access_code;
  /* OUT parameters. */
  ssb_mtd_partition_t pos;
  long rc;
} sra_op_verify_t;

#define SRA_OP_READ_PART_INFO 6
typedef struct sra_op_read_part_info {
  /* IN/OUT parameters */
  ssb_mtd_partition_t part;
  /* OUT parameters */
  long rc;
} sra_op_read_part_info_t;

#define SRA_OP_COMPARE 7
/* sub commands */
#define SRA_OP_COMPARE_IMAGE 1
#define SRA_OP_COMPARE_POS 2
#define SRA_OP_COMPARE_PART_INFO 3
#define SRA_OP_COMPARE_PARTS 4
typedef struct sra_op_compare {
  /* IN parameters */
  int cmd;
  partition_type_t ptype;
  image_type_t itype;
  default_struct_t image;
  ssb_mtd_partition_t pos;
  ssb_transfer_container_t parts;
  ssb_master_boot_block_t parts_pos;
  /* OUT parameters */
  long rc;
} sra_op_compare_t;

typedef struct sra_op {
    uint32_t cmd; /* SRA_OP_* */
    uint32_t interface_version; /* SRA_INTERFACE_VERSION */
    union {
      sra_op_read_t read_image;
      sra_op_read_part_info_t read_part;
      sra_op_read_enc_part_t read_enc_image;
      sra_op_write_t write_image;
      sra_op_update_t update_image;
      sra_op_verify_t verify_image;
      sra_op_compare_t comp_image;
    } u;
} sra_op_t;
DEFINE_XEN_GUEST_HANDLE(sra_op_t);

#ifdef __XEN__
int sra_op_init(struct domain *d);
#endif

long do_sra_ops(struct sra_op *uop);
long sra_func_read(sra_op_read_t *op_read);
long sra_func_read_part_info(sra_op_read_part_info_t *op_read_part);
long sra_func_read_enc_part(sra_op_read_enc_part_t *op_read_enc);
long sra_func_write(sra_op_write_t *op_write); /* for SP3 */
long sra_func_update(sra_op_update_t *op_update); /* for SP1 */
long sra_func_verify(sra_op_verify_t *op_verify);
long sra_func_compare(sra_op_compare_t *op_compare);
/* about SP2, depends on policy conductor */

#endif /* __XEN_PUBLIC_SRA_OPS_H__ */
