# Release Notes

## Version 3.2.0

The MiniGUI development team announces the availability of MiniGUI 3.2.0.
All users of MiniGUI are recommended strongly to use this version for new
MiniGUI apps. Please report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui. 

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
        
        
  
