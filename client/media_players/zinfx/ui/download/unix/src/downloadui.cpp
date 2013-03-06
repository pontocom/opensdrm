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

        $Id: downloadui.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <gtk/gtk.h>
#include "downloadui.h" 
#include "eventdata.h"

extern "C" {

UserInterface *Initialize(FAContext *context) {
    return new DownloadUI(context);
}
           }

DownloadUI::DownloadUI(FAContext *context)
{
    m_context = context;
    m_prefs = context->prefs;
    m_plm = m_context->plm;
    m_target = m_context->target;
    m_propManager = m_context->props;
    m_dlm = m_context->downloadManager;

    m_currentindex = 0;
    m_initialized = false;
    isVisible = false;
}

DownloadUI::~DownloadUI()
{
}


Error DownloadUI::Init(int32 startup_level) 
{
    if (startup_level != SECONDARY_UI_STARTUP) {
        cout << "The Download Manager cannot run as the primary ui\n"; 
        return kError_InitFailedSafely;
    }

    if (!getenv("DISPLAY"))
        return kError_InitFailedSafely;

    return kError_NoErr;
}

Error DownloadUI::AcceptEvent(Event *e)
{
    
    switch (e->Type()) {
        case CMD_Cleanup: {
            for (uint32 i = 0; i < downloadList.size(); i++) {
                DownloadItem *dli = downloadList[i];
                if (dli->GetState() == kDownloadItemState_Downloading) {
                    m_dlm->CancelDownload(dli, true);
                }
            } 
            m_target->AcceptEvent(new Event(INFO_ReadyToDieUI));

            break; }
        case CMD_ToggleDownloadUI: {
            gdk_threads_enter();
            if (m_initialized && isVisible) {
                bool close = gtk_toggle_button_get_active(
                                     GTK_TOGGLE_BUTTON(m_closeComplete));
                m_prefs->SetPrefBoolean(kCloseDLMOnCompletePref, close);
                gtk_widget_hide(m_downloadUI);
                isVisible = false;
            }
            else {
                if (m_initialized)
                    gtk_widget_show(m_downloadUI);
                else {
                    m_initialized = true;
                    CreateDownloadUI();
                }
                isVisible = true;
                UpdateDownloadList();
                UpdateOverallProgress();
            }
            gdk_threads_leave();
            break; }
        case INFO_DownloadItemAdded: {
            DownloadItemAddedEvent *dliae = (DownloadItemAddedEvent *)e;
            downloadList.push_back(dliae->Item());

            if (isVisible && m_initialized) {
                gdk_threads_enter();
                AddItem(dliae->Item());
                UpdateOverallProgress();
                gdk_threads_leave();
            }
            else {
                gdk_threads_enter();
                if (m_initialized)
                    gtk_widget_show(m_downloadUI); 
                else {
                    m_initialized = true;
                    CreateDownloadUI();
                }
                isVisible = true;
                UpdateDownloadList();
                UpdateOverallProgress();
                gdk_threads_leave();
            }

	    if (m_dlm->IsPaused())
	        m_dlm->ResumeDownloads();

            break; }
	case INFO_DownloadItemNewState: {
	    DownloadItemNewStateEvent *dlinse = (DownloadItemNewStateEvent *)e;
	    if (isVisible) {
	        gdk_threads_enter();
		UpdateItem(dlinse->Item());
		UpdateOverallProgress();
		
		uint32 count = 0;
		for (count = 0; count < downloadList.size(); count++) {
		    if (downloadList[count] == dlinse->Item()) {
		        if (dlinse->Item()->GetState() == 
			    kDownloadItemState_Downloading) {
			    gtk_clist_moveto(GTK_CLIST(m_List), count, 0, 0.5, 
			                     -1);
			}
                    }
                }
		gdk_threads_leave();
            }
	    break; }
        case INFO_DownloadItemRemoved: {
            DownloadItemRemovedEvent *dire = (DownloadItemRemovedEvent *)e;
            if (isVisible) {
                gdk_threads_enter();
                RemoveItem(dire->Item());
                UpdateOverallProgress();
                gdk_threads_leave();
            }
            break; }
        case INFO_DownloadItemProgress: {
            DownloadItemProgressEvent *dipe = (DownloadItemProgressEvent *)e;
            if (isVisible) {
                gdk_threads_enter();
                UpdateItem(dipe->Item());
                UpdateOverallProgress();
                gdk_threads_leave();
            }
            break; }
        default:
            break;
    }
    return kError_NoErr;
}

