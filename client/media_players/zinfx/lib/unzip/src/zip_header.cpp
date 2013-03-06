#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "zip_header.h"

unsigned int read_byte(unsigned char *buffer,int *counter)
{
	int a=*counter;
	(*counter)++;
	return (unsigned int)buffer[a];
}

unsigned int read_int(unsigned char *buffer,int *counter)
{
	int b=0,c=0;
	b=read_byte(buffer,counter);
	c=read_byte(buffer,counter);
	return ((c<<8)+b);
}

unsigned long read_long(unsigned char *buffer,int *counter)
{
	unsigned long b=0,c=0,d=0,a=0;
	a=read_byte(buffer,counter);
	b=read_byte(buffer,counter);
	c=read_byte(buffer,counter);
	d=read_byte(buffer,counter);
	
	return ((d<<24)+(c<<16)+(b<<8)+a);
}

/* zip_header_find()
   Function searches next header from file.
   File can be arj/zip archive.  */

ZipHeaderType zip_find_header(FILE *file,int loop)
{
	int mode=0;
	char headerSign[3]={ 0x50,0x4b,0 };
	char arjHeaderSign[3]={ 0x60,0xea, 0 };
	while(loop!=0) {
		char ch;
		ch=fgetc(file);
		if (ch==headerSign[mode])
			mode++;
		else if (ch==headerSign[0])
                        mode=1;
                else
			mode=0;
		if (ch==arjHeaderSign[0]) {
			ch=fgetc(file);
			if (ch == arjHeaderSign[1])
				return ARJ_HEADER;
			fseek(file,-1,SEEK_CUR); /* go back 1 byte */
		}
		if (mode>=2) {
			char a,b;
			a=fgetc(file);
			b=fgetc(file);
			if (a == 0x01 && b == 0x02)
				return ZIP_CENTRAL_DIR_RECORD;
			else if (a == 0x03 && b == 0x04)
				return ZIP_LOCAL_HEADER;
			else if (a == 0x05 && b == 0x06)
				return ZIP_END_OF_CENTRAL_DIR_RECORD;
			fseek(file,-2,SEEK_CUR); /* 2 bytes back */
			mode=0;
		}
		if (feof(file))
			return ZIP_NONE;
		loop--;
	}
  
	return ZIP_NONE;
}

/*  zip_local_header_read()
    Function will read local header from zip archive.
    if verify = 1 then function will check local header signature */

int zip_read_local_header(FILE *file,ZipLocalHeader *h,int verify) /* 0 = failure */
{
	unsigned char tmp[27];
	unsigned long id;
	int counter=0;
	int status=0;
	int a;

	if (h == NULL)
		return 0;

	CLEAR_BUFFER(tmp,27,a);

	if (verify!=0) {
		fread(tmp,4,1,file);
		id=read_long(tmp,&counter);
                if (id != 0x04034b50)
                    return 0;
	}

	if (!feof(file)) {
		counter=0;
		status=0;
		status=fread(tmp,1,26,file);
                if (status != 26)
                    return 0;
		h->version=read_int(tmp,&counter);
		h->flag=read_int(tmp,&counter);
		h->method=read_int(tmp,&counter);
		h->modtime=read_int(tmp,&counter);
		h->moddate=read_int(tmp,&counter);
		h->crc=read_long(tmp,&counter);
		h->csize=read_long(tmp,&counter);
		h->size=read_long(tmp,&counter);
		h->filenamelen=read_int(tmp,&counter);
		h->extrafieldlen=read_int(tmp,&counter);
		h->filename='\0';
		if (h->filenamelen>0) {
			h->filename=(char*)malloc(h->filenamelen+1);
			if (h->filename) {
				fread(h->filename,h->filenamelen,1,file);
				h->filename[h->filenamelen]='\0';
			}
		}
		if (h->extrafieldlen>0)
			fseek(file,h->extrafieldlen,SEEK_CUR);

		h->place=ftell(file);
		
		if (h->flag&0x04) {
			cerr << "Ouhh, some header data is after "
		             << "compressed file\n";
			if (h->csize>0) {
				fseek(file,h->csize,SEEK_CUR);
				
				counter=0;
				status=fread(tmp,12,1,file);
                                if (status == 1)
                                    return 0;
				h->crc=read_long(tmp,&counter);
				h->csize=read_long(tmp,&counter);
				h->size=read_long(tmp,&counter);
				fseek(file,-(h->csize+12),SEEK_CUR);
			}
		}

	} else
		return 0;

	return 1;
}

