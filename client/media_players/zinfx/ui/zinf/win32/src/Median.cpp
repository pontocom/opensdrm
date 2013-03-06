/*
** File:        median.c             Copyright (c) Truda Software
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

#define  FAST_REMAP

#include <stdio.h>
#include <stddef.h>              /* for NULL                   */
#include <stdlib.h>              /* for "qsort"                */
#include <float.h>               /* for FLT_MAX, FLT_MIN       */

#define MAXCOLORS 256            /* maximum # of output colors */
#define HSIZE     32768          /* size of image histogram    */
typedef unsigned  char byte;     /* range 0-255                */
typedef unsigned  short word;    /* range 0-65,535             */
typedef unsigned  long dword;    /* range 0-4,294,967,295      */

/* Macros for converting between (r,g,b)-colors and 15-bit     */
/* colors follow.                                              */
#define RGB(r,g,b) (word)(((b)&~7)<<7)|(((g)&~7)<<2)|((r)>>3)
#define RED(x)     (byte)(((x)&31)<<3)
#define GREEN(x)   (byte)((((x)>>5)&255)<< 3)
#define BLUE(x)    (byte)((((x)>>10)&255)<< 3)

typedef  struct {       /* structure for a cube in color space */
   word  lower;         /* one corner's index in histogram     */
   word  upper;         /* another corner's index in histogram */
   dword count;         /* cube's histogram count              */
   int   level;         /* cube's level                        */
   byte  rmin,rmax;   
   byte  gmin,gmax;   
   byte  bmin,bmax;   
} cube_t;

static cube_t list[MAXCOLORS];   /* list of cubes              */
static int longdim;              /* longest dimension of cube  */
static word HistPtr[HSIZE];      /* points to colors in "Hist" */

void Shrink(cube_t * Cube);
void InvMap(word * Hist, byte ColMap[][3],word ncubes);
int  compare(const void * a1, const void * a2);

word MedianCut(word Hist[], byte ColMap[][3], int maxcubes)
{
   /*
   ** Accepts "Hist", a 32,768-element array that contains 15-bit
   ** color counts of the input image. Uses Heckbert's median-cut
   ** algorithm to divide the color space into "maxcubes" cubes,
   ** and returns the centroid (average value) of each cube in
   ** "ColMap". "Hist" is also updated so that it functions as an
   ** inverse color map. MedianCut returns the actual number of
   ** cubes, which may be less that "maxcubes".
   */

   byte        lr,lg,lb;
   word        i,median,color;
   dword       count;
   int         k,level,ncubes,splitpos;
   void        *base;
   size_t      num,width;
   cube_t      Cube,CubeA,CubeB;

   /*
   ** Create the initial cube, which is the whole RGB-cube.
   */

   ncubes = 0;
   Cube.count = 0;
   for (i=0,color=0;i<=HSIZE-1;i++){
      if (Hist[i] != 0){
         HistPtr[color++] = i;
         Cube.count = Cube.count + Hist[i];
      }
   }
   Cube.lower = 0; Cube.upper = color-1;
   Cube.level = 0;
   Shrink(&Cube);
   list[ncubes++] = Cube;

   /*
   ** The main loop follows.  Search the list of cubes for the
   ** next cube to split, which is the lowest level cube.  A
   ** special case is when a cube has only one color, so that it
   ** cannot be split.
   */

   while (ncubes < maxcubes){

      level = 255; splitpos = -1;
      for (k=0;k<=ncubes-1;k++){
         if (list[k].lower == list[k].upper)  
                  ;                            /* single color */
         else if (list[k].level < level){
            level = list[k].level;
            splitpos = k;
         }
      }
      if (splitpos == -1)            /* no more cubes to split */
         break;

      /*
      ** Must split the cube "splitpos" in the list of cubes.
      ** Next find the longest dimension of this cube, and update
      ** the external variable "longdim", which is used by the 
      ** sort routine so that it knows along which axis to sort.
      */

      Cube = list[splitpos];
      lr = Cube.rmax - Cube.rmin;
      lg = Cube.gmax - Cube.gmin;
      lb = Cube.bmax - Cube.bmin;
      if (lr >= lg && lr >= lb) longdim = 0;
      if (lg >= lr && lg >= lb) longdim = 1;
      if (lb >= lr && lb >= lg) longdim = 2;

      /*
      ** Sort along "longdim". This prepares for the next step,
      ** namely, finding the median. Use standard lib's "qsort".
      */

      base = (void *)&HistPtr[Cube.lower];
      num  = (size_t)(Cube.upper - Cube.lower + 1);
      width = (size_t)sizeof(HistPtr[0]);
      qsort(base,num,width,compare);

      /*
      ** Find median by scanning through the cube, and computing
      ** a running sum. When the running sum equals half the
      ** total for the cube, the median has been found.
      */
      
      count = 0;
      for (i=Cube.lower;i<=Cube.upper-1;i++){
         if (count >= Cube.count/2) break;
         color = HistPtr[i];
         count = count + Hist[color];
      }
      median = i;

      /*
      ** Now split "Cube" at the median. Then add the two new
      ** cubes to the list of cubes.
      */

      CubeA = Cube; CubeA.upper = median-1;
      CubeA.count = count;
      CubeA.level = Cube.level + 1;
      Shrink(&CubeA);
      list[splitpos] = CubeA;               /* add in old slot */

      CubeB = Cube; CubeB.lower = median; 
      CubeB.count = Cube.count - count;
      CubeB.level = Cube.level + 1;
      Shrink(&CubeB);
      list[ncubes++] = CubeB;               /* add in new slot */
      if ((ncubes % 10) == 0)
         fprintf(stderr,".");               /* pacifier        */
   }

   /*
   ** We have enough cubes, or we have split all we can. Now
   ** compute the color map, the inverse color map, and return
   ** the number of colors in the color map.
   */

   InvMap(Hist, ColMap,ncubes);
   return((word)ncubes);
}


