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

        $Id: gtkdownloadui.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <strstream>
#include <iostream>

#ifdef __QNX__
#include <strings.h>
#endif

#include "utility.h"
#include "downloadui.h"
#include "help.h"
#include "utility.h"
#include "gtkmessagedialog.h"

static const char *szEMusicText =
   "The Download Manager enables you to download music from the downloadable "
   "page at the EMusic site and other sites that support /"
   "EMusic Package (EMP) downloads.";
static const char *szEMusicURLText = "Go to your EMusic Collection page";
static const char *szEMusicURL = "https://secure.emusic.com/perl/secure/downloadables.pl";

static const char *szFreeAmpText =
   "The Download Manager enables you to download music from sites that "
   "support the EMusic package (EMP) download format. To try it check "
   "out the free music at:";

static const char *szFreeAmpURLText = "http://www.emusic.com/music/free.html";
static const char *szFreeAmpURL = "http://www.emusic.com/music/free.html";

void DownloadUI::ToggleVisEvent(void)
{
    m_initialized = false;
    isVisible = false;
    m_currentindex = 0;
}

void toggle_vis_internal(GtkWidget *widget, DownloadUI *p)
{
    p->ToggleVisEvent();
}

void DownloadUI::CloseWindow(void)
{
    bool close = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_closeComplete));
    m_prefs->SetPrefBoolean(kCloseDLMOnCompletePref, close);
    gtk_widget_destroy(m_downloadUI);
}

void close_internal(GtkWidget *widget, DownloadUI *p)
{
    p->CloseWindow();
}

void DownloadUI::UpdateInfo(void)
{
    if (!isVisible)
        return;

    DownloadItem *dli = downloadList[m_currentindex];

    if (!dli) {
        gtk_label_set_text(GTK_LABEL(artist), "");
        gtk_label_set_text(GTK_LABEL(album), "");
        gtk_label_set_text(GTK_LABEL(title), "");
        gtk_label_set_text(GTK_LABEL(genre), "");
        gtk_label_set_text(GTK_LABEL(playlist), "");
        gtk_label_set_text(GTK_LABEL(name), "");
        gtk_label_set_text(GTK_LABEL(size), "");
        return;
    }

    gtk_label_set_text(GTK_LABEL(artist), dli->GetMetaData().Artist().c_str());
    gtk_label_set_text(GTK_LABEL(album), dli->GetMetaData().Album().c_str());
    gtk_label_set_text(GTK_LABEL(title), dli->GetMetaData().Title().c_str());
    gtk_label_set_text(GTK_LABEL(genre), dli->GetMetaData().Genre().c_str());
    gtk_label_set_text(GTK_LABEL(playlist), dli->PlaylistName().c_str());
    gtk_label_set_text(GTK_LABEL(name), dli->DestinationFile().c_str());
    float total;
    char totsize[64];
    
    total = dli->GetTotalBytes();
    if (total >= 1048576) {
        total /= 1048576;
        sprintf(totsize, "%.2f MB", total);;
    }
    else if(total >= 1024) {
        total /= 1024;
        sprintf(totsize, "%.2f KB", total);
    }
    else
        sprintf(totsize, "%.2f Bytes", total);
 
    string display = totsize;

    gtk_label_set_text(GTK_LABEL(size), display.c_str());
}

