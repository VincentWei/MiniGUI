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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#include "define.h"
#include "xxvfbhdr.h"
#include "xxvfb_common.h"
#include "xxvfb_input.h"

XXVFBInfo xInfo;
static int displayid;
static int lockId = -1;
static char* data; //shell memory address

static int my_connect(int sockfd, struct sockaddr *address, int len)
{
	return connect(sockfd, address, len);
}

static Display* InitXDisplay(Display* display, int* screen, unsigned int* dis_width, unsigned int* dis_height)
{
    // 和X 服务器连接
    if ((display=XOpenDisplay(NULL)) == NULL )
    {
        printf("Cannot connect to X server %s\n",XDisplayName(NULL));
        exit(-1);
    }

    //获得缺省的 screen_num
    *screen = DefaultScreen(display);

    //获得屏幕的宽度和高度
    *dis_width = DisplayWidth(display, *screen);
    *dis_height = DisplayHeight(display, *screen);

    return display;
}

static void initLock()
{
    lockId = semget( displayid, 0, 0 );
}

static void lock()
{
    if ( lockId == -1 )
        initLock();
    {
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_op  = -1;
        sops.sem_flg = SEM_UNDO;

        semop(lockId,&sops,1);
    }
}
static void unlock()
{
    if ( lockId >= 0 ) {
        struct sembuf sops;

        sops.sem_num = 0;
        sops.sem_op  = +1;
        sops.sem_flg = SEM_UNDO;

        semop(lockId,&sops,1);
    }
}
static void DrawDirtyRect()
{
    int width, height, length;
    char* mem_data;
    char* fb_data;
    int bpp = (xInfo.xhdr->depth+7)/8;

    if(!xInfo.xhdr->dirty) return;

    lock();

    if(xInfo.xhdr->dirty_rc_l < 0)
        xInfo.xhdr->dirty_rc_l = 0;
    if(xInfo.xhdr->dirty_rc_t < 0)
        xInfo.xhdr->dirty_rc_t = 0;
    if(xInfo.xhdr->dirty_rc_r > xInfo.xhdr->width)
        xInfo.xhdr->dirty_rc_r = xInfo.xhdr->width;
    if(xInfo.xhdr->dirty_rc_b > xInfo.xhdr->height)
        xInfo.xhdr->dirty_rc_b = xInfo.xhdr->height;

    width = xInfo.xhdr->dirty_rc_r - xInfo.xhdr->dirty_rc_l;
    height = xInfo.xhdr->dirty_rc_b - xInfo.xhdr->dirty_rc_t;
    length = bpp * width * height;

    fb_data = data + xInfo.xhdr->fb_offset 
        + xInfo.xhdr->pitch * xInfo.xhdr->dirty_rc_t 
        + bpp * xInfo.xhdr->dirty_rc_l;

    mem_data = (char*)calloc(1, length);
    memcpy(mem_data,fb_data,length);

    DrawImage(xInfo.xhdr->dirty_rc_l,xInfo.xhdr->dirty_rc_t,width,height,mem_data); 

    xInfo.xhdr->dirty = 0;
    unlock();
}

static void MarkDrawAll()
{
    lock();
    xInfo.xhdr->dirty = True;
    xInfo.xhdr->dirty_rc_l = xInfo.xhdr->dirty_rc_t = 0;
    xInfo.xhdr->dirty_rc_r = xInfo.xhdr->width;
    xInfo.xhdr->dirty_rc_b = xInfo.xhdr->height;
    unlock();
}

