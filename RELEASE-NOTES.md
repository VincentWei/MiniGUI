# Release Notes

## Version 4.0.0

The MiniGUI development team announces the availability of MiniGUI 4.0.0.
All users of MiniGUI are recommended strongly to use this version.
Please report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui

### What's new in this version

In this version, we mainly enhanced and tuned the APIs of font rendering.

#### Tunning the logical font APIs

The styles of LOGFONT changed.

* Add new rendering style:
  1. `FS_RENDER_ANY`: Not specified
  1. `FS_RENDER_MONO`:
  1. `FS_RENDER_GREY`:
  1. `FS_RENDER_SUBPIXEL`:
* Some old styles are deprecated:
  1. `FS_WEIGHT_BOOK`; use `FS_RENDER_GREY` instead.
  1. `FS_WEIGHT_SUBPIXEL`; use `FS_RENDER_SUBPIXEL` instead.
* Introduced or redefined the weight styles to follow OpenType specification:
  1. `FONT_WEIGHT_ANY`: Not specified
  1. `FONT_WEIGHT_THIN`: Thin.
  1. `FONT_WEIGHT_EXTRA_LIGHT`: Extra light (Ultra Light).
  1. `FONT_WEIGHT_LIGHT`: Light.
  1. `FONT_WEIGHT_REGULAR`: Regular (Normal).
  1. `FONT_WEIGHT_MEDIUM`: Medium.
  1. `FONT_WEIGHT_DEMIBOLD`: Demi Bold (Semi Bold)
  1. `FONT_WEIGHT_BOLD`: Bold.
  1. `FONT_WEIGHT_EXTRA_BOLD`: Extra Bold (Ultra Bold).
  1. `FONT_WEIGHT_BLACK`: Black (Heavy).
* Introduce the new decoration styles and replace 
`FONT_UNDERLINE_LINE` and `FONT_STRUCKOUT_LINE` with them:
  1. `FONT_DECORATE_ANY`: Not specified.
  1. `FONT_DECORATE_NONE`: None.
  1. `FONT_DECORATE_UNDERLINE`: glyphs are underscored.
  1. `FONT_DECORATE_STRUCKOUT`: glyphs are overstruck.
  1. `FONT_DECORATE_US`: Both `FONT_DECORATE_UNDERLINE` and `FONT_DECORATE_STRUCKOUT`.
  1. `FONT_DECORATE_OUTLINE`: Outline (hollow) glyphs.
  1. `FONT_DECORATE_REVERSE`: Reverved for future. Glyphs have their foreground and background reversed.
* The following style are deprecated:
  1. `FONT_OTHER_LCDPORTRAIT`
  1. `FONT_OTHER_LCDPORTRAITKERN`

For a new app, you should use the new function `CreateLogFontEx` to
create a LOGFONT, and specify the weight and rendering method of the glyph.

For the back-compatibility, you can still use `CreateLogFont` to create a new
LOGFONT. However, `FS_WEIGHT_BOOK` will be treated `FS_WEIGHT_REGULAR` and
`FS_RENDER_GREY`, while `FS_WEIGHT_SUBPIXEL` will be treated
`FS_WEIGHT_REGULAR` and `FS_RENDER_SUBPIXEL`.

You can still use `CreateLogFontByName` to create a new LOGFONT.
But the style string in the font name changed from

    <weight><slant><flipping><other><underline><struckout>

to

    <weight><slant><flipping><other><decoration><rendering>

Note that `<underline>` and `<struckout>` are merged to `<decoration>`
in order to keep the style string is still 6-character long.

Consequently, if you want to use the rendering method SUPIXEL for a TTF font,
please define the logical font name in the following way:

    ttf-Courier-rrncns-*-16-UTF-8

Moreover, the family name of a DEVFONT supports aliases since 4.0.0:

    <fonttype>-<family[,aliase]*>-<styles>-<width>-<height>-<charset[,charset]*>

for example:

    ttf-Arial,Sans Serif-rrncnn-8-16-ISO8859-1,UTF-8
    ttf-courier,monospace,serif-rrncnn-8-16-ISO8859-1,UTF-8

Note that the length of one DEVFONT name can not exceed 255 bytes.

#### Changed APIs

* The fields `height` and `descent` have been removed from GLYPHINFO struct.
One should get the font metrics information by calling `GetFontMetrics` function
if you want to get the height and descent data of one font.

