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

        $Id: browsermenu.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <sys/stat.h>
#include <unistd.h>

#include "gtkmusicbrowser.h"
#include "fileselector.h"
#include "gtkmessagedialog.h"
#include "player.h"
#include "help.h"

#include "../res/new_pic.xpm"
#include "../res/save_pic.xpm"
#include "../res/import_pic.xpm"
#include "../res/edit_pic.xpm"
#include "../res/add_track.xpm"
#include "../res/add_file.xpm"
#include "../res/up_pic.xpm"
#include "../res/down_pic.xpm"
#include "../res/trash_pic.xpm"


static void add_tool(GtkWidget *widget, GTKMusicBrowser *p)
{
    p->SetClickState(kContextPlaylist);
    FileSelector *filesel = new FileSelector(p->GetContext(),"Add a Track");
    filesel->SetExtended();
    if (filesel->Run()) {
        char *returnpath = filesel->GetReturnPath();
        char *ext = p->GetContext()->player->GetExtension(returnpath);
        uint32 length = strlen(returnpath) + 10;
        char *tempurl = new char[length];
        if (IsntError(FilePathToURL(returnpath, tempurl, &length))) {
            if (ext && p->GetContext()->plm->IsSupportedPlaylistFormat(ext)) {
                string tobeloaded = tempurl;
                p->LoadPlaylist(tobeloaded);
            }
            else {
                char *filereturn = strdup_new(filesel->GetReturnPath());
                if (filereturn) {
                    char *temp;
                    char *first= strtok(filereturn, "\n");

                    while ((temp = strtok(NULL, "\n"))) {
                        if (ext)
                            delete [] ext;
                        ext = p->GetContext()->player->GetExtension(temp);
                        if (ext &&
                            p->GetContext()->player->IsSupportedExtension(ext)) 
                        {
                            p->AddTrackPlaylistEvent(temp);
                            p->m_lastindex++;
                        }
                    }
                    if (ext)
                        delete [] ext;
                    ext = p->GetContext()->player->GetExtension(first);
                    if (ext &&
                        p->GetContext()->player->IsSupportedExtension(ext))
                        p->AddTrackPlaylistEvent(first);
                }
                delete [] filereturn;
            }
        }
        delete [] tempurl;
        delete [] ext;
    }
    delete filesel;
}

static void move_up_tool(GtkWidget *widget, GTKMusicBrowser *p)
{
    p->MoveUpEvent();
    p->SetClickState(kContextPlaylist);
}

static void move_down_tool(GtkWidget *widget, GTKMusicBrowser *p)
{
    p->MoveDownEvent();
    p->SetClickState(kContextPlaylist);
}

static void import_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    FileSelector *filesel = new FileSelector(p->GetContext(),"Import a Track or Playlist into My Music");
    if (filesel->Run()) {
        FAContext *m_context = p->GetContext();
        char *returnpath = filesel->GetReturnPath();
        char *filereturn = strdup_new(returnpath);
        if (filereturn)
        {
            char *first = strtok(filereturn, "\n");
            while (first) {
                char *ext = m_context->player->GetExtension(first);
                uint32 length = strlen(first) + 10;
                char *tempurl = new char[length];

                if (IsntError(FilePathToURL(first, tempurl, &length))) 
                {
                    if (ext && m_context->plm->IsSupportedPlaylistFormat(ext))
                        p->ImportPlaylist(tempurl);
                    else if (ext && 
                             m_context->player->IsSupportedExtension(ext)) 
                    {
                        PlaylistItem *plist = new PlaylistItem(tempurl);
                        m_context->plm->RetrieveMetaDataNow(plist);

                        m_context->catalog->WriteMetaDataToDatabase(tempurl,
                                                          plist->GetMetaData());
                        m_context->catalog->AddSong(tempurl);

                        delete plist;
                    }
                }
                delete [] tempurl;
                delete ext;

                first = strtok(NULL, "\n");
            }
            delete [] filereturn;
        }
    }
    delete filesel;
}

static void remove_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    p->DeleteEvent();
}

static void edit_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    p->PopUpInfoEditor();
}

static void infoedit(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    edit_tool(w, p);
}

static void new_plist(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SaveCurrentPlaylist();

    p->CreateNewEditor(NULL);
}

static void new_plist_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    new_plist(p, 1, w);
}

static void open_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SaveCurrentPlaylist();

    FileSelector *filesel = new FileSelector(p->GetContext(),"Open a Playlist from Disk");
    if (filesel->Run())
        p->CreateNewEditor(filesel->GetReturnPath());
    delete filesel;
}

