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
** resource.c: This file include some functions for system resource loading.
**           some functions are from misc.c.
**
** Create date: 2003/09/06
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cursor.h"
#include "icon.h"
#include "sysres.h"
#include "misc.h"
#include "resmgr.h"
#include "sysres.h"

BOOL GUIAPI RegisterResFromFile (HDC hdc, const char* file)
{
    if (!file) {
        _WRN_PRINTF ("file is NULL\n");
        return FALSE;
    }

    return LoadResource(file, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN)!=NULL;
}

BOOL GUIAPI RegisterResFromMem (HDC hdc, const char* file,
        const unsigned char* data, size_t data_size)
{
    if (!data || !file) {
        _WRN_PRINTF ("file or data is NULL\n");
        return FALSE;
    }

    {
        INNER_RES inner_res = {
            Str2Key(file),
            (void*)data,
            data_size,
            (void*)((char*)strrchr(file,'.')+1)
        };

        if (AddInnerRes(&inner_res, 1, TRUE) == 0) {
            return LoadResource(file, RES_TYPE_IMAGE, (DWORD)hdc) != NULL;
        }
    }

    return FALSE;
}

BOOL GUIAPI RegisterResFromBitmap (const char* file, const BITMAP* bmp)
{
    if (!bmp || !file) {
        _WRN_PRINTF ("file or bitmap is NULL\n");
        return FALSE;
    }

    {
        INNER_RES inner_res = {
            Str2Key(file),
            (void*)bmp,
            sizeof(BITMAP),
            NULL
        };

        if (AddInnerRes(&inner_res, 1, TRUE) == 0){
            return LoadResource(file, RES_TYPE_IMAGE, 0) != NULL;
        }
    }

    return FALSE;
}

const BITMAP* GUIAPI RetrieveRes (const char *file)
{
    return (BITMAP*)GetResource (Str2Key(file));
}

void GUIAPI UnregisterRes (const char* file)
{
    ReleaseRes (Str2Key(file));
}

/****************************** System resource support *********************/
HICON GUIAPI LoadSystemIconEx (HDC hdc, const char* rdr_name,
        const char* szItemName, int which)
{
    char szValue[MAX_NAME + 1];
    char *iconname;
    HICON hIcon;
    MEM_RES* dwMem;
    const WINDOW_ELEMENT_RENDERER* rdr;
    const char* ext;
    size_t size;

    rdr = GetWindowRendererFromName (rdr_name);

    if (!rdr)
        return 0;

    strcpy (szValue, "icon/");
    iconname = szValue + strlen(szValue);

    if (GetMgEtcValue (rdr->name, szItemName,
            iconname, sizeof(szValue)-(iconname-szValue)) < 0 ) {
        _WRN_PRINTF ("can't get %s's value from section %s in etc.\n",
                szItemName, rdr->name);
        return 0;
    }

    ext = __mg_get_extension (szValue);
    //load resource didnot support which param, so, we must
    //get the mem of this icon, and then load it
    dwMem = LoadResource (szValue, RES_TYPE_MEM_RES, (DWORD)&size);
    if (dwMem) {
        if (ext && strcasecmp (ext, "ico")) {
            /* XXX: for bitmap icon, two same copies for large and small */
            hIcon = LoadBitmapIconFromMem (hdc, dwMem->data, size, ext);
        }
        else {
            hIcon = LoadIconFromMem (hdc, dwMem->data, which);
        }
        ReleaseRes (Str2Key (szValue));
    }
    else {
        //load from file
        char szfile[MAX_PATH+1];
        sprintf(szfile, "%s/%s",__sysres_get_system_res_path(), szValue);
        if (ext && strcasecmp (ext, "ico")) {
            /* XXX: for bitmap icon, two same copies for large and small */
            hIcon = LoadBitmapIconFromFile (hdc, szfile);
        }
        else {
            hIcon = LoadIconFromFile (hdc, szfile, which);
        }
    }

    return hIcon;
}