void Shrink(cube_t * Cube)
{
   /*
   ** Encloses "Cube" with a tight-fitting cube by updating the
   ** (rmin,gmin,bmin) and (rmax,gmax,bmax) members of "Cube".
   */

   byte        r,g,b;
   word        i,color;

   Cube->rmin = 255; Cube->rmax = 0;
   Cube->gmin = 255; Cube->gmax = 0;
   Cube->bmin = 255; Cube->bmax = 0;
   for (i=Cube->lower;i<=Cube->upper;i++){
      color = HistPtr[i];
      r = RED(color);
      if (r > Cube->rmax) Cube->rmax = r;
      if (r < Cube->rmin) Cube->rmin = r;
      g = GREEN(color);
      if (g > Cube->gmax) Cube->gmax = g;
      if (g < Cube->gmin) Cube->gmin = g;
      b = BLUE(color);
      if (b > Cube->bmax) Cube->bmax = b;
      if (b < Cube->bmin) Cube->bmin = b;
   }
}


void InvMap(word * Hist, byte ColMap[][3],word ncubes)
{
   /*
   ** For each cube in the list of cubes, computes the centroid
   ** (average value) of the colors enclosed by that cube, and
   ** then loads the centroids in the color map. Next loads the
   ** histogram with indices into the color map. A preprocessor
   ** directive "#define FAST_REMAP" controls whether the cube
   ** centroids become the output color for all the colors in a
   ** cube, or whether a "best remap" method is followed.
   */

   byte        r,g,b;
   word        i,k,color;
   float       rsum,gsum,bsum;
   cube_t      Cube;

   for (k=0;k<=ncubes-1;k++){
      Cube = list[k];
      rsum = gsum = bsum = (float)0.0;
      for (i=Cube.lower;i<=Cube.upper;i++){
         color = HistPtr[i];
         r = RED(color);  
         rsum += (float)r*(float)Hist[color];
         g = GREEN(color);
         gsum += (float)g*(float)Hist[color];
         b = BLUE(color);
         bsum += (float)b*(float)Hist[color];
      }

      /* Update the color map */  
 
      ColMap[k][0] = (byte)(rsum/(float)Cube.count);
      ColMap[k][1] = (byte)(gsum/(float)Cube.count);
      ColMap[k][2] = (byte)(bsum/(float)Cube.count);
   }

#ifdef FAST_REMAP

   /*
   ** Fast remap: for each color in each cube, load the corre- 
   ** sponding slot in "Hist" with the centroid of the cube.
   */

   for (k=0;k<=ncubes-1;k++){
      Cube = list[k];
      for (i=Cube.lower;i<=Cube.upper;i++){
         color = HistPtr[i];
         Hist[color] = k;
      }
      if ((k % 10) == 0) fprintf(stderr,".");   /* pacifier    */
   }

#else

   /*
   ** Best remap: for each color in each cube, find the entry in
   ** "ColMap" that has the smallest Euclidian distance from the
   ** color. Record this information in "Hist".
   */

   for (k=0;k<=ncubes-1;k++){
      Cube = list[k];
      for (i=Cube.lower;i<=Cube.upper;i++){
         color = HistPtr[i];
         r = RED(color);  g = GREEN(color); b = BLUE(color);

         /* Search for closest entry in "ColMap" */

         dmin = (float)FLT_MAX;
         for (j=0;j<=ncubes-1;j++){
            dr = (float)ColMap[j][0] - (float)r;
            dg = (float)ColMap[j][1] - (float)g;
            db = (float)ColMap[j][2] - (float)b;
            d = dr*dr + dg*dg + db*db;
            if (d == (float)0.0){
               index = j; break;
            }
            else if (d < dmin){
               dmin = d; index = j;
            }
         }
         Hist[color] = index;
      }
      if ((k % 10) == 0) fprintf(stderr,".");   /* pacifier    */
   }

#endif

   return;
}

int compare(const void * a1, const void * a2)
{
   /*
   ** Called by the sort routine in "MedianCut". Compares two
   ** colors based on the external variable "longdim".
   */

   word        color1,color2;
   byte        c1,c2;

   color1 = (word)*(word *)a1;
   color2 = (word)*(word *)a2;
   switch (longdim){
      case 0:  
         c1 = RED(color1),  c2 = RED(color2);
         break;
      case 1:
         c1 = GREEN(color1), c2 = GREEN(color2);
         break;
      case 2:
         c1 = BLUE(color1), c2 = BLUE(color2);
         break;
   }
   return ((int)(c1-c2));
}