static void saveas_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    GTKMessageDialog oBox;
    string oMessage = string("What do you want to call this playlist?");

    if (oBox.Show(oMessage.c_str(), "Save This List to Disk?", kMessageOkCancel,
                  true, true)
                  == kMessageReturnOk) {

        char *temp = oBox.GetEntryText();
        if (!temp)
            return;

        for (unsigned int i = 0; i < strlen(temp); i++)
            if (temp[i] == '/')
                temp[i] = '_';

        char *fadir = FreeampDir(NULL);
        string name = string(fadir) + string("/") + string(temp) +
                      string(".m3u");
        p->m_currentListName = string(name);

        delete [] fadir;

        p->SaveCurrentPlaylist();
    }
}

static void save_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    if (p->m_currentListName.length() == 0) {
        saveas_list(p, action, w);
    }
    else
        p->SaveCurrentPlaylist();
}

static void save_list_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    save_list(p, 1, w);
}

static void import_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    import_tool(w, p);
}

static void export_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    FileSelector *filesel = new FileSelector(p->GetContext(),"Export This Playlist to Disk");
    if (filesel->Run())
        p->SaveCurrentPlaylist(filesel->GetReturnPath());
    delete filesel;
}

static void music_search(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->StartMusicSearch();
}

static void quit_menu(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->ToggleVisEvent();
}

static void add_track_mb(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    if (p->GetClickState() == kContextBrowser) {
        vector<PlaylistItem *> *newlist = p->GetTreeSelection();
        p->AddTracksPlaylistEvent(newlist, true);
    }
}

static void add_track_tool(GtkWidget *w, GTKMusicBrowser *p)
{
    add_track_mb(p, 0, w);
}

static void add_track(GTKMusicBrowser *p, guint action, GtkWidget *w)
{   
    FileSelector *filesel = new FileSelector(p->GetContext(),"Add a Track");
    filesel->SetExtended();
    if (filesel->Run()) {
        char *filereturn = strdup_new(filesel->GetReturnPath());
        if (filereturn) {
            char *temp;
            char *first;
            first = strtok(filereturn, "\n");
            while ((temp = strtok(NULL, "\n"))) {
                p->AddTrackPlaylistEvent(temp);
                p->m_lastindex++;
            }
            p->AddTrackPlaylistEvent(first);
       }
       delete filereturn;
    }
    delete filesel;
}

bool GTKMusicBrowser::AskToDelete(string &url)
{
    bool retvalue = false;

    uint32 length = url.size() + 1;
    char *filename = new char[length];

    if (IsntError(URLToFilePath(url.c_str(), filename, &length))) {
        GTKMessageDialog oBox;
        string oMessage = string("Are you sure you want to delete ")
                          + string(filename) + string("?");

        if (oBox.Show(oMessage.c_str(), "Delete Confirmation", kMessageYesNo,
                      true, false, "Delete it Permantly From the Disk")
                      == kMessageReturnYes) {

            if (oBox.GetCheckStatus()) 
                unlink(filename);

            retvalue = true;
        }
    }

    delete [] filename;

    return retvalue;
}

static void delete_sel(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    string urlToDel;
 
    if (p->GetClickState() == kContextPlaylist) {
        set<uint32>::reverse_iterator i = p->m_plSelected.rbegin();
        for (; i != p->m_plSelected.rend(); i++) {
            urlToDel = p->GetContext()->plm->ItemAt(*i)->URL();

            if (p->AskToDelete(urlToDel)) 
                p->DeletePlaylistItem(*i);
        }
    }
    else if (p->GetClickState() == kContextBrowser) {
        vector<TreeData *> local_mbSelections(*(p->mbSelections));
        vector<TreeData *>::iterator i = local_mbSelections.begin();
        for (; i != local_mbSelections.end(); i++) {
            TreeNodeType type = (*i)->type;
            
            if (type == kTreePlaylist) 
                urlToDel = (*i)->playlistname;
            else if (type == kTreeTrack)
                urlToDel = (*i)->track->URL();
            else if (type == kTreeArtist) {
                ArtistList *list = (*i)->artist;
                vector<AlbumList *>::reverse_iterator j = 
                                                 list->m_albumList->rbegin();
                for (; j != list->m_albumList->rend(); j++) {
                    vector<PlaylistItem *>::reverse_iterator k = 
                                                    (*j)->m_trackList->rbegin();
                    for (; k != (*j)->m_trackList->rend(); k++) {
                        urlToDel = (*k)->URL();
                        if (p->AskToDelete(urlToDel))
                            p->GetContext()->catalog->RemoveSong(urlToDel.c_str());
                    }
                }
            }
            else if (type == kTreeAlbum) {
                AlbumList *list = (*i)->album;
                vector<PlaylistItem *>::reverse_iterator j = 
                                                   list->m_trackList->rbegin();
                for (; j != list->m_trackList->rend(); j++) {
                    urlToDel = (*j)->URL();
                    if (p->AskToDelete(urlToDel))
                        p->GetContext()->catalog->RemoveSong(urlToDel.c_str());
                }
            }
            else
                continue;

            if (p->AskToDelete(urlToDel) && 
                (type == kTreeTrack || type == kTreePlaylist)) {
                switch (type) {
                    case kTreePlaylist: {
                        p->GetContext()->catalog->RemovePlaylist(urlToDel.c_str());
                        break; }
                    case kTreeTrack: {
                        p->GetContext()->catalog->RemoveSong(urlToDel.c_str());
                        break; }
                    default:
                        break;
                }
            }
        }
    }
}

