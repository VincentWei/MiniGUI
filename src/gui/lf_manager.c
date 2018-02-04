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
** lf_manager.c: The Look and Feel manager file.
*/

#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "element.h"

/* The default maximum number of LF renderers */
#define MAX_NR_RENDERERS    6

typedef struct _LFINFO {
    char name[LEN_RENDERER_NAME + 1];
    WINDOW_ELEMENT_RENDERER * wnd_rdr;
} LFINFO;

extern WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_classic;

#ifdef _MGLF_RDR_FLAT
extern WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_flat;
#endif

#ifdef _MGLF_RDR_SKIN
extern WINDOW_ELEMENT_RENDERER __mg_wnd_rdr_skin;
#endif

#ifdef _MGRM_THREADS
/** For MiniGUI renderer manager. */
static pthread_mutex_t  gRendererMmutex;
#endif

static LFINFO wnd_lf_info [MAX_NR_RENDERERS] =
{
    {"classic", &__mg_wnd_rdr_classic},
#ifdef _MGLF_RDR_FLAT
    {"flat", &__mg_wnd_rdr_flat},
#endif
#ifdef _MGLF_RDR_SKIN
    {"skin", &__mg_wnd_rdr_skin},
#endif
};

/** The default renderer */
WINDOW_ELEMENT_RENDERER * __mg_def_renderer = &__mg_wnd_rdr_classic;

WINDOW_ELEMENT_RENDERER* GUIAPI GetWindowRendererFromName (const char* name)
{
    int i;
    if (NULL == name)
        return NULL;
    for (i = 0; i < MAX_NR_RENDERERS; ++i)
    {
        /** find matching renderer by name which is case-insensitive */
        if (0 == strcasecmp (name, wnd_lf_info[i].name))
        {
            return wnd_lf_info[i].wnd_rdr;
        }
    }
    return NULL;
}

BOOL GUIAPI AddWindowElementRenderer (const char* name, 
        const WINDOW_ELEMENT_RENDERER* we_rdr)
{
    int i;
    int available_pos = -1;
    if (NULL == name || '\0' == name[0] || NULL == we_rdr)
        return FALSE;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&gRendererMmutex);
#endif

    for (i = 0; i < MAX_NR_RENDERERS; ++i)
    {
        /** find matching renderer by name which is case-insensitive */
        if (0 == strcasecmp (name, wnd_lf_info[i].name))
        {
            available_pos = -1;
            break;
        }
        
        /** find available slot */
        if (-1 == available_pos && '\0' == wnd_lf_info[i].name[0]) 
        {
            available_pos = i;
            break;
        }
    }

    /** no available slot */
    if (-1 == available_pos)
    {
#ifdef _MGRM_THREADS
        pthread_mutex_unlock (&gRendererMmutex);
#endif 
        return FALSE; 
    }

    /** get available slot */
    strncpy (wnd_lf_info[i].name, name, LEN_RENDERER_NAME);
    wnd_lf_info[i].name[LEN_RENDERER_NAME] = '\0';
    wnd_lf_info[i].wnd_rdr = (WINDOW_ELEMENT_RENDERER*) we_rdr;

    /** initialize renderer private info */
    if (0 != we_rdr->init ((WINDOW_ELEMENT_RENDERER*)we_rdr))
    {
#ifdef _MGRM_THREADS
        pthread_mutex_unlock (&gRendererMmutex);
#endif 
        return FALSE;
    }

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&gRendererMmutex);
#endif 
    return TRUE;
}

BOOL GUIAPI RemoveWindowElementRenderer (const char* name)
{
    int i;
    BOOL find = FALSE;
    if (NULL == name)
        return FALSE;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&gRendererMmutex);
#endif 

    /** find matching renderer by name which is case-insensitive */
    for (i = 0; i < MAX_NR_RENDERERS; ++i)
    {
        if (0 == strcasecmp (name, wnd_lf_info[i].name))
        {
            find = TRUE;
            break;
        }
    }
    
    /** find it out, then continue*/
    if (FALSE == find || NULL == wnd_lf_info[i].wnd_rdr 
            || 0 != wnd_lf_info[i].wnd_rdr->refcount)
    {
#ifdef _MGRM_THREADS
        pthread_mutex_unlock (&gRendererMmutex);
#endif 
        return FALSE;
    }
    
    /** release and clean renderer data as refcount is 0 */ 
    wnd_lf_info[i].wnd_rdr->deinit (wnd_lf_info[i].wnd_rdr);
    wnd_lf_info[i].wnd_rdr = NULL;
    wnd_lf_info[i].name[0] = '\0';

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&gRendererMmutex);
#endif 

    return TRUE;
}