string DownloadUI::StatusString(DownloadItem *dli)
{
    string statusString;

    switch (dli->GetState()) {
        case kDownloadItemState_Queued: {
            char outtext[128];
            float total;

            total = dli->GetTotalBytes();

            if (total >= 1048576) {
                total /= 1048576;
                sprintf(outtext, "Queued (%.2f MB)", total);
            }
            else if (total >= 1024) {
                total /= 1024;
                sprintf(outtext, "Queued (%.2f KB)", total);
            }
            else
                sprintf(outtext, "Queued (%.2f Bytes)", total);

            statusString = outtext;
            break; }
        case kDownloadItemState_Downloading: {
            float total;
            float recvd;
            uint32 percent;
            char outtext[128];

            total = dli->GetTotalBytes();
            recvd = dli->GetBytesReceived();
            percent = (uint32)(recvd/total*100);

            if (total >= 1048576) {
                total /= 1048576;
                recvd /= 1048576;
                sprintf(outtext, "%d%% (%.2f of %.2f MB)", percent, recvd, total);
            }
            else if(total >= 1024) {
                total /= 1024;
                recvd /= 1024;
                sprintf(outtext, "%d%% (%.2f of %.2f KB)", percent, recvd, total);
            }
            else
                sprintf(outtext, "%d%% (%.2f of %.2f Bytes)", percent, recvd, total);

            statusString = outtext;
            break; }
        case kDownloadItemState_Cancelled: {
            statusString = "Cancelled";
            break; }
        case kDownloadItemState_Paused: {
            char outtext[128];
            float total;
            float recvd;
            uint32 percent;

            total = dli->GetTotalBytes();
            recvd = dli->GetBytesReceived();
            percent = (uint32)(recvd/total*100);

            if (total >= 1048576) {
                total /= 1048576;
                recvd /= 1048576;
                sprintf(outtext, "Paused (%.2f of %.2f MB - %d%%)", recvd, total, percent);
            }
            else if(total >= 1024) {
                total /= 1024;
                recvd /= 1024;
                sprintf(outtext, "Paused (%.2f of %.2f KB - %d%%)", recvd, total, percent);
            }
            else
                sprintf(outtext, "Paused (%.2f of %.2f Bytes - %d%%)", recvd, total, percent);

            statusString = outtext;
            break; }
        case kDownloadItemState_Error: {
            char outtext[1024];
            sprintf(outtext, "Error: %s\n", ErrorString[dli->GetDownloadError()]);
            statusString = outtext;
            break; }
        case kDownloadItemState_Done: {
            statusString = "Download Complete\0";
            break; }
        default:
            break;
    }
    return statusString;
}

void DownloadUI::UpdateDownloadList(void)
{
    if (!m_List || !isVisible)
        return;

    gtk_clist_freeze(GTK_CLIST(m_List));
    gtk_clist_clear(GTK_CLIST(m_List));

    uint32 iLoop = downloadList.size();

    if (iLoop == 0) {
        gtk_clist_thaw(GTK_CLIST(m_List));
        return;
    }

    for (uint32 i = 0; i < iLoop; i++) {
        DownloadItem *dli = downloadList[i];
        char *iText[2];

        string displayString = dli->GetMetaData().Title();

        iText[0] = (char *)displayString.c_str();  
        iText[1] = (char *)StatusString(dli).c_str();

        int row = gtk_clist_append(GTK_CLIST(m_List), iText);
        gtk_clist_set_row_data(GTK_CLIST(m_List), row, (gpointer)dli);
    }

    gtk_clist_select_row(GTK_CLIST(m_List), m_currentindex, 0);
    SelChangeEvent(m_currentindex);
    gtk_clist_thaw(GTK_CLIST(m_List));
}

void DownloadUI::AddItem(DownloadItem *dli)
{
    if (!dli || !m_List || !isVisible)
        return;

    char *iText[2];
    string displayString = dli->GetMetaData().Title();

    iText[0] = (char *)displayString.c_str();
    iText[1] = (char *)StatusString(dli).c_str();

    int row = gtk_clist_append(GTK_CLIST(m_List), iText);
    gtk_clist_set_row_data(GTK_CLIST(m_List), row, (gpointer)dli);
}

void DownloadUI::UpdateItem(DownloadItem *dli)
{
    if (!dli || !m_List || !isVisible)
        return;

    int row = gtk_clist_find_row_from_data(GTK_CLIST(m_List), (gpointer)dli);

    if (row < 0)
        return;

    char *iText[2];
    iText[0] = (char *)StatusString(dli).c_str();
    if (gtk_clist_get_text(GTK_CLIST(m_List), row, 1, &iText[1])) {
        if (!strcmp(iText[0], iText[1]))
            return;
    }

    gtk_clist_set_text(GTK_CLIST(m_List), row, 1, iText[0]);

    if (row == (int)m_currentindex) 
        SelChangeEvent(m_currentindex);
}

