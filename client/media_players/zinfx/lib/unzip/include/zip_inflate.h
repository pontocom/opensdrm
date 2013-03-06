#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "zip_header.h"

#ifndef TK_INFLATE_H
#define TK_INFLATE_H

#ifdef __cplusplus
extern "C" {
#endif

int uncompress_file(ZipCDR *h,FILE *infile,FILE *outfile); /* 0 = failure */
int uncompress_file_with_dir(ZipCDR *h,FILE *infile,
			     const char *dir,int extra_flags);
int uncompress_inflated(unsigned char *compr,
			unsigned long comprLen,
			FILE *file); /* Z_OK = success */
int uncompress_stored(unsigned char *compr,
		      unsigned long comprLen,
		      FILE *file); /* Z_OK = success */

#ifdef __cplusplus
}
#endif

#endif
