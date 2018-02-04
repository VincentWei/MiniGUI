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

static HCURSOR load_cursor_from_file (const char* filename)
{
    FILE* fp;
    WORD16 wTemp;
    int  w, h, xhot, yhot, colornum;
#if 0
    DWORD32 size;
#endif
    DWORD32 offset, imagesize, imagew, imageh;
    BYTE* image;
    HCURSOR csr = 0;
    
    if (!(fp = fopen(filename, "rb")))
        return 0;

    fseek(fp, sizeof(WORD16), SEEK_SET);

    /* the cbType of struct CURSORDIR. */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 2) {
        _MG_PRINTF ("LoadCursorFromFile: bad file type: %d\n", wTemp);
        goto error;
    }

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    fseek(fp, sizeof(WORD16), SEEK_CUR);
    
    /* cursor info, read the members of struct CURSORDIRENTRY. */
    w = fgetc (fp);  /* the width of first cursor. */
    h = fgetc (fp);  /* the height of first cursor. */
    if (w != CURSORWIDTH || h != CURSORHEIGHT) {
        _MG_PRINTF ("LoadCursorFromFile: bad first cursor width (%d) and height (%d)\n", w, h);
        goto error;
    }

    fseek(fp, sizeof(BYTE)*2, SEEK_CUR); /* skip bColorCount and bReserved. */
    wTemp = MGUI_ReadLE16FP (fp);
    xhot = wTemp;
    wTemp = MGUI_ReadLE16FP (fp);
    yhot = wTemp;
#if 0
    size = MGUI_ReadLE32FP (fp);
#else
    fseek (fp, sizeof(DWORD32), SEEK_CUR); /* skip size. */
#endif
    offset = MGUI_ReadLE32FP (fp);

    /* read the cursor image info. */
    fseek(fp, offset, SEEK_SET);
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biSize member. */
    imagew = MGUI_ReadLE32FP (fp);
    imageh = MGUI_ReadLE32FP (fp);
    if (imagew != CURSORWIDTH || imageh != (CURSORHEIGHT*2)) {
        _MG_PRINTF ("LoadCursorFromFile: bad cursor image width (%d) and height (%d)\n", imagew, imageh);
        goto error;
    }

    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if (wTemp != 1) {
        _MG_PRINTF ("LoadCursorFromFile: bad planes (%d)\n", wTemp);
        goto error;
    }

    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if (wTemp > 4) {
        _MG_PRINTF ("LoadCursorFromFile: bad bit count (%d)\n", wTemp);
        goto error;
    }

    colornum = (int)wTemp;
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biCompression members. */
    imagesize = MGUI_ReadLE32FP (fp);

    /* skip the rest members and the color table. */
    fseek(fp, sizeof(DWORD32)*4 + sizeof(BYTE)*(4<<colornum), SEEK_CUR);
    
    /* allocate memory for image. */
    if ((image = (BYTE*)ALLOCATE_LOCAL (imagesize)) == NULL) {
        _MG_PRINTF ("LoadCursorFromFile: error when allocating memory for image (%d)\n", imagesize);
        goto error;
    }

    /* read image */
    if (fread (image, 1, imagesize, fp) < imagesize) {
        _MG_PRINTF ("LoadCursorFromFile: error when reading data from file\n");
        goto error;
    }
    
    csr = CreateCursor (xhot, yhot, w, h, 
                        image + (imagesize - MONOSIZE), image, colornum);

    DEALLOCATE_LOCAL (image);
    fclose (fp);
    return csr;

error:
    _MG_PRINTF ("LoadCursorFromFile: failed when loading cursor from %s\n", filename);
    fclose (fp);
    return csr;
}

static HCURSOR load_cursor_from_mem (const void* area)
{
    const Uint8* p = (Uint8*)area;
    WORD16 wTemp;

    int  w, h, xhot, yhot, colornum;
#if 0
    DWORD32 size;
#endif
    DWORD32 offset, imagesize, imagew, imageh;
    
    p += sizeof (WORD16);
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp != 2) goto error;

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    p += sizeof (WORD16);
    
    /* cursor info, read the members of struct CURSORDIRENTRY. */
    w = *p++;  /* the width of first cursor. */
    h = *p++;  /* the height of first cursor. */
    if (w != CURSORWIDTH || h != CURSORHEIGHT)
        goto error;

    /* skip the bColorCount and bReserved. */
    p += sizeof(BYTE)*2;
    xhot = MGUI_ReadLE16Mem (&p);
    yhot = MGUI_ReadLE16Mem (&p);
#if 0
    size = MGUI_ReadLE32Mem (&p);
#else
    p += sizeof(DWORD32); /* skip size. */
#endif
    offset = MGUI_ReadLE32Mem (&p);

    /* read the cursor image info. */
    p = (Uint8*)area + offset;

    /* skip the biSize member. */
    p += sizeof (DWORD32);    
    imagew = MGUI_ReadLE32Mem (&p);
    imageh = MGUI_ReadLE32Mem (&p);
    if (imagew > 32 || imageh > 32) {
        goto error;
    }

    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp != 1) goto error;

    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp > 4) goto error;
    colornum = wTemp;

    /* skip the biCompression members. */
    p += sizeof (DWORD32);    
    imagesize = MGUI_ReadLE32Mem (&p);

    /* skip the rest members and the color table. */
    p += sizeof(DWORD32)*4 + sizeof(BYTE)*(4<<colornum);
    
    return CreateCursor (xhot, yhot, w, h, 
                        p + (imagesize - MONOSIZE), p, colornum);

error:
    _MG_PRINTF ("LoadCursorFromMem: failed when loading cursor from %p\n", area);
    return 0;
}