static void move_up(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->MoveUpEvent();
}

static void move_down(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->MoveDownEvent();
}

static void clear_list(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->DeleteListEvent();
}

static void catalog_tog(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->ExpandCollapseEvent();
}

static void play_menu(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->PlayMenu();
}

static void stop_menu(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->StopMenu();
}

static void next_menu(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->NextMenu();
}

static void prev_menu(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->PrevMenu();
}

static void repeat_none(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
   p->SetRepeat(0);
}

static void repeat_one(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
   p->SetRepeat(1);
}

static void repeat_all(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
   p->SetRepeat(2);
}

static void sort_artist(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Artist, kPlaylistSortType_Ascending);
}

static void sort_album(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Album, kPlaylistSortType_Ascending);
}

static void sort_title(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Title, kPlaylistSortType_Ascending);

}

static void sort_year(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Year, kPlaylistSortType_Ascending);
}

static void sort_track(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Track, kPlaylistSortType_Ascending);
}

static void sort_comment(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Comment, kPlaylistSortType_Ascending);
}

static void sort_genre(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Genre, kPlaylistSortType_Ascending);
}

static void sort_time(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Time, kPlaylistSortType_Ascending);
}

static void sort_location(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Location, kPlaylistSortType_Ascending);
}

static void sort_filename(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_FileName, kPlaylistSortType_Ascending);
}

static void sort_normal(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->iSetShuffleMode = true;
    p->SortPlaylistEvent(kPlaylistSortKey_LastKey, kPlaylistSortType_Descending);
}

static void sort_random2(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->iSetShuffleMode = true;
    p->SortPlaylistEvent(kPlaylistSortKey_LastKey, kPlaylistSortType_Ascending);
}

static void sort_random(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->SortPlaylistEvent(kPlaylistSortKey_Random, kPlaylistSortType_Ascending);
}

static void zinf_web(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    LaunchBrowser("http://www.zinf.org/");
}

static void show_about(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->ShowOptions(9);
}

static void options_show(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->ShowOptions();
}

static void add_stream(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->AddStreamToFavs();
}

static void eject_cd(GTKMusicBrowser *p, guint action, GtkWidget *w)
{
    p->EjectCD();
}