/* zip_local_header_skip()
   Function will skip h->csize bytes (over compressed data) */

int zip_skip_local_header(FILE *file,ZipLocalHeader *h) /* 0 = failure */
{
	if (h->csize>0)
		fseek(file,h->csize,SEEK_CUR);

	if (h->flag&0x04)
		fseek(file,12,SEEK_CUR);

	return 1;
}

int zip_find_end_of_central_dir_record2(FILE *file,int place) /* 0 = failure */
{
	ZipHeaderType t;
	fseek(file,-place,SEEK_END);
	while(t=zip_find_header(file,1000),t!=ZIP_END_OF_CENTRAL_DIR_RECORD) {
		if (t==ZIP_NONE)
			return 0;
	}
	
	return 1;
}


int zip_find_end_of_central_dir_record(FILE *file) /* 0 = failure */
{
	ZipHeaderType t;
	fseek(file,-30,SEEK_END);
	while(t=zip_find_header(file,1000),t!=ZIP_END_OF_CENTRAL_DIR_RECORD) {
		if (t==ZIP_NONE)
			return zip_find_end_of_central_dir_record2(file,1000);
	}
	
	return 1;
}

/* zip_read_end_of_central_dir_record()
   Function reads end of central dir record from zip archive  */

int zip_read_end_of_central_dir_record(FILE *file,ZipCDREnd *h)
 /* 0 = failure */
{
	unsigned char tmp[19];
	int a;

	if (h == NULL)
		return 0;

	CLEAR_BUFFER(tmp,19,a);

	if (!feof(file)) {
		int counter=0;
		int status=0;
		status=fread(tmp,1,18,file);
                if (status != 18)
                    return 0;
		h->disknumber=read_int(tmp,&counter);
		h->startdisknumber=read_int(tmp,&counter);
		h->totalentriesonthisdisk=read_int(tmp,&counter);
		h->totalentries=read_int(tmp,&counter);
		h->size=read_long(tmp,&counter);
		h->start=read_long(tmp,&counter);
		h->commentlen=read_int(tmp,&counter);
		h->comment=0;
		
	} else
		return 0;

	return 1;
}

int zip_goto_central_dir_record_start(FILE *file,ZipCDREnd *h)
{
	int status=0;
        if (!h)
            return 0;

	status=fseek(file,h->start,SEEK_SET);

        if (status)
            return 0;

	return 1;
}

int zip_read_central_dir_record(FILE *file,ZipCDR *h)
{
	unsigned char tmp[43];
	unsigned long id;
	int counter=0;
	int status=0;
	int a;

	if (h == NULL)
		return 0;

	CLEAR_BUFFER(tmp,43,a);

	fread(tmp,4,1,file);
	id=read_long(tmp,&counter);
        if (id != 0x02014b50)
            return 0;

	if (!feof(file)) {
		counter=0;
		status=0;
		status=fread(tmp,1,42,file);
		h->madebyver=read_int(tmp,&counter);     /* 2 bytes */
		h->neededver=read_int(tmp,&counter);     /* 2 */
		h->flag=read_int(tmp,&counter);          /* 2 */
		h->method=read_int(tmp,&counter);        /* 2 */
		h->modtime=read_int(tmp,&counter);       /* 2 */
		h->moddate=read_int(tmp,&counter);       /* 2 */
		h->crc=read_long(tmp,&counter);          /* 4 */
		h->csize=read_long(tmp,&counter);        /* 4 */
		h->size=read_long(tmp,&counter);         /* 4 */
		h->filenamelen=read_int(tmp,&counter);   /* 2 */
		h->extrafieldlen=read_int(tmp,&counter); /* 2 */
		h->commentlen=read_int(tmp,&counter);    /* 2 */
		h->disknumberstart=read_int(tmp,&counter); /* 2 */
		h->iattribs=read_int(tmp,&counter);      /* 2 */
		h->eattribs=read_long(tmp,&counter);     /* 4 */
		h->place=read_long(tmp,&counter);        /* 4 */
		h->filename='\0';
		if (h->filenamelen>0) {
			h->filename=(char*)malloc(h->filenamelen+1);
			if (h->filename) {
				fread(h->filename,h->filenamelen,1,file);
				h->filename[h->filenamelen]='\0';
			}
		}
		if (h->extrafieldlen>0)
			fseek(file,h->extrafieldlen,SEEK_CUR);
		if (h->commentlen>0)
			fseek(file,h->commentlen,SEEK_CUR);
	} else
		return 0;

	return 1;
}
