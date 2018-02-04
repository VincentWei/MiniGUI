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

#ifndef _GAL_s3c6410video_h
#define _GAL_s3c6410video_h

#include "sysvideo.h"
#include "videomem-bucket.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData { /* for 6410 only */
    int fd_g2d;

    int fd_fb;
    unsigned int smem_start; /* physics address */
    unsigned int smem_len;

    unsigned char *pixels;
    int width;
    int height;
    int pitch;

    gal_vmbucket_t vmbucket;
};

struct private_hwdata { /* for GAL_SURFACE */
    gal_vmblock_t *vmblock;
    unsigned int addr_phy;
    int fd_g2d;
};

extern int s3c6410_size_table[48];

/******************************************************************
 * copy from g2d driver
 */
typedef enum
{
	PAL1, PAL2, PAL4, PAL8,
	RGB8, ARGB8, RGB16, ARGB16, RGB18, RGB24, RGB30, ARGB24,RGBA16,RGBX24,RGBA24,
	YC420, YC422, // Non-interleave
	CRYCBY, CBYCRY, YCRYCB, YCBYCR, YUV444 // Interleave
} G2D_COLOR_SPACE;

typedef unsigned int u32;
typedef struct
{
	u32	src_base_addr;			//Base address of the source image
	u32	src_full_width;			//source image full width
	u32	src_full_height;		//source image full height
	u32	src_start_x;			//coordinate start x of source image
	u32	src_start_y;			//coordinate start y of source image
	u32	src_work_width;			//source image width for work
	u32 src_work_height;		//source image height for work
    u32 src_colormode;

	u32	dst_base_addr;			//Base address of the destination image	
	u32	dst_full_width;			//destination screen full width
	u32	dst_full_height;		//destination screen full width
	u32	dst_start_x;			//coordinate start x of destination screen
	u32	dst_start_y;			//coordinate start y of destination screen
	u32	dst_work_width;			//destination screen width for work
	u32 dst_work_height;		//destination screen height for work
    u32 dst_colormode;

	// Coordinate (X, Y) of clipping window
	u32 cw_x1, cw_y1;
	u32 cw_x2, cw_y2;

	u32 color_val[8];

	u32	alpha_mode;			//true : enable, false : disable
	u32	alpha_val;
	u32	color_key_mode;			//treu : enable, false : disable
	u32	color_key_val;			//transparent color value
} s3c_g2d_params;

typedef enum
{   
    G2D_BLACK = 0, G2D_RED = 1, G2D_GREEN = 2, G2D_BLUE = 3, G2D_WHITE = 4, 
    G2D_YELLOW = 5, G2D_CYAN = 6, G2D_MAGENTA = 7
} G2D_COLOR;

#define G2D_IOCTL_MAGIC 'G'

#define S3C_G2D_COLOR_RGB_565               (0x0<<0)
#define S3C_G2D_COLOR_RGBA_5551             (0x1<<0)
#define S3C_G2D_COLOR_ARGB_1555             (0x2<<0)
#define S3C_G2D_COLOR_RGBA_8888             (0x3<<0)
#define S3C_G2D_COLOR_ARGB_8888             (0x4<<0)
#define S3C_G2D_COLOR_XRGB_8888             (0x5<<0)
#define S3C_G2D_COLOR_RGBX_8888             (0x6<<0)

/* Alpha Blending Mode */
#define S3C_G2D_ROP_REG_ABM_NO_BLENDING         (0<<10)
#define S3C_G2D_ROP_REG_ABM_SRC_BITMAP          (1<<10)
#define S3C_G2D_ROP_REG_ABM_REGISTER            (2<<10)
#define S3C_G2D_ROP_REG_ABM_FADING          (4<<10)

#define S3C_G2D_ROTATOR_0			_IO(G2D_IOCTL_MAGIC,0)
#define S3C_G2D_ROTATOR_90			_IO(G2D_IOCTL_MAGIC,1)
#define S3C_G2D_ROTATOR_180			_IO(G2D_IOCTL_MAGIC,2)
#define S3C_G2D_ROTATOR_270			_IO(G2D_IOCTL_MAGIC,3)
#define S3C_G2D_ROTATOR_X_FLIP			_IO(G2D_IOCTL_MAGIC,4)
#define S3C_G2D_ROTATOR_Y_FLIP			_IO(G2D_IOCTL_MAGIC,5)

/* V1 */
#define S3CFB_OSD_START         _IO  ('F', 201)
#define S3CFB_OSD_STOP          _IO  ('F', 202)
#define S3CFB_OSD_ALPHA_UP      _IO  ('F', 203)
#define S3CFB_OSD_ALPHA_DOWN        _IO  ('F', 204)
#define S3CFB_OSD_MOVE_LEFT     _IO  ('F', 205)
#define S3CFB_OSD_MOVE_RIGHT        _IO  ('F', 206)
#define S3CFB_OSD_MOVE_UP       _IO  ('F', 207)
#define S3CFB_OSD_MOVE_DOWN     _IO  ('F', 208)
#define S3CFB_OSD_SET_INFO      _IOW ('F', 209, s3cfb_win_info_t)
#define S3CFB_OSD_ALPHA0_SET        _IOW ('F', 210, unsigned int)
#define S3CFB_OSD_ALPHA1_SET        _IOW ('F', 211, unsigned int)
#define S3CFB_OSD_ALPHA_MODE        _IOW ('F', 212, unsigned int)

#define S3CFB_COLOR_KEY_START       _IO  ('F', 300)
#define S3CFB_COLOR_KEY_STOP        _IO  ('F', 301)
#define S3CFB_COLOR_KEY_ALPHA_START _IO  ('F', 302)
#define S3CFB_COLOR_KEY_ALPHA_STOP  _IO  ('F', 303)
#define S3CFB_COLOR_KEY_SET_INFO    _IOW ('F', 304, s3cfb_color_key_info_t)
#define S3CFB_COLOR_KEY_VALUE       _IOW ('F', 305, s3cfb_color_val_info_t)

#define S3CFB_ALPHA_MODE_PLANE      0
#define S3CFB_ALPHA_MODE_PIXEL      1

/* V2
struct s3cfb_user_window {
    int x;
    int y;
};

struct s3cfb_user_plane_alpha {
    int         channel;
    unsigned char   red;
    unsigned char   green;
    unsigned char   blue;
};

struct s3cfb_user_chroma {
    int         enabled;
    unsigned char   red;
    unsigned char   green;
    unsigned char   blue;
};

#define FBIO_WAITFORVSYNC       _IO  ('F', 32)
#define S3CFB_WIN_ON            _IO  ('F', 200)
#define S3CFB_WIN_OFF           _IO  ('F', 201)
#define S3CFB_WIN_OFF_ALL       _IO  ('F', 202)
#define S3CFB_WIN_POSITION      _IOW ('F', 203, struct s3cfb_user_window)
#define S3CFB_WIN_SET_PLANE_ALPHA   _IOW ('F', 204, struct s3cfb_user_plane_alpha)
#define S3CFB_WIN_SET_CHROMA        _IOW ('F', 205, struct s3cfb_user_chroma)
#define S3CFB_SET_VSYNC_INT     _IOW ('F', 206, unsigned int)
#define S3CFB_SET_SUSPEND_FIFO      _IOW ('F', 300, unsigned long)
#define S3CFB_SET_RESUME_FIFO       _IOW ('F', 301, unsigned long)
*/

#endif /* _GAL_s3c6410video_h */
