/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 2000 EMusic.com

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
	
    $Id: GTKPreferenceWindow.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include "config.h"

#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "player.h"
#include "eventdata.h"
#include "GTKPreferenceWindow.h"
#include "GTKWindow.h"
#include "GTKFileSelector.h"
#include "MessageDialog.h"
#include "help.h"

GTKPreferenceWindow::GTKPreferenceWindow(FAContext *context,
                                         ThemeManager *pThemeMan,
                                         uint32 defaultPage) :
     PreferenceWindow(context, pThemeMan)
{    
    startPage = defaultPage;
    done = false;
    visiblePane = NULL;
    m_PMOnames = new vector<string>;
    paneList = new vector<OptionsPane *>;
}

GTKPreferenceWindow::~GTKPreferenceWindow(void)
{
    if (m_PMOnames) 
        delete m_PMOnames;
    if (paneList) {
        while (paneList->size() > 0) {
            paneList->erase(paneList->begin());
        }
        delete paneList;
    }
} 

static gboolean pref_destroy(GtkWidget *widget, GTKPreferenceWindow *p)
{
    p->done = true;
    return FALSE;
}

void GTKPreferenceWindow::ApplyInfo(void)
{
    if (proposedValues != currentValues) 
        SavePrefsValues(m_pContext->prefs, &proposedValues);
}

static void pref_ok_click(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->ApplyInfo();
    gtk_widget_destroy(p->mainWindow);
    p->done = true;
}

static void pref_apply_click(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->ApplyInfo();
}

void GTKPreferenceWindow::CancelInfo(void)
{
    if (currentValues != originalValues) 
        SavePrefsValues(m_pContext->prefs, &originalValues);
}

static void pref_cancel_click(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->CancelInfo();
    gtk_widget_destroy(p->mainWindow);
    p->done = true;
}

void GTKPreferenceWindow::ShowHelp(void)
{
    if (!::ShowHelp(m_pContext, Preferences_General))
    {
        MessageDialog oBox(m_pContext);
        string oMessage("Cannot find the help files. Please make sure that the help files are properly installed, and you are not running "the_BRANDING" from the build directory.");
        oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk, true);
    }
}

static void help_click(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->ShowHelp();
}

void GTKPreferenceWindow::SetPane(OptionsPane *pane)
{
    if (visiblePane == pane->m_pane)
        return;

    if (visiblePane)
        gtk_widget_hide(visiblePane);

    gtk_widget_show(pane->m_pane);
    visiblePane = pane->m_pane;

    gtk_label_set_text(GTK_LABEL(paneLabel), pane->m_description.c_str());
}

void GTKPreferenceWindow::SetPane(uint32 panenum)
{
    if (paneList->size() < panenum)
        return;

    SetPane((*paneList)[panenum]);
}

static void pref_tree_clicked(GtkWidget *widget, GdkEventButton *event,
                              GTKPreferenceWindow *p)
{
    if (!event)
        return;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_CTREE(widget));
    g_return_if_fail(event != NULL);

    GtkCTree *ctree = GTK_CTREE(widget);
    GtkCList *clist = GTK_CLIST(widget);

    if (event->window != clist->clist_window)
        return;

    int row, column;
    if (!gtk_clist_get_selection_info(clist, (int)event->x, (int)event->y, 
                                      &row, &column))
        return;

    GtkCTreeNode *node = GTK_CTREE_NODE(g_list_nth(clist->row_list, row));
    
    OptionsPane *opane = (OptionsPane *)gtk_ctree_node_get_row_data(ctree, 
                                                                    node);

    p->SetPane(opane);
}

static void kill_optionspane(OptionsPane *die)
{
    gtk_widget_hide(die->m_pane);
    gtk_widget_destroy(die->m_pane);
    delete die;
}

void GTKPreferenceWindow::AddPane(OptionsPane *pane)
{
    GtkCTreeNode *node;

    char *name[1];
    name[0] = (char *)pane->m_label.c_str();
    node = gtk_ctree_insert_node(prefTree, NULL, NULL, name, 5, NULL, NULL,
                                 NULL, NULL, true, false);
    gtk_ctree_node_set_row_data_full(prefTree, node, pane, 
                                     (GtkDestroyNotify)kill_optionspane);

    gtk_widget_hide(pane->m_pane);
    gtk_box_pack_start(GTK_BOX(paneVbox), pane->m_pane, FALSE, FALSE, 5);

    paneList->push_back(pane);
}

bool GTKPreferenceWindow::Show(Window *pWindow)
{
    GetPrefsValues(m_pContext->prefs, &originalValues);
    GetPrefsValues(m_pContext->prefs, &proposedValues);
    GetPrefsValues(m_pContext->prefs, &currentValues);     

    fontDialog = NULL;

    gdk_threads_enter();

    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_modal(GTK_WINDOW(mainWindow), TRUE);
    gtk_signal_connect(GTK_OBJECT(mainWindow), "destroy",
                       GTK_SIGNAL_FUNC(pref_destroy), this);
    gtk_window_set_title(GTK_WINDOW(mainWindow), BRANDING" - Preferences");

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(mainWindow), vbox);
    gtk_widget_show(vbox);

    GtkWidget *mainHbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(mainHbox), 5);
    gtk_container_add(GTK_CONTAINER(vbox), mainHbox);
    gtk_widget_show(mainHbox);

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                   GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_box_pack_start(GTK_BOX(mainHbox), scrolledWindow, TRUE, TRUE, 0);
    gtk_widget_set_usize(scrolledWindow, 140, 300);
    gtk_widget_show(scrolledWindow);

    const char *name[1];
    name[0] = "Category";

    prefTree = GTK_CTREE(gtk_ctree_new_with_titles(1, 0, (gchar **)name));
    gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(prefTree));
    gtk_signal_connect(GTK_OBJECT(prefTree), "button_press_event",
                       GTK_SIGNAL_FUNC(pref_tree_clicked), this);
    gtk_clist_set_row_height(GTK_CLIST(prefTree), 16);
    gtk_clist_column_titles_passive(GTK_CLIST(prefTree));
    gtk_widget_show(GTK_WIDGET(prefTree));

    paneVbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(paneVbox), 5);
    gtk_box_pack_start(GTK_BOX(mainHbox), paneVbox, FALSE, FALSE, 0);
    gtk_widget_show(paneVbox);

    paneStyle = gtk_style_copy(gtk_widget_get_style(mainWindow));
    GdkColor temp = paneStyle->bg[GTK_STATE_NORMAL];
    paneStyle->bg[GTK_STATE_NORMAL] = paneStyle->fg[GTK_STATE_NORMAL];
    paneStyle->fg[GTK_STATE_NORMAL] = temp;
    GdkFont *font = 
            gdk_font_load("-adobe-helvetica-bold-r-normal--*-180-*-*-*-*-*-*");
    if (font) {
        gdk_font_unref(paneStyle->font);
        paneStyle->font = font;
    }

    GtkWidget *stupid_gtk = gtk_event_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(stupid_gtk), 0);
    gtk_box_pack_start(GTK_BOX(paneVbox), stupid_gtk, FALSE, FALSE, 0);
    gtk_widget_set_style(stupid_gtk, paneStyle);
    gtk_widget_show(stupid_gtk);

    paneLabel = gtk_label_new(NULL); 
    gtk_container_add(GTK_CONTAINER(stupid_gtk), paneLabel);
    gtk_widget_set_style(paneLabel, paneStyle);
    gtk_label_set_justify(GTK_LABEL(paneLabel), GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment(GTK_MISC(paneLabel), 0.0, 0.5);
    gtk_widget_realize(paneLabel);

    gtk_widget_show(paneLabel);
    
    GtkWidget *pane;
    OptionsPane *opane;

    pane = CreateGeneral();
    opane = new OptionsPane("General", " General Preferences", 0, pane);
    AddPane(opane);

    pane = CreateThemes();
    opane = new OptionsPane("Themes", " Theme Preferences", 1, pane);
    AddPane(opane);

    pane = CreateDirectories();
    opane = new OptionsPane("Directories", "Directory Preferences", 2, pane);
    AddPane(opane);

    pane = CreateStreaming();
    opane = new OptionsPane("Streaming", " Stream Preferences", 3, pane);
    AddPane(opane);

    pane = CreatePlugins();
    opane = new OptionsPane("Plugins", " Plugin Preferences", 4, pane);
    AddPane(opane);

    pane = CreateCD();
    opane = new OptionsPane("MusicBrainz", "MusicBrainz Preferences", 5, pane);
    AddPane(opane);

    pane = CreateAdvanced();
    opane = new OptionsPane("Advanced", " Advanced Preferences", 6, pane);
    AddPane(opane);

    pane = CreatePlaylistHeaders();
    opane = new OptionsPane("Playlist", "Playlist Preferences", 8, pane);
    AddPane(opane);

    pane = CreateAbout();
    opane = new OptionsPane("About", " About "The_BRANDING, 9, pane);
    AddPane(opane);

    GtkWidget *separator = gtk_hseparator_new();
    gtk_container_add(GTK_CONTAINER(vbox), separator);
    gtk_widget_show(separator);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);
    gtk_widget_show(hbox);

    GtkWidget *button;

    button = gtk_button_new_with_label("  Help  ");
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", 
                       GTK_SIGNAL_FUNC(help_click), this);
    gtk_widget_show(button);

    applyButton = gtk_button_new_with_label("  Apply  ");
    gtk_signal_connect(GTK_OBJECT(applyButton), "clicked",
                       GTK_SIGNAL_FUNC(pref_apply_click), this);
    gtk_box_pack_end(GTK_BOX(hbox), applyButton, FALSE, FALSE, 0);
    gtk_widget_show(applyButton);
    gtk_widget_set_sensitive(applyButton, FALSE);

    button = gtk_button_new_with_label("  Cancel  ");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(pref_cancel_click), this);
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("  OK  ");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(pref_ok_click), this);
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_widget_show(button);

    SetPane(startPage);

    gtk_widget_show(mainWindow);

    firsttime = false;

    gdk_threads_leave();

    while (!done) 
        usleep(20);

    return true;
}