void GTKMusicBrowser::CreateMenu(GtkWidget *topbox)
{
    GtkItemFactoryEntry menu_items[] = {
     {"/_File",                 NULL,           0,         0, "<Branch>" },
     {"/File/_New Playlist",    "<control>N",   (GtkItemFactoryCallback)new_plist, 0, 0 },
     {"/File/_Open Playlist",   "<control>O",   (GtkItemFactoryCallback)open_list, 0, 0 },
     {"/File/_Save Playlist",   "<control>S",   (GtkItemFactoryCallback)save_list, 0, 0 },
     {"/File/Save Playlist _As","<control>A",   (GtkItemFactoryCallback)saveas_list,0, 0 },
     {"/File/sep1",             NULL,           0,         0, "<Separator>" },
     {"/File/_Import Tracks and Playlists","<control>I", (GtkItemFactoryCallback)import_list, 0, 0 },
     {"/File/_Export Playlist", NULL,           (GtkItemFactoryCallback)export_list, 0, 0 },
     {"/File/sep0",             NULL,           0,         0, "<Separator>" },
     {"/File/Search Computer for Music", NULL, (GtkItemFactoryCallback)music_search, 0, 0 },
     {"/File/sep2",             NULL,           0,         0, "<Separator>" },
     {"/File/_Close",           "<control>Q",   (GtkItemFactoryCallback)quit_menu, 0, 0 },

     {"/_Edit",                 NULL,           0,         0, "<Branch>" },
     {"/_Edit/_Add Items to Playlist", NULL,   (GtkItemFactoryCallback)add_track_mb, 0, 0 },
     {"/_Edit/Add _Tracks or Playlists from Disk", NULL,  (GtkItemFactoryCallback)add_track, 0, 0 },
     {"/_Edit/_Remove Items from My Music", NULL,(GtkItemFactoryCallback)delete_sel,0, 0 },
     {"/_Edit/Add Stream to Favorites", NULL,   (GtkItemFactoryCallback)add_stream, 0, 0 },
     {"/_Edit/sep3",            NULL,           0,         0, "<Separator>" },
     {"/_Edit/Move _Up",         NULL,           (GtkItemFactoryCallback)move_up,   0, 0 },
     {"/_Edit/Move _Down",       NULL,           (GtkItemFactoryCallback)move_down, 0, 0 },
     {"/_Edit/sep4",            NULL,           0,         0, "<Separator>" },
     {"/_Edit/_Clear Playlist",  NULL,           (GtkItemFactoryCallback)clear_list, 0, 0 },
     {"/_Edit/Edit _Info",       NULL,           (GtkItemFactoryCallback)infoedit,  0, 0 },

     {"/_View",                 NULL,           0,         0, "<Branch>" },
     {"/_View/View _Playlist Only",NULL,        (GtkItemFactoryCallback)catalog_tog, 0, 0 },
     {"/_View/_Options",         NULL,         (GtkItemFactoryCallback)options_show, 0, 0 },

     {"/_Controls",             NULL,           0,         0, "<Branch>" },
     {"/_Controls/_Play",        NULL,           (GtkItemFactoryCallback)play_menu,0, 0 },
     {"/_Controls/_Stop",        NULL,           (GtkItemFactoryCallback)stop_menu, 0, 0 },
     {"/_Controls/sep6",        NULL,           0,         0, "<Separator>" },
     {"/_Controls/_Eject CD",    NULL,           (GtkItemFactoryCallback)eject_cd, 0, 0 },
     {"/_Controls/sep65",       NULL,           0,         0, "<Separator>" },
     {"/_Controls/_Next Track",   NULL,          (GtkItemFactoryCallback)next_menu, 0, 0 },
     {"/_Controls/Pre_vious Track", NULL,        (GtkItemFactoryCallback)prev_menu, 0, 0 },
     {"/_Controls/sep7",        NULL,           0,         0, "<Separator>" },
     {"/_Controls/Play Tracks in Nor_mal Order", NULL, (GtkItemFactoryCallback)sort_normal, 0, "<RadioItem>" },
     {"/_Controls/Play Tracks in _Random Order", NULL, (GtkItemFactoryCallback)sort_random2, 0, "/Controls/Play Tracks in Normal Order" },
     {"/_Controls/sep8",        NULL,           0,         0, "<Separator>" },
     {"/_Controls/Repeat N_o Tracks", NULL,      (GtkItemFactoryCallback)repeat_none, 0, "<RadioItem>" },
     {"/_Controls/Repeat _Current Track",  NULL, (GtkItemFactoryCallback)repeat_one, 0, "/Controls/Repeat No Tracks" },
     {"/_Controls/Repeat _All Tracks",  NULL,    (GtkItemFactoryCallback)repeat_all, 0, "/Controls/Repeat No Tracks" },

     {"/_Sort Playlist",        NULL,           0,         0, "<Branch>" },
     {"/_Sort Playlist/by _Artist",  NULL,      (GtkItemFactoryCallback)sort_artist, 0, 0 },
     {"/_Sort Playlist/by A_lbum", NULL,         (GtkItemFactoryCallback)sort_album, 0, 0 },
     {"/_Sort Playlist/by _Title", NULL,         (GtkItemFactoryCallback)sort_title, 0, 0 },
     {"/_Sort Playlist/by _Year", NULL,          (GtkItemFactoryCallback)sort_year,  0, 0 },
     {"/_Sort Playlist/by Trac_k Number", NULL,  (GtkItemFactoryCallback)sort_track, 0, 0 },
     {"/_Sort Playlist/by _Genre", NULL,         (GtkItemFactoryCallback)sort_genre, 0, 0 },
     {"/_Sort Playlist/by _Length", NULL,        (GtkItemFactoryCallback)sort_time,  0, 0 },
     {"/_Sort Playlist/by L_ocation", NULL,   (GtkItemFactoryCallback)sort_location, 0, 0 },
     {"/_Sort Playlist/by _Filename", NULL,   (GtkItemFactoryCallback)sort_filename, 0, 0 },
     {"/_Sort Playlist/by _Comment", NULL,    (GtkItemFactoryCallback)sort_comment, 0, 0},
     {"/_Sort Playlist/_Randomly", NULL,        (GtkItemFactoryCallback)sort_random, 0, 0 },

     {"/_Help",                 NULL,           0,          0, "<Branch>" },
     {"/_Help/_Zinf Web Site", NULL,         (GtkItemFactoryCallback)zinf_web, 0, 0 },
     {"/_Help/_About",           NULL,           (GtkItemFactoryCallback)show_about, 0, 0 }
    };

    int nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);


    GtkAccelGroup *accel_group;
    GtkWidget *separator;

    accel_group = gtk_accel_group_new();
    menuFactory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<blah>",
                                        accel_group);
    gtk_item_factory_create_items(menuFactory, nmenu_items, menu_items, 
                                  (void *)this);

    gtk_accel_group_attach(accel_group, GTK_OBJECT(musicBrowser));
    gtk_box_pack_start(GTK_BOX(topbox), gtk_item_factory_get_widget(
                       menuFactory, "<blah>"), FALSE, TRUE, 0);
    gtk_widget_show(gtk_item_factory_get_widget(menuFactory, "<blah>"));

    separator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(topbox), separator, FALSE, TRUE, 0);
    gtk_widget_show(separator);
}

