/*
** File:        median.h             Copyright (c) Truda Software
** Author:      Anton Kruger         215 Marengo Rd, #2, 
** Date:        March 1992           Oxford, IA 52322-9383
** Revision:    1.0                  March 1992
** 
** Description: Contains an implementation of Heckbert's median-
**              cut color quantization algorithm.
**
** Compilers:  MSC 5.1, 6.0.
**
** Note:       1) Compile in large memory model.
**             2) Delete the "#define FAST_REMAP" statement below
**                in order to deactivate fast remapping.
*/
#ifndef __INCLUDED_MEDIAN_H
#define __INCLUDED_MEDIAN_H

/* Macros for converting between (r,g,b)-colors and 15-bit     */
/* colors follow.                                              */
#define mcRGB(r,g,b) (WORD)(((b)&~7)<<7)|(((g)&~7)<<2)|((r)>>3)
#define mcRED(x)     (BYTE)(((x)&31)<<3)
#define mcGREEN(x)   (BYTE)((((x)>>5)&255)<< 3)
#define mcBLUE(x)    (BYTE)((((x)>>10)&255)<< 3)

WORD MedianCut(WORD Hist[], BYTE ColMap[][3], int maxcubes);

#endif