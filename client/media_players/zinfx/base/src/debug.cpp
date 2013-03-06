#include <stdio.h>
#include "debug.h"

#ifdef WIN32

#include <windows.h>
/*-------------------------------------------------------------------------------*\
  Globals:
\*-------------------------------------------------------------------------------*/

static HWND hDebugWind;

/*-------------------------------------------------------------------------------*\
  Typedefs, structs:
\*-------------------------------------------------------------------------------*/ 

/*-------------------------------------------------------------------------------*\
  Code:
\*-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------*\
  Function: 
     Notes:
\*-------------------------------------------------------------------------------*/
void DebugInit_v(void)
{
    hDebugWind = FindWindow("#32770", "Debug32");
}

/*-------------------------------------------------------------------------------*\
  Function: 
     Notes:
\*-------------------------------------------------------------------------------*/
void Debug_v(const char * format, ...)
{
    if (hDebugWind == NULL) 
       {
        DebugInit_v();
        if (hDebugWind == NULL) 
           return;
       }

    char szBuffer[4096];
    va_list argptr;
    ATOM hAtom;

    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    szBuffer[254] = 0;
    hAtom = GlobalAddAtom(szBuffer);
    SendMessage( hDebugWind, WM_USER + 1, 0, (LPARAM)hAtom);
    GlobalDeleteAtom(hAtom);
}

#else

#include <stdarg.h>

void DebugInit_v(void)
{
}

void Debug_v(const char * format, ...)
{
    char szBuffer[4096];
    va_list argptr;

    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    szBuffer[254] = 0;
    printf("%s\n", szBuffer);
}

#endif
