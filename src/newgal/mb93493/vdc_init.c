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
**  $Id: vdc_init.c 7353 2007-08-16 04:58:32Z xgwang $
**  
**  Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <strings.h>

#include "vdc_init.h"

static int fd = -1;
static int mmap_len = 0;
static struct fr400vdc_config cfg;
static unsigned char *mmap_area = NULL;

extern unsigned char	*VRAM_addr;		/* point to display memory of VDC */
static void* mmap_fd(int fd);

/* VDC regs
 * 	PHTC  RHDC  RHFP  RHSC  RHBP  
 * 	RVTC  RVDC  RVFP  RVSC  RVBP
 * 	RHIP  RVIP  RCK
 */
static int prms_lcd[13] = { 323, 320, 1, 1, 1, 302, 242, 27, 6, 27, 0, 0, 8 };
static int prms_rgb[13] = { 858, 640,11, 67, 60, 1, 480, 2, 4, 16, 0, 0, 2 };
static int prms_ntsc[13]= { 858, 720, 16, 120 ,2, 262, 240, 2, 19, 1, 0, 0, 1 };
static int prms_pal[13] = { 864, 720, 16, 120, 8, 312, 288, 1, 22, 1, 0, 0, 1 }; 

static void* vdc_init(int mode)
{
	int	i;
	
	unsigned short *addr;

	/* open VDC device */
	if((fd = open("/dev/fr400cc_vdc", O_RDWR | O_NONBLOCK)) < 0) {
		fprintf (stderr, "MB93493 NEWGAL Engine: Failed to open device - /dev/fr400cc_vdc!\n");
		return 0;
	}

	/* get VDC config */
	if(ioctl(fd, VDCIOCGCFG, &cfg) != 0) {
		fprintf (stderr, "MB93493 NEWGAL Engine:  Failed to call ioctl (VDCIOCGCFG)!\n");
		close(fd);
		return 0;
	}

	/* init VDC's config parameters */
	switch(mode)
	{
		case MODE_LCD: /* for LCD */
			cfg.pix_x = 320;
			cfg.pix_y = 242;
			cfg.hls   = 0;
			cfg.cscv  = 2;
			cfg.vsop  = 1;
			cfg.enop  = 1;
			cfg.dsm   = 1;
			cfg.dbls  = 0;
			cfg.r601  = 0;
			cfg.die   = 1;
			cfg.dpf   = 1;
			bcopy(prms_lcd, cfg.prm, 4*13);
			break;

		case MODE_NTSC: /* for TV */
			cfg.pix_x = 720;
			cfg.pix_y = 480;
			cfg.hls   = 1;
			cfg.cscv  = 0;
			cfg.dbls  = 1;
			cfg.r601  = 1;
			cfg.tfop  = 0;
			cfg.dsm   = 1;
			cfg.die   = 1;
			cfg.dsr   = 1;
			cfg.dpf   = 3;
			bcopy(prms_ntsc, cfg.prm, 4*13);
			break;
			
		case MODE_PAL:
			cfg.pix_x = 720;
			cfg.pix_y = 576;
			cfg.hls   = 1;
			cfg.cscv  = 0;
			cfg.dbls  = 1;
			cfg.r601  = 1;
			cfg.tfop  = 0;
			cfg.dsm   = 1;
			cfg.die   = 1;
			cfg.dsr   = 1;
			cfg.dpf   = 3;
			cfg.pal   = 1;
			bcopy(prms_pal, cfg.prm, 4*13);
			break;
			
		case MODE_VGA: /* VGA for monitor */
			cfg.pix_x = 640;
			cfg.pix_y = 480;
			cfg.hls   = 0;
			cfg.cscv  = 2;
			cfg.vsop  = 1;
			cfg.enop  = 1;
			cfg.dsm   = 1; 
			cfg.dbls  = 0;
			cfg.r601  = 0;
			cfg.die   = 1;
			cfg.dpf   = 1;
			bcopy(prms_rgb, cfg.prm, 4*13);
			break;

		default:
			fprintf(stderr, "MB93493 NEWGAL Engine:  bad display mode!\n");
			return 0;
	}
	
	cfg.skipbf = 1;	/* skip bottom field */

	cfg.buf_num = 2;
	cfg.buf_unit_sz = cfg.pix_x * cfg.pix_y * 2;

	mmap_len = cfg.buf_unit_sz * cfg.buf_num;

	if((void *)(mmap_area = (unsigned char *)mmap_fd(fd)) == (void*)-1) {
		fprintf(stderr, "MB93493 NEWGAL Engine:  VDC mmap error!\n");
		close(fd);
		return 0;
	}

	addr = (unsigned short *)mmap_area;

	for(i = 0 ; i < cfg.pix_x * cfg.pix_y * cfg.buf_num; i++) *addr++ = 0x0000;

	cfg.rddl = 1; /* for underrun */

	/* set config parameters */
	if(ioctl(fd, VDCIOCSCFG, &cfg) != 0) {
		fprintf(stderr, "MB93493 NEWGAL Engine: VDC ioctl set cfg err!\n");
		munmap(mmap_area, mmap_len);
		close(fd);
		return 0;
	}

	/* get config parameters */
	if(ioctl(fd, VDCIOCGCFG, &cfg) != 0)
	{
		fprintf(stderr, "MB93493 NEWGAL Engine: VDC ioctl get cfg err!\n");
		munmap(mmap_area, mmap_len);
		close(fd);
		return 0;
	}
	
	return mmap_area;
}

