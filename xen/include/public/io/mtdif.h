#ifndef _MTDIF_H
#define _MTDIF_H

#include <linux/mtd/compatmac.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/ctype.h>

#define PARAM_SIZE			30

/* For check CRC */
/* #define VMTD_TRANSFER_WITH_CRC */

/* For debug */
/* #define VMTD_TRANSFER_DEBUG_IDENT */


/* MTD operations */
typedef enum {
    VMTDOP_READ         = ( 1L<<0 ),
    VMTDOP_WRITE        = ( 1L<<1 ),
    VMTDOP_ERASE        = ( 1L<<2 ),
    
    VMTDOP_WRITEV       = ( 1L<<3 ),
    VMTDOP_READV        = ( 1L<<4 ),
    VMTDOP_WRITEVECC    = ( 1L<<5 ),
    
    VMTDOP_BLOCK_ISBAD  = ( 1L<<6 ),
    VMTDOP_BLOCK_MARKBAD= ( 1L<<7 ),
    
    VMTDOP_READECC      = ( 1L<<8 ),
    VMTDOP_READOOB      = ( 1L<<9 ),
    VMTDOP_WRITEECC     = ( 1L<<10 ),
    VMTDOP_WRITEOOB     = ( 1L<<11 ),

    VMTDOP_SUSPEND      = ( 1L<<12 ),
    VMTDOP_RESUME       = ( 1L<<13 ),
    
    VMTDOP_SYNC         = ( 1L<<14 ),
    
    VMTDOP_INIT         = ( 1L<<15 ),
    VMTDOP_TEST         = ( 1L<<16 ),
    
    VMTDOP_FAILIO       = ( 1L<<17 )
} vmtd_op_t;

#define VMTDOP_VEC  ( VMTDOP_WRITEV | VMTDOP_READV | VMTDOP_WRITEVECC )
#define VMTD_IS_VECOP(op) ( (op) & VMTDOP_VEC )

typedef enum {
    VMTD_FLAG_OFF = 0,
    VMTD_FLAG_ON = 1
} vmtd_flag_t;


typedef struct {
    loff_t from;
    size_t len;
    size_t retlen;
    u_char *buf;
} vmtd_read_op_t;

typedef struct {
    loff_t to;
    size_t len;
    size_t retlen;
    const u_char *buf;
} vmtd_write_op_t;

typedef struct {
    struct erase_info *instr;
} vmtd_erase_op_t;

typedef struct {
    loff_t from;
    size_t len;
    u_char *buf;
    u_char *eccbuf;
    struct nand_oobinfo *oobsel;
    size_t retlen;
} vmtd_readecc_op_t;

typedef struct {
    loff_t from;
    //size_t len;
    //u_char *buf;
    //size_t retlen;
#if 1	// by yjhyun 080404
    struct mtd_oob_ops *ops;
#endif
} vmtd_readoob_op_t;

typedef struct {
    loff_t to;
    size_t len;
    size_t retlen;
    const u_char *buf;
    u_char *eccbuf; 
    struct nand_oobinfo *oobsel;
} vmtd_writeecc_op_t;

typedef struct {
    loff_t to;
    //size_t len;
    //size_t retlen; 
    //const u_char *buf;
#if 1	// by yjhyun 080404
    struct mtd_oob_ops *ops;
#endif
} vmtd_writeoob_op_t;

typedef struct {
    /* const struct kvec *vecs; */
    struct kvec *vecs;
    unsigned long count;
    loff_t to;
    size_t retlen;
} vmtd_writev_op_t;

typedef struct {
    struct kvec *vecs;
    unsigned long count;
    loff_t from;
    size_t retlen;
} vmtd_readv_op_t;

typedef struct {
    loff_t ofs;
} vmtd_block_isbad_op_t;

typedef struct {
    loff_t ofs;
} vmtd_block_markbad_op_t;

typedef struct {
/*    const struct kvec *vecs; */
    struct kvec *vecs;
    unsigned long count;
    loff_t to;
    size_t retlen;
    u_char *eccbuf;
    struct nand_oobinfo *oobsel;
} vmtd_writevecc_op_t;

typedef struct {
    loff_t iov_base_ofs;
    size_t iov_len;
} vmtd_vecs_t;

typedef struct {
    u_int32_t type_op;

    vmtd_read_op_t  read;
    vmtd_write_op_t write;
    vmtd_erase_op_t erase;
    
    vmtd_writev_op_t writev;
    vmtd_readv_op_t  readv;
    vmtd_writevecc_op_t writevecc;
    
    vmtd_block_isbad_op_t   block_isbad;
    vmtd_block_markbad_op_t block_markbad;
    
    vmtd_readecc_op_t  readecc;
    vmtd_readoob_op_t  readoob;
    vmtd_writeecc_op_t writeecc;
    vmtd_writeoob_op_t writeoob;
    
    struct mtd_info *mtd;
} vmtd_transfer_t;


/* Magic words */
#define VMTD_IDENT_BE	0x11223344
#define VMTD_IDENT_FE	0x55667788


/*
 * Struct for transfer data between backend and frontend
 */

