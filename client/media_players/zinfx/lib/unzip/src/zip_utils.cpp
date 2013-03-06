#include "config.h"

#ifdef WIN32
#include <direct.h> 
#define MKDIR(z) mkdir(z)
#else
#include <unistd.h>
#define MKDIR(z) mkdir(z, 0700)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <zlib.h>


#include "zip_utils.h"

void make_dirs(const char *path)
{
  char dirname[FNAME_MAX];
  char *cp;
  
  strcpy(dirname,path);
  cp=dirname;
  while((cp=strchr(cp,'/')))
    {
      *cp=0;
      MKDIR(dirname);
      *cp='/';
      cp++;
    }
}

void convert_date(unsigned int d,int *yr,int *mo,int *dy)
{
  *yr=((d>>9)&0x7f)+1980;
  *mo=((d>>5)&0x0f);
  *dy=d&0x1f;
}
