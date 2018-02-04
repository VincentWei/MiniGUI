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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//#include <unistd.h>

#define BASE					100
#define BUTTON_ALPHA1		BASE+0
#define BUTTON_ALPHA2		BASE+1
#define BUTTON_ALPHA3		BASE+2

#define BASE2					200
#define BUTTON_COLORKEY1	BASE2+0
#define BUTTON_COLORKEY2	BASE2+1
#define BUTTON_COLORKEY3	BASE2+2
#define BUTTON_RESETOFF		BASE2+3

#define BASE3					300
#define BUTTON_DELETE1		BASE3+0
#define BUTTON_DELETE2		BASE3+1
#define BUTTON_DELETE3		BASE3+2

#define BASE4					400
#define BUTTON_ZORDER1		BASE4+0
#define BUTTON_ZORDER2		BASE4+1
#define BUTTON_ZORDER3		BASE4+2

#define BUTTON_MASTERALPHA  BASE4+3

static DLGTEMPLATE DlgYourTaste =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    180, 0, 450, 180,
    "mlshadow test",
    0, 0,
    12, NULL,
    0
};

static CTRLDATA CtrlYourTaste[] =
{ 
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        30, 20, 80, 28,
        BUTTON_ALPHA1, 
        "Set alpha[1]",
        0
    },
   {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        130, 20, 80, 28,
        BUTTON_COLORKEY1, 
        "Set colorkey[1]",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        230, 20, 80, 28,
        BUTTON_DELETE1, 
        "Delete [1]",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        330, 20, 80, 28,
        BUTTON_ZORDER1, 
        "Set zorder [1]",
        0
    },


    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        30, 60, 80, 28,
        BUTTON_ALPHA2,
        "Set alpha[2]",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        130, 60, 80, 28,
        BUTTON_COLORKEY2,
        "Set colorkey[2]",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        230, 60, 80, 28,
        BUTTON_DELETE2,
        "Delete [2]",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        330, 60, 80, 28,
        BUTTON_ZORDER2,
        "Set zorder [2]",
        0
    },

    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        30, 100, 110, 28,
        BUTTON_RESETOFF,
        "Reset offset-x(y)",
        0
    },	
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        150, 100, 110, 28,
        BUTTON_MASTERALPHA,
        "Set MasterAlpha",
        0
    },	
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        270, 100, 70, 28,
        IDOK, 
        "OK",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        350, 100, 70, 28,
        IDCANCEL,
        "Cancel",
        0
    },
};

#define SLAVE_NUM		2

void init_slave(void);
void clean_slave(void);

int step_index = 0;
int _x_step1 = 250;

pthread_t update_th;

HDC slave_hdc[SLAVE_NUM] = {0};
BITMAP bmpinfo[SLAVE_NUM];
char bmp_path[SLAVE_NUM][10] = {
    "1.jpg",
    "3.jpg",
};
BOOL alpha_en[SLAVE_NUM] = {0};
BOOL colorkey_en[SLAVE_NUM] = {0};
int	alpha[SLAVE_NUM] = {0x20, 0x20, 0x20};
char mode_string[3][255];
int	colorkey_index = 0;
gal_pixel color_key[SLAVE_NUM][2] = {{0x0,0xffff}, {0x0, 0xffff}, {0x0,0xffff}};

static void* do_update (void* data)
{
    int i = 0;
    int change_step = 0;
    while (1) {  
        usleep (30000);
        for(i = 0; i < SLAVE_NUM; i++){
            if(alpha_en[i]){
                if(alpha[i]++ > 0xff){
                    alpha[i] = 0;
                }
                if(slave_hdc[i])
                    mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_BLEND, 
                            0, 0, MLS_BLENDMODE_ALPHA, 0, alpha[i], 0);				
            }
            if(colorkey_en[i]){
                if(slave_hdc[i])	
                    mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_BLEND, 0, 0, 
                            MLS_BLENDMODE_COLORKEY, color_key[i][colorkey_index], 0, 0);						
            }
        }
        if(change_step++ > 0x10){
            change_step = 0;
            if(colorkey_index == 0)
                colorkey_index = 1;
            else
                colorkey_index = 0;
        }
    }
    return NULL;
}

static void alpha_set(int index, BOOL enabled)
{
    if(index < 0 || index > SLAVE_NUM) return;
    if(alpha_en[index])
        alpha_en[index] = FALSE;
    else
        alpha_en[index] = TRUE;
}

static void colorkey_set(int index, BOOL enabled)
{
    if(index < 0 || index > SLAVE_NUM) return;
    if(colorkey_en[index])
        colorkey_en[index] = FALSE;
    else
        colorkey_en[index] = TRUE;	
}

void create_surface(int i)
{
    int ret = 0;
    if(slave_hdc[i]) return;
    if((ret = LoadBitmapFromFile(HDC_SCREEN, (PBITMAP)&bmpinfo[i], bmp_path[i])) == ERR_BMP_OK){
        sprintf(mode_string[i], "%dx%d-%dbpp", bmpinfo[i].bmWidth, 
                bmpinfo[i].bmHeight, bmpinfo[i].bmBitsPerPixel);
        printf(mode_string[i]);
        slave_hdc[i] = InitSlaveScreen("mlshadow", mode_string[i]);
        mlsEnableSlaveScreen(slave_hdc[i], TRUE);
        mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_OFFSET, _x_step1*i+180, 250, 0, 0, 0, 0);
        FillBoxWithBitmap(slave_hdc[i], 0, 0, 0, 0, (PBITMAP)&bmpinfo[i]);
    }
}

