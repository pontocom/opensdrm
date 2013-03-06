#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>

#include "zip_header.h"
#include "zip_utils.h"
#include "zip_inflate.h"

int uncompress_file(ZipCDR *h,FILE *infile,FILE *outfile)
{
	ZipLocalHeader lh;
	unsigned char *buffer;
	int status=0;
	unsigned long size=0;

        if (!infile || !h || !outfile)
            return 0;

	status=fseek(infile,h->place,SEEK_SET);
        if (status)
            return 0;

	status=zip_read_local_header(infile,&lh,1);
        if (!status)
            return 0;

        if (h->csize <= 0)
            return 0;

	buffer= new unsigned char[h->csize];
        if (!buffer)
            return 0;

	size=fread(buffer,h->csize,1,infile);
	if (h->method == 8) /* inflated */
		status=uncompress_inflated(buffer,h->csize,outfile);
	else if (h->method == 0) /* stored */
		status=uncompress_stored(buffer,h->csize,outfile);
	else
		status=Z_ERRNO;

        delete [] buffer;

	if (status!=Z_OK)
		return 0;

	return 1;
}

/* uncompress_file_with_dir()
   extra_flags:
    0x01 - skip errors
    0x02 - lowercase names */

int uncompress_file_with_dir(ZipCDR *h,FILE *infile,const char *dir,
			     int extra_flags)
{
	char *filename=0;
	char *filename2;
        char *nodir;
	FILE *outfile=0;
	int status=0;

        if (!h || !h->filename || !dir || !infile)
            return 0;

        filename2 = new char[strlen(h->filename) + 1];
        if (!filename2)
            return 0;

        if ((nodir = strrchr(h->filename, DIR_MARKER))) {
            nodir++;
            if (nodir && *nodir)
                strcpy(filename2, nodir);
            else
                strcpy(filename2, h->filename);
        }
        else 
            strcpy(filename2, h->filename);

	if (extra_flags&0x02) {
                char *s = filename2;
                while (*s) {
                    *s = tolower(*s);
                    s++;
                }
	}

        filename = new char[strlen(dir) + strlen(filename2) + 5];
        sprintf(filename, "%s/%s", dir, filename2);
	delete [] filename2;

	make_dirs(filename);

	outfile=fopen(filename,"wb");
	if (outfile == NULL)
		return 0;

	status=uncompress_file(h,infile,outfile);

	delete [] filename;
	fclose(outfile);
	
	return status;
}

int uncompress_inflated(unsigned char *compr,
			unsigned long comprLen, FILE *file)
{
  int i=0;
  int windowBits=0;
  int status=Z_OK;
  unsigned char unCompr[0x8000+1];
  z_stream z;
  z.zalloc=Z_NULL;
  z.zfree=Z_NULL;
  z.opaque=0;
  for (i=0x8000,windowBits=0;!(i&1);i>>=1,++windowBits);
  
  z.avail_out=0x8000;
  z.next_out=unCompr;
  z.next_in=compr;
  z.avail_in=comprLen;
  
  status=inflateInit2(&z,-windowBits);
  if (status!=Z_OK)
    return status;

  while(status!=Z_STREAM_END)
    {
      while(z.avail_out>0)
	{
	  status=inflate(&z,Z_SYNC_FLUSH);
	  
	  if (status==Z_STREAM_END)
	    break;
	  else if (status!=Z_OK)
	    return status;

	  if (z.avail_in<=0)
	    {
	      z.next_in=compr;
	      z.avail_in=comprLen;
	    }
	}

      fwrite(unCompr,0x8000-z.avail_out,1,file);
      z.next_out=unCompr;
      z.avail_out=0x8000;
    }
  while(status!=Z_STREAM_END)
    {
      status=inflate(&z,Z_SYNC_FLUSH);
      if (status==Z_STREAM_END)
	break;
      if (status!=Z_OK)
	return status;
      z.next_out=unCompr;
      z.avail_out=0x8000;
    }

  status=inflateEnd(&z);
  if (status!=Z_OK)
    {
      return status;
    }

  return Z_OK;
}

int uncompress_stored(unsigned char *compr,unsigned long comprLen, FILE *file)
{
  unsigned int size=0;
  size=fwrite(compr,comprLen,1,file);
  if (size!=comprLen)
    return Z_ERRNO;

  return Z_OK;
}
