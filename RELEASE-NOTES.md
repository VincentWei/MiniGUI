# Release Notes

## Version 3.0.13

The MiniGUI development team announces the availability of MiniGUI 3.0.13 RC1.
This release is the first Release Candidate for 3.0.13. All users of MiniGUI are
encouraged to test this version carefully, and report any bugs and incompatibilities
in [the bug tracking system](http://bugs.minigui.org) (see soon). 

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

