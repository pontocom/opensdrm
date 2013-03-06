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

        $Id: EditTrackInfoDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

// system includes
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

// project includes
#include "config.h"
#include "utility.h"
#include "resource.h"
#include "EditTrackInfoDialog.h"
#include "help.h"
#include "eventdata.h"

EditTrackInfoDialog::EditTrackInfoDialog(FAContext* context,
                                         HINSTANCE hinst,
                                         HWND hwnd, 
                                         const vector<ArtistList*>* artistList,
                                         MetaData* editMetaData,
                                         const char* location):
    m_hwnd(hwnd), m_artistList(artistList), 
    m_editMetaData(editMetaData), m_context(context), 
    m_location(location), m_hinst(hinst)
{

}

EditTrackInfoDialog::~EditTrackInfoDialog()
{

}

static
BOOL CALLBACK EditTrackInfoDlgProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    EditTrackInfoDialog* _this = 
        (EditTrackInfoDialog*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            _this = (EditTrackInfoDialog*)lParam;
            assert(_this != NULL);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)_this);
            break;
        }            
    }

    return _this->DialogProc(hwnd, msg, wParam, lParam);
}    

bool EditTrackInfoDialog::Show()   
{
    bool result = false;

    result = (0 < DialogBoxParam(
                          m_hinst, 
                          MAKEINTRESOURCE(IDD_EDITINFO),
                          m_hwnd, 
                          ::EditTrackInfoDlgProc, 
                          (LPARAM )this));
    return result;
}