WINDOW_ELEMENT_RENDERER * GUIAPI GetDefaultWindowElementRenderer (void)
{
    return __mg_def_renderer;
}

const char* GUIAPI SetDefaultWindowElementRenderer (const char* name)
{
    int i;
    BOOL find = FALSE;
    const char* old_name;

    if (NULL == name || '\0' == name[0])
    {
        return NULL;
    }

    /** return if default renderer is not different */
    if (0 == strcasecmp (name, __mg_def_renderer->name))
        return __mg_def_renderer->name;

    /** find matching renderer by name which is case-insensitive */
    for (i = 0; i < MAX_NR_RENDERERS; ++i)
    {
        if (0 == strcasecmp (name, wnd_lf_info[i].name))
        {
            find = TRUE;
            break;
        }
    }

    /** find it out, then continue*/
    if (FALSE == find)
    {
        return NULL;
    }

    old_name = __mg_def_renderer->name;
    /** reset new renderer as different name which is case-insensitive */
    __mg_def_renderer = wnd_lf_info[i].wnd_rdr;
    __mg_dsk_win->we_rdr = wnd_lf_info[i].wnd_rdr;

    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    return old_name;
}

BOOL GUIAPI SetWindowElementRenderer (HWND hWnd, const char* werdr_name, 
        const WINDOW_ELEMENT_ATTR* we_attrs)
{
    int i;
    BOOL update = FALSE;
    WINDOW_ELEMENT_RENDERER * rdr;
    PMAINWIN pwnd;

    MG_CHECK_RET (MG_IS_NORMAL_WINDOW (hWnd), FALSE);
    pwnd = MG_GET_WINDOW_PTR(hWnd);

    if (NULL == pwnd || 
            ((NULL == werdr_name || '\0' == werdr_name) && NULL == we_attrs))
    {
        return FALSE;
    }

    /** ingnore this window style */
    if (pwnd->dwExStyle & WS_EX_USEPARENTRDR)
    {   
        return FALSE;
    }   

    rdr = (WINDOW_ELEMENT_RENDERER*)GetWindowRendererFromName (werdr_name); 

    /** change to new renderer */
    if (NULL != rdr)
    {
        if(0 != strcasecmp (pwnd->we_rdr->name, werdr_name))
        {
            pwnd->we_rdr = rdr;
            update = TRUE;
        }
    }

    /** append element attributes */
    if (NULL != we_attrs)
    {
        update = TRUE;
        for (i = 0; we_attrs[i].we_attr_id != -1; ++i)
        {
            append_window_element_data (&pwnd->wed, 
                    we_attrs[i].we_attr_id, we_attrs[i].we_attr);
        }
    }
    
    /** update window with new renderer and attributes */
    if (update)
        UpdateWindow (hWnd, TRUE);
    return TRUE;
}

/************ window element block data heap interface ***********/
#define WE_HEAP 1

#ifdef WE_HEAP

#define SIZE_WE_HEAP 32
static BLOCKHEAP we_heap;

/* 
 * init_we_heap:
 *
 * This function initializes a block data heap. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
static void init_we_heap (void)
{
    InitBlockDataHeap (&we_heap, sizeof (WND_ELEMENT_DATA), SIZE_WE_HEAP);
    return;
}

/* 
 * alloc_we_attr:
 *
 * This function request a block mem from block data heap. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
static WND_ELEMENT_DATA* alloc_we_attr (void)
{
    return (WND_ELEMENT_DATA*) BlockDataAlloc (&we_heap);
}

/* 
 * free_we_attr:
 *
 * This function return a block mem to block data heap. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
static void free_we_attr (WND_ELEMENT_DATA* wed)
{
    BlockDataFree (&we_heap, wed);
    return;
}


/* 
 * destroy_we_heap:
 *
 * This function destroy block data heap. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
static void destroy_we_heap (void)
{
    DestroyBlockDataHeap (&we_heap);
    return;
}

#endif /*End of WE_HEAP*/

