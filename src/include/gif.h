// $Id: gif.h 241 2000-01-08 09:10:27Z weiym $
//

#ifndef _GIF_H_
#define _GIF_H_

#define GIF_GLOBAL_SIZE 13
#define GIF_PALETTE_SIZE 768
#define GIF_LOCAL_SIZE   9

typedef struct tagGlobal {
    char magic_word[3];
    char version[3];
    short width;
    short height;
    unsigned char flag;
    unsigned char bg_color;
    unsigned char aspect;
}gif_global_t;

typedef struct tagPalette {
    unsigned char palette[768];
}gif_palette_t;

typedef struct tagLocal {
    short x;
    short y;
    short width;
    short height;
    unsigned char flag;
}gif_local_t;

typedef struct tagCode {
    char first;
    char last;
    int  prefix;
}gif_code_t;

typedef struct tagGifInfo {
	gif_global_t  gif_global;
	gif_local_t   gif_local;
	gif_palette_t gif_global_palette;
	gif_palette_t gif_local_palette;

	int gif_buffer_length;
	unsigned char* gif_buffer;
	unsigned char* gif_cur_buffer;

	gif_code_t code_table[4096];
	unsigned char code_string[1024];

	int code_limit;

	int b_local_palette;
	int b_interlaced;

	int request_bits;
	int first_ch;

	int pass;

	int code;
	int prefix;

	int free_code;
	int EOI,CLEAR;

    int remain_bytes;
    int remain_bits;
    char unsigned data;

	unsigned char* gif_surface;
	int gif_x;
	int gif_y;
}gif_info_t;

extern int unpack_gif(gif_info_t*);
extern int unpack_local_image(gif_info_t*);
extern int unpack_image_data(gif_info_t*);

extern void clear_code_table(gif_info_t*,int);
extern void insert_code_table(gif_info_t*,int,int);
extern void output_code(gif_info_t*,int);
extern void output_pixel(gif_info_t*,unsigned char); 
extern int  get_code(gif_info_t*);
extern void draw_pixel(gif_info_t*,int,int,int);
extern void destroy_gif(gif_info_t*);
 
#endif

