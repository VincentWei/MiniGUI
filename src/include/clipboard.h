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
** clipboard.h: the internal clipboard APIs.
**
** Create Date: 2003/12/xx
*/

#ifndef MG_CLIPBOARD
#define MG_CLIPBOARD
#ifdef _MGHAVE_CLIPBOARD
#define CLIPBOARD_BUFFLENGTH	1024

#ifdef _cplusplus
extern "C" {
#endif /* _cplusplus */

void mg_InitClipBoard(void);
void mg_TerminateClipBoard(void);

#define CBREQ_CREATE        1
#define CBREQ_DESTROY       2
#define CBREQ_SETDATA       3
#define CBREQ_GETDATALEN    4
#define CBREQ_GETDATA       5
#define CBREQ_GETBYTE       6

typedef struct _REQ_CLIPBOARD
{
    int id;                                 /* request id */
    char name [LEN_CLIPBOARD_NAME + 1];     /* the name of the clipboard involved */
    int cbop;                               /* clipboard operation types */

    //int ret;                                /* return value of the request */
    //void *data;                             /* buffer of attached or returned data */
    size_t len_data;                        /* length of the attached data. */

} REQ_CLIPBOARD;


#ifdef _cplusplus
}
#endif /* _cplusplus */

#endif
#endif