* More fields added for GLYPHBITMAP struct in order to return the completed
rasterized glyph bitmap information.

* `GetGlyphInfo` now can return the basic glyph type and break type of a
UNICODE character. If the glyph is an ASCII character, the old `MCHAR_TYPE`
can also be returned.

* Rename `mg_FT_LcdFilter` to `FT2LCDFilter` in order to follow MiniGUI naming
rules.

* Rename `UChar32` to `Uchar32` and `UChar16` to `Uchar16` in order to
avoid the conflict with typedef of UChar32 in 'unicode/umachine.h'.

* Redefine `Uchar32` and `Glyph32` as `Uint32` instead of `int`.

#### New APIs

* `GetGlyphsByRules` function calculates and allocates the glyph string from
a multi-byte string under the specified white space rule and transformation
rule.

* `GetGlyphsExtentPointEx` function gets the visual extent information
of a glyph string which can fit in a line with the specified maximal extent.

* `DrawGlyphStringEx` function draws a glyph string to the specific
positions of a DC.

* `CreateLogFontIndirectEx` creates a new LOGFONT by using the value of
an existed LOGFONT but with a new rotation value.

#### Others

* Support for FreeType1 removed.

You should always use FreeType2 to support vector fonts, such as TrueType
fonts (TTF), TrueType collections (TTC), OpenType fonts (OTF, both TrueType
and CFF variants), OpenType collections (OTC), and Type 1 fonts (PFA and PFB).

### Other Changes

* A new BITMAP type: `BMP_TYPE_REPLACEKEY`. When `bmType` of a BITMAP object
has this bit set, any pixel which is equal to `bmColorKey` will be replaced by
`bmColorRep`.

## Version 3.2.1

The MiniGUI development team announces the availability of MiniGUI 3.2.1.
All users of MiniGUI are recommended strongly to use this version.
Please report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui

### What's new in this version

* ENHANCEMENTS:
  1. Add a new key `dpi` for NEWGAL engine to define the DPI of the screen.
     If it is absent, use 96 as the default DPI.
  1. Add an item for `GetGDCapability` to return DPI of the DC.
  1. New API `InitSlaveScreenEx` to specify the DPI of slave screen.
     Define `InitSlaveScreen` as an inline function calling `InitSlaveScreenEx`.
  1. New API: `SyncUpdateDC`. You can use this function to synchronize
    the update rectangles of a surface to screen, if the surface
    represents the shadow frame buffer of the screen.
    * configure option: `--enable-syncupdate`
    * macro: `_MGUSE_SYNC_UPDATE`
  1. New API: `UpdateInvalidClient`. You can use this function to update
    the invalid client region of a window instantly.
  1. Use different colors for the output of `_DBG_PRINTF` and `_ERR_PRINTF`.
  1. Add `__mg_save_jpg` function for storing MYBITMAP as JPEG file (@10km).
  1. Modified logic for checking JPEG format (@10km).
  1. Support BIDI for UNICODE charsets and cleanup the implementation.
    * New API: `GetGlyphBidiType` to get the glyph type in BIDI.
    * New DC attribute: BIDI flag.
  1. Enhance commlcd engint to support more pixel type and synchronously update.
  1. New USVFB IAL engine and NEWGAL engine for web display server.
  1. New type: `QDWORD` for a quauter of DWORD. This type is 16-bit long on
64-bit architecture, and 8-bit long on 32-bit.
  1. New macros for QDWORD:
    * `MAKEDWORD`: Make a DWROD from four QDWORDs.
    * `FIRST_QDWORD`: get the first (LSB) QDWORD from a DWORD.
    * `SECOND_QDWORD`: get the second (LSB) QDWORD from a DWORD.
    * `THIRD_QDWORD`: get the third (LSB) QDWORD from a DWORD.
    * `FOURTH_QDWORD`: get the fourth (LSB) QDWORD from a DWORD.

* BUGFIXING:
  1. handle `PNG_COLOR_TYPE_GRAY_ALPHA` color type of PNG files.
  1. Fix a bug to free a null pointer (ReleaseDC).
  1. No need to make the pitch of FT2 monobitmap is single-byte aligned.
     This bug may generate dirty dots for monobitmap glyph from TTF.
  1. Skip null pixels for SUBPIXEL glyphs. This bug will always show background
     pixels of one SUBPIXEL glyph.
  1. Fix the bug of wrong bounding box handling for SUBPIXEL rendering of glyph.

