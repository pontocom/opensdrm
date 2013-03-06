/*____________________________________________________________________________

  FreeAmp - The Free MP3 Player

           Copyright (C) 2000 Michael Rich
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

  $Id: wav.h,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_WAV_H_
#define INCLUDED_WAV_H_

#ifndef WIN32
typedef unsigned int DWORD;
#endif

struct WaveHeader
{
	DWORD MainChunkID;			// it will be 'RIFF'
	DWORD Length;
	DWORD ChunkTypeID;			// it will be 'WAVE'
	DWORD SubChunkID;			// it will be 'fmt '
	DWORD SubChunkLength;
	unsigned short Format;
	unsigned short Modus;
	DWORD SampleFreq;
	DWORD BytesPerSec;
	unsigned short BytesPerSample;
	unsigned short BitsPerSample;
	DWORD DataChunkID;			// it will be 'data'
	DWORD DataLength;
};

#ifndef WIN32
#define WAVE_FORMAT_PCM 1
struct WAVEFORMATEX
{
   DWORD              cbSize;
	unsigned short     wFormatTag;
	unsigned short     nChannels;
	DWORD              nSamplesPerSec;
	DWORD              nAvgBytesPerSec;
	unsigned short     nBlockAlign;
	unsigned short     wBitsPerSample;
};
#endif

class WaveWriter
{
public:
	WaveWriter();
	~WaveWriter();

	int Create(const char *FileName, WAVEFORMATEX *waveformat);
	void Close();

	DWORD Write(const char *data, DWORD data_size);

private:

   DWORD StuffFourChars(char one, char two, char three, char four); 

	FILE		*m_FP;
	struct WaveHeader	m_WH;
};

#endif /* INCLUDED_WAV_H_ */
