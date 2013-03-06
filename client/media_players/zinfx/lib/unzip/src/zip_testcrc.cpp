#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "zip_header.h"
#include "zip_testcrc.h"

int zip_test_crc(ZipCDR *h,FILE *infile) /* 0 = failure */
{
	ZipLocalHeader lh;
	unsigned char *buffer;
	int status=0;
	unsigned long size=0;

        if (!infile || !h)
            return 0;

	status = fseek(infile,h->place, SEEK_SET);
        if (status)
            return 0;

	status = zip_read_local_header(infile,&lh,1);
        if (!status)
            return 0;

        if (h->csize <= 0)
            return 0;

	buffer = new unsigned char[h->csize];
        if (!buffer)
            return 0;

	size = fread(buffer,h->csize,1,infile);
	if (h->method == 8) /* inflated */
		status = test_inflated(buffer, h->csize, h->crc);
	else if (h->method == 0) /* stored */
		status=test_stored(buffer, h->csize, h->crc);
	else
		status=Z_ERRNO;

        delete [] buffer;

	if (status != Z_OK)
		return 0;

	return 1;
}

int test_inflated(unsigned char *compr,unsigned long comprLen,
		  unsigned long original_crc)
{
  /* source code taken&modified from the source of infozip */
  int i=0;
  int windowBits=0;
  int status=Z_OK;
  unsigned char unCompr[0x8000+1];
  unsigned long crc;
  z_stream z;
  z.zalloc=Z_NULL;
  z.zfree=Z_NULL;
  z.opaque=0;
  for (i=0x8000,windowBits=0;!(i&1);i>>=1,++windowBits);
  
  crc=crc32(0L,Z_NULL,0);
  
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
	  status=inflate(&z,Z_PARTIAL_FLUSH);
	  if (status==Z_STREAM_END)
	    break;
	  if (status!=Z_OK && status!=Z_STREAM_END)
	    return status;

	  if (z.avail_in<=0)
	    {
	      z.next_in=compr;
	      z.avail_in=comprLen;
	    }
	}
      if (0x8000-z.avail_out>0)
	crc=crc32(crc,unCompr,0x8000-z.avail_out);

      z.next_out=unCompr;
      z.avail_out=0x8000;
    }
  while(status!=Z_STREAM_END)
    {
      status=inflate(&z,Z_PARTIAL_FLUSH);
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
  if (crc != original_crc)
    return Z_ERRNO;

  return Z_OK;
}

int test_stored(unsigned char *compr,unsigned long comprLen,
		unsigned long original_crc)
{
  unsigned char unCompr[0x8000+1];
  unsigned long crc;
  unsigned int place=0;
  
  crc=crc32(0L,Z_NULL,0);
  
  while(place<comprLen)
    {
      int len=0x8000;
      if (place+len>comprLen)
	len=comprLen-place;
      if (len>0)
	{
	  memcpy(unCompr,compr,len);
	  crc=crc32(crc,unCompr,len);
	  place+=len;
	}
    }
  if (crc != original_crc)
    return Z_ERRNO;

  return Z_OK;
}