void DownloadUI::UpdateOverallProgress(void)
{

    uint32 itemCount = downloadList.size();
    uint32 totalBytes = 0, doneBytes = 0;
    uint32 totalItems = 0, doneItems = 0;
    DownloadItem *dli = NULL;
 
    if (itemCount) {
        for (uint32 i = 0; i < itemCount; i++) {
             dli = downloadList[i];
             DownloadItemState state = dli->GetState();

             if (state == kDownloadItemState_Queued ||
                 state == kDownloadItemState_Downloading) {
                 totalItems++;
                 totalBytes += dli->GetTotalBytes();
                 doneBytes += dli->GetBytesReceived();
             } 
             else if (state == kDownloadItemState_Done) {
                 doneItems++;
                 totalItems++;
                 totalBytes += dli->GetTotalBytes();
                 doneBytes += dli->GetTotalBytes();
             }
        }
    }
    if (doneItems == totalItems && totalItems != 0) {
        bool set = gtk_toggle_button_get_active(
                                          GTK_TOGGLE_BUTTON(m_closeComplete));
        if (set) {
            m_prefs->SetPrefBoolean(kCloseDLMOnCompletePref, set);
            gtk_widget_hide(m_downloadUI);
            isVisible = false;
        }
    }
}

void DownloadUI::CancelEvent(void)
{
    if (downloadList.size() == 0 || !isVisible)
        return;

    DownloadItem *dli = downloadList[m_currentindex];
    if (dli == *downloadList.end())
        return;

    m_dlm->CancelDownload(dli, false);
    gtk_widget_set_sensitive(m_PauseButton, FALSE);
    gtk_widget_set_sensitive(m_CancelButton, FALSE);
    gtk_widget_set_sensitive(m_ResumeButton, TRUE);
}

void DownloadUI::PauseEvent(void)
{
    if (downloadList.size() == 0 || !isVisible)
        return;

    DownloadItem *dli = downloadList[m_currentindex];
    if (dli == *downloadList.end())
        return;

    m_dlm->CancelDownload(dli, true);
    m_dlm->PauseDownloads();
    gtk_widget_set_sensitive(m_PauseButton, FALSE);
    gtk_widget_set_sensitive(m_CancelButton, TRUE);
    gtk_widget_set_sensitive(m_ResumeButton, TRUE);
}

void DownloadUI::ResumeEvent(void)
{ 
    if (downloadList.size() == 0 || !isVisible)
        return;

    DownloadItem *dli = downloadList[m_currentindex];
    if (dli != *downloadList.end()) {
        if (!m_resumeLabelIsStart) {
            m_dlm->QueueDownload(dli, true);
            m_dlm->ResumeDownloads();
        }
        else
            m_dlm->ResumeDownloads();
        gtk_widget_set_sensitive(m_PauseButton, TRUE);
        gtk_widget_set_sensitive(m_CancelButton, TRUE);
        gtk_widget_set_sensitive(m_ResumeButton, FALSE);
    }
}

bool DownloadUI::UpdateButtons(int row)
{
    DownloadItem *dli = downloadList[row];

    if (dli == *downloadList.end() || !isVisible)
        return false; 

    gtk_label_set_text(GTK_LABEL(m_ResumeLabel), "  Resume  ");
    m_resumeLabelIsStart = false;

    switch (dli->GetState()) {
        case kDownloadItemState_Queued: {
            gtk_widget_set_sensitive(m_PauseButton, FALSE);
            gtk_widget_set_sensitive(m_CancelButton, TRUE);
       
            int active = m_dlm->IsPaused();
            if (!active && downloadList.size() == 1)
                active = TRUE;
            gtk_widget_set_sensitive(m_ResumeButton, active);
            gtk_label_set_text(GTK_LABEL(m_ResumeLabel), "  Start  ");
            m_resumeLabelIsStart = true;
            break; }
        case kDownloadItemState_Downloading: {
            gtk_widget_set_sensitive(m_PauseButton, TRUE);
            gtk_widget_set_sensitive(m_CancelButton, TRUE);
            gtk_widget_set_sensitive(m_ResumeButton, FALSE);
            break; }
        case kDownloadItemState_Cancelled:
        case kDownloadItemState_Error: {
            gtk_widget_set_sensitive(m_PauseButton, FALSE);
            gtk_widget_set_sensitive(m_CancelButton, FALSE);
            gtk_widget_set_sensitive(m_ResumeButton, TRUE);
            break; }
        case kDownloadItemState_Paused: {
            gtk_widget_set_sensitive(m_PauseButton, FALSE);
            gtk_widget_set_sensitive(m_CancelButton, TRUE);
            gtk_widget_set_sensitive(m_ResumeButton, TRUE);
            break; }
        case kDownloadItemState_Done: {
            gtk_widget_set_sensitive(m_PauseButton, FALSE);
            gtk_widget_set_sensitive(m_CancelButton, FALSE);
            gtk_widget_set_sensitive(m_ResumeButton, FALSE);
            break; }
        default:
            break;
    }
    return true;
}
    
void DownloadUI::SelChangeEvent(int row)
{
    m_currentindex = row;
 
    if (UpdateButtons(m_currentindex))
        UpdateInfo();
}
