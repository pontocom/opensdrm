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

        $Id: infoeditor.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <set>
using namespace std;

#include "eventdata.h"
#include "utility.h"
#include "infoeditor.h"
#include "metadata.h"
#include "musiccatalog.h"
#include "gtkmessagedialog.h"

infoeditorUI::infoeditorUI(FAContext *context, PlaylistManager *plm,
                           vector<PlaylistItem *> *itemlist)
{
    m_context = context;
    m_itemlist = itemlist;
    m_plm = plm;
    title_change = false;
    artist_change = false;
    album_change = false;
    genre_change = false;
    year_change = false;
    comment_change = false;
    track_change = false;
    count_change = false;
    gartistlist = NULL;
    galbumlist = NULL;
    ggenrelist = NULL;
}

infoeditorUI::~infoeditorUI()
{
   gtk_widget_destroy(m_window);
   delete m_itemlist;

   if (gartistlist)
       g_list_free(gartistlist);
   if (galbumlist)
       g_list_free(galbumlist); 
   if (ggenrelist)
       g_list_free(ggenrelist);
}

void infoeditorUI::DoApplyInfoEdit(void)
{
    if (!changed)
        return;

    gtk_widget_set_sensitive(m_okButton, FALSE);
    gtk_widget_set_sensitive(m_applyButton, FALSE);

    vector<PlaylistItem *>::iterator i = m_itemlist->begin();
    for (; i != m_itemlist->end(); i++) {
        MetaData oldmeta, newmeta;
        gchar *text;

        oldmeta = newmeta = (*i)->GetMetaData();

        if (title_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_titleEntry));
            newmeta.SetTitle(text);
        }
        if (artist_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_artistEntry));
            newmeta.SetArtist(text);
        }
        if (album_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_albumEntry));
            newmeta.SetAlbum(text);
        }
        if (year_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_yearEntry));
            newmeta.SetYear(atoi(text));
        }
        if (comment_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_commentEntry));
            newmeta.SetComment(text);
        }
        if (genre_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_genreEntry));
            newmeta.SetGenre(text);
        }
        if (track_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_trackEntry));
            newmeta.SetTrack(atoi(text));
        }
        if (count_change) {
            text = gtk_entry_get_text(GTK_ENTRY(m_countEntry));
            newmeta.SetPlayCount(atoi(text));
        }

        newmeta.SetTime(oldmeta.Time());
        newmeta.SetSize(oldmeta.Size());
        newmeta.SetGUID(oldmeta.GUID().c_str());

        if (newmeta != oldmeta) {
            (*i)->SetMetaData(&newmeta);
            if (m_plm) 
                m_plm->UpdateTrackMetaData(*i, true);
            m_context->catalog->UpdateSong(*i);
        }
    }
}

gint info_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

void info_ok_button_event(GtkWidget *widget, infoeditorUI *p)
{
    p->DoApplyInfoEdit();
    delete p;
}

void info_close_button_event(GtkWidget *widget, infoeditorUI *p)
{
    delete p;
}

void info_apply_button_event(GtkWidget * widget, infoeditorUI *p)
{
    p->DoApplyInfoEdit();
}

void text_changed_event(GtkWidget *widget, infoeditorUI *p)
{
   gtk_widget_set_sensitive(p->m_okButton, TRUE);
   gtk_widget_set_sensitive(p->m_applyButton, TRUE);
   p->changed = true;
   p->CheckWidget(widget);
}

void infoeditorUI::CheckWidget(GtkWidget *widget)
{
   if (widget == m_titleEntry)
       title_change = true;
   else if (widget == m_artistEntry)
       artist_change = true;
   else if (widget == m_albumEntry)
       album_change = true;
   else if (widget == m_yearEntry)
       year_change = true;
   else if (widget == m_genreEntry)
       genre_change = true;
   else if (widget == m_trackEntry)
       track_change = true;
   else if (widget == m_commentEntry)
       comment_change = true;
   else if (widget == m_countEntry)
       count_change = true;
}

