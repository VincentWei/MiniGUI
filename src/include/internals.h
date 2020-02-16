///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** internals.h: this head file declares all internal types and data.
**
** Create date: 1999/05/22
*/

#ifndef GUI_INTERNALS_H
    #define GUI_INTERNALS_H

#include "constants.h"
#include "cliprect.h"

/******************* Internal data *******************************************/

/* Internal window message. */
#if (defined(_MG_ENABLE_SCREENSAVER) || \
        defined(_MG_ENABLE_WATERMARK)) && defined(_MGRM_THREADS)
#define MSG_CANCELSCREENSAVER   0x0201
#endif

/* Internal use extended style. */
#define WS_EX_MODALDISABLED     0x10000000L

/**
 * \def WS_EX_CTRLASMAINWIN
 * \brief The control can be displayed out of the main window
 *        which contains the control.
 */
#define WS_EX_CTRLASMAINWIN     0x20000000L

/* Make sure the internal window extended styles above are ok */
#define MGUI_COMPILE_TIME_ASSERT(name, x)               \
       typedef int MGUI_dummy_ ## name[((x)?1:0) * 2 - 1]

MGUI_COMPILE_TIME_ASSERT(ws_ex_int_1, WS_EX_INTERNAL_MASK & WS_EX_MODALDISABLED);
MGUI_COMPILE_TIME_ASSERT(ws_ex_int_2, WS_EX_INTERNAL_MASK & WS_EX_CTRLASMAINWIN);

#undef MGUI_COMPILE_TIME_ASSERT

/******************** Handle type and child type. ***************************/
#define TYPE_HWND           0x01
    #define TYPE_MAINWIN    0x11
    #define TYPE_CONTROL    0x12
    #define TYPE_ROOTWIN    0x13
    #define TYPE_VIRTWIN    0x14
#define TYPE_HMENU          0x02
    #define TYPE_MENUBAR    0x21
    #define TYPE_PPPMENU    0x22
    #define TYPE_NMLMENU    0x23
#define TYPE_HACCEL         0x03
#define TYPE_HCURSOR        0x05
#define TYPE_HICON          0x07
#define TYPE_HDC            0x08
    #define TYPE_SCRDC      0x81
    #define TYPE_GENDC      0x82
    #define TYPE_MEMDC      0x83

#define TYPE_WINTODEL       0xF1
#define TYPE_UNDEFINED      0xFF

#include "zorder.h"

struct GAL_Surface;
struct _MAINWIN;
typedef struct _MAINWIN* PMAINWIN;

struct _TIMER;
typedef struct _TIMER TIMER;

typedef struct _SCROLLWINDOWINFO
{
    int iOffx;
    int iOffy;
    const RECT* rc1;
    const RECT* rc2;
} SCROLLWINDOWINFO;
typedef SCROLLWINDOWINFO* PSCROLLWINDOWINFO;

typedef struct _CARETINFO {
    int     x;              // position of caret
    int     y;
    void*   pNormal;        // normal bitmap.
    void*   pXored;         // bit-Xored bitmap.

    PBITMAP pBitmap;        // user defined caret bitmap.

    int     nWidth;         // original size of caret
    int     nHeight;
    int     nBytesNr;       // number of bitmap bytes.

    BITMAP  caret_bmp;      // bitmap of caret.

    BOOL    fBlink;         // does blink?
    BOOL    fShow;          // show or hide currently.

    HWND    hOwner;         // the window owns the caret.
    UINT    uTime;          // the blink time.
} CARETINFO;
typedef CARETINFO* PCARETINFO;

/* ime status info */
typedef struct _IME_STATUS_INFO
{
    HWND hWnd;
    BYTE bEnabled;
    BYTE bAutoTrack;
    BYTE reserved[2];
    IME_TARGET_INFO TargetInfo;
} IME_STATUS_INFO;

typedef struct _QMSG
{
    MSG                 Msg;
    struct _QMSG*       next;
} QMSG;
typedef QMSG* PQMSG;

typedef struct _MSGQUEUE MSGQUEUE;
typedef MSGQUEUE* PMSGQUEUE;

#ifdef _MGHAVE_VIRTUAL_WINDOW
typedef struct _SYNCMSG
{
    MSG              Msg;
    LRESULT          retval;
    sem_t*           sem_handle;
    struct _SYNCMSG* pNext;
} SYNCMSG;
typedef SYNCMSG* PSYNCMSG;
#endif   /* defined _MGHAVE_VIRTUAL_WINDOW */

