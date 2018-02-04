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
** type1.c: Type1 font support based on t1lib.
** 
** Create date: 2000/08/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"
#include "devfont.h"
#include "charset.h"
#include "fontname.h"

#ifdef _MGFONT_T1F

#include <t1lib.h>
#include "type1.h"

#define ADJUST_SIZE 1.1;

/************************ Init/Term of FreeType fonts ************************/
static const void* get_char_bitmap (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len, unsigned short* scale);
static const void* get_char_pixmap (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len, 
                int* pitch, unsigned short* scale);
static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont);
static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont);

static encstruct default_enc;

#define SECTION_NAME    "type1fonts"
#define T1LIBENCFILE    "IsoLatin1.enc"

/*
 * I always use full path file name ,so it maybe of no use...:-)
 * Only for the sake of complety,I set search path to this .
 * */
#define T1FONTPATH      "/usr/local/lib/minigui/fonts"

#define CHARSPACEUNIT2PIXEL(x)  (x/1000)

static void* load_font_data (const char* font_name, const char* file_name)
{
    int j;
	int font_id;
    TYPE1INFO * type1_info;

    T1_AddFont(file_name);
    font_id = T1_AddFont(file_name);
    if (font_id <= 0) 
        return NULL;

    type1_info = (TYPE1INFO*) calloc (1, sizeof(TYPE1INFO));

    j = strstr(file_name,"pfb") - file_name;
    strncpy(file_name+j,"afm",3);	

    if (T1_SetAfmFileName(font_id,file_name) < 0)
        goto error;

    strncpy(file_name+j,"pfb",3);	

    type1_info = (TYPE1INFO*) malloc (sizeof(TYPE1INFO));
    type1_info->font_id = font_id;
    return type1_info;

error:
    free (type1_info);
    return NULL;
}

static void unload_font_data (void* data)
{
    free (data);
}

/*
 * Because the uplayer did not provide the option of whether
 * to use anti-aliaing.We use it in default.
 * On day it supports the option,the code may be easily updated.
 * */

BOOL InitType1Fonts (void)
{
	int loglevel = 0;

	loglevel |= NO_LOGFILE;
	loglevel |= IGNORE_CONFIGFILE;
	loglevel |= IGNORE_FONTDATABASE;

	T1_InitLib(loglevel);

	T1_SetBitmapPad(8);
	T1_AASetBitsPerPixel(8);
	T1_AASetGrayValues(0,1,2,3,4);

	/* 
	 * Set some default value  base on my own idea. 
	 * So u can change it if u have enough reason.
	 * */
#if 1  
	//T1_SetDeviceResolutions(72,72);
	default_enc.encoding = T1_LoadEncoding (T1LIBENCFILE);
	default_enc.encfilename = (char *) malloc (strlen (T1LIBENCFILE) + 1);
	strcpy (default_enc.encfilename, T1LIBENCFILE);
    	T1_SetDefaultEncoding (default_enc.encoding);
	T1_AASetLevel (T1_AA_LOW);
#endif

	/*
	 * I always use full path file name ,so it maybe of no use...:-)
	 * Only for the sake of complety,I set search path to this .
	 **/
	T1_SetFileSearchPath (T1_PFAB_PATH | T1_AFM_PATH | T1_ENC_PATH ,T1FONTPATH);
	return TRUE;
}

void TermType1Fonts (void)
{
	T1_CloseLib();
}

/*
 * The width of a character is the amount of horizontal 
 * escapement that the next character is shifted 
 * to the right with respect to the current position
 * */
static int get_char_width (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	TYPE1INFO* type1_info = type1_inst_info->type1_info;

	int width;
	width = T1_GetCharWidth ( type1_info->font_id, *mchar );
	width *= type1_inst_info->csUnit2Pixel;
	return width;
}

/*
 * The width of a character is the amount of horizontal 
 * escapement that the next character is shifted 
 * to the right with respect to the current position
 * */
static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	return type1_inst_info->max_width;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	return type1_inst_info->ave_width;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	return type1_inst_info->font_height;
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
	return expect;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	return type1_inst_info->font_ascent;
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	return  -type1_inst_info->font_descent;
}

