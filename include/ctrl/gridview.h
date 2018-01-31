/**
 * \file gridview.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: gridview.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef EXT_GRIDVIEW_H
#define EXT_GRIDVIEW_H


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_grid GridView control
     * @{
     */
/**
 * \def CTRL_GRIDVIEW
 * \brief The class name of gridview control.
 */
#define CTRL_GRIDVIEW               "gridview"

#define GVS_COLFIRST                0x0000
#define GVS_ROWFIRST                0x0001

/**
 * \def GRID_OKAY
 * \brief Gridview return value.
 */
#define GRID_OKAY                   0

/**
 * \def GRID_ERR
 * \brief Gridview return value.
 */
#define GRID_ERR                    (-1)

/** Struct of the whole grid control */
typedef struct _GRIDVIEWDATA
{
    /** The number of the rows */
    int nr_rows;
    /** The number of the columns */
    int nr_cols;
    /** The default height of a row */
    int row_height;
    /** The default width of a column */
    int col_width;
} GRIDVIEWDATA;

/** The alignment of the cell */
/** Column text horizontal left align, default */
#define GV_HALIGN_LEFT                          0x00000001
/** Column text horizontal right align */
#define GV_HALIGN_RIGHT                         0x00000002
/** Column text horizontal center align */
#define GV_HALIGN_CENTER                        0x00000003
/** Column text vertical top align */
#define GV_VALIGN_TOP                           0x00000010
/** Column text vertical bottom align */        
#define GV_VALIGN_BOTTOM                        0x00000020
/** Column text vertical center align */        
#define GV_VALIGN_CENTER                        0x00000030

/** Mask of horizontal align type */
#define GV_HALIGN_MASK                          0x0000000F

/** Mask of vertical align type */
#define GV_VALIGN_MASK                          0x000000F0

/** The type of the unknown cell, use unknow type to query cell type */
#define GV_TYPE_UNKNOWN                         0x00000000
/** The text type of the cell */
#define GV_TYPE_TEXT                            0x00000100
/** The digit type of the cell */
#define GV_TYPE_NUMBER                          0x00000200
/** The checkbox type of the cell */
#define GV_TYPE_CHECKBOX                        0x00000300
/** The type of the cell having selection */
#define GV_TYPE_SELECTION                       0x00000400
/** The cell type of the row header or column header */
#define GV_TYPE_HEADER                          0x00000500
/** The cell type of the cell */
#define GV_TYPE_TABLEHEADER                     0x00000600

/** Mask of grid cell type */
#define GV_TYPE_MASK                            0x0000FF00

/** A style of the cell, which has a icon bitmap */
#define GVS_WITHICON                            0x00010000
/** A style of the cell, which contains a checkbox */
#define GVS_WITHCHECKBOX                        0x00020000

/** A style of the cell, which text is multiple line*/
#define GVS_MULTLINE                          0x00100000
/** A status of the cell, which is read only*/
#define GVS_READONLY                            0x00200000
/** A status of the cell which has a checkbox and the checkbox is checked */
#define GVS_BOXCHECKED                          0x00400000

#define GVITEM_STYLE                            0x0001
#define GVITEM_BGCOLOR                          0x0002
#define GVITEM_FGCOLOR                          0x0004
#define GVITEM_FONT                             0x0008
#define GVITEM_IMAGE                            0x0010
#define GVITEM_MAINCONTENT                      0x0020 
#define GVITEM_SIZE                             0x0040

#define GVITEM_ALLCONTENT                       0x00FF

/** Struct of the selected cell(s) */
typedef struct _GRIDCELLS
{
    /** The start row of the selected cell(s) */
    int row;
    /** The start column of the selected cell(s) */
    int column;
    /** The number of the column(s) which contain(s) selected cell(s) */
    int width;
    /** The number of the row(s) which contain(s) selected cell(s) */
    int height;
}GRIDCELLS;

typedef int (*GRIDCELLEVALCALLBACK)(GRIDCELLS* target, GRIDCELLS* source, DWORD dwAddData);

typedef struct _GRIDCELLDEPENDENCE
{
    /** The dependence of source cell */
    GRIDCELLS source;
    /** The dependence of target cell */
    GRIDCELLS target;
    /** The callback function of one special cell */
    GRIDCELLEVALCALLBACK callback;
    /** The addition data of cell */
    DWORD dwAddData;
}GRIDCELLDEPENDENCE;

/** Struct of the header type cell */
typedef struct _GRIDCELLDATAHEADER
{
    /** The height of a row or the width of a column */
    int size;
    /** The caption of the row or column */
    char* buff;
    /** The length of the buff string */
    int len_buff;
}GRIDCELLDATAHEADER;

