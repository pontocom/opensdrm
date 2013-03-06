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

        $Id: browserlist.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gtkmusicbrowser.h"
#include "utility.h"
#include "player.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <algorithm>
using namespace std;

struct GtkDragDestSite
{
  GtkDestDefaults    flags;
  GtkTargetList     *target_list;
  GdkDragAction      actions;
  GdkWindow         *proxy_window;
  GdkDragProtocol    proxy_protocol;
  gboolean           do_proxy : 1;
  gboolean           proxy_coords : 1;
  gboolean           have_drag : 1;
};

PlaylistManager *BADPLM = NULL;

class comp_pi_pos {
  public:
    bool operator()(PlaylistItem *a, PlaylistItem *b)
    {
        if (!BADPLM)
            return false;
        return (BADPLM->IndexOf(a) < BADPLM->IndexOf(b));
    }
};

void int_destroy(int *oldint)
{
    delete oldint;
}

void GTKMusicBrowser::RenumberPlaylistList(int starting)
{
    if (starting > GTK_CLIST(playlistList)->rows)
        return;

    gtk_clist_freeze(GTK_CLIST(playlistList));

    int totaltime = 0;
    int32 maxCDtime = 0;

    if (m_bCDMode) {
        if (starting >= 1)
            totaltime = *(int *)(gtk_clist_get_row_data(GTK_CLIST(playlistList),
                                                        starting - 1));
        m_context->prefs->GetPrefInt32(kAudioCDLengthPref, &maxCDtime);
    }

    for (int i = starting; i < GTK_CLIST(playlistList)->rows; i++) {
        char location[50];

        sprintf(location, "%d", i + 1);
        gtk_clist_set_text(GTK_CLIST(playlistList), i, 0, location);

        if (m_bCDMode) {
            PlaylistItem *item = m_plm->ItemAt(i);
            totaltime += item->GetMetaData().Time();

            if (totaltime > maxCDtime)
                gtk_clist_set_row_style(GTK_CLIST(playlistList), i, redStyle);
            else
                gtk_clist_set_row_style(GTK_CLIST(playlistList), i,
                                        greenStyle);

            int *totTime = (int *)gtk_clist_get_row_data(
                                                    GTK_CLIST(playlistList), i);
            *totTime = totaltime;
        }
    }
    
    gtk_clist_thaw(GTK_CLIST(playlistList));
}

void GTKMusicBrowser::UpdatePlaylistItems(const vector<PlaylistItem *> *items)
{
    if (!items)
        return;

    vector<PlaylistItem *>::const_iterator i = items->begin();

    uint32 minpos = kInvalidIndex;

    for (; i != items->end(); i++)  {
        uint32 pos = m_plm->IndexOf(*i);
 
        if (pos == kInvalidIndex)
            continue;

        if (pos < minpos)
            minpos = pos;

        MetaData mdata = (*i)->GetMetaData();
        char *iText[9];
        char position[40];
        char *title;
        char *artist;
        char *album;
        char *genre;
        char location[_MAX_PATH];
        uint32 loclength = _MAX_PATH;
        char *comment;
        char year[10];
        char length[50];
        string empty = " ";

        sprintf(position, "%d", pos + 1);
        title = (char *)mdata.Title().c_str();
        artist = (char *)mdata.Artist().c_str();
        album = (char *)mdata.Album().c_str();
        genre = (char *)mdata.Genre().c_str();
        comment = (char *)mdata.Comment().c_str();

        URLToFilePath((*i)->URL().c_str(), location, &loclength);

        if (mdata.Year() == 0)
            sprintf(year, "Unknown");
        else
            sprintf(year, "%d", mdata.Year());

        if (mdata.Time() == 0)
            sprintf(length, "Unknown");
        else {
            int secs = mdata.Time();
            if (secs > 3600)
                sprintf(length, "%d:%02d:%02d", secs / 3600, (secs / 60) % 60,
                        secs % 60);
            else
                sprintf(length, "%d:%02d", (secs / 60) % 60, secs % 60);
        }

        for (int i = 0; i < 9; i++)
        {
            switch (playlistCols[i])
            {
            case kArtistColumn:
                iText[i] = artist;
                break;
            case kAlbumColumn:
                iText[i] = album;
                break;
            case kCommentColumn:
                iText[i] = comment;
                break;
            case kGenreColumn:
                iText[i] = genre;
                break;
            case kLocationColumn:
                iText[i] = location;
                break;
            case kPositionColumn:
                iText[i] = position;
                break;
            case kTitleColumn:
                iText[i] = title;
                break;
            case kTimeColumn:
                iText[i] = length;
                break;
            case kYearColumn:
                iText[i] = year;
                break;
            default:
                iText[i] = (char *)empty.c_str();
                break;
            }
        }

        for (uint32 count = 0; count < 9; count++)
            gtk_clist_set_text(GTK_CLIST(playlistList), pos, count, iText[count]);
    }

    if (m_bCDMode)
        RenumberPlaylistList(minpos);
}