void init_slave(void)
{
    int ret = 0, i = 0;
    for(i = 0; i < SLAVE_NUM; i++){
        create_surface(i);
    }
    {
        pthread_attr_t new_attr;
        pthread_attr_init(&new_attr);
#ifndef __LINUX__
        pthread_attr_setstacksize (&new_attr, 1024);
#endif
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);//PTHREAD_CREATE_DETACHED);
        ret = pthread_create(&update_th, &new_attr, do_update, NULL);
        pthread_attr_destroy(&new_attr);
    }
}

void clean_slave(void)
{
    int i = 0;
    for(i = 0; i < SLAVE_NUM; i++)
    {
        TerminateSlaveScreen(slave_hdc[i]);		
        slave_hdc[i] = 0;
    }		
}

static int DialogBoxProc2 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int index = 0, i = 0, step = 120;
    static int zorder = 0;
    static int zorder_index[SLAVE_NUM] = {0};
    static int zorder_data[SLAVE_NUM][SLAVE_NUM] ={{3, 2, 1}, {3,1,2}, {1, 2, 3}};
    char tmp_buf[10];

    switch (message) {
        case MSG_INITDIALOG:
            init_slave();
            return 1;
        case MSG_COMMAND:
            switch (wParam) {			
                case BUTTON_ALPHA1:
                case BUTTON_ALPHA2:
                case BUTTON_ALPHA3:	
                    index = wParam - BASE;			
                    alpha_set(index, TRUE);
                    break;

                case BUTTON_COLORKEY1:
                case BUTTON_COLORKEY2:
                case BUTTON_COLORKEY3:	
                    index = wParam - BASE2;			
                    colorkey_set(index, TRUE);														
                    break;

                case BUTTON_DELETE1:
                case BUTTON_DELETE2:
                case BUTTON_DELETE3:	
                    index = wParam - BASE3;
                    if(slave_hdc[index]){	
                        TerminateSlaveScreen(slave_hdc[index]);
                        slave_hdc[index] = 0;
                        sprintf(tmp_buf, "Create [%d]", index+1);
                        SetWindowCaption(GetDlgItem(hDlg, wParam), tmp_buf);						
                    }
                    else{
                        create_surface(index);
                        sprintf(tmp_buf, "Delete [%d]", index+1);
                        SetWindowCaption(GetDlgItem(hDlg, wParam), tmp_buf);
                    }
                    break;
                case BUTTON_ZORDER1:
                case BUTTON_ZORDER2:
                case BUTTON_ZORDER3:	
                    index = wParam - BASE4;
                    if(slave_hdc[index]){
                        if(zorder_index[index] > SLAVE_NUM-1) zorder_index[index] = 0;				
                        mlsSetSlaveScreenInfo(slave_hdc[index], MLS_INFOMASK_ZORDER, 
                                0, 0, 0, 0, 0, 
                                zorder_data[index][zorder_index[index]++]);									
                    }
                    break;					
                case BUTTON_RESETOFF:	
                    for(i = 0; i < SLAVE_NUM; i++){
                        if(!slave_hdc[i]) continue;
                        mlsEnableSlaveScreen(slave_hdc[i], TRUE);
                        if(i == SLAVE_NUM-1)
                            mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_OFFSET, 
                                    step*i-50, 325, 0, 0, 0, 0);
                        else
                            mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_OFFSET, 
                                    step*i+50, 200, 0, 0, 0, 0);							
                        if(zorder == 0){
                            mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_ZORDER, 
                                    0, 0, 0, 0, 0, SLAVE_NUM-i);
                        }
                        else{
                            mlsSetSlaveScreenInfo(slave_hdc[i], MLS_INFOMASK_ZORDER, 
                                    0, 0, 0, 0, 0, i+1);
                        }						
                    }
                    if(zorder == 0){
                        zorder = 1;
                    }
                    else{
                        zorder = 0;
                    }							
                    break;
                case BUTTON_MASTERALPHA:    
                    mlsSetSlaveScreenInfo(HDC_SCREEN, MLS_INFOMASK_BLEND, 0, 0, 
                            MLS_BLENDMODE_ALPHA, 0, 0x50, 0);
                    break;
                case IDOK:
                case IDCANCEL:
                    EndDialog (hDlg, wParam);
                    clean_slave();				
                    break;
            }
            break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
#ifdef _MGRM_PROCESSES
static pthread_mutex_t mutex;

static int __mg_lock_cli_req(void)
{
	return pthread_mutex_lock (&mutex); 
}

static int __mg_trylock_cli_req(void)
{
	return pthread_mutex_trylock (&mutex); 
}

static void __mg_unlock_cli_req(void)
{
	pthread_mutex_unlock (&mutex);
}
#endif
int MiniGUIMain (int argc, const char* argv[])
{
    #ifdef _MGRM_PROCESSES
	pthread_mutex_init (&mutex, NULL);

	OnLockClientReq = __mg_lock_cli_req;
	OnTrylockClientReq = __mg_trylock_cli_req;
	OnUnlockClientReq = __mg_unlock_cli_req;

	JoinLayer(NAME_DEF_LAYER , "mlshadowtestright" , 0 , 0);
    #endif
    DlgYourTaste.controls = CtrlYourTaste;
    DialogBoxIndirectParam (&DlgYourTaste, HWND_DESKTOP, DialogBoxProc2, 0L);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

