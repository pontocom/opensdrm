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

   $Id: Parse.cpp,v 1.1 2004/10/13 16:16:09 linha2 Exp $
____________________________________________________________________________*/ 

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <map>
#include <assert.h>
#include "Parse.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

const int iMaxElementLineLength = 2048;
const int iMaxElementNameLength = 255;
const int iMaxAttrLength = 255;
const int iMaxValueLength = 1024;
const int iMaxPCDataLength = 4096;

struct EntityDefs
{
    const char *szEntity;
    const char *szReplacement;
};

EntityDefs pEntities[] =
{
    { "&amp;", "&"},
    { "&lt;",  "<"},   
    { "&gt;",  ">"},   
    { "&apos;",  "'"},   
    { "&quot;",  "\""},   
    { "\0", "\0"}
};

Parse::Parse(void)
{
	m_oLastError = string("");
    m_iErrorLine = 1;
    m_fpFile = NULL;
}

Parse::~Parse(void)
{
}


Error Parse::ParseFile(const string &oFile)
{
    Error eRet;

    m_fpFile = fopen(oFile.c_str(), "r");
    if (m_fpFile == NULL)
    {
       m_oLastError = string("File ") + oFile + string(" not found"); 
       return kError_FileNotFound;
    }   

    eRet = DoParse();

    fclose(m_fpFile);
    m_fpFile = NULL;

    return eRet;
}

Error Parse::ParseString(const string &oXML)
{
    m_uScanOffset = 0;
    m_oXML = oXML;

    return DoParse();
}

int Parse::Scanf(const char *szFormat, char *szData)
{
    char *szCustomFormat;
    int   iRet, iOffset;

    if (m_fpFile)
        return fscanf(m_fpFile, szFormat, szData);

    szCustomFormat = new char[strlen(szFormat) + 3];
    strcpy(szCustomFormat, szFormat);
    strcat(szCustomFormat, "%n");

    iOffset = 0;
    assert(m_uScanOffset <= m_oXML.size());
    iRet = sscanf(m_oXML.c_str() + m_uScanOffset, szCustomFormat, 
                  szData, &iOffset);
    if (iRet > 0 || iOffset > 0)
       m_uScanOffset += iOffset;

    delete [] szCustomFormat;

    return iRet;
}

bool Parse::Eof(void)
{
    if (m_fpFile)
       return feof(m_fpFile) != 0;

    if (m_uScanOffset >= m_oXML.size())
       return true;

    return false;
}

Error Parse::DoParse(void)
{
    char    *szElement, *szElementName, *szAttr, *szValue, *szData;
    char     szDummy[10];
    string   oElementName, oAttr, oValue, oData;
    int      iRet, iOffset, iTemp;
    bool     bError = false, bEmptyTag = false, bFirst = true;
    Error    eRet;
    AttrMap  oAttrMap; 

    szElement = new char[iMaxElementLineLength];
    szElementName = new char[iMaxElementNameLength];
    szAttr = new char[iMaxAttrLength];
    szValue = new char[iMaxValueLength];
    szData = new char[iMaxPCDataLength];

    for(szElement[0] = 0; !bError; bFirst = false)
    {
    	m_iErrorLine += CountNewlines(szElement);
        
        iRet = Scanf(" < %1024[^>] >", szElement);
        if (iRet < 1)
        {
            if (Eof())
               break;

		      iRet = Scanf(" %4095[^<]", szData);
            if (iRet < 1)
		      {
		        m_oLastError = string("Unrecognized characters found");
                bError = true;
                break;
            }
            m_iErrorLine += CountNewlines(szData);
            oData = string(szData);
            UnXMLize(oData);
            eRet = PCData(oData);
            if (IsError(eRet))
            {
                bError = true;
                break;
            }   
            continue; 
        }

        if (bFirst && szElement[0] == '?' && 
                      szElement[strlen(szElement) - 1] == '?')
           continue;

        iRet = Scanf("%[\n\t \r]", szElementName);
        if (iRet > 0)
    	    m_iErrorLine += CountNewlines(szElementName);

        iTemp = 0;
        sscanf(szElement, " /%254[A-Za-z0-9:_]%n", szElementName, &iTemp);
        if (iTemp > 0)
        {
            oElementName = szElementName;
            eRet = EndElement(oElementName);
            if (IsError(eRet))
               bError = true;

            continue;
        }

        if (sscanf(szElement, "%[!-]", szDummy))
        {
            continue;
        }
        
		  iOffset = 0;
        int iRet = sscanf(szElement, " %254[A-Za-z0-9:_] %n", szElementName, &iOffset);
        oElementName = szElementName;
        if (iOffset == 0)
            iOffset = strlen(szElementName);

        oAttrMap.clear();
        bEmptyTag = false;

        for(;;)
        {
#ifdef __QNX__
cout << szElement << endl;
#endif
            iTemp = 0;
            sscanf(szElement + iOffset, " /%n", &iTemp);
            if (iTemp > 0)
            {
               bEmptyTag = true;
               iOffset += iTemp;
            }

            iTemp = 0;
            sscanf(szElement + iOffset, " >%n", &iTemp);
            if (strlen(szElement + iOffset) == 0 || iTemp > 0)
                break;

            iRet = sscanf(szElement + iOffset, 
                          " %254[A-Za-z0-9:] = \"%254[^\"] \"%n", 
                          szAttr, szValue, &iTemp);
            if (iRet < 2 || iTemp == 0)
            {
               iTemp = 0;
               iRet = sscanf(szElement + iOffset, 
                             " %254[A-Za-z0-9:] = \" \" %n", 
                             szAttr, &iTemp);
               if (iRet < 1 || iTemp == 0)
               {
                   m_oLastError = string("Improperly formatted attribute list");
                   bError = true;
                   break;
               }
               szValue[0] = 0;
            }

            iOffset += iTemp;
            oAttr = szAttr;
            oValue = szValue;
            oAttrMap[szAttr] = szValue;
        }

        if (!bError)
        {
            eRet = BeginElement(oElementName, oAttrMap);
            if (bEmptyTag && !IsError(eRet))
            eRet = EndElement(oElementName);

            if (IsError(eRet))
            bError = true;
        }
    }

    delete [] szElement;
    delete [] szElementName,
    delete [] szAttr;
    delete [] szValue;
    delete [] szData;

    if (bError)
    {
        return kError_ParseError;
    }

    return kError_NoErr;
}

void Parse::GetErrorString(string &oError)
{
	char *szError;
    
    szError = new char[m_oLastError.length() + 100];
    sprintf(szError, "%s on line %d.", 
            m_oLastError.c_str(), m_iErrorLine - 1);
    oError = string(szError);
    delete [] szError;
}


int Parse::CountNewlines(char *szElement)
{
	int   iCount;
    char *pPtr;
    
    for(iCount = 0, pPtr = szElement; *pPtr; pPtr++)
       if (*pPtr == '\n')
          iCount++;

    return iCount;
}

int Parse::GetErrorLine(void)
{
    return m_iErrorLine;
}

void Parse::UnXMLize(string &oData)
{
    string::size_type  pos;
    EntityDefs        *pEntity;
    
    for(pEntity = pEntities; *pEntity->szEntity; pEntity++)
    {
        for(;;)
        {
            pos = oData.find(string(pEntity->szEntity), 0);
            if (pos == string::npos)
               break;
        
            oData.replace(pos, strlen(pEntity->szEntity), pEntity->szReplacement);
        }    
    }    
}