static int vdc_start(int still)
{
	int i;

	if (!still)
	{
		for(i = 0; i < cfg.buf_num; i++) {
			if(ioctl(fd, VDCIOCSDAT, i) < 0)
			{
				fprintf(stderr, "MB93493 NEWGAL Engine: ioctl: VDCIOCSDAT error!\n");
				munmap(mmap_area, mmap_len);
				close(fd);
				return -1;
			}
		}
	} else {
		if(ioctl(fd,VDCIOCSDAT, 0) < 0)
		{
			fprintf(stderr, "MB93493 NEWGAL Engine: ioctl: VDCIOCSDAT error!\n");
			munmap(mmap_area, mmap_len);
			close(fd);
			return -1;
		}
	}

	if(ioctl(fd, VDCIOCSTART, 0) != 0) {
		fprintf(stderr, "MB93493 NEWGAL Engine: VDC ioctl start err\n");
		munmap(mmap_area, mmap_len);
		close(fd);
		return -1;
	}
	
	return 0;
}

void vdc_set_data(int idx)
{
	while(-1 == ioctl(fd, VDCIOCSDAT, idx));
}

static void * mmap_fd(int fd)
{
	void *pt;
	void *start;
	int length, prot, flags, offset;

	start  = NULL;
	length = cfg.buf_unit_sz * cfg.buf_num;
	prot   = PROT_READ | PROT_WRITE;
	flags  = MAP_SHARED;
	offset = 0;

#if 0
	printf("pix_x = %d\n", cfg.pix_x);
	printf("pix_y = %d\n", cfg.pix_y);
	printf("pix_sz = %d\n", cfg.pix_sz);
	printf("skipbf = %d\n", cfg.skipbf);
	printf("prm[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", 
						cfg.prm[0],	cfg.prm[1],	cfg.prm[2],	cfg.prm[3],
							cfg.prm[4],	cfg.prm[5],	cfg.prm[6],	cfg.prm[7],
								cfg.prm[8],	cfg.prm[9],	cfg.prm[10],	cfg.prm[11],	cfg.prm[12]);
	printf("rddl = %d\n", cfg.rddl);
	printf("hls = %d\n", cfg.hls);
	printf("pal = %d\n", cfg.pal);
	printf("cscv = %d\n", cfg.cscv);
	printf("dbls = %d\n", cfg.dbls);
	printf("r601 = %d\n", cfg.r601);
	printf("tfop = %d\n", cfg.tfop);
	printf("dsm = %d\n", cfg.dsm);
	printf("dfp = %d\n", cfg.dfp);
	printf("die = %d\n", cfg.die);
	printf("enop = %d\n", cfg.enop);
	printf("vsop = %d\n", cfg.vsop);
	printf("hsop = %d\n", cfg.hsop);
	printf("dsr = %d\n", cfg.dsr);
	printf("csron = %d\n", cfg.csron);
	printf("dpf = %d\n", cfg.dpf);
	printf("dms = %d\n", cfg.dms);
	printf("dma_mode = %d\n", cfg.dma_mode);
	printf("dma_ats = %d\n", cfg.dma_ats);
	printf("dma_rs = %d\n", cfg.dma_rs);
		
	printf("Enter mmap_fd(%d): start = %p, length = %d, prot = %x, flags = %x, fd = %d, offset = %d[%x]\n", 
			fd, start, length, prot, flags, fd, offset, offset);
#endif

	pt = mmap(start, length, prot, flags, fd, offset);
	if (((void*)-1) == pt)
	{
		int er = errno;
	}
	return pt;
}

int open_vdc_device(int mode)
{
	/* get display memory of LCD */
	VRAM_addr = (unsigned char*)vdc_init (mode);
	if (VRAM_addr == 0) {
		return -1;
	}

	/* start VDC(LCD) */	
	if (vdc_start(1) < 0) {
		return -1;
	}
	
	return 0;
}

void close_vdc_device (void)
{
	munmap (mmap_area, mmap_len);
	ioctl (fd, VDCIOCSTOP, 0);
	close (fd);
}