void GTKPreferenceWindow::GetPrefsValues(Preferences* prefs, 
                                         PrefsStruct* values)
{
    uint32 bufferSize = 1;
    uint32 size;

    char *buffer = (char *)malloc(bufferSize);

    size = bufferSize;

    if (kError_BufferTooSmall == prefs->GetPrefString(kPMOPref, buffer, &size)) {
        buffer = (char *)realloc(buffer, size);
        prefs->GetPrefString(kPMOPref, buffer, &size);
    }
    values->defaultPMO = buffer;
    size = bufferSize;

    prefs->GetPrefInt32(kInputBufferSizePref, &values->inputBufferSize);
    prefs->GetPrefInt32(kOutputBufferSizePref, &values->outputBufferSize);
    prefs->GetPrefInt32(kPreBufferPref, &values->preBufferLength);

    prefs->GetPrefBoolean(kSaveStreamsPref, &values->saveStreams);
    
    if (kError_BufferTooSmall == prefs->GetPrefString(kProxyHostPref, buffer, &size)) {
        buffer = (char *)realloc(buffer, size);
        prefs->GetPrefString(kProxyHostPref, buffer, &size);
    }
    values->proxyServer = buffer;

    prefs->GetPrefBoolean(kUseProxyPref, &values->useProxyServer);

    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kSaveStreamsDirPref, buffer, &size)) {
        size++;
        buffer = (char*)realloc(buffer, size);
        prefs->GetPrefString(kSaveStreamsDirPref, buffer, &size);
    }
    values->saveStreamsDirectory = buffer;
    
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kAlternateNICAddressPref, buffer, &size)) {
        buffer = (char*)realloc(buffer, size);
        prefs->GetPrefString(kAlternateNICAddressPref, buffer, &size);
    }

    values->alternateIP = buffer;
    size = bufferSize;

    prefs->GetPrefBoolean(kUseAlternateNICPref, &values->useAlternateIP);

    prefs->GetPrefBoolean(kUseDebugLogPref, &values->enableLogging);
    prefs->GetPrefBoolean(kLogMainPref, &values->logMain);
    prefs->GetPrefBoolean(kLogDecodePref, &values->logDecoder);
    prefs->GetPrefBoolean(kLogInputPref, &values->logInput);
    prefs->GetPrefBoolean(kLogOutputPref, &values->logOutput);
    prefs->GetPrefBoolean(kLogPerformancePref, &values->logPerformance);

    if(kError_BufferTooSmall == prefs->GetPrefString(kThemeDefaultFontPref, buffer, &size)) {
        size++;
        buffer = (char*)realloc(buffer, size);
        prefs->GetPrefString(kThemeDefaultFontPref, buffer, &size);
    }

    values->defaultFont = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kSaveMusicDirPref, buffer, &size)) {
        buffer = (char*)realloc(buffer, size);
        prefs->GetPrefString(kSaveMusicDirPref, buffer, &size);
    }
    values->saveMusicDirectory = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size);
    }
    values->playlistHeaders = buffer;
    size = bufferSize;

    m_pThemeMan->GetCurrentTheme(values->currentTheme);

    prefs->GetPrefBoolean(kShowToolbarTextLabelsPref, &values->useTextLabels);
    prefs->GetPrefBoolean(kShowToolbarImagesPref, &values->useImages);
    prefs->GetPrefBoolean(kSaveCurrentPlaylistOnExitPref, &values->savePlaylistOnExit);
    prefs->GetPrefBoolean(kPlayImmediatelyPref, &values->playImmediately);
    
    prefs->GetPrefBoolean(kSetLastResumePref, &values->setLastResume);
    
    prefs->GetPrefBoolean(kAllowMultipleInstancesPref, &values->allowMultipleInstances);

    if(kError_BufferTooSmall == prefs->GetPrefString(kSaveMusicDirPref, buffer, &size))
    {
        size++;
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kSaveMusicDirPref, buffer, &size);
    }

    values->saveMusicDirectory = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kALSADevicePref, 
                                                     buffer, &size)) {
        size++;
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kALSADevicePref, buffer, &size);
    }
 
    values->alsaOutput = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kWatchThisDirectoryPref,
                                                     buffer, &size)) {
        size++;
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kWatchThisDirectoryPref, buffer, &size);
    }

    values->watchThisDirectory = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kCDDevicePathPref,
                                                     buffer, &size)) {
        size++;
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kCDDevicePathPref, buffer, &size);
    }

    values->CDDevicePath = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kMBServerPref,
                                                     buffer, &size)) {
        size++;
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        prefs->GetPrefString(kMBServerPref, buffer, &size);
    }

    values->MBServer = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == prefs->GetPrefString(kUsersPortablePlayersPref, buffer, &size)) {
        buffer = (char*)realloc(buffer, size);
        prefs->GetPrefString(kUsersPortablePlayersPref, buffer, &size);
    }

    char* cp = buffer;
    char* name = cp;

    while ((cp = strchr(cp, ';'))) {
        *cp = 0x00;
        values->portablePlayers.insert(string(name));
        cp++;
        name = cp;
    }

    if (*name) 
        values->portablePlayers.insert(string(name));

    free(buffer);

    prefs->GetPrefBoolean(kAskToReclaimFiletypesPref, &values->askReclaimFiletypes);
    prefs->GetPrefBoolean(kReclaimFiletypesPref, &values->reclaimFiletypes);
    prefs->GetPrefInt32(kWatchThisDirTimeoutPref, &values->watchThisDirTimeout);
    prefs->GetPrefBoolean(kCheckCDAutomaticallyPref, &values->pollCD);
}