/*
 * FIXME: what the meaning of this functin when rotation is taken 
 * into account?
 * This implementation conflicts with that of freetype now.
 * */
static size_t char_bitmap_size (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len)
{
	int pixel_width,pixel_height;
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);

	get_char_bitmap (logfont, devfont, mchar, len, NULL);
	pixel_width = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing;
	pixel_height = type1_inst_info->last_ascent - type1_inst_info->last_descent;

	return pixel_width * pixel_height ;
}

static size_t max_bitmap_size (LOGFONT* logfont, DEVFONT* devfont)
{
	return get_max_width(logfont,devfont) * get_font_height(logfont,devfont);
}

/* 
 * NULL function
 * */
static void start_str_output (LOGFONT* logfont, DEVFONT* devfont)
{
	return;
}

/* 
 * call this function before getting the bitmap/pixmap of the char
 * to get the bbox of the char 
 * */

/*
 * FIXME: Because the limits of the interface(either ours of theirs
 * we can get the bounding box when rotation is in regard!
 * In detail: T1lib provide functinos to draw a string,
 * while we choose to draw it one by one!!
 * */
static int get_char_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len, 
                int* px, int* py, int* pwidth, int* pheight)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
    
	get_char_bitmap(logfont, devfont, mchar, len, NULL);
	
	if(px){ 
		*px += type1_inst_info->last_leftSideBearing;
	}
	if(py){
		*py -= type1_inst_info->last_ascent;
	}
  	if(pwidth) {
		*pwidth = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing;
	}
	if(pheight){
		*pheight = type1_inst_info->last_ascent - type1_inst_info->last_descent;
	}
	if(pwidth)
		return *pwidth;
	else 
		return -1;
}