BOOL InitRendererSystemIcon (const char* rdr_name,
            HICON *small_icon, HICON *large_icon)
{
    int i;
    int nIconNr;
    char szValue [12];
    /*
     * Load system icons here.
     */
    if (GetMgEtcValue (rdr_name, "iconnumber",
                            szValue, 10) < 0) {
        _WRN_PRINTF ("can't get icon number for LFRDR %s.\n", rdr_name);
        return FALSE;
    }

    nIconNr = atoi(szValue);

    if (nIconNr <= 0) return FALSE;

    nIconNr = nIconNr < SYSICO_ITEM_NUMBER ? nIconNr : SYSICO_ITEM_NUMBER;

    for (i = 0; i < nIconNr; i++) {
        sprintf(szValue, "icon%d", i);

        small_icon [i] = LoadSystemIconEx (HDC_SCREEN, rdr_name, szValue, 1);
        large_icon [i] = LoadSystemIconEx (HDC_SCREEN, rdr_name, szValue, 0);

        if (small_icon [i] == 0 || large_icon [i] == 0)
            return FALSE;
    }

    return TRUE;
}


void TermRendererSystemIcon (HICON *small_icon,
        HICON *large_icon)
{
    int i;
    for (i = 0; i < SYSICO_ITEM_NUMBER; i++) {
        if (small_icon [i])
            DestroyIcon (small_icon [i]);

        if (large_icon [i])
            DestroyIcon (large_icon [i]);
    }
}


HICON GUIAPI GetLargeSystemIconEx (HWND hWnd, int iItem)
{
    return (HICON)GetWindowElementAttr (hWnd, WE_ATTR_TYPE_LARGEICON | iItem);
}

HICON GUIAPI GetSmallSystemIconEx (HWND hWnd, int iItem)
{
    return (HICON)GetWindowElementAttr (hWnd, WE_ATTR_TYPE_SMALLICON | iItem);
}

HICON GUIAPI LoadSystemIcon (const char* szItemName, int which)
{
    const WINDOW_ELEMENT_RENDERER * rdr =
        GetDefaultWindowElementRenderer();

    return LoadSystemIconEx (HDC_SCREEN, rdr->name, szItemName, which);
}

/* Since 5.0.0 */
const BITMAP* GUIAPI
GetSystemBitmapEx2 (HDC hdc, const char* rdr_name, const char* id)
{
    char file[MAX_NAME + 1] = "bmp/";
    char *filename = file + strlen (file);
    char key [MAX_NAME + 64];
    int nr_written;
    Uint32 Rmask, Gmask, Bmask, Amask;
    map_entry_t* entry;

    if (GetMgEtcValue (rdr_name, id,
                filename, sizeof(file)-(filename-file)) < 0 ) {
        _WRN_PRINTF ("can't get bitmap file name for LFRDR %s: %s!\n",
                    rdr_name, id);
        return NULL;
    }

    /* XXX: we use the RGBA masks for different pixel formats.
       This does not work for surface with pallete. */
    Rmask = GetGDCapability (hdc, GDCAP_RMASK);
    Gmask = GetGDCapability (hdc, GDCAP_GMASK);
    Bmask = GetGDCapability (hdc, GDCAP_BMASK);
    Amask = GetGDCapability (hdc, GDCAP_AMASK);
    nr_written = snprintf (key, sizeof (key), "%s-%s-%x%x%x%x", file, id,
            Rmask, Gmask, Bmask, Amask);
    if (nr_written > sizeof (key)) {
        _WRN_PRINTF ("key was truncated\n");
    }

    if ((entry = __mg_map_find (__mg_sys_bmp_map, key))) {
        return (BITMAP*)entry->val;
    }
    else {
        BITMAP* bmp;
        MYBITMAP* mybmp;
        RGB* pal;

        if ((mybmp = LoadMyBitmapFromRes (file, &pal)) == NULL)
            goto error;

        bmp = malloc (sizeof (BITMAP));
        if (ExpandMyBitmap (hdc, bmp, mybmp, pal, 0)) {
            free (bmp);
            goto error;
        }

        if (__mg_map_insert (__mg_sys_bmp_map, key, bmp)) {
            UnloadBitmap (bmp);
            free (bmp);
            goto error;
        }

        return bmp;
    }

error:
    return NULL;
}

const BITMAP* GUIAPI GetSystemBitmap (HWND hWnd, const char* id)
{
    HDC hdc;
    const BITMAP* bmp;
    const WINDOWINFO* win_info;

    if (hWnd == HWND_NULL)
        return NULL;

    win_info = GetWindowInfo (hWnd);
    
    hdc = GetDC (hWnd);
    bmp = GetSystemBitmapEx2 (hdc, win_info->we_rdr->name, id);
    ReleaseDC (hdc);

    return bmp;
}

