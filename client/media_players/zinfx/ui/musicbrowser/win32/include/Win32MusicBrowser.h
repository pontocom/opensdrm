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

        $Id: Win32MusicBrowser.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_WIN32MUSICBROWSER_H_
#define INCLUDED_WIN32MUSICBROWSER_H_

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <windows.h>
#include <commctrl.h>
#include <dbt.h>

#include <vector>
#include <string>
#include <set>

using namespace std;

#include "config.h"
#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"
#include "musiccatalog.h"
#include "DataIndex.h"
#include "DropTarget.h"
#include "timer.h"
#include "ColumnInfo.h"

class FAContext;

#define STATE_COLLAPSED 0
#define STATE_EXPANDED  1

#define PLAYERSTATE_STOPPED 0
#define PLAYERSTATE_PLAYING 1
#define PLAYERSTATE_PAUSED 2

#define IDC_REBAR          13000
#define IDC_TOOLBAR        13001
#define TOOLBAR_INDENT	   8

#define ALBUM_COLUMN       "Album"
#define ARTIST_COLUMN      "Artist"
#define COMMENT_COLUMN     "Comment"
#define GENRE_COLUMN       "Genre"
#define LOCATION_COLUMN    "Location"
#define TIME_COLUMN        "Time"
#define TITLE_COLUMN       "Title"
#define YEAR_COLUMN        "Year"
#define TRACK_COLUMN       "Track"

bool operator<(const TreeData &A, const TreeData &b);
bool operator==(const TreeData &A, const TreeData &b);
void ClientToWindow(HWND hWnd, POINT *Pt); 
extern HINSTANCE g_hinst;

class TrackSort : public binary_function<PlaylistItem*, PlaylistItem*, bool> {

 public:
    TrackSort(){ }

    bool operator() (PlaylistItem* item1, PlaylistItem* item2) const;
};


LRESULT WINAPI 
ListViewWndProc(HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam);

LRESULT WINAPI 
TreeViewWndProc(HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam);

BOOL CALLBACK 
MusicSearchDlgProc(HWND hwnd, 
                   UINT msg, 
                   WPARAM wParam, 
                   LPARAM lParam);

BOOL CALLBACK 
SavePlaylistDlgProc(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam);

BOOL CALLBACK 
RemoveTracksDlgProc(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam);

BOOL CALLBACK 
PerformDBCheckDlgProc(HWND hwnd, 
                      UINT msg, 
                      WPARAM wParam, 
                      LPARAM lParam );

class MusicBrowserUI : public UserInterface 
{
 public:

             MusicBrowserUI(FAContext      *context, 
                            MusicBrowserUI *parent,
                            HWND            hParent,
                            const string   &oPlaylistName);

             MusicBrowserUI(FAContext      *context, 
                            MusicBrowserUI *parent,
                            HWND            hParent,
                            DeviceInfo     *pDevice);

    virtual ~MusicBrowserUI();

    virtual Error AcceptEvent(Event *);
    virtual Error Init(int32);
						 
    static void UIThreadFunc(void* arg);
    void   InitDialog(HWND hwnd);
    void   SetMinMaxInfo();
    void   EditItemLabel(HWND hwnd, HTREEITEM item);

    bool   IntroductionWizard(vector<string>* searchPaths, 
                              APSInterface *pInterface);

    BOOL   DialogProc(HWND hwnd, UINT msg, 
                      WPARAM wParam, LPARAM lParam);


    LRESULT TreeViewWndProc(HWND hwnd, 
                            UINT msg, 
                            WPARAM wParam, 
                            LPARAM lParam);

    LRESULT ListViewWndProc(HWND hwnd, 
                            UINT msg, 
                            WPARAM wParam, 
                            LPARAM lParam);

    BOOL MusicSearchDlgProc(HWND hwnd, 
                            UINT msg, 
                            WPARAM wParam, 
                            LPARAM lParam);   
    
    bool FileOpenDialog(HWND hwnd, 
                    const char* title,
                    const char* filter,
                    vector<string>* fileList,
                    bool allowURL);
    
    const PlaylistManager* PLManager() const { return m_plm; }

 protected:
    FAContext *m_context;

 private:

    void   Init();

    // Functions in Dialog.cpp
    void   ShowBrowser(bool bShowExpanded);
    void   HideBrowser();
    void   Close();
    void   Destroy();
	void   ExpandCollapseEvent();
    void   GetMinMaxInfo(MINMAXINFO *pInfo);
    void   SizeWindow(int type, int iWidth, int iHeight);
    BOOL   SetCursor(int hitTest, int mouseMsg);
    void   DropFiles(HDROP dropHandle, bool filesAreURLs);
    void   MouseMove(uint32 uFlags, POINT &sPoint);
    void   MouseButtonDown(int keys, int x, int y);
    void   MouseButtonUp(int keys, int x, int y);
    BOOL   DrawItem(int32 controlId, DRAWITEMSTRUCT* dis);
    void   SetStatusText(const char *text);
    void   CreateToolbar();
    void   AddToolbarButtons(bool textLabels, bool images);
    void   ToggleVisEvent();
    void   SetTitles();
    void   UpdateMenuStates();
    void   UpdateButtonStates();
    void   MoveControls(int iPixelsToMove);
    bool   CreateMainDialog();
    Error  CloseMainDialog();
    uint32 CalcStringEllipsis(HDC hdc, string& displayString, 
                                 int32 columnWidth);

