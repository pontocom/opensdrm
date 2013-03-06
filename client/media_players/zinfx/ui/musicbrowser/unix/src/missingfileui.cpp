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

        $Id: missingfileui.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utility.h"
#include "fileselector.h"
#include "missingfileui.h"
#include "missingfile.h"

static const char *szMissingMessage1 =
  "For some reason, "the_BRANDING" cannot find the following file: ";
static const char *szMissingMessage2 =
  "Would you like to: ";
static const char *szBrowseMessage =
  "I moved it.  Here it is:";
static const char *szIgnoreMessage =
  "Remove this track from the playlist";

static gboolean missing_destroy(GtkWidget *w, gpointer p)
{
    return FALSE;
}

static gint missing_kill(GtkWidget *w, GdkEvent *ev, int *ret)
{
    *ret = 4;
    return FALSE;
}
    
MissingFileUI::MissingFileUI(FAContext *context, PlaylistItem *missing)
{
    m_context = context;
    m_missing = missing;
}

MissingFileUI::~MissingFileUI()
{
}

void MissingFileUI::SetFunc(int func)
{
    m_iFunction = func;
}

static void ignore_select(GtkWidget *w, MissingFileUI *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) 
        p->SetFunc(0);
}

static void browse_select(GtkWidget *w, MissingFileUI *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
        p->SetFunc(1);
        p->EnableEntry(true);
    }
}

void MissingFileUI::ChangeTextEntry(const char *text, bool set)
{
    m_entryText = text;
    if (set)
        gtk_entry_set_text(GTK_ENTRY(m_entry), text);
}

static void entry_change(GtkWidget *w, MissingFileUI *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->ChangeTextEntry(text);
}

static void browse_click(GtkWidget *w, MissingFileUI *p)
{
    FileSelector *filesel = new FileSelector(p->GetContext(),"Browse to Missing File");
    if (filesel->Run()) {
        string filepath = filesel->GetReturnPath();
        struct stat st;

        if (stat(filepath.c_str(), &st) == 0)
            p->ChangeTextEntry(filepath.c_str(), true);
    }

    delete filesel;
}

static void ok_click(GtkWidget *w, int *ret)
{
   *ret = 1;
}

static void cancel_click(GtkWidget *w, int *ret)
{
   *ret = 2;
}

void MissingFileUI::EnableEntry(bool enable)
{
    gtk_widget_set_sensitive(m_entry, enable);
    gtk_widget_set_sensitive(m_browse, enable);
}

void MissingFileUI::Run(void)
{
    assert(m_missing);

    gdk_threads_enter();

    int iRet = 0;
    m_iFunction = 0;

    m_entryText = "";
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_signal_connect(GTK_OBJECT(window), "destroy",
                       GTK_SIGNAL_FUNC(missing_destroy), NULL);
    gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                       GTK_SIGNAL_FUNC(missing_kill), &iRet);
    gtk_window_set_title(GTK_WINDOW(window), "Missing File Error");
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    GtkWidget *message = gtk_label_new(szMissingMessage1);
    gtk_label_set_line_wrap(GTK_LABEL(message), FALSE);
    gtk_label_set_justify(GTK_LABEL(message), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), message, FALSE, FALSE, 2); 
    gtk_widget_show(message);

    char *filename = strdup_new(m_missing->URL().c_str());
    uint32 length = m_missing->URL().size();
    URLToFilePath(m_missing->URL().c_str(), filename, &length);
    message = gtk_label_new(filename);
    delete [] filename;
    gtk_label_set_line_wrap(GTK_LABEL(message), TRUE);
    gtk_label_set_justify(GTK_LABEL(message), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), message, FALSE, FALSE, 2);
    gtk_widget_show(message);

    message = gtk_label_new(szMissingMessage2);
    gtk_label_set_line_wrap(GTK_LABEL(message), FALSE);
    gtk_label_set_justify(GTK_LABEL(message), GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment(GTK_MISC(message), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), message, FALSE, FALSE, 2);
    gtk_widget_show(message);

    GtkWidget *button = gtk_radio_button_new_with_label(NULL, szIgnoreMessage);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(ignore_select), this);
    if (m_iFunction == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button);

    button = gtk_radio_button_new_with_label(
                              gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                              szBrowseMessage);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(browse_select), this);
    if (m_iFunction == 1) 
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button); 

    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(vbox), frame);
    gtk_widget_show(frame);

    GtkWidget *ovbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(ovbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), ovbox);
    gtk_widget_show(ovbox);
   
    m_entry = gtk_entry_new();
    gtk_signal_connect(GTK_OBJECT(m_entry), "changed",
                       GTK_SIGNAL_FUNC(entry_change), this);
    gtk_box_pack_start(GTK_BOX(ovbox), m_entry, TRUE, FALSE, 0);
    gtk_widget_show(m_entry);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
    gtk_container_add(GTK_CONTAINER(ovbox), hbox);
    gtk_widget_show(hbox);

    m_browse = gtk_button_new_with_label("Browse");
    gtk_signal_connect(GTK_OBJECT(m_browse), "clicked",
                       GTK_SIGNAL_FUNC(browse_click), this);
    gtk_box_pack_end(GTK_BOX(hbox), m_browse, FALSE, FALSE, 0);
    gtk_widget_show(m_browse);
   
    if (m_iFunction == 1)
        EnableEntry(true);
    else
        EnableEntry(false); 

    GtkWidget *separator = gtk_hseparator_new();
    gtk_container_add(GTK_CONTAINER(vbox), separator);
    gtk_widget_show(separator);    

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);
    gtk_widget_show(hbox);

    button = gtk_button_new_with_label("  Cancel  ");
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(cancel_click), &iRet);
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
                              GTK_SIGNAL_FUNC(gtk_widget_destroy),
                              GTK_OBJECT(window));
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("  OK  ");
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(ok_click), &iRet);
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
                              GTK_SIGNAL_FUNC(gtk_widget_destroy),
                              GTK_OBJECT(window));
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);
    gtk_widget_grab_default(button);
    gtk_widget_show(button);   

    gtk_widget_show(window);

    gdk_threads_leave();

    while (iRet == 0)
        usleep(20);

    if (iRet == 2) {
        m_context->plm->RemoveItem(m_missing);
        return;
    }

    switch (m_iFunction) {
        /* browse */
        case 1: {
            MissingFile *mf = new MissingFile(m_context);

            struct stat st;

            if (stat(m_entryText.c_str(), &st) == 0) 
                mf->AcceptLocation(m_missing, m_entryText);
            else
                /* complain */
               ;
              
            delete mf;
            break; }
        default: {
            m_context->plm->RemoveItem(m_missing);
            break;
        }
    }
}