void GTKMusicBrowser::AddPlaylistItems(vector<PlaylistItem *> *items)
{
    if (!items)
        return;

    uint32 minpos = (uint32)-1;

    // hack hack hack hack
    BADPLM = m_plm;
    sort(items->begin(), items->end(), comp_pi_pos());
    BADPLM = NULL;

    gtk_clist_freeze(GTK_CLIST(playlistList));

    vector<PlaylistItem *>::iterator i = items->begin();
    for (; i != items->end(); i++) {
        PlaylistItem *item = *i;

       if (!item)
            continue;

        MetaData mdata = item->GetMetaData();
        char *iText[9];
        char position[40];
        char *title;
        char *artist;
        char *album;
        char *genre;
        char location[_MAX_PATH];
        uint32 loclength = _MAX_PATH;
        char *comment;
        char length[50];
        string empty = " ";

        uint32 pos = m_plm->IndexOf(item);
        sprintf(position, "%d", pos + 1);
        title = (char *)mdata.Title().c_str();
        artist = (char *)mdata.Artist().c_str();
        album = (char *)mdata.Album().c_str();
        genre = (char *)mdata.Genre().c_str();
        comment = (char *)mdata.Comment().c_str();

        URLToFilePath(item->URL().c_str(), location, &loclength);

        if (mdata.Time() == 0)
            sprintf(length, "Unknown");
        else {
            int secs = mdata.Time();
            if (secs > 3600)
                sprintf(length, "%d:%02d:%02d", secs / 3600, (secs / 60) % 60,
                        secs % 60);
            else
                sprintf(length, "%d:%02d", (secs / 60) % 60, secs % 60);
        }

        for (int i = 0; i < 9; i++)
        {
            switch (playlistCols[i])
            {
            case kArtistColumn:
                iText[i] = artist;
                break;
            case kAlbumColumn:
                iText[i] = album;
                break;
            case kCommentColumn:
                iText[i] = comment;
                break;
            case kGenreColumn:
                iText[i] = genre;
                break;
            case kLocationColumn:
                iText[i] = location;
                break;
            case kPositionColumn:
                iText[i] = position;
                break;
            case kTitleColumn:
                iText[i] = title;
                break;
            case kTimeColumn:
                iText[i] = length;
                break;
            default:
                iText[i] = (char *)empty.c_str();
                break;
            }
        }

        gtk_clist_insert(GTK_CLIST(playlistList), pos, iText);

        int *totTime = new int;
        *totTime = 0;

        gtk_clist_set_row_data_full(GTK_CLIST(playlistList), pos, totTime,
                                    (GtkDestroyNotify)int_destroy);

        if (pos < minpos)
            minpos = pos;
    }

    RenumberPlaylistList(minpos);

    gtk_clist_columns_autosize(GTK_CLIST(playlistList));
    gtk_clist_select_row(GTK_CLIST(playlistList), m_lastindex, 0);
    gtk_clist_moveto(GTK_CLIST(playlistList), m_lastindex, 0, 0.5, -1);
    gtk_clist_thaw(GTK_CLIST(playlistList));
}

void GTKMusicBrowser::RemovePlaylistItems(vector<uint32> *indices)
{
    if (!indices)
        return;

    uint32 minpos = (uint32)-1;

    sort(indices->begin(), indices->end(), greater<uint32>());

    gtk_clist_freeze(GTK_CLIST(playlistList));
    vector<uint32>::iterator i = indices->begin();
    for (; i != indices->end(); i++) {
        gtk_clist_remove(GTK_CLIST(playlistList), *i);
        if (*i < minpos)
            minpos = *i;
    }

    RenumberPlaylistList(minpos);

    gtk_clist_columns_autosize(GTK_CLIST(playlistList));
    gtk_clist_select_row(GTK_CLIST(playlistList), minpos, 0);
    gtk_clist_moveto(GTK_CLIST(playlistList), minpos, 0, 0.5, -1);
    gtk_clist_thaw(GTK_CLIST(playlistList));
}

