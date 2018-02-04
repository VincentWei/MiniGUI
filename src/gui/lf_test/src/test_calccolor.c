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
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/fixedmath.h>
#include <minigui/window.h>
#include <assert.h>



//#include "3dbox.h"

#define VAL_DEBUG
#define INFO_DEBUG
#include "my_debug.h"


#define TO_LIGHTEST_COLOR   2
#define TO_LIGHTER_COLOR    1
#define TO_DARKER_COLOR     (-1)
#define TO_DARKEST_COLOR    (-2)

#define UNDEFINED_HUE   (itofix(-1))

#define FULL_V    (itofix(1))

#define DWINDLE_RECT(rc) \
    do \
    { \
        (rc).left++;      \
        (rc).top++;       \
        (rc).right--;     \
        (rc).bottom--;    \
    }while (0)



static void RGB2HSV(fixed r, fixed g, fixed b, fixed* h, fixed* s, fixed* v)
{
    fixed min; 
    fixed max; 
    fixed delta;
    fixed tmp;

    /*change r g b to [0, 1]*/
    r = fixdiv (r, itofix(255));
    g = fixdiv (g, itofix(255));
    b = fixdiv (b, itofix(255));

    tmp = MIN(r, g);
    min = MIN(tmp, b);
    tmp = MAX(r, g);
    max = MAX(tmp, b);

    *v = max; // v
    delta = max - min;

    if (max != 0)
        *s = fixdiv(delta, max);
    else {
        *s = 0;
        *h = UNDEFINED_HUE;
        return;
    }

    if (fixtof(delta) == 0)
    {
        *h = 0;
        *s = 0;
        return;
    }


    if (r == max)
        /*between yellow & magenta*/
        *h = fixdiv(fixsub(g, b), delta);
    else if( g == max )
        /*between cyan & yellow*/
        *h = fixadd(itofix(2), fixdiv(fixsub(b, r), delta));
    else 
        /*magenta & cyan*/
        *h = fixadd(itofix(4), fixdiv(fixsub(r, g), delta));

    /*degrees*/
    *h = fixmul(*h, itofix(60)); 
    if (*h < itofix(0))
        *h = fixadd(*h, itofix(360));

}

static void HSV2RGB(fixed h, fixed s, fixed v, fixed* r, fixed* g, fixed* b)
{
    int i;
    fixed f, p, q, t;

    if (s == 0) {
        *r = fixmul(v, itofix(255));
        *g = fixmul(v, itofix(255));
        *b = fixmul(v, itofix(255));
        return;
    }

    /*sector 0 to 5*/
    h = fixdiv(h, itofix(60));
    i = (h >> 16) % 6;
    /*factorial part of h*/
    f = fixsub(h, itofix(i));
    p = fixmul(v, fixsub(itofix(1), s));
    /*q = v * (1 - s*f)*/
    q = fixmul (v, fixsub (itofix(1), fixmul(s, f)));
   
    /*t = v * (1 - s*(1-f))*/
    t = fixmul (v, fixsub (itofix(1), fixmul(s, fixsub( itofix(1), f))));
    
    switch (i) {
        case 0: 
            *r = fixmul (v, itofix(255)); 
            *g = fixmul (t, itofix(255)); 
            *b = fixmul (p, itofix(255));
            break;
        case 1:
            *r = fixmul (q, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (p, itofix(255));
            break;
        case 2:
            *r = fixmul (p, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (t, itofix(255));
            break;
        case 3:
            *r = fixmul (p, itofix(255));
            *g = fixmul (q, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 4:
            *r = fixmul (t, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 5:
            *r = fixmul (v, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (q, itofix(255));
            break;
    }

}

static DWORD (*calc_3dbox_color) (DWORD color, int flag);

int MiniGUIMain (int argc, const char* argv[])
{
    fixed h, s, v;
    int r_val;
    int g_val;
    int b_val;
    fixed rr_val = 0;
    fixed gg_val = 0;
    fixed bb_val = 0;
    DWORD darker_color;
    DWORD darkest_color;
    DWORD lighter_color;
    DWORD lightest_color;
    
    const WINDOW_ELEMENT_RENDERER* def_rdr =
             GetWindowRendererFromName ("classic");
    calc_3dbox_color = def_rdr->calc_3dbox_color;

#if 1
    TEST_INFO("start test RGB2HSV and HSV2RGB");
    for (r_val=0; r_val<256; r_val+=1)
    {
        if (r_val % 15 == 0)
            fprintf(stderr, "   r=[%d, %d]\n", r_val, r_val+14);
        for (g_val=0; g_val<256; g_val+=1)
        {
            for (b_val=0; b_val<256; b_val+=1)
            {
                RGB2HSV(itofix(r_val), itofix(g_val), itofix(b_val), &h, &s, &v);
                HSV2RGB(h, s, v, &rr_val, &gg_val, &bb_val);

                assert (r_val == fixtoi(rr_val));
                assert (g_val == fixtoi(gg_val));
                assert (b_val == fixtoi(bb_val));
            }
        }
    }
    TEST_INFO("success ----  test RGB2HSV and HSV2RGB");
#endif

    TEST_INFO("start test classic - calc_3dbox_color");
    for (r_val=0; r_val<256; r_val+=1)
    {
        if (r_val % 15 == 0)
            fprintf(stderr, "   r=[%d, %d]\n", r_val, r_val+14);

        for (g_val=0; g_val<256; g_val+=1)
            for (b_val=0; b_val<256; b_val+=1)
            {
                lighter_color = calc_3dbox_color(MakeRGB(r_val, g_val, b_val), 
                        LFRDR_3DBOX_COLOR_LIGHTER);
                assert(GetRValue(lighter_color) >= r_val);
                assert(GetGValue(lighter_color) >= g_val);
                assert(GetBValue(lighter_color) >= b_val);

                lightest_color = calc_3dbox_color(MakeRGB(r_val, g_val, b_val), 
                        LFRDR_3DBOX_COLOR_LIGHTER);
                assert(GetRValue(lightest_color) >= GetRValue(lighter_color) );
                assert(GetGValue(lightest_color) >= GetGValue(lighter_color) );
                assert(GetBValue(lightest_color) >= GetBValue(lighter_color) );

                darker_color = calc_3dbox_color(MakeRGB(r_val, g_val, b_val), 
                        LFRDR_3DBOX_COLOR_DARKER);
                assert(GetRValue(darker_color) <= r_val);
                assert(GetGValue(darker_color) <= g_val);
                assert(GetBValue(darker_color) <= b_val);

                darkest_color = calc_3dbox_color(MakeRGB(r_val, g_val, b_val), 
                        LFRDR_3DBOX_COLOR_DARKEST);
                assert(GetRValue(darkest_color) <= GetRValue(darker_color) );
                assert(GetGValue(darkest_color) <= GetGValue(darker_color) );
                assert(GetBValue(darkest_color) <= GetBValue(darker_color) );
            }
    }
    TEST_INFO("success --- test classic calc_3dbox_color");

    return 0;
}

#ifdef _USE_MINIGUIENTRY
int main(int argc, char* argv[])
{
    main_entry(argc, argv);
    return 0;

}
#endif

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

