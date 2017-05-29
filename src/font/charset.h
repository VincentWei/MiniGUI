/*
** $Id: charset.h 11102 2008-10-23 01:58:25Z tangjianbin $
**
** charset.h: the head file of charset operation set.
**
** Copyright (C) 2000 ~ 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
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