BOOL EditTrackInfoDialog::DialogProc(HWND hwnd, 
                                     UINT msg, 
                                     WPARAM wParam, 
                                     LPARAM lParam )
{
    BOOL result = FALSE;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            set<string> artists;
            set<string> albums;
            set<string> genres;
            char number[64];
            
            CreateEditInfoLists(artists, albums, genres);
  
            HWND hwndArtist = GetDlgItem(hwnd, IDC_ARTIST);
            HWND hwndAlbum = GetDlgItem(hwnd, IDC_ALBUM);
            HWND hwndGenre = GetDlgItem(hwnd, IDC_GENRE);
            HWND hwndTitle = GetDlgItem(hwnd, IDC_TITLE);
            HWND hwndTrack = GetDlgItem(hwnd, IDC_TRACK);
            HWND hwndSpinTrack = GetDlgItem(hwnd, IDC_SPINTRACK);
            HWND hwndYear = GetDlgItem(hwnd, IDC_YEAR);
            HWND hwndSpinYear = GetDlgItem(hwnd, IDC_SPINYEAR);
            HWND hwndComment = GetDlgItem(hwnd, IDC_COMMENT);
            HWND hwndLocation = GetDlgItem(hwnd, IDC_LOCATION);
            HWND hwndTitleText =  FindWindowEx(hwnd, NULL, NULL, "Title:");
            HWND hwndTrackText =  FindWindowEx(hwnd, NULL, NULL, "Track #:");
            HWND hwndPlayCount = GetDlgItem(hwnd, IDC_PLAYCOUNT);           
            HWND hwndMusicBrainz = GetDlgItem(hwnd, IDC_BUTTON_MB);
            HWND hwndBitzi = GetDlgItem(hwnd, IDC_BITZILOOKUP);

            SYSTEMTIME sysTime;

            GetSystemTime(&sysTime);

            SendMessage(hwndSpinTrack, UDM_SETRANGE, 0, MAKELPARAM(1000, 0));
            SendMessage(hwndSpinYear, UDM_SETRANGE, 0, MAKELPARAM(sysTime.wYear, 0));

            set<string>::iterator i;

            // track name
            Edit_SetText(hwndTitle, m_editMetaData->Title().c_str());

            // track number
            if(m_editMetaData->Track() == -1)
            {
                Edit_SetText(hwndTrack, "<Multiple>");
                EnableWindow(hwndTrack, FALSE);
                EnableWindow(hwndTrackText, FALSE);

                EnableWindow(hwndTitle, FALSE);
                EnableWindow(hwndTitleText, FALSE);
				Button_Enable(hwndMusicBrainz, FALSE);
				Button_Enable(hwndMusicBrainz, FALSE);
				Button_Enable(hwndBitzi, FALSE);
            }
            else
            {
                sprintf(number, "%d", m_editMetaData->Track());
                Edit_SetText(hwndTrack, number);
            }

            // track year
            if(m_editMetaData->Year() == -1)
            {
                Edit_SetText(hwndYear, "<Multiple>");
            }
            else
            {
                sprintf(number, "%d", m_editMetaData->Year());
                Edit_SetText(hwndYear, number);
            }

            // playcount
            if(m_editMetaData->PlayCount() == -1)
            {
                Edit_SetText(hwndPlayCount, "<Multiple>");
            }
            else
            {
                sprintf(number, "%d", m_editMetaData->PlayCount());
                Edit_SetText(hwndPlayCount, number);
            }

            // track comment
            Edit_SetText(hwndComment, m_editMetaData->Comment().c_str());

            // location
            Edit_SetText(hwndLocation, m_location);


            // add artists
            for(i = artists.begin(); i != artists.end(); i++)
            {
                uint32 length = (*i).size();

                if(length)
                {
                    bool notJustWhiteSpace = false;

                    for(uint32 index = 0; index < length; index++)
                    {
                        if(!isspace((*i)[index]))
                        {
                            notJustWhiteSpace = true;
                            break;
                        }
                    }

                    if(notJustWhiteSpace)
                        ComboBox_AddString(hwndArtist, (*i).c_str());
                }
            }

            ComboBox_SetText(hwndArtist, m_editMetaData->Artist().c_str());

            // add albums
            for(i = albums.begin(); i != albums.end(); i++)
            {
                uint32 length = (*i).size();

                if(length)
                {
                    bool notJustWhiteSpace = false;

                    for(uint32 index = 0; index < length; index++)
                    {
                        if(!isspace((*i)[index]))
                        {
                            notJustWhiteSpace = true;
                            break;
                        }
                    }

                    if(notJustWhiteSpace)
                        ComboBox_AddString(hwndAlbum, (*i).c_str());
                }
            }

            ComboBox_SetText(hwndAlbum, m_editMetaData->Album().c_str());

            // add genres
            for(i = genres.begin(); i != genres.end(); i++)
            {
                uint32 length = (*i).size();

                if(length)
                {
                    bool notJustWhiteSpace = false;

                    for(uint32 index = 0; index < length; index++)
                    {
                        if(!isspace((*i)[index]))
                        {
                            notJustWhiteSpace = true;
                            break;
                        }
                    }

                    if(notJustWhiteSpace)
                        ComboBox_AddString(hwndGenre, (*i).c_str());
                }
            }

            ComboBox_SetText(hwndGenre, m_editMetaData->Genre().c_str());
  
            break;
        }      

        case WM_HELP:
        {
            ShowHelp(m_context, Edit_Info);
            result = TRUE;
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDHELP:
                {
                    ShowHelp(m_context, Edit_Info);
                    break;
                }

                case IDCANCEL:
                    EndDialog(hwnd, FALSE);
                    break;

                case IDOK:
                {
                    HWND hwndArtist = GetDlgItem(hwnd, IDC_ARTIST);
                    HWND hwndAlbum = GetDlgItem(hwnd, IDC_ALBUM);
                    HWND hwndGenre = GetDlgItem(hwnd, IDC_GENRE);
                    HWND hwndTitle = GetDlgItem(hwnd, IDC_TITLE);
                    HWND hwndTrack = GetDlgItem(hwnd, IDC_TRACK);
                    HWND hwndYear = GetDlgItem(hwnd, IDC_YEAR);
                    HWND hwndComment = GetDlgItem(hwnd, IDC_COMMENT);
                    HWND hwndPlayCount = GetDlgItem(hwnd, IDC_PLAYCOUNT);
                    uint32 length;
                    char* data;
                    char number[64];

                    length = ComboBox_GetTextLength(hwndArtist) + 1;
                    data = new char[length];
                    ComboBox_GetText(hwndArtist, data, length);
                    m_editMetaData->SetArtist(data);
                    delete [] data;

                    length = ComboBox_GetTextLength(hwndAlbum) + 1;
                    data = new char[length];
                    ComboBox_GetText(hwndAlbum, data, length);
                    m_editMetaData->SetAlbum(data);
                    delete [] data;

                    length = ComboBox_GetTextLength(hwndGenre) + 1;
                    data = new char[length];
                    ComboBox_GetText(hwndGenre, data, length);
                    m_editMetaData->SetGenre(data);
                    delete [] data;

                    length = Edit_GetTextLength(hwndTitle) + 1;
                    data = new char[length];
                    Edit_GetText(hwndTitle, data, length);
                    m_editMetaData->SetTitle(data);
                    delete [] data;

                    length = Edit_GetTextLength(hwndComment) + 1;
                    data = new char[length];
                    Edit_GetText(hwndComment, data, length);
                    m_editMetaData->SetComment(data);
                    delete [] data;

                    Edit_GetText(hwndTrack, number, sizeof(number));

                    if(!strcmp(number, "Unknown"))
                        strcpy(number, "0");
                    else if(!strcmp(number, "<Multiple>"))
                        strcpy(number, "-1");

                    m_editMetaData->SetTrack(atoi(number));

                    Edit_GetText(hwndYear, number, sizeof(number));

                    if(!strcmp(number, "Unknown"))
                        strcpy(number, "0");
                    else if(!strcmp(number, "<Multiple>"))
                        strcpy(number, "-1");

                    m_editMetaData->SetYear(atoi(number));

                    // deal with playcount attribute
                    Edit_GetText(hwndPlayCount, number, sizeof(number));

                    if(!strcmp(number, "Unknown"))
                        strcpy(number, "0");
                    else if(!strcmp(number, "<Multiple>"))
                        strcpy(number, "-1");

                    m_editMetaData->SetPlayCount(atoi(number));
                    
                    EndDialog(hwnd, TRUE);
                    break;
                }

                case IDC_TRACK:
                case IDC_YEAR:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char track[6];

                        Edit_GetText( (HWND)lParam, 
                                      track,
                                      sizeof(track));

                        if(!strcmp(track, "0"))
                        {
                            Edit_SetText((HWND)lParam, "Unknown");
                        }
                    }
                    break;
                }                
            }
  
            break;
        }
    }

    return result;
}

void EditTrackInfoDialog::CreateEditInfoLists(set<string>& artists,
                                              set<string>& albums,
                                              set<string>& genres)
{
    vector<ArtistList*>::const_iterator   artist;    

    for(artist = m_artistList->begin(); 
        artist != m_artistList->end(); 
        artist++)
    {
        vector<AlbumList*>::iterator album;

        artists.insert((*artist)->name);

        for(album = (*artist)->m_albumList->begin();
            album != (*artist)->m_albumList->end();
            album++)
        {
            vector<PlaylistItem*>::iterator track;

            albums.insert((*album)->name);

            for(track = (*album)->m_trackList->begin();
                track != (*album)->m_trackList->end();
                track++)
            {
                MetaData metadata = (*track)->GetMetaData();

                genres.insert(metadata.Genre());
            }
        }
    }
}