void GTKPreferenceWindow::SavePrefsValues(Preferences* prefs, 
                                          PrefsStruct* values)
{
    prefs->SetPrefBoolean(kCheckCDAutomaticallyPref, values->pollCD);
    prefs->SetPrefBoolean(kShowToolbarTextLabelsPref, values->useTextLabels);
    prefs->SetPrefBoolean(kShowToolbarImagesPref, values->useImages);
    prefs->SetPrefBoolean(kSaveCurrentPlaylistOnExitPref, values->savePlaylistOnExit);
    prefs->SetPrefBoolean(kPlayImmediatelyPref, values->playImmediately);
    
    prefs->SetPrefBoolean(kSetLastResumePref, values->setLastResume);
    
    prefs->SetPrefBoolean(kAllowMultipleInstancesPref, values->allowMultipleInstances);
    prefs->SetPrefBoolean(kAskToReclaimFiletypesPref, values->askReclaimFiletypes);
    prefs->SetPrefBoolean(kReclaimFiletypesPref, values->reclaimFiletypes);

    prefs->SetPrefString(kPMOPref, values->defaultPMO.c_str());
    prefs->SetPrefInt32(kInputBufferSizePref, values->inputBufferSize);
    prefs->SetPrefInt32(kOutputBufferSizePref, values->outputBufferSize);
    prefs->SetPrefInt32(kPreBufferPref, values->preBufferLength);

    prefs->SetPrefBoolean(kSaveStreamsPref, values->saveStreams);
    prefs->SetPrefString(kSaveStreamsDirPref, values->saveStreamsDirectory.c_str());
    prefs->SetPrefString(kProxyHostPref, values->proxyServer.c_str());
    prefs->SetPrefBoolean(kUseProxyPref, values->useProxyServer);
    prefs->SetPrefString(kAlternateNICAddressPref, values->alternateIP.c_str());
    prefs->SetPrefBoolean(kUseAlternateNICPref, values->useAlternateIP);

    prefs->SetPrefString(kPlaylistHeaderColumnsPref, values->playlistHeaders.c_str());

    prefs->SetPrefBoolean(kUseDebugLogPref, values->enableLogging);
    prefs->SetPrefBoolean(kLogMainPref, values->logMain);
    prefs->SetPrefBoolean(kLogDecodePref, values->logDecoder);
    prefs->SetPrefBoolean(kLogInputPref, values->logInput);
    prefs->SetPrefBoolean(kLogOutputPref, values->logOutput);
    prefs->SetPrefBoolean(kLogPerformancePref, values->logPerformance);

    prefs->SetPrefString(kThemeDefaultFontPref, values->defaultFont.c_str());

    prefs->SetPrefString(kALSADevicePref, values->alsaOutput.c_str());
    prefs->SetPrefString(kCDDevicePathPref, values->CDDevicePath.c_str());
    prefs->SetPrefString(kMBServerPref, values->MBServer.c_str());

    map<string, string>::iterator i;
    int32 iLoop = 0;

    for (i = m_oThemeList.begin(); i != m_oThemeList.end(); i++, iLoop++) {
         if (iLoop == values->listboxIndex)
             values->currentTheme = (*i).first;
    }
    m_pThemeMan->UseTheme(m_oThemeList[values->currentTheme]);

    prefs->SetPrefString(kSaveMusicDirPref, values->saveMusicDirectory.c_str());
    prefs->SetPrefString(kWatchThisDirectoryPref, values->watchThisDirectory.c_str());
    prefs->SetPrefInt32(kWatchThisDirTimeoutPref, values->watchThisDirTimeout);

    PortableSet::const_iterator j = values->portablePlayers.begin();
    string portableList;

    for (; j != values->portablePlayers.end(); j++) {
        portableList += *j;
        portableList += ";";
    }
    prefs->SetPrefString(kUsersPortablePlayersPref, portableList.c_str());

    prefs->SetPrefString(kPMOPref, values->defaultPMO.c_str());
    
    if (*values != currentValues) {
        m_pContext->target->AcceptEvent(new Event(INFO_PrefsChanged));
        currentValues = proposedValues = *values;
    }

    m_pContext->prefs->Save();
}

void GTKPreferenceWindow::SetToolbar(bool text, bool pics)
{
    proposedValues.useTextLabels = text;
    proposedValues.useImages = pics;
    if (!firsttime) 
        gtk_widget_set_sensitive(applyButton, TRUE);
}

static void text_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetToolbar(true, false);
}

static void images_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetToolbar(false, true);
}

static void both_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetToolbar(true, true);
}

void GTKPreferenceWindow::SaveOnExitToggle(int active)
{
    proposedValues.savePlaylistOnExit = active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}
    
static void save_onexit_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->SaveOnExitToggle(i);
}

void GTKPreferenceWindow::PlayImmediatelyToggle(int active)
{
    proposedValues.playImmediately = !active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

static void play_now_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->PlayImmediatelyToggle(i);
}

void GTKPreferenceWindow::SetLastResumeToggle(int active)
{
    proposedValues.setLastResume = active;
    if(!firsttime)
	gtk_widget_set_sensitive(applyButton,TRUE);
}

static void set_last_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->SetLastResumeToggle(i);
}

void GTKPreferenceWindow::AllowMultipleToggle(int active)
{
    proposedValues.allowMultipleInstances = active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

static void allow_multiple_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->AllowMultipleToggle(i);
}

void GTKPreferenceWindow::ConvertUnderscoresToggle(int active)
{
    proposedValues.convertUnderscores = active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

static void convert_underscores_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->AllowMultipleToggle(i);
}

void GTKPreferenceWindow::ReclaimTypesToggle(int active)
{
    proposedValues.reclaimFiletypes = active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

void reclaim_types_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->ReclaimTypesToggle(i);
}

void GTKPreferenceWindow::AskReclaimToggle(int active)
{
    proposedValues.askReclaimFiletypes = active;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

void ask_reclaim_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->AskReclaimToggle(i);
}

GtkWidget *GTKPreferenceWindow::CreateGeneral(void)
{
    firsttime = true;

    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *frame = gtk_frame_new("Show 'My Music' Toolbars As");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    GtkWidget *temphbox = gtk_hbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(temphbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), temphbox);
    gtk_widget_show(temphbox);

    bool setSomething = false;

    GtkWidget *button = gtk_radio_button_new_with_label(NULL, "Text Only");
    if (originalValues.useTextLabels && !originalValues.useImages) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(text_selected), this);
    gtk_widget_show(button);

    button = gtk_radio_button_new_with_label(
                             gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                             "Images Only");
    if (!originalValues.useTextLabels && originalValues.useImages) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(images_selected), this);
    gtk_widget_show(button);


    button = gtk_radio_button_new_with_label(
                             gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                             "Text and Images");
    if (originalValues.useTextLabels && originalValues.useImages) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething  = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(both_selected), this);
    gtk_widget_show(button);

    if (!setSomething)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    frame = gtk_frame_new("Playlist");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    GtkWidget *check = gtk_check_button_new_with_label("Save current playlist when exiting the application");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(save_onexit_toggle), this);
    if (originalValues.savePlaylistOnExit)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);
   
    check = gtk_check_button_new_with_label("By default queue tracks rather than play them immediately");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(play_now_toggle), this);
    if (!originalValues.playImmediately) 
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    check = gtk_check_button_new_with_label("Open File to Last Selected");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE,FALSE,0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
		       GTK_SIGNAL_FUNC(set_last_toggle), this);
    if(originalValues.setLastResume)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    frame = gtk_frame_new("Miscellaneous");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    check = gtk_check_button_new_with_label("Allow multiple instances of "the_BRANDING);
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(allow_multiple_toggle), this);
    if (originalValues.allowMultipleInstances)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    check = gtk_check_button_new_with_label("Convert underscores to spaces");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(convert_underscores_toggle), this);
    if (originalValues.convertUnderscores)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    frame = gtk_frame_new("File Associations");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    check = gtk_check_button_new_with_label("Reclaim music file associations when application starts");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(reclaim_types_toggle), this);
    if (originalValues.reclaimFiletypes)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    check = gtk_check_button_new_with_label("Ask before reclaiming music file associations");
    gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(ask_reclaim_toggle), this);
    if (originalValues.askReclaimFiletypes)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    return pane;
}

void GTKPreferenceWindow::SaveLocalToggle(int active)
{
    if (!firsttime) {
        gtk_widget_set_sensitive(saveStreamLabel, active);
        gtk_widget_set_sensitive(saveStreamBox, active);
        gtk_widget_set_sensitive(saveBrowseBox, active);
        proposedValues.saveStreams = active;
        gtk_widget_set_sensitive(applyButton, TRUE);
    }
}

static void save_local_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->SaveLocalToggle(i);
}

void GTKPreferenceWindow::ProxyToggle(int active)
{
    if (!firsttime) {
        gtk_widget_set_sensitive(proxyAddyBox, active);
        gtk_widget_set_sensitive(proxyPortBox, active);
        gtk_widget_set_sensitive(proxyColon, active);
        gtk_widget_set_sensitive(proxyAddyLabel, active);
        gtk_widget_set_sensitive(proxyPortLabel, active);
        proposedValues.useProxyServer = active;
        gtk_widget_set_sensitive(applyButton, TRUE);
    }
}

static void proxy_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->ProxyToggle(i);
}

void GTKPreferenceWindow::AltIPToggle(int active)
{
    if (!firsttime) {
        gtk_widget_set_sensitive(ipLabel, active);
        gtk_widget_set_sensitive(ipOneBox, active);
        gtk_widget_set_sensitive(ipPeriod1, active);
        gtk_widget_set_sensitive(ipTwoBox, active);
        gtk_widget_set_sensitive(ipPeriod2, active);
        gtk_widget_set_sensitive(ipThreeBox, active);
        gtk_widget_set_sensitive(ipPeriod3, active);
        gtk_widget_set_sensitive(ipFourBox, active);
        proposedValues.useAlternateIP = active;
        gtk_widget_set_sensitive(applyButton, TRUE);
    }
}

static void alt_ip_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->AltIPToggle(i);
}

void GTKPreferenceWindow::SaveLocalSet(char *newpath, bool set)
{
    proposedValues.saveStreamsDirectory = newpath;
    gtk_widget_set_sensitive(applyButton, TRUE);
    if (set)
        gtk_entry_set_text(GTK_ENTRY(saveStreamBox), newpath);
}

