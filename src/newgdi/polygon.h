/*
** $Id: polygon.h 9850 2008-03-18 04:26:12Z tangjianbin $
**
** polygon.h: internals for polygon generator.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef GUI_GDI_POLYGON_H
    #define GUI_GDI_POLYGON_H

/* number of fractional bits used by the polygon rasteriser */
#define POLYGON_FIX_SHIFT     16

#define POLYGON_64_SUPPORT

#ifdef POLYGON_64_SUPPORT
typedef signed int POLYGON_Int32;
typedef unsigned long POLYGON_Word32;

typedef struct  POLYGON_Int64_
{
    POLYGON_Word32 lo;
    POLYGON_Word32 hi;
}POLYGON_Int64;
#endif

/* an active polygon edge */
typedef struct POLYGON_EDGE 
{
   int top;                         /* top y position */
   int bottom;                      /* bottom y position */
#ifndef POLYGON_64_SUPPORT
   fixed x, dx;                     /* fixed point x position and gradient */
#else
   POLYGON_Int64 x;
   fixed dx;
#endif
   fixed w;                         /* width of line segment */

   struct POLYGON_EDGE *prev;       /* doubly linked list */
   struct POLYGON_EDGE *next;
   struct POLYGON_INFO *poly;	    /* father polygon */
} POLYGON_EDGE;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* polygon helper functions */
void _fill_edge_structure (POLYGON_EDGE *edge, const int *i1, const int *i2);
POLYGON_EDGE* _add_edge (POLYGON_EDGE *list, POLYGON_EDGE *edge, int sort_by_x);
POLYGON_EDGE* _remove_edge (POLYGON_EDGE *list, POLYGON_EDGE *edge);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_POLYGON_H

