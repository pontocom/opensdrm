#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <iostream.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>

#include "win32impl.h"

const char *SpecialLibraries[] = 
{
    "alsa",
    "esound",
    "\0"
};

class FindData {
 public:
    DIR *pDir;
    char *dir; // points to directory name of search
    char *rest; // points to remainder of the filename, which we should match against
    ~FindData();
    FindData();
};
FindData::FindData() {
    dir = NULL;
    rest = NULL;
    pDir = NULL;
}
FindData::~FindData() {
}

bool Match(char *,char *);
void FillWin32FindData(char *,char *,WIN32_FIND_DATA *);

HANDLE FindFirstFile(char *lpFileName, WIN32_FIND_DATA *lpFindFileData) {
    //cout << "FindFirstFile: begin" << endl;
    if (!lpFileName || !lpFindFileData) return INVALID_HANDLE_VALUE;

    FindData *pFD = new FindData();
    // 1) find earliest wildcard, chop up to directory marker
    char *ps = strchr(lpFileName,'*');
    char *pQ = strchr(lpFileName,'?');
    char *chopChar = '\0';
    if (!ps && !pQ) {
	// no wildcards... fill in the win32_find_data structure
	FillWin32FindData(NULL,lpFileName,lpFindFileData);
	// no more files...
	pFD->pDir = NULL;
	pFD->dir = NULL;
	pFD->rest = NULL;
	return (HANDLE)pFD;
    } else if (!ps && pQ) {
	chopChar = pQ;
    } else if (ps && !pQ) {
	chopChar = ps;
    } else {
	chopChar = (ps > pQ) ? pQ : ps;
    }

    char holdChar = *chopChar;
    *chopChar = '\0';
    char *lastSlash = strrchr(lpFileName, '/');
    *chopChar = holdChar;
    char *pDir, *pRest;
    if (lastSlash == lpFileName) {
        pRest = lastSlash + 1;
        pFD->dir = new char[2];
        pFD->rest = new char[strlen(pRest)+1];
        strcpy(pFD->dir, "/");
        strcpy(pFD->rest, pRest);
    } else if (lastSlash) {
	pDir = lpFileName;
	pRest = lastSlash + sizeof(char);
	*lastSlash = '\0';
	pFD->dir = new char[strlen(pDir)+1];
	pFD->rest = new char[strlen(pRest)+1];
	strcpy(pFD->dir,pDir);
	strcpy(pFD->rest,pRest);
	*lastSlash = '/';
    } else {
	pDir = NULL;
	pRest = lpFileName;
	pFD->rest = new char[strlen(pRest)+1];
	strcpy(pFD->rest,pRest);
    }

    if (pFD->dir) {
	pFD->pDir = opendir(pFD->dir);
	if (pFD->pDir) {
	    struct dirent *pdirent = readdir(pFD->pDir);
	    while (pdirent) {
		if (Match(pFD->rest,pdirent->d_name)) {
		    FillWin32FindData(pFD->dir, pdirent->d_name,lpFindFileData);
		    return (HANDLE)pFD;
		}
		pdirent = readdir(pFD->pDir);
	    }
        }
    }

    FindClose(pFD);
    return INVALID_HANDLE_VALUE;
}

bool Match(char *pattern,char *string) {
    //cout << "Comparing " << pattern << " to " << string << endl;
    if (!pattern || !string) return false;
    char *ps1 = pattern;
    char *ps2 = string;

    while ((*ps1 != '\0') && (ps2 != '\0')) {
	switch (*ps1) {
	    case '?':
		ps1++;
		ps2++;
		break;
	    case '*': {
	        // find existance of next block
		ps1++;
                if (*ps1 == '\0')
                    return true;
		char *pS = strchr(ps1,'*');
		char *pQ = strchr(ps1,'?');
		if (pS) {*pS = '\0';}  if (pQ) {*pQ = '\0';}
		char *pStr = strstr(ps2,ps1);
		if (pS) {*pS = '*';} if (pQ) {*pQ = '?';}
		if (pStr) {
		    ps2 = pStr;
		    return Match(ps1,ps2);
		}
		return false;
		break; }
	    default:
		if (tolower(*ps1) != tolower(*ps2)) return false;
		ps1++;
		ps2++;
		break;
	}
    }
    if ((*ps1 == '\0') && (*ps2 == '\0'))
	return true;
    else
	return false;
}


