/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Id: ThemeZip.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include <time.h>
#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#include <direct.h>
#define unlink(a) _unlink(a)
#else
#define TRUE 1
#define FALSE 0
#include <unistd.h>
#include <netinet/in.h>
#include "win32impl.h"
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#include "zlib.h"
#include "config.h"
#include "ThemeZip.h"
#include "debug.h"

#include "zip_wrapper.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

const int iMajorVersion = 1;
const int iMinorVersion = 0;
const int iBufferSize = 8192;

ThemeZip::ThemeZip(void)
{
}

ThemeZip::~ThemeZip(void)
{
}

//
//
// Standard Archive Format - Standard TAR - USTAR
//
#define  RECORDSIZE  512
#define  NAMSIZ      100
#define  TUNMLEN      32
#define  TGNMLEN      32

#if WIN32
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
union tar_record {
    char   charptr[RECORDSIZE];
    struct _header {
        char    name[NAMSIZ];
        char    mode[8];
        char    uid[8];
        char    gid[8];
        char    size[12];
        char    mtime[12];
        char    chksum[8];
        char    typeflag;
        char    linkname[NAMSIZ];
        char    magic[8];
        char    uname[TUNMLEN];
        char    gname[TGNMLEN];
        char    devmajor[8];
        char    devminor[8];
    } header;
};
#if WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

// The checksum field is filled with this while the checksum is computed.
#define    CHKBLANKS    "        "        // 8 blanks, no null 
// The magic field is filled with this if uname and gname are valid.
#define    TMAGIC    "ustar  "        // 7 chars and a null

/* Values used in typeflag field.  */
#define REGTYPE  '0'        /* regular file */
#define AREGTYPE '\0'       /* regular file */
#define LNKTYPE  '1'        /* link */
#define SYMTYPE  '2'        /* reserved */
#define CHRTYPE  '3'        /* character special */
#define BLKTYPE  '4'        /* block special */
#define DIRTYPE  '5'        /* directory */
#define FIFOTYPE '6'        /* FIFO special */
#define CONTTYPE '7'        /* reserved */


Error ThemeZip::CompressThemeZip(const string &oDestFile, 
                                 vector<string *> &oFileList)
{
   gzFile  pOut;
   FILE   *pIn;
   char   *pBuffer;
   tar_record TarRecord;
   string  oFile;
   vector<string *>::iterator i;
   int     iWrite, iRead, iSize, iBlock,iPadding;

   pOut = gzopen(oDestFile.c_str(), "wb");
   if (pOut == NULL)
       return kError_FileNoAccess;

   pBuffer = new char[iBufferSize];
   for(i = oFileList.begin(); i != oFileList.end(); i++)
   {
       char *pPtr;
       time_t ltime;

       oFile = *(*i);
       pPtr = strrchr(oFile.c_str(), DIR_MARKER);
       if (pPtr)
           oFile.erase(0, ((int)pPtr - (int)oFile.c_str()) + 1);

       pIn = fopen((*i)->c_str(), "rb");
       if (pIn == NULL)
       {
           gzclose(pOut);
           delete [] pBuffer;
           return kError_FileNotFound;
       }

       // create header
       memset(&TarRecord,0,sizeof(tar_record));
       strcpy(TarRecord.header.name,oFile.c_str());
       strcpy(TarRecord.header.mode,"100644 ");
       strcpy(TarRecord.header.uid,"   764 ");
       strcpy(TarRecord.header.gid,"   144 ");

       fseek(pIn, 0, SEEK_END);
       iSize = ftell(pIn);
       fseek(pIn, 0, SEEK_SET);
       sprintf(TarRecord.header.size,"%11o",iSize);
       TarRecord.header.size[11]='\x20';

       time(&ltime);
       sprintf(TarRecord.header.mtime,"%11lo",ltime);
       TarRecord.header.mtime[11]='\x20';

       TarRecord.header.typeflag = REGTYPE; // normal file
       strcpy(TarRecord.header.magic,TMAGIC);


       // last, create checksum
       memcpy(TarRecord.header.chksum,CHKBLANKS,8);
       int ctr,sum;
       for(ctr=sum=0;ctr<(int)sizeof(tar_record);ctr++) 
           sum+=TarRecord.charptr[ctr];
       sprintf(TarRecord.header.chksum,"%6o",sum);

       if (gzwrite(pOut, (void *)&TarRecord, sizeof(tar_record)) != sizeof(tar_record))
       {
           fclose(pIn);
           gzclose(pOut);
           delete [] pBuffer;
           return kError_WriteFile;
       }


       iPadding = (iSize/512+1)*512-iSize;

       for(; iSize > 0;)
       {
           iBlock = min(iSize, iBufferSize);
           iRead = fread(pBuffer, 1, iBlock, pIn);
           if (iRead != iBlock)
           {
               fclose(pIn);
               gzclose(pOut);
               delete [] pBuffer;
               return kError_ReadFile;
           }
           iWrite = gzwrite(pOut, pBuffer, iBlock);
           if (iWrite != iBlock)
           {
               fclose(pIn);
               gzclose(pOut);
               delete [] pBuffer;
               return kError_WriteFile;
           }

           iSize -= iBlock;
       }          
       fclose(pIn);

       if(iPadding)
       {
           memset(pBuffer,0,iPadding);
           gzwrite(pOut,pBuffer,iPadding);
       }
   }
   delete [] pBuffer;

   // write end-marker (zero-ed tar-record)
   memset(&TarRecord,0,sizeof(tar_record));
   gzwrite(pOut,(void*)&TarRecord, sizeof(tar_record));
   // ... and padding (and why the hell TAR needs extra 4kb with zeroes?)
   iSize=gztell(pOut);
   iPadding = (iSize/512+1)*512-iSize;
   if(iPadding)
   {
       memset(pBuffer,0,0x1000+iPadding);
       gzwrite(pOut,pBuffer,0x1000+iPadding);
   }

   gzclose(pOut);

   return kError_NoErr;
}