static void save_stream_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->SaveLocalSet(text, false);
}

static void save_stream_browse(GtkWidget *w, GTKPreferenceWindow *p)
{
    GTKFileSelector *filesel = new GTKFileSelector(p->GetContext(),"Select a New Directory");
    if (filesel->Run(true)) {
        char *returnpath = filesel->GetReturnPath();
 
        struct stat st;
 
        if (stat(returnpath, &st)) {
            if (S_ISDIR(st.st_mode)) 
                p->SaveLocalSet(returnpath, true);
            else {
                MessageDialog oBox(p->GetContext());
                oBox.Show("Please select a directory to save streams locally.",
                          "Save Stream Browser Error", kMessageOk, true);
            }
        }
    }
    delete filesel;
}

void GTKPreferenceWindow::ProxyAddySet()
{
    char *name = gtk_entry_get_text(GTK_ENTRY(proxyAddyBox));
    char *port = gtk_entry_get_text(GTK_ENTRY(proxyPortBox));

    proposedValues.proxyServer = name;
    if (*port) {
        proposedValues.proxyServer += string(":") + string(port);
    }
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void proxy_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->ProxyAddySet();
}

void GTKPreferenceWindow::AltIPSet()
{
    char *one = gtk_entry_get_text(GTK_ENTRY(ipOneBox));
    char *two = gtk_entry_get_text(GTK_ENTRY(ipTwoBox));
    char *three = gtk_entry_get_text(GTK_ENTRY(ipThreeBox));
    char *four = gtk_entry_get_text(GTK_ENTRY(ipFourBox));
    if (*one)
        proposedValues.alternateIP = one;
    else
        proposedValues.alternateIP = "0";
    proposedValues.alternateIP += ".";
    if (*two)
        proposedValues.alternateIP += two;
    else
        proposedValues.alternateIP += "0";
    proposedValues.alternateIP += ".";
    if (*three)
        proposedValues.alternateIP += three;
    else
        proposedValues.alternateIP += "0";
    proposedValues.alternateIP += ".";
    if (*four)
        proposedValues.alternateIP += four;
    else
        proposedValues.alternateIP += "0";
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void ip_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->AltIPSet();
}

GtkWidget *GTKPreferenceWindow::CreateStreaming(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *check = gtk_check_button_new_with_label("Save SHOUTCast/icecast Streams Locally");
    gtk_container_add(GTK_CONTAINER(pane), check);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(save_local_toggle), this);
    if (originalValues.saveStreams)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(pane), frame);
    gtk_widget_show(frame);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    saveStreamLabel = gtk_label_new("Save Location:");
    gtk_misc_set_alignment(GTK_MISC(saveStreamLabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), saveStreamLabel, FALSE, FALSE, 0);
    gtk_widget_show(saveStreamLabel);

    char copys[256];

    GtkWidget *temphbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), temphbox, FALSE, FALSE, 0);
    gtk_widget_show(temphbox);

    strncpy(copys, originalValues.saveStreamsDirectory.c_str(), 256);
    saveStreamBox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(saveStreamBox), copys); 
    gtk_entry_set_max_length(GTK_ENTRY(saveStreamBox), 64);
    gtk_signal_connect(GTK_OBJECT(saveStreamBox), "changed",
                       GTK_SIGNAL_FUNC(save_stream_change), this);
    gtk_box_pack_start(GTK_BOX(temphbox), saveStreamBox, TRUE, TRUE, 0);
    gtk_widget_show(saveStreamBox);

    saveBrowseBox = gtk_button_new_with_label(" Browse ");
    gtk_box_pack_start(GTK_BOX(temphbox), saveBrowseBox, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(saveBrowseBox), "clicked",
                       GTK_SIGNAL_FUNC(save_stream_browse), this);
    gtk_widget_show(saveBrowseBox);

    if (!originalValues.saveStreams) {
        gtk_widget_set_sensitive(saveStreamLabel, FALSE);
        gtk_widget_set_sensitive(saveStreamBox, FALSE);
        gtk_widget_set_sensitive(saveBrowseBox, FALSE);
    }

    check = gtk_check_button_new_with_label("Use Proxy Server");
    gtk_container_add(GTK_CONTAINER(pane), check);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(proxy_toggle), this);
    if (originalValues.useProxyServer)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(pane), frame);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);
    gtk_widget_show(hbox);

    proxyAddyLabel = gtk_label_new("Proxy Address:");
    gtk_box_pack_start(GTK_BOX(hbox), proxyAddyLabel, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(proxyAddyLabel), 0.0, 0.5);
    gtk_widget_show(proxyAddyLabel);

    proxyPortLabel = gtk_label_new("Port:");
    gtk_box_pack_end(GTK_BOX(hbox), proxyPortLabel, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(proxyPortLabel), 0.9, 0.5);
    gtk_widget_show(proxyPortLabel);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);
    gtk_widget_show(hbox);

    char *port = NULL;
    char tempstr[256];

    strncpy(tempstr, originalValues.proxyServer.c_str(), 256);
    port = strrchr(tempstr, ':');
    if (port) {
        *port = '\0';
        port++;
    }

    proxyAddyBox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(proxyAddyBox), tempstr);
    gtk_signal_connect(GTK_OBJECT(proxyAddyBox), "changed",
                       GTK_SIGNAL_FUNC(proxy_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), proxyAddyBox, TRUE, TRUE, 0);
    gtk_widget_show(proxyAddyBox);

    proxyColon = gtk_label_new(":");
    gtk_box_pack_start(GTK_BOX(hbox), proxyColon, FALSE, FALSE, 5);
    gtk_widget_show(proxyColon);

    proxyPortBox = gtk_entry_new();
    if (port)
        gtk_entry_set_text(GTK_ENTRY(proxyPortBox), port);
    gtk_entry_set_max_length(GTK_ENTRY(proxyPortBox), 5);
    gtk_widget_set_usize(proxyPortBox, 60, 0);
    gtk_signal_connect(GTK_OBJECT(proxyPortBox), "changed",
                       GTK_SIGNAL_FUNC(proxy_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), proxyPortBox, FALSE, FALSE, 0);
    gtk_widget_show(proxyPortBox);

    if (!originalValues.useProxyServer) {
        gtk_widget_set_sensitive(proxyAddyBox, FALSE);
        gtk_widget_set_sensitive(proxyPortBox, FALSE);
        gtk_widget_set_sensitive(proxyColon, FALSE);
        gtk_widget_set_sensitive(proxyAddyLabel, FALSE);
        gtk_widget_set_sensitive(proxyPortLabel, FALSE);
    }

    check = gtk_check_button_new_with_label("Use Alternate IP");
    gtk_container_add(GTK_CONTAINER(pane), check);
    gtk_signal_connect(GTK_OBJECT(check), "toggled",
                       GTK_SIGNAL_FUNC(alt_ip_toggle), this);
    if (originalValues.useAlternateIP)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);
    gtk_widget_show(check);

    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(pane), frame);
    gtk_widget_show(frame);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    gtk_widget_show(hbox);

    ipLabel = gtk_label_new("IP Address: ");
    gtk_box_pack_start(GTK_BOX(hbox), ipLabel, FALSE, FALSE, 10);
    gtk_widget_show(ipLabel);

    char *dot = NULL;
    char *ip[4];
    int32 i = 1;

    if (originalValues.alternateIP.length() != 0) {
        strncpy(tempstr, originalValues.alternateIP.c_str(), 256);
        ip[0] = tempstr;
        dot = tempstr;

        while ((dot = strchr(dot, '.'))) {
            *dot = 0x00;
            ip[i++] = ++dot;
            if (i == 4)
                break;
        }
    }

    ipOneBox = gtk_entry_new();
    if (originalValues.alternateIP.length() == 0)
        gtk_entry_set_text(GTK_ENTRY(ipOneBox), "0");
    else 
        gtk_entry_set_text(GTK_ENTRY(ipOneBox), ip[0]);
    gtk_entry_set_max_length(GTK_ENTRY(ipOneBox), 3);
    gtk_widget_set_usize(ipOneBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(ipOneBox), "changed",
                       GTK_SIGNAL_FUNC(ip_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), ipOneBox, FALSE, FALSE, 0);
    gtk_widget_show(ipOneBox);

    ipPeriod1 = gtk_label_new(".");
    gtk_container_add(GTK_CONTAINER(hbox), ipPeriod1);
    gtk_widget_show(ipPeriod1);

    ipTwoBox = gtk_entry_new();
    if (originalValues.alternateIP.length() == 0)
        gtk_entry_set_text(GTK_ENTRY(ipTwoBox), "0");
    else
        gtk_entry_set_text(GTK_ENTRY(ipTwoBox), ip[1]);
    gtk_entry_set_max_length(GTK_ENTRY(ipTwoBox), 3);
    gtk_widget_set_usize(ipTwoBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(ipOneBox), "changed",
                       GTK_SIGNAL_FUNC(ip_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), ipTwoBox, FALSE, FALSE, 0);
    gtk_widget_show(ipTwoBox);

    ipPeriod2 = gtk_label_new(".");
    gtk_container_add(GTK_CONTAINER(hbox), ipPeriod2);
    gtk_widget_show(ipPeriod2);

    ipThreeBox = gtk_entry_new();
    if (originalValues.alternateIP.length() == 0)
        gtk_entry_set_text(GTK_ENTRY(ipThreeBox), "0");
    else
        gtk_entry_set_text(GTK_ENTRY(ipThreeBox), ip[2]);
    gtk_entry_set_max_length(GTK_ENTRY(ipThreeBox), 3);
    gtk_widget_set_usize(ipThreeBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(ipThreeBox), "changed",
                       GTK_SIGNAL_FUNC(ip_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), ipThreeBox, FALSE, FALSE, 0);
    gtk_widget_show(ipThreeBox);

    ipPeriod3 = gtk_label_new(".");
    gtk_container_add(GTK_CONTAINER(hbox), ipPeriod3);
    gtk_widget_show(ipPeriod3);

    ipFourBox = gtk_entry_new();
    if (originalValues.alternateIP.length() == 0)
        gtk_entry_set_text(GTK_ENTRY(ipFourBox), "0");
    else
        gtk_entry_set_text(GTK_ENTRY(ipFourBox), ip[3]);
    gtk_entry_set_max_length(GTK_ENTRY(ipFourBox), 3);
    gtk_widget_set_usize(ipFourBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(ipFourBox), "changed",
                       GTK_SIGNAL_FUNC(ip_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), ipFourBox, FALSE, FALSE, 0);
    gtk_widget_show(ipFourBox);

    if (!originalValues.useAlternateIP) {
        gtk_widget_set_sensitive(ipLabel, FALSE);
        gtk_widget_set_sensitive(ipOneBox, FALSE);
        gtk_widget_set_sensitive(ipPeriod1, FALSE);
        gtk_widget_set_sensitive(ipTwoBox, FALSE);
        gtk_widget_set_sensitive(ipPeriod2, FALSE);
        gtk_widget_set_sensitive(ipThreeBox, FALSE);
        gtk_widget_set_sensitive(ipPeriod3, FALSE);
        gtk_widget_set_sensitive(ipFourBox, FALSE);
    }

    return pane;
}

