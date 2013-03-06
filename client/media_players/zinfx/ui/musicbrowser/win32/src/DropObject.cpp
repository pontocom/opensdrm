/*____________________________________________________________________________

        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1999 EMusic.com

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

        $Id: DropObject.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

// system header files
#include <windows.h>
#include <shlobj.h>

#include <vector>
#include <string>

using namespace std;

// project header files
#include "DropObject.h"
#include "EnumFormatEtc.h"
#include "resource.h"


// IUnknown Methods

STDMETHODIMP DataObject::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if((iid == IID_IUnknown) ||(iid == IID_IDataObject)) 
    {
        *ppv = this;
        ++m_refs;
        return NOERROR;
    }

    *ppv = NULL;

    return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) DataObject::AddRef(void) 
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG) DataObject::Release(void) 
{
    if(--m_refs == 0) 
    {
        delete this;
        return 0;
    }
    return m_refs;
}  

// DataObject Constructor
 
DataObject::DataObject(LPCSTR type, vector<string>* urls) 
{
    m_urls = urls;
    m_refs = 1;  
    m_cfe = 1;

    m_fe[0].cfFormat = RegisterClipboardFormat(type);
    m_fe[0].ptd = NULL;
    m_fe[0].dwAspect = DVASPECT_CONTENT;
    m_fe[0].lindex = -1;
    m_fe[0].tymed = TYMED_HGLOBAL;
}

DataObject::~DataObject() 
{
    delete m_urls;   
}

// private function
char* DataObject::CreateDropFiles(DWORD* size)
{
    char* result = NULL;

    if(m_urls)
    {
        *size = sizeof(DROPFILES);
        // this is the NULL after each url and the final double NULL
        *size += m_urls->size() + 1; 

        vector<string>::iterator i;

        // iterate the urls and add their lengths
        for(i = m_urls->begin(); i != m_urls->end(); i++)
        {
            *size += i->size();
        }

        result = new char[*size];

        // skip past DROPFILES struct
        char* cp = result + sizeof(DROPFILES);

        // iterate the urls and add them to the buffer
        // NULL terminating each one
        for(i = m_urls->begin(); i != m_urls->end(); i++)
        {
            strcpy(cp, i->c_str()); // copy url
            cp += i->size() + 1; // skip past NULL
        }

        // add final extra NULL
        *cp = 0x00;

        // fill out DROPFILES struct
        DROPFILES* df = (DROPFILES*) result;

        df->pFiles = sizeof(DROPFILES);
        df->pt.x = 0;
        df->pt.y = 0;
        df->fNC = FALSE;
        df->fWide = FALSE;
    }

    if(!result)
        *size = 0;


    return result;
}


//	________________________________________
//
//	IDataObject Methods

//	________________________________________
//
//	DataObject::GetData
// 
//	Purpose:
//		Retrieves data described by a specific FormatEtc into a StgMedium
//		allocated by this function.  Used like GetClipboardData.
//
//	Parameters:
//		pFE             LPFORMATETC describing the desired data.
//		pSTM            LPSTGMEDIUM in which to return the data.
//
//	Return Value:
//		HRESULT         NOERROR or a general error value.
//

STDMETHODIMP DataObject::GetData(LPFORMATETC pFE, 
		                         LPSTGMEDIUM pSTM)  
{  
    HGLOBAL hMem;
    UINT uFormat;
    CHAR* p;

    m_format = 0;
    uFormat = pFE->cfFormat;

	// CFSTR_FREEAMP_MUSICBROWSERITEM clipboard format
    if(uFormat == m_fe[0].cfFormat) 
    {
        if(pFE->lindex != -1) 
            return DV_E_LINDEX;
        if(!(pFE->dwAspect & DVASPECT_CONTENT))
            return DV_E_DVASPECT;
        if(!(pFE->tymed & TYMED_HGLOBAL))
            return DV_E_FORMATETC;

        DWORD size;
        char* data;

        data = CreateDropFiles(&size);

        hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, size);

        if(hMem == NULL) 
            return STG_E_MEDIUMFULL;

        p = (LPSTR) GlobalLock(hMem);
        memcpy(p, data, size);
        GlobalUnlock(hMem);

        delete [] data;

        pSTM->hGlobal = hMem;
        pSTM->tymed = TYMED_HGLOBAL;
        pSTM->pUnkForRelease = NULL;
        m_format = uFormat;

        return S_OK;	
	}

    return DV_E_FORMATETC;
}


//	________________________________________
//
//	DataObject::GetDataHere
//
//	Purpose:
//		Renders the specific FormatEtc into caller-allocated medium
//		provided in pSTM.
//
//	Parameters:
//		pFE             LPFORMATETC describing the desired data.
//		pSTM            LPSTGMEDIUM providing the medium into which
//						we render the data.
//
//	Return Value:
//		HRESULT         NOERROR or a general error value.
//

STDMETHODIMP DataObject::GetDataHere(LPFORMATETC pFE, 
		                             LPSTGMEDIUM pSTM) 
{
    //Check the aspects we support.
    if(!(pFE->dwAspect & DVASPECT_CONTENT))
        return DV_E_DVASPECT;

    if(pFE->lindex != -1) 
        return DV_E_LINDEX;

    return DV_E_FORMATETC;
}

//	________________________________________
//
//	DataObject::QueryGetData
//
//	Purpose:
//      Tests if a call to GetData with this FormatEtc will provide
//      any rendering; used like IsClipboardFormatAvailable.
//
//	Parameters:
//      pFE             LPFORMATETC describing the desired data.
//
//	Return Value:
//      HRESULT         NOERROR or a general error value.
//

STDMETHODIMP DataObject::QueryGetData(LPFORMATETC pFE) 
{
    UINT uFormat = pFE->cfFormat;
    BOOL bRet = FALSE;

    //Check the aspects we support.
    if(!(pFE->dwAspect & DVASPECT_CONTENT))
        return DV_E_DVASPECT;

    if(uFormat == m_fe[0].cfFormat) 		// private FREEAMP format
    {
        if(pFE->lindex != -1) 
            return DV_E_LINDEX;
        bRet =(BOOL)(pFE->tymed & TYMED_HGLOBAL);
    }
    else 
        bRet = FALSE;

    return(bRet ? S_OK : DV_E_FORMATETC);
}

//	________________________________________
//
//	DataObject::EnumFormatEtc
//
//	Purpose:
//		Returns an IEnumFORMATETC object through which the caller can
//		iterate to learn about all the data formats this object can
//		provide through either GetData[Here] or SetData.
//
//	Parameters:
//		dwDir           DWORD describing a data direction, either
//						DATADIR_SET or DATADIR_GET.
//		ppEnum          LPENUMFORMATETC * in which to return the
//						pointer to the enumerator.
//
//	Return Value:
//		HRESULT         NOERROR or a general error value.
//

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDir, 
                                       LPENUMFORMATETC* ppEnum) 
{
    switch(dwDir) 
    {
        case DATADIR_GET:
            *ppEnum = new CEnumFormatEtc(m_cfe, &m_fe[0]);
            break;

        case DATADIR_SET:
            *ppEnum = NULL;
            break;

        default:
            *ppEnum = NULL;
            break;
    }

    if(*ppEnum == NULL) 
        return E_NOTIMPL;

    return S_OK;
}

// Unimplemented methods 

STDMETHODIMP DataObject::DAdvise(LPFORMATETC pFE, 
                                 DWORD dwFlags, 
                                 LPADVISESINK 	pIAdviseSink, 
                                 LPDWORD pdwConn) 
{
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::DUnadvise(DWORD dwConn) 
{
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::EnumDAdvise(LPENUMSTATDATA *ppEnum) 
{
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::SetData(LPFORMATETC pFE, 
                                 LPSTGMEDIUM pSTM, 
                                 BOOL fRelease) 
{
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(LPFORMATETC pFEIn, 
                                               LPFORMATETC pFEOut) 
{
    pFEOut->ptd = NULL;
    return E_NOTIMPL;
}