static void EventProc(Display* display, Window win, GC gc, XEvent report)
{
    Colormap colormap;
    switch (report.type) {
        //曝光事件, 窗口应重绘
        case Expose:
            MarkDrawAll();
            break;

        //窗口尺寸改变, 重新取得窗口的宽度和高度
        case ConfigureNotify:
            xInfo.xhdr->width = report.xconfigure.width;
            xInfo.xhdr->height = report.xconfigure.height;
            break;

        case ButtonPress:
        case ButtonRelease:
            {
                fprintf(stderr,"button release \n");
                int x = report.xbutton.x ;
                int y = report.xbutton.y ;
                SendMouseData(x, y,(int)report.xbutton.button);
            }
            break;
        case MotionNotify:
            {
                int x = report.xmotion.x;
                int y = report.xmotion.y;
                SendMouseData(x, y, (int)report.xbutton.button);
            }
            break;

        case KeyPress:
            {
                int keycode = report.xkey.keycode;
                int unicode = report.xkey.keycode;
                setPressedKey(keycode,unicode);
                if (isAlpha(keycode))
                {
                    sendKeyboardData(unicode, keycode, 
                            0,//e->state()&(ShiftButton|ControlButton|AltButton),
                            True, False);
                }
                else
                {
                    sendKeyboardData(unicode, keycode, 
                            0,//e->state()&(ShiftButton|ControlButton|AltButton),
                            True, False);
                    sendKeyboardData(unicode, keycode, 
                            0,//e->state()&(ShiftButton|ControlButton|AltButton),
                            False, False);
                }
            }
            break;
        case KeyRelease:
            {
                int keycode = report.xkey.keycode;
                int unicode = report.xkey.keycode;
                if(!clearPressedKey(keycode))
                {
                    return ;
                }
                if (isAlpha(keycode))
                {
                    sendKeyboardData(unicode, keycode, 
                            0,//e->state()&(ShiftButton|ControlButton|AltButton),
                            False, False);
                }
            }
            break;

        case DestroyNotify:
            {
                unsigned int type = XXVFB_CLOSE_TYPE;
                write(xInfo.sockfd, &type, sizeof(type));
            }

            break;
        default:
            break;
    }
}
int MainWndLoop(int ppid, int wnd_width, int wnd_height, int depth, char* caption)
{
    Display *display;
    int screen_num;

    Window win;
    XEvent report;

    GC gc;
    unsigned long valuemask = 0;
    XGCValues values;
    XSizeHints *sz; 
    XSetWindowAttributes attr; 
    Colormap colormap; 

    unsigned int width, height; //窗口尺寸
    unsigned int display_width, display_height;//屏幕尺寸
    unsigned int border_width = BORDER_WIDTH;

    int dev_depth;
    int shmId;
    int withalpha = 0;
    int nr_entry = 0;
    XXVFBHeader *hdr;
    int sockfd;
    
    displayid = ppid;
    if(depth <= 8) 
        nr_entry = 1 << depth;

    display = InitXDisplay(display,&screen_num,&display_width,&display_height); 
    dev_depth = XDefaultDepth(display,screen_num);

    //创建窗口
    attr.background_pixel = WhitePixel(display,screen_num);
    win = XCreateWindow(display, //display
            RootWindow(display,screen_num), //父窗口
            0, 0, wnd_width, wnd_height, //位置和大小
            border_width, //边界宽度
            dev_depth,
            InputOutput,
            DefaultVisual(display, DefaultScreen(display)),
            CWBackPixel, &attr);

    //设置窗口大小位置
    XStoreName(display, win, caption); 
    sz = XAllocSizeHints(); 
    sz->x = 0; 
    sz->y = 0; 
    sz->width = wnd_width; 
    sz->height = wnd_height; 
    sz->flags = USPosition | USSize; 
    XSetNormalHints(display, win, sz); 

    //建立GC
    gc = XCreateGC(display, win, valuemask, &values);

    //显示窗口
    XMapWindow(display, win);

    //调整colormap
    colormap = DefaultColormap(display, screen_num); 

    //选择窗口感兴趣的事件掩码
    XSelectInput(display, win,
            ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
            PointerMotionMask | StructureNotifyMask);

    //记录数据交互
    memset(&xInfo,sizeof(xInfo),0);

    xInfo.display = display;
    xInfo.win = win;
    xInfo.gc = gc;
    xInfo.visual = DefaultVisual(display, screen_num);
    xInfo.colormap = colormap;

    {
        key_t key = ppid;
        int bpl;
        int dataSize;

        if ( depth == 1 )
            bpl = (wnd_width*depth+7)/8;
        else
            bpl = ((wnd_width*depth+31)/32)*4;

        dataSize = bpl * wnd_height + sizeof(XXVFBHeader) + nr_entry * sizeof(XXVFBPalEntry);
        shmId = shmget( key, dataSize, IPC_CREAT|0666);
        if ( shmId != -1 )
            data = (unsigned char *)shmat( shmId, 0, 0 );
        else {
            struct shmid_ds shm;
            shmctl( shmId, IPC_RMID, &shm );
            shmId = shmget( key, dataSize, IPC_CREAT|0666);
            data = (unsigned char *)shmat( shmId, 0, 0 );
        }

        if ( (int)data == -1 )
        {
            fprintf(stderr, "Cannot attach to shared memory" );
            return;
        }
    
        hdr = (XXVFBHeader *)data;
        hdr->width           = wnd_width;
        hdr->height          = wnd_height;
        hdr->depth           = depth;
        hdr->pitch           = bpl;
        hdr->fb_offset       = sizeof(XXVFBHeader);
        hdr->palette_changed = 0;
        hdr->palette_offset  = sizeof(XXVFBHeader);
        hdr->fb_offset       = sizeof(XXVFBHeader) + nr_entry * sizeof(XXVFBPalEntry);
        hdr->dirty           = 0;
        hdr->dirty_rc_l      = 0;
        hdr->dirty_rc_t      = 0;
        hdr->dirty_rc_r      = 0;
        hdr->dirty_rc_b      = 0;
        hdr->MSBLeft         = 0;
        switch(depth)
        {
#if 0
            case 2:
                hdr->MSBLeft = 1;
                break;
            case 1:
            case 4:
            case 8:
                hdr->MSBLeft = 0;
                break;
#endif
            case 16:
                if (withalpha) {
                    hdr->Amask = 0x8000;
                    hdr->Rmask = 0x7C00;
                    hdr->Gmask = 0x03E0;
                    hdr->Bmask = 0x001F;
                }
                else {
                    hdr->Amask = 0x0000;
                    hdr->Rmask = 0xF800;
                    hdr->Gmask = 0x07E0;
                    hdr->Bmask = 0x001F;
                }
                break;
            case 24:
            case 32:
                if (withalpha) {
                    hdr->Amask = 0xFF000000;
                    hdr->Rmask = 0x00FF0000;
                    hdr->Gmask = 0x0000FF00;
                    hdr->Bmask = 0x000000FF;
                }
                else {
                    hdr->Amask = 0x00000000;
                    hdr->Rmask = 0x00FF0000;
                    hdr->Gmask = 0x0000FF00;
                    hdr->Bmask = 0x000000FF;
                }
                break;
            default:
                break;
        }
    }

    {
        int len;
        int result;
        char socket_file[50];
        struct sockaddr_un address;

        sprintf(socket_file, "/tmp/pcxvfb_socket%d", ppid);
        socket_file[49] = '\0';
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        address.sun_family = AF_UNIX;
        strcpy(address.sun_path, socket_file);
        len = sizeof(address);

        result = my_connect(sockfd, (struct sockaddr *)&address, len);
        if (result == -1) {
            perror("oops:client1 ");
            exit(1);
        }

        write(sockfd, &shmId, sizeof(int));
    }

    //把hdr地址添加到xInfo保存
    xInfo.xhdr = hdr;
    xInfo.dev_depth = dev_depth;
    xInfo.sockfd = sockfd;

    //初始化键盘映射
    init_code_map ();

    while(1)
    {
        if(XPending(display))
        {
            XNextEvent(display,&report);
            EventProc(display,win,gc,report);
        }
        else
        {
            DrawDirtyRect();
        }
    }
   
    XCloseDisplay(display);
}

