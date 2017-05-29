#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#ifndef _NETIAL_CTRLWIN_H_
#define _NETIAL_CTRLWIN_H_

#define KEYDOWN 0
#define KEYUP 1

#define IAL_MOUSEEVENT          1
#define IAL_KEYEVENT            2


extern int CreateCtrlWin(char *pURL, int nPort);

#endif