void GTKPreferenceWindow::SetPMO(int newsel)
{
    newsel = (m_PMOnames->size() - 1) - newsel;
    proposedValues.defaultPMO = (*m_PMOnames)[newsel];
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void pmo_select(GtkWidget *item, GTKPreferenceWindow *p)
{
    int i = 0;

    if (!GTK_WIDGET_MAPPED(item))
        return;

    GSList *glist = gtk_radio_menu_item_group((GtkRadioMenuItem *)
                        (((GtkOptionMenu *)p->pmoOptionMenu)->menu_item));
    while (glist && !((GtkCheckMenuItem *)(glist->data))->active) {
        glist = glist->next;
        i++;
    }
 
    p->SetPMO(i);
}

void GTKPreferenceWindow::AlsaSet(void)
{
    char *one = gtk_entry_get_text(GTK_ENTRY(alsaOneBox));
    char *two = gtk_entry_get_text(GTK_ENTRY(alsaTwoBox));

    proposedValues.alsaOutput = string(one) + string(":") + string(two);
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void alsa_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->AlsaSet();
}

GtkWidget *GTKPreferenceWindow::CreatePlugins(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *frame = gtk_frame_new("Audio");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 0);
    gtk_widget_show(frame);

    GtkWidget *table = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    gtk_widget_show(table);

    Registry *pmo = m_pContext->player->GetPMORegistry();
    int32 i = 0;
    RegistryItem *item;

    GtkWidget *label = gtk_label_new("Audio Output");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL,
                     5, 0);
    gtk_widget_show(label);

    pmoOptionMenu = gtk_option_menu_new();
    pmoMenu = gtk_menu_new();
    GSList *group = NULL;
    GtkWidget *menuitem;
    uint32 outputIndex = 0;
    uint32 iLoop = 0;

    while (pmo && (item = pmo->GetItem(i++))) {
        if (!strncmp("cd.pmo", item->Name(), 7) ||
            !strncmp("signature.pmo", item->Name(), 13))
            continue;

	m_PMOnames->push_back(string(item->Name()));
    }

    vector<string>::iterator vsi = m_PMOnames->begin();
    for (; vsi != m_PMOnames->end(); vsi++, iLoop++) {
        menuitem = gtk_radio_menu_item_new_with_label(group, (*vsi).c_str());
        gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
                           GTK_SIGNAL_FUNC(pmo_select), this);
        group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(menuitem));
        gtk_menu_append(GTK_MENU(pmoMenu), menuitem);
        if (originalValues.defaultPMO == (*vsi)) {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem), TRUE);

            outputIndex = iLoop;
        }
        gtk_widget_show(menuitem);
    }

    gtk_option_menu_set_menu(GTK_OPTION_MENU(pmoOptionMenu), pmoMenu);
    gtk_table_attach(GTK_TABLE(table), pmoOptionMenu, 1, 2, 1, 2, GTK_FILL,
                     GTK_FILL, 5, 5);
    gtk_option_menu_set_history(GTK_OPTION_MENU(pmoOptionMenu), outputIndex);
    gtk_widget_show(pmoOptionMenu);

    frame = gtk_frame_new("ALSA Setup");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 1);
    gtk_widget_show(frame);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    gtk_widget_show(hbox);

    label = gtk_label_new("Default ALSA Output Device: ");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 10);
    gtk_widget_show(label);

    char tempstr[256];
    char *port = NULL;
    strncpy(tempstr, originalValues.alsaOutput.c_str(), 256);
    port = strrchr(tempstr, ':');
    if (port) {
        *port = '\0';
        port++;
    }

    alsaOneBox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(alsaOneBox), tempstr);
    gtk_entry_set_max_length(GTK_ENTRY(alsaOneBox), 3);
    gtk_widget_set_usize(alsaOneBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(alsaOneBox), "changed",
                       GTK_SIGNAL_FUNC(alsa_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), alsaOneBox, FALSE, FALSE, 0);
    gtk_widget_show(alsaOneBox);

    label = gtk_label_new(":");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    alsaTwoBox = gtk_entry_new();
    if (port)
        gtk_entry_set_text(GTK_ENTRY(alsaTwoBox), port);
    gtk_entry_set_max_length(GTK_ENTRY(alsaTwoBox), 3);
    gtk_widget_set_usize(alsaTwoBox, 32, 0);
    gtk_signal_connect(GTK_OBJECT(alsaTwoBox), "changed",
                       GTK_SIGNAL_FUNC(alsa_change), this);
    gtk_box_pack_start(GTK_BOX(hbox), alsaTwoBox, FALSE, FALSE, 0);
    gtk_widget_show(alsaTwoBox);

/*
    frame = gtk_frame_new("Portable Devices");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 1);
    gtk_widget_show(frame);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 1);
    gtk_widget_show(vbox);

    GtkWidget *listwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(listwindow),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), listwindow, TRUE, TRUE, 5);
    gtk_widget_show(listwindow);

    GtkWidget *list = gtk_clist_new(1);
    gtk_container_add(GTK_CONTAINER(listwindow), list);
    gtk_widget_show(list);

    GtkWidget *textlabel = gtk_label_new("Select from the list above any portable devices you own.  This will enable you to edit the contents of your portable device directly from the \"My Music\" window.  If you do not see your portable device listed try checking for an update.  We might have added support for it since you installed.");
    gtk_label_set_line_wrap(GTK_LABEL(textlabel), TRUE);
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), textlabel, TRUE, FALSE, 0);
    gtk_widget_show(textlabel);
*/

    return pane;
}

int GTKPreferenceWindow::SetInputBufferSize(int newvalue)
{
    if (newvalue < 8)
    {
        MessageDialog oBox(m_pContext);
        string        oMessage;

        oMessage = string("The input buffer size must be at least 8k.");

        oBox.Show(oMessage.c_str(), "Input buffer size", kMessageOk, true);
        return proposedValues.inputBufferSize;
    }
    else
    {
        proposedValues.inputBufferSize = newvalue;
        gtk_widget_set_sensitive(applyButton, TRUE);
        return 0;
    }
}

static void input_buffer_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    int newdata = atoi(text);
    int last = p->SetInputBufferSize(newdata);
    if (last > 0)
    {
        char temp[10];

        sprintf(temp, "%d", last);
        gtk_entry_set_text(GTK_ENTRY(w), temp);
    }
}

