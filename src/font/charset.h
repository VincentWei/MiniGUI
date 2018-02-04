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
** charset.h: the head file of charset operation set.
*/

#ifndef GUI_FONT_CHARSET_H
    #define GUI_FONT_CHARSET_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

CHARSETOPS* GetCharsetOps (const char* charset);
CHARSETOPS* GetCharsetOpsEx (const char* charset);
BOOL IsCompatibleCharset (const char* charset, CHARSETOPS* ops);

#ifdef _MGCHARSET_GB
extern unsigned short __mg_gbunicode_map [];
#endif

#ifdef _MGCHARSET_GBK
extern unsigned short __mg_gbkunicode_map [];
#endif

#ifdef _MGCHARSET_GB18030
extern unsigned short __mg_gb18030_0_unicode_map [];
#endif

#ifdef _MGCHARSET_BIG5
extern unsigned short __mg_big5_unicode_map [];
#endif

#ifdef _MGCHARSET_EUCKR
extern unsigned short __mg_ksc5601_0_unicode_map [];
#endif

#ifdef _MGCHARSET_EUCJP
extern unsigned short __mg_jisx0208_0_unicode_map [];
#endif

#ifdef _MGCHARSET_SHIFTJIS
extern unsigned short __mg_jisx0208_1_unicode_map [];
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_CHARSET_H

