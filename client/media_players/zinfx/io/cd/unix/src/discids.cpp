/*____________________________________________________________________________

        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1999-2000 EMusic.com
        Portions Copyright (C) 1998-1999 Tony Arcieri

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

        $Id: discids.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"
#include "cdaudio.h"

#include <stdio.h>
#include <string.h>

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* CDDB sum function */
static int
cddb_sum(long val)
{
   char *bufptr, buf[16];
   int ret = 0;

   snprintf(buf, 16, "%lu", val);
   for(bufptr = buf; *bufptr != '\0'; bufptr++)
     ret += (*bufptr - '0');

   return ret;
}

/* Produce CDDB ID for the CD in the device referred to by cd_desc */
unsigned long
cddb_direct_discid(struct disc_info disc)
{
   int index, tracksum = 0, discid;

   for(index = 0; index < disc.disc_total_tracks; index++)
     tracksum += cddb_sum(disc.disc_track[index].track_pos.minutes * 60 + disc.disc_track[index].track_pos.seconds);

   discid = (disc.disc_length.minutes * 60 + disc.disc_length.seconds) - (disc.disc_track[0].track_pos.minutes * 60 + disc.disc_track[0].track_pos.seconds);

   return ((tracksum % 0xFF) << 24 | discid << 8 | disc.disc_total_tracks);
}

long
cddb_discid(int cd_desc)
{
   struct disc_info disc;

   if(cd_stat(cd_desc, &disc) < 0)
     return -1;

   if(!disc.disc_present)
     return -1;

   return cddb_direct_discid(disc);
}

#ifndef HAVE_SNPRINTF
/* The system doesn't have a snprintf() function.  Punt, and map it to
   sprintf(). */
int
snprintf(char *dest, int size, const char *format, ...)
{
    va_list ap;
    int result;

    va_start(ap, format);
    result = vsprintf(dest, format, ap);
    va_end(ap);
    return result;
}
#endif  /* HAVE_SNPRINTF */

static unsigned char oddity_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";

static int
cdindex_encode64(unsigned char *outbuffer, unsigned char *inbuffer, int inlen, int outlen)
{
   unsigned long outdex = 0, s_count = inlen, index;

   for (index = 0; s_count; inbuffer += 3) {
      outbuffer[outdex++] = oddity_64[inbuffer[0] >> 2];
      outbuffer[outdex++] = oddity_64[((inbuffer[0] << 4) + (--s_count ? (inbuffer[1] >> 4) : 0)) & 0x3f];
      outbuffer[outdex++] = s_count ? oddity_64[((inbuffer[1] << 2) + (--s_count ? (inbuffer[2] >> 6) : 0)) & 0x3f] : oddity_64[64];
      outbuffer[outdex++] = s_count ? oddity_64[inbuffer[2] & 0x3f] : oddity_64[64];
      if (s_count) s_count--;
      if ((++index) == 15) {
         index = 0;
         outbuffer[outdex++] = '\n';
      }
   }
   outbuffer[outdex++] = '\0';
   return outdex;
}

#define SHA_BLOCKSIZE 64
#define SHA_VERSION 1

#define f1(x,y,z)       ((x & y) | (~x & z))
#define f2(x,y,z)       (x ^ y ^ z)
#define f3(x,y,z)       ((x & y) | (x & z) | (y & z))
#define f4(x,y,z)       (x ^ y ^ z)

#define CONST1          0x5a827999L
#define CONST2          0x6ed9eba1L
#define CONST3          0x8f1bbcdcL
#define CONST4          0xca62c1d6L

#define T32(x)  ((x) & 0xffffffffL)
#define R32(x,n)        T32(((x << n) | (x >> (32 - n))))

#define FA(n)   \
        T = T32(R32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n); B = R32(B,30)