void GTKPreferenceWindow::SetOutputBufferSize(int newvalue)
{
    proposedValues.outputBufferSize = newvalue;
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void output_buffer_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    int newdata = atoi(text);
    p->SetOutputBufferSize(newdata);
}

void GTKPreferenceWindow::SetPreBufferLength(int newvalue)
{
    proposedValues.preBufferLength = newvalue;
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void prestream_buffer_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    int newdata = atoi(text);
    p->SetPreBufferLength(newdata);
}

GtkWidget *GTKPreferenceWindow::CreateAdvanced(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *frame = gtk_frame_new("Buffer Sizes");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 0);
    gtk_widget_show(frame);

    GtkWidget *table = gtk_table_new(3, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    gtk_widget_show(table);

    int32 value;
    char tempstr[256];

    GtkWidget *label = gtk_label_new("Input Buffer Size (Kilobytes)");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    GtkWidget *entry = gtk_entry_new();
    value = originalValues.inputBufferSize;
    sprintf(tempstr, "%d", value);
    gtk_entry_set_text(GTK_ENTRY(entry), tempstr);
    gtk_entry_set_max_length(GTK_ENTRY(entry), 6);
    gtk_signal_connect(GTK_OBJECT(entry), "changed",
                       GTK_SIGNAL_FUNC(input_buffer_change), this);
    gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL,
                     10, 3);
    gtk_widget_show(entry);

    label = gtk_label_new("Output Buffer Size (Kilobytes)");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    entry = gtk_entry_new();
    value = originalValues.outputBufferSize;
    sprintf(tempstr, "%d", value);
    gtk_entry_set_text(GTK_ENTRY(entry), tempstr);
    gtk_entry_set_max_length(GTK_ENTRY(entry), 4);
    gtk_signal_connect(GTK_OBJECT(entry), "changed",
                       GTK_SIGNAL_FUNC(output_buffer_change), this);
    gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL,
                     10, 3);
    gtk_widget_show(entry);

    label = gtk_label_new("Prebuffer Streams (Seconds)");
    gtk_misc_set_alignment(GTK_MISC(label), (gfloat)1.0, (gfloat)0.5);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL,
                     10, 1);
    gtk_widget_show(label);

    entry = gtk_entry_new();
    value = originalValues.preBufferLength;
    sprintf(tempstr, "%d", value);
    gtk_entry_set_text(GTK_ENTRY(entry), tempstr);
    gtk_entry_set_max_length(GTK_ENTRY(entry), 2);
    gtk_signal_connect(GTK_OBJECT(entry), "changed",
                       GTK_SIGNAL_FUNC(prestream_buffer_change), this);
    gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL,
                     10, 3);
    gtk_widget_show(entry);

    return pane;
}

static void freeamp_press(GtkWidget *w, GTKPreferenceWindow *p)
{
    LaunchBrowser("http://www.zinf.org/");    
}

GtkWidget *GTKPreferenceWindow::CreateAbout(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 0);
    gtk_widget_show(pane);

    GtkWidget *textlabel = gtk_label_new(BRANDING);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
    gtk_widget_show(textlabel);

    textlabel = gtk_label_new("version "BRANDING_VERSION);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
    gtk_widget_show(textlabel);

    if (strcmp(BRANDING, "FreeAmp")) {
        textlabel = gtk_label_new("(based on FreeA*p)");
        gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
        gtk_widget_show(textlabel);
    }       
 
    textlabel = gtk_label_new("Zinf is an Open Source effort to build the most useful\n"
                              "digital audio player around. In the interest of\n"
                              "supporting the free software community, while at\n"
                              "the same time fostering the growth of the online\n"
                              "delivery of music, EMusic.com has funded both the\n"
                              "FreeA*p.org domain and the efforts of the FreeA*p\n"
                              "team. The FreeA*p team consisted of: Mark B. Elrod,\n"
                              "Robert Kaye, Isaac Richards, Brett Thomas, and\n"
                              "Jason Woodward.");
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
    gtk_widget_show(textlabel);

    textlabel = gtk_label_new("Other people have also contributed to FreeA*p:           ");
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, TRUE, TRUE, 0);
    gtk_widget_show(textlabel);

    textlabel = gtk_label_new("William Bull, Alan Cutter, Gabor Fleischer,\n"
                              "Jean-Michel HERVE, Hiromasa Kato, Michael Bruun\n"
                              "Petersen, Sylvain Rebaud, The Snowblind Alliance,\n"
                              "Tom Spindler, and Valters Vingolds.");
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
    gtk_widget_show(textlabel);

    textlabel = gtk_label_new("Zinf is being released under the terms of the\n"
                              "GPL. Zinf is looking for some talented programmers\n"
                              "who are interested in continuing to work on Zinf.\n"
                              "Please check the zinf.org website for more details.");
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_FILL);
    gtk_box_pack_start(GTK_BOX(pane), textlabel, FALSE, FALSE, 0);
    gtk_widget_show(textlabel);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
    gtk_container_add(GTK_CONTAINER(pane), hbox);
    gtk_widget_show(hbox);

    GtkWidget *button;

    button = gtk_button_new_with_label(" Visit Zinf.org ");
    gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", 
                       GTK_SIGNAL_FUNC(freeamp_press), this);
    gtk_widget_show(button);

    return pane;
}

void GTKPreferenceWindow::SelectTheme(int row)
{
    proposedValues.listboxIndex = row;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}


static void theme_click(GtkWidget *w, int row, int column, 
                        GdkEventButton *button, GTKPreferenceWindow *p)
{
    p->SelectTheme(row);
}

void GTKPreferenceWindow::AddThemeEvent(const char *newpath)
{
    string newThemeFile = newpath;
    Error err = m_pThemeMan->AddTheme(newThemeFile);

    if (IsError(err)) {
        MessageDialog oBox(m_pContext);
        string        oErr, oMessage;

        oErr = ErrorString[err];
        oMessage = "Couldn't Add Theme for the Following Reason: " + oErr;

        oBox.Show(oMessage.c_str(), "Add Theme Error", kMessageOk, true);
    }
    else
        UpdateThemeList();
}

static void add_theme_press(GtkWidget *w, GTKPreferenceWindow *p)
{
    GTKFileSelector *filesel = new GTKFileSelector(p->GetContext(),"Select a Theme to Add");
    if (filesel->Run(true)) {
        char *returnpath = filesel->GetReturnPath();
        p->AddThemeEvent(returnpath);
    }
    delete filesel;
}

void GTKPreferenceWindow::DeleteThemeEvent(void)
{
    map<string, string>::iterator i;
    int32 iLoop = 0;
    string themeToDelete;

    for (i = m_oThemeList.begin(); i != m_oThemeList.end(); i++, iLoop++) {
         if (iLoop == proposedValues.listboxIndex)
             themeToDelete = (*i).first;
    }
    Error err = m_pThemeMan->DeleteTheme(m_oThemeList[themeToDelete]);

    if (IsError(err)) {
        MessageDialog oBox(m_pContext);
        string        oErr, oMessage;

        oErr = ErrorString[err];
        oMessage = "Couldn't Delete the Theme for Because: " 
                   + oErr;

        oBox.Show(oMessage.c_str(), "Delete Theme Error", kMessageOk, true);
    }
    else
        UpdateThemeList();
}

static void delete_theme_press(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->DeleteThemeEvent();
}

void GTKPreferenceWindow::UpdateThemeList(void)
{
    int iLoop = 0;
    map<string, string>::iterator i;
    proposedValues.listboxIndex = 0;

    m_pThemeMan->GetCurrentTheme(originalValues.currentTheme);
    m_oThemeList.clear();

    gtk_clist_freeze(GTK_CLIST(themeList));
    gtk_clist_clear(GTK_CLIST(themeList));

    m_pThemeMan->GetThemeList(m_oThemeList);
    for (i = m_oThemeList.begin(); i != m_oThemeList.end(); i++, iLoop++) {
         char *Text[1];
         Text[0] = (char *)((*i).first.c_str());
         gtk_clist_append(GTK_CLIST(themeList), Text); 
         if ((*i).second == originalValues.currentTheme) 
             originalValues.listboxIndex = proposedValues.listboxIndex 
                                         = currentValues.listboxIndex = iLoop;
         else {
             char *name = strrchr((*i).second.c_str(), '/');
             if (name) {
                 name++;
                 if (name && *name) {
                     if (!strcmp(name, originalValues.currentTheme.c_str())) 
                         originalValues.listboxIndex = 
                                             proposedValues.listboxIndex 
                                           = currentValues.listboxIndex = iLoop;
                 }
             }
         }
    }

    gtk_clist_select_row(GTK_CLIST(themeList), proposedValues.listboxIndex, 0);

    gtk_clist_thaw(GTK_CLIST(themeList));
}