// some trophy from TAR code -------------------------

#define ISODIGIT(Char) \
  ((unsigned char) (Char) >= '0' && (unsigned char) (Char) <= '7')
#define ISSPACE(Char) (isascii (Char) && isspace (Char))


long from_oct (int digs, char *where)
{
  long value;

  while (ISSPACE (*where))
    {               /* skip spaces */
      where++;
      if (--digs <= 0)
    return -1;      /* all blank field */
    }
  value = 0;
  while (digs > 0 && ISODIGIT (*where))
    {
      /* Scan til nonoctal.  */

      value = (value << 3) | (*where++ - '0');
      --digs;
    }

  if (digs > 0 && *where && !ISSPACE (*where))
    return -1;          /* ended on non-space/nul */

  return value;
}

// ---------------------------- TAR code ends

Error ThemeZip::DecompressTheme(const string &oSrcFile, const string &oDestPath)
{
    char buf[4];
    FILE *f;
    
    f = fopen(oSrcFile.c_str(), "rb");
    if (!f)
        return kError_FileNotFound;

    for (uint32 i = 0; i < 4; i++)
        fread(&buf[i], 1, 1, f);
    fclose(f);

    if (!strncmp("PK\003\004", buf, 4))
        return DecompressZip(oSrcFile, oDestPath);
    return DecompressGZ(oSrcFile, oDestPath);   
}

Error ThemeZip::DecompressZip(const string &oSrcFile, const string &oDestPath)
{
   ZipFile *zip = zip_file_new();
   
   if (!zip)
       return kError_FileNotFound;

   if (!zip_file_open(oSrcFile.c_str(), zip))
       return kError_FileNotFound;

   if (!zip_file_uncompress_all(oDestPath.c_str(), zip, 0x03))
       return kError_NoDataAvail;

   zip_file_free(zip);

   return kError_NoErr;
}

