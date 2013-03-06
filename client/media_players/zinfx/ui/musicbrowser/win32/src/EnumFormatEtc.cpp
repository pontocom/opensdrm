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

        $Id: EnumFormatEtc.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
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

// project header files
#include "EnumFormatEtc.h"


// IUnknown Methods

STDMETHODIMP CEnumFormatEtc::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if((iid == IID_IUnknown) || (iid == IID_IDataObject)) 
    {
        *ppv = this;
        ++m_refs;
        return NOERROR;
    }

    *ppv = NULL;

    return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CEnumFormatEtc::AddRef(void) 
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG) CEnumFormatEtc::Release(void) 
{
    if(--m_refs == 0) 
    {
        if(m_prgfe != NULL) 
            delete [] m_prgfe;	

        m_prgfe = NULL;		
        
        delete this;
        return 0;
    }
    return m_refs;
}  

// CEnumFormatEtc Constructor
 
CEnumFormatEtc::CEnumFormatEtc(ULONG cFE, LPFORMATETC prgFE)
{
    UINT i;

    m_refs = 1;

    m_iCur = 0;
    m_cfe = cFE;
    m_prgfe = new FORMATETC[(UINT)cFE];

    if(m_prgfe != NULL) 
    {
        for(i = 0; i < cFE; i++)
            m_prgfe[i] = prgFE[i];
    }
}

CEnumFormatEtc::~CEnumFormatEtc() 
{
    if(m_prgfe != NULL)
        delete [] m_prgfe;      
}

//	________________________________________
//
//	ICEnumFormatEtc Methods

//	________________________________________
//
//	CEnumFormatEtc::Next
//
//	Purpose:
//		Returns the next element in the enumeration.
//
//	Parameters:
//		cFE             ULONG number of FORMATETCs to return.
//		pFE             LPFORMATETC in which to store the returned
//					    structures.
//		pulFE           ULONG * in which to return how many we
//						enumerated.
// 
//	Return Value:
//		HRESULT         NOERROR if successful, S_FALSE otherwise,
// 

STDMETHODIMP CEnumFormatEtc::Next(ULONG cFE, 
                                 LPFORMATETC pFE, 
                                 ULONG* pulFE) 
{
    ULONG cReturn=0L;

    if(m_prgfe == NULL)
        return S_FALSE;

    if(pulFE == NULL) 
    {
        if(cFE != 1L) 
            return S_FALSE;
    }
    else 
        *pulFE=0L;

    if((pFE == NULL) ||(m_iCur >= m_cfe))
        return S_FALSE;

    while((m_iCur < m_cfe) &&(cFE > 0)) 
    {
        *pFE++=m_prgfe[m_iCur++];
        cReturn++;
        cFE--;
    }

    if(pulFE != NULL) 
        *pulFE=cReturn;

    return S_OK;
}


//	________________________________________
//
//	CEnumFormatEtc::Skip
//
//	Purpose:
//		Skips the next n elements in the enumeration.
//
//	Parameters:
//		cSkip           ULONG number of elements to skip.
//
//	Return Value:
//		HRESULT         NOERROR if successful, S_FALSE if we could not
//						skip the requested number.
//

STDMETHODIMP CEnumFormatEtc::Skip(ULONG cSkip) 
{
    if(((m_iCur+cSkip) >= m_cfe) ||(m_prgfe == NULL))
        return S_FALSE;

    m_iCur+=cSkip;
    return S_OK;
}


//	________________________________________
//
//	CEnumFormatEtc::Reset
//
//	Purpose:
//		Resets the current element index in the enumeration to zero.
//
//	Parameters:
//		None
//
//	Return Value:
//		HRESULT         NOERROR
//

STDMETHODIMP CEnumFormatEtc::Reset(void) 
{
    m_iCur=0;
    return S_OK;
}


//	________________________________________
//
//	CEnumFormatEtc::Clone
//
//	Purpose:
//		Returns another IEnumFORMATETC with the same state as ourselves.
//
//	Parameters:
//		ppEnum          LPENUMFORMATETC* in which to return the
//						new object.
//
//	Return Value:
//		HRESULT         NOERROR or a general error value.
//

STDMETHODIMP CEnumFormatEtc::Clone(LPENUMFORMATETC *ppEnum) 
{
    CEnumFormatEtc*     pNew;

    *ppEnum=NULL;

    //Create the clone
    pNew = new CEnumFormatEtc(m_cfe, m_prgfe);

    if(pNew == NULL) 
        return E_OUTOFMEMORY;

    pNew->m_iCur = m_iCur;

    *ppEnum = pNew;
    return S_OK;
}