void GTKMusicBrowser::ParsePlaylistCols(void)
{
    unsigned int size = 100;
    char *buffer = (char *)malloc( size );
    if (kError_BufferTooSmall == m_context->prefs->GetPrefString(
                                                    kPlaylistHeaderColumnsPref,
                                                    buffer, &size))
    {
        int bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_context->prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size);
    }

    int column = 1;
    char *token = strtok(buffer, "|");
    while (token != NULL && column < 9)
    {
        PlaylistColumns newcol = kEmptyColumn;
 
        if (!strcmp(token, "Artist"))
            newcol = kArtistColumn;
        else if (!strcmp(token, "Album"))
            newcol = kAlbumColumn;
        else if (!strcmp(token, "Comment"))
            newcol = kCommentColumn;
        else if (!strcmp(token, "Genre"))
            newcol = kGenreColumn;
        else if (!strcmp(token, "Location"))
            newcol = kLocationColumn;
        else if (!strcmp(token, "Title"))
            newcol = kTitleColumn;
        else if (!strcmp(token, "Time"))
            newcol = kTimeColumn;
        else if (!strcmp(token, "Year"))
            newcol = kYearColumn;
        else
            newcol = kEmptyColumn;

        if (playlistCols[column] != newcol)
            playlistColsChanged = true;

        playlistCols[column] = newcol;
        token = strtok(NULL, "|");
        column++;
    }
    for (; column < 9; column++) 
        playlistCols[column] = kEmptyColumn;
}

void GTKMusicBrowser::UpdateColumnHeaders(void)
{
    gtk_clist_column_titles_show(GTK_CLIST(playlistList));

    for (int column = 0; column < 9; column++) 
    {
        string title;
        bool visible = true;

        switch (playlistCols[column]) 
        {
            case kArtistColumn:
                title = "Artist";
                break;
            case kAlbumColumn:
                title = "Album";
                break;
            case kCommentColumn:
                title = "Comment";
                break;
            case kGenreColumn:
                title = "Genre";
                break;
            case kLocationColumn:
                title = "Location";
                break;
            case kPositionColumn:
                title = "#";
                break;
            case kTitleColumn:
                title = "Title";
                break;
            case kTimeColumn:
                title = "Length"; 
                break;
            case kYearColumn:
                title = "Year";
                break;
            default:
                title = " ";
                visible = false;
                break;
        }
        gtk_clist_set_column_title(GTK_CLIST(playlistList), column, 
                                   title.c_str());
        gtk_clist_set_column_visibility(GTK_CLIST(playlistList), column, 
                                        visible);
    }
} 

void GTKMusicBrowser::UpdatePlaylistList(void)
{
    if (!playlistList || !m_plm)
        return;

    ParsePlaylistCols();

    gtk_clist_freeze(GTK_CLIST(playlistList));
    gtk_clist_clear(GTK_CLIST(playlistList));

    if (playlistColsChanged)
        UpdateColumnHeaders();

    uint32 iLoop = m_plm->CountItems();

    int32 totaltime = 0;
    int32 maxCDtime = 0;
    m_context->prefs->GetPrefInt32(kAudioCDLengthPref, &maxCDtime);

    int row = 0;

    for (uint32 i = 0; i < iLoop; i++) {
        PlaylistItem *item = m_plm->ItemAt(i);

        if (!item)
            continue;

        MetaData mdata = item->GetMetaData();
        char *iText[9];
        char position[40];
        char *title;
        char *artist;
        char *album;
        char *genre;
        char location[_MAX_PATH];
        uint32 loclength = _MAX_PATH;
        char *comment;
        char length[50];
        char year[10];
        string empty = " ";

        sprintf(position, "%d", i + 1);
        title = (char *)mdata.Title().c_str();
        artist = (char *)mdata.Artist().c_str();
        album = (char *)mdata.Album().c_str();
        genre = (char *)mdata.Genre().c_str();
        comment = (char *)mdata.Comment().c_str();
        
        URLToFilePath(item->URL().c_str(), location, &loclength);

        if (mdata.Year() == 0)
            sprintf(year, "Unknown");
        else
            sprintf(year, "%d", mdata.Year());

        if (mdata.Time() == 0)
            sprintf(length, "Unknown");
        else {
            int secs = mdata.Time();
            if (secs > 3600)
                sprintf(length, "%d:%02d:%02d", secs / 3600, (secs / 60) % 60,
                        secs % 60);
            else
                sprintf(length, "%d:%02d", (secs / 60) % 60, secs % 60);
        }

        for (int i = 0; i < 9; i++)
        {
            switch (playlistCols[i]) 
            {
            case kArtistColumn:
                iText[i] = artist;
                break;
            case kAlbumColumn:
                iText[i] = album;
                break;
            case kCommentColumn:
                iText[i] = comment;
                break;
            case kGenreColumn:
                iText[i] = genre;
                break;
            case kLocationColumn:
                iText[i] = location;
                break;
            case kPositionColumn:
                iText[i] = position;
                break;
            case kTitleColumn:
                iText[i] = title;
                break;
            case kTimeColumn:
                iText[i] = length;
                break;
            case kYearColumn:
                iText[i] = year;
                break;
            default:
                iText[i] = (char *)empty.c_str();
                break;
            }
        }

        totaltime = totaltime + mdata.Time();

        row = gtk_clist_append(GTK_CLIST(playlistList), iText);

        if (m_bCDMode) {
            if (totaltime > (int)maxCDtime)
                gtk_clist_set_row_style(GTK_CLIST(playlistList), row, redStyle);
            else
                gtk_clist_set_row_style(GTK_CLIST(playlistList), row, 
                                        greenStyle);
        }
        
        int *totTime = new int;
        *totTime = totaltime;

        gtk_clist_set_row_data_full(GTK_CLIST(playlistList), row, totTime,
                                    (GtkDestroyNotify)int_destroy);
    }

    if (iLoop == 0) {
        gtk_clist_set_column_width(GTK_CLIST(playlistList), 0, 10);
        gtk_clist_set_column_width(GTK_CLIST(playlistList), 1, 45);
        gtk_clist_set_column_width(GTK_CLIST(playlistList), 2, 45);
    }
    else
        gtk_clist_columns_autosize(GTK_CLIST(playlistList));

    gtk_clist_select_row(GTK_CLIST(playlistList), m_lastindex, 0);
    gtk_clist_moveto(GTK_CLIST(playlistList), m_lastindex, 0, 0.5, -1);
    ChangeCurrentPlayingIndex(m_playingindex, m_playingindex);
    gtk_clist_thaw(GTK_CLIST(playlistList));
}