void FillWin32FindData(char *pDir, char *pF,WIN32_FIND_DATA *wfd) {
//    cout << "FillWin32FindData: Entering: " << pF << endl;
    strncpy(wfd->cFileName,pF, MAX_PATH);

    struct stat st;
    char *name;
    if (pDir)
        name = new char[strlen(pDir) + strlen(DIR_MARKER_STR) + strlen(pF) + 2];
    else
        name = new char[strlen(pF)]; 

    if (pDir)
    {
       strcpy(name, pDir);
       if (name[strlen(name) - 1] != DIR_MARKER)
           strcat(name, DIR_MARKER_STR);
       strcat(name, pF);
    }
    else
       strcpy(name, pF);

    if (lstat(name, &st) == -1) {
        delete name;
        return;
    }
    if (S_ISLNK(st.st_mode)) 
        wfd->dwFileAttributes = FILE_ATTRIBUTE_SYMLINK;
    else if (S_ISDIR(st.st_mode))
        wfd->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    else
        wfd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

    delete name;
}


bool FindNextFile(HANDLE hFindHandle, WIN32_FIND_DATA *lpFindFileData) {
//    cout << "FindNext: begin" << endl;
    if (!hFindHandle || !lpFindFileData) return false;
    FindData *pFD = (FindData *)hFindHandle;
    if (!pFD->pDir) {
	// no dir, that means only one match, which we already did
	return false;
    }
    struct dirent *pdirent = readdir(pFD->pDir);
    while (pdirent) {
	if (Match(pFD->rest,pdirent->d_name)) {
	    FillWin32FindData(pFD->dir, pdirent->d_name,lpFindFileData);
	    return true;
	}
	pdirent = readdir(pFD->pDir);
    }
    // no matches, or dir empty...
    return false;
}


bool FindClose(HANDLE hFindFile) {
    if (!hFindFile) return false;
    FindData *pFD = (FindData *)hFindFile;
    if (pFD->pDir)
        closedir(pFD->pDir);
    if (pFD->dir)
        delete pFD->dir;
    if (pFD->rest)
        delete pFD->rest;
    delete pFD;

    return true;
}



HINSTANCE LoadLibrary(char *lpLibFileName) {
    HINSTANCE hInst = dlopen(lpLibFileName, RTLD_NOW | RTLD_GLOBAL);
    if (!hInst) 
    {
        int i;
        char *ptr;

        ptr = strrchr(lpLibFileName, '/');
        if (ptr == NULL)
           ptr = lpLibFileName;
        else
           ptr++;

        for(i = 0; SpecialLibraries[i][0]; i++)
        {
            if (strncmp(ptr, SpecialLibraries[i], 
                        strlen(SpecialLibraries[i])) == 0)
            {
               printf("%s\n\nThis is *not* a fatal error. The plugin ", 
                      dlerror());
               printf("'%s' in "the_BRANDING"'s plugin \ndirectory could not ", ptr);
               printf("be loaded. "The_BRANDING" will use the default PMO.\n");
               printf("To get rid of this warning either fix the problem, ");
               printf("or delete the\n'%s' file from "the_BRANDING"'s plugin ", ptr);
               printf("directory.\n");
               return hInst;
            }
        }
        cout << dlerror() << endl;
    }
    return hInst;
}



bool FreeLibrary(HMODULE hLibModule) {
    return dlclose(hLibModule) ? true : true;
}


FARPROC GetProcAddress(HMODULE hModule, const char *lpProcName) {
    FARPROC *rtn = (FARPROC *) dlsym(hModule,lpProcName);
    if (!rtn) {
	cout << "GetProcAddress: dlsym error: " << dlerror() << endl;
    }
    return rtn;
}