* TUNNING:
  1. Tune GLYPHINFO structure and GetGlyphInfo to return BIDI glyph type.

## Version 3.2.0

The MiniGUI development team announces the availability of MiniGUI 3.2.0.
All users of MiniGUI are recommended strongly to use this version for new
MiniGUI apps. Please report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui

### What's new in this version

  * Support for 64-bit platform. Note that the definitions of some types
    and APIs changed.
  * Compliant to the latest GCC, and eliminate all compilation warnings.

### Type changes

#### Changes of handle types

All handle types, including `GHANDLE`, `HWND`, `HDC`, etc.,
are now defined as aliases of `PVOID` (`typedef void* PVOID`).
You may need to check your code to reflect this change.

#### Changes of integer types.

The type of `DWORD` now has pointer precision. That is,
the size of `DWORD` will be 4 bytes on 32-bit platform, and 8 bytes on
64-bit platform.

Similarly, `WPARAM` and `LPARAM` now have pointer precision.

`WORD` and `SWORD` has a half of pointer precision. The size of these
two types is 2 bytes on 32-bit platform, and 4 bytes on 64-bit platform.

`RGBCOLOR` now is defined as an alias of `DWORD32` (see below).

Note that the type of `BYTE` always has the size of 8-bit on both
32-bit and 64-bit platforms.

#### New integer types:

We introduce `DWORD32` and `SDWORD32` types,
which have the size of 4 bytes on both 32-bit and 64-bit platforms.
You should use these types when reading/writing 32-bit integers from
a binary files for the portability. Of course, you can also use
`Uint32` or `Sint32` types.

Similarly, we introduce `WORD16` and `SWORD16` types,
which have the size of 2 bytes on both 32-bit and 64-bit platforms.
You should use these types when reading/writing 16-bit integers from
a binary file for the portability. Of course, you can also use
`Uint16` or `SUint16` types.

`LRESULT` is defined for window callback procedure, and it has
pointer precision.

`LINT` is a new integer type with pointer precision.

### API changes

#### Integer macros

`MAKEWPARAM`: this new macro makes a WPARAM value by using four bytes.
On the contrary, `FIRSTBYTE`, `SECONDBYTE`, `THIRDBYTE`, and `FOURTH`
macros get the four bytes from a `WPARAM` or a `Uint32` value.

`MAKEWORD16`: this new macro makes a 16-bit word by using two bytes.
Meanwhile, `MAKEWORD` makes a 16-bit word on 32-bit platform, and a 32-bit
word on 64-bit platform.

Note that `MAKELONG` macro always makes a `DWORD` integer, which has pointer
precision. Meanwhile, `MAKELONG32` macro makes a `Uint32` integer.

Note that `MakeRGB` and `MakeRGBA` macros always make `DWORD32` integers.
In contract, `GetRValue`, `GetRValue`, `GetBValue`, `GetAValue` always
get red, green, blue, and alpha components from a `DWORD32` integer
respectively.

Note that you should use `(-1)` instead of `0xFFFFFFFF` for the invalid
integer or pointer type value for good portability.

#### Structure and functions

The main changes in structure and functions:

 * We now use a `UINT` instead of an `int` integer for the message identifier.

 * We now use a `DWORD` integer for the time tick count. Meanwhile, you can
    create 64 timers on 64-bit platform.

 * We now use a `LRESULT` integer for the return value of a window callback
    procedure. Now it is safe to return a pointer from the callback procedure
    on 64-bit platform. This is a very important change, and it will break the
    source compatibilty of your code. You should check the source code (use
    gcc option `-Wall`) carefully.

 * We now use a `LINT` integer for the identifier of a timer. So you can pass
    a pointer as the identifier of the timer on 64-bit platform. mGNCS uses
    MiniGUI timer in this manner.

 * We now use a `LINT` integer for the identifier of a control/widget and a
    menu item. So you can pass a pointer as the identifier of the timer on
    64-bit platform. mGNCS works in this manner.

##### Message

The strcuture `MSG` and all message-related functions changed.
For example, the prototype of `SendMessage` changed from

    int SendMessage (HWND hWnd, int nMsg, WPARAM wParam, LPARAM lParam)