void GTKMusicBrowser::ChangeCurrentPlayingIndex(uint32 oldindex,
                                                uint32 newindex)
{
    if (oldindex != kInvalidIndex)
        gtk_clist_set_row_style(GTK_CLIST(playlistList), oldindex, normStyle);
    gtk_clist_set_row_style(GTK_CLIST(playlistList), newindex, boldStyle);
    gtk_clist_columns_autosize(GTK_CLIST(playlistList));
}

void list_keypress(GtkWidget *w, GdkEventKey *ev, GTKMusicBrowser *p)
{
    if (ev->keyval == GDK_Delete)
        p->DeleteEvent();
}

static void playlist_row_move_internal(GtkWidget *widget, int source, int dest,
                                       GTKMusicBrowser *p)
{
    p->MoveItemEvent(source, dest);
    p->SetClickState(kContextPlaylist);
}

static void set_current_index_internal(GtkWidget *widget, int row, int column,
                                       GdkEventButton *button,
                                       GTKMusicBrowser *p)
{
    p->m_lastindex = row;
    p->m_plSelected.insert(row);
    if (button && button->type == GDK_2BUTTON_PRESS)
        p->PlayEvent();
    p->SetClickState(kContextPlaylist);
}

static void unset_current_index_internal(GtkWidget *widget, int row, int column,
                                         GdkEventButton *button,
                                         GTKMusicBrowser *p)
{
    p->m_plSelected.erase(row);
}

static gboolean list_drag_drop_internal(GtkWidget *widget,
                                        GdkDragContext *context,
                                        gint x, gint y, guint time,
                                        GTKMusicBrowser *p)
{
    GList *list = context->targets;
    GdkAtom atom = gdk_atom_intern("gtk-clist-drag-reorder", FALSE);
    GdkAtom tree = gdk_atom_intern("tree-drag", FALSE);
    GdkAtom plain = gdk_atom_intern("text/plain", FALSE);
    GdkAtom html = gdk_atom_intern("text/html", FALSE);
    while (list) {
        if (((int)atom == GPOINTER_TO_INT(list->data)) ||
            ((int)tree == GPOINTER_TO_INT(list->data)) ||
            ((int)plain == GPOINTER_TO_INT(list->data)) ||
            ((int)html == GPOINTER_TO_INT(list->data))) {
            gtk_drag_get_data(widget, context, GPOINTER_TO_INT(list->data), 
                              time);
            return TRUE;
        }
        list = list->next;
    }
    return FALSE;
}

static void drag_dest_info_destroy(gpointer data)
{
    GtkCListDestInfo *info = (GtkCListDestInfo *)data;
    free(info);
}

#define ROW_TOP_YPIXEL(clist, row) (((clist)->row_height * (row)) + \
                                    ((row) + 1) + (clist)->voffset)
#define ROW_FROM_YPIXEL(clist, y)  (((y) - (clist)->voffset) / \
                                    ((clist)->row_height + 1))
#define GTK_CLIST_CLASS_FW(_widget_) GTK_CLIST_CLASS (((GtkObject*) (_widget_))->klass)


