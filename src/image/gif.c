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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "gif.h"

const int __mg_v_incr[4]={8,8,4,2};
const int __mg_v_start[4]={0,4,2,1};

int unpack_gif(gif_info_t* ginfo)
{
    int bpp;
    int palette_size;

    assert(ginfo->gif_surface==NULL);
	assert(ginfo->gif_buffer!=NULL);

    ginfo->gif_cur_buffer=ginfo->gif_buffer;
    
    memcpy(&(ginfo->gif_global),
             ginfo->gif_cur_buffer,
             sizeof(gif_global_t));

    ginfo->gif_x=0;
    ginfo->gif_y=0;
    ginfo->gif_surface=malloc(ginfo->gif_global.width*ginfo->gif_global.height*3);

    ginfo->gif_cur_buffer+=GIF_GLOBAL_SIZE;

    bpp=(ginfo->gif_global.flag&0x07)+1;
    palette_size=(1<<bpp)*3;

    memcpy(&(ginfo->gif_global_palette),
             ginfo->gif_cur_buffer,
             palette_size);

    if (strncmp(ginfo->gif_global.magic_word,"GIF",3)!=0) {
        return -1;
    }

    ginfo->gif_cur_buffer+=palette_size;

    while(1) {
        char block_type=*(ginfo->gif_cur_buffer);
        ginfo->gif_cur_buffer++;
        switch(block_type) {
        case ',': 
            // unpack local image block
            return unpack_local_image(ginfo);
            break;
        case '!':
            {
            // char sub_block_type=*(ginfo->gif_cur_buffer++);
			unsigned char sub_block_size=*(ginfo->gif_cur_buffer++);

            ginfo->gif_cur_buffer+=sub_block_size; // skip subblock
            ginfo->gif_cur_buffer++;  // skip '0x00'
            }
            break;
        default:
            // unknown block , return false .
            return -1; 
        }
    }
    return 0;
}

int unpack_local_image(gif_info_t* ginfo)
{
    memcpy(&(ginfo->gif_local),
             ginfo->gif_cur_buffer,
             sizeof(gif_local_t));

    ginfo->gif_cur_buffer+=GIF_LOCAL_SIZE;

    ginfo->b_interlaced=ginfo->gif_local.flag&0x40;
    ginfo->b_local_palette=ginfo->gif_local.flag&0x08;

    if (ginfo->b_local_palette!=0) {
        // load local palette ...
        int bpp;
        int palette_size;

        bpp=(ginfo->gif_local.flag&0x07)+1;
        palette_size=(1<<bpp)*3;

        memcpy(&(ginfo->gif_local_palette),
                 ginfo->gif_cur_buffer,
                 palette_size);
        ginfo->gif_cur_buffer+=palette_size;
    }
    return unpack_image_data(ginfo);
}

int unpack_image_data(gif_info_t* ginfo)
{
    ginfo->first_ch=*(ginfo->gif_cur_buffer);
    
    assert(ginfo->first_ch==1||
           ginfo->first_ch==2||
           ginfo->first_ch==4||
           ginfo->first_ch==8);
    
    ginfo->gif_cur_buffer++;

    ginfo->gif_x=0;
    ginfo->gif_y=0;

    ginfo->CLEAR=1<<ginfo->first_ch;
    ginfo->EOI=ginfo->CLEAR+1;

    ginfo->request_bits=ginfo->first_ch+1;

    while((ginfo->code=get_code(ginfo))!=ginfo->EOI) {
        if (ginfo->code==ginfo->CLEAR) {
            clear_code_table(ginfo,ginfo->CLEAR);
        }
        else  {
            if (ginfo->code_table[ginfo->code].prefix==-2) {
                insert_code_table(ginfo,ginfo->prefix,ginfo->prefix);
            }
            else {
                if (ginfo->prefix!=-1) {
                    insert_code_table(ginfo,ginfo->code,ginfo->prefix);
                }
            }
            // output code ...
            output_code(ginfo,ginfo->code); 
            ginfo->prefix=ginfo->code;
        }
    }

    return 0;
}