Error ThemeZip::DecompressGZ(const string &oSrcFile, const string &oDestPath)
{
   FILE   *pOut;
   gzFile  pIn;
   tar_record TarRecord;

   char   *pBuffer;
   int     iWrite, iRead, iSize, iBlock, iPadding;
   int     fDone,fOneDirectory;
   string  oFile,oUnpackFile;

   if (oSrcFile.length() == 0)
      return kError_FileNotFound;

   m_oCreatedFiles.clear();

   pIn = gzopen(oSrcFile.c_str(), "rb");
   if (pIn == NULL)
       return kError_FileNotFound;

   pBuffer = new char[iBufferSize];
   fDone = FALSE;
   fOneDirectory = FALSE; // make sure TAR contains only one directory at most

   //
   // unpack the TAR inside GZ
   //

   while(!fDone)
   {
       if (gzread(pIn, &TarRecord, sizeof(tar_record)) != sizeof(tar_record))
       {
           gzclose(pIn);
           delete [] pBuffer;
           CleanupThemeZip();
           return kError_ReadFile;
       }

       //
       // sanity checks
       //

       // check if we are done (header record all 0)
       int ctr,sum;
       for(ctr=sum=0; ctr<(int)sizeof(tar_record); ctr++) 
           sum+=TarRecord.charptr[ctr];
       if(sum==0)
       {
           // empty record detected, bail out
           fDone = TRUE;
           break;
       }

       // check for magic
       if(TarRecord.header.magic[7]!=0 || strcmp(TarRecord.header.magic,TMAGIC))
       {
           // nope, something not right
           gzclose(pIn);
           delete [] pBuffer;
           CleanupThemeZip();
           return kError_NoDataAvail;
       }

       // check if checksum is still OK
       int our_sum = from_oct(6,TarRecord.header.chksum);
       memcpy(TarRecord.header.chksum,CHKBLANKS,8);
       for(ctr=sum=0;ctr<(int)sizeof(tar_record);ctr++) 
           sum+=TarRecord.charptr[ctr];
       if(sum!=our_sum)
       {
           // checksum failed
           gzclose(pIn);
           delete [] pBuffer;
           CleanupThemeZip();
           return kError_NoDataAvail;
       }

       // check if we are dealing with file
       if(TarRecord.header.typeflag == REGTYPE || TarRecord.header.typeflag == AREGTYPE)
       {
           // all fine, just proceed
           ;
       }
       else
       {
           if(TarRecord.header.typeflag == DIRTYPE && !fOneDirectory)
           {
               // will tolearate one directory
               fOneDirectory = TRUE;

               continue; // go on to next record
           }
           else
           {
               // ... but not many:
               // this archive is likely not theme anyway!
               gzclose(pIn);
               delete [] pBuffer;
               CleanupThemeZip();
               return kError_NoDataAvail;
           }
       }
       // ... sanity checks ok, proceed with file


       //
       // the unpacking itself
       //

       oUnpackFile = string(TarRecord.header.name);
       // in case we have sub-directory in tar, take only filename
       unsigned int uPos;
       uPos=oUnpackFile.rfind('/');
       if(uPos!=oUnpackFile.npos) oUnpackFile.erase(0,uPos+1);

       // open output file
       oFile = oDestPath + string(DIR_MARKER_STR) + oUnpackFile;
       pOut = fopen(oFile.c_str(), "wb");
       if (pOut == NULL)
       {
           gzclose(pIn);
           delete [] pBuffer;
           CleanupThemeZip();
           return kError_FileNotFound;
       }
       m_oCreatedFiles.push_back(oFile);

       iSize = from_oct(11,TarRecord.header.size); 
       iPadding = (iSize/512+1)*512-iSize;

       // write it's data out
       for(; iSize > 0;)
       {
           iBlock = min(iSize, iBufferSize);
           iRead = gzread(pIn, pBuffer, iBlock);
           if (iRead != iBlock)
           {
               gzclose(pIn);
               fclose(pOut);
               delete [] pBuffer;
               CleanupThemeZip();
               return kError_ReadFile;
           }
           iWrite = fwrite(pBuffer, 1, iBlock, pOut);
           if (iWrite != iBlock)
           {
               gzclose(pIn);
               fclose(pOut);
               delete [] pBuffer;
               CleanupThemeZip();
               return kError_WriteFile;
           }

           iSize -= iBlock;
       }          
       fclose(pOut);

       if(iPadding)
       {
           gzseek(pIn, iPadding, SEEK_CUR);
       }

   }

   // all went fine

   delete [] pBuffer;
   gzclose(pIn);

   return kError_NoErr;
}

