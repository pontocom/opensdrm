// $Id: utils.cpp,v 1.1 2004/10/13 16:16:11 linha2 Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#include "id3config.h"
#include "utils.h"

#ifndef WIN32
#include <unistd.h>
#include <iostream>
#include <fstream.h>
#endif

#if defined HAVE_ICONV_H
# include <iconv.h>
# include <errno.h>
#endif


using namespace dami;

size_t dami::renderNumber(uchar *buffer, uint32 val, size_t size)
{
  uint32 num = val;
  for (size_t i = 0; i < size; i++)
  {
    buffer[size - i - 1] = (uchar)(num & MASK8);
    num >>= 8;
  }
  return size;
}

String dami::renderNumber(uint32 val, size_t size)
{
  String str(size, '\0');
  uint32 num = val;
  for (size_t i = 0; i < size; i++)
  {
    str[size - i - 1] = (uchar)(num & MASK8);
    num >>= 8;
  }
  return str;
}


namespace 
{
#if !defined(HAVE_ICONV_H)
  // converts an ASCII string into a Unicode one
  String mbstoucs(String data)
  {
    size_t size = data.size();
    String unicode(size * 2, '\0');
    for (index_t i = 0; i < size; i++)
    {
      unicode[i*2+1] = toascii(data[i]);
    }
    return unicode;
  }

  // converts a Unicode string into ASCII
  
  String ucstombs(String data)
  {
    size_t size = data.size() / 2;
    String ascii(size, '\0');
    for (index_t i = 0; i < size; i++)
    {
      ascii[i] = toascii(data[i*2+1]);
    }
    return ascii;
  }
#else

  String convert_i(iconv_t cd, String source)
  {
    String target;
    size_t source_size = source.size();
    const char* source_str = source.data();
#define BUFSIZ 1024
    char buf[BUFSIZ];
    char* target_str = buf;
    size_t target_size = BUFSIZ;
    
    do
    {
      size_t nconv = iconv(cd, 
                           &source_str, &source_size, 
                           &target_str, &target_size);
      if (nconv == (size_t) -1 && errno != EINVAL && errno != E2BIG)
      {
        return target;
      }
      target.append(buf, BUFSIZ - target_size);
      target_str = buf;
      target_size = BUFSIZ;
    }
    while (source_size > 0);
    return target;
  }

  const char* getFormat(ID3_TextEnc enc)
  {
    const char* format = NULL;
    switch (enc)
    {
      case ID3TE_ASCII:
        format = ID3_ICONV_FORMAT_ASCII;
        break;

      case ID3TE_UTF16:
        format = ID3_ICONV_FORMAT_UTF16;
        break;
        
      case ID3TE_UTF16BE:
        format = ID3_ICONV_FORMAT_UTF16BE;
        break;
        
      case ID3TE_UTF8:
        format = ID3_ICONV_FORMAT_UTF8;
        break;
        
      default:
        break;
    }
    return format;
  }
#endif
}

String dami::convert(String data, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
  String target;
#if !defined HAVE_ICONV_H
#define ID3_IS_ASCII(enc)      \
  ((enc) == ID3TE_ASCII     || \
   (enc) == ID3TE_ISO8859_1 || \
   (enc) == ID3TE_UTF8)
#define ID3_IS_UNICODE(enc)    \
  ((enc) == ID3TE_UNICODE   || \
   (enc) == ID3TE_UTF16     || \
   (enc) == ID3TE_UTF16BE)
  if (ID3_IS_ASCII(sourceEnc) && ID3_IS_UNICODE(targetEnc))
  {
    target = mbstoucs(data);
  }
  else if (ID3_IS_UNICODE(sourceEnc) && ID3_IS_ASCII(targetEnc))
  {
    target = ucstombs(data);
  }
#else
  if (sourceEnc != targetEnc)
  {
    const char* targetFormat = getFormat(targetEnc);
    const char* sourceFormat = getFormat(sourceEnc);
    
    iconv_t cd = iconv_open (targetFormat, sourceFormat);
    if (cd != (iconv_t) -1)
    {
      target = convert_i(cd, data);
    }
    iconv_close (cd);
  }
#endif
  return target;
}

size_t dami::ucslen(const unicode_t *unicode)
{
  if (NULL != unicode)
  {
    for (size_t size = 0; true; size++)
    {
      if (NULL_UNICODE == unicode[size])
      {
        return size;
      }
    }
  }
  return 0;
}

#ifdef WIN32
namespace {
  bool exists(String name)
  {
    ifstream file(name.c_str(), ios::nocreate);
    return file.is_open() != 0;
  }
}
#else

using namespace std;
  bool exists(String name)
  {
    return access(name.c_str(), F_OK) == 0;
  }
using namespace dami;

#endif


ID3_Err dami::createFile(String name, fstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
    
  file.open(name.c_str(), ios::out | ios::binary | ios::trunc);
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

size_t dami::getFileSize(fstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellg();
    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(curpos);
  }
  return size;
}

size_t dami::getFileSize(ifstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellg();
    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(curpos);
  }
  return size;
}

size_t dami::getFileSize(ofstream& file)
{
  size_t size = 0;
  if (file.is_open())
  {
    streamoff curpos = file.tellp();
    file.seekp(0, ios::end);
    size = file.tellp();
    file.seekp(curpos);
  }
  return size;
}

ID3_Err dami::openWritableFile(String name, fstream& file)
{
  if (!exists(name))
  {
    return ID3E_NoFile;
  }
    
  if (file.is_open())
  {
    file.close();
  }
  file.open(name.c_str(), ios::in | ios::out | ios::binary);
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openWritableFile(String name, ofstream& file)
{
  if (!exists(name))
  {
    return ID3E_NoFile;
  }
    
  if (file.is_open())
  {
    file.close();
  }
  file.open(name.c_str(), ios::in | ios::out | ios::binary );
  if (!file)
  {
    return ID3E_ReadOnly;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openReadableFile(String name, fstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
  file.open(name.c_str(), ios::in | ios::binary);
  if (!file)
  {
    return ID3E_NoFile;
  }
    
  return ID3E_NoError;
}

ID3_Err dami::openReadableFile(String name, ifstream& file)
{
  if (file.is_open())
  {
    file.close();
  }
  file.open(name.c_str(), ios::in | ios::binary);
  if (!file)
  {
    return ID3E_NoFile;
  }
    
  return ID3E_NoError;
}

String dami::toString(uint32 val)
{
  if (val == 0)
  {
    return "0";
  }
  String text;
  while (val > 0)
  {
    String tmp;
    char ch = (val % 10) + '0';
    tmp += ch;
    text = tmp + text;
    val /= 10;
  }
  return text;
}

WString dami::toWString(const unicode_t buf[], size_t len)
{
  WString str;
  str.reserve(len);

  for (size_t i = 0; i < len; ++i)
  {
    str += static_cast<WString::value_type>(buf[i]);
  }
  return str;
}