static gint COLUMN_FROM_XPIXEL (GtkCList * clist, gint x)
{
  gint i, cx;

  for (i = 0; i < clist->columns; i++)
    if (clist->column[i].visible)
      {
        cx = clist->column[i].area.x + clist->hoffset;

        if (x >= (cx - 4) &&
            x <= (cx + clist->column[i].area.width + 3))
          return i;
      }

  return -1;
}

static void drag_dest_cell (GtkCList *clist, gint x, gint y,
                     GtkCListDestInfo *dest_info)
{
    GtkWidget *widget;

    widget = GTK_WIDGET (clist);
    dest_info->insert_pos = GTK_CLIST_DRAG_NONE;

    y -= (GTK_CONTAINER (clist)->border_width +
          widget->style->klass->ythickness + clist->column_title_area.height);

    dest_info->cell.row = ROW_FROM_YPIXEL (clist, y);
    if (dest_info->cell.row >= clist->rows) {
        dest_info->cell.row = clist->rows - 1;
        y = ROW_TOP_YPIXEL (clist, dest_info->cell.row) + clist->row_height;
    }
    if (dest_info->cell.row < 0) {
        dest_info->cell.column = -1;
        dest_info->cell.row = 0;
        dest_info->insert_pos = GTK_CLIST_DRAG_AFTER;
        return;
    }

    x -= GTK_CONTAINER (widget)->border_width +
         widget->style->klass->xthickness;
    dest_info->cell.column = COLUMN_FROM_XPIXEL (clist, x);

    if (dest_info->cell.row >= 0) {
        gint y_delta;
        gint h = 0;

        y_delta = y - ROW_TOP_YPIXEL (clist, dest_info->cell.row);

        if (GTK_CLIST_DRAW_DRAG_RECT(clist)) {
            dest_info->insert_pos = GTK_CLIST_DRAG_INTO;
            h = clist->row_height / 4;
        }
        else if (GTK_CLIST_DRAW_DRAG_LINE(clist)) {
            dest_info->insert_pos = GTK_CLIST_DRAG_BEFORE;
            h = clist->row_height / 2;
        }

        if (GTK_CLIST_DRAW_DRAG_LINE(clist)) {
            if (y_delta < h)
                dest_info->insert_pos = GTK_CLIST_DRAG_BEFORE;
            else if (clist->row_height - y_delta < h)
                dest_info->insert_pos = GTK_CLIST_DRAG_AFTER;
        }
    }
}

static void list_drag_rec_internal(GtkWidget *widget, GdkDragContext *context,
                                   gint x, gint y, GtkSelectionData *data,
                                   guint info, guint time, GTKMusicBrowser *p)
{
    if (widget == gtk_drag_get_source_widget(context))
        return;
    if (info == TARGET_TREE) {
        if (data->length == sizeof(vector<PlaylistItem *> *)) {
            vector<PlaylistItem *> *newlist;
            newlist = *((vector<PlaylistItem *>**)(data->data));
            if (newlist) {
                GtkCListDestInfo dest_info;

                drag_dest_cell(GTK_CLIST(widget), x, y, &dest_info);
                if (dest_info.insert_pos == GTK_CLIST_DRAG_AFTER)
                    dest_info.cell.row++;

                p->m_lastindex = dest_info.cell.row;
                p->AddTracksPlaylistEvent(newlist);
 
                p->SetClickState(kContextPlaylist);

                g_dataset_remove_data(context, "gtk-clist-drag-dest"); 
                gtk_drag_finish(context, TRUE, FALSE, time);

                delete newlist;
            }
        }
    }
    else if (info == TARGET_STRING || info == TARGET_URL) {
        if (data->data) {
            vector<PlaylistItem *> *itemList = new vector<PlaylistItem *>;
            vector<string> oFileList;
            char *filereturn = strdup_new((char *)(data->data));
            char *temp = strtok(filereturn, "\n");
            do {
                char *realname = strchr(temp, ':');
                realname++;
                if (realname && *realname) {
                    uint32 length = strlen(realname);
                    if (realname[length - 1] == '\r')
                        realname[length - 1] = '\0';
                    oFileList.push_back(string(realname));
                }
            }
            while ((temp = strtok(NULL, "\n")));
            delete [] filereturn;

            char *url;
            uint32 length;
            vector<string>::iterator i;

            url = new char[_MAX_PATH + 7];

            for (i = oFileList.begin(); i != oFileList.end(); i++) {
                char *ext = NULL;
                struct stat st;

                stat((*i).c_str(), &st);
                if (st.st_mode & S_IFDIR) {
                    vector<string> oList, oQuery;
                    oQuery.push_back(string("*.mp1"));
                    oQuery.push_back(string("*.mp2"));
                    oQuery.push_back(string("*.mp3"));
 
                    FindMusicFiles((*i).c_str(), oList, oQuery);
                    if (oList.size() > 0) {
                        vector<string>::iterator j = oList.begin();
                        for (; j != oList.end(); j++) {
                            PlaylistItem *newitem = new PlaylistItem((*j).c_str());
                            itemList->push_back(newitem);
                        }
                    }
                }
                else {
                    ext = p->GetContext()->player->GetExtension((*i).c_str());
                    FilePathToURL((*i).c_str(), url, &length);

                    if (ext) {
                        PlaylistManager *plm = p->GetContext()->plm;
                        if (plm->IsSupportedPlaylistFormat(ext)) 
                            plm->ReadPlaylist(url, itemList);
                        else if (p->GetContext()->player->IsSupportedExtension(ext))
                        {
                            PlaylistItem *newitem = new PlaylistItem(url);
                            itemList->push_back(newitem);
                        }
                        delete [] ext;
                    }
                }
            }

            delete [] url;

            GtkCListDestInfo dest_info;

            drag_dest_cell(GTK_CLIST(widget), x, y, &dest_info);
            if (dest_info.insert_pos == GTK_CLIST_DRAG_AFTER)
                dest_info.cell.row++;

            p->m_lastindex = dest_info.cell.row;
            p->AddTracksPlaylistEvent(itemList);

            delete itemList;

            p->SetClickState(kContextPlaylist);

            g_dataset_remove_data(context, "gtk-clist-drag-dest");
            gtk_drag_finish(context, TRUE, FALSE, time);
        }
    }
    gtk_drag_finish(context, FALSE, FALSE, time);
}