/******** get/set window element interface *****************/

/* 
 * get_attr_from_rdr:
 *
 * This function get a attribute value from render. 
 *
 * Author: Peng LIU
 * Date: 2007-12-17
 */
static DWORD get_attr_from_rdr (WINDOW_ELEMENT_RENDERER *rdr, int we_attr_id)
{
    int index;

    index = we_attr_id & WE_ATTR_INDEX_MASK;
    switch (we_attr_id & WE_ATTR_TYPE_MASK) {

        case WE_ATTR_TYPE_FONT:
            return (DWORD)rdr->we_fonts[index];

        case WE_ATTR_TYPE_METRICS:
            return (DWORD)rdr->we_metrics[index];

        case WE_ATTR_TYPE_COLOR: {
            int color_index = (we_attr_id & WE_ATTR_TYPE_COLOR_MASK) >> 8;
            return rdr->we_colors[index][color_index];
        }

        case WE_ATTR_TYPE_ICON:
        {
            int icon_idx = (we_attr_id & WE_ATTR_TYPE_ICON_MASK) >> 8;
            return (DWORD)rdr->we_icon [icon_idx][index];
        }

        case WE_ATTR_TYPE_RDR: 
            if (!rdr->on_get_rdr_attr)
                break;
            return rdr->on_get_rdr_attr (rdr, we_attr_id);
    }

    return -1;    
}

static int get_window_element_data (HWND hwnd, Uint32 id, DWORD* data)
{
    PMAINWIN pwnd = (PMAINWIN)hwnd;
    list_t* me;

    if (pwnd == NULL)
        return WED_INVARG;

    if (pwnd->wed == NULL)
        return WED_NODEFINED;

    list_for_each (me, &pwnd->wed->list) {
        WND_ELEMENT_DATA* wed;
        wed = list_entry (me, WND_ELEMENT_DATA, list);
        if (wed->id == id) {
            *data = wed->data;
            return WED_OK;
        }
    }

    return WED_NOTFOUND;
}