static const void* get_char_bitmap (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len, unsigned short* scale)
{
	GLYPH * glyph;
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	TYPE1INFO* type1_info = type1_inst_info->type1_info;
	unsigned char c = *mchar;

    if (scale) *scale = 1;

	if(type1_inst_info->last_bitmap_char == *mchar) {
		return type1_inst_info->last_bits;	
	}

	glyph = T1_SetChar (type1_info->font_id, c, type1_inst_info->size, type1_inst_info->pmatrix);

	type1_inst_info->last_bitmap_char = *mchar;
	type1_inst_info->last_pixmap_char = -1;
	if(type1_inst_info->last_bitmap_str){
		free (type1_inst_info->last_bitmap_str);
		type1_inst_info->last_bitmap_str = NULL;
	}
	if(type1_inst_info->last_pixmap_str){
		free (type1_inst_info->last_pixmap_str);
		type1_inst_info->last_pixmap_str = NULL;
	}

    	type1_inst_info->last_ascent = glyph->metrics.ascent;
    	type1_inst_info->last_descent = glyph->metrics.descent;
    	type1_inst_info->last_leftSideBearing = glyph->metrics.leftSideBearing;
    	type1_inst_info->last_rightSideBearing = glyph->metrics.rightSideBearing;
    	type1_inst_info->last_advanceX = glyph->metrics.advanceX;
    	type1_inst_info->last_advanceY = glyph->metrics.advanceY;
    	type1_inst_info->last_bpp = glyph->bpp;

	/*free the last char's bitmap*/
	if(!type1_inst_info->last_bits)
		free(type1_inst_info->last_bits);
    	type1_inst_info->last_bits = glyph->bits;
	/*change the endian*/
	{
		int height,width;
		int i,j,k;
		unsigned char c;
		unsigned char d;
		height = type1_inst_info->last_ascent - type1_inst_info->last_descent ;
		width  = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing ;   
			
		width  = ( width + 7 ) >> 3 << 3 ;
		
		//fprintf(stderr, "height:%d, width %d\n", height, width);
		
		for (i = 0; i < height; i++) {
			for (j = 0; j < width/8 ; j++) {
				c = type1_inst_info->last_bits[ i * width / 8 + j];
				if( c ) {
					d = 0;	
					for ( k = 0; k < 8; k++ ) {
						if ( ( c >> k ) & 0x01 )
							d |= 0x80 >> k ;
					}
					type1_inst_info->last_bits[ i * width / 8 + j] = d ;
				}
#if 0  
				c = type1_inst_info->last_bits[ i * width / 8 + j];
                		for (k = 0; k < 8; k++)
              			      if (c  & (0x80>>k))
                   			     fprintf (stderr, "*");
                		      else
                     			     fprintf (stderr, " ");

#endif 
			}
#if 0
            		fprintf (stderr, "\n");
#endif
		}
	}
	/*
	 * if not set it to null, T1_SetChar will free it next time it is called,
	 * we do so to keep it by ourself
	 * */ 
	glyph->bits = NULL;

	return type1_inst_info->last_bits;	

}
static const void* get_char_pixmap (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len, 
            int* pitch, unsigned short* scale)
{
	GLYPH * glyph;
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	TYPE1INFO* type1_info = type1_inst_info->type1_info;

    if (scale) *scale = 1;

	if(type1_inst_info->last_pixmap_char == *mchar){
	/* same char as last call, use the cache one.*/
		if ( pitch )
	 		*pitch = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing;   
		return type1_inst_info->last_bits;	
	}
	
	glyph = T1_AASetChar ( type1_info->font_id , *mchar , type1_inst_info->size , type1_inst_info->pmatrix) ;

	type1_inst_info->last_pixmap_char = *mchar;
	type1_inst_info->last_bitmap_char = -1;
	if(type1_inst_info->last_bitmap_str){
		free (type1_inst_info->last_bitmap_str);
		type1_inst_info->last_bitmap_str = NULL;
	}
	if(type1_inst_info->last_pixmap_str){
		free (type1_inst_info->last_pixmap_str);
		type1_inst_info->last_pixmap_str = NULL;
	}
    	type1_inst_info->last_ascent = glyph->metrics.ascent;
    	type1_inst_info->last_descent = glyph->metrics.descent;
    	type1_inst_info->last_leftSideBearing = glyph->metrics.leftSideBearing;
    	type1_inst_info->last_rightSideBearing = glyph->metrics.rightSideBearing;
    	type1_inst_info->last_advanceX = glyph->metrics.advanceX;
    	type1_inst_info->last_advanceY = glyph->metrics.advanceY;
    	type1_inst_info->last_bpp = glyph->bpp;
	/*free the last char's bitmap*/
	if(!type1_inst_info->last_bits)
		free(type1_inst_info->last_bits);
    	type1_inst_info->last_bits = glyph->bits;

	if ( pitch )
	 	*pitch = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing;   

#if 0
	{
		int height,width;
		int i,j,k;
		unsigned char c;
		unsigned char d;
		height = type1_inst_info->last_ascent - type1_inst_info->last_descent ;
		width  = type1_inst_info->last_rightSideBearing - type1_inst_info->last_leftSideBearing ;   
			
		
		for (i = 0; i < height; i++) {
			for (j = 0; j < width ; j++) {
				c = type1_inst_info->last_bits[ i * width + j];
				if( c ) 
					fprintf(stderr, "%d",c);
			 	else 
					fprintf(stderr, " ");
				
			}
            		fprintf (stderr, "\n");
		}
	}
#endif
	/*
	 * if not set it to null, T1_AASetChar will free it next time it is called,
	 * we do so to keep it by ourself
	 * */ 
	glyph->bits = NULL;

	return type1_inst_info->last_bits;	
}

/* 
 * call this function after getting the bitmap/pixmap of the char 
 * to get the advance of the char 
 * */
static void get_char_advance (LOGFONT* logfont, DEVFONT* devfont,
                const unsigned char* mchar, int len, 
                int* px, int* py)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	*px += type1_inst_info->last_advanceX;
	*py -= type1_inst_info->last_advanceY;

}