BOOL GUIAPI RegisterSystemBitmap (HDC hdc, const char* rdr_name, const char* id)
{
    char file[MAX_NAME + 1] = "bmp/";
    char* file_name;
    char key [MAX_NAME + 64];
    int nr_written;
    Uint32 Rmask, Gmask, Bmask, Amask;
    map_entry_t* entry;

    file_name = file + strlen(file);

    if (GetMgEtcValue (rdr_name, id,
                file_name, sizeof(file) - (file_name - file)) < 0)
        return FALSE;

    /* XXX: we use the RGBA masks for different pixel formats.
       This does not work for surface with pallete. */
    Rmask = GetGDCapability (hdc, GDCAP_RMASK);
    Gmask = GetGDCapability (hdc, GDCAP_GMASK);
    Bmask = GetGDCapability (hdc, GDCAP_BMASK);
    Amask = GetGDCapability (hdc, GDCAP_AMASK);
    nr_written = snprintf (key, sizeof (key), "%s-%s-%x%x%x%x", file, id,
            Rmask, Gmask, Bmask, Amask);
    if (nr_written > sizeof (key)) {
        _WRN_PRINTF ("key was truncated\n");
    }

    if ((entry = __mg_map_find (__mg_sys_bmp_map, key))) {
        return TRUE;
    }
    else {
        BITMAP* bmp;
        MYBITMAP* mybmp;
        RGB* pal;

        if ((mybmp = LoadMyBitmapFromRes (file, &pal)) == NULL)
            return FALSE;

        bmp = malloc (sizeof (BITMAP));
        if (ExpandMyBitmap (hdc, bmp, mybmp, pal, 0)) {
            free (bmp);
            return FALSE;
        }

        if (__mg_map_insert (__mg_sys_bmp_map, key, bmp)) {
            UnloadBitmap (bmp);
            free (bmp);
            return FALSE;
        }

        _DBG_PRINTF ("Registered system bitmap for key: %s\n", key);
    }

    return TRUE;
}

void GUIAPI UnregisterSystemBitmap (HDC hdc, const char* rdr_name, const char* id)
{
    char file[MAX_NAME + 1] = "bmp/";
    char* filename = file + strlen(file);
    char key [MAX_NAME + 64];
    int nr_written;
    Uint32 Rmask, Gmask, Bmask, Amask;

    if (GetMgEtcValue (rdr_name, id,
                filename, sizeof(file) - (filename - file)) < 0)
        return;

    /* XXX: we use the RGBA masks for different pixel formats.
       This does not work for surface with pallete. */
    Rmask = GetGDCapability (hdc, GDCAP_RMASK);
    Gmask = GetGDCapability (hdc, GDCAP_GMASK);
    Bmask = GetGDCapability (hdc, GDCAP_BMASK);
    Amask = GetGDCapability (hdc, GDCAP_AMASK);
    nr_written = snprintf (key, sizeof (key), "%s-%s-%x%x%x%x", file, id,
            Rmask, Gmask, Bmask, Amask);
    if (nr_written > sizeof (key)) {
        _WRN_PRINTF ("key was truncated\n");
    }

    if (__mg_map_erase (__mg_sys_bmp_map, key)) {
        _WRN_PRINTF ("not registered system bitmap for key: %s\n", key);
        return;
    }

    ReleaseRes (Str2Key (file));
}

/* The hash table size should be a prime. The following table gives all primes
 * less than 1000:
 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107
 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223
 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337
 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457
 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593
 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719
 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857
 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997
*/
BOOL mg_InitSystemRes (void)
{
#ifdef _MGCHARSET_UNICODE
#   ifdef _MGCOMPLEX_SCRIPTS
#       define _SIZE_RES_HASH_TABLE 277
#   else
#       define _SIZE_RES_HASH_TABLE 167
#   endif
#else
#   define _SIZE_RES_HASH_TABLE 71
#endif

    mg_InitResManager(_SIZE_RES_HASH_TABLE);

#undef _SIZE_RES_HASH_TABLE

    __sysres_init_inner_resource();
    return TRUE;
}

void mg_TerminateSystemRes (void)
{
    TerminateResManager();
}

#if 0   /* deprecated code */
/* This function is redundant absolutely. Since 5.0.0, it is defined as an alias
   of GetSystemBitmap. */
const BITMAP* GUIAPI GetSystemBitmapByHwnd (HWND hWnd, const char* id)
{
    const WINDOWINFO* win_info;

    if (hWnd == HWND_NULL) {
        return NULL;
    }

    win_info = GetWindowInfo(hWnd);
    return GetSystemBitmapEx (win_info->we_rdr->name, id);
}
#endif  /* deprecated code */

