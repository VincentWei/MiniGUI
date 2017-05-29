/*
**  $Id: fb3dfx.h 6566 2006-05-10 01:44:57Z xwyan $
**  
**  3Dfx hardware acceleration for the GAL framebuffer console driver
**
**  Copyright (C) 2003 ~ 2006 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/ 

#include "fbvideo.h"

/* Set up the driver for 3Dfx acceleration */
extern void FB_3DfxAccel(_THIS, __u32 card);