to

    LRESULT SendMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)

_IMPORTANT NOTE_

For best portability, you should use `FIRSTBYTE` to `FOURTHBYTE` macros
to get the bytes of a character when you extract the bytes from `WPARAM`
parameter of a `MSG_CHAR` message:

    MSG_CHAR
    unsigned char ch_buff [4];
    unsigned char ch_buff [0] = FIRSTBYTE(wParam);
    unsigned char ch_buff [1] = SECONDBYTE(wParam);
    unsigned char ch_buff [2] = THIRDBYTE(wParam);
    unsigned char ch_buff [3] = FOURTHBYTE(wParam);

##### Window callback procedure

Furthermore, the structure and functions to register window class,
create main window, and create dialog box changed. For example, the prototype
of `WNDPROC` changed from

    typedef int (* WNDPROC)(HWND, int, WPARAM, LPARAM)

to

    typedef LRESULT (* WNDPROC)(HWND, UINT, WPARAM, LPARAM)

Therefore, the prototype of `DefaultWindowProc` changed from

    int DefaultWindowProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)

to

    LRESULT DefaultWindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

All main window procedures, control class procedures, and dialog box procedures
defined by your app should change the implementation to reflect the changes
above.

_IMPORTANT NOTE_

Do not cast the result returned by a window procedure to `int` on 64-bit
platform, unless you know what your are doing.

##### Notification callback procedure

The type of notification callback changes from:

    typedef void (* NOTIFPROC) (HWND hwnd, int id, int nc, DWORD add_data);

to

    typedef void (* NOTIFPROC) (HWND hwnd, LINT id, int nc, DWORD add_data);

_IMPORTANT NOTE_

If you use `MSG_COMMAND` message to handle the notification sent from children
controls, you should make sure the identifier is small enough on 64-bit
platform. Because MiniGUI packs the identifier and the notification code
in the `WPARAM` parameter:

    MSG_COMMAND
    int id = LOWORD(wParam);
    int code = HIWORD(wParam);
    HWND hwnd = (HWND)lParam;

The code above will not work on 64-bit if you use a pointer as the identifier
of the control.

Therefore, we recommend strongly that you use a `NOTIFYPOROC` callback to
handle the notification sent from controls. To do this, please call
`SetNotificationCallback` function to set the notification callback function.

##### Time and timer

The prototype of `GetTickCount` changed from

    unsigned int GetTickCount (void)

to

    DWORD GetTickCount (void);

And the prototye of `TIMERPROC` changed from

    typedef BOOL (* TIMERPROC)(HWND, int, DWORD)

to

    typedef BOOL (* TIMERPROC)(HWND, LINT, DWORD)

##### Miscellaneous

In addition, we correct the bad or wrong definitions of some APIs:

  * `DWORD2PIXEL` to `DWORD2Pixel`. The old one has a bad name.
  * `GetWindowRendererFromName`: The return type changes from
    `const WINDOW_ELEMENT_RENDERER*` to `WINDOW_ELEMENT_RENDERER*`.
    So you can overload some methods directly of a renderer.
  * `GetDefaultWindowElementRenderer`: The return type changes from
    `const WINDOW_ELEMENT_RENDERER*` to `WINDOW_ELEMENT_RENDERER*`.
    So you can overload some methods directly of the default renderer.

### Configuration option changes

We add some new options for autoconf script (`configure`):

  * `--with-runmode`: Now you can use this option to specify the runtime
    mode of MiniGUI. The old enable options for runmode were removed.
    Note that MiniGUI-Processes now is the default runmode.

  * `--enable-develmode`: You should use this option to define `_DEBUG` macro,
    enable `-Wall -Werror` option, and enable all features of MiniGUI,
    if you were a MiniGUI developer.

## Version 3.0.13

The MiniGUI development team announces the availability of MiniGUI 3.0.13.
All users of MiniGUI are encouraged to test this version carefully, and
report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui.

### What's new in this version

  * Merge new APIs which are introduced by miniStudio.
  * The autoconf/automake config scripts are cleaned up.
  * Remove most of compilation warnings (not used variables).
  * Remove some inline function definitions, especially GetIMEPos/SetIMEPos.
  * Fix some bugs.

