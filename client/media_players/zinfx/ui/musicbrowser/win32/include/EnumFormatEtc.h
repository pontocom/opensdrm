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

        $Id: EnumFormatEtc.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_ENUMFORMATETC_H_
#define INCLUDED_ENUMFORMATETC_H_

#include <ole2.h>

class FAR CEnumFormatEtc : public IEnumFORMATETC {
 public:    
    CEnumFormatEtc(ULONG cFE, LPFORMATETC prgFE);
	~CEnumFormatEtc();

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    /* IEnumFORMATETC methods */
    STDMETHODIMP Next (ULONG, LPFORMATETC, ULONG *);
    STDMETHODIMP Skip (ULONG);
    STDMETHODIMP Reset (void);
    STDMETHODIMP Clone (IEnumFORMATETC **);
 
 private:
    ULONG			m_refs;     
    ULONG           m_iCur;         //Current element.
    ULONG           m_cfe;          //Number of FORMATETCs in us
    LPFORMATETC     m_prgfe;        //Source of FORMATETCs
};  

#endif // INCLUDED_ENUMFORMATETC_H_