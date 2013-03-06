/*____________________________________________________________________________
       
       FreeAmp - The Free MP3 Player

       Portions Copyright (C) 1998-2000 EMusic.com

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
       
       $Id: ColumnInfo.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/


#ifndef _COLUMNINFO_H_
#define _COLUMNINFO_H_

#include <string>
#include <vector>

using namespace std;

/*
 * Simple holder class, for holding column data.
 */
class ColumnHolder
{
  public:
    void SetText( char *text ) { m_text = text; };
    const char *GetText() const { return( m_text.c_str() ); };

    void SetIndex( int index ) { m_index = index; };
    int GetIndex() const { return( m_index ); };

  private:
    int m_index;
    string m_text;
};

/*
 * Holds a collection of Column Name + Index pairs.
 *
 * These are used to find the mapping between an original
 * column, and its current index - This is necessary because
 * we cannot assume that, say, column 1 is the Artist, etc.
 *
 *  Steve
 *  ---
 *
 */
class ColumnInfo
{
  public:
     /*
      * Add a new entry.
      */
     void AddColumn( char *title, int index )
     {
         ColumnHolder *ch = new ColumnHolder();
         ch->SetText( title );
         ch->SetIndex( index );
         m_columnData.push_back( ch );
     };
  
     /*
      * Find the title of the column with the particular index.
      */
     const char *FindTitle ( int index )
     {
         for( int i = 0; i < m_columnData.size(); i++ )
         {
             ColumnHolder *ch = m_columnData[i];
             if ( index == ch->GetIndex() )
                 return( ch->GetText() );
         }
         return( NULL );
     };

     /*
      * Find the index of the column with the particular text.
      */
     int FindColumn ( char *text )
     {
         for( int i = 0; i < m_columnData.size(); i++ )
         {
             ColumnHolder *ch = m_columnData[i];
             if ( stricmp( ch->GetText(), text ) == 0 )
                 return( ch->GetIndex() );
         }
         return( 0 );
     };
  
     int GetNColumns()
     {
         return( m_columnData.size() );
     };

     void Erase()
     {
         while (m_columnData.size() > 0) 
         {
             delete m_columnData[0];
             m_columnData.erase(m_columnData.begin());
         }
     }

  private:
     vector<ColumnHolder *> m_columnData;
};

#endif /* _COLUMNINFO_H_ */

