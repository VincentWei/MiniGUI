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
** The Input Abstract Layer of MiniGUI.
**
** Create date: 2000/06/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "minigui.h"
#include "misc.h"
#include "ial.h"

/* General IAL engiens ... */
#ifdef _MGIAL_QVFB
    #include "qvfb.h"
#endif
#ifdef _MGIAL_JZ4740
    #include "jz4740.h"
#endif
#ifdef _MGIAL_2440
    #include "2440.h"
#endif
#ifdef _MGIAL_WVFB
    #include "wvfb.h"
#endif
#ifdef _MGIAL_DUMMY
    #include "dummy.h"
#endif
#ifdef _MGIAL_AUTO
    #include "auto.h"
#endif
#ifdef _MGIAL_RANDOM
    #include "random.h"
#endif
#ifdef _MGIAL_COMM
    #include "comminput.h"
#endif
#ifdef _MGGAL_PCXVFB
    #include "pcxvfb.h"
#endif
#ifdef _MGGAL_RTOSXVFB
    #include "rtos_xvfb.h"
#endif
#ifdef _MGIAL_CONSOLE
    #include "native/native.h"
#endif
#ifdef _MGIAL_NET
    #include "netial/netial.h"
#endif
#ifdef _MGIAL_IPAQ_H3600
    #include "ipaq-h3600.h"
#endif
#ifdef _MGIAL_IPAQ_H5400
    #include "ipaq-h5400.h"
#endif
#ifdef _MGIAL_QEMU
   #include "qemu.h"
#endif
#ifdef _MGIAL_TSLIB
   #include "tslibial.h"
#endif
#ifdef _MGIAL_NEXUS
   #include "nexusial/nexus.h"
#endif
#ifdef _MGIAL_DLCUSTOM
   #include "dlcustom/dlcustom.h"
#endif
#ifdef _MGIAL_DAVINCI6446
    #include "davinci6446.h"
#endif
#ifdef _MGIAL_CISCO_TOUCHPAD
    #include "cisco_touchpad.h"
#endif
#ifdef _MGIAL_MSTAR
    #include "mstarial.h"
#endif
#ifdef _MGIAL_DFB
    #include "dfb.h"
#endif

#define LEN_ENGINE_NAME        16
#define LEN_MTYPE_NAME         16

static INPUT inputs [] = 
{
/* General IAL engines ... */
#ifdef _MGIAL_QVFB
    {"qvfb", InitQVFBInput, TermQVFBInput},
#endif
#ifdef _MGIAL_JZ4740
    {"jz4740", InitJZ4740Input, TermJZ4740Input},
#endif
#ifdef _MGIAL_2440
    {"2440", Init2440Input, Term2440Input},
#endif
#ifdef _MGIAL_WVFB
    {"wvfb", InitWVFBInput, TermWVFBInput},
#endif
#ifdef _MGIAL_DUMMY
    {"dummy", InitDummyInput, TermDummyInput},
#endif
#ifdef _MGIAL_AUTO
    {"auto", InitAutoInput, TermAutoInput},
#endif
#ifdef _MGIAL_RANDOM
    {"random", InitRandomInput, TermRandomInput},
#endif
#ifdef _MGIAL_CUSTOM
    {"custom", InitCustomInput, TermCustomInput},
#endif
#ifdef _MGIAL_DLCUSTOM
    {"dlcustom", InitDLCustomInput, TermDLCustomInput},
#endif
#ifdef _MGIAL_COMM
    {"comm", InitCOMMInput, TermCOMMInput},
#endif
#ifdef _MGGAL_PCXVFB
    {"pc_xvfb", InitPCXVFBInput, TermPCXVFBInput},
#endif
#ifdef _MGGAL_RTOSXVFB
    {"rtos_xvfb", InitRTOSXVFBInput, TermRTOSXVFBInput},
#endif
#ifdef _MGIAL_CONSOLE
    {"console", InitNativeInput, TermNativeInput},
#endif
#ifdef _MGIAL_NET
    {"net", InitNetInput, TermNetInput},
#endif
#ifdef _MGIAL_IPAQ_H3600
    {"ipaq3600", InitIPAQH3600Input, TermIPAQH3600Input},
#endif
#ifdef _MGIAL_IPAQ_H5400
    {"ipaq5400", InitIPAQH5400Input, TermIPAQH5400Input},
#endif
#ifdef _MGIAL_QEMU
    {"qemu", InitQEMUInput, TermQEMUInput},
#endif
#ifdef _MGIAL_TSLIB
    {"tslib", InitTSLibInput, TermTSLibInput},
#endif
#ifdef _MGIAL_NEXUS
	{"nexus", InitNexusInput, TermNexusInput},
#endif
#ifdef _MGIAL_DAVINCI6446
    {"davinci6446", InitDavinci6446Input, TermDavinci6446Input},
#endif
#ifdef _MGIAL_CISCO_TOUCHPAD
    {"cisco_touchpad", InitCiscoTouchpadInput, TermCiscoTouchpadInput},
#endif
#ifdef _MGIAL_MSTAR
    {"mstar", InitMStarInput, TermMStarInput},
#endif
#ifdef _MGIAL_DFB
    {"dfb", InitDFBInput, TermDFBInput},
#endif
/* ... end of general IAL engines */
};