typedef struct {
    u_int32_t ident;            /* Magic number */
    u_int32_t type_op;          /* operation */
    u_int32_t crc;              /* crc */

    loff_t buff_offset;         /* data buffer offset to read/write op */
    loff_t eccbuf_offset;       /* oob selection structure */

    struct nand_oobinfo oobsel; /* filesystem supplied oob data buffer */
    struct erase_info erase_instr; /* erase instruction */
#if 1	// by yjhyun 080404
    struct mtd_oob_ops oobops;
#endif		

    size_t len;                 /* number of bytes to read/write */
    size_t eccbuf_length;
    u_int32_t oobsel_flag;

    loff_t from;                /* offset to read from */
    loff_t to;                  /* offset to write to */
    loff_t ofs;                 /* offset relative to mtd start */

    unsigned long count_vecs;   /* number of vectors */
    loff_t structs_kvec_offs;

    size_t retlen;               /* pointer to variable to store the number of written bytes */

    int retval;

    struct mtd_info mtd;       /* for init */
    u_int32_t state;
} vmtd_transfer_data_t;



/*
 * Default: 16 pages for shared memory between domains
 */

#define DEFAULT_ORDER           6
#define DEFAULT_PAGE_NUM        (1<<DEFAULT_ORDER)
#define VMTD_SHARED_SIZE        (DEFAULT_PAGE_NUM * PAGE_SIZE)

#define DEFAULT_NUM_ALLOC_KVEC  50


#define VMTD_FE_IO_LOCK(__info)     \
({                                  \
	  down(&vmtd_fe_mutex);	\
})

#define VMTD_FE_IO_UNLOCK(__info)   \
({                                  \
  	up(&vmtd_fe_mutex);	\
})


/* For check CRC */
#ifdef VMTD_TRANSFER_WITH_CRC

/* Put CRC info in struct */
#define VMTD_PUT_CRC(p)         \
({                              \
    u_int32_t local_crc;        \
                                \
    local_crc = vmtd_get_crc((void *)(p), VMTD_SHARED_SIZE); \
    (p)->crc = local_crc;       \
})

/* BUG on CRC error */
#define VMTD_BUG_CRC_ERROR(p)           \
({                                      \
    u_int32_t local_crc, otherend_crc;  \
                                        \
    otherend_crc = (p)->crc;            \
    local_crc = vmtd_get_crc((void *)(p), VMTD_SHARED_SIZE); \
    BUG_ON(otherend_crc != local_crc);  \
})

#else /* VMTD_TRANSFER_WITH_CRC */

#define VMTD_PUT_CRC(p)
#define VMTD_BUG_CRC_ERROR(p)

#endif /* VMTD_TRANSFER_WITH_CRC */


#ifdef VMTD_TRANSFER_DEBUG_IDENT
#define VMTD_ACT_ONIDERROR() ({ BUG(); })
#else  /* VMTD_TRANSFER_DEBUG_IDENT */
#define VMTD_ACT_ONIDERROR() ({ return -EIO; })
#endif  /* VMTD_TRANSFER_DEBUG_IDENT */


/*
 * Send data to backend and wait response
 */

#define	VMTD_DELAY_TIMEOUT	(200* HZ)
#define VMTD_FE_FLUSH_AND_WAIT(__info, __op)                        \
({                                                                  \
	int ret = 0 ;			\
    flush_requests(__info);                                         \
    while(1)                                                        \
    {                                                               \
    	mb();	\
        ret = wait_event_interruptible_timeout( __info->wq, atomic_read(&__info->io_transfer),VMTD_DELAY_TIMEOUT ); \
        atomic_xchg(&__info->io_transfer, 0);                        \
        mb();							\
        if(!ret) {				\
			printk("VMTD_FE_FLUSH_AND_WAIT : timeout\n"); \
			return -ETIMEDOUT;	\
        }	\
        if( (transfer_data_s->ident == VMTD_IDENT_BE) &&            \
            (transfer_data_s->type_op == __op) )                    \
            break;                                                  \
        SDPRINTK("Error response!!!");                              \
        VMTD_ACT_ONIDERROR();                                       \
    }                                                               \
    SDPRINTK("FE: get valid response from BE");                     \
})

/*
 * Check CRC( working if enable VMTD_TRANSFER_WITH_CRC )
 */

static inline u_int32_t
vmtd_get_crc(void *buff, int length)
{
    char *p = (char *)buff;
    u_int32_t crc_summ = 0;

    vmtd_transfer_data_t *transfer_data_s;
    
    transfer_data_s = (vmtd_transfer_data_t *)buff;
    transfer_data_s->crc = 0;

    do
    {
        crc_summ += *(u_int32_t *)p;
        p += sizeof(u_int32_t);
    }while( (p - (char *)buff) <= (length - sizeof(u_int32_t)) );

    return crc_summ;
}

static inline int small_atoi(char *p)
{
    int val = 0;
	char *name = p;

    for (;; name++) {
		switch (*name) {
			case '0'...'9':
				val = 10*val+(*name-'0');
				break;
			default:
				return val;
		}
    }
}

static inline int name_to_num(char *name)
{
char *t = name;

while (*++t != '\0') ;
t--;

while (isdigit(*--t) && t!=name) ;
t++;

return small_atoi(t);
}

#endif  /* _MTDIF_H */