### API changes

  * [NEW] CopyCursor: Copies a cursor object.
  * [NEW] GetKeyboardState: Gets status of all keys on keyboard.
  * [NEW] ToUnicode/ToAscii: Converts the key code between Unicode and ASCII.
  * [NEW] GetWindowRegion: The function obtains a copy of the window region of a window.
  * [NEW] InvalidateRegion: Invalidates the client area within the specified region.
  * [NEW] ValidateRect: Validates the client area within a rectangle by removing the
            rectangle from the update region of the specified window.
  * [NEW] ValidateRegion: Validates the client area within a region by removing the
            region from the current update region of the specified window.
  * [NEW] GetUpdateRegion: Copy the update region of a window to a region.
  * [NEW] AdjustWindowRectEx: Calculates the required size of the window rectangle
            based on the desired size of the client rectangle.
  * [NEW] WindowFromPointEx: Retrieves a handle to the window that contains the specified point.
  * [NEW] ChildWindowFromPointEx: Retrieves a handle to the child window that contains the
            speicified point and meets the certain criteria.
  * [NEW] ScrollWindowEx: Scrolls the content of a window's client area.
  * [ADJUST] GetIMEPos/SetIMEPos: inline functions -> normal functions.
  * [ADJUST] WindowFromPoint: normal function -> inline function.
  * [ADJUST] ChildWindowFromPoint: normal function -> inline function.
  * [ADJUST] ScrollWindow: normal function -> inline function.


## Version 3.0.2

The MiniGUI development team announces the availability of MiniGUI 3.0.2.
All users of MiniGUI are encouraged to test this version carefully, and
report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui.

### What's new in this version

  * Append non-rectangle window support.
  * Append support of double buffer of maindow.
  * Support for Look and Feel (LF) concept.
  * Remove SKIN interfaces of MGExt library, move MGExt library and vcongui into mGUtils component.
  * Newly added bitmap font.
  * Developing interfaces of desktop for MiniGUI.
  * Support direct draw to DC for higher efficiency.
  * Enhancement of resource management
  * Enhancement of controls: Support of independent scrollbar and transparent control.
  * Enhancement of font and character set, support bidi text support.
  * Enhancement of zoom of picture


### API changes

  * [NEW] ScaleBitmapEx: a bilinear interpolation algorithm which is better than the original.
  * [NEW] GetWindowInfo: get New structure WINDOWINFO.
  * [NEW] CreateMainWindowEx: enhance of CreeateMainWindow, compatiable with CreeateMainWindow.
  * [NEW] CreateWindowEx2：enhance of CreateWindowEx2, compatiable with CreateWindowEx2.
  * [NEW] DialogBoxIndirectParamEx: new create dialog.
  * [NEW] CreateMainWindowIndirectParamEx:new create dialog.
  * [NEW] GetWindowRendererFromName: get LF renderer by name
  * [NEW] AddWindowElementRenderer:  append a LF renderer to MiniGU
  * [NEW] RemoveWindowElementRenderer: delete a LF renderer from MiniG
  * [NEW] GetDefaultWindowElementRenderer: get default LF renderer
  * [NEW] SetDefaultWindowElementRenderer: set default LF renderer
  * [NEW] SetWindowElementRenderer: set a LF renderer for one window’s render
  * [NEW] SetWindowElementAttr: set window’s attributes.
  * [NEW] GetWindowElementAttr: get window’s attributes value
  * [NEW] CreateBMPDevFont: create an bitmap device font.
  * [NEW] AddGlyphsToBMPFont: add an bitmap font glyph.
  * [NEW] DestroyBMPFont: destroy bitmap device font.
  * [NEW] SetCustomDesktopOperationSet: set the custom desktop operation set.
  * [NEW] DesktopUpdateAllWindow:  refresh all windows on the desktop.
  * [NEW] PopupDesktopMenu: pop up a menu on the desktop
  * [NEW] GetTextAlign: get the current text-alignment flags of a DC.
  * [NEW] SetTextAlign: set text-alignment flags of a DC.
  * [NEW] SetWindowMask: set non-rectangle window mask rect.
  * [NEW] SetWindowRegion: set non-rectangle window  visible region
  * [NEW] LockDCEx: support for directly read and write the pixels in a DC.
  * [NEW] CreateSecondaryDC: Creates a secondary window DC of a window.
  * [NEW] SetSecondaryDC:set a window's secondary DC and the callback for double buffer window.