typedef BOOL (* IDLEHANDLER) (PMSGQUEUE msg_que);

// the MSGQUEUE struct is a internal struct.
// using semaphores to implement message queue.
struct _MSGQUEUE
{
    DWORD dwState;              // message queue states

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_t th;               // the thread identifier this message queue lives
                                // moved from window structures since 4.2.0.
    pthread_mutex_t lock;       // lock
    sem_t wait;                 // the semaphore for wait message
    sem_t sync_msg;             // the semaphore for sync message
#endif

    PQMSG  pFirstNotifyMsg;     // head of the notify message queue
    PQMSG  pLastNotifyMsg;      // tail of the notify message queue

#ifdef _MGHAVE_VIRTUAL_WINDOW
    PSYNCMSG pFirstSyncMsg;     // head of the sync message queue
    PSYNCMSG pLastSyncMsg;      // tail of the sync message queue
#endif

    IDLEHANDLER OnIdle;         // XXX: idle handler

#ifdef _MGHAVE_VIRTUAL_WINDOW
    PMAINWIN pRootMainWin;      // the root main window of this message queue.
#endif
    int nrWindows;              // the number of main/virtual windows.

    /* buffer for post message */
    int len;                    // buffer length
    MSG* msg;                   // post message buffer
    int readpos, writepos;      // positions for reading and writing

    int loop_depth;             // message loop depth, for dialog boxes

    /* Since 4.2.0, MiniGUI provides support for timers per message thread */
    int FirstTimerSlot;         // the first timer slot to be checked
    TIMER* timer_slots [DEF_NR_TIMERS];
                                // slots for timer
    DWORD TimerMask;            // timer slots mask
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL mg_InitFreeQMSGList (void);
void mg_DestroyFreeQMSGList (void);
BOOL mg_InitMsgQueue (PMSGQUEUE pMsgQueue, int iBufferLen);
void mg_DestroyMsgQueue (PMSGQUEUE pMsgQueue);
BOOL kernel_QueueMessage (PMSGQUEUE pMsgQueue, PMSG msg);

extern PMSGQUEUE __mg_dsk_msg_queue;

/* Running stages */
#define _MG_QUITING_STAGE_RUNNING 1
/* Try to quit main thread, and for MiniGUI-Threads, to quit
   other main window threads */
#define _MG_QUITING_STAGE_START   (-10)
/* Force to quit main thread, and other main window threads */
#define _MG_QUITING_STAGE_FORCE   (-15)
/* Quit Desktop thread (MiniGUI-Threads only) */
#define _MG_QUITING_STAGE_DESKTOP (-20)
/* Quit EventLoop (MiniGUI-Threads only) */
#define _MG_QUITING_STAGE_EVENT   (-30)
/* Quit TimerEntry (MiniGUI-Threads only) */
#define _MG_QUITING_STAGE_TIMER   (-40)
extern int __mg_quiting_stage;

static inline BOOL QueueDeskMessage (PMSG msg)
{
    return kernel_QueueMessage (__mg_dsk_msg_queue, msg);
}

#ifndef _MGRM_THREADS
static inline BOOL InitDskMsgQueue (void)
{
    return mg_InitMsgQueue (__mg_dsk_msg_queue, 0);
}

static inline void DestroyDskMsgQueue (void)
{
    mg_DestroyMsgQueue (__mg_dsk_msg_queue);
}

static inline void SetDskIdleHandler (IDLEHANDLER idle_handler)
{
    __mg_dsk_msg_queue->OnIdle = idle_handler;
}
#endif /* not defined _MGRM_THREADS */

#ifdef _MGHAVE_VIRTUAL_WINDOW

  #define MG_MUTEX_INIT(lock)      pthread_mutex_init(lock, NULL)
  #define MG_MUTEX_DESTROY(lock)   pthread_mutex_destroy(lock)
  #define MG_MUTEX_LOCK(lock)      pthread_mutex_lock(lock)
  #define MG_MUTEX_UNLOCK(lock)    pthread_mutex_unlock(lock)

