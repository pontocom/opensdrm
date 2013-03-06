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

   $Id: MakeTheme.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include<stdio.h>
#ifdef WIN32
#include<io.h>
#define access(a,b) _access(a,b)
#else
#include <unistd.h>
#endif
#include<errno.h>
#include "ThemeZip.h"

int main(int argc, char *argv[])
{
    vector<string *> list;
    string           destFile;
    int              i;
    Error            err;
    ThemeZip         oZip;
    
    if (argc < 2)
    {
        printf("Usage: \n\n");
        printf("To zip a theme:\n");
        printf("   MakeTheme <.fat file> <file1> [file2] ...\n\n");
        printf("To unzip a theme:\n");
        printf("   MakeTheme -d <.fat file>\n");
        return -1;
    }
    
    destFile = argv[1];
    if (destFile == string("-d"))
    {
        destFile = argv[2];
        err = oZip.DecompressTheme(destFile, string("."));
        if (err == kError_NoErr)
        {
            printf("Theme unzipped ok.\n");
    	    return 0;
        }
        printf("Theme failed to unzip. Corrupt theme or full disk?\n");
        return -1;    
    }
    
    for(i = 2; i < argc; i++)
    {
	if (access(argv[i], 2))
        {
            printf("File %s: ", argv[i]);
            if (errno == ENOENT)
                printf("File not found.\n");
            else    
            if (errno == EACCES)
                printf("Permission denied.\n");
            else
                printf("Unknown error.\n");
            return 1;
        }            
        printf("Adding file %s.\n", argv[i]);
        list.push_back(new string(argv[i]));
    }            

    err = oZip.CompressThemeZip(destFile, list);
    if (err == kError_NoErr)
    {
       printf("Wrote theme file %s.\n", argv[1]);
       return 0;
    }
    else
    if (err == kError_WriteFile)
       printf("Cannot write to theme file. Disk full or permission denied.\n");
    else
    if (err == kError_ReadFile)
       printf("Cannot read from theme components: Permission denied.\n");
    else
    if (err == kError_FileNoAccess)
       printf("Cannot create theme file: Permission denied.\n");
    else
       printf("Unknown error. We suck. Please download the source and fix it. :-)\n");
       
    return 1;
}

