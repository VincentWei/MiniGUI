/*
** $Id: fontname.h 9892 2008-03-20 02:50:02Z xwyan $
**
** charset.h: the head file of charset operation set.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
*/

#ifndef GUI_FONT_NAME_H
    #define GUI_FONT_NAME_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define NR_LOOP_FOR_STYLE   2
#define NR_LOOP_FOR_WIDTH   3
#define NR_LOOP_FOR_HEIGHT  4
#define NR_LOOP_FOR_CHARSET 5

int fontGetFontTypeFromName (const char* name);
int fontGetTypeNameFromName (const char* name, char* type);
int fontConvertFontType (const char* type);

BOOL fontGetFamilyFromName (const char* name, char* family);

BOOL fontCopyStyleFromName (const char* name, char* style);
DWORD fontGetStyleFromName (const char* name);
DWORD fontConvertStyle (const char* style_part);

int fontGetWidthFromName (const char* name);
int fontGetHeightFromName (const char* name);
BOOL fontGetCharsetFromName (const char* name, char* charset);
BOOL fontGetCompatibleCharsetFromName (const char* name, char* charset);

BOOL fontGetCharsetPartFromName (const char* name, char* charset);
int charsetGetCharsetsNumber (const char* charsets);
BOOL charsetGetSpecificCharset (const char* charsets, int index, char* charset);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_NAME_H

