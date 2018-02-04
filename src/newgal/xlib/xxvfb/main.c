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
#include "xxvfb.h"
static void usage( char *app )
{
    printf( "Usage: %s [-width width] [-height height] [-depth depth] "
            "Supported depths: 1, 4, 8, 32\n", app );
}

int main(int argc, char *argv[])
{
    int width = 240;
    int height = 320;
    int depth = 24;
    int ppid;
    char caption[256];
    char *skin;

    if(argc >= 2)
    {
        ppid = atoi (argv[1]);
    }

    if(argc >= 3)
    {
        char* str = argv[2];
        memcpy(caption,str,strlen(str)+1);
    }
    if(argc >= 4)
    {
        width = atoi (argv[3]);
        height = atoi (strchr (argv[3], 'x') + 1);
        depth = atoi (strrchr (argv[3], '-') + 1);
    }
    if(argc >= 5) //skin
    {
        skin=argv[4];
    }

    fprintf(stderr,"ppid:%d,width:%d,height:%d,depth:%d,caption:%s\n",ppid,width,height,depth,caption);
    MainWndLoop(ppid,width,height,depth,caption);
}
