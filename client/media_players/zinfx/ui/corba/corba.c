/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-1999 EMusic.com
        Portions Copyright (C) 1999      Henrik Johnson 
        Portions Copyright (C) 2000      Martin Enlund

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
        
        $Id: corba.c,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/


#include <stdio.h>
#include <unistd.h>

#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include "orb/orbit.h"
#include "Freeamp.h"

extern int freeamp_toggle_pause(void *pclcio);
extern int freeamp_next(void *pclcio);
extern int freeamp_previous(void *pclcio);
extern int freeamp_quit(void *pclcio);
extern int freeamp_play(void *pclcio);
extern int freeamp_stop(void *pclcio);
extern int freeamp_addsong(void *pclcio, char *url);
extern int freeamp_getplaylist(void *pclcio, char ***lst);
extern int freeamp_playnumber(void *pclcio, char *number);
extern unsigned long freeamp_getnumber(void *pclcio);
extern int freeamp_deletenumber(void *pclcio, unsigned long number);

/* This is so we can get out a valid IOR later... */
Freeamp freeamp_client = CORBA_OBJECT_NIL;

static void *tptr = 0;

// #define CORBADEBUG 
static void corba_togglepause(PortableServer_Servant servant,
												 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_togglepause\n");
#endif
	freeamp_toggle_pause(tptr); 
#ifdef CORBADEBUG
	printf("<- corba_togglepause\n");
#endif
}

static void corba_previous(PortableServer_Servant servant,
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_previous\n");
#endif
	freeamp_previous(tptr);
#ifdef CORBADEBUG
	printf("<- corba_previous\n");
#endif
}

static void corba_next(PortableServer_Servant servant,
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_next\n");
#endif
	freeamp_next(tptr);
#ifdef CORBADEBUG
	printf("<- corba_next\n");
#endif
}

static void corba_play(PortableServer_Servant servant,
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_play\n");
#endif
	freeamp_play(tptr);
#ifdef CORBADEBUG
	printf("<- corba_play\n");
#endif
}

static void corba_stop(PortableServer_Servant servant,
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_stop\n");
#endif
	freeamp_stop(tptr);
#ifdef CORBADEBUG
	printf("<- corba_stop\n");
#endif
}

static void corba_quit(PortableServer_Servant servant,
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_quit\n");
#endif
	freeamp_quit(tptr);
#ifdef CORBADEBUG
	printf("<- corba_quit\n");
#endif
}

static void corba_addsong(PortableServer_Servant servant, const CORBA_char *url, 
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_addsong\n");
#endif
	freeamp_addsong(tptr, (char *)url);
#ifdef CORBADEBUG
	printf("<- corba_addsong\n");
#endif
}

static void corba_getplaylist(PortableServer_Servant servant, Freeamp_StringList** list, CORBA_Environment *ev) {
	int i=0;
	char **p;
	int num_entries = freeamp_getplaylist(tptr, &p);
#ifdef CORBADEBUG
	printf("-> corba_getplaylist %d\n", num_entries);
#endif
	*list = CORBA_sequence_CORBA_string__alloc();
	(*list)->_buffer = CORBA_sequence_CORBA_string_allocbuf(num_entries);
	(*list)->_length = num_entries;
	for (i=0; i<num_entries; i++) {
		(*list)->_buffer[i] = CORBA_string_dup(p[i]);
		free(p[i]);
		p[i]=0;
	}
	free(p);
#ifdef CORBADEBUG
	printf("<- corba_getplaylist\n");
#endif
}

static void corba_playnumber(PortableServer_Servant servant, const CORBA_char *number, 
													 CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_playnumber\n");
#endif
	freeamp_playnumber(tptr, (char *)number);
#ifdef CORBADEBUG
	printf("<- corba_playnumber\n");
#endif
}

static CORBA_long corba_getnumber(PortableServer_Servant servant, CORBA_Environment *ev) {
	CORBA_long fisk;
#ifdef CORBADEBUG
	printf("-> corba_getnumber\n");
#endif
	fisk = freeamp_getnumber(tptr);
#ifdef CORBADEBUG
	printf("<- corba_getnumber\n");
#endif
	return fisk;
}

static void corba_deletenumber(PortableServer_Servant servant, const CORBA_long number,
														CORBA_Environment *ev) {
#ifdef CORBADEBUG
	printf("-> corba_deletenumber\n");
#endif
	freeamp_deletenumber(tptr, number);
#ifdef CORBADEBUG
	printf("<- corba_deletenumber\n");
#endif
}


/*
 * I have **no** idea what this bit does
 */
PortableServer_ServantBase__epv base_epv = {
  NULL,
  NULL,
  NULL
};

/* NULL REMOVED */
POA_Freeamp__epv freeamp_epv = { NULL,
																 corba_togglepause, 
																 corba_previous,
																 corba_next,
																 corba_quit,
																 corba_play,
																 corba_stop,
																 corba_addsong,
																 
																 corba_playnumber,
																 corba_getplaylist, 
																 corba_getnumber,
																 corba_deletenumber
};

POA_Freeamp__vepv poa_freeamp_vepv = { &base_epv, &freeamp_epv };
POA_Freeamp poa_freeamp_servant = { NULL, &poa_freeamp_vepv };

void corba_init(void *p) {
        PortableServer_ObjectId objid = {0, sizeof("Freeamp-ORBit"), "Freeamp-ORBit"};
        PortableServer_POA poa;
        
        CORBA_Environment ev;
        char *retval;
        CORBA_ORB orb;
        FILE * ofp;
        int argc=1;
        char *argv;
				char *homedir;
				char iorfile[1024];
        char tmpbuf[1] = {0};
        tptr = p;

				// :TODO: this should be fixed to pass on argv?
				argv=&tmpbuf[0]; 
        
        CORBA_exception_init(&ev); 
        orb = CORBA_ORB_init(&argc, &argv, "orbit-local-orb", &ev);
        POA_Freeamp__init(&poa_freeamp_servant, &ev);
        poa = (PortableServer_POA)CORBA_ORB_resolve_initial_references(orb, "RootPOA", &ev);
        PortableServer_POAManager_activate(PortableServer_POA__get_the_POAManager(poa, &ev), &ev);
        PortableServer_POA_activate_object_with_id(poa, &objid, &poa_freeamp_servant, &ev);
        freeamp_client = PortableServer_POA_servant_to_reference(poa, &poa_freeamp_servant, &ev);
        if (!freeamp_client) {
					printf("Cannot get objref\n");
					return;
        }
				
				homedir = getenv("HOME");
				strncpy(iorfile, homedir, 1024); // :TODO: add error checking
				strncat(iorfile, "/.freeamp.ior", 1024);
        retval = CORBA_ORB_object_to_string(orb, freeamp_client, &ev);
        ofp = fopen(iorfile,"w");
        fprintf(ofp,"%s", retval);
        fclose(ofp);
        
        CORBA_free(retval);
        
        fprintf(stdout,"FreeAMP-CORBA Ready. | Waiting for commands.\n");
        fflush(stdout);
        CORBA_ORB_run(orb, &ev);
}


