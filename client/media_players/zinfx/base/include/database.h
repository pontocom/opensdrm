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

        $Id: database.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/


#ifndef INCLUDED_DATABASE_H_
#define INCLUDED_DATABASE_H_

#include "gdbm_fa.h"
#include "mutex.h"

class Database
{
 public:
   Database(const char *name = NULL, int version = -1);
   ~Database();
   bool  Working(void);
   
   int   Insert(const char *key, const char *content);
   void  Remove(const char *key);
   char *Value(const char *key);
   int   Exists(const char *key);
   char *NextKey(char *key);
   void  Sync(void);

   bool  IsUpgraded(void);
  
   int   GetSubVersion(void);
   void  StoreSubVersion(int version);

 private:
   bool  TestDatabaseVersion(int version);
   void  StoreDatabaseVersion(int version);

   GDBM_FILE m_dbase;
   Mutex    *m_lock;
   bool      m_upgraded;
};

#endif