static DEVFONT* new_instance (LOGFONT* logfont, DEVFONT* devfont, BOOL need_sbc_font)
{
	TYPE1INFO* type1_info = TYPE1_INFO_P (devfont);
	TYPE1INSTANCEINFO* type1_inst_info = NULL;

	DEVFONT* new_devfont = NULL;
	
	unsigned char c ;
	BBox bbox;
	int i;
	int width=0;
	int sum=0;
	int count=0;
	int max_width=0;

	if ((new_devfont = (DEVFONT *)calloc (1, sizeof (DEVFONT))) == NULL)
	        goto out;

	if ((type1_inst_info = (TYPE1INSTANCEINFO *)calloc (1, sizeof (TYPE1INSTANCEINFO))) == NULL)
		goto out;

	memcpy (new_devfont, devfont, sizeof (DEVFONT));

	new_devfont->data = type1_inst_info;
	type1_inst_info->type1_info = type1_info;

	type1_inst_info->size     = logfont->size;

	if(logfont->rotation)
	{
		type1_inst_info->rotation = logfont->rotation; /* in tenthdegrees */
		type1_inst_info->pmatrix = T1_RotateMatrix( NULL ,     type1_inst_info->rotation / 10.0 );	
	}

	type1_inst_info->last_bitmap_char = -1;
	type1_inst_info->last_pixmap_char = -1;
	type1_inst_info->last_bitmap_str  = NULL;	
	type1_inst_info->last_pixmap_str  = NULL;	
	c = 'A';
	get_char_bitmap (logfont,new_devfont, &c, 1, NULL);
	bbox = T1_GetCharBBox(type1_info->font_id,'A');
	type1_inst_info->csUnit2Pixel = (double) ( type1_inst_info->last_rightSideBearing - 
		type1_inst_info->last_leftSideBearing ) / ( double ) ( bbox.urx - bbox.llx )  ;

	bbox = T1_GetFontBBox(type1_info->font_id);
	type1_inst_info->font_ascent = bbox.ury * type1_inst_info->csUnit2Pixel ;
	type1_inst_info->font_descent = bbox.lly * type1_inst_info->csUnit2Pixel ; 
	type1_inst_info->font_height = type1_inst_info->font_ascent - type1_inst_info->font_descent ;
	
	for( i = 0 ; i<=255; i++)
	{
		width = T1_GetCharWidth ( type1_info->font_id, i );
		if ( width > 0 ) {
			count ++;
			sum += width;
			if ( width > max_width)
				max_width = width;
		}	
	}
	
	type1_inst_info->max_width = max_width * type1_inst_info->csUnit2Pixel;
	type1_inst_info->ave_width = ( sum / count + 1 ) * type1_inst_info->csUnit2Pixel ;

	return new_devfont;
	out:
		free (type1_inst_info);
		free (new_devfont);
		return NULL;
}

static void delete_instance (DEVFONT* devfont)
{
	TYPE1INSTANCEINFO* type1_inst_info = TYPE1_INST_INFO_P (devfont);
	TYPE1INFO* type1_info = type1_inst_info->type1_info;

	T1_DeleteSize(type1_info->font_id,type1_inst_info->size);
	if(type1_inst_info->pmatrix)
		free(type1_inst_info->pmatrix);
	if(type1_inst_info->last_bits)
		free(type1_inst_info->last_bits);
	if(type1_inst_info->last_bitmap_str)
		free(type1_inst_info->last_bitmap_str);
	if(type1_inst_info->last_pixmap_str)
		free(type1_inst_info->last_pixmap_str);
	free (type1_inst_info);
	free (devfont);
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return rot_desired;
}

/**************************** Global data ************************************/
FONTOPS t1f_ops = {
	get_char_width,
	get_ave_width,
	get_max_width,
	get_font_height,
	get_font_size,
	get_font_ascent,
	get_font_descent,
	get_char_bitmap,
	get_char_pixmap,
	start_str_output,
	get_char_bbox,
	get_char_advance,
	new_instance,
	delete_instance,
    is_rotatable,
    load_font_data,
    unload_font_data
};


/* General transformation */
/*  
       		( y11   y21 )   
 (x1,x2)    *   (           ) = (x1*y11 +x2 *y12, x1 *y21+x2*y22;
   		( y12   y22 )   
*/
/*
static void my_TransformMatrix( T1_TMATRIX *matrix, double * x1, double  * x2 )
{
	double y1 ;
	double y2 ;
	if (matrix==NULL) return( NULL);
   
	y1 = ( *x1 ) * matrix->cxx + ( *x2 ) * matrix->cxy;
	y2 = ( *x1 ) * matrix->cyx + ( *x2 ) * matrix->cyy; 
	*x1 = y1;
	*x2 = y2;

	return;
}
*/

#endif /* _TYPE1_SUPPORT */