#define FB(n)   \
        E = T32(R32(T,5) + f##n(A,B,C) + D + *WP++ + CONST##n); A = R32(A,30)

#define FC(n)   \
        D = T32(R32(E,5) + f##n(T,A,B) + C + *WP++ + CONST##n); T = R32(T,30)

#define FD(n)   \
        C = T32(R32(D,5) + f##n(E,T,A) + B + *WP++ + CONST##n); E = R32(E,30)

#define FE(n)   \
        B = T32(R32(C,5) + f##n(D,E,T) + A + *WP++ + CONST##n); D = R32(D,30)

#define FT(n)   \
        A = T32(R32(B,5) + f##n(C,D,E) + T + *WP++ + CONST##n); C = R32(C,30)

struct sha_data {
   unsigned long digest[5];
   unsigned long count_lo, count_hi;
   unsigned char data[SHA_BLOCKSIZE];
   int local;
};

static void
sha_init(struct sha_data *sha)
{
   sha->digest[0] = 0x67452301L;
   sha->digest[1] = 0xefcdab89L;
   sha->digest[2] = 0x98badcfeL;
   sha->digest[3] = 0x10325476L;
   sha->digest[4] = 0xc3d2e1f0L;
   sha->count_lo = 0L;
   sha->count_hi = 0L;
   sha->local = 0;
}

static void
sha_transform(struct sha_data *sha)
{
   int i;
   unsigned char *dp;
   unsigned long T, A, B, C, D, E, W[80], *WP;

   dp = sha->data;

#if SIZEOF_LONG == 4
#ifndef WORDS_BIGENDIAN
#define SWAP_DONE
   for (i = 0; i < 16; ++i) {
      T = *((unsigned long *) dp);
      dp += 4;
      W[i] =  ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
              ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
   }
#endif

#ifdef WORDS_BIGENDIAN
#define SWAP_DONE
   for (i = 0; i < 16; ++i) {
      T = *((unsigned long *) dp);
      dp += 4;
      W[i] = T32(T);
   }
#endif
#endif

#if SIZEOF_LONG == 8
#ifndef WORDS_BIGENDIAN
#define SWAP_DONE
   for (i = 0; i < 16; i += 2) {
      T = *((unsigned long *) dp);
      dp += 8;
      W[i] =  ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
              ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
      T >>= 32;
      W[i+1] = ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
               ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
   }
#endif

#ifdef WORDS_BIGENDIAN
#define SWAP_DONE
   for (i = 0; i < 16; i += 2) {
      T = *((unsigned long *) dp);
      dp += 8;
      W[i] = T32(T >> 32);
      W[i+1] = T32(T);
   }
#endif
#endif

    for (i = 16; i < 80; ++i) {
        W[i] = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16];
#if (SHA_VERSION == 1)
        W[i] = R32(W[i], 1);
#endif
    }
    A = sha->digest[0];
    B = sha->digest[1];
    C = sha->digest[2];
    D = sha->digest[3];
    E = sha->digest[4];
    WP = W;

    FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1); FC(1); FD(1);
    FE(1); FT(1); FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1);
    FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2); FE(2); FT(2);
    FA(2); FB(2); FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2);
    FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3); FA(3); FB(3);
    FC(3); FD(3); FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3);
    FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4); FC(4); FD(4);
    FE(4); FT(4); FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4);
    sha->digest[0] = T32(sha->digest[0] + E);
    sha->digest[1] = T32(sha->digest[1] + T);
    sha->digest[2] = T32(sha->digest[2] + A);
    sha->digest[3] = T32(sha->digest[3] + B);
    sha->digest[4] = T32(sha->digest[4] + C);
}

static void
sha_update(struct sha_data *sha, unsigned char *buffer, int count)
{
   int i;
   unsigned long clo;

   clo = T32(sha->count_lo + ((unsigned long) count << 3));
   if(clo < sha->count_lo)
     ++sha->count_hi;
   sha->count_lo = clo;
   sha->count_hi += (unsigned long) count >> 29;
   if(sha->local) {
      i = SHA_BLOCKSIZE - sha->local;
      if(i > count)
        i = count;
      memcpy(((unsigned char *) sha->data) + sha->local, buffer, i);
      count -= i;
      buffer += i;
      sha->local += i;
      if(sha->local == SHA_BLOCKSIZE)
        sha_transform(sha);
      else return;
   }

   while (count >= SHA_BLOCKSIZE) {
      memcpy(sha->data, buffer, SHA_BLOCKSIZE);
      buffer += SHA_BLOCKSIZE;
      count -= SHA_BLOCKSIZE;
      sha_transform(sha);
   }

   memcpy(sha->data, buffer, count);
   sha->local = count;
}