void DownloadUI::RemoveItem(DownloadItem *dli)
{
    if (!dli || !m_List || !isVisible)
        return;

    int row = gtk_clist_find_row_from_data(GTK_CLIST(m_List), (gpointer)dli);

    if (row < 0)
        return;

    gtk_clist_remove(GTK_CLIST(m_List), row);

    vector<DownloadItem *>::iterator i = downloadList.begin();
    for (; i != downloadList.end(); i++) {
        if ((*i) == dli) 
            break;
    }
    if (i != downloadList.end())
        downloadList.erase(i);
}

void set_current_sel_internal(GtkWidget *widget, int row, int column,
                              GdkEventButton *button, DownloadUI *p)
{
   p->SelChangeEvent(row);
}

void DownloadUI::CreateDownloadList(GtkWidget *box)
{
    static const char *titles[] =
    {
      "Song Title", "Status"
    };

    m_List = gtk_clist_new_with_titles(2, (gchar **)titles);
    gtk_container_add(GTK_CONTAINER(box), m_List);
    gtk_clist_set_column_width(GTK_CLIST(m_List), 0, 200);
    gtk_signal_connect(GTK_OBJECT(m_List), "select_row",
                       GTK_SIGNAL_FUNC(set_current_sel_internal), this);
    gtk_widget_show(m_List);
    gtk_clist_column_titles_passive(GTK_CLIST(m_List));
}

void cancel_internal(GtkWidget *w, DownloadUI *p)
{
    p->CancelEvent();
}

void pause_internal(GtkWidget *w, DownloadUI *p)
{
    p->PauseEvent();
}

void resume_internal(GtkWidget *w, DownloadUI *p)
{
    p->ResumeEvent();
}

void DownloadUI::ShowHelp(void)
{
    if (!::ShowHelp(m_context, Download_Manager))
    {
        GTKMessageDialog oBox;
        string oMessage("Cannot find the help files. Please make sure that the help files are properly installed, and you are not running "the_BRANDING" from the build directory.");
        oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk, true);
    }
}

void help_button_click(GtkWidget *w, DownloadUI *p)
{
    p->ShowHelp();
}

void emusic_click(GtkWidget *w, DownloadUI *p)
{
    if (!strncasecmp(BRANDING_COMPANY, "EMusic", 6))
        LaunchBrowser((char *)szEMusicURL);
    else
        LaunchBrowser((char *)szFreeAmpURL);
}

