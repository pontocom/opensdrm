#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cassert>
#include <io.h>

using namespace std;

#pragma warning(disable:4267)
#include "aes.h"
#include "filters.h"
#include "hex.h"
#include "modes.h"

unsigned char hex2bin(char hex)
{
    return (hex >= '0' && hex <= '9') ?  hex - '0' : 
          ((hex >= 'A' && hex <= 'F') ?  hex - 'A' + 10 :
        /* (hex >= 'a' && hex <= 'f') */ hex - 'a' + 10); 
}

using namespace CryptoPP;

// Current issues:
//    CRC - we're not recalculating CRCs after ciphering the blocks.
//    ID3v2 - we don't support ID3v2 tags.

struct MP3Header
{
	unsigned emphasis    :  2;
	unsigned original    :  1;
	unsigned copyright   :  1;
	unsigned mode_ext    :  2;
	unsigned mode        :  2;
	unsigned private_    :  1;
	unsigned padding     :  1;
	unsigned frequency   :  2;
	unsigned bitrate     :  4;
	unsigned protection  :  1;
	unsigned layer       :  2;
	unsigned version     :  2;
	unsigned eleven_ones : 11;

	unsigned short crc;

	long pos;
};

struct DoRM_Box
{
	// BEGIN HEADER
	char dorm    [4]; // DoRM
	char version [2]; // version : 00
	char reserved[2]; // reserved: 00
	char length  [4]; // length  : 36
	// END HEADER
	
	char prot_id [4]; // protection system: E000001

	// BEGIN BODY
	char cid    [16]; // content id: 00
	char iv     [16]; // IV for AES: IV
	// END BODY
};

SecByteBlock HexDecodeString(const char *hex)
{
	StringSource ss(hex, true, new HexDecoder);
	SecByteBlock result(ss.MaxRetrievable());
	ss.Get(result, result.size());
	return result;
}

bool checkHeader(const MP3Header * pHeader)
{
	if (pHeader->eleven_ones != 0x07FF) return false; // unexpected
	if (pHeader->version     ==  0x01 ) return false; // unexpected
	if (pHeader->layer       ==  0x00 ) return false; // unexpected
	if (pHeader->bitrate     ==  0x00 ) return false; // unsupported
	if (pHeader->bitrate     ==  0x0F ) return false; // invalid
	if (pHeader->frequency   ==  0x03 ) return false; // unexpected
	if (pHeader->emphasis    ==  0x02 ) return false; // unexpected
	return true;
}

struct ID3v1
{
	char tag    [ 3];
	char title  [30];
	char artist [30];
	char album  [30];
	char year   [ 4];
	union {
		char comment [30];
		struct {
			char comment [28];
			unsigned char zero;
			unsigned char track;
		} v1_1;
	};
	unsigned char genre;
};

bool checkID3v1(const ID3v1 * pID3v1)
{
	return memcmp(pID3v1->tag, "TAG", 3) == 0;
}

bool isID3v1_1(const ID3v1 * pID3v1)
{
	return pID3v1->v1_1.zero == '\0' && pID3v1->v1_1.track != '\0';
}

const char * GENRE[148] =
{
	"Blues", "Classic Rock", "Country", "Dance",
	"Disco", "Funk", "Grunge", "Hip-Hop",
	"Jazz", "Metal", "New Age", "Oldies",
	"Other", "Pop", "R&B", "Rap",
	"Reggae", "Rock", "Techno", "Industrial",
	"Alternative", "Ska", "Death Metal", "Pranks",
	"Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
	"Vocal", "Jazz+Funk", "Fusion", "Trance",
	"Classical", "Instrumental", "Acid", "House",
	"Game", "Sound Clip", "Gospel", "Noise",
	"AlternRock", "Bass", "Soul", "Punk",
	"Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
	"Electronic", "Pop-Folk", "Eurodance", "Dream",
	"Southern Rock", "Comedy", "Cult", "Gangsta",
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle",
	"Native American", "Cabaret", "New Wave", "Psychadelic",
	"Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka",
	"Retro", "Musical", "Rock & Roll", "Hard Rock",
	// WinAmp extensions
	"Folk", "Folk-Rock", "National Folk", "Swing",
	"Fast Fusion", "Bebob", "Latin", "Revival",
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
	"Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
	"Big Band", "Chorus", "Easy Listening", "Acoustic",
	"Humour", "Speech", "Chanson", "Opera",
	"Chamber Music", "Sonata", "Symphony", "Booty Brass",
	"Primus", "Porn Groove", "Satire", "Slow Jam",
	"Club", "Tango", "Samba", "Folklore",
	"Ballad", "Poweer Ballad", "Rhytmic Soul", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "A Capela",
	"Euro-House", "Dance Hall", "Goa", "Drum & Bass",
	"Club-House", "Hardcore", "Terror", "Indie",
	"BritPop", "Negerpunk", "Polsk Punk", "Beat",
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
	"Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
	"Trash Metal", "Anime", "Jpop", "Synthpop" 
};

