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

    $Id: gtkmusicbrowser.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_GTKMUSICBROWSER_H_
#define INCLUDED_GTKMUSICBROWSER_H_

#include "config.h"

#include <set>
using namespace std;

#include <gtk/gtk.h>

#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"
#include "musiccatalog.h"
#include "timer.h"

class FAContext;
class MusicBrowserUI;

typedef enum {
    kEmptyColumn,
    kArtistColumn,
    kAlbumColumn,
    kCommentColumn,
    kGenreColumn,
    kLocationColumn,
    kPositionColumn,
    kTitleColumn,
    kTimeColumn,
    kYearColumn
} PlaylistColumns;

typedef enum {
    kStateCollapsed,
    kStateExpanded
} MusicBrowserView;

typedef enum {
    kContextNone,
    kContextPlaylist,
    kContextBrowser
} ClickState;

typedef enum {
   kTreeMyMusic,
   kTreeAll,
   kTreeUncat,
   kTreeArtist,
   kTreeAlbum,
   kTreeTrack,
   kTreeStream,
   kTreeFavStream,
   kTreeCD,
   kTreePlaylistHead,
   kTreePlaylist,
   kTreeCDHead,
   kTreeStreamsHead,
   kTreeFavoriteStreamsHead,
   kTreeStreamFolder,
   kTreeRelatableHead
} TreeNodeType;

enum {
   TARGET_TREE = 1, 
   TARGET_STRING,
   TARGET_URL
};

typedef struct {
    TreeNodeType            type;
    MusicCatalog           *catalog;
    ArtistList             *artist;
    AlbumList              *album;
    PlaylistItem           *track;
    string                  playlistname;
    string                  message;
    vector<PlaylistItem *> *cdtracks;
} TreeData;

class GTKMusicBrowser {
 public:
    GTKMusicBrowser(FAContext *, MusicBrowserUI *masterUI, 
                    string playlistURL = "",
                    bool cdCreationMode = false);
    virtual ~GTKMusicBrowser();

    void ShowMusicBrowser(void);
    void Close(bool inMain = true);
 
    bool Visible(void) { return isVisible; }
    Error AcceptEvent(Event *e);

    uint32 m_lastindex;
    set<uint32> m_plSelected;
    
    PlaylistSortType m_playlistColumnSort;
    int m_playlistLastSort;
    string m_currentListName;

    vector<TreeData *> *mbSelections;

    GtkCTree *musicBrowserTree;
 
    FAContext *GetContext(void) { return m_context; }
    void UpdateCatalog(void);

    int pauseState;
    int stopState;

    vector<PlaylistItem *> *CDTracks;

 protected:
    FAContext *m_context;

    static void stream_timer_func(void *arg);

 private:
    MusicBrowserUI *parentUI;

    bool     stream_timer_started; 
    TimerRef stream_timer;
    GtkCTreeNode *StreamGetParentNode(string &treePath);

    uint32 CD_DiscID;
    uint32 CD_numtracks;
    bool   scheduleCDredraw;
    void   UpdateCDTree(PlaylistItem *update);
    void   RegenerateCDTree(void);

    bool m_bCDMode;

    bool isVisible;
    void UpdatePlaylistList(void);
    void AddPlaylistItems(vector<PlaylistItem *> *items);
    void RemovePlaylistItems(vector<uint32> *indices);
    void RenumberPlaylistList(int starting);
    void UpdatePlaylistItems(const vector<PlaylistItem *> *items);

    Properties *m_propManager;

    bool m_initialized;

    bool master;

    bool iSetRepeatMode;

    uint32 m_playingindex;

    bool m_bIgnoringMusicCatalogMessages;

    PlaylistManager *m_plm;
    MusicCatalog *m_musicCatalog;
 
    MusicBrowserView m_state;
    ClickState m_clickState;
    TreeNodeType m_mbState;

    int32 lastPanedPosition;
    int32 lastPanedHandle;

    /* Widget creation */
    void CreateExpanded(void);
    void CreateMenu(GtkWidget *);
    void CreateToolbar(GtkWidget *);
    void CreatePlaylistList(GtkWidget *);
    void CreatePlaylist(void);
    void CreateTree(void);

    GtkWidget *musicBrowser;
    GtkWidget *masterBox;
    GtkWidget *masterBrowserBox;
    GtkWidget *masterPlaylistBox;
    GtkWidget *musicBrowserWindow;
    GtkWidget *playlistList;
    GtkWidget *playlistOMenu;
    GtkWidget *playlistMenu;
    GtkWidget *addTrack;
    GtkWidget *addFile;
    GtkWidget *toolRemove;
    GtkWidget *toolUp;
    GtkWidget *toolDown;
    GtkWidget *toolbar;
    GtkWidget *playlistLabel;

    GtkItemFactory *menuFactory;
    GtkItemFactory *playlistPopup;
    GtkItemFactory *playlist2Popup;
    GtkItemFactory *genstreamPopup;
    GtkItemFactory *streamPopup;
    GtkItemFactory *favPopup;
    GtkItemFactory *cdPopup;
    GtkItemFactory *trackPopup;
    GtkItemFactory *artistalbumPopup;
    GtkItemFactory *playlistCatPopup;
    GtkItemFactory *otherPopup;
    
