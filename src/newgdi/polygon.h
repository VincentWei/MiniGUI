/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** polygon.h: internals for polygon generator.
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

