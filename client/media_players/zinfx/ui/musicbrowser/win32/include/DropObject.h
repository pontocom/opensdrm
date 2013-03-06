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

        $Id: DropObject.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DATAOBJECT_H_
#define INCLUDED_DATAOBJECT_H_

#define CFSTR_FREEAMP_CATALOGITEM ("FREEAMP_CATALOGITEM")
#define CFSTR_FREEAMP_PLAYLISTITEM ("FREEAMP_PLAYLISTITEM")

#include <ole2.h>

#include <vector>
#include <string>

using namespace std;

class FAR DataObject : public IDataObject
{
 public:    
    DataObject(LPCSTR type, vector<string>* urls);
	~DataObject();

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    /* IDataObject methods */
	/* IDataObject methods required for Drag and Drop */
	STDMETHODIMP GetData(LPFORMATETC, LPSTGMEDIUM);
    STDMETHODIMP GetDataHere(LPFORMATETC, LPSTGMEDIUM);
    STDMETHODIMP QueryGetData(LPFORMATETC);
    STDMETHODIMP EnumFormatEtc(DWORD, LPENUMFORMATETC*);

	/*  IDataObject methods not required for Drag and Drop */
    STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC,LPFORMATETC);
    STDMETHODIMP SetData(LPFORMATETC, LPSTGMEDIUM, BOOL);
    STDMETHODIMP DAdvise(LPFORMATETC, DWORD,  LPADVISESINK, DWORD*);
    STDMETHODIMP DUnadvise(DWORD);
    STDMETHODIMP EnumDAdvise(LPENUMSTATDATA*);
 
 private:
    char* CreateDropFiles(DWORD* size);

    vector<string>* m_urls;

    ULONG m_refs;  
	ULONG m_cfe;        // number of FORMATETC structs
	FORMATETC m_fe[4];  // pointer to FORMATETC structs
	UINT m_format;      // format used to actually render object
};  

#endif // INCLUDED_DATAOBJECT_H_