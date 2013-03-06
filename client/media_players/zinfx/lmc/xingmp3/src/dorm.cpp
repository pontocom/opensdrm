#include "dorm.h"

#include <stdio.h>
#include <string.h>

unsigned int BITRATE_MPEG1_LAYERI  [16] = { BR_FREE, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, BR_BAD };
unsigned int BITRATE_MPEG1_LAYERII [16] = { BR_FREE, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, BR_BAD }; 
unsigned int BITRATE_MPEG1_LAYERIII[16] = { BR_FREE, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, BR_BAD };

unsigned int BITRATE_MPEG2_LAYERI  [16] = { BR_FREE, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, BR_BAD };
unsigned int BITRATE_MPEG2_LAYERII [16] = { BR_FREE,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, BR_BAD };
unsigned int BITRATE_MPEG2_LAYERIII[16] = { BR_FREE,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, BR_BAD };

unsigned int * BITRATE[2][4] =
{
	{ 0, &BITRATE_MPEG2_LAYERIII[0], &BITRATE_MPEG2_LAYERII[0], &BITRATE_MPEG2_LAYERI[0] },
	{ 0, &BITRATE_MPEG1_LAYERIII[0], &BITRATE_MPEG1_LAYERII[0], &BITRATE_MPEG1_LAYERI[0] }
};

unsigned int FREQUECY_MPEG1 [4] = { 44100, 48000, 32000, SF_RESERVED };
unsigned int FREQUECY_MPEG2 [4] = { 22050, 24000, 16000, SF_RESERVED };
unsigned int FREQUECY_MPEG25[4] = { 11025, 12000,  8000, SF_RESERVED };

unsigned int * FREQUENCY[4] = { &FREQUECY_MPEG25[0], 0, &FREQUECY_MPEG2[0], &FREQUECY_MPEG1[0]  };

int checkHeader(const MP3Header * pHeader)
{
	if (pHeader->eleven_ones != 0x07FF) return -1; // unexpected
	if (pHeader->version     ==  0x01 ) return -2; // unexpected
	if (pHeader->layer       ==  0x00 ) return -3; // unexpected
	if (pHeader->bitrate     ==  0x00 ) return -4; // unsupported
	if (pHeader->bitrate     ==  0x0F ) return -5; // invalid
	if (pHeader->frequency   ==  0x03 ) return -6; // unexpected
	if (pHeader->emphasis    ==  0x02 ) return -7; // unexpected
	return 0;
}

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

void getDoRMKey(const char * str_cid, char * str_key) {
    char key_line[256];
    FILE * fkeys = fopen("fkeys.txt", "r");
    if (fkeys != NULL) {
        char * ptr_cid;
        char * ptr_key;
        while (true) {
            memset(key_line, 0, 256);
            if (fgets(key_line, 256, fkeys) == NULL) break;
            key_line[255] = '\0';
            ptr_cid = strtok(key_line, " ,;\t:=");
            if (ptr_cid != NULL) {
               while (ptr_cid[0] == ' ' || ptr_cid[0] == ',' || ptr_cid[0] == ';' || ptr_cid[0] == '\t' || ptr_cid[0] == ':' || ptr_cid[0] == '=') ptr_cid++;
               unsigned int lpos = strlen(ptr_cid) - 1;
               while (ptr_cid[lpos] == ' ' || ptr_cid[lpos] == ',' || ptr_cid[lpos] == ';' || ptr_cid[lpos] == '\t' || ptr_cid[lpos] == ':' || ptr_cid[lpos] == '=' || ptr_cid[lpos] == '\r' || ptr_cid[lpos] == '\n') lpos--;
            }
            ptr_key = strtok(NULL, " ,;\t:=");
            if (ptr_key != NULL) {
               while (ptr_key[0] == ' ' || ptr_key[0] == ',' || ptr_key[0] == ';' || ptr_key[0] == '\t' || ptr_key[0] == ':' || ptr_key[0] == '=') ptr_key++;
               unsigned int lpos = strlen(ptr_key) - 1;
               while (ptr_key[lpos] == ' ' || ptr_key[lpos] == ',' || ptr_key[lpos] == ';' || ptr_key[lpos] == '\t' || ptr_key[lpos] == ':' || ptr_key[lpos] == '=' || ptr_key[lpos] == '\r' || ptr_key[lpos] == '\n') ptr_key[lpos--] = '\0';
            }
            if (ptr_cid != NULL && strcmp(ptr_cid, str_cid) == 0 &&
                ptr_key != NULL && strlen(ptr_key) == 32) {
                strcpy(str_key, ptr_key);
                fclose(fkeys);
                return;
            }
        }
    }
    strcpy(str_key, "00000000000000000000000000000000");
    fclose(fkeys);
}
