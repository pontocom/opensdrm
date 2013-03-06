/*____________________________________________________________________________

    FreeAmp - The Free MP3 Player

    Portions Copyright (C) 1999-2000 EMusic.com

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

    $Id: musicbrowser.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include <musicbrainz/bitzi/bitcollider.h>
#include "musicbrowserui.h"
#include "gtkmusicbrowser.h" 
#include "gtkmessagedialog.h" 
#include "missingfileui.h"
#include "infoeditor.h"
#include "eventdata.h"
#include "player.h"
#include "pmo.h"

#include <algorithm>
using namespace std;

extern "C" {

UserInterface *Initialize(FAContext *context) {
    return new MusicBrowserUI(context);
}
           }

MusicBrowserUI::MusicBrowserUI(FAContext *context)
{
    m_context = context;
    searching = NULL;
    wiz = NULL;
}

MusicBrowserUI::~MusicBrowserUI()
{
}

Error MusicBrowserUI::Init(int32 startup_level) 
{
    if ((m_startupType = startup_level) != SECONDARY_UI_STARTUP) {
        cout << "The musicbrowser cannot run as the primary ui\n"; 
        return kError_InitFailedSafely;
    }

    if (!getenv("DISPLAY"))
        return kError_InitFailedSafely;

    m_playerEQ = m_context->target;

    char *fadir = FreeampDir(NULL);
    string URL = string("file://") + string(fadir) +
                 string("/currentlist.m3u");
    delete [] fadir;
    mainBrowser = new GTKMusicBrowser(m_context, this, URL);

    return kError_NoErr;
}

Error MusicBrowserUI::AcceptEvent(Event *event)
{
    switch (event->Type()) {
        case CMD_Cleanup: {
            mainBrowser->Close(false);
            delete mainBrowser;

            while (browserWindows.size() > 0) {
                browserWindows[0]->Close(false);
                delete browserWindows[0];
                browserWindows.erase(browserWindows.begin());
            }

            gdk_threads_enter();
            if (searching)
                searching->Close();
            if (wiz)
                wiz->Close();
            gdk_threads_leave();

            m_playerEQ->AcceptEvent(new Event(INFO_ReadyToDieUI));
            break; }
        case CMD_ToggleMusicBrowserUI: {
            if (mainBrowser->Visible())
                mainBrowser->Close();
            else
                mainBrowser->ShowMusicBrowser();
            break; }
        case CMD_AddFiles:
        case CMD_EditCurrentPlaylistItemInfo:
        case INFO_MusicCatalogRegenerating:
        case INFO_MusicCatalogDoneRegenerating:
        case INFO_PlaylistCurrentItemInfo:
        case INFO_PlaylistItemsUpdated:
        case INFO_PlaylistItemMoved:
        case INFO_CDDiscStatus: 
        case INFO_SearchMusicDone:
        case INFO_BrowserMessage:
        case INFO_PrefsChanged: {
            mainBrowser->AcceptEvent(event);
            vector<GTKMusicBrowser *>::iterator i = browserWindows.begin();
            for (; i != browserWindows.end(); i++)
                    (*i)->AcceptEvent(event);
            break; }
        case INFO_PlaylistItemAdded:
        case INFO_PlaylistItemsAdded:
        case INFO_PlaylistItemRemoved:
        case INFO_PlaylistRepeat:
        case INFO_PlaylistShuffle:
        case INFO_PlaylistSorted:
        case INFO_Paused:
        case INFO_Stopped:
        case INFO_Playing:
        case INFO_MusicCatalogCleared:
        case INFO_MusicCatalogPlaylistAdded:
        case INFO_MusicCatalogPlaylistRemoved:
        case INFO_MusicCatalogStreamAdded:
        case INFO_MusicCatalogStreamRemoved:
        case INFO_MusicCatalogTrackChanged:
        case INFO_MusicCatalogTrackRemoved: 
        case INFO_MusicCatalogTrackAdded: {
            if (mainBrowser->Visible())
                mainBrowser->AcceptEvent(event);
            vector<GTKMusicBrowser *>::iterator i = browserWindows.begin();
            for (; i != browserWindows.end(); i++)
                if ((*i)->Visible())
                    (*i)->AcceptEvent(event);
            break; }
        case INFO_FileNotFound: {
            MissingFileEvent *mfe = (MissingFileEvent *)event;
            
            MissingFileUI *mfui = new MissingFileUI(m_context, mfe->Item());
            mfui->Run();

            break; }
       case INFO_CDNotFound: {
            CDNotFoundEvent *ev = (CDNotFoundEvent *)event;
            GTKMessageDialog *dialog = new GTKMessageDialog();
            string message = "This CD was not found in MusicBrainz. Would "
               "you like to enter the information for this CD and submit "
               "the data for inclusion in the MusicBrainz metadatabase?";

            gdk_threads_enter();
            if (dialog->Show(message.c_str(), "CD Not Found", 
                             kMessageYesNo) == kMessageReturnYes) 
            {
                LaunchBrowser(ev->URL().c_str());
            }
            gdk_threads_leave();
            delete dialog;

            break; }
        case INFO_DatabaseUpgraded: {
            GTKMessageDialog *dialog = new GTKMessageDialog();
            string message = "Due to internal changes, "the_BRANDING" has "
                             "modified the format of the database that stores "
                             "the My Music tree.  Unfortunately, this means "
                             "that you need to Search For Music again to "
                             "rebuild "the_BRANDING"'s internal catalog.";

            gdk_threads_enter();
            dialog->Show(message.c_str(), "Database Upgraded", kMessageOk); 
            gdk_threads_leave();
            delete dialog;

            break; }
        default:
            break;
    }
    return kError_NoErr;
}

void MusicBrowserUI::CreateNewEditor(string &newPlaylist, bool cd_mode)
{
    GTKMusicBrowser *newUI = new GTKMusicBrowser(m_context, this, newPlaylist,
                                                 cd_mode);
    gdk_threads_leave();
    newUI->ShowMusicBrowser();
    gdk_threads_enter();
    browserWindows.push_back(newUI);
}

void MusicBrowserUI::WindowClose(GTKMusicBrowser *oldUI)
{
    vector<GTKMusicBrowser *>::iterator
        loc = find(browserWindows.begin(), browserWindows.end(), oldUI);

    if (loc != browserWindows.end()) {
        delete *loc;
        browserWindows.erase(loc);
    }
}

void MusicBrowserUI::StartSearch(bool runMain, bool intro)
{
    if (wiz || searching)
        return;

    if (intro) {
        wiz = new IntroWizardUI(m_context, this);
	wiz->Show(runMain);
    }
    else {
        searching = new musicsearchUI(m_context, this);
        searching->Show(runMain);
    }    
}

void MusicBrowserUI::SearchClose(void)
{
    if (!searching)
        return;

    delete searching;
    searching = NULL;
}

void MusicBrowserUI::WizardClose(void)
{
    if (!wiz)
        return;

    delete wiz;
    wiz = NULL;
}
