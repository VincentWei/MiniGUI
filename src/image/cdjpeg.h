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

#define JPEG_CJPEG_DJPEG	/* define proper options in jconfig.h */
#define JPEG_INTERNAL_OPTIONS	/* cjpeg.c,djpeg.c need to see xxx_SUPPORTED */
#include "jpeglib.h"
#include "jpg.h"		/* get library error codes too */


/*
 * Object interface for cjpeg's source file decoding modules
 */

typedef struct cjpeg_source_struct * cjpeg_source_ptr;

struct cjpeg_source_struct {
  JMETHOD(void, start_input, (j_compress_ptr cinfo,
			      cjpeg_source_ptr sinfo));
  JMETHOD(JDIMENSION, get_pixel_rows, (j_compress_ptr cinfo,
				       cjpeg_source_ptr sinfo));
  JMETHOD(void, finish_input, (j_compress_ptr cinfo,
			       cjpeg_source_ptr sinfo));

  FILE *input_file;

  JSAMPARRAY buffer;
  JDIMENSION buffer_height;
};


/*
 * Object interface for djpeg's output file encoding modules
 */

typedef struct djpeg_dest_struct * djpeg_dest_ptr;

struct djpeg_dest_struct {
  /* start_output is called after jpeg_start_decompress finishes.
   * The color map will be ready at this time, if one is needed.
   */
  JMETHOD(void, start_output, (j_decompress_ptr cinfo,
			       djpeg_dest_ptr dinfo));
  /* Emit the specified number of pixel rows from the buffer. */
  JMETHOD(void, put_pixel_rows, (j_decompress_ptr cinfo,
				 djpeg_dest_ptr dinfo,
				 JDIMENSION rows_supplied));
  /* Finish up at the end of the image. */
  JMETHOD(void, finish_output, (j_decompress_ptr cinfo,
				djpeg_dest_ptr dinfo));

  /* Target file spec; filled in by djpeg.c after object is created. */
  FILE * output_file;

  /* Output pixel-row buffer.  Created by module init or start_output.
   * Width is cinfo->output_width * cinfo->output_components;
   * height is buffer_height.
   */
  JSAMPARRAY buffer;
  JDIMENSION buffer_height;
};


/*
 * cjpeg/djpeg may need to perform extra passes to convert to or from
 * the source/destination file format.  The JPEG library does not know
 * about these passes, but we'd like them to be counted by the progress
 * monitor.  We use an expanded progress monitor object to hold the
 * additional pass count.
 */

struct cdjpeg_progress_mgr {
  struct jpeg_progress_mgr pub;	/* fields known to JPEG library */
  int completed_extra_passes;	/* extra passes completed */
  int total_extra_passes;	/* total extra */
  /* last printed percentage stored here to avoid multiple printouts */
  int percent_done;
};

typedef struct cdjpeg_progress_mgr * cd_progress_ptr;


/* Module selection routines for I/O modules. */

EXTERN(cjpeg_source_ptr) jinit_read_bmp JPP((j_compress_ptr cinfo));
EXTERN(djpeg_dest_ptr) jinit_write_bmp JPP((j_decompress_ptr cinfo,
					    boolean is_os2));
EXTERN(cjpeg_source_ptr) jinit_read_gif JPP((j_compress_ptr cinfo));
EXTERN(djpeg_dest_ptr) jinit_write_gif JPP((j_decompress_ptr cinfo));
EXTERN(cjpeg_source_ptr) jinit_read_ppm JPP((j_compress_ptr cinfo));
EXTERN(djpeg_dest_ptr) jinit_write_ppm JPP((j_decompress_ptr cinfo));
EXTERN(cjpeg_source_ptr) jinit_read_rle JPP((j_compress_ptr cinfo));
EXTERN(djpeg_dest_ptr) jinit_write_rle JPP((j_decompress_ptr cinfo));
EXTERN(cjpeg_source_ptr) jinit_read_targa JPP((j_compress_ptr cinfo));
EXTERN(djpeg_dest_ptr) jinit_write_targa JPP((j_decompress_ptr cinfo));

/* cjpeg support routines (in rdswitch.c) */

EXTERN(boolean) read_quant_tables JPP((j_compress_ptr cinfo, char * filename,
				    int scale_factor, boolean force_baseline));
EXTERN(boolean) read_scan_script JPP((j_compress_ptr cinfo, char * filename));
EXTERN(boolean) set_quant_slots JPP((j_compress_ptr cinfo, char *arg));
EXTERN(boolean) set_sample_factors JPP((j_compress_ptr cinfo, char *arg));

/* djpeg support routines (in rdcolmap.c) */

EXTERN(void) read_color_map JPP((j_decompress_ptr cinfo, FILE * infile));

/* common support routines (in cdjpeg.c) */

EXTERN(void) enable_signal_catcher JPP((j_common_ptr cinfo));
EXTERN(void) start_progress_monitor JPP((j_common_ptr cinfo,
					 cd_progress_ptr progress));
EXTERN(void) end_progress_monitor JPP((j_common_ptr cinfo));
EXTERN(boolean) keymatch JPP((char * arg, const char * keyword, int minchars));
EXTERN(FILE *) read_stdin JPP((void));
EXTERN(FILE *) write_stdout JPP((void));

/* miscellaneous useful macros */

#ifdef DONT_USE_B_MODE		/* define mode parameters for fopen() */
#define READ_BINARY	"r"
#define WRITE_BINARY	"w"
#else
#define READ_BINARY	"rb"
#define WRITE_BINARY	"wb"
#endif

#ifndef EXIT_FAILURE		/* define exit() codes if not provided */
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS  0
#endif
#ifndef EXIT_WARNING
#define EXIT_WARNING  2
#endif