/** Struct of the text type cell */
typedef struct _GRIDCELLDATATEXT
{
    /** The caption of the row or column */
    char* buff;
    /** The length of the buff string */
    int len_buff;
}GRIDCELLDATATEXT;

/** Struct of the number type cell */
typedef struct _GRIDCELLDATANUMBER
{
    /** The value of the number type cell*/
    double number;
    /** The format of the number to display */
    char* format;
    /** The length of the format string */
    int len_format;
}GRIDCELLDATANUMBER;

/** Struct of the checkbox type cell */
typedef struct _GRIDCELLDATACHECKBOX
{
    /** Whether this checkbox is checked */
    BOOL checked;
    /** The text after checkbox */
    char* text;
    /** When used for set, -1 means null-terminated */
    int len_text;
}GRIDCELLDATACHECKBOX;

/** Struct of the selection type cell */
typedef struct _GRIDCELLDATASELECTION
{
    /** The index of the combobox that current selected */
    int cur_index;
    /** "Yes\nNo\n"*/
    char* selections;
    /** When used for set, -1 means null-terminated */
    int len_sel;
}GRIDCELLDATASELECTION;

#if 0
/** struct of the evaluation type cell */
typedef struct _GRIDCELLDATAEVAL
{
    /** the value of the evaluation type cell */
    double number;
    /** the evaluation of one special cell */
    GRIDCELLS sourcecells;
    /** the callback function of one special cell */
    GRIDCELLEVALCALLBACK eval_callback;
    /** the format of the number to display */
    char* format;
    /** the length of the format string */
    int len_format;
}GRIDCELLDATAEVAL;
#endif

/** Struct of the cell */
typedef struct _GRIDCELLDATA
{
    /** Mask of properties, can be OR'ed with following values:
     * Set or get a cell style
     * - GVITEM_STYLE\n
     * Set or get a cell text color 
     * - GVITEM_FGCOLOR\n
     * Set or get a cell background color 
     * - GVITEM_BGCOLOR\n
     * Set or get a cell text font 
     * - GVITEM_FONT\n
     * Set or get a cell's image 
     * - GVITEM_IMAGE\n
     * Set or get all of the content of a cell
     * - GVITEM_ALLCONTENT\n
     * Set or get the main content of a cell
     * - GVITEM_MAINCONTENT\n
     * Set or get the header size
     * - GVITEM_SIZE
     */
    DWORD mask;

    /** The style of the cell */
    DWORD style;
    /** Text color */
    gal_pixel color_fg;
    /** The background color */
    gal_pixel color_bg;
    /** Text font */
    PLOGFONT font;
    /** Pointer to the bitmap of one cell */
    PBITMAP image;
    /** The concrete data of one cell */
    void* content;
}GRIDCELLDATA;

    /** \defgroup mgext_control_grid_msgs Messages of grid control
     * @{
     */

/**
  * \def GRIDM_SETCELLPROPERTY
  *  
  * \brief An Application sends a GRIDM_SETCELLPROPERTY message to set the 
  * cells' property in the grid control. 
  *
  * \code
  * GRIDM_SETCELLPROPERTY
  * GRIDCELLS* cells;
  * GRIDCELLDATA* celldata;
  *
  * wParam = (WPARAM)cells; // The cell(s) which you want to set it's(their) property
  * lParam = (LPARAM)celldata; // The pointer of the property you want to set. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  * \sa GRIDCELLDATA
  */
#define  GRIDM_SETCELLPROPERTY    0xF210

/**
  * \def GRIDM_GETCELLPROPERTY
  * \brief An Application sends a GRIDM_GETCELLPROPERTY message to get a single 
  * cell's property in the grid control.
  *
  * \code
  * GRIDM_GETCELLPROPERTY
  * GRIDCELLS* cells;
  * GRIDCELLDATA* celldata;
  *
  * wParam = (WPARAM)cell; // The cell(only for one cell now) which you want to get it's property
  * lParam = (LPARAM)celldata; // The pointer of the property you want to get. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  * \sa GRIDCELLDATA
  */

#define  GRIDM_GETCELLPROPERTY    0xF211

/**
  * \def GRIDM_SETCOLWIDTH
  *
  * \brief An Application sends a GRIDM_SETCOLWIDTH message to set column width in the grid control.
  *
  * if (lParam < 1 or lParam > nCols) , it will do nothing. nCows is the columns' count of the grid control.
  *
  * \code
  * GRIDM_SETCOLWIDTH
  * int index;
  * int width;
  *
  * wParam = (WPARAM)index; // The index of the column which you want to set width.
  * lParam = (LPARAM)width; // The width of the column you want to set. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_SETCOLWIDTH       0xF212

/**
  * \def GRIDM_GETCOLWIDTH
  *
  * \brief An Application sends a GRIDM_GETCOLWIDTH message to get a column width in the grid control.
  *
  * if (lParam < 1 or lParam > nCols) , it will do nothing. nCows is the columns' count of the grid control.
  *
  * \code
  * GRIDM_GETCOLWIDTH
  * int index;
  *
  * wParam = 0; 
  * lParam = (LPARAM)index; // The index of the column which you want to get width.
  * \endcode
  *
  * \return  The width of the column on success, otherwise -1.
  */
