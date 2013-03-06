
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include <vector>
using namespace std;

#include "zip_header.h"
#include "zip_inflate.h"
#include "zip_testcrc.h"
#include "zip_utils.h"

#ifndef TK_ZIP_WRAPPER_H
#define TK_ZIP_WRAPPER_H

typedef struct _ZipFile {
	char *filename;
	
	vector<ZipCDR *> *records; /* central dir records */
	ZipCDREnd *cdre;

	int opened;

} ZipFile;

typedef struct _ZipFileVars{
	int skip_errors;
	const char *dir;
	int error_count;
	FILE *file;
	vector<char *> *errors;
} ZipFileVars;

#ifdef __cplusplus
extern "C" {
#endif

ZipFile* zip_file_new(void); /* NULL = failure */
int zip_file_open(const char *filename,ZipFile *zip); /* 0 = failure */
int zip_file_add_records_with_file(FILE *file,ZipFile *zip); /* 0 = failure */
int zip_file_add_records(ZipFile *zip); /* 0 = failure */
void zip_file_free_current(ZipCDR *data);
int zip_file_free(ZipFile *zip); /* 0 = failure */
int zip_file_close(ZipFile *zip); /* 0 = failure */
int zip_file_uncompress_all(const char *dir,
			    ZipFile *zip,int extra_flags); /* 1 = success */
int zip_file_uncompress_nth(unsigned int n,const char *dir,ZipFile *zip,
			    int extra_flags); /* 1 = success */
void zip_file_uncompress_current(ZipCDR *r, ZipFileVars *vars);
vector<char *> *zip_file_test(ZipFile *zip,int *status); /* damaged files are in list */
void zip_file_test_current(ZipCDR *r, ZipFileVars *vars);

#ifdef __cplusplus
}
#endif

#endif
