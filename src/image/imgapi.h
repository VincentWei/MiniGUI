#ifndef _IMGAPI_H_
#define _IMGAPI_H_

#include "common.h"
#include "gdi.h"

extern int CreateBitmapFromGIFFile(PBITMAP,const char*);
extern int CreateBitmapFromGIFMemory(PBITMAP,void*,int);

extern int CreateBitmapFromJPGFile(PBITMAP,const char*);
extern int CreateBitmapFromJPGMemory(PBITMAP,void*,int);

#endif