static void list_drag_leave_internal(GtkWidget *widget, GdkDragContext *context,
                                     guint time, GTKMusicBrowser *p)
{
    if (widget == gtk_drag_get_source_widget(context))
        return;

    GtkCList *clist = GTK_CLIST(widget);
    GtkCListDestInfo *dest_info;
    dest_info = (GtkCListDestInfo *)g_dataset_get_data(context,
                                                       "gtk-clist-drag-dest");
    if (dest_info) {
        if (dest_info->cell.row >= 0) {
            GList *list = context->targets;
            GdkAtom tree = gdk_atom_intern("tree-drag", FALSE);
            GdkAtom plain = gdk_atom_intern("text/plain", FALSE);
            GdkAtom html = gdk_atom_intern("text/html", FALSE); 
            while (list) {
                if (((int)tree == GPOINTER_TO_INT(list->data)) ||
                    ((int)plain == GPOINTER_TO_INT(list->data)) ||
                    ((int)html == GPOINTER_TO_INT(list->data))) {
                    GTK_CLIST_CLASS_FW(clist)->draw_drag_highlight(clist,
                            (GtkCListRow *)g_list_nth(clist->row_list,
                                                     dest_info->cell.row)->data,
                             dest_info->cell.row, dest_info->insert_pos);
                    break;
                }
                list = list->next;
            }
        }
        g_dataset_remove_data(context, "gtk-clist-drag-dest");
    }
}

static GdkAtom i_drag_dest_find_target(GtkWidget *widget,
                                       GtkDragDestSite *site,
                                       GdkDragContext *context)
{
  GList *tmp_target;
  GList *tmp_source = NULL;
  GtkWidget *source_widget = gtk_drag_get_source_widget (context);

  tmp_target = site->target_list->list;
  while (tmp_target)
    {
      GtkTargetPair *pair = (GtkTargetPair *)(tmp_target->data);
      tmp_source = context->targets;
      while (tmp_source)
        {
          if (tmp_source->data == GUINT_TO_POINTER (pair->target))
            {
              if ((!(pair->flags & GTK_TARGET_SAME_APP) || source_widget) &&
                  (!(pair->flags & GTK_TARGET_SAME_WIDGET) || (source_widget == widget)))
                return pair->target;
              else
                break;
            }
          tmp_source = tmp_source->next;
        }
      tmp_target = tmp_target->next;
    }

  return GDK_NONE;
}

