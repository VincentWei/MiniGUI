#ifndef __VDC_INIT_H__
#define __VDC_INIT_H__

#include <linux/fujitsu/mb93493-vdc.h>

/* define macro */
#define MODE_LCD	0
#define MODE_VGA	1
#define MODE_NTSC	2
#define MODE_PAL 	3

int open_vdc_device(int mode);
void close_vdc_device(void);
void vdc_set_data(int idx);

#endif /* !__VDC_INIT_H__ */
