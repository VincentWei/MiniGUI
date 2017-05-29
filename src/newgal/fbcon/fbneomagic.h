/*
** $Id: fbneomagic.h 7352 2007-08-16 04:57:12Z xgwang $
**
** Port from DirectFB by Wei Yongming. 
**
** Copyright (C) 2001 ~ 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** NeoMagic hardware acceleration for the MiniGUI framebuffer console driver
*/ 

#include "fbvideo.h"

/* Set up the driver for NeoMagic acceleration */
extern void FB_NeoMagicAccel(_THIS, __u32 card);

