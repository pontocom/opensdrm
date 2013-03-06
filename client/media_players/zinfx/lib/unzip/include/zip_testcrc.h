#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "zip_header.h"

#ifndef TK_ZIP_TESTCRC_H
#define TK_ZIP_TESTCRC_H

#ifdef __cplusplus
extern "C" {
#endif

int zip_test_crc(ZipCDR *h,FILE *infile); /* 0 = failure */
int test_inflated(unsigned char *compr,unsigned long comprLen,
		  unsigned long original_crc); /* Z_OK = success */
int test_stored(unsigned char *compr,unsigned long comprLen,
		unsigned long original_crc); /* Z_OK = success */

#ifdef __cplusplus
}
#endif

#endif