const char * getGenre(unsigned char genre)
{
	return (genre > 147) ? "Unknown" : GENRE[genre];
}

#define ISO_VERSION(version) ((version) & 0x01)
const unsigned int ISO_MPEG_1 = 1;
const unsigned int ISO_MPEG_2 = 0;

const unsigned int MPEG_1_0 = 3;
const unsigned int MPEG_2_0 = 2;
const unsigned int MPEG_2_5 = 0;

const char * VERSION[4] =
{
	"MPEG Version 2.5 (later extension of MPEG 2)",
	"unknown",
	"MPEG Version 2 (ISO/IEC 13818-3)",
	"MPEG Version 1 (ISO/IEC 11172-3)"
};

const unsigned int LAYER_I   = 3;
const unsigned int LAYER_II  = 2;
const unsigned int LAYER_III = 1;

const char * LAYER[4] =
{
	"unknown",
	"Layer III",
	"Layer II",
	"Layer I"
};

const unsigned int BR_FREE = 0x00000000;
const unsigned int BR_BAD  = 0xFFFFFFFF;

unsigned int BITRATE_MPEG1_LAYERI  [16] = { BR_FREE, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, BR_BAD };
unsigned int BITRATE_MPEG1_LAYERII [16] = { BR_FREE, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, BR_BAD }; 
unsigned int BITRATE_MPEG1_LAYERIII[16] = { BR_FREE, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, BR_BAD };

unsigned int BITRATE_MPEG2_LAYERI  [16] = { BR_FREE, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, BR_BAD };
unsigned int BITRATE_MPEG2_LAYERII [16] = { BR_FREE,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, BR_BAD };
unsigned int BITRATE_MPEG2_LAYERIII[16] = { BR_FREE,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, BR_BAD };

unsigned int * BITRATE[2][4] =
{
	{ NULL, BITRATE_MPEG2_LAYERIII, BITRATE_MPEG2_LAYERII, BITRATE_MPEG2_LAYERI },
	{ NULL, BITRATE_MPEG1_LAYERIII, BITRATE_MPEG1_LAYERII, BITRATE_MPEG1_LAYERI }
};

unsigned int getBitrate(MP3Header * pHeader)
{
	return BITRATE[ISO_VERSION(pHeader->version)][pHeader->layer][pHeader->bitrate];
}

const unsigned int SF_RESERVED = 0;

unsigned int FREQUECY_MPEG1 [4] = { 44100, 48000, 32000, SF_RESERVED };
unsigned int FREQUECY_MPEG2 [4] = { 22050, 24000, 16000, SF_RESERVED };
unsigned int FREQUECY_MPEG25[4] = { 11025, 12000,  8000, SF_RESERVED };

unsigned int * FREQUENCY[4] = { FREQUECY_MPEG25, NULL, FREQUECY_MPEG2, FREQUECY_MPEG1  };

unsigned int getFrequency(MP3Header * pHeader)
{
	return FREQUENCY[pHeader->version][pHeader->frequency];
}

const unsigned int STEREO         = 0;
const unsigned int JOINT_STEREO   = 1;
const unsigned int DUAL_CHANNEL   = 2;
const unsigned int SINGLE_CHANNEL = 3;

