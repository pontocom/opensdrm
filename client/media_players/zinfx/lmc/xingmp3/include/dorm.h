#ifndef INCLUDED_DORM_H_
#define INCLUDED_DORM_H_

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
};

int checkHeader(const MP3Header * pHeader);

#define ISO_VERSION(version) ((version) & 0x01)
const unsigned int ISO_MPEG_1 = 1;
const unsigned int ISO_MPEG_2 = 0;

const unsigned int MPEG_1_0 = 3;
const unsigned int MPEG_2_0 = 2;
const unsigned int MPEG_2_5 = 0;

const unsigned int LAYER_I   = 3;
const unsigned int LAYER_II  = 2;
const unsigned int LAYER_III = 1;

const unsigned int BR_FREE = 0x00000000;
const unsigned int BR_BAD  = 0xFFFFFFFF;

extern unsigned int BITRATE_MPEG1_LAYERI  [16];
extern unsigned int BITRATE_MPEG1_LAYERII [16];
extern unsigned int BITRATE_MPEG1_LAYERIII[16];

extern unsigned int BITRATE_MPEG2_LAYERI  [16];
extern unsigned int BITRATE_MPEG2_LAYERII [16];
extern unsigned int BITRATE_MPEG2_LAYERIII[16];

extern unsigned int * BITRATE[2][4];

inline unsigned int getBitrate(MP3Header * pHeader)
{
	return BITRATE[ISO_VERSION(pHeader->version)][pHeader->layer][pHeader->bitrate];
}

const unsigned int SF_RESERVED = 0;

extern unsigned int FREQUECY_MPEG1 [4];
extern unsigned int FREQUECY_MPEG2 [4];
extern unsigned int FREQUECY_MPEG25[4];

extern unsigned int * FREQUENCY[4];

inline unsigned int getFrequency(MP3Header * pHeader)
{
	return FREQUENCY[pHeader->version][pHeader->frequency];
}

const unsigned int STEREO         = 0;
const unsigned int JOINT_STEREO   = 1;
const unsigned int DUAL_CHANNEL   = 2;
const unsigned int SINGLE_CHANNEL = 3;

const unsigned int EMPH_NONE      = 0;
const unsigned int EMPH_50_15     = 1;
const unsigned int EMPH_RESERVED  = 2;
const unsigned int EMPH_CCITT_J17 = 3;

int getFrameLength(MP3Header * pHeader);

inline char bin2hex(unsigned char b) {
   return (b > 9) ? ((b - 10) + 'A') : (b + '0');
}

void getDoRMKey(const char * str_cid, char * str_key);

#endif /* _DORM_H */
