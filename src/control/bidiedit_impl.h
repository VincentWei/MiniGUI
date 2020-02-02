///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** bidiedit_impl.h: the head file of BidiEdit Control module.
**
** Create date: 2008/03/10
*/

#ifndef BIDI_EDIT_IMPL_H_
#define BIDI_EDIT_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct _ACHARBUF
{
    ACHARMAPINFO* achar_map;     /* achar info array */
    Achar32* achars;             /* achar array  */
    int   achars_len;            /* achar array len or buffer used size */
    int   blocksize;             /* block size, a block is an allocate unit */
    int   achars_buffsize;       /* achar string buffer size */
    int   acharmap_buffsize;     /* achar map buffer size */
} ACHARBUF;

typedef struct tagBIDISLEDITDATA
{
    StrBuffer str_content;  // string text buffer
    ACHARBUF achar_content; // achar string buffer

    int     editPos;        // current edit position
    int     selStart;       // selection start position
    int     selEnd;         // selection end position

    DWORD   status;         // status of box

    int     nContX;         // content x offset position
    int     nContW;         // content width
    RECT    rcVis;          // content rect
    int     nVisW;          // visible area width
    int     startx;         // text x display position
    int     starty;         // text y display position

    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin

    int     nBlockSize;     /* string buffer block size */
    char    *tiptext;       // tip text

    int     hardLimit;      // hard limit

    int     passwdChar;     // password character

    int     changed;        // change flag

}BIDISLEDITDATA;

typedef BIDISLEDITDATA* PBIDISLEDITDATA;

static inline void*
acharbuf_alloc (ACHARBUF* achar_buff, int len, int block_size)
{
    achar_buff->achars_buffsize = (len + (block_size - len % block_size))
                                     * sizeof (Achar32);
    achar_buff->achars = malloc (achar_buff->achars_buffsize);

    achar_buff->acharmap_buffsize = (len + (block_size - len % block_size))
                                    * sizeof (ACHARMAPINFO);
    achar_buff->achar_map = malloc (achar_buff->acharmap_buffsize);

    achar_buff->achars_len = len;
    achar_buff->blocksize = block_size;

    return achar_buff->achars;
}

static inline void acharbuf_free (ACHARBUF *achar_buff)
{
    if (achar_buff) {
        free (achar_buff->achars);
        free (achar_buff->achar_map);
    }
}

static inline void* acharbuf_realloc (ACHARBUF *achar_buff, int len)
{
    if (len * sizeof(int) > achar_buff->achars_buffsize ||
        len * sizeof(int) < achar_buff->achars_buffsize - achar_buff->blocksize)    {
        /* realloc achar string mem */
        achar_buff->achars_buffsize = (len + (achar_buff->blocksize -
                            len % achar_buff->blocksize))* sizeof(Achar32);
        achar_buff->achars = realloc (achar_buff->achars,
                                achar_buff->achars_buffsize);

        /* realloc achar map mem */
        achar_buff->acharmap_buffsize = (len + (achar_buff->blocksize -
                        len % achar_buff->blocksize))* sizeof(ACHARMAPINFO);
        achar_buff->achar_map = realloc (achar_buff->achar_map,
                        achar_buff->acharmap_buffsize);
    }

    return achar_buff->achars;
}

BOOL RegisterBIDISLEditControl (void);

#ifdef __cplusplus
}
#endif

#endif /* BIDI_EDIT_IMPL_H_ */

