
#ifndef LF_COMMON_H
  #define LF_COMMON_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define CHECKNOT_RET_ERR(cond) \
        if (!(cond)) return -1

#define CHECK_RET_VAL(cond, val) \
        if (cond) return val

#define IS_BORDER_VISIBLE(win_info) \
            ((win_info)->dwStyle & WS_BORDER || \
             (win_info)->dwStyle & WS_THINFRAME || \
             (win_info)->dwStyle & WS_THICKFRAME) 

#define IS_CAPTION_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_CAPTION)

#define IS_MINIMIZEBTN_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_CAPTION && \
         (win_info)->dwStyle & WS_MINIMIZEBOX)

#define IS_MAXIMIZEBTN_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_CAPTION && \
         (win_info)->dwStyle & WS_MAXIMIZEBOX)

#define IS_CAPICON_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_CAPTION && (win_info)->hIcon)

#define IS_CLOSEBTN_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_CAPTION && \
        !((win_info)->dwExStyle & WS_EX_NOCLOSEBOX))

#define IS_MENUBAR_VISIBLE(win_info) ((win_info)->hMenu)

#define IS_VSCROLL_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_VSCROLL &&  \
         !((win_info)->vscroll.status & SBS_HIDE))

#define IS_HSCROLL_VISIBLE(win_info) \
        ((win_info)->dwStyle & WS_HSCROLL &&  \
         !((win_info)->hscroll.status & SBS_HIDE))

#define IS_LEFT_VSCOLLBAR(win_info) \
        ((win_info)->dwExStyle & WS_EX_LEFTSCROLLBAR)

#define DWINDLE_RECT(rc) \
    do \
    { \
        (rc).left++;      \
        (rc).top++;       \
        (rc).right--;     \
        (rc).bottom--;    \
    }while (0)

#define gui_UnLoadIconRes(hdc, rdr_name, id) {                      \
    char file[MAX_NAME+ 1] = "icon/";                               \
    char* filename = file + strlen(file);                           \
    int len = sizeof(file)-(filename-file);                         \
    if (GetMgEtcValue (rdr_name, id, filename, len) < 0 )           \
        return -1;                                                     \
    ReleaseRes(Str2Key(file));                                      \
}

extern char* GetIconFile(const char* rdr_name, char* file, char* _szValue);
extern BOOL LoadIconRes(HDC hdc, const char* rdr_name, char* file);
extern BOOL wndGetHScrollBarRect (const MAINWIN* pWin, RECT* rcHBar);
extern BOOL wndGetVScrollBarRect (const MAINWIN* pWin, RECT* rcVBar);

static int 
get_window_border (HWND hWnd, int dwStyle, int win_type);

static int calc_we_metrics (HWND hWnd, 
            LFRDR_WINSTYLEINFO* style_info, int which);

static inline int lf_get_win_type (HWND hWnd)
{
    if(IsDialog(hWnd))
        return LFRDR_WINTYPE_DIALOG;
    else if(IsMainWindow(hWnd))
        return LFRDR_WINTYPE_MAINWIN;
    else if(IsControl(hWnd))
        return LFRDR_WINTYPE_CONTROL;

    return LFRDR_WINTYPE_UNKNOWN;
}

static inline int get_window_caption (HWND hWnd)
{
    return calc_we_metrics (hWnd, NULL, LFRDR_METRICS_CAPTION_H);
}

static inline int get_window_menubar (HWND hWnd)
{
    return calc_we_metrics (hWnd, NULL, LFRDR_METRICS_MENU_H);
}

static inline int get_window_scrollbar (HWND hWnd, BOOL is_vertival)
{
    if (is_vertival) {
        return calc_we_metrics (hWnd, NULL, LFRDR_METRICS_VSCROLL_W);
    }
    else {
        return calc_we_metrics (hWnd, NULL, LFRDR_METRICS_HSCROLL_H);
    }
}

static inline void erase_bkgnd (HWND hWnd, HDC hdc, const RECT *rect)
{
    gal_pixel old_color;

    if (hWnd != HWND_NULL)
        old_color = SetBrushColor (hdc, GetWindowBkColor (hWnd));
    else
        old_color = SetBrushColor (hdc, 
                GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP));

    FillBox(hdc, rect->left, rect->top, RECTWP(rect), RECTHP(rect));
    SetBrushColor (hdc, old_color);
}

BOOL gui_fill_box_with_bitmap_part_except_incompatible (HDC hdc,
    int x, int y, int w, int h, int bw, int bh, const BITMAP * 	bmp, int xo, int yo);

BOOL gui_fill_box_with_bitmap_except_incompatible (HDC hdc,
    int x, int y, int w, int h, const BITMAP *  bmp);

/* For trackbar control */
/* use these definition when drawing trackbar */
#define TB_BORDER               2

#define WIDTH_HORZ_SLIDER       24
#define HEIGHT_HORZ_SLIDER      12

#define WIDTH_VERT_SLIDER       12
#define HEIGHT_VERT_SLIDER      24

#define WIDTH_VERT_RULER        6
#define HEIGHT_HORZ_RULER       6
/* please keep it even for good appearance */
#define LEN_TICK                4
#define GAP_TICK_SLIDER         6

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* LF_COMMON_H */
