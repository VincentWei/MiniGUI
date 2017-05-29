/*
** $Id: coolbar_impl.h 8810 2007-12-26 05:17:04Z xwyan $
**
** Copyright (c) 2003 ~ 2007 Feynman Software
** Copyright (c) 2001 ~ 2002 Wei Yongming and others.
**
** Create date: 2000/9/20
*/


#ifndef __COOLBAR_IMPL_H_
#define __COOLBAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LEN_HINT    50
#define LEN_TITLE   10

typedef struct coolbarCTRL
{
    int    	nCount;
    int 	ItemWidth;
    int		ItemHeight;
    PBITMAP BackBmp;
    struct 	coolbarItemData* head;
    struct 	coolbarItemData* tail;
    struct 	coolbarCTRL* nline;     // control linked list next
    int 	iSel; 				    // selected selected pic's insPos
    int 	iMvOver;
    BOOL    ShowHint;
    HWND    hToolTip;
	// add end
 }COOLBARCTRL;
typedef COOLBARCTRL* PCOOLBARCTRL;

typedef struct coolbarItemData
{
      RECT      RcTitle;            // title and where clicked
      int       hintx, hinty;       // position of hint box
      int       id;                 // id
      int       insPos;
      int	    ItemType;	
      BOOL	    Disable;
      PBITMAP   Bmp;
      char      Hint [LEN_HINT + 1];
      char      Caption[LEN_TITLE + 1];
      HMENU	    ItemMenu; 
      struct    coolbarItemData* next; //page linked list next
} COOLBARITEMDATA;
typedef  COOLBARITEMDATA* PCOOLBARITEMDATA;

BOOL RegisterCoolBarControl (void);
#ifdef  __cplusplus
}
#endif

#endif // GUI_COOBAR_IMPL_H_


