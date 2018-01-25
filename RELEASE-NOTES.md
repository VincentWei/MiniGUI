# Release Notes

## Version 3.2.0

The MiniGUI development team announces the availability of MiniGUI 3.2.0.
All users of MiniGUI are recommended strongly to use this version for new
MiniGUI apps. Please report any bugs and incompatibilities in

    https://github.com/VincentWei/minigui. 

### What's new in this version

  * Support for 64-bit platform. The definitions of some types and APIs
    changed. 
  * Eliminate all compilation warnings.

### Type changes

  1. Handle types. All handle types, including `GHANDLE`, `HWND`, `HDC`, and so on,
     are now defined as aliases of `PVOID` (`typedef void* PVOID`). 
     You may need to check your code to reflect this change. 

  2. Integer types:
     * The type of `DWORD` now has the pointer precision. That is,
     the size of `DWORD` will be 4 bytes on 32-bit platform, and 8 bytes on 
     64-bit platform. 

     * Similarly, `WPARAM` and `LPARAM` are now have the pointer precision.

     * `WORD` and `SWORD` has a half of pointer precision. The size of these 
        two types is 2 bytes on 32-bit platform, and 4 bytes on 64-bit platform.

  3. New integer types:
     * `DWORD32` and `SWORD32`: We introduce `DWORD32` and `SWORD32` types, which have
       the size of 4 bytes on both 32-bit and 64-bit platforms. You should use these
       types when reading/writing 32-bit integer from a binary files for the
       portibility. Of course, you can still use Uint8, Uint16, Uint32, and Uint64 types.

     * `LRESULT`: this type is defined for window callback procedure, and it has
       the pointer precision.


### API changes

### Configuration option changes

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
  * [NEW] ToUnicode/ToAscii: Converts the key code between unicode and ASCII.
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
  * [NEW] ChildWindowFromPointEx: Retrives a handle to the child window that contains the
            speicified point and meets the certain criteria.
  * [NEW] ScrollWindowEx: Scrolls the content of a window's client area.
  * [ADJUST] GetIMEPos/SetIMEPos: inline functions -> normal functions.
  * [ADJUST] WindowFromPoint: normal function -> inline function.
  * [ADJUST] ChildWindowFromPoint: normal function -> inline function.
  * [ADJUST] ScrollWindow: normal function -> inline function.

