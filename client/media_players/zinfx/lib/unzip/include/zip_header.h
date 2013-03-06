#include <stdio.h>
#include <stdlib.h>

#ifndef TK_ZIP_HEADER_H
#define TK_ZIP_HEADER_H

typedef struct _ZipLocalHeader {
	unsigned int version;
	unsigned int flag;
	unsigned int method;
	unsigned int modtime;
	unsigned int moddate;
	unsigned long crc;
	unsigned long csize; /* compressed size */
	unsigned long size;
	unsigned int filenamelen;
	unsigned int extrafieldlen;
	unsigned long place; /* place for compressed data */
	char *filename;
} ZipLocalHeader;

typedef struct _ZipCentralDirRecord {
	unsigned int madebyver;
	unsigned int neededver;
	unsigned int flag;
	unsigned int method;
	unsigned int modtime;
	unsigned int moddate;
	unsigned long crc;
	unsigned long csize;
	unsigned long size;
	unsigned int filenamelen;
	unsigned int extrafieldlen;
	unsigned int commentlen;
	unsigned int disknumberstart;
	unsigned int iattribs;
	unsigned long eattribs;
	unsigned long place; /* place for local header */
	
	char *filename;
	/* char *comment;
	   char *extrafield; */
	
} ZipCDR;

typedef struct _ZipEndOfCentralDirRecord
{
	unsigned int disknumber;
	unsigned int startdisknumber;
	unsigned int totalentriesonthisdisk;
	unsigned int totalentries;
	unsigned long size;
	unsigned long start;
	unsigned int commentlen;
	
	char *comment;

} ZipCDREnd;

typedef enum {
	ZIP_NONE,
	ZIP_LOCAL_HEADER,
	ZIP_CENTRAL_DIR_RECORD,
	ZIP_END_OF_CENTRAL_DIR_RECORD,
	ARJ_HEADER,
} ZipHeaderType;

#ifdef __cplusplus
extern "C" {
#endif
	
unsigned int read_byte(unsigned char *buffer,int *counter);
unsigned int read_int(unsigned char *buffer,int *counter);
unsigned long read_long(unsigned char *buffer,int *counter);

ZipHeaderType zip_find_header(FILE *file,int loop); /* 0 = failure */
int zip_read_local_header(FILE *file,ZipLocalHeader *h,int verify); /* 0=fail*/
int zip_skip_local_header(FILE *file,ZipLocalHeader *h); /* 0 = failure */

int zip_find_end_of_central_dir_record2(FILE *file, int place); /* 0=failure*/
int zip_find_end_of_central_dir_record(FILE *file); /* 0 = failure */
int zip_read_end_of_central_dir_record(FILE *file,ZipCDREnd *h); /* 0 = fail */

int zip_goto_central_dir_record_start(FILE *file,ZipCDREnd *h); /* 0 = fail */
int zip_read_central_dir_record(FILE *file,ZipCDR *h); /* 0 = failure */

#ifndef CLEAR_BUFFER
#define CLEAR_BUFFER(x,y,z) for (z=0;z<y;z++) x[z]='\0';
#endif

#ifdef __cplusplus
}
#endif

#endif