    GtkCTreeNode *mainTree;
    GtkCTreeNode *allTree;
    GtkCTreeNode *uncatTree;
    GtkCTreeNode *playlistTree;
    GtkCTreeNode *CDTree;
    GtkCTreeNode *streamTree;
    GtkCTreeNode *favoritesTree;
    GtkCTreeNode *streamSpace;

    bool streamExpanded;
 
    bool m_sigsExist;
    bool m_sigsStart;

    GtkWidget *NewPixmap(char **data);

    GtkStyle *normStyle;
    GtkStyle *boldStyle;
    GtkStyle *redStyle;
    GtkStyle *greenStyle;

    GdkPixmap *album_pmap;
    GdkBitmap *album_mask;
    GdkPixmap *artist_pmap;
    GdkBitmap *artist_mask;
    GdkPixmap *playlist_pmap;
    GdkBitmap *playlist_mask;
    GdkPixmap *track_pmap;
    GdkBitmap *track_mask;
    GdkPixmap *stream_pmap;
    GdkBitmap *stream_mask;
    
    void ChangeCurrentPlayingIndex(uint32 oldindex, uint32 newindex);

    void SetStatusText(const char *text);
    GtkWidget *statusBar;
    unsigned int statusContext;

    GtkWidget *artistSubTree;
    GtkWidget *playlistSubTree;
 
    PlaylistColumns playlistCols[9];
    bool playlistColsChanged;
    void ParsePlaylistCols();
    void UpdateColumnHeaders();

    void UpdatePlayPause();
    TreeData *NewTreeData(TreeNodeType type, MusicCatalog *cat = NULL, 
                          ArtistList *art = NULL, AlbumList *alb = NULL, 
                          PlaylistItem *tr = NULL, char *pname = NULL,
                          char *message = NULL,
                          vector<PlaylistItem *> *cdlist = NULL);

    void SetToolbarType();

    GtkCTreeNode *FindNode(TreeNodeType type, ArtistList *artist,
                           AlbumList *album, PlaylistItem *item,
                           GtkCTreeNode *searchFrom = NULL);
    GtkCTreeNode *FindStreamNode(PlaylistItem *item);
    GtkCTreeNode *FindPlaylistNode(const string &playlist);

    void CreateMainTreeItems(void);

    void ClearTree(void);
    void AddCatTrack(ArtistList *artist, AlbumList *album, PlaylistItem *item,
                     bool expand = false);
    void RemoveCatTrack(ArtistList *artist, AlbumList *album, 
                        PlaylistItem *item);
    void AddCatStream(PlaylistItem *item);
    void RemoveCatStream(PlaylistItem *item);
    void AddCatPlaylist(const string &playlist);
    void RemoveCatPlaylist(const string &playlist);
    void SetRepeatType(RepeatMode mode);
    void SetShuffleType(bool shuffled);
    bool CheckEmptyDatabase(void);
  
  public:

    vector<PlaylistItem *> *GetTreeSelection(void);
    bool iSetShuffleMode;

    ClickState GetClickState() { return m_clickState; }
    void SetClickState(ClickState newState);

    TreeNodeType GetTreeClick() { return m_mbState; }
    void SetTreeClick(TreeNodeType newState) { m_mbState = newState; }

    /* event callbacks */
    void ReadPlaylist(char *filename, vector<PlaylistItem *> *plist);

    void ExpandCollapseEvent();
    void ToggleVisEvent();

    void AddFileCMD();
    void DeleteListEvent();
    void DeleteEvent();
    bool AskToDelete(string &url);
    void DeletePlaylistItem(uint32 loc);
    void HandleKeypress(char key);
    void MoveUpEvent();
    void MoveDownEvent();
    void MoveItemEvent(int source, int dest);
    void AddTrackPlaylistEvent(char *path);
    void AddTrackPlaylistEvent(PlaylistItem *newitem);
    void AddTracksPlaylistEvent(vector<PlaylistItem *> *newlist, 
                                bool end = false, bool forcePlay = false,
                                bool forceNoPlay = false);
    void AddTracksDoubleClick(vector<PlaylistItem *> *newlist);
    void PlayEvent();
    void StartMusicSearch(bool runMain = true, bool intro = false);
    void SortPlaylistEvent(PlaylistSortKey order, PlaylistSortType type);
    void PopUpInfoEditor(PlaylistItem *editee = NULL);
    void SaveCurrentPlaylist(char *path = NULL);
    void LoadPlaylist(string &oPlaylist);
    void ImportPlaylist(char *path);

    void CreateNewEditor(char *playlisturl, bool cd_mode = false);

    void ShowOptions(int page = 0);
    void StopMenu(void);
    void PlayMenu(void);
    void NextMenu(void);
    void PrevMenu(void);
    void SetRepeat(int numrepeat);

    void PlaylistRightClick(int x, int y, uint32 time);
    void TreeRightClick(int x, int y, uint32 time);

    void CreateTreePopups(void);
    void EjectCD(void);
    void UpdateCD(void);

    void AddStreamToFavs(void);
    void AddPLStreamToFavs(void);
    void AddNewStream(void);
    void VisitWebsite(void);
    void TipArtist(PlaylistItem *tipee = NULL);
};

#endif