#define  GRIDM_GETCOLWIDTH       0xF213 

/**
  * \def GRIDM_SETROWHEIGHT
  *
  * \brief An Application sends a GRIDM_SETROWHEIGHT message to set a row height in the grid control.
  *
  * if (lParam < 1 or lParam > nRows) , it will do nothing. nRows is the rows' count of the grid control.
  *
  * \code
  * GRIDM_SETROWHEIGHT
  * int index 
  * int height;
  *
  * wParam = (WPARAM)index; // The index of the row which you want to set height.
  * lParam = (LPARAM)height; //The height of the row you want to set. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_SETROWHEIGHT      0xF214

/**
  * \def GRIDM_GETROWHEIGHT
  *
  * \brief An Application sends a GRIDM_GETROWHEIGHT message to get a row height in the grid control.
  *
  * if (lParam < 1 or lParam > nRows) , it will do nothing. nRows is the rows' count of the grid control.
  *
  * \code
  * GRIDM_GETROWHEIGHT
  * int index ;
  *
  * wParam = 0 
  * lParam = (LPARAM)index // The index of the row which you want to get height.
  * \endcode
  *
  * \return  The height of the row on success, otherwise -1.
  */
#define  GRIDM_GETROWHEIGHT      0xF215

/**
  * \def GRIDM_ADDROW
  *
  * \brief An Application sends a GRIDM_ADDROW message to add a new row to the control.
  *
  * if (wParam > nRows) , it will do nothing. nRows is the rows' count of the grid control.
  *
  * \code
  * GRIDM_ADDROW
  * int index;
  * GRIDCELLDATA* celldata;
  * 
  * wParam = (WPARAM)index; // The index of the row which you want to add after it, zero base.
  * lParam = (LPARAM)celldata; // The property which is setted to the new row. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_ADDROW         0xF216

/**
  * \def GRIDM_DELROW
  *
  * \brief An Application sends a GRIDM_DELROW message to delete a row frow the control.
  *
  * if (wParam < 1 or wParam > nRows) , it will do nothing. nRows is the rows' count of the grid control.
  *
  * \code
  * GRIDM_DELROW
  * int index;
  *
  * wParam = 0; 
  * lParam = index; // The index of the row which you want to delete.
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_DELROW         0xF217
/**
  * \def GRIDM_ADDCOLUMN
  *
  * \brief An Application sends a GRIDM_ADDCOLUMN message to add a new column to the control.
  *
  * if (wParam > nCols) , it will do nothing. nCols is the columns' count of the grid control.
  *
  * \code
  * GRIDM_ADDCOLUMN
  * int index;
  * GRIDCELLDATA* celldata;
  * 
  * wParam = (WPARAM)index; // The index of the column which you want to add after it, zero base.
  * lParam = (LPARAM)celldata; // The property which is setted to the new column. 
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_ADDCOLUMN      0xF218
/**
  * \def GRIDM_DELCOLUMN
  *
  * \brief An Application sends a GRIDM_DELCOLUMN message to delete a column frow the control.
  *
  * if (wParam < 1 or wParam > nCols) , it will do nothing. nCols is the columns' count of the grid control.
  *
  * \code
  * GRIDM_DELCOLUMN
  * int index;
  *
  * wParam = 0;
  * lParam = (LPARAM)index; // The index of the column which you want to delete.
  * \endcode
  *
  * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
  */
#define  GRIDM_DELCOLUMN      0xF219
/**
 * \def GRIDM_GETROWCOUNT
 * \brief Gets the number of all rows in the grid control.
 *
 * An application sends an GRIDM_GETROWCOUNT message to get the number of all rows in the grid control.
 *
 * \code
 * GRIDM_GETROWCOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The rows number on success, otherwise -1.
 */
#define  GRIDM_GETROWCOUNT    0xF21A
/**
 * \def GRIDM_GETCOLCOUNT
 * \brief Gets the number of all columns in the grid control.
 *
 * An application sends an GRIDM_GETCOLCOUNT message to get the number of all columns in the grid control.
 *
 * \code
 * GRIDM_GETCOLCOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The cols number on success, otherwise -1.
 */