static gint list_drag_motion_internal(GtkWidget *widget,
                                      GdkDragContext *context,
                                      gint x, gint y, guint time,
                                      GTKMusicBrowser *p)
{
    GtkDragDestSite *site;
    GdkDragAction action = (GdkDragAction)0;
    site = (GtkDragDestSite *)gtk_object_get_data(GTK_OBJECT(widget), "gtk-drag-dest");

    if (context->suggested_action & site->actions)
        action = context->suggested_action;
    else {
        gint i;
        for (i = 0; i < 8; i++) {
            if ((site->actions & (1 << i)) && (context->actions & (1 << i))) {
                action = (GdkDragAction)(1 << i);
                break;
            }
        }
    }

    if (action && i_drag_dest_find_target(widget, site, context)) {
        if (!site->have_drag) 
            site->have_drag = TRUE;
        gdk_drag_status(context, action, time);
    }

    GtkCList *clist = GTK_CLIST(widget);
    GtkCListDestInfo new_info;
    GtkCListDestInfo *dest_info;

    dest_info = (GtkCListDestInfo *)g_dataset_get_data(context,
                                                       "gtk-clist-drag-dest");
    if (!dest_info) {
        dest_info = (GtkCListDestInfo *)malloc(sizeof(GtkCListDestInfo));
        dest_info->insert_pos = GTK_CLIST_DRAG_NONE;
        dest_info->cell.row = -1;
        dest_info->cell.column = -1;
        g_dataset_set_data_full(context, "gtk-clist-drag-dest", dest_info,
                                drag_dest_info_destroy);
    }
    drag_dest_cell(clist, x, y, &new_info);

    GList *list = context->targets;
    GdkAtom atom = gdk_atom_intern("gtk-clist-drag-reorder", FALSE);
    GdkAtom tree = gdk_atom_intern("tree-drag", FALSE);
    GdkAtom plain = gdk_atom_intern("text/plain", FALSE);
    GdkAtom html = gdk_atom_intern("text/html", FALSE);
    list = context->targets;
    while (list) {
        if ((int)tree == GPOINTER_TO_INT(list->data))
            break;
        else if ((int)plain == GPOINTER_TO_INT(list->data))
            break;
        else if ((int)html == GPOINTER_TO_INT(list->data))
            break;
        else if ((int)atom == GPOINTER_TO_INT(list->data))
            break;
        list = list->next;
    }
    if (list) {
        site->have_drag = TRUE;
        if (new_info.cell.row != dest_info->cell.row ||
            (new_info.cell.row == dest_info->cell.row &&
             dest_info->insert_pos != new_info.insert_pos)) {
            if (dest_info->cell.row >= 0 && dest_info->cell.column != -1)
                GTK_CLIST_CLASS_FW(clist)->draw_drag_highlight(clist,
                        (GtkCListRow *)g_list_nth(clist->row_list,
                                                  dest_info->cell.row)->data,
                        dest_info->cell.row, dest_info->insert_pos);
            dest_info->insert_pos = new_info.insert_pos;
            dest_info->cell.row = new_info.cell.row;
            dest_info->cell.column = new_info.cell.column;

            if (dest_info->cell.column != -1)
                GTK_CLIST_CLASS_FW(clist)->draw_drag_highlight(clist,
                        (GtkCListRow *)g_list_nth(clist->row_list,
                                                  dest_info->cell.row)->data,
                        dest_info->cell.row, dest_info->insert_pos);
            gdk_drag_status(context, context->suggested_action, time);
        }
        return TRUE;
    }

    dest_info->insert_pos = new_info.insert_pos;
    dest_info->cell.row = new_info.cell.row;
    dest_info->cell.column = new_info.cell.column;
    return TRUE;
}

static void play_now_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->PlayEvent();
}

static void move_up_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->MoveUpEvent();
}

static void move_down_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->MoveDownEvent();
}

static void delete_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->DeleteEvent();
}

static void edit_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->PopUpInfoEditor();
}

static void add_fav_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->AddPLStreamToFavs();
}

static void tip_pop(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->TipArtist();
}

void GTKMusicBrowser::PlaylistRightClick(int x, int y, uint32 time)
{
    if (m_lastindex == kInvalidIndex)
        return;

    PlaylistItem *sel = m_plm->ItemAt(m_lastindex);
    if (!sel)
        return;

    string url = sel->URL();
    if (url.find("http://") < url.length() || url.find("rtp://") < url.length())
        gtk_item_factory_popup(playlist2Popup, x, y, 3, time);
    else
        gtk_item_factory_popup(playlistPopup, x, y, 3, time);
}

static void list_clicked(GtkWidget *w, GdkEventButton *event,
                         GTKMusicBrowser *p)
{
    if (!event)
        return;

    g_return_if_fail(w != NULL);
    g_return_if_fail(GTK_IS_CLIST(w));
    g_return_if_fail(event != NULL);

    GtkCList *clist = GTK_CLIST(w);

    if (event->window != clist->clist_window)
        return;

    p->SetClickState(kContextPlaylist);

    if (event->button == 3) {
        int row;

        gtk_clist_get_selection_info(clist, (int)event->x, 
                                     (int)event->y, &row, NULL);
        p->m_lastindex = row;
        gtk_clist_select_row(clist, row, 0);
        p->PlaylistRightClick((int)event->x_root, 
                              (int)event->y_root,
                              event->time);
    }
}