INPUT* __mg_cur_input;
void (*__mg_ial_change_mouse_xy_hook) (int* x, int* y) = NULL;
#define NR_INPUTS  (sizeof (inputs) / sizeof (INPUT))

int mg_InitIAL (void)
{
    int  i;
#ifndef __NOUNIX__
    char* env_value;
#endif
    char engine [LEN_ENGINE_NAME + 1];
    char mdev [MAX_PATH + 1];
    char mtype[LEN_MTYPE_NAME + 1];

    if (NR_INPUTS == 0)
        return ERR_NO_ENGINE;

#ifndef __NOUNIX__
    if ((env_value = getenv ("MG_IAL_ENGINE"))) {
        strncpy (engine, env_value, LEN_ENGINE_NAME);
        engine [LEN_ENGINE_NAME] = '\0';
    }
    else
#endif
#ifndef _MG_MINIMALGDI
    if (GetMgEtcValue ("system", "ial_engine", engine, LEN_ENGINE_NAME) < 0)
        return ERR_CONFIG_FILE;
#else /* _MG_MINIMALGDI */
#   ifdef _MGGAL_PCXVFB
    strcpy(engine, "pc_xvfb");
#   else
    strcpy(engine, "dummy");
#   endif
#endif /* _MG_MINIMALGDI */

#ifndef __NOUNIX__
    if ((env_value = getenv ("MG_MDEV"))) {
        strncpy (mdev, env_value, MAX_PATH);
        mdev [MAX_PATH] = '\0';
    }
    else
#endif
    if (GetMgEtcValue ("system", "mdev", mdev, MAX_PATH) < 0)
        return ERR_CONFIG_FILE;

#ifndef __NOUNIX__
    if ((env_value = getenv ("MG_MTYPE"))) {
        strncpy (mtype, env_value, LEN_MTYPE_NAME);
        mtype [LEN_MTYPE_NAME] = '\0';
    }
    else
#endif
    if (GetMgEtcValue ("system", "mtype", mtype, LEN_MTYPE_NAME) < 0)
        return ERR_CONFIG_FILE;

    for (i = 0; i < NR_INPUTS; i++) {
        if (strncmp (engine, inputs[i].id, LEN_ENGINE_NAME) == 0) {
            __mg_cur_input = inputs + i;
            break;
        }
    }
   
    if (__mg_cur_input == NULL) {
        _ERR_PRINTF ("IAL: Does not find the request engine: %s.\n", engine);
        if (NR_INPUTS) {
            __mg_cur_input = inputs;
            _MG_PRINTF ("IAL: Use the first engine: %s\n", __mg_cur_input->id);
        }
        else
            return ERR_NO_MATCH;
    }

    strcpy (__mg_cur_input->mdev, mdev);

    if (!IAL_InitInput (__mg_cur_input, mdev, mtype)) {
        _ERR_PRINTF ("IAL: Init IAL engine failure.\n");
        return ERR_INPUT_ENGINE;
    }

    _DBG_PRINTF ("IAL: Use %s engine.\n", __mg_cur_input->id);

    return 0;
}

void mg_TerminateIAL (void)
{
    if (__mg_cur_input && __mg_cur_input->term_input) {
        IAL_TermInput ();
    }
}

#ifdef _MGHAVE_MOUSECALIBRATE

DO_MOUSE_CALIBRATE_PROC __mg_do_mouse_calibrate;
POINT __mg_mouse_org_pos;

void GUIAPI GetOriginalMousePosition (int* x, int* y)
{
    *x = __mg_mouse_org_pos.x;
    *y = __mg_mouse_org_pos.y;
}

#define NR_EQUATIONS    6
#define LSHIFT(x)       ((x)<<10)
#define RSHIFT(x)       ((x)>>12)

static int vars1 [NR_EQUATIONS], vars2 [NR_EQUATIONS], 
           vars3 [NR_EQUATIONS], vars4 [NR_EQUATIONS];