#define  GRIDM_GETCOLCOUNT    0xF21B
/**
 * \def GRIDM_SETNUMFORMAT
 * \brief Sets the number format of a cell in the grid control. the format is
 * just same as format in c printf.
 *
 * An application sends an GRIDM_SETNUMFORMAT message to
 * set the number format of a cell in the grid control.
 *
 * \code
 * GRIDCELLS cells;
 * char* format 
 *
 * GRIDM_SETNUMFORMAT
 *
 * wParam = (WPARAM)cells; // The cells which you want to set.
 * lParam = (WPARAM)format; // The cells' number format, like "%1.2f";
 * \endcode
 *
 * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
 */
#define  GRIDM_SETNUMFORMAT   0xF21C

/**
 * \def GRIDM_SETSELECTED
 * \brief Sets the selected cell(s) in the grid control.
 *
 * An application sends an GRIDM_SETSELECTED message to
 * set the selected cell(s) in the grid control.
 *
 * \code
 * GRIDCELLS* cells
 *
 * GRIDM_SETSELECTED
 *
 * wParam = 0;
 * lParam = (WPARAM)cells; // The cell(s) which you want to set selected.
 * \endcode
 *
 * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
 */
#define  GRIDM_SETSELECTED    0xF21D

/**
 * \def GRIDM_GETSELECTED
 * \brief Gets the selected cell(s) in the grid control.
 *
 * An application sends an GRIDM_GETSELECTED message to
 * get the selected cell(s) in the grid control.
 *
 * \code
 * GRIDCELLS* cells
 *
 * GRIDM_GETSELECTED
 *
 * wParam = 0;
 * lParam = (WPARAM)cells;  // Get the selected cell(s) to it.
 * \endcode
 *
 * \return Returns GRID_OKAY if successful, or GRID_ERR otherwise.
 */
#define  GRIDM_GETSELECTED    0xF21E

/**
 * \def GRIDM_ADDDEPENDENCE
 * \brief Adds a dependence relationship of cells to another cells.
 *
 * An application sends an GRIDM_ADDDEPENDENCE message to
 * add a dependence relationship of cells to another source cells,
 * when the source cells changed, the target cells could updated automatically.
 *
 * \code
 * GRIDCELLDEPENDENCE* dependence
 *
 * GRIDM_ADDDEPENDENCE
 *
 * wParam = 0;
 * lParam = (WPARAM)dependence;  // Dependence relationship for adding.
 * \endcode
 *
 * \return Returns a dependence id if successful, or GRID_ERR otherwise.
 */
#define  GRIDM_ADDDEPENDENCE    0xF21F

/**
 * \def GRIDM_DELDEPENDENCE
 * \brief Deletes a dependence relationship of cells to another cells.
 *
 * An application sends an GRIDM_DELDEPENDENCE message to
 * delete a dependence relationship of cells to another source cells.
 *
 * \code
 * int dependence_id
 *
 * GRIDM_ADDDEPENDENCE
 *
 * wParam = (WPARAM)dependence_id;
 * lParam = 0.
 * \endcode
 *
 * \return Returns GRID_OKAY, or GRID_ERR otherwise.
 */
#define  GRIDM_DELDEPENDENCE    0xF220

/**
 * \def GRIDM_ENDCELLEDIT
 * \brief This message is used inside minigui.
 */
#define  GRIDM_ENDCELLEDIT      0xF221

/** @} end of mgext_ctrl_grid_msgs */

    /**
     * \defgroup mgext_ctrl_grid_ncs Notification code of Grid control
     * @{
     */

/**
 * \def GRIDN_HEADLDOWN
 * \brief This notification code informs a application when the left mouse button down
 *        on the gridview.header.
 */
#define  GRIDN_HEADLDOWN       1
/**
 * \def GRIDN_HEADLUP
 * \brief This notification code informs a application when the left mouse button up
 *        on the gridview.header.
 */
#define  GRIDN_HEADLUP         2
/**
 * \def GRIDN_KEYDOWN
 * \brief This notification code informs a application when that a key has been pressed.
 */
#define  GRIDN_KEYDOWN         3
/**
 * \def GRIDN_CELLDBCLK
 * \brief This notification code informs a application when the current selected cell 
 *        has be double clicked.
 */
#define  GRIDN_CELLDBCLK       4
/**
 * \def GRIDN_CELLCLK
 * \brief This notification code informs a application when the current selected cell 
 *        has been clicked.
 */
#define  GRIDN_CELLCLK         5
/**
 * \def GRIDN_FOCUSCHANGED
 * \brief This notification code informs a application when the current selected item 
 *        has changed.
 */
#define  GRIDN_FOCUSCHANGED    6
/**
 * \def GRIDN_CELLTEXTCHANGED
 * \brief This notification code informs a application when the cell text
 *        has changed.
 */
#define  GRIDN_CELLTEXTCHANGED 7

    /** @} end of mgext_ctrl_grid_ncs */

    /** @} end of mgext_ctrl_grid */

    /** @} end of controls */

#ifdef  __cplusplus
}
#endif

#endif /* EXT_GRIDVIEW_H */