void clear_code_table(gif_info_t* ginfo,int clear)
{
    int i;

    ginfo->free_code=clear+2;
    ginfo->prefix=-1;

    for(i=0;i<clear;i++) {
        ginfo->code_table[i].first=i;   // first char 
        ginfo->code_table[i].last=i;    // last char 
        ginfo->code_table[i].prefix=-1; // prefix index 
    }
    for(i=clear;i<4096;i++) {
        ginfo->code_table[i].prefix=-2;
    }
    ginfo->request_bits=ginfo->first_ch+1;
    ginfo->code_limit=1<<ginfo->request_bits;
}

void insert_code_table(gif_info_t* ginfo,int code,int prefix)
{
    ginfo->code_table[ginfo->free_code].prefix=ginfo->prefix;
    ginfo->code_table[ginfo->free_code].last=ginfo->code_table[code].first;
    ginfo->code_table[ginfo->free_code].first=ginfo->code_table[prefix].first;

    ginfo->free_code++;
    if (ginfo->free_code==ginfo->code_limit) {
        if (ginfo->request_bits<12) {
            ginfo->request_bits++;
            ginfo->code_limit=1<<ginfo->request_bits;
        }
    }
}

void output_code(gif_info_t* ginfo,int code)
{
    int i=0;
    while(code!=-1) {
        ginfo->code_string[i++]=ginfo->code_table[code].last;
        code=ginfo->code_table[code].prefix;
    }
    for(i--;i>=0;i--) {
        output_pixel(ginfo,ginfo->code_string[i]);
    }
}

void output_pixel(gif_info_t* ginfo,unsigned char pixel) 
{
    unsigned char r,g,b;

    if (ginfo->b_local_palette!=0) {
        r=ginfo->gif_local_palette.palette[pixel*3];
        g=ginfo->gif_local_palette.palette[pixel*3+1];
        b=ginfo->gif_local_palette.palette[pixel*3+2];
    }
    else {
        r=ginfo->gif_global_palette.palette[pixel*3];
        g=ginfo->gif_global_palette.palette[pixel*3+1];
        b=ginfo->gif_global_palette.palette[pixel*3+2];
    }

    draw_pixel(ginfo,r,g,b);

    ginfo->gif_x++;
    if (ginfo->gif_x>=ginfo->gif_local.width) {
        ginfo->gif_x=0;
        if (ginfo->b_interlaced==0) {
            ginfo->gif_y++;
        }
        else {
            ginfo->gif_y+=__mg_v_incr[ginfo->pass];
            if (ginfo->gif_y>=ginfo->gif_local.height) {
                ginfo->pass++;
                ginfo->gif_y=__mg_v_start[ginfo->pass];
            }
        }
    }
}

/*
 * get request_bits from buffer
 */
int get_code(gif_info_t* ginfo)
{
    int request_bits=ginfo->request_bits;
    int  value=0;
    int  mask;
    int  position=0,use_bits=0;

    while(request_bits>0) {
        if (ginfo->remain_bits==0) {
            if (ginfo->remain_bytes==0) {
                ginfo->remain_bytes=*ginfo->gif_cur_buffer++;
            }
            ginfo->remain_bytes--;
            ginfo->remain_bits=8;
            ginfo->data=*ginfo->gif_cur_buffer++;
        }

        if (request_bits>=ginfo->remain_bits) {
            use_bits=ginfo->remain_bits;
        }
        else {
            use_bits=request_bits;
        }
        ginfo->remain_bits-=use_bits;
        mask=0xff>>(8-use_bits);
        mask&=ginfo->data;
        // store remain useful bits to low bits of data
        ginfo->data>>=use_bits;
        mask<<=position;
        // new position that bits should be placed next time 
        position+=use_bits;
        // insert these bits to the value
        value|=mask;
        request_bits-=use_bits;
    }
    return value;
}

void draw_pixel(gif_info_t* ginfo,int r,int g,int b)
{
    int x,y;
    int offset;
    unsigned char* mem;

    x=ginfo->gif_x;
    y=ginfo->gif_y;

    offset=ginfo->gif_local.width*y+x;

    mem=(unsigned char*)(ginfo->gif_surface);
    mem+=offset*3;

    if (r>=0) {
        *mem=(char)r;
    }
    mem++;
    if (g>=0) {
        *mem=(char)g;
    }
    mem++;
    if (b>=0) {
        *mem=(char)b;
    }
    mem++;
}

void destroy_gif(gif_info_t* ginfo) 
{
    if (ginfo->gif_surface!=NULL) {
        free(ginfo->gif_surface);
    }
}