  #define POST_MSGQ(pMsgQueue) \
  do { \
    int sem_value; \
    /* Signal that the msg queue contains one more element for reading */ \
    sem_getvalue (&(pMsgQueue)->wait, &sem_value); \
    if (sem_value <= 0) {\
        sem_post(&(pMsgQueue)->wait); \
    } \
  } while(0)

#else /* defined _MGHAVE_VIRTUAL_WINDOW */

  #define MG_MUTEX_INIT(lock)
  #define MG_MUTEX_DESTROY(lock)
  #define MG_MUTEX_LOCK(lock)
  #define MG_MUTEX_UNLOCK(lock)

  #define POST_MSGQ(pMsgQueue)

#endif /* not defined _MGHAVE_VIRTUAL_WINDOW */

#define LOCK_MSGQ(pMsgQueue)     MG_MUTEX_LOCK(&(pMsgQueue)->lock)
#define UNLOCK_MSGQ(pMsgQueue)   MG_MUTEX_UNLOCK(&(pMsgQueue)->lock)

struct _wnd_element_data;

#define WF_ERASEBKGND    0x01 // flag to erase bkground or not

/* Since 4.2.0 */
typedef struct _VIRTWIN
{
    /*
     * Fields for data type
     */
    unsigned char DataType; // the data type.
    unsigned char WinType;  // the window type, always be TYPE_VIRTWIN.
    unsigned short Flags;   // some flags for this viritual window.

    /*
     * Fields for both virtual window and main window.
     */
#ifdef _MGUSE_VIRTUAL_WINDOW
    //pthread_t th;         // the thread which creates this virtual window.
                            // moved to message queue structure since 4.2.0.
#endif
    PMSGQUEUE pMsgQueue;    // the message queue.

    char* spCaption;        // the caption of main window.
    LINT  id;               // the identifier of main window.

    WNDPROC   WndProc;      // the window procedure of this virtual window.
    NOTIFPROC NotifProc;    // the notification callback procedure (no use).

    DWORD dwAddData;        // the additional data.
    DWORD dwAddData2;       // the second addtional data.

    struct _VIRTWIN* pMainWin;      // the main window that contains this window.
                                    // for virtual window, always be itself.
    struct _VIRTWIN* pHosting;      // the hosting virtual window.
    struct _VIRTWIN* pFirstHosted;  // the first hosted virtual window.
    struct _VIRTWIN* pNextHosted;   // the next hosted virtual window.
} VIRTWIN;
typedef struct _VIRTWIN* PVIRTWIN;

// the structure represents a real main window.
typedef struct _MAINWIN
{
    /*
     * These following fields are similiar with CONTROL struct.
     */

    /*
     * Fields for data type
     * VM[2020-02-14]: Move these fields back to support virtual main window.
     * VM[2018-01-18]: Move these fields from header to here to compatible with
     *      WINDOWINFO
     */
    unsigned char DataType; // the data type.
    unsigned char WinType;  // the window type.
    unsigned short Flags;   // special runtime flags, such EraseBkGnd flags

    /*
     * Common fields for control, virtual window, and main window.
     * VM[2020-02-14]: Move these fields to header to support virtual window.
     */
#ifdef _MGUSE_VIRTUAL_WINDOW
    // pthread_t th;        // the thread which creates this main window.
                            // moved to message queue structure since 4.2.0.
#endif
    PMSGQUEUE pMsgQueue;    // the message queue.

    char* spCaption;        // the caption of main window.
    LINT  id;               // the identifier of main window.

    WNDPROC MainWindowProc; // the window procedure of this main window.
    NOTIFPROC NotifProc;    // the notification callback procedure.

    DWORD dwAddData;        // the additional data.
    DWORD dwAddData2;       // the second addtional data.

    /*
     * The following members are implemented for main window and virtual window.
     */
    struct _MAINWIN* pMainWin;  // the main window that contains this window.
                                // for main window, always be itself.
    struct _MAINWIN* pHosting;      // the hosting main window.
    struct _MAINWIN* pFirstHosted;  // the first hosted main window.
    struct _MAINWIN* pNextHosted;   // the next hosted main window.

    /*
     * Fields for appearance of this main window.
     */
    int left, top;      // the position and size of main window.
    int right, bottom;

    int cl, ct;         // the position and size of client area.
    int cr, cb;

    DWORD dwStyle;      // the styles of main window.
    DWORD dwExStyle;    // the extended styles of main window.

    int idx_znode;      // the index of the z-node.
    //gal_pixel iFgColor; // the foreground color.
    gal_pixel iBkColor; // the background color.

    HMENU hMenu;        // handle of menu.
    HACCEL hAccel;      // handle of accelerator table.
    HCURSOR hCursor;    // handle of cursor.
    HICON hIcon;        // handle of icon.
    HMENU hSysMenu;     // handle of system menu.
    PLOGFONT pLogFont;  // pointer to logical font.

    LFSCROLLBARINFO vscroll; // the vertical scroll bar information.
    LFSCROLLBARINFO hscroll; // the horizital scroll bar information.

    /*
     * Fields for window element data.
     */
    WINDOW_ELEMENT_RENDERER* we_rdr;    // the window renderer
    struct _wnd_element_data* wed;      // the data of window elements

    HDC   privCDC;      // the private client DC.
    INVRGN InvRgn;      // the invalid region of this main window.
#ifdef _MGSCHEMA_COMPOSITING
    struct GAL_Surface* surf;  // the shared surface of this main window.
#else
    PGCRINFO pGCRInfo;      // pointer to global clip region info struct.
#endif

    PCARETINFO pCaretInfo;  // pointer to system caret info struct.

    /*
     * Fields to manage the relationship among main windows and controls.
     */
    HWND hParent;           // the parent of this window.
                            // for main window, always be HWND_DESKTOP.

    HWND hFirstChild;       // the handle of first child window.
    HWND hActiveChild;      // the currently active child window.
    HWND hOldUnderPointer;  // the old child window under pointer.
    HWND hPrimitive;        // the premitive child of mouse event.

    /*
     * The following members are only implemented for main window.
     */
#ifndef _MGSCHEMA_COMPOSITING
    GCRINFO GCRInfo;    // the global clip region info struct.
                        // put here to avoid invoking malloc function.
#endif

    // the controls as main window
    HWND hFirstChildAsMainWin;

    HDC   secondaryDC;                // the secondary window dc.
    ON_UPDATE_SECONDARYDC update_secdc; // the callback of secondary window dc
    RECT  update_rc;
} MAINWIN;

/************************* Initialization/Termination ************************/
void __mg_init_local_sys_text (void);

/* the zorder information of the server */
extern ZORDERINFO* __mg_zorder_info;

#ifdef _MGRM_STANDALONE

BOOL salone_IdleHandler4StandAlone (PMSGQUEUE msg_que);
BOOL salone_StandAloneStartup (void);
void salone_StandAloneCleanup (void);

#elif defined (_MGRM_PROCESSES)

/*
 * Common for server and client.
 */

/* the client id is zero for the server */
extern int __mg_client_id;

BOOL kernel_IsOnlyMe (void);
void* kernel_LoadSharedResource (void);
void kernel_UnloadSharedResource (void);
BOOL server_IdleHandler4Server (PMSGQUEUE msg_que);
void server_ServerCleanup (void);

/* Only for client. */
void* kernel_AttachSharedResource (void);
void kernel_UnattachSharedResource (void);
BOOL client_IdleHandler4Client (PMSGQUEUE msg_que);
BOOL client_ClientStartup (void);
void client_ClientCleanup (void);

#endif  /* defined (_MGRM_PROCESSES) */

BOOL mg_InitScreenDC (void);
void mg_TerminateScreenDC (void);

BOOL mg_InitGDI (void);
void mg_TerminateGDI (void);

BOOL mg_InitFixStr (void);
void mg_TerminateFixStr (void);

BOOL mg_InitMenu (void);
void mg_TerminateMenu (void);

BOOL mg_InitDesktop (void);
void mg_TerminateDesktop (void);

/* send MSG_IME_OPEN/CLOSE message to ime window */
void __gui_open_ime_window (PMAINWIN pWin, BOOL open_close, HWND rev_hwnd);

/* return main window contains hWnd. */
PMAINWIN gui_GetMainWindowPtrOfControl (HWND hWnd);

/* check whether hWnd is main window and return pointer to main window hWnd. */
PMAINWIN gui_CheckAndGetMainWindowPtr (HWND hWnd);

PMAINWIN gui_GetMainWindowPtrUnderPoint (int x, int y);

/* return the next window need to repaint. */
HWND kernel_CheckInvalidRegion (PMAINWIN pWin);

#ifndef _MGSCHEMA_COMPOSITING
/* return global clipping region of window. */
PGCRINFO kernel_GetGCRgnInfo (HWND hWnd);
#endif

/* internal variables */
typedef struct _TRACKMENUINFO* PTRACKMENUINFO;

extern DWORD __mg_timer_counter;

extern HWND __mg_capture_wnd;
extern HWND __mg_ime_wnd;
#ifndef _MGRM_PROCESSES
extern PMAINWIN __mg_active_mainwnd;
extern PTRACKMENUINFO __mg_ptmi;
#endif
extern PMAINWIN __mg_dsk_win;
extern HWND __mg_hwnd_desktop;

extern LRESULT SendSyncMessage (HWND hWnd,
                UINT msg, WPARAM wParam, LPARAM lParam);

#ifndef _MGRM_THREADS
#   ifndef _MGSCHEMA_COMPOSITING
extern unsigned int __mg_csrimgsize;
extern unsigned int __mg_csrimgpitch;
#   endif
#else /* not defined _MGRM_THREADS */
extern pthread_mutex_t __mg_gdilock, __mg_mouselock;
extern pthread_t __mg_desktop, __mg_parsor, __mg_timer;
#endif /* defined _MGRM_THREADS */

/* hWnd is a window, including HWND_DESKTOP */
#define MG_IS_WINDOW(hWnd)                                  \
        (hWnd && hWnd != HWND_INVALID &&                    \
         ((PMAINWIN)hWnd)->DataType == TYPE_HWND)

/* hWnd a normal window, not including HWND_DESKTOP */
#define MG_IS_NORMAL_WINDOW(hWnd)                           \
        (hWnd != HWND_DESKTOP && MG_IS_WINDOW(hWnd))

/* Whether hWnd is a main window */
#define MG_IS_MAIN_WINDOW(hWnd)                             \
        (MG_IS_WINDOW(hWnd) &&                              \
         ((PMAINWIN)hWnd)->WinType == TYPE_MAINWIN)

/* Whether hWnd is a virtual window */
#define MG_IS_VIRTUAL_WINDOW(hWnd)                          \
        (MG_IS_WINDOW(hWnd) &&                              \
         ((PMAINWIN)hWnd)->WinType == TYPE_VIRTWIN)

#define MG_IS_NORMAL_MAIN_WINDOW(hWnd)                      \
        (hWnd != HWND_DESKTOP && MG_IS_MAIN_WINDOW(hWnd))

#define MG_IS_DESTROYED_WINDOW(hWnd)                        \
        (hWnd && (hWnd != HWND_INVALID) &&                  \
         (((PMAINWIN)hWnd)->DataType == TYPE_WINTODEL))

#define MG_GET_WINDOW_PTR(hWnd)             ((PMAINWIN)hWnd)
#define MG_GET_MAIN_WINDOW_PTR(hWnd)        ((PMAINWIN)hWnd)
#define MG_GET_CONTROL_PTR(hWnd)            ((PCONTROL)hWnd)
#define MG_GET_VIRTUAL_WINDOW_PTR(hWnd)     ((PVIRTWIN)hWnd)

#define MG_CHECK_RET(condition, ret)                        \
            if (!(condition)) return ret

#define MG_CHECK(condition)                                 \
            if (!(condition)) return

/* get main window pointer of a window, including desktop window */
static inline PMAINWIN getMainWindowPtr (HWND hWnd)
{
    PMAINWIN pWin;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), NULL);
    pWin = MG_GET_WINDOW_PTR (hWnd);

    return pWin->pMainWin;
}