const char * MODE[4] =
{
	"Stereo",
	"Joint stereo (Stereo)",
	"Dual channel (2 mono channels)",
	"Single channel (Mono)"
};

const char * MODE_EXT_LAYER_I_II[4] =
{
	"Intensity stereo on bands 4 to 31",
	"Intensity stereo on bands 8 to 31",
	"Intensity stereo on bands 12 to 31",
	"Intensity stereo on bands 16 to 31"
};

const char * MODE_EXT_LAYER_III[4] =
{
	"Intensity stereo OFF; MS stereo OFF",
	"Intensity stereo ON; MS stereo OFF",
	"Intensity stereo OFF; MS stereo ON",
	"Intensity stereo ON; MS stereo ON"
};

const unsigned int EMPH_NONE      = 0;
const unsigned int EMPH_50_15     = 1;
const unsigned int EMPH_RESERVED  = 2;
const unsigned int EMPH_CCITT_J17 = 3;

const char * EMPHASIS[4] =
{
	"none",
	"50/15 ms",
	"unknown",
	"CCITT J.17"
};

int getFrameLength(MP3Header * pHeader)
{
	int frame_length = -1;
	if (pHeader->bitrate != 0 /*BR_FREE*/ && pHeader->bitrate != 15 /*BR_BAD*/ && pHeader->frequency != 3/*SF_RESERVED*/) {
		if (pHeader->layer == LAYER_I) {
			frame_length = (12 * getBitrate(pHeader) * 1024 / getFrequency(pHeader) + pHeader->padding) * 4;
        } else if (pHeader->layer == LAYER_II || !(pHeader->version == MPEG_2_0)) {
			frame_length = 144 * getBitrate(pHeader) * 1000 / getFrequency(pHeader) + pHeader->padding;
        } else {
			frame_length = 72 * getBitrate(pHeader) * 1000 / getFrequency(pHeader) + pHeader->padding;
		}
	}
	return frame_length;
}

void printHeader(MP3Header * pHeader, const char * prefix = "")
{
	printf("%sLocation   : %d\n", prefix, pHeader->pos);

	printf("%sVersion    : %s\n", prefix, VERSION[pHeader->version]);
	
	printf("%sLayer      : %s\n", prefix, LAYER[pHeader->layer]);
	
	printf("%sProtection : %s\n", prefix, (pHeader->protection == 0x00) ?
	                                      "yes" :
	                                      "no");
	
	printf("%sBitrate    : ", prefix);
	(pHeader->bitrate == 0x00 || pHeader->bitrate == 0x0F) ?
		printf((pHeader->bitrate == 0x00) ? "free" : "bad") :
		printf("%u kbps", getBitrate(pHeader));
	printf("\n");

	printf("%sFrequency  : ", prefix);
		(pHeader->frequency == 0x03) ?
			printf("unknown\n") :
			printf("%u Hz\n", getFrequency(pHeader));

	printf("%sPadding    : %s", prefix, (pHeader->padding == 0x00) ? "no" : "yes");
	if (pHeader->padding == 0x01 && pHeader->layer != 0x00) { // reserved
		printf(" (%d bits)", (pHeader->layer == LAYER_I) ? 32 : 8);
	}
	printf("\n");
	
	printf("%sPrivate    : %s\n", prefix, (pHeader->private_ == 0x00) ?
	                                      "no" :
	                                      "yes");
	
	printf("%sMode       : %s\n", prefix, MODE[pHeader->mode]);

	printf("%sMode ext.  : ", prefix);
	if (pHeader->mode == JOINT_STEREO) {
		if (pHeader->layer == LAYER_I || pHeader->layer == LAYER_II) {
			printf("%s\n", MODE_EXT_LAYER_I_II[pHeader->mode_ext]);
		} else if (pHeader->layer == LAYER_III) {
			printf("%s\n", MODE_EXT_LAYER_III[pHeader->mode_ext]);
		} else {
			printf("unknown\n");
		}
	} else {
		printf("n/a\n");
	}

	printf("%sCopyright  : %s\n", prefix, (pHeader->copyright == 0x00) ?
	                                      "no" :
	                                      "yes");
	
	printf("%sOriginal   : %s\n", prefix, (pHeader->original == 0x00) ?
	                                      "no" :
	                                      "yes");
	
	printf("%sEmphasis   : %s\n", prefix, EMPHASIS[pHeader->emphasis]);

	if (pHeader->protection == 0x00) {
		printf("%sCRC        : %04hX\n", pHeader->crc);
	}

	int frame_length = getFrameLength(pHeader);
	if (frame_length != -1) {
		printf("%sFrame len  : %d\n", prefix, frame_length);
	} else {
		printf("%sFrame len  : unknown\n", prefix);
	}
}

