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
** jpg.c: Low-level JPEG file read/save routines.
** 
** Current maintainer: Wei Yongming
**
** Create date: 2000/08/29
*/

/*
 * JPEG decompression routine
 *
 * JPEG support must be enabled (see README.txt in contrib/jpeg)
 *
 * SOME FINE POINTS: (from libjpeg)
 * In the below code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.doc for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.doc for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

#ifdef _MGIMAGE_JPG

#include <jpeglib.h>
#include <jerror.h>

#define SIZEOF(object)       ((size_t) sizeof(object))

/* Expanded data source object for stdio input */
typedef struct {
  struct jpeg_source_mgr pub;        /* public fields */

  MG_RWops *infile;                /* source stream */
  JOCTET * buffer;                /* start of buffer */
  boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

struct my_error_mgr {
    struct jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

typedef struct _jpeg_init_info_t {
    struct jpeg_decompress_struct *jpeg_info;
    struct my_error_mgr *jpeg_err;
    int dec_started;
} jpeg_init_info_t;

#define INPUT_BUF_SIZE  4096        /* choose an efficiently fread'able size */

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

static void init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 */

static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

  nbytes = MGUI_RWread ((MG_RWops*) src->infile, src->buffer, 1, INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)        /* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 */

static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

static void term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}

/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

static void my_jpeg_data_src (j_decompress_ptr cinfo, MG_RWops* infile)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {        /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  SIZEOF(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

static void my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp (myerr->setjmp_buffer, 1);
}

/*
 * Common JPEG markers
 * see also https://en.wikipedia.org/wiki/JPEG
 * */
#define JMK_SOI         0xFFD8     /* Start Of Image */
#define JMK_SOF0        0xFFC0     /* Start Of Frame (baseline DCT) */
#define JMK_SOF2        0xFFC2     /* Start Of Frame (progressive DCT) */
#define JMK_DHT         0xFFC4     /* Define Huffman Table(s) */
#define JMK_DQT         0xFFDB     /* Define Quantization Table(s) */
#define JMK_DRI         0xFFDD     /* Define Restart Interval */
#define JMK_SOS         0xFFDA     /* Start Of Scan */
#define JMK_RST_mask    0xFFD0     /* mask of Restart */
#define JMK_APP_mask    0xFFE0     /* mask of Application-specific */
#define JMK_COM         0xFFFE     /* Comment */
#define JMK_EOI         0xFFD9     /* End Of Image */
/**
 * Reads a 16-bit big endian integer from a MG_RWops object.
 * return TRUE if sucess,or FALSE if the read failed.
 */
static BOOL read_be16 (MG_RWops *fp,Uint16 *value)
{
    if(-1 == MGUI_RWread(fp, value, (sizeof *value), 1))
        return FALSE;
    *value = (ArchSwapBE16(*value));
    return TRUE;
}

void* __mg_init_jpg (MG_RWops *fp, MYBITMAP* mybmp, RGB* pal)
{
    int i;
    Uint16 soi_marker;

    /* This struct contains the JPEG decompression parameters
     * and pointers to working space 
     * (which is allocated as needed by the JPEG library).
     */
    struct jpeg_decompress_struct *cinfo;
    struct my_error_mgr *jerr;
    jpeg_init_info_t* init_info;

    if(!read_be16(fp,&soi_marker) || JMK_SOI != soi_marker )
    	goto err; /* not JPEG image*/

    MGUI_RWseek (fp, 0, SEEK_SET);

    /* Step 1: allocate and initialize JPEG decompression object */
    cinfo = calloc (1, sizeof(struct jpeg_decompress_struct));
    jerr = calloc (1, sizeof(struct my_error_mgr));
    init_info = calloc (1, sizeof(jpeg_init_info_t));

    if (cinfo == NULL || jerr == NULL || init_info == NULL)
        goto err;

    /* We set up the normal JPEG error routines first. */
    cinfo->err = jpeg_std_error (&jerr->pub);
    jerr->pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp (jerr->setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, free the objects and return.
         */
        jpeg_destroy_decompress (cinfo);

        free (init_info);
        free (jerr);
        free (cinfo);
        goto err;
    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress (cinfo);
    init_info->dec_started = 0;

    /* Step 2: specify data source */
    my_jpeg_data_src (cinfo, fp);

    /* Step 3: read file parameters with jpeg_read_header() */
    if(JPEG_HEADER_OK != jpeg_read_header (cinfo, TRUE))
    {
    	longjmp (jerr->setjmp_buffer, 1);
    }

    /* Step 4: set parameters for decompression */
    cinfo->out_color_space = (mybmp->flags & MYBMP_LOAD_GRAYSCALE) ? JCS_GRAYSCALE: JCS_RGB;
    cinfo->quantize_colors = FALSE;

    if (!(mybmp->flags & MYBMP_LOAD_GRAYSCALE)) {
        if (mybmp->depth <= 8) {
            cinfo->quantize_colors = TRUE;
    
            /* Get system palette */
            cinfo->actual_number_of_colors = 0x01 << mybmp->depth;
    
            /* Allocate jpeg colormap space */
            cinfo->colormap = (*cinfo->mem->alloc_sarray)
                ((j_common_ptr) cinfo, JPOOL_IMAGE,
                       (JDIMENSION)cinfo->actual_number_of_colors,
                (JDIMENSION)3);

            /* Set colormap from system palette */
            for(i = 0; i < cinfo->actual_number_of_colors; ++i)
            {
                cinfo->colormap[0][i] = pal[i].r >> 2;
                cinfo->colormap[1][i] = pal[i].g >> 2;
                cinfo->colormap[2][i] = pal[i].b >> 2;
            }
        }
    }
    else {
        /* Grayscale output asked */
        cinfo->quantize_colors = TRUE;
        cinfo->out_color_space = JCS_GRAYSCALE;
        cinfo->desired_number_of_colors = 256;
        for (i = 0; i < 256; i++) {
            pal [i].r = i;
            pal [i].g = i;
            pal [i].b = i;
        }
    }

    jpeg_calc_output_dimensions (cinfo);

    if (mybmp->flags & MYBMP_LOAD_GRAYSCALE) {
        mybmp->depth = 8;
    }
    else {
        mybmp->flags |= MYBMP_TYPE_RGB;
        if (cinfo->output_components == 3)
            mybmp->flags |= MYBMP_RGBSIZE_3;
        else if (cinfo->output_components == 4)
            mybmp->flags |= MYBMP_RGBSIZE_4;
        else
            mybmp->flags |= MYBMP_TYPE_NORMAL;

        mybmp->depth = cinfo->output_components * 8;
    }

    mybmp->w = cinfo->output_width;
    mybmp->h = cinfo->output_height;
    mybmp->frames = 1;
    bmp_ComputePitch (mybmp->depth, mybmp->w, &mybmp->pitch, TRUE);
    mybmp->flags |= MYBMP_TYPE_NORMAL | MYBMP_FLOW_DOWN;

    init_info->jpeg_info = cinfo;
    init_info->jpeg_err = jerr;
    /* Step 5: Start decompressor */

    return init_info;

err:
    fprintf(stderr, "__mg_init_jpg error!\n");
    return NULL;
}

void __mg_cleanup_jpg (void* init_info)
{
    struct jpeg_decompress_struct *cinfo;
    struct my_error_mgr *jerr;
    jpeg_init_info_t* info = (jpeg_init_info_t*)init_info;

    cinfo = (struct jpeg_decompress_struct *)info->jpeg_info;
    jerr = (struct my_error_mgr *)info->jpeg_err;

    /* Step 7: Finish decompression */
    if (info->dec_started)
        jpeg_finish_decompress (cinfo);

    /* Step 8: Release JPEG decompression object */
    jpeg_destroy_decompress (cinfo);

    free (jerr);
    free (cinfo);
    free (info);
}

int __mg_load_jpg (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp,
                   CB_ONE_SCANLINE cb, void* context)
{
    jpeg_init_info_t *info;
    BYTE* bits;
    struct jpeg_decompress_struct* cinfo;
    struct my_error_mgr *jerr;
    int i, pitch = 0;

    info = (jpeg_init_info_t *)init_info;
    cinfo = (struct jpeg_decompress_struct *)info->jpeg_info;
    jerr = (struct my_error_mgr *)info->jpeg_err;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp (jerr->setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to return error.
         */
        return ERR_BMP_LOAD;
    }

    jpeg_start_decompress (cinfo);
    info->dec_started = 1;

    if (!(my_bmp->flags & MYBMP_LOAD_ALLOCATE_ONE))
        pitch = my_bmp->pitch;

    bits = my_bmp->bits;
    /* Step 6: while (scan lines remain to be read) */
    for (i = 0; i < cinfo->output_height; i++, bits += pitch) {
        JSAMPROW rowptr[1];
        rowptr[0] = (JSAMPROW)(bits);
        jpeg_read_scanlines (cinfo, rowptr, 1);
        if (cb && !pitch)
            cb (context, my_bmp, i);
    }

    return ERR_BMP_OK;
}


/**
 * loop read JPEG marker from MG_RWops object,until reach SOF0 or SOF2.
 * return TRUE if match JPEG image format,or FALSE if the read failed or not JPEG image
 */
BOOL __mg_check_jpg (MG_RWops* fp)
{
    for(Uint16 jpeg_marker;read_be16(fp,&jpeg_marker) /* read JPEG marker */;){
        /* payload length of current marker */
        Uint16 payload = 1; /* set 1 for default, mean that current marker followed by payload bytes*/
        switch(jpeg_marker)
        {
        case JMK_SOI:
            payload = 0; /* no payload */
            break;
        case JMK_SOF0:
        case JMK_SOF2:
            return TRUE; /* JPEG image*/
        case JMK_DHT:
        case JMK_DQT:
        case JMK_DRI:
        case JMK_SOS:
        case JMK_COM:
            break;
        case JMK_EOI:
            return FALSE; /* not JPEG image*/
        default:
            if((0XFFF8 & jpeg_marker) == JMK_RST_mask){
                payload = 0; /* RST0~7(FFD0~FFD7),no payload */
            }else if((0XFFF0 & jpeg_marker) == JMK_APP_mask){
                /* APP0~APP15,do nothing */
            }else
                return FALSE; /* not JPEG image*/
        }
        if(payload){
            /*read payload length and skip next marker */
            if(!read_be16(fp,&payload))
                return FALSE; /* not JPEG image*/
            MGUI_RWseek (fp, payload- sizeof(payload), SEEK_CUR);
        }
    }
    return FALSE; /* not JPEG image*/
}

#endif /* _MGIMAGE_JPG */