void GTKPreferenceWindow::SetFont()
{
    char *font_name = gtk_font_selection_dialog_get_font_name(
                                         GTK_FONT_SELECTION_DIALOG(fontDialog));
    if (font_name) {
        char realfontname[128];
        char *marker1, *marker2;
        int length;
   
        marker1 = strchr(font_name + 1, '-');
        marker2 = strchr(marker1 + 1, '-');

        marker1++;
        length = strlen(marker1) - strlen(marker2);

        strncpy(realfontname, marker1, length);
        realfontname[length] = '\0';
        proposedValues.defaultFont = string(realfontname);
        g_free(font_name);
    }
    gtk_widget_destroy(fontDialog);
}

static void font_ok(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->SetFont();
}

void GTKPreferenceWindow::ChooseFont(void)
{
    fontDialog = gtk_font_selection_dialog_new("FreeAmp Default Font Selection Dialog");
    gtk_window_set_position(GTK_WINDOW(fontDialog), GTK_WIN_POS_MOUSE);
    gtk_window_set_modal(GTK_WINDOW(fontDialog), TRUE);

    gtk_signal_connect(GTK_OBJECT(fontDialog), "destroy",
                       GTK_SIGNAL_FUNC(gtk_widget_destroyed), &fontDialog);
    gtk_signal_connect_object(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(fontDialog)->                              cancel_button), "clicked",
                              GTK_SIGNAL_FUNC(gtk_widget_destroy), 
                              GTK_OBJECT(fontDialog));
    gtk_signal_connect(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(fontDialog)->
                       ok_button), "clicked", GTK_SIGNAL_FUNC(font_ok),
                       this);
    gtk_widget_show(fontDialog);
}

static void choose_font_press(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->ChooseFont();
}

GtkWidget *GTKPreferenceWindow::CreateThemes(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *frame = gtk_frame_new("Theme Selection");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
    gtk_container_add(GTK_CONTAINER(pane), frame);
    gtk_widget_show(frame);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    gtk_widget_show(hbox);

    GtkWidget *listwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(listwindow),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(hbox), listwindow, TRUE, TRUE, 5);
    gtk_widget_set_usize(listwindow, 200, 200);
    gtk_widget_show(listwindow);

    themeList = gtk_clist_new(1);
    gtk_signal_connect(GTK_OBJECT(themeList), "select_row",
                       GTK_SIGNAL_FUNC(theme_click), this);
    gtk_container_add(GTK_CONTAINER(listwindow), themeList);
    gtk_widget_show(themeList);

    UpdateThemeList();

    GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 5);
    gtk_widget_show(vbox);

    GtkWidget *button = gtk_button_new_with_label("Add Theme");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(add_theme_press), this);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("Delete Theme");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(delete_theme_press), this);
    gtk_widget_show(button);

    frame = gtk_frame_new("Default Font");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
    gtk_container_add(GTK_CONTAINER(pane), frame);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    GtkWidget *textlabel = gtk_label_new("A theme may specify a font type that is not installed on your       \nsystem. The default font will by substituted in place of the\nmissing font.");
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), textlabel, TRUE, TRUE, 0);
    gtk_widget_show(textlabel);

    hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    gtk_widget_show(hbox);

    textlabel = gtk_label_new("Note: Only the font name will be used.  The font will appear in the style specified in the theme.");
    gtk_label_set_line_wrap(GTK_LABEL(textlabel), TRUE);
    gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_FILL);
    gtk_box_pack_start(GTK_BOX(hbox), textlabel, FALSE, FALSE, 5);
    gtk_widget_show(textlabel);

    button = gtk_button_new_with_label(" Choose Font ");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(choose_font_press), this);
    gtk_widget_show(button);

    return pane;
}

void GTKPreferenceWindow::SaveMusicSet(char *newpath, bool set)
{
    proposedValues.saveMusicDirectory = newpath;
    gtk_widget_set_sensitive(applyButton, TRUE);
    if (set)
        gtk_entry_set_text(GTK_ENTRY(saveMusicBox), newpath);
}

static void save_music_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->SaveMusicSet(text, false);
}

static void save_music_browse(GtkWidget *w, GTKPreferenceWindow *p)
{
    GTKFileSelector *filesel = new GTKFileSelector(p->GetContext(),"Select a New Directory");
    if (filesel->Run(true)) {
        char *returnpath = filesel->GetReturnPath();

        struct stat st;

        if (stat(returnpath, &st)) {
            if (S_ISDIR(st.st_mode))
                p->SaveMusicSet(returnpath, true);
            else {
                MessageDialog oBox(p->GetContext());
                oBox.Show("Please select a directory to save dowloaded music.",
                          "Save Music Browser Error", kMessageOk, true);
            }
        }
    }
    delete filesel;
}

void GTKPreferenceWindow::WatchDirSet(char *newpath, bool set)
{
    proposedValues.watchThisDirectory = newpath;
    gtk_widget_set_sensitive(applyButton, TRUE);
    if (set)
        gtk_entry_set_text(GTK_ENTRY(watchDirBox), newpath);
}

static void watch_dir_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->WatchDirSet(text, false);
}

static void watch_dir_browse(GtkWidget *w, GTKPreferenceWindow *p)
{
    GTKFileSelector *filesel = new GTKFileSelector(p->GetContext(),"Select a New Directory");
    if (filesel->Run(true)) {
        char *returnpath = filesel->GetReturnPath();

        struct stat st;

        if (stat(returnpath, &st)) {
            if (S_ISDIR(st.st_mode))
                p->WatchDirSet(returnpath, true);
            else {
                MessageDialog oBox(p->GetContext());
                oBox.Show("Please select a directory to have "the_BRANDING
                          " watch for updated music files.",
                          "Watch This Directory Browse Error", kMessageOk, 
                          true);
            }
        }
    }
    delete filesel;
}

void GTKPreferenceWindow::SetWatchTimeout(int32 timeout)
{
    proposedValues.watchThisDirTimeout = timeout;
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
}

static void never_timeout_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetWatchTimeout(0);
}

static void one_timeout_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetWatchTimeout(60);
}

static void five_timeout_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetWatchTimeout(300);
}

static void ten_timeout_selected(GtkWidget *w, GTKPreferenceWindow *p)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
        p->SetWatchTimeout(600);
}

GtkWidget *GTKPreferenceWindow::CreateDirectories(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *frame = gtk_frame_new("Save Music Folder");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    char copys[_MAX_PATH];

    GtkWidget *temphbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), temphbox, FALSE, FALSE, 5);
    gtk_widget_show(temphbox);

    strncpy(copys, originalValues.saveMusicDirectory.c_str(), 256);
    saveMusicBox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(saveMusicBox), copys);
    gtk_entry_set_max_length(GTK_ENTRY(saveMusicBox), 64);
    gtk_signal_connect(GTK_OBJECT(saveMusicBox), "changed",
                       GTK_SIGNAL_FUNC(save_music_change), this);
    gtk_box_pack_start(GTK_BOX(temphbox), saveMusicBox, TRUE, TRUE, 0);
    gtk_widget_show(saveMusicBox);

    GtkWidget *button = gtk_button_new_with_label(" Browse ");
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(save_music_browse), this);
    gtk_widget_show(button);

    frame = gtk_frame_new("Watch This Directory");
    gtk_box_pack_start(GTK_BOX(pane), frame, FALSE, FALSE, 5);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    temphbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), temphbox, FALSE, FALSE, 5);
    gtk_widget_show(temphbox);

    strncpy(copys, originalValues.watchThisDirectory.c_str(), 256);
    watchDirBox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(watchDirBox), copys);
    gtk_entry_set_max_length(GTK_ENTRY(watchDirBox), 64);
    gtk_signal_connect(GTK_OBJECT(watchDirBox), "changed",
                       GTK_SIGNAL_FUNC(watch_dir_change), this);
    gtk_box_pack_start(GTK_BOX(temphbox), watchDirBox, TRUE, TRUE, 0);
    gtk_widget_show(watchDirBox);

    button = gtk_button_new_with_label(" Browse ");
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(watch_dir_browse), this);
    gtk_widget_show(button);

    GtkWidget *label = gtk_label_new("Update Frequency:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);

    temphbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), temphbox, FALSE, FALSE, 5);
    gtk_widget_show(temphbox);

    bool setSomething = false;

    button = gtk_radio_button_new_with_label(NULL, "Never");
    if (originalValues.watchThisDirTimeout == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(never_timeout_selected), this);
    gtk_widget_show(button);

    button = gtk_radio_button_new_with_label(
                             gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                             "One Minute");
    if (originalValues.watchThisDirTimeout == 60) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(one_timeout_selected), this);
    gtk_widget_show(button);


    button = gtk_radio_button_new_with_label(
                             gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                             "5 Minutes");
    if (originalValues.watchThisDirTimeout == 300) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething  = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(five_timeout_selected), this);
    gtk_widget_show(button);

    button = gtk_radio_button_new_with_label(
                             gtk_radio_button_group(GTK_RADIO_BUTTON(button)),
                             "10 Minutes");
    if (originalValues.watchThisDirTimeout == 600) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        setSomething  = true;
    }
    gtk_box_pack_start(GTK_BOX(temphbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(ten_timeout_selected), this);
    gtk_widget_show(button);

    if (!setSomething)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    return pane;
}