static void do_mouse_calibrate (int* x, int* y)
{
    int x1, y1, x2, y2, x3, y3, x4, y4;

    x1 = vars1 [0] * (*x) + vars1 [1] * (*y) + vars1 [2];
    y1 = vars1 [3] * (*x) + vars1 [4] * (*y) + vars1 [5];

    x2 = vars2 [0] * (*x) + vars2 [1] * (*y) + vars2 [2];
    y2 = vars2 [3] * (*x) + vars2 [4] * (*y) + vars2 [5];

    x3 = vars3 [0] * (*x) + vars3 [1] * (*y) + vars3 [2];
    y3 = vars3 [3] * (*x) + vars3 [4] * (*y) + vars3 [5];

    x4 = vars4 [0] * (*x) + vars4 [1] * (*y) + vars4 [2];
    y4 = vars4 [3] * (*x) + vars4 [4] * (*y) + vars4 [5];

    *x = RSHIFT (x1 + x2 + x3 + x4);
    *y = RSHIFT (y1 + y2 + y3 + y4);
}

static BOOL 
doGaussianElimination (int* x, const POINT* src_pts, const POINT* dst_pts)
{
    int x12, x23, y12, y23, nx12, nx23, ny12, ny23;
    int numerator, denominator1, denominator2;

    x12 = (src_pts [0].x - src_pts [1].x);
    x23 = (src_pts [1].x - src_pts [2].x);
    y12 = (src_pts [0].y - src_pts [1].y);
    y23 = (src_pts [1].y - src_pts [2].y);

    nx12 = (dst_pts [0].x - dst_pts [1].x);
    nx23 = (dst_pts [1].x - dst_pts [2].x);
    ny12 = (dst_pts [0].y - dst_pts [1].y);
    ny23 = (dst_pts [1].y - dst_pts [2].y);

    denominator1 = x12*y23 - x23*y12;
    if (denominator1 == 0)
        return FALSE;

    denominator2 = y12*x23 - y23*x12;
    if (denominator2 == 0)
        return FALSE;

    numerator = nx12*y23 - nx23*y12;
    x [0] = LSHIFT (numerator) / denominator1;
    numerator = nx12*x23 - nx23*x12;
    x [1] = LSHIFT (numerator) / denominator2;
    x [2] = LSHIFT (dst_pts [0].x) - x [0] * src_pts [0].x - x [1] * src_pts [0].y;
    
    numerator = ny12*y23 - ny23*y12;
    x [3] = LSHIFT (numerator) / denominator1;
    numerator = ny12*x23 - ny23*x12;
    x [4] = LSHIFT (numerator) / denominator2;
    x [5] = LSHIFT (dst_pts [0].y) - x [3] * src_pts [0].x - x [4] * src_pts [0].y;

    return TRUE;
}

BOOL GUIAPI 
SetMouseCalibrationParameters (const POINT* src_pts, const POINT* dst_pts)
{
    POINT my_src_pts [3];
    POINT my_dst_pts [3];

    my_src_pts [0] = src_pts [0];
    my_src_pts [1] = src_pts [1];
    my_src_pts [2] = src_pts [4];
    my_dst_pts [0] = dst_pts [0];
    my_dst_pts [1] = dst_pts [1];
    my_dst_pts [2] = dst_pts [4];
    if (!doGaussianElimination (vars1, my_src_pts, my_dst_pts))
        return FALSE;

    my_src_pts [0] = src_pts [1];
    my_src_pts [1] = src_pts [2];
    my_src_pts [2] = src_pts [4];
    my_dst_pts [0] = dst_pts [1];
    my_dst_pts [1] = dst_pts [2];
    my_dst_pts [2] = dst_pts [4];
    if (!doGaussianElimination (vars2, my_src_pts, my_dst_pts))
        return FALSE;

    my_src_pts [0] = src_pts [2];
    my_src_pts [1] = src_pts [3];
    my_src_pts [2] = src_pts [4];
    my_dst_pts [0] = dst_pts [2];
    my_dst_pts [1] = dst_pts [3];
    my_dst_pts [2] = dst_pts [4];
    if (!doGaussianElimination (vars3, my_src_pts, my_dst_pts))
        return FALSE;

    my_src_pts [0] = src_pts [0];
    my_src_pts [1] = src_pts [3];
    my_src_pts [2] = src_pts [4];
    my_dst_pts [0] = dst_pts [0];
    my_dst_pts [1] = dst_pts [3];
    my_dst_pts [2] = dst_pts [4];
    if (!doGaussianElimination (vars4, my_src_pts, my_dst_pts))
        return FALSE;

    __mg_do_mouse_calibrate = do_mouse_calibrate;
    return TRUE;
}

#endif /* _MGHAVE_MOUSECALIBRATE */