void GTKMusicBrowser::CreatePlaylistList(GtkWidget *box)
{
    GtkItemFactoryEntry popup_items[] = {
     {"/Play Now",     NULL,      (GtkItemFactoryCallback)play_now_pop,  0, 0 },
     {"/Move Up",      NULL,      (GtkItemFactoryCallback)move_up_pop,   0, 0 },
     {"/Move Down",    NULL,      (GtkItemFactoryCallback)move_down_pop, 0, 0 },
     {"/sep1",         NULL,      0,                        0, "<Separator>" },
     {"/Remove",       NULL,      (GtkItemFactoryCallback)delete_pop,     0, 0 },
     {"/sep2",         NULL,      0,                        0, "<Separator>" },
     {"/Edit Info",    NULL,      (GtkItemFactoryCallback)edit_pop,      0, 0 },
     {"/sep3",         NULL,      0,                        0, "<Separator>" },
     {"/Tip This Artist", NULL,   (GtkItemFactoryCallback)tip_pop,       0, 0 }
    };
 
    int nmenu_items = sizeof(popup_items) / sizeof(popup_items[0]);

    GtkItemFactoryEntry popup2_items[] = {
     {"/Play Now",     NULL,      (GtkItemFactoryCallback)play_now_pop,  0, 0 },
     {"/Move Up",      NULL,      (GtkItemFactoryCallback)move_up_pop,   0, 0 },
     {"/Move Down",    NULL,      (GtkItemFactoryCallback)move_down_pop, 0, 0 },
     {"/sep1",         NULL,      0,                        0, "<Separator>" },
     {"/Remove",       NULL,      (GtkItemFactoryCallback)delete_pop,    0, 0 },
     {"/sep2",         NULL,      0,                        0, "<Separator>" },
     {"/Edit Info",    NULL,      (GtkItemFactoryCallback)edit_pop,      0, 0 },
     {"/sep3",         NULL,      0,                        0, "<Separator>" },
     {"/Add Stream to Favorites", NULL, (GtkItemFactoryCallback)add_fav_pop, 0, 0 }
    };

    int nmenu2_items = sizeof(popup2_items) / sizeof(popup2_items[0]);
     
    playlistList = gtk_clist_new(9);
    gtk_container_add(GTK_CONTAINER(box), playlistList);
    gtk_signal_connect(GTK_OBJECT(playlistList), "row_move",
                       GTK_SIGNAL_FUNC(playlist_row_move_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "select_row",
                       GTK_SIGNAL_FUNC(set_current_index_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "unselect_row",
                       GTK_SIGNAL_FUNC(unset_current_index_internal), this);
    gtk_clist_set_selection_mode(GTK_CLIST(playlistList),
                                 GTK_SELECTION_EXTENDED);
    GtkTargetEntry new_clist_target_table[4] = {
        {"gtk-clist-drag-reorder", 0, 0},
        {"tree-drag", 0, TARGET_TREE},
        {"text/plain", 0, TARGET_STRING},
        {"text/html", 0, TARGET_URL},
    };
    GTK_CLIST_SET_FLAG(GTK_CLIST(playlistList), CLIST_REORDERABLE);
    gtk_drag_dest_set(playlistList, (GtkDestDefaults)0,
                      (GtkTargetEntry *)&new_clist_target_table, 4,
                      GDK_ACTION_MOVE);
    gtk_signal_connect(GTK_OBJECT(playlistList), "drag_leave",
                       GTK_SIGNAL_FUNC(list_drag_leave_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "drag_drop",
                       GTK_SIGNAL_FUNC(list_drag_drop_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "drag_data_received",
                       GTK_SIGNAL_FUNC(list_drag_rec_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "drag_motion",
                       GTK_SIGNAL_FUNC(list_drag_motion_internal), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "key_press_event",
                       GTK_SIGNAL_FUNC(list_keypress), this);
    gtk_signal_connect(GTK_OBJECT(playlistList), "button_press_event",
                       GTK_SIGNAL_FUNC(list_clicked), this);

    playlistPopup = gtk_item_factory_new(GTK_TYPE_MENU, "<plist_popup>",
                                         NULL);
    gtk_item_factory_create_items(playlistPopup, nmenu_items, popup_items, 
                                  (void *)this);

    playlist2Popup = gtk_item_factory_new(GTK_TYPE_MENU, "<plist2_popup>", 
                                          NULL);
    gtk_item_factory_create_items(playlist2Popup, nmenu2_items,
                                  popup2_items, (void *)this);

    gtk_clist_columns_autosize(GTK_CLIST(playlistList));
    gtk_widget_show(playlistList);

    m_lastindex = m_plm->GetCurrentIndex();

    UpdatePlaylistList();
}