void GTKPreferenceWindow::PollCDToggle(int active)
{
    if (!firsttime)
        gtk_widget_set_sensitive(applyButton, TRUE);
    proposedValues.pollCD = (bool)active;
}

static void pollcd_toggle(GtkWidget *w, GTKPreferenceWindow *p)
{
    int i = GTK_TOGGLE_BUTTON(w)->active;
    p->PollCDToggle(i);
}

void GTKPreferenceWindow::CDPathSet(char *newpath, bool set)
{
    proposedValues.CDDevicePath = newpath;
    gtk_widget_set_sensitive(applyButton, TRUE);
    if (set)
        gtk_entry_set_text(GTK_ENTRY(cdPath), newpath);
}

static void cd_path_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->CDPathSet(text, false);
}

void GTKPreferenceWindow::MBServerSet(char *newpath, bool set)
{
    proposedValues.MBServer = newpath;
    gtk_widget_set_sensitive(applyButton, TRUE);
    if (set)
        gtk_entry_set_text(GTK_ENTRY(mbServer), newpath);
}

static void mb_server_change(GtkWidget *w, GTKPreferenceWindow *p)
{
    char *text = gtk_entry_get_text(GTK_ENTRY(w));
    p->MBServerSet(text, false);
}

GtkWidget *GTKPreferenceWindow::CreateCD(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    pollCD = gtk_check_button_new_with_label("Automatically update the CD Audio"
                                             " item in My Music");
    gtk_container_add(GTK_CONTAINER(pane), pollCD);
    gtk_signal_connect(GTK_OBJECT(pollCD), "toggled",
                       GTK_SIGNAL_FUNC(pollcd_toggle), this);
    if (originalValues.pollCD)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pollCD),
                                     TRUE);
    gtk_widget_show(pollCD);

    GtkWidget *temphbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(pane), temphbox, FALSE, FALSE, 5);
    gtk_widget_show(temphbox);

    GtkWidget *label = gtk_label_new("CD-ROM path: ");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(temphbox), label, FALSE, FALSE, 5);
    gtk_widget_show(label);

    char copys[256];
   
    strncpy(copys, originalValues.CDDevicePath.c_str(), 256);
    cdPath = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(cdPath), copys);
    gtk_entry_set_max_length(GTK_ENTRY(cdPath), 64);
    gtk_signal_connect(GTK_OBJECT(cdPath), "changed",
                       GTK_SIGNAL_FUNC(cd_path_change), this);
    gtk_box_pack_start(GTK_BOX(temphbox), cdPath, TRUE, TRUE, 0);
    gtk_widget_show(cdPath);

    temphbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(pane), temphbox, FALSE, FALSE, 5);
    gtk_widget_show(temphbox);

    label = gtk_label_new("MusicBrainz server URL for Audio CD/metadata lookups: ");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(pane), label, FALSE, FALSE, 5);
    gtk_widget_show(label);

    strncpy(copys, originalValues.MBServer.c_str(), 256);
    mbServer = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(mbServer), copys);
    gtk_entry_set_max_length(GTK_ENTRY(mbServer), 64);
    gtk_signal_connect(GTK_OBJECT(mbServer), "changed",
                       GTK_SIGNAL_FUNC(mb_server_change), this);
    gtk_box_pack_start(GTK_BOX(pane), mbServer, TRUE, TRUE, 0);
    gtk_widget_show(mbServer);

    return pane;
}

void GTKPreferenceWindow::UpdatePLHeaders(void)
{
    string newheader;
    for (int row = 0; row < GTK_CLIST(plShownList)->rows; row++)
    {
        char *text[1];
        gtk_clist_get_text(GTK_CLIST(plShownList), row, 0, text);
        if (row != 0)
            newheader += "|";
        newheader += text[0];
    }
    proposedValues.playlistHeaders = newheader;
}

void GTKPreferenceWindow::AddPLSelection(void)
{
    if (!GTK_CLIST(plAvailableList)->selection)
        return;

    gint row = GPOINTER_TO_INT(GTK_CLIST(plAvailableList)->selection->data);

    char *text[1];
    gtk_clist_get_text(GTK_CLIST(plAvailableList), row, 0, text);
    gtk_clist_append(GTK_CLIST(plShownList), text);

    gtk_clist_remove(GTK_CLIST(plAvailableList), row);

    UpdatePLHeaders();
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void pl_add(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->AddPLSelection();
}

void GTKPreferenceWindow::RemovePLSelection(void)
{
    if (!GTK_CLIST(plShownList)->selection)
        return;

    gint row = GPOINTER_TO_INT(GTK_CLIST(plShownList)->selection->data);

    char *text[1];
    gtk_clist_get_text(GTK_CLIST(plShownList), row, 0, text);
    gtk_clist_append(GTK_CLIST(plAvailableList), text);

    gtk_clist_remove(GTK_CLIST(plShownList), row);
    gtk_clist_sort(GTK_CLIST(plAvailableList));

    UpdatePLHeaders();
    gtk_widget_set_sensitive(applyButton, TRUE);
}

static void pl_remove(GtkWidget *w, GTKPreferenceWindow *p)
{
    p->RemovePLSelection();
}

static void pl_row_move(GtkWidget *w, int source, int dest, 
                        GTKPreferenceWindow *p)
{
    p->UpdatePLHeaders();
}

GtkWidget *GTKPreferenceWindow::CreatePlaylistHeaders(void)
{
    GtkWidget *pane = gtk_vbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(pane), 5);
    gtk_widget_show(pane);

    GtkWidget *label = gtk_label_new("This will allow you to change what fields are shown in the Playlist editor");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(pane), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
 
    GtkWidget *hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(pane), hbox, TRUE, TRUE, 0);
    gtk_widget_show(hbox);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
    gtk_widget_show(vbox);

    label = gtk_label_new("Available:");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, FALSE, 0);
    gtk_widget_set_usize(scrolledWindow, 50, 200);
    gtk_widget_show(scrolledWindow);

    plAvailableList = gtk_clist_new(1);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), plAvailableList);
    gtk_widget_show(plAvailableList);

    vbox = gtk_vbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(vbox), GTK_BUTTONBOX_SPREAD);
    gtk_button_box_set_spacing(GTK_BUTTON_BOX(vbox), 10);

    gtk_widget_show(vbox);

    GtkWidget *button =  gtk_button_new_with_label(" << ");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(pl_remove), this);
    gtk_widget_show(button);

    button =  gtk_button_new_with_label(" >> ");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(pl_add), this);
    gtk_widget_show(button);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
    gtk_widget_show(vbox);

    label = gtk_label_new("Shown:");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);

    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, FALSE, 0);
    gtk_widget_set_usize(scrolledWindow, 50, 200);
    gtk_widget_show(scrolledWindow);

    plShownList = gtk_clist_new(1);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), plShownList);
    gtk_clist_set_reorderable(GTK_CLIST(plShownList), TRUE);
    gtk_signal_connect_after(GTK_OBJECT(plShownList), "row_move",
                             GTK_SIGNAL_FUNC(pl_row_move), this);
    gtk_widget_show(plShownList);

    char *columns = new char[originalValues.playlistHeaders.size() + 1];
    strcpy(columns, originalValues.playlistHeaders.c_str());

    string used;
    char *token = strtok(columns, "|");
    char *text[1];
    while (token != NULL)
    {
        text[0] = token;
        gtk_clist_append(GTK_CLIST(plShownList), text);
        used += token;
        token = strtok(NULL, "|");
    }

    delete [] columns;
 
    char *available[] = {"Artist", "Album", "Comment", "Genre", "Location",
                         "Time", "Title", "Year"};
    int nCols = sizeof(available) / sizeof(available[0]);
    for (int i = 0; i < nCols; i++)
    {
        if (!strstr(used.c_str(), available[i]))
        {
            text[0] = available[i];
            gtk_clist_append(GTK_CLIST(plAvailableList), text);
        }
    }

    return pane;
}
