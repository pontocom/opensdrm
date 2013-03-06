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

        $Id: database.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "gdbm_fa.h"
#include "errors.h"
#include "database.h"

#ifdef WIN32
#define S_IRWXU _S_IREAD|_S_IWRITE
#endif

#define DATABASE_VERSION_KEY "FREEAMP_DATABASE_VERSION"
#define SUB_VERSION_KEY      "FREEAMP_SUB_VERSION"

Database::Database(const char *name, int version)
{
    m_dbase = NULL;

    assert(name);

    m_lock = new Mutex();
    m_dbase = gdbm_open((char *)name, 0, GDBM_WRCREAT|GDBM_NOLOCK|GDBM_SYNC, 
                        S_IRWXU, NULL);
    
    m_upgraded = false;
    assert(m_dbase);

    if (version >= 0) {
        if (!TestDatabaseVersion(version)) {
            gdbm_close(m_dbase);
            m_dbase = gdbm_open((char *)name, 0, 
                                GDBM_NEWDB|GDBM_NOLOCK|GDBM_SYNC, S_IRWXU, 
                                NULL);
            assert(m_dbase);
            m_upgraded = true;
        }
        StoreDatabaseVersion(version);
    }
}

Database::~Database()
{
    m_lock->Acquire();
    if (m_dbase) {
        gdbm_sync(m_dbase);
        gdbm_close(m_dbase);
    }
    delete m_lock;
}

bool Database::Working(void)
{
    if (!m_dbase)
        return false;
    return true;
}

bool Database::IsUpgraded(void)
{
    return m_upgraded;
}

int Database::Insert(const char *key, const char *content)
{
    datum gdbmKey;
    datum gdbmContent;
    int returnValue;

    gdbmKey.dptr = (char *)key;
    gdbmKey.dsize = strlen(key) * sizeof(char) + 1;
    gdbmContent.dptr = (char *)content;
    gdbmContent.dsize = strlen(content) * sizeof(char) + 1;

    m_lock->Acquire();
    returnValue = gdbm_store(m_dbase, gdbmKey, gdbmContent, GDBM_REPLACE);
    m_lock->Release();

   if (returnValue != 0)
       return kError_DbaseItemNotStored;
   return kError_NoErr;
}

void Database::Remove(const char *key)
{
    datum gdbmKey;

    gdbmKey.dptr = (char *)key;
    gdbmKey.dsize = strlen(key) + 1;

    m_lock->Acquire();
    gdbm_delete(m_dbase, gdbmKey);
    m_lock->Release();
}

char *Database::Value(const char *key)
{
    datum gdbmKey;
    datum returnData;

    gdbmKey.dptr = (char *)key;
    gdbmKey.dsize = strlen(key) + 1;

    m_lock->Acquire();
    returnData = gdbm_fetch(m_dbase, gdbmKey);
    m_lock->Release();

    if (returnData.dptr == NULL)
       return NULL; // deal with not found error..

    char *returninfo = new char[returnData.dsize + 1];
    strcpy(returninfo, returnData.dptr);
    free(returnData.dptr);

    return returninfo;
}

int Database::Exists(const char *key)
{
    datum gdbmKey;
    int found;

    gdbmKey.dptr = (char *)key;
    gdbmKey.dsize = strlen(key) + 1;

    m_lock->Acquire();
    found = gdbm_exists(m_dbase, gdbmKey);
    m_lock->Release();

    return found;
}

char *Database::NextKey(char *key)
{
    datum returnKey;
    char *nextKey;

    m_lock->Acquire();
    if (key)
    {
        datum gdbmKey;
        gdbmKey.dptr = (char *)key;
        gdbmKey.dsize = strlen(key) + 1;

        returnKey = gdbm_nextkey(m_dbase, gdbmKey);

        delete key;
    }
    else
        returnKey = gdbm_firstkey(m_dbase);

    m_lock->Release();

    if (!returnKey.dptr) 
        return NULL;
 
    nextKey = new char[returnKey.dsize + 1]; 
    strcpy(nextKey, returnKey.dptr);

    free(returnKey.dptr);

    if ((nextKey != NULL) && (!strcmp(DATABASE_VERSION_KEY, nextKey)))
        nextKey = NextKey(nextKey);

    return nextKey;
}

void Database::Sync(void)
{
    m_lock->Acquire();
    gdbm_sync(m_dbase);
    m_lock->Release();
}

int Database::GetSubVersion(void)
{
    int sub_ver = 0;
    char *stored_ver = NULL;

    stored_ver = Value(SUB_VERSION_KEY);

    if (!stored_ver)
        sub_ver = 0;
    else
        sub_ver = atoi(stored_ver);

    delete [] stored_ver;
    return sub_ver;
}

void Database::StoreSubVersion(int version)
{
    char version_store[15];

    sprintf(version_store, "%d", version);

    Insert(SUB_VERSION_KEY, version_store);
}

bool Database::TestDatabaseVersion(int version)
{
    int database_ver = 0;
    char *stored_ver = NULL;
    
    stored_ver = Value(DATABASE_VERSION_KEY);
 
    if (!stored_ver)
        return false;

    database_ver = atoi(stored_ver);

    delete [] stored_ver;

    if (version != database_ver)
        return false;
    return true;
}

void Database::StoreDatabaseVersion(int version)
{
    char version_store[15];
    
    sprintf(version_store, "%d", version);

    Insert(DATABASE_VERSION_KEY, version_store);
}