/* Get message queue by window handle.
   Note that hWnd may belong to a different thread. */
static inline PMSGQUEUE getMsgQueue (HWND hWnd)
{
    PMAINWIN pWin;
    pWin = getMainWindowPtr (hWnd);
    if (pWin)
        return pWin->pMsgQueue;
    return NULL;
}

#ifdef _MGHAVE_VIRTUAL_WINDOW

MSGQUEUE* mg_AllocMsgQueueForThisThread (void);
void mg_FreeMsgQueueForThisThread (void);
MSGQUEUE* mg_GetMsgQueueForThisThread (BOOL alloc);

extern pthread_key_t __mg_threadinfo_key;

static inline BOOL createThreadInfoKey (void)
{
    if (pthread_key_create (&__mg_threadinfo_key, NULL))
        return FALSE;
    return TRUE;
}

static inline void deleteThreadInfoKey (void)
{
    pthread_key_delete (__mg_threadinfo_key);
}

static inline MSGQUEUE* getMsgQueueForThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = (MSGQUEUE*)pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue == (void *)-1) {
        pMsgQueue = NULL;
    }
#endif

    return pMsgQueue;
}

static inline TIMER** getTimerSlotsForThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = (MSGQUEUE*)pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue == (void *)-1) {
        pMsgQueue = NULL;
    }