bool findNextSync(FILE * mp3_file)
{
	int b = 0;
	while ((b = getc(mp3_file)) != EOF) {
		if (b == 0xFF) {
			b = getc(mp3_file);
			if (b == EOF) {
				return false;
			} else if ((b & 0xE0) == 0xE0) {
				ungetc(b, mp3_file);
				return true;
			}
		}
	}
	return false;
}

bool readHeader(FILE * mp3_file, void * pSpaceForHeader, bool fromFF = false)
{
	int b;

	if (fromFF == false) {
		((MP3Header*)pSpaceForHeader)->pos = ftell(mp3_file) - 1;
	} else {
		((MP3Header*)pSpaceForHeader)->pos = ftell(mp3_file);
		b = getc(mp3_file);
		if (b == EOF)  return false;
		if (b != 0xFF) return false;
	}

	((unsigned char *)pSpaceForHeader)[3] = 0xFF;
	b = getc(mp3_file); if (b == EOF) return false;
	((unsigned char *)pSpaceForHeader)[2] = b;
	b = getc(mp3_file); if (b == EOF) return false;
	((unsigned char *)pSpaceForHeader)[1] = b;
	b = getc(mp3_file); if (b == EOF) return false;
	((unsigned char *)pSpaceForHeader)[0] = b;

	((MP3Header*)pSpaceForHeader)->crc = 0;
	if (((MP3Header*)pSpaceForHeader)->protection == 1) {
		if (fread(&(((MP3Header*)pSpaceForHeader)->crc), 2, 1, mp3_file) != 1) {
			return false;
		}
	}

	return true;
}

bool findNextHeader(FILE * mp3_file, void * pSpaceForHeader)
{
	bool success = false;
	long mark;
	for (;;) {
		if (findNextSync(mp3_file)) {
			mark = ftell(mp3_file);
			if (readHeader(mp3_file, pSpaceForHeader)) {
				if (checkHeader((MP3Header*)pSpaceForHeader)) {
					success = true;
					break;
				} else {
					fseek(mp3_file, mark, SEEK_SET);
				}
			} else break;
		} else break;
	}
	return success;
}

int synch_tries;
bool synchronize(FILE * mp3_file)
{
//printf("\n\nSynchronizing...\n");
	long pos_1st;
	long pos_2nd;
	int  len_1st;
	int  len_2nd;

	MP3Header mp3_header;
	for (synch_tries = 1; ; ++synch_tries) {
		if (!findNextHeader(mp3_file, &mp3_header)) return false;
//printf("\n1st header instance\n");
//printHeader(&mp3_header);
		pos_1st = mp3_header.pos;
		len_1st = getFrameLength(&mp3_header);
		if (len_1st > 0) {
			fseek(mp3_file, pos_1st + len_1st, SEEK_SET);
			if (readHeader(mp3_file, &mp3_header, true) && checkHeader(&mp3_header)) {
//printf("\n2nd header instance\n");
//printHeader(&mp3_header);
				pos_2nd = mp3_header.pos;
				len_2nd = getFrameLength(&mp3_header);
				if (len_2nd > 0) {
					fseek(mp3_file, pos_2nd + len_2nd, SEEK_SET);
					if (readHeader(mp3_file, &mp3_header, true) && checkHeader(&mp3_header)) {
//printf("\n3rd header instance\n");
//printHeader(&mp3_header);
						fseek(mp3_file, pos_1st, SEEK_SET);
						return true;
					}
				}
			}
		}
		fseek(mp3_file, pos_1st + 1, SEEK_SET);
	}
}