static void
sha_final(unsigned char digest[20], struct sha_data *sha)
{
   int count;
   unsigned long lo_bit_count, hi_bit_count;

   lo_bit_count = sha->count_lo;
   hi_bit_count = sha->count_hi;
   count = (int) ((lo_bit_count >> 3) & 0x3f);
   ((unsigned char *) sha->data)[count++] = 0x80;
   if (count > SHA_BLOCKSIZE - 8) {
      memset(((unsigned char *) sha->data) + count, 0, SHA_BLOCKSIZE - count);
      sha_transform(sha);
      memset((unsigned char *) sha->data, 0, SHA_BLOCKSIZE - 8);
   } else
     memset(((unsigned char *) sha->data) + count, 0, SHA_BLOCKSIZE - 8 - count);

   sha->data[56] = (hi_bit_count >> 24) & 0xff;
   sha->data[57] = (hi_bit_count >> 16) & 0xff;
   sha->data[58] = (hi_bit_count >>  8) & 0xff;
   sha->data[59] = (hi_bit_count >>  0) & 0xff;
   sha->data[60] = (lo_bit_count >> 24) & 0xff;
   sha->data[61] = (lo_bit_count >> 16) & 0xff;
   sha->data[62] = (lo_bit_count >>  8) & 0xff;
   sha->data[63] = (lo_bit_count >>  0) & 0xff;
   sha_transform(sha);
   digest[0] = (unsigned char) ((sha->digest[0] >> 24) & 0xff);
   digest[1] = (unsigned char) ((sha->digest[0] >> 16) & 0xff);
   digest[2] = (unsigned char) ((sha->digest[0] >> 8) & 0xff);
   digest[3] = (unsigned char) ((sha->digest[0]) & 0xff);
   digest[4] = (unsigned char) ((sha->digest[1] >> 24) & 0xff);
   digest[5] = (unsigned char) ((sha->digest[1] >> 16) & 0xff);
   digest[6] = (unsigned char) ((sha->digest[1] >> 8) & 0xff);
   digest[7] = (unsigned char) ((sha->digest[1]) & 0xff);
   digest[8] = (unsigned char) ((sha->digest[2] >> 24) & 0xff);
   digest[9] = (unsigned char) ((sha->digest[2] >> 16) & 0xff);
   digest[10] = (unsigned char) ((sha->digest[2] >> 8) & 0xff);
   digest[11] = (unsigned char) ((sha->digest[2]) & 0xff);
   digest[12] = (unsigned char) ((sha->digest[3] >> 24) & 0xff);
   digest[13] = (unsigned char) ((sha->digest[3] >> 16) & 0xff);
   digest[14] = (unsigned char) ((sha->digest[3] >> 8) & 0xff);
   digest[15] = (unsigned char) ((sha->digest[3]) & 0xff);
   digest[16] = (unsigned char) ((sha->digest[4] >> 24) & 0xff);
   digest[17] = (unsigned char) ((sha->digest[4] >> 16) & 0xff);
   digest[18] = (unsigned char) ((sha->digest[4] >> 8) & 0xff);
   digest[19] = (unsigned char) ((sha->digest[4]) & 0xff);
}

int
cdindex_direct_discid(struct disc_info disc, char *discid, int len)
{
   int index;
   struct sha_data sha;
   unsigned char digest[20], temp[9];

   memset(sha.data, '\0', 64);
   sha_init(&sha);

   snprintf((char *)temp, 9, "%02X", disc.disc_first_track);
   sha_update(&sha, temp, strlen((char *)temp));
   snprintf((char *)temp, 9, "%02X", disc.disc_total_tracks);
   sha_update(&sha, temp, strlen((char *)temp));
   snprintf((char *)temp, 9, "%08X", disc.disc_track[disc.disc_total_tracks].track_lba + 150);
   sha_update(&sha, temp, strlen((char *)temp));
   for(index = 0; index < 99; index++) {
      if(index < disc.disc_total_tracks)
        snprintf((char *)temp, 9, "%08X", disc.disc_track[index].track_lba + 150);
      else
        snprintf((char *)temp, 9, "%08X", 0);
      sha_update(&sha, temp, strlen((char *)temp));
   }
   sha_final(digest, &sha);
   return cdindex_encode64((unsigned char *)discid, digest, 20, len);
}

int
cdindex_discid(int cd_desc, char *discid, int len)
{
   struct disc_info disc;

   if(cd_stat(cd_desc, &disc) < 0)
     return -1;

   if(!disc.disc_present)
     return -1;

   if(cdindex_direct_discid(disc, discid, len) < 0)
     return -1;

   return 0;
}

