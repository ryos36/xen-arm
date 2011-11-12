/******************************************************************************
 * xg_private.c
 * 
 * Helper functions for the rest of the library.
 */

#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

#include "xg_private.h"

char *xc_read_image(const char *filename, unsigned long *size)
{
    int kernel_fd = -1;
    gzFile kernel_gfd = NULL;
    char *image = NULL;
    unsigned int bytes;

    if ( (filename == NULL) || (size == NULL) )
        return NULL;

    if ( (kernel_fd = open(filename, O_RDONLY)) < 0 )
    {
        PERROR("Could not open kernel image");
        goto out;
    }

    if ( (*size = xc_get_filesz(kernel_fd)) == 0 )
    {
        PERROR("Could not read kernel image");
        goto out;
    }

    if ( (kernel_gfd = gzdopen(kernel_fd, "rb")) == NULL )
    {
        PERROR("Could not allocate decompression state for state file");
        goto out;
    }

    if ( (image = malloc(*size)) == NULL )
    {
        PERROR("Could not allocate memory for kernel image");
        goto out;
    }

    if ( (bytes = gzread(kernel_gfd, image, *size)) != *size )
    {
        PERROR("Error reading kernel image, could not"
               " read the whole image (%d != %ld).", bytes, *size);
        free(image);
        image = NULL;
    }

 out:
    if ( kernel_gfd != NULL )
        gzclose(kernel_gfd);
    else if ( kernel_fd >= 0 )
        close(kernel_fd);
    return image;
}

char *xc_inflate_buffer(const char *in_buf, unsigned long in_size,
                        unsigned long *out_size)
{
    int           sts;
    z_stream      zStream;
    unsigned long out_len;
    char         *out_buf;

    /* Not compressed? Then return the original buffer. */
    if ( ((unsigned char)in_buf[0] != 0x1F) ||
         ((unsigned char)in_buf[1] != 0x8B) )
    {
        if ( out_size != NULL )
            *out_size = in_size;
        return (char *)in_buf;
    }

    out_len = (unsigned char)in_buf[in_size-4] +
        (256 * ((unsigned char)in_buf[in_size-3] +
                (256 * ((unsigned char)in_buf[in_size-2] +
                        (256 * (unsigned char)in_buf[in_size-1])))));

    bzero(&zStream, sizeof(zStream));
    out_buf = malloc(out_len + 16);        /* Leave a little extra space */
    if ( out_buf == NULL )
    {
        ERROR("Error mallocing buffer\n");
        return NULL;
    }

    zStream.next_in = (unsigned char *)in_buf;
    zStream.avail_in = in_size;
    zStream.next_out = (unsigned char *)out_buf;
    zStream.avail_out = out_len+16;
    sts = inflateInit2(&zStream, (MAX_WBITS+32)); /* +32 means "handle gzip" */
    if ( sts != Z_OK )
    {
        ERROR("inflateInit failed, sts %d\n", sts);
        free(out_buf);
        return NULL;
    }

    /* Inflate in one pass/call */
    sts = inflate(&zStream, Z_FINISH);
    if ( sts != Z_STREAM_END )
    {
        ERROR("inflate failed, sts %d\n", sts);
        free(out_buf);
        return NULL;
    }

    if ( out_size != NULL )
        *out_size = out_len;

    return out_buf;
}

/*******************/

int pin_table(
    int xc_handle, unsigned int type, unsigned long mfn, domid_t dom)
{
    struct mmuext_op op;

    op.cmd = type;
    op.arg1.mfn = mfn;

    if ( xc_mmuext_op(xc_handle, &op, 1, dom) < 0 )
        return 1;

    return 0;
}

/* This is shared between save and restore, and may generally be useful. */
unsigned long csum_page(void *page)
{
    int i;
    unsigned long *p = page;
    unsigned long long sum=0;

    for ( i = 0; i < (PAGE_SIZE/sizeof(unsigned long)); i++ )
        sum += p[i];

    return sum ^ (sum>>32);
}
