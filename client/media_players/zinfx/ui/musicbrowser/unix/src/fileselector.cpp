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

    $Id: fileselector.cpp,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <unistd.h>
#include <dirent.h>

#ifdef __QNX__
#include <strings.h>
#endif

#include "fileselector.h"
#include "utility.h"
#include "preferences.h"

char old_path[PATH_MAX]="/\0";

FileSelector::FileSelector(FAContext *context,const char *windowtitle)
{
    m_context = context;
    title = windowtitle;
    returnpath = "";
    extended = true;
    ok = false;
    done = false;
}

gboolean filesel_destroy(GtkWidget *widget, gpointer p)
{
    bool runmain = (bool)p;
    if (runmain)
        gtk_main_quit();
    return FALSE;
}

void ok_internal(GtkWidget *widget, FileSelector *p)
{
    p->AddEvent();
}

void cancel_internal(GtkWidget *widget, FileSelector *p)
{
    p->CancelEvent();
}

string recursive_path(char subdirs[PATH_MAX], string returnpath,int *sub_rnum)
{
    DIR *dir=NULL;
    struct dirent *current=NULL;
    if ((dir = opendir(subdirs))!=NULL){
        current = readdir(dir);
        current = readdir(dir);
        while((current=readdir(dir))!=NULL){
            if(current->d_type==DT_DIR){
		char temp_subdir[PATH_MAX];
		strncpy(temp_subdir, subdirs,PATH_MAX);
		strcat(temp_subdir,"/");
		strncat(temp_subdir,current->d_name,FILENAME_MAX);
		returnpath = recursive_path(temp_subdir,returnpath,sub_rnum);
	    }
	    else {
    	    returnpath += "\n";
	    returnpath += subdirs;
	    returnpath += "/";
	    returnpath += current->d_name;
	    sub_rnum++;
	    }
	}
    }
    closedir(dir);
    return (returnpath);
}

void FileSelector::CancelEvent()
{
    gtk_widget_destroy(filesel);
    ok = false;
    done = true;
}

void FileSelector::AddEvent()
{
    GtkFileSelection *gfile = GTK_FILE_SELECTION(filesel);
    char *raw_path = NULL;
    GList *row = GTK_CLIST(gfile->file_list)->row_list;
    gint rownum = 0;
    gint sub_rnum = 0;
    char *temp, *path_temp;

    char *rawtext = gtk_entry_get_text(GTK_ENTRY(gfile->selection_entry));
    if (!strncasecmp("http://", rawtext, 7) ||
        !strncasecmp("rtp://", rawtext, 6)) {
        returnpath = strdup_new(rawtext);
        gtk_widget_destroy(GTK_WIDGET(gfile));
        ok = true;
        done = true;

        return;
    }

    returnpath = gtk_file_selection_get_filename(gfile);
    path_temp = strdup(returnpath.c_str());
    raw_path = 	gtk_entry_get_text(GTK_ENTRY(gfile->selection_entry));
    char *tempdir = strrchr(path_temp, '/');
    strncpy(old_path,path_temp,PATH_MAX);
    if (!tempdir) {
        strcpy(path_temp, "./");
        path_temp = (char *)realloc(path_temp, strlen(path_temp) + 1);
    }
    else
        tempdir[0] = '\0';
    if(row){
	while (row) {
    	    if (GTK_CLIST_ROW(row)->state == GTK_STATE_SELECTED) {
        	if (gtk_clist_get_cell_type(GTK_CLIST(gfile->file_list), rownum, 0)
            	    == GTK_CELL_TEXT) {
            	    gtk_clist_get_text(GTK_CLIST(gfile->file_list), rownum, 0, &temp);
		    if (!strcmp(temp, raw_path))
                	goto next_iter;
		    returnpath += "\n";
            	    returnpath += path_temp;
            	    returnpath += "/";
            	    returnpath += temp;
        	}
    	    }
	    next_iter:
    	    rownum++;
    	    row = g_list_next(row);
	}
    } 
    else {
	char subdirs[PATH_MAX];
	strncpy(subdirs,old_path,PATH_MAX);
	returnpath="";
	returnpath = recursive_path(subdirs, returnpath,&sub_rnum);
    }
    rownum += sub_rnum;
    gtk_widget_destroy(GTK_WIDGET(gfile));
    free(path_temp);

    ok = true;
    done = true;
}

bool FileSelector::Run(bool runMain)
{
    bool dummyBool;
    if (!runMain)
        gdk_threads_enter();

    filesel = gtk_file_selection_new(title.c_str());
    m_context->prefs->GetPrefBoolean(kSetLastResumePref, &dummyBool);
    if(dummyBool)
		gtk_file_selection_set_filename (GTK_FILE_SELECTION (filesel), old_path);
    gtk_window_set_modal(GTK_WINDOW(filesel), TRUE);
    gtk_signal_connect(GTK_OBJECT(filesel), "destroy",
                       GTK_SIGNAL_FUNC(filesel_destroy), (gpointer)runMain);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->cancel_button),
                       "clicked", GTK_SIGNAL_FUNC(cancel_internal), this);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
                       "clicked", GTK_SIGNAL_FUNC(ok_internal), this);

    if (extended)
        gtk_clist_set_selection_mode(GTK_CLIST(GTK_FILE_SELECTION(filesel)->
                                     file_list), GTK_SELECTION_EXTENDED);
    gtk_widget_show(filesel);

    if (runMain)
        gtk_main();
    else {
        gdk_threads_leave();
        while (!done)
            usleep(20);
    }

    if (ok)
       return true;
    return false;
}