Error ThemeZip::GetDescriptiveName(const string &oSrcFile, string &oDescriptiveName)
{
    gzFile pIn;
    tar_record TarRecord;
    int iSize, iPadding;
    int fDone;

    // sanity check
    if (oSrcFile.length() == 0)
       return kError_FileNotFound;

    oDescriptiveName=string(""); // return at least empty string in any case

    // open theme file
    pIn = gzopen(oSrcFile.c_str(), "rb");
    if (pIn == NULL)
        return kError_FileNotFound;

    //
    // loop to find directory record (descriptive name, that is)
    //
    fDone = FALSE;
    while(!fDone)
    {
        if (gzread(pIn, &TarRecord, sizeof(tar_record)) != sizeof(tar_record))
        {
            gzclose(pIn);
            return kError_ReadFile;
        }

        //
        // sanity checks
        //

        // check if we are done (header record all 0)
        int ctr,sum;
        for(ctr=sum=0; ctr<(int)sizeof(tar_record); ctr++) 
            sum+=TarRecord.charptr[ctr];
        if(sum==0)
        {
            // empty record detected, bail out
            fDone = TRUE;
            break;
        }

        // check for magic
        if(TarRecord.header.magic[7]!=0 || strcmp(TarRecord.header.magic,TMAGIC))
        {
            // nope, something not right
            gzclose(pIn);
            return kError_NoDataAvail;
        }

        // check if checksum is still OK
        int our_sum = from_oct(6,TarRecord.header.chksum);
        memcpy(TarRecord.header.chksum,CHKBLANKS,8);
        for(ctr=sum=0;ctr<(int)sizeof(tar_record);ctr++) 
            sum+=TarRecord.charptr[ctr];
        if(sum!=our_sum)
        {
            // checksum failed
            gzclose(pIn);
            return kError_NoDataAvail;
        }

        // see if it is directory record we are looking for
        if(TarRecord.header.typeflag == DIRTYPE)
        {
            unsigned int uPos;
            oDescriptiveName = string (TarRecord.header.name);

            // erase trailing slash
            uPos=oDescriptiveName.rfind('/');
            if(uPos!=oDescriptiveName.npos) oDescriptiveName.erase(uPos,uPos+1);
            // erase (possible) extension
            uPos=oDescriptiveName.rfind(".fat");
            if(uPos!=oDescriptiveName.npos) oDescriptiveName.erase(uPos,uPos+1);

            break; //we're done
        }

        if (strcasecmp(TarRecord.header.name, "title.txt") == 0)
        {
            char *pText;
            
            iSize = from_oct(11,TarRecord.header.size);
            if(iSize)
            {
                char *pPtr;
                
                iPadding = (iSize/512+1)*512-iSize;
                pText = new char[iSize + iPadding + 1];
                if (gzread(pIn, pText, iSize+iPadding) != iSize+iPadding)
                {
                    gzclose(pIn);
                    return kError_ReadFile;
                }
                
                pPtr = strchr(pText, '\n');
                if (pPtr)
                   *pPtr = 0;
                pPtr = strchr(pText, '\r');
                if (pPtr)
                   *pPtr = 0;
                   
                oDescriptiveName = pText;
                delete [] pText;
                break;
            }
            
            continue;
        }

        // skip actual content
        iSize = from_oct(11,TarRecord.header.size);
        if(iSize)
        {
            iPadding = (iSize/512+1)*512-iSize;
            gzseek(pIn, iSize+iPadding, SEEK_CUR);
        }
    }

    gzclose(pIn);
    return kError_NoErr;
}

Error ThemeZip::CleanupThemeZip(void)
{
   vector<string>::iterator i;
   
   for(i = m_oCreatedFiles.begin(); i != m_oCreatedFiles.end(); i++)
   {
   	   if (unlink((*i).c_str()))
           return kError_UnlinkFailed;
   }        

   return kError_NoErr;
}      

Error ThemeZip::CleanupThemeZip(string &oDir)
{
   WIN32_FIND_DATA find;
   HANDLE handle;
   string oBaseDir;

   oBaseDir = oDir + string(DIR_MARKER_STR) + string("*");
#ifdef WIN32
   oBaseDir += string(".*");
#endif
   handle = FindFirstFile((char *)oBaseDir.c_str(), &find);
   if (handle != INVALID_HANDLE_VALUE) {
       do {
           string rmfile = oDir + string(DIR_MARKER_STR) + 
                           string(find.cFileName);
           if (!strcmp(".", find.cFileName) || !strcmp("..", find.cFileName))
               continue;
           if (!(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
               if (unlink(rmfile.c_str())) {
                   FindClose(handle);
                   return kError_UnlinkFailed;
               }
           }
           else {
               if (rmdir(rmfile.c_str())) {
                   CleanupThemeZip(rmfile);
                   if (rmdir(rmfile.c_str())) {
                       FindClose(handle);
                       return kError_UnlinkFailed;
                   }
               }
           }
       } while (FindNextFile(handle, &find));
       FindClose(handle);
   }
   return kError_NoErr;
}

#if 0
void main(int argc, char **argv)
{
   ThemeZip a;
   vector<string *> list;
   string src;

   if (argv[1][0] == 'e')
   {
       int i;

       for(i = 3; i < argc; i++)
       {
           string *p;

           p = new string(argv[i]);
           list.push_back(p);
       }
       int r = a.CompressThemeZip(string(argv[2]), list);
       printf("ret: %d", r);
       exit(0);
   }  
   if (argv[1][0] == 'd')
   {
       int r = a.DecompressTheme(string(argv[2]), string(argv[3]));
       printf("ret: %d", r);
       exit(0);
   }  
   printf("Did nothing\n");
}
#endif