    // Functions in OpenSavePlaylist.cpp
    void OpenPlaylist();
    void NewPlaylist();
    void SavePlaylist();
    void SavePlaylistAs();
    void ImportTracksAndPlaylists();
    void AddPlaylist(const string &oName);
    void LoadPlaylist(const string &oPlaylist);
    bool SaveNewPlaylist(string &oName);
    void EditPlaylist(const string &oList);
    bool ExportPlaylist(string &oName);
    void EditPortablePlaylist(DeviceInfo* device);
    void SavePortablePlaylist();

    // Functions in Event.cpp
    int   Notify(WPARAM wParam, NMHDR* pNMHDR);
    void  StartStopMusicSearch(bool useWizard = false);
    void  MoveUpEvent();
    void  MoveDownEvent();
    void  AddTrackEvent();
    void  VisitWebEvent(void);
    void  AddTrackAndPlayEvent();
    void  AddFileEvent(HWND hwndParent, bool playNow = false);
    void  EditPlaylistEvent();
    void  ClearPlaylistEvent();
    void  SortEvent(int id);
    void  EmptyDBCheck();
    void  RemoveEvent();
    void  ImportEvent();
    void  MoveItemEvent(int source, int dest);
    void  PlayerControlsEvent(int command);
    void  ChangeShuffleMode(bool shuffled);
    void  ChangePlayerState(int32 event);
    void  ChangeRepeatMode(RepeatMode mode);
    void  ExportPlaylistEvent();
    void  EditInfoEvent();
    void  EditInfo(vector<PlaylistItem*>& items, HWND hwnd = NULL);
    void  RenameEvent();
    void  PlayNowEvent();
    bool  DeleteFromDrive(const char* url);
    void  EjectCDEvent();
    void  AddFavoriteEvent();
    void  NewFavoriteEvent();
    void  EditStreamInfoEvent();
    void  RemoveFavoriteEvent();
	void  TipEvent(PlaylistItem *item = NULL);

    // Functions in PlaylistView.cpp
    void  PlaylistListItemAdded(const PlaylistItem* item);
    void  PlaylistListItemsAdded(const vector<PlaylistItem*>* items);
    void  PlaylistListItemsUpdated(const vector<PlaylistItem*>* items);
    void  PlaylistListItemMoved(const PlaylistItem* item, 
                                uint32 oldIndex, uint32 newIndex);
    void  PlaylistListItemRemoved(const vector<PlaylistItem*>* itemList, 
                                  const vector<uint32>* oldIndexList);
    void  PlaylistListSorted();
    void  InitList();
    
    void  InsertColumn(const char *title, int position);
    void  RemoveAllColumns();
    void  ResizeColumns();

    void  LVBeginDrag(HWND hwnd, NM_LISTVIEW* nmlv);
    void  UpdateTotalTime();
    void  GetSelectedPlaylistItems(vector<PlaylistItem*>* items);
    void  ResizeHeader(HWND hwnd, uint32 column);
    void  DisplayCurrentItem();
    int   GetColumnIndex(const char *columnTitle);
    const char *GetColumnText(int column);


    // Functions in Win32MusicBrowser.cpp
    void  AddMusicBrowserWindow(MusicBrowserUI* pWindow);
    void  RemoveMusicBrowserWindow(MusicBrowserUI* pWindow);
    void  MusicSearchDone();
    void  DisplayBrowserMessage(const char* msg);
    void  SaveCurrentPlaylist();