void infoeditorUI::DisplayInfo(void)
{
   GtkWidget *vbox;
   GtkWidget *hbox;
   GtkWidget *table;
   GtkWidget *label;
   GtkWidget *separator;
   GtkWidget *close_button;

   if (!m_itemlist || m_itemlist->size() == 0) 
       return;

   BuildLists();

   m_listsize = m_itemlist->size();
   m_artists = true;
   m_albums = true;
   m_years = true;
   m_genres = true;
   m_counts = true;

   MetaData firstdata = (*(m_itemlist->begin()))->GetMetaData();
   vector<PlaylistItem *>::iterator i = m_itemlist->begin();
   i++;
   for (; i != m_itemlist->end(); i++) {
       MetaData compare = (*i)->GetMetaData();
       if (firstdata.Artist() != compare.Artist())
           m_artists = false;
       if (firstdata.Album() != compare.Album())
           m_albums = false;
       if (firstdata.Year() != compare.Year())
           m_years = false;
       if (firstdata.Genre() != compare.Genre())
           m_genres = false;
       if (firstdata.PlayCount() != compare.PlayCount())
           m_counts = false;
   }

   m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title(GTK_WINDOW(m_window), BRANDING" - Edit Track Info");
   gtk_signal_connect(GTK_OBJECT(m_window), "delete_event",
                      GTK_SIGNAL_FUNC(info_delete_event), this);
   gtk_container_set_border_width(GTK_CONTAINER(m_window), 5);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(m_window), vbox);
   gtk_widget_show(vbox);

   label = gtk_label_new("Use the fields below to change the information associated with your selected\ntrack(s).");
   gtk_label_set_line_wrap(GTK_LABEL(label), FALSE);
   gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
   gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
   gtk_widget_show(label);

   table = gtk_table_new(6, 5, FALSE);
   gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 5);
   gtk_widget_show(table);

   /* Song title entry */
   label = gtk_label_new("Title:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL,
                    10, 1);
   gtk_widget_show(label);

   m_titleEntry = gtk_entry_new();
   if (m_listsize > 1) 
      gtk_entry_set_text(GTK_ENTRY(m_titleEntry), "<Multiple Tracks Selected>");
   else if (firstdata.Title().c_str())
      gtk_entry_set_text(GTK_ENTRY(m_titleEntry), firstdata.Title().c_str());
   gtk_signal_connect(GTK_OBJECT(m_titleEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), m_titleEntry, 1, 3, 0, 1);
   gtk_widget_show(m_titleEntry);

   if (m_listsize > 1) {
       gtk_widget_set_sensitive(label, FALSE);
       gtk_widget_set_sensitive(m_titleEntry, FALSE);
   }

   /* Album entry */
   label = gtk_label_new("Album:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   GtkWidget *combo = gtk_combo_new();
   if (galbumlist)
       gtk_combo_set_popdown_strings(GTK_COMBO(combo), galbumlist);
   m_albumEntry = GTK_COMBO(combo)->entry;
   if (!m_albums)
      gtk_entry_set_text(GTK_ENTRY(m_albumEntry), "<Multiple Albums Selected>");
   else if (firstdata.Album().c_str())
      gtk_entry_set_text(GTK_ENTRY(m_albumEntry), firstdata.Album().c_str());
   gtk_signal_connect(GTK_OBJECT(m_albumEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 3, 1, 2);
   gtk_widget_show(combo);

   /* Artist entry */
   label = gtk_label_new("Artist:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   combo = gtk_combo_new();
   if (gartistlist)
       gtk_combo_set_popdown_strings(GTK_COMBO(combo), gartistlist);
   m_artistEntry = GTK_COMBO(combo)->entry;
   if (!m_artists) 
      gtk_entry_set_text(GTK_ENTRY(m_artistEntry), "<Multiple Artists Selected>");
   else if (firstdata.Artist().c_str())
      gtk_entry_set_text(GTK_ENTRY(m_artistEntry), firstdata.Artist().c_str());
   gtk_signal_connect(GTK_OBJECT(m_artistEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 3, 2, 3);
   gtk_widget_show(combo);

   /* Year entry */
   label = gtk_label_new("Year:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 3, 4, 1, 2, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   m_yearEntry = gtk_entry_new();
   if (!m_years) 
       gtk_entry_set_text(GTK_ENTRY(m_yearEntry), "Unknown");
   else if (firstdata.Year() != 0) {
       char tempstr[10];
       sprintf(tempstr, "%d", firstdata.Year());
       gtk_entry_set_text(GTK_ENTRY(m_yearEntry), tempstr);
   }
   gtk_signal_connect(GTK_OBJECT(m_yearEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), m_yearEntry, 4, 5, 1, 2);
   gtk_widget_show(m_yearEntry);

   /* Genre entry */
   label = gtk_label_new("Genre:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   combo = gtk_combo_new();
   if (ggenrelist)
       gtk_combo_set_popdown_strings(GTK_COMBO(combo), ggenrelist);
   m_genreEntry = GTK_COMBO(combo)->entry;
   if (!m_genres) 
      gtk_entry_set_text(GTK_ENTRY(m_genreEntry), "<Multiple Genres Selected>");
   else if (firstdata.Genre().c_str())
      gtk_entry_set_text(GTK_ENTRY(m_genreEntry), firstdata.Genre().c_str());
   gtk_signal_connect(GTK_OBJECT(m_genreEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 3, 3, 4);
   gtk_widget_show(combo);

   /* Track Entry */
   label = gtk_label_new("Track #:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   m_trackEntry = gtk_entry_new();
   if (m_listsize > 1) 
      gtk_entry_set_text(GTK_ENTRY(m_trackEntry), "<Multiple>");
   else if (firstdata.Track() != 0) {
      char strtemp[5];
      sprintf(strtemp, "%d", firstdata.Track());
      gtk_entry_set_text(GTK_ENTRY(m_trackEntry), strtemp);
   }
   gtk_signal_connect(GTK_OBJECT(m_trackEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), m_trackEntry, 4, 5, 0, 1);
   gtk_widget_show(m_trackEntry);

   if (m_listsize > 1) {
       gtk_widget_set_sensitive(m_trackEntry, FALSE);
       gtk_widget_set_sensitive(label, FALSE);
   }

   /* Comment entry */
   label = gtk_label_new("Comments:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   m_commentEntry = gtk_entry_new();
   if (m_listsize > 1)
      gtk_entry_set_text(GTK_ENTRY(m_commentEntry), "<Multiple Tracks Selected>");
   else if (firstdata.Comment().c_str())
      gtk_entry_set_text(GTK_ENTRY(m_commentEntry), firstdata.Comment().c_str());
   gtk_signal_connect(GTK_OBJECT(m_commentEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), m_commentEntry, 1, 5, 4, 5);
   gtk_widget_show(m_commentEntry);

   if (m_listsize > 1) {
       gtk_widget_set_sensitive(m_commentEntry, FALSE);
       gtk_widget_set_sensitive(label, FALSE);
   }

   /* Length display */
   label = gtk_label_new("Length:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 3, 4, 2, 3, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   GtkWidget *time_entry = gtk_entry_new();
   if (m_listsize > 1) {
       gtk_entry_set_text(GTK_ENTRY(time_entry), "<Multiple Tracks Selected>");
   }
   else {
       gchar *length;
       char minutes[5];
       char seconds[5];
       int min, sec;

       min = firstdata.Time() / 60;
       sec = firstdata.Time() % 60;
       sprintf(minutes, "%02d", min);
       sprintf(seconds, "%02d", sec);
       length = g_strconcat((gchar *) minutes, ":", (gchar *) seconds, NULL);
       gtk_entry_set_text(GTK_ENTRY(time_entry), length);
   }
   gtk_entry_set_editable(GTK_ENTRY(time_entry), FALSE);
   gtk_table_attach_defaults(GTK_TABLE(table), time_entry, 4, 5, 2, 3);
   gtk_widget_show(time_entry);
 
   gtk_widget_set_sensitive(time_entry, FALSE);  
 
   /* playcount */
   label = gtk_label_new("Playcount:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 3, 4, 3, 4, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   m_countEntry = gtk_entry_new();
   if (!m_counts)
       gtk_entry_set_text(GTK_ENTRY(m_countEntry), "<Multiple>");
   else if (firstdata.PlayCount() != 0) {
       char tempstr[10];
       sprintf(tempstr, "%d", firstdata.PlayCount());
       gtk_entry_set_text(GTK_ENTRY(m_countEntry), tempstr);
   }
   gtk_signal_connect(GTK_OBJECT(m_countEntry), "changed",
                      GTK_SIGNAL_FUNC(text_changed_event), this);
   gtk_table_attach_defaults(GTK_TABLE(table), m_countEntry, 4, 5, 3, 4);
   gtk_widget_show(m_countEntry);

   if (m_listsize > 1) {
       gtk_widget_set_sensitive(m_countEntry, FALSE);
       gtk_widget_set_sensitive(label, FALSE);
   }

   /* location */
   label = gtk_label_new("Location:");
   gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
   gtk_table_attach(GTK_TABLE(table), label, 0, 1, 5, 6, GTK_FILL, GTK_FILL,
                    10, 5);
   gtk_widget_show(label);

   GtkWidget *location_entry = gtk_entry_new();
   if (m_listsize > 1)
       gtk_entry_set_text(GTK_ENTRY(location_entry), 
                          "<Multiple Tracks Selected>");
   else
       gtk_entry_set_text(GTK_ENTRY(location_entry), 
                          (*(m_itemlist->begin()))->URL().c_str());
   gtk_entry_set_editable(GTK_ENTRY(location_entry), FALSE);
   gtk_table_attach_defaults(GTK_TABLE(table), location_entry, 1, 5, 5, 6);
   gtk_widget_show(location_entry);

   gtk_widget_set_sensitive(location_entry, FALSE);

   /* Control buttons at the bottom */
   separator = gtk_hseparator_new();
   gtk_container_add(GTK_CONTAINER(vbox), separator);
   gtk_widget_show(separator);

   hbox = gtk_hbox_new(FALSE, 10);
   gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
   gtk_container_add(GTK_CONTAINER(vbox), hbox);
   gtk_widget_show(hbox);

   close_button = gtk_button_new_with_label("  Close  ");
   gtk_signal_connect(GTK_OBJECT(close_button), "clicked",
                      GTK_SIGNAL_FUNC(info_close_button_event), this);
   gtk_box_pack_end(GTK_BOX(hbox), close_button, FALSE, FALSE, 2);
   gtk_widget_show(close_button);

   m_applyButton = gtk_button_new_with_label("  Apply  ");
   gtk_signal_connect(GTK_OBJECT(m_applyButton), "clicked",
                      GTK_SIGNAL_FUNC(info_apply_button_event), this);
   gtk_box_pack_end(GTK_BOX(hbox), m_applyButton, FALSE, FALSE, 2);
   gtk_widget_show(m_applyButton);

   m_okButton = gtk_button_new_with_label("  OK  ");
   gtk_signal_connect(GTK_OBJECT(m_okButton), "clicked",
                      GTK_SIGNAL_FUNC(info_ok_button_event), this);
   gtk_box_pack_end(GTK_BOX(hbox), m_okButton, FALSE, FALSE, 2);
   gtk_widget_show(m_okButton);

   gtk_widget_show(m_window);

   gtk_widget_set_sensitive(m_okButton, FALSE);
   gtk_widget_set_sensitive(m_applyButton, FALSE);

   changed = FALSE;
}

void infoeditorUI::BuildLists(void)
{
    set<string> artists;
    set<string> albums;
    set<string> genres;

    const vector<ArtistList *> *artList = m_context->catalog->GetMusicList();
    vector<ArtistList *>::const_iterator artist;

    for (artist = artList->begin(); artist != artList->end(); artist++) 
    {
        vector<AlbumList *>::iterator album;
        artists.insert((*artist)->name);
        for (album = (*artist)->m_albumList->begin();
             album != (*artist)->m_albumList->end(); album++) 
        {
            vector<PlaylistItem *>::iterator track;
            albums.insert((*album)->name);
            for (track = (*album)->m_trackList->begin();
                 track != (*album)->m_trackList->end(); track++)
            {
                MetaData metadata = (*track)->GetMetaData();
                genres.insert(metadata.Genre());
            }
        }
    }

    set<string>::iterator i;

    for (i = artists.begin(); i != artists.end(); i++) 
        gartistlist = g_list_append(gartistlist, (char *)(*i).c_str());

    for (i = albums.begin(); i != albums.end(); i++)
        galbumlist = g_list_append(galbumlist, (char *)(*i).c_str());
   
    for (i = genres.begin(); i != genres.end(); i++)
        ggenrelist = g_list_append(ggenrelist, (char *)(*i).c_str());
}