#endif

    if (pMsgQueue)
        return pMsgQueue->timer_slots;
    return NULL;
}

/* Be careful: does not check validity of hWnd */
static inline BOOL isWindowInThisThread (HWND hWnd)
{
    PMAINWIN pMainWin = getMainWindowPtr(hWnd);
#ifdef WIN32
    if (pMainWin && pMainWin->pMsgQueue->th.p == pthread_self().p)
#else
    if (pMainWin && pMainWin->pMsgQueue->th == pthread_self())
#endif
        return TRUE;

    return FALSE;
}

#else   /* define _MGHAVE_VIRTUAL_WINDOW */

static inline MSGQUEUE* getMsgQueueForThisThread (void)
{
    return __mg_dsk_msg_queue;
}

static inline TIMER** getTimerSlotsForThisThread (void)
{
    return __mg_dsk_msg_queue->timer_slots;
}

static inline BOOL isWindowInThisThread (HWND hWnd)
{
    return TRUE;
}

#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

#ifndef _MGRM_THREADS
static inline void SetDesktopTimerFlag (void)
{
    __mg_dsk_msg_queue->dwState |= QS_DESKTIMER;
}
#else   /* not defined _MGRM_THREADS */
static inline void AlertDesktopTimerEvent (void)
{
    if (__mg_dsk_msg_queue) {
        __mg_dsk_msg_queue->TimerMask = 1;
        POST_MSGQ (__mg_dsk_msg_queue);
    }
}
#endif /* defined _MGRM_THREADS */

