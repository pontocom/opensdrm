#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <zlib.h>

#ifndef TK_ZIP_UTILS_H
#define TK_ZIP_UTILS_H

#ifndef FNAME_MAX
#define FNAME_MAX 1024
#endif

#ifndef DIRMODE
#define DIRMODE 0700
#endif

#ifdef __cplusplus
extern "C" {
#endif

void make_dirs(const char *path);
void convert_date(unsigned int d,int *yr, int *mo,int *dy);

#ifdef __cplusplus
}
#endif

#endif