void DownloadUI::CreateDownloadUI(void)
{
    m_downloadUI = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(m_downloadUI), BRANDING" - DownloadManager");
    gtk_signal_connect(GTK_OBJECT(m_downloadUI), "destroy",
                       GTK_SIGNAL_FUNC(toggle_vis_internal), this);
    gtk_container_set_border_width(GTK_CONTAINER(m_downloadUI), 5);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(m_downloadUI), vbox);
    gtk_widget_set_usize(vbox, 400, 400);
    gtk_widget_show(vbox);

    GtkWidget *text = gtk_label_new(NULL);
    gtk_label_set_line_wrap(GTK_LABEL(text), TRUE);
    if (!strncasecmp(BRANDING_COMPANY, "EMusic", 6))
        gtk_label_set_text(GTK_LABEL(text), szEMusicText);
    else
        gtk_label_set_text(GTK_LABEL(text), szFreeAmpText);
    gtk_label_set_justify(GTK_LABEL(text), GTK_JUSTIFY_FILL);
    gtk_misc_set_alignment(GTK_MISC(text), (gfloat)0.0, (gfloat)0.0);
    
    gtk_box_pack_start(GTK_BOX(vbox), text, FALSE, FALSE, 2);
    gtk_widget_set_usize(text, 400, 46);
    gtk_widget_show(text);

    GtkWidget *emusic_button;
    if (!strncasecmp(BRANDING_COMPANY, "EMusic", 6))
        emusic_button = gtk_button_new_with_label(szEMusicURLText);
    else
        emusic_button = gtk_button_new_with_label(szFreeAmpURLText);

    gtk_box_pack_start(GTK_BOX(vbox), emusic_button, FALSE, FALSE, 2);
    gtk_signal_connect(GTK_OBJECT(emusic_button), "clicked",
                       GTK_SIGNAL_FUNC(emusic_click), this);

    gtk_widget_show(emusic_button);
 
    GtkWidget *listwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(listwindow),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), listwindow, TRUE, TRUE, 0);
    gtk_widget_set_usize(listwindow, 400, 140);
    gtk_widget_show(listwindow);

    GtkWidget *table = gtk_table_new(7, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
    gtk_widget_show(table);

    GtkWidget *label = gtk_label_new("Artist:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    artist = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(artist), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), artist, 1, 2, 0, 1);
    gtk_widget_show(artist);

    label = gtk_label_new("Album:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    album = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(album), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), album, 1, 2, 1, 2);
    gtk_widget_show(album);

    label = gtk_label_new("Title:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    title = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(title), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), title, 1, 2, 2, 3);
    gtk_widget_show(title);

    label = gtk_label_new("Genre:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    genre = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(genre), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), genre, 1, 2, 3, 4);
    gtk_widget_show(genre);

    label = gtk_label_new("Playlist:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    playlist = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(playlist), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), playlist, 1, 2, 4, 5);
    gtk_widget_show(playlist);

    label = gtk_label_new("File Name:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 5, 6, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    name = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(name), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), name, 1, 2, 5, 6);
    gtk_widget_show(name);

    label = gtk_label_new("File Size:");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 6, 7, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    size = gtk_label_new(" ");
    gtk_misc_set_alignment(GTK_MISC(size), (gfloat)0.0, (gfloat)0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), size, 1, 2, 6, 7);
    gtk_widget_show(size);

    CreateDownloadList(listwindow);

    GtkWidget *sep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, TRUE, 2);
    gtk_widget_show(sep);

    m_closeComplete = gtk_check_button_new_with_label("Close the Download Manager  when all downloads finish");
    gtk_box_pack_start(GTK_BOX(vbox), m_closeComplete, FALSE, FALSE, 1);
    bool set = false;
    m_prefs->GetPrefBoolean(kCloseDLMOnCompletePref, &set);
    if (set)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_closeComplete), TRUE);
    gtk_widget_show(m_closeComplete);

    sep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, TRUE, 5);
    gtk_widget_show(sep);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    m_CancelButton = gtk_button_new_with_label("  Cancel  ");
    gtk_box_pack_start(GTK_BOX(hbox), m_CancelButton, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(m_CancelButton), "clicked",
                       GTK_SIGNAL_FUNC(cancel_internal), this);
    gtk_widget_show(m_CancelButton);

    m_PauseButton = gtk_button_new_with_label("  Pause  ");
    gtk_box_pack_start(GTK_BOX(hbox), m_PauseButton, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(m_PauseButton), "clicked",
                       GTK_SIGNAL_FUNC(pause_internal), this);
    gtk_widget_show(m_PauseButton);

    m_ResumeButton = gtk_button_new();
    m_ResumeLabel = gtk_label_new("  Resume  ");
    gtk_container_add(GTK_CONTAINER(m_ResumeButton), m_ResumeLabel);
    gtk_box_pack_start(GTK_BOX(hbox), m_ResumeButton, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(m_ResumeButton), "clicked", 
                       GTK_SIGNAL_FUNC(resume_internal), this);
    gtk_widget_show(m_ResumeLabel);
    gtk_widget_show(m_ResumeButton);

    sep = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(hbox), sep, TRUE, FALSE, 5);
    gtk_widget_show(sep);

    m_CloseButton = gtk_button_new_with_label("  Close  ");
    gtk_box_pack_end(GTK_BOX(hbox), m_CloseButton, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(m_CloseButton), "clicked",
                       GTK_SIGNAL_FUNC(close_internal), this);
    gtk_widget_show(m_CloseButton);

    m_helpButton = gtk_button_new_with_label("  Help  ");
    gtk_box_pack_end(GTK_BOX(hbox), m_helpButton, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(m_helpButton), "clicked",
                       GTK_SIGNAL_FUNC(help_button_click), this);
    gtk_widget_show(m_helpButton);

    gtk_widget_show(m_downloadUI);
}