    // Functions is MusicTree.cpp
    void    InitTree();
    void    FillArtists();
    void    FillAlbums(TV_ITEM* pItem);
    void    FillPlaylists();
    void    FillTracks(TV_ITEM* pItem);
	static void fill_all_tracks(void* arg);
    void    FillAllTracks();
    void    FillUncatTracks();
    void    FillPortables();
    void    FillStreams();
    HTREEITEM MusicBrowserUI::CreateStreamFolder(string& treePath);
    void    FillFavorites();
    int32   GetCurrentItemFromMousePos();
    void    GetSelectedMusicTreeItems(vector<PlaylistItem*>* items);
    BOOL    FindSelectedItems(HTREEITEM root, vector<PlaylistItem*>* items);
    void    AddTrackItems(TV_ITEM* tv_item, vector<PlaylistItem*>* items);
    void    AddAllTrackItems(vector<PlaylistItem*>* items);
    void    AddUncatagorizedTrackItems(vector<PlaylistItem*>* items);
    void    GetSelectedPlaylistItems(vector<string>* urls);
    void    GetSelectedStreamItems(vector<PlaylistItem*>* items, HTREEITEM root = NULL);
    void    GetSelectedCDItems(vector<string>* urls);
    void    GetSelectedFavoritesItems(vector<PlaylistItem*>* items);
    void    TVBeginDrag(HWND hwnd, NM_TREEVIEW* nmtv);
    void    MusicCatalogTrackChanged(const ArtistList *oldArtist,
                                     const ArtistList *newArtist,
                                     const AlbumList *oldAlbum,
                                     const AlbumList *newAlbum,
                                     const PlaylistItem *oldItem,
                                     const PlaylistItem *newItem);
    void    MusicCatalogTrackAdded(const ArtistList* artist,
                                   const AlbumList* album,
                                   const PlaylistItem* item);
    void    MusicCatalogPlaylistAdded(string item);
    void    MusicCatalogTrackRemoved(const ArtistList* artist,
                                     const AlbumList* album,
                                     const PlaylistItem* item);
    void    MusicCatalogPlaylistRemoved(string item);
    void    MusicCatalogCleared();
    void    MusicCatalogStreamAdded(const PlaylistItem* item);
    void    MusicCatalogStreamRemoved(const PlaylistItem* item);

    HTREEITEM FindArtist(const ArtistList* artist);
    HTREEITEM FindAlbum(HTREEITEM artistItem, const AlbumList* album);
    HTREEITEM FindTrack(HTREEITEM albumItem, const PlaylistItem* track);
    HTREEITEM FindPlaylist(const string playlist);
    HTREEITEM MusicBrowserUI::FindFavorite(const PlaylistItem* stream);

    void UpdateTrackName(PlaylistItem* track, const char* name);
    void UpdatePlaylistName(const string& playlist, const char* name);
    void UpdateAlbumName(AlbumList* album, const char* name);
    void UpdateArtistName(ArtistList* artist, const char* name);
    void UpdateUncatagorizedTrackName(PlaylistItem* track, const char* name);

    uint32 GetSelectedTrackCount();
    uint32 GetSelectedPlaylistCount();
    uint32 GetSelectedStreamCount();
    uint32 CountSelectedItems(HTREEITEM root);
    bool IsItemSelected(HTREEITEM item);

    void RefreshCDList(vector<PlaylistItem*>* tracks);
    void DeviceChanged(uint32 event, PDEV_BROADCAST_HDR data);
    void CheckForCD();

    // Functions in EditTrackInfoDialog.cpp
    void CreateEditInfoLists(set<string>& artists,
                             set<string>& albums,
                             set<string>& genres);
    

    // Data members
    EventQueue*         m_playerEQ;
    int32               m_state, m_startupType, m_playerState;
    int32               m_currentplaying;
  	HWND                m_hWnd, m_hStatus, m_hParent, m_hRebar;
    HWND                m_hMusicView, m_hPlaylistView;
    HWND                m_hPlaylistTitle, m_hMusicViewTitle;
    PlaylistManager*    m_plm;
    bool                m_initialized, isVisible, m_bListChanged, 
                        m_bSearchInProgress, m_bDragging;
    string              m_currentListName, m_activeListName;
    Thread*             m_uiThread;
    POINT               m_sMinSize;
    HTREEITEM	        m_hPlaylistItem, m_hMyMusicItem, m_hPortableItem;
    HTREEITEM           m_hAllItem, m_hUncatItem, m_hStreamsItem, m_hCDItem;
    HTREEITEM           m_hNewPlaylistItem, m_hNewPortableItem;
    HTREEITEM           m_hFavoritesItem, m_hNewFavoritesItem;
    int                 m_iCollapseMoveAmount;
    HCURSOR             m_hSavedCursor, m_hDragCursor, m_hNoDropCursor;
    HCURSOR             m_hSplitterCursor, m_hPointerCursor, m_hCurrentCursor;
    MusicBrowserUI*     m_pParent;
    vector<MusicBrowserUI*> m_oWindowList;
    bool                m_overSplitter;
    bool                m_trackSplitter;
    RECT                m_splitterRect;
    HBRUSH              m_splitterBrush;

    HWND                m_hPlaylistHeader;
    HWND                m_hToolbar, m_hTextToolbar, m_hImageToolbar, m_hBothToolbar;
    DropTarget*         m_playlistDropTarget;
    vector<string>      m_searchPathList;
    
    uint32              m_initialCount;
    uint32              m_itemsAddedBeforeWeWereCreated;

    DeviceInfo*         m_portableDevice;

    bool                m_autoPlayHack;

    vector<PlaylistItem*>* m_cdTracks;

    uint32              m_cdId;
    TimerRef            m_streamsTimer;
    Thread*             m_fillAllThread;

    ColumnInfo          m_columnInfo;
    // cached.
    string              m_columnCache;
};

#endif