int
set_window_element_data (HWND hwnd, Uint32 id, DWORD new_data, DWORD* old_data)
{
    PMAINWIN pwnd = (PMAINWIN)hwnd;
    WND_ELEMENT_DATA* new_wed;
    list_t* me;


    if (pwnd == NULL)
        return WED_INVARG;

    *old_data = get_attr_from_rdr (pwnd->we_rdr, id);

    if ((id& WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_COLOR) {
        int index = id & WE_ATTR_INDEX_MASK;
        int color_index = (id & WE_ATTR_TYPE_COLOR_MASK) >> 8;
        /* SetWindowBkColor.*/
        if ( index == WE_WINDOW && color_index == 1) {
            SetWindowBkColor(hwnd, RGBA2Pixel(HDC_SCREEN, GetRValue(new_data), 
                        GetGValue(new_data), GetBValue(new_data), GetAValue(new_data)));
        }
    }

    if (pwnd->wed == NULL) {
        //*******************   create   *************************/
#ifdef WE_HEAP
        pwnd->wed = alloc_we_attr ();
#else
        pwnd->wed = calloc (1, sizeof (WND_ELEMENT_DATA));
#endif

        if (pwnd->wed == NULL)
            return WED_MEMERR;

        INIT_LIST_HEAD (&pwnd->wed->list);

#ifdef WE_HEAP
        new_wed = alloc_we_attr ();
#else
        new_wed = calloc (1, sizeof (WND_ELEMENT_DATA));
#endif
        if (new_wed == NULL)
            return WED_MEMERR;

        if (((id & WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_RDR)
                && pwnd->we_rdr->on_set_rdr_attr) 
            new_data = pwnd->we_rdr->on_set_rdr_attr (pwnd->we_rdr, id, new_data, FALSE); 
        
        new_wed->id = id;
        new_wed->data = new_data;
        list_add_tail (&new_wed->list, &pwnd->wed->list);

        return WED_NEW_DATA;
    }
    else {
        //*******************   change   *************************/
        list_for_each (me, &pwnd->wed->list) {
            WND_ELEMENT_DATA* wed;
            wed = list_entry (me, WND_ELEMENT_DATA, list);
            if (wed->id == id) {
                *old_data = wed->data;
            
                if (((id & WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_RDR)
                        && pwnd->we_rdr->on_set_rdr_attr) 
                    new_data = pwnd->we_rdr->on_set_rdr_attr (pwnd->we_rdr, id, new_data, FALSE);

                if (new_data == *old_data) {
                    return WED_NOT_CHANGED;
                }
                else {
                    wed->data = new_data;
           
                    return WED_OK;
                }
            }
        }

        //*******************   add   *************************/
#ifdef WE_HEAP
        new_wed = alloc_we_attr ();
#else
        new_wed = calloc (1, sizeof (WND_ELEMENT_DATA));
#endif
        if (new_wed == NULL)
            return WED_MEMERR;

        new_wed->id = id;
        if (((id & WE_ATTR_TYPE_MASK) == WE_ATTR_TYPE_RDR)
                && pwnd->we_rdr->on_set_rdr_attr) 
            new_wed->data = pwnd->we_rdr->on_set_rdr_attr (pwnd->we_rdr, id, new_data, FALSE); 
        else
            new_wed->data = new_data;
       
        list_add_tail (&new_wed->list, &pwnd->wed->list);
        return WED_NEW_DATA;
    }
    return WED_NEW_DATA;
}

int free_window_element_data (HWND hwnd)
{
    PMAINWIN pwnd = (PMAINWIN)hwnd;
    WND_ELEMENT_DATA* wed;

    if (pwnd == NULL)
        return WED_INVARG;

    if (pwnd->wed == NULL)
        return WED_OK;

    while (!list_empty (&pwnd->wed->list)) {
        wed = list_entry (pwnd->wed->list.next, WND_ELEMENT_DATA, list);
        list_del (&wed->list);
#ifdef WE_HEAP
        free_we_attr (wed);
#else
        free (wed);
#endif
    }
   
#ifdef WE_HEAP 
    free_we_attr (pwnd->wed);
#else
    free (pwnd->wed);
#endif

    pwnd->wed = NULL;
    return WED_OK;
}

int append_window_element_data (WND_ELEMENT_DATA** wed, Uint32 id, DWORD data)
//int append_window_element_data (PMAINWIN pwnd, Uint32 id, DWORD data)
{
    WND_ELEMENT_DATA* new_wed;
 
    if (*wed == NULL) {
#ifdef WE_HEAP
        *wed = alloc_we_attr ();
#else
        *wed = calloc (1, sizeof (WND_ELEMENT_DATA));
#endif
        if (*wed == NULL)
            return WED_MEMERR;

        INIT_LIST_HEAD (&(*wed)->list);
    }

#ifdef WE_HEAP
    new_wed = alloc_we_attr ();
#else       
    new_wed = calloc (1, sizeof (WND_ELEMENT_DATA));
#endif

    if (new_wed == NULL)
        return WED_MEMERR;

    new_wed->id = id;
    new_wed->data = data;
    list_add_tail (&new_wed->list, &(*wed)->list);

    return WED_NEW_DATA;
}

#ifdef _DEBUG
void dump_window_element_data (HWND hwnd)
{
    PMAINWIN pwnd = (PMAINWIN)hwnd;
    list_t* me;
    WND_ELEMENT_DATA* wed;

    if (pwnd == NULL)
        return;

    if (pwnd->wed == NULL) {
        _MG_PRINTF ("GUI>DumpWED: No attributes in list.\n");
        return;
    }

    _MG_PRINTF ("GUI>DumpWED: The attributes of window elements:\n");
    list_for_each (me, &pwnd->wed->list) {
        wed = list_entry (me, WND_ELEMENT_DATA, list);
        switch (wed->id & WE_ATTR_TYPE_MASK) {
            case WE_ATTR_TYPE_FONT:
                _MG_PRINTF ("\tfont in list:%s-%s-%s.\n", 
                         ((PLOGFONT)(wed->data))->type, 
                         ((PLOGFONT)(wed->data))->family, 
                         ((PLOGFONT)(wed->data))->charset);
                break;

            case WE_ATTR_TYPE_METRICS:
                _MG_PRINTF ("\tmetrics in list:%d.\n", (int)wed->data);
                break;

            case WE_ATTR_TYPE_COLOR: 
                _MG_PRINTF ("\tcolor in list: %p.\n", (PVOID)wed->data); 
        }
    }
    _MG_PRINTF ("GUI>DumpWED: Done\n");
}

#endif /* _DEBUG */

/* 
 * SetWindowElementAttr:
 *
 * This function set a attribute value. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
DWORD GUIAPI SetWindowElementAttr (HWND hwnd, int we_attr_id, DWORD we_attr)
{
    DWORD old_data;
    int index;

    if (hwnd == HWND_DESKTOP || hwnd == HWND_NULL) {
        index = we_attr_id & WE_ATTR_INDEX_MASK;
        switch (we_attr_id & WE_ATTR_TYPE_MASK) {
            case WE_ATTR_TYPE_FONT:
                old_data = (DWORD)__mg_def_renderer->we_fonts[index];
                __mg_def_renderer->we_fonts[index] = (PLOGFONT)we_attr;
                return old_data;

            case WE_ATTR_TYPE_METRICS:
                old_data = __mg_def_renderer->we_metrics[index];
                __mg_def_renderer->we_metrics[index] = (int)we_attr;
                return old_data;

            case WE_ATTR_TYPE_COLOR: {
                int color_index = (we_attr_id & WE_ATTR_TYPE_COLOR_MASK) >> 8;
                old_data = (DWORD)__mg_def_renderer->we_colors[index][color_index];
                __mg_def_renderer->we_colors[index][color_index] = we_attr;
                return old_data;
            }

            case WE_ATTR_TYPE_ICON:
            {
                int icon_idx = (we_attr_id & WE_ATTR_TYPE_ICON_MASK) >> 8;
                old_data = (DWORD)__mg_def_renderer->we_icon [icon_idx][index];
                __mg_def_renderer->we_icon [icon_idx][index] = (HICON)we_attr;
                return old_data;
            }

            case WE_ATTR_TYPE_RDR: 
                return __mg_def_renderer->on_set_rdr_attr (__mg_def_renderer, we_attr_id, we_attr, TRUE);
        }
    }

    if (set_window_element_data (hwnd, we_attr_id, we_attr, &old_data) >= 0)
        return old_data;

    return -1;
}


/* 
 * GetWindowElementAttr:
 *
 * This function get a attribute value. 
 *
 * Author: Peng LIU
 * Date: 2007-11-22
 */
DWORD GUIAPI GetWindowElementAttr (HWND hwnd, int we_attr_id)
{
    DWORD data;
    WINDOW_ELEMENT_RENDERER *rdr;
    PMAINWIN pwnd;

    if (hwnd == HWND_DESKTOP || hwnd == HWND_NULL) {
        rdr =  __mg_def_renderer;
    }
    else {
        if (get_window_element_data (hwnd, we_attr_id, &data) == WED_OK)
            return (DWORD)data;
        pwnd = (PMAINWIN)hwnd;
        rdr = pwnd->we_rdr;
    }

    data = get_attr_from_rdr (rdr, we_attr_id);
    return data;
}

gal_pixel GUIAPI 
GetWindowElementPixelEx (HWND hwnd, HDC hdc, int we_attr_id)
{
    DWORD data;
    Uint8 r, g, b, a;
    HDC dc;
    gal_pixel pixel;
   
    if ((we_attr_id & WE_ATTR_TYPE_MASK) != WE_ATTR_TYPE_COLOR) {
        _MG_PRINTF ("GUI: Can't get color; invalid attr id: %x.\n", we_attr_id);
        return -1;
    }

    data = GetWindowElementAttr (hwnd, we_attr_id);   
    if (data < 0)
        return -1;

    r = GetRValue (data);
    g = GetGValue (data);
    b = GetBValue (data);
    a = GetAValue (data);

    if (hdc == HDC_INVALID) {
        if (hwnd == HWND_NULL || hwnd == HWND_DESKTOP)
            pixel = RGBA2Pixel (HDC_SCREEN, r, g, b, a);
        else {
            dc = GetDC (hwnd);
            pixel = RGBA2Pixel (dc, r, g, b, a);
            ReleaseDC (dc);
        }
    }
    else
        pixel = RGBA2Pixel (hdc, r, g, b, a);

    return pixel;
}

static const char* szMWMKeyNames [WE_METRICS_NUMBER] = {
    "caption",
    "menu",
    NULL,
    NULL,
    "border",
    "scrollbar"
};

static const char* szCKeyNames [WE_COLORS_NUMBER][3] = {
    {"fgc_active_caption", "bgca_active_caption", "bgcb_active_caption"},
    {"fgc_menu", "bgc_menu"},
    {"fgc_msgbox"},
    {"fgc_tip", "bgc_tip"},
    {"fgc_active_border", "fgc_inactive_border"},
    {NULL,NULL,NULL},
    {"fgc_inactive_caption", "bgca_inactive_caption", "bgcb_inactive_caption"},
    {"fgc_window", "bgc_window"},
    {"fgc_3dbox", "mainc_3dbox"},
    {"fgc_selected_item", "bgc_selected_item", "bgc_selected_lostfocus"},
    {"fgc_disabled_item", "bgc_disabled_item"},
    {"fgc_hilight_item", "bgc_hilight_item"},
    {"fgc_significant_item", "bgc_significant_item"},
    {NULL, "bgc_desktop"},
};

/* 
 * InitWindowElementAttrs:
 *
 * This function initializes window element attributes of the specified 
 * renderer according to the section of renderer name in MiniGUI.cfg.
 *
 * Author: XiaoweiYan
 * Date: 2007-11-22
 */
BOOL GUIAPI InitWindowElementAttrs (PWERENDERER rdr)
{
    int i, j, tmp;
    char buff [20];
    DWORD rgba;

    if (!rdr || !rdr->name || rdr->name[0] == '\0')
        return FALSE;

    _MG_PRINTF ("GUI>InitWEA: Initialize %s renderer window element attributes.\n", 
            rdr->name);

    for (i = 0; i < WE_METRICS_NUMBER; i++) {

        if (szMWMKeyNames[i]) {
            if ( GetMgEtcValue (rdr->name, szMWMKeyNames[i], 
                    buff, 12) != ETC_OK)
                return FALSE;
            else {
                tmp = atoi (buff);
                rdr->we_metrics [i] = tmp;
            }
        }
    }

    for (i = 0; i < WE_COLORS_NUMBER; i++) {

        if (i == 5)
            continue;

        for (j = 0; j < 3; j++) {

            if (szCKeyNames[i][j]) {
                if (GetMgEtcValue (rdr->name, szCKeyNames[i][j], 
                        buff, 12) != ETC_OK) {
                    _MG_PRINTF ("GUI>InitWEA: InitWindowElementAttrs error: \
                            Can not Get %s color. \n", szCKeyNames[i][j]);
                    return FALSE;
                }

                else {
                    rgba = strtoul (buff, NULL, 0);
                    rdr->we_colors [i][j] = rgba;
                }
            }
        }
    }

    return TRUE;
}

BOOL mg_InitLFManager (void)
{
    int i;
    WINDOW_ELEMENT_RENDERER * rdr;
#ifdef _MGRM_THREADS
    pthread_mutex_init (&gRendererMmutex, NULL);
#endif

    /** be sure to success initializing default renderer */
    rdr = wnd_lf_info[0].wnd_rdr;
    if (NULL == rdr) 
        return FALSE;

    if (rdr->init && 0 != rdr->init (rdr)) {
        _MG_PRINTF ("GUI>InitLFM: failed to initialize default renderer's private info.\n");
        return FALSE;
    }

    /** initialize other renderer */
    for (i = 1; i < MAX_NR_RENDERERS; ++i) 
    {
        rdr = wnd_lf_info [i].wnd_rdr;
        if (NULL == rdr)   continue;

        if (rdr->init && 0 != rdr->init (rdr)) {
            wnd_lf_info[i].wnd_rdr = NULL;
            _MG_PRINTF ("GUI>InitLFM: fail to initialize renderer[%d]'s private info.\n", i);
            continue;
        }
    }

#ifdef WE_HEAP
    /** initialize window element block data heap.*/
    init_we_heap();
#endif

    return TRUE;
}

void mg_TerminateLFManager (void)
{
    int i;
    WINDOW_ELEMENT_RENDERER * rdr;

    /** terminate renderer */
    for (i = 0; i < MAX_NR_RENDERERS; ++i) {
        rdr = wnd_lf_info [i].wnd_rdr;
        if (rdr && rdr->deinit) {
            rdr->deinit (rdr);
        }
    }

#ifdef _MGRM_THREADS
        pthread_mutex_destroy (&gRendererMmutex);
#endif
#ifdef WE_HEAP
    destroy_we_heap ();
#endif
}