void GTKMusicBrowser::SetToolbarType(void)
{
    bool useText, useImages;
    m_context->prefs->GetPrefBoolean(kShowToolbarTextLabelsPref, &useText);
    m_context->prefs->GetPrefBoolean(kShowToolbarImagesPref, &useImages);

    if (useText && useImages)
        gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);
    else if (useText)
        gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_TEXT);
    else if (useImages)
        gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
}

void GTKMusicBrowser::CreateToolbar(GtkWidget *vbox)
{
    toolbar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
                                         GTK_TOOLBAR_BOTH);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar), GTK_RELIEF_NONE);
    gtk_toolbar_set_space_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_SPACE_LINE);

    gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "New Playlist",
                            "Start Editing a new Playlist",
                            "Toolbar/New", NewPixmap(new_pic),
                            GTK_SIGNAL_FUNC(new_plist_tool), this);

    gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Save Playlist",
                            "Save the Current Playlist to Disk",
                            "Toolbar/Save", NewPixmap(save_pic),
                            GTK_SIGNAL_FUNC(save_list_tool), this);

    gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));

    gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Import Items",
                            "Import a Playlist or Track into My Music",
                            "Toolbar/Import", NewPixmap(import_pic),
                            GTK_SIGNAL_FUNC(import_tool), this);

    toolRemove = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Remove Track",
                                         "Remove a Track from the Playlist",
                                         "Toolbar/Remove", NewPixmap(trash_pic),
                                         GTK_SIGNAL_FUNC(remove_tool), this);

    gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Edit",
                            "Edit a Track or Playlist",
                            "Toolbar/Edit", NewPixmap(edit_pic),
                            GTK_SIGNAL_FUNC(edit_tool), this);

    gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));

    addTrack = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Add Items",
                            "Add Music from My Music to the Playlist",
                            "Toolbar/AddItem", NewPixmap(add_track_pic),
                            GTK_SIGNAL_FUNC(add_track_tool), this);

    addFile = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Add File",
                            "Add a File to the Playlist",
                            "Toolbar/Add", NewPixmap(add_file),
                            GTK_SIGNAL_FUNC(add_tool), this);

    toolUp = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Move Up",
                            "Move The Currently Selected Track in the Playlist Up",
                            "Toolbar/Up", NewPixmap(up_pic),
                            GTK_SIGNAL_FUNC(move_up_tool), this);

    toolDown = gtk_toolbar_append_item(GTK_TOOLBAR(toolbar), "Move Down",
                            "Move the Currently Selected Track in the Playlist Down",
                            "Toolbar/Down", NewPixmap(down_pic),
                            GTK_SIGNAL_FUNC(move_down_tool), this);

    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, TRUE, 0);
    SetToolbarType();
    gtk_widget_show(toolbar);
}