static inline void setMsgQueueTimerFlag (PMSGQUEUE pMsgQueue, int slot)
{
    pMsgQueue->TimerMask |= (0x01 << slot);
    POST_MSGQ (pMsgQueue);
}

static inline void removeMsgQueueTimerFlag (PMSGQUEUE pMsgQueue, int slot)
{
    pMsgQueue->TimerMask &= ~(0x01 << slot);
}

BOOL mg_InitTimer (void);
BOOL mg_InstallIntervalTimer (void);
BOOL mg_UninstallIntervalTimer (void);

/*window element renderer manager interface*/
extern WINDOW_ELEMENT_RENDERER * __mg_def_renderer;

BOOL mg_InitLFManager (void);
void mg_TerminateLFManager (void);

char* gui_GetIconFile(const char* rdr_name, char* file, char* _szValue);
BOOL gui_LoadIconRes(HDC hdc, const char* rdr_name, char* file);

void gui_WndRect(HWND hWnd, PRECT prc);
void gui_WndClientRect(HWND hWnd, PRECT prc);

/* Undisclosed APIs */
HDC GetSecondarySubDC (HDC secondary_dc, HWND hwnd_child, BOOL client);
void ReleaseSecondarySubDC (HDC secondary_subdc);
HDC CreateMemDCFromSurface (struct GAL_Surface* surface);
struct GAL_Surface* GetSurfaceFromDC (HDC hdc);

/* Since 4.2.0 */
#ifdef _MGRM_PROCESSES
struct _SemSetManager;
typedef struct _SemSetManager SemSetManager;

SemSetManager* __mg_create_sem_set_manager (int semid, int nr_sems);
void __mg_delete_sem_set_manager (int code, void* manager);
int __mg_alloc_mutual_sem (SemSetManager* manager, int *semid);
int __mg_free_mutual_sem (SemSetManager* manager, int sem_num);
#endif /* _MGRM_PROCESSES */

/* Since 4.2.0 */
#ifdef _MGSCHEMA_COMPOSITING
BOOL mg_InitCompositor (void);
void mg_TerminateCompositor (void);
void __mg_composite_dirty_znodes (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_INTERNALS_H