bool readID3v1(FILE * mp3_file, void * pSpaceForID3v1)
{
	return fread(pSpaceForID3v1, sizeof(ID3v1), 1, mp3_file) == 1;
}

void printID3v1(ID3v1 * pID3v1, const char * prefix = "")
{
	printf("%sTitle   : %.*s\n", prefix, 30, pID3v1->title); 
	printf("%sArtist  : %.*s\n", prefix, 30, pID3v1->artist); 
	printf("%sAlbum   : %.*s\n", prefix, 30, pID3v1->album); 
	printf("%sYear    : %.*s\n", prefix,  4, pID3v1->year); 
	if (!isID3v1_1(pID3v1)) {
		printf("%sComment : %.*s\n", prefix, 30, pID3v1->comment); 
	} else {
		printf("%sComment : %.*s\n", prefix, 28, pID3v1->v1_1.comment); 
		printf("%sTrack   : %d\n", prefix, pID3v1->v1_1.track); 
	}
	printf("%sGenre   : %s\n", prefix, getGenre(pID3v1->genre)); 
}

int main(int argc, char* argv[])
{
	if (!((argc == 5 && strcmp(argv[1], "-e") == 0) || (argc == 4 && strcmp(argv[1], "-d") == 0))) {
        printf("To encrypt: %s -e <mp3 file> <key> <content id>\n"
               "To decrypt: %s -d <mp3 file> <key>\n",
               argv[0], argv[0]);
		exit(1);
	}
    const char * str_op = argv[1];
	const char * mp3_file_name = argv[2];
    const char * str_key = argv[3];
    const char * str_cid = ((argc == 5) ? argv[4] : NULL);

    if (strlen(mp3_file_name) < 4 || strcmp(&mp3_file_name[strlen(mp3_file_name) - 4], ".mp3") != 0) {
        printf("File must be \".mp3\".\n");
        exit(-1);
    }

    if (strlen(str_key) != 32) {
        printf("The key must be 16 bytes (32 hexachars) long.\n");
        exit(-1);
    }

    if (str_cid != NULL && strlen(str_cid) != 32) {
        printf("Content id must be 16 bytes (32 hexachars) long.\n");
        exit(-1);
    }

    for (int i = 0; i < 32; ++i) {
        if (!(str_key[i] >= '0' && str_key[i] <= '9' ||
              str_key[i] >= 'a' && str_key[i] <= 'f' ||
              str_key[i] >= 'A' && str_key[i] <= 'F')) {
            printf("The key must be hexadecimal.\n");
            exit(-1);
        }
        if (str_cid != NULL &&
            !(str_key[i] >= '0' && str_key[i] <= '9' ||
              str_key[i] >= 'a' && str_key[i] <= 'f' ||
              str_key[i] >= 'A' && str_key[i] <= 'F')) {
            printf("The key must be hexadecimal.\n");
            exit(-1);
        }
    }

	FILE * mp3_file;

	// Open the file
	mp3_file = fopen(mp3_file_name, "rb");
	if (mp3_file == NULL) {
		printf("MP3 file not found: \"%s\"\n", mp3_file_name);
	} else {
		printf("\n");

		// Get total mp3 file length
		long file_length = filelength(fileno(mp3_file));

		// Check for ID3v1 or ID3v1.1 header
		bool hasID3v1 = false;
		long id3v1_pos = file_length - 128;
		ID3v1 id3v1;
		if (file_length >= 128) {
			fseek(mp3_file, id3v1_pos, SEEK_SET);
			hasID3v1 = readID3v1(mp3_file, &id3v1) && checkID3v1(&id3v1);
		}

		// Check for ID3v2
		bool hasID3v2 = false;
		long id3v2_pos = 0;
		long id3v2_len = 0;
		if (file_length >= 10) {
			unsigned char id3v2_header[10];
			fseek(mp3_file, 0, SEEK_SET);
			if (fread(id3v2_header, 10, 1, mp3_file) == 1 && memcmp(id3v2_header, "ID3", 3) == 0) {
				hasID3v2 = true;
				id3v2_len = 10 + (((unsigned int)(id3v2_header[6])) << 21) +
				                 (((unsigned int)(id3v2_header[7])) << 14) +
				                 (((unsigned int)(id3v2_header[8])) <<  7) +
				                  ((unsigned int)(id3v2_header[9]));
				if (id3v2_header[5] & 0x10) id3v2_len += 10;
			}
		}

		// Check for Lyrics3 v1
		bool hasLyrics3v1 = false;
		long lyrics3v1_pos = 0;
		long lyrics3v1_len = 0;
		if (hasID3v1 && file_length >= 148) {
			char lyricsEnd[9];
			fseek(mp3_file, id3v1_pos - 9, SEEK_SET);
			if (fread(lyricsEnd, 9, 1, mp3_file) == 1 && memcmp(lyricsEnd, "LYRICSEND", 9) == 0) {
				// Found the end of a Lyrics3 field. Now, look backwards for LYRICSBEGIN.
				long maxLyricsLength = 11 + 5100;
				if (maxLyricsLength > file_length - 128 - 9) {
					maxLyricsLength = file_length - 128 - 9;
				}
				char * pLyrics = new char[maxLyricsLength];
				long searchPos = file_length - 128 - 9 - maxLyricsLength;
				fseek(mp3_file, searchPos, SEEK_SET);
				fread(pLyrics, maxLyricsLength, 1, mp3_file);
				char * pSearch = pLyrics;
				char * pPlace;
				do {
					pPlace = (char*)memchr(pSearch, 'L', maxLyricsLength - 11 + 1);
					if (memcmp(pPlace, "LYRICSBEGIN", 11) == 0) {
						hasLyrics3v1 = true;
						lyrics3v1_pos = searchPos + (pPlace - pLyrics);
						lyrics3v1_len = file_length - 128 - 9 - lyrics3v1_pos;
						break;
					} else {
						maxLyricsLength -= (long)(pPlace - pSearch) + 1;
						pSearch = pPlace + 1;
					}
				} while (pPlace != NULL);
				delete [] pLyrics;
			}
		}

		// Check for Lyrics3 v2
		bool hasLyrics3v2 = false;
		long lyrics3v2_pos = 0;
		long lyrics3v2_len = 0;
		if (hasID3v1 && file_length >= 148) {
			char lyricsEnd[9];
			fseek(mp3_file, id3v1_pos - 9, SEEK_SET);
			if (fread(lyricsEnd, 9, 1, mp3_file) == 1 && memcmp(lyricsEnd, "LYRICS200", 9) == 0) {
				// Found the end of a Lyrics3 field. Now, read the length of the lyrics.
				char lyricsLength[6];
				fseek(mp3_file, id3v1_pos - 9 - 6, SEEK_SET);
				if (fread(lyricsLength, 6, 1, mp3_file) == 1) {
					lyrics3v2_len = atol(lyricsLength);
					lyrics3v2_pos = id3v1_pos - 9 - 6 - lyrics3v2_len;
					// Confirm there's a LYRICSBEGIN tag.
					if (lyrics3v2_pos > 0) {
						char lyricsBegin[11];
						fseek(mp3_file, lyrics3v2_pos, SEEK_SET);
						if (fread(lyricsBegin, 11, 1, mp3_file) == 1 && memcmp(lyricsBegin, "LYRICSBEGIN", 11) == 0) {
							hasLyrics3v2 = true;
						}
					}
				}
			}
		}

		// Look for the first frame in the file
		fseek(mp3_file, 0, SEEK_SET);
		if (!synchronize(mp3_file)) {
			printf("Is this a real mp3 file?\n");
		} else {

			char * aes_file_name = new char[strlen(mp3_file_name) + 4 + 1];
			strcpy(aes_file_name, mp3_file_name);
			strcat(aes_file_name, ".aes");

			FILE * aes_file = fopen(aes_file_name, "wb");
			if (aes_file == NULL) {
				printf("Couldn't open the destination file \"%s\".\n", aes_file_name);
			} else {

				char buffer[4096];
				size_t nbToRead;
				size_t nbRead;

				// Save the current position (1st frame)
				long pos_1st_frame = ftell(mp3_file);
				fseek(mp3_file, 0, SEEK_SET);

				// Copy ID3v2
				if (hasID3v2) {
					long nbID3v2ToGo = id3v2_len;
					while (nbID3v2ToGo > 0) {
						nbToRead = ((nbID3v2ToGo >= sizeof(buffer)) ? sizeof(buffer) : nbID3v2ToGo);
						nbRead = fread(buffer, 1, nbToRead, mp3_file);
						fwrite(buffer, 1, nbRead, aes_file);
						nbID3v2ToGo -= (long)nbRead;
					}
				}

				byte iv_buf[16]; // The buffer for AES's IV.

				DoRM_Box dormBox;
				bool hasDoRMBox = false;
				fread(buffer, 1, 4, mp3_file);
				if (memcmp(buffer, "DoRM", 4) == 0) {
					// This is a protected file. Read the Dorm_Box e decrypt the file.
                    if (strcmp(str_op, "-e") == 0) {
                        printf("This file is already protected.\n");
                        fclose(aes_file);
                        unlink(aes_file_name);
                        delete [] aes_file_name;
                        fclose(mp3_file);
                        exit(-1);
                    }
					hasDoRMBox = true;
					fseek(mp3_file, -4, SEEK_CUR);
					fread(&dormBox, sizeof(DoRM_Box), 1, mp3_file);
					// Get the IV from the DoRM_Box.
					memcpy(iv_buf, dormBox.iv, 16);
				} else {
					// This is an unprotected file. Write the DoRM_Box and encrypt the file.
                    if (strcmp(str_op, "-d") == 0) {
                        printf("No DoRM box.\n");
                        fclose(aes_file);
                        unlink(aes_file_name);
                        delete [] aes_file_name;
                        fclose(mp3_file);
                        exit(-1);
                    }
					fseek(mp3_file, -4, SEEK_CUR);
					// Generate an IV (with no FFs)
					srand((unsigned)time(NULL));
					for (int i = 0; i < 16; ++i) {
						do { iv_buf[i] = rand(); } while (iv_buf[i] == 0xff);
					}
					// Write the DoRM box
					DoRM_Box dormBox;
					memcpy(dormBox.dorm, "DoRM", 4);
					memset(dormBox.version, 0, 2);
					memset(dormBox.reserved, 0, 2);
					size_t dormBoxLen = sizeof(DoRM_Box) - 10;
					dormBox.length[3] = (dormBoxLen & 0x0000007f);
					dormBox.length[2] = (dormBoxLen & 0x00003f80) >>  7;
					dormBox.length[1] = (dormBoxLen & 0x001fc000) >> 14;
					dormBox.length[0] = (dormBoxLen & 0x0fe00000) >> 21;
					unsigned dormProtId = 0x010000E0; // 0xE0000001 in big endian
					memcpy(dormBox.prot_id, &dormProtId, 4);
					//memset(dormBox.cid, 0, 16); // cid is 0 for now
                    for (int i = 0; i < 16; ++i) {
                        dormBox.cid[i] = (hex2bin(str_cid[i * 2 + 0]) << 4) |
                                         (hex2bin(str_cid[i * 2 + 1]));
                    }
					memcpy(dormBox.iv, iv_buf, 16);
					fwrite(&dormBox, sizeof(DoRM_Box), 1, aes_file);
				}

				// Instantiate AES
				SecByteBlock key = HexDecodeString(str_key);
				SecByteBlock iv(iv_buf, 16);
				OFB_Mode<AES>::Encryption aes(key, key.size(), iv);

				// Write whatever exists between ID3v2 and the 1st frame
				{
					long nbPreambleToGo = pos_1st_frame - id3v2_len - (hasDoRMBox ? sizeof(DoRM_Box) : 0);
					while (nbPreambleToGo > 0) {
						nbToRead = ((nbPreambleToGo >= sizeof(buffer)) ? sizeof(buffer) : nbPreambleToGo);
						nbRead = fread(buffer, 1, nbToRead, mp3_file);
						fwrite(buffer, 1, nbRead, aes_file);
						nbPreambleToGo -= (long)nbRead;
					}
				}
				assert(ftell(mp3_file) == pos_1st_frame);

				long last_pos = 0;
				size_t buf_size = 4096;
				char * pBuffer = new char[buf_size];
				int frame_length = 0;

				MP3Header mp3_header, mp3_nextHeader;
				for (;;) {
					for (;;) {
						last_pos = ftell(mp3_file);
						if (!readHeader(mp3_file, &mp3_header, true) ||
											!checkHeader(&mp3_header)) {
							fseek(mp3_file, last_pos, SEEK_SET);
							break;
						}
						frame_length = getFrameLength(&mp3_header);
						if (!(frame_length > 0)) break;

						// Check if the next header is correct. If not, don't
						// cipher this block. It may ruin some unsupported field.
						fseek(mp3_file, last_pos + frame_length, SEEK_SET);
						if (!readHeader(mp3_file, &mp3_nextHeader, true) || !checkHeader(&mp3_nextHeader)) break;

						fseek(mp3_file, last_pos, SEEK_SET);

						if (frame_length < 4) {
							break;
						} else {
							nbToRead = frame_length;
							if (nbToRead > buf_size) {
								delete [] pBuffer;
								pBuffer = new char[nbToRead];
							}
							nbRead = fread(pBuffer, 1, nbToRead, mp3_file);
                            if ((((*(int*)(void*)(&mp3_header)) ^ (*(int*)(void*)(&mp3_nextHeader))) & 0xFFFFFDFF) == 0) {
//printf(".");
    							aes.ProcessData(((byte *)pBuffer) + 4, ((const byte *)pBuffer) + 4, nbRead - 4);
                            } else {
//printf("-");
                            }
							fwrite(pBuffer, 1, nbRead, aes_file);
						}
					}

					// Did we get to the end of the file or to a known extension?
					if ((last_pos == file_length) ||
					    (hasID3v1 && (last_pos == id3v1_pos)) ||
					    (hasLyrics3v2 && (last_pos == lyrics3v2_pos)) ||
					    (hasLyrics3v1 && (last_pos == lyrics3v1_pos))) break;

					// Something odd happened. Either the file is broken or
					// it has some unknown extension. As I can't probe for
					// the existence of an unknown extension (that's forbidden
					// in my religion) I'll assume the file is broken and
					// try to resynchronize.
					if (!synchronize(mp3_file)) {
						fseek(mp3_file, last_pos, SEEK_SET);
						break;
					} else {

						// We managed to resynchronize. Let's save the broken
						// part in clear (!) and continue ciphering.
						long sync_pos = ftell(mp3_file);

						fseek(mp3_file, last_pos, SEEK_SET);
						long nbToGo = sync_pos - last_pos;
						while (nbToGo > 0) {
							nbToRead = (((unsigned)nbToGo > buf_size) ? buf_size : (unsigned)nbToGo);
							nbRead = fread(pBuffer, 1, nbToRead, mp3_file);
							fwrite(pBuffer, 1, nbRead, aes_file);
							if (nbRead < nbToRead) {
							printf("Error reading file.\n");
								goto exit;
							}
							nbToGo -= (long)nbRead;
						}
					}
				}
exit:
				long nbToGo = file_length - last_pos;
				nbToRead = buf_size;
				while (nbToGo > 0) {
					nbRead = fread(pBuffer, 1, nbToRead, mp3_file);
					fwrite(pBuffer, 1, nbRead, aes_file);
					if (nbToRead <= (unsigned)nbToGo && nbRead < nbToRead) {
					printf("Error reading file.\n");
						break;
					}
					nbToGo -= (long)nbRead;
				}

				delete [] pBuffer;
				fclose(aes_file);
			}

			if (aes_file_name != NULL) {
				delete [] aes_file_name;
				aes_file_name = NULL;
			}
		}
		fclose(mp3_file);
	}
	return 0;
}
