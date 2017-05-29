#ifndef _XXVFB_COMMON_H
#define _XXVFB_COMMON_H

extern void InitWndImage( Display *pDisplay, Window Win, XImage **ppImage );
extern void DrawImage(int x, int y, int w, int h, char* data);

#endif /* _XXVFB_COMMON_H */
