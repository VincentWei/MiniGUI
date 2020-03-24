# Release Notes

- [Version 5.0.0](#version-500)
   + [What's new in version 5.0.0](#whats-new-in-version-500)
   + [Compositing schema](#compositing-schema)
   + [New main window styles](#new-main-window-styles)
   + [Virtual window](#virtual-window)
   + [Other enhancements](#other-enhancements)
   + [Other new APIs](#other-new-apis)
   + [Changes leading to incompatibility](#changes-leading-to-incompatibility)
   + [Deprecated APIs](#deprecated-apis)

## Version 5.0.0

The MiniGUI development team announces the availability of MiniGUI 5.0.0,
which is the first official release of MiniGUI 5.0.x.

This version brings us some new and exciting features, and we did our best
to ensure backward compatibility of the existed APIs so that the old
applications can smoothly migrate to the new version.
We recommend that you use this version and report any bugs and
incompatibilities in

<https://github.com/VincentWei/minigui/tree/rel-5-0>

### What's new in version 5.0.0

In the development of version 5.0.x, we introduced the following new and
exciting features for MiniGUI:

* ENHANCEMENTS:
   - Support for compositing schema under MiniGUI-Processes runtime mode.
     This feature brings the exciting visual effects which are popular
     on modern desktop computers or smart phones to MiniGUI.
   - New main window types/levels. You now can easily create main windows
     in different z-order levels. This enhancement allows us to create
     a special app which acts as screen lock, docker, or launcher.
   - Virtual Window. You now can easily create message threads under all
     runtime modes to exploit the messaging mechanism of MiniGUI in
     non GUI threads - we call them message threads.
   - Enhanced timer support. MiniGUI now manages the timers per message thread.
     Under MiniGUI-Threads runtime mode, you can set up 32 (64 on 64-bit
     architecture) timers for each GUI threads. If you enabled virtual window,
     you can also do this for each message thread.
   - Support for listening a file descriptor as long as the underlying system
     has the `select()` system call for all runtime modes. Now you can call
     `RegisterListenFD()` to register a file descriptor to be listened, and
     handle `MSG_FDEVENT` in your window callback procedure to read/write
     from/to the file descriptor. Before this version, this feature only
     available for MiniGUI-Processes runtime mode.
   - Support for local data of windows. You can now set or retrieve a local data
     which is bound with a string name for a window. This will give you an
     eays-to-use interface to manage various data of a window.
   - Support for hardware cursors under compositing schema. MiniGUI now can
     utilize the hardware cursors if your graphics device support it.
     You can also load a cursor from a PNG file.
   - Support for loading icon from a bitmap file. You can now load an icon
     from a bitmap file such as a PNG file.
   - Unified the message hook functions for all runtime modes. MiniGUI now
     provides the consistent message hook functions for all runtime modes.
   - Use the update regions for cumulative updating the screen. This will
     eliminate the flickers due to the frequently redrawing of controls.
* ADJUSTMENTS:
   - `g_rcScr` now is defined a macro calling function `GetScreenRect()`.
   - `mgIsServer` now is define a macro calling function `IsServer()`.
* TUNNING
   - Tune the `drm` (DRM) engine to support MiniGUI-Processes runtime mode
     and compositing schema.
   - Tune the `fbcon` (Linux Frame Buffer) engine to support compositing schema.
   - Tune the `commlcd` (common LCD) engine to support cumulative updating and
     compositing schema.
   - Tune the `dummy` GAL engine to support MiniGUI-Processes runtime mode.
     compositing schema.
   - Fix some bugs.
* CLEANUP:
   - Cleaned up a lot of internal symbols (the external functions and
     global variables) in order to avoid name polution.
   - Refactored the code for the following modules: z-order management,
     message queue, event/message handling, and part of graphics abstract
     layer.

### Compositing schema

In this version, we enhanced the MiniGUI-Processes runtime mode to support
the compositing schema. Under compositing schema, regardless a main window
is created by the server (`mginit`) or a client, it renders the content in
a separate rendering buffer, and the server composites the contents from
all visible main windows to the ultimate scan-out frame buffer according to
the z-order information.

On the contrary, the legacy schema of MiniGUI-Processes uses the same
frame buffer for all processes (and all main windows) in the system.
So the legacy schema is also called the shared frame buffer schema.

Note that we often call a rendering buffer as a `surface`.

MiniGUI Core implements a compositor called 'fallback' as the built-in
compositor, which composites the contents of all windows in the classical
overlapped way.

But you can implement your own compositor by writing your
own server, i.e., `mginit`. You can also implement your customized compositor
in a shared library which can be loaded by MiniGUI Core dynamically.

By enabling the compositing schema, MiniGUI now provides a better
implementation for multi-process environment:

- Easy to implement advanced user interfaces with rounded corners, shadows,
  alpha blending, blurring, and so on.
- Easy to implement animations for switching among main windows.
- Better security. One client created by different user cannot
  read/write contents in/to another windows owned by other clients.

The major flaws of the compositing schema are as follow:

- It needs larger memory than the legacy schema to show multiple
  windows at the same time. Therefore, we may need a client manager
  to kill a client which runs in background and in full screen mode
  if you are running MiniGUI on an embedded system, like Android
  or iOS does.
- It may need a hardware-accelerated NEWGAL engine to get a smooth
  user experience.

#### Compile-time configuration for compositing schema

- Use `--enable-compositing` to enable the compositing
  schema when you configure the runtime mode of MiniGUI as
  MiniGUI-Processes (`--with-runmode=procs`).
- Use `--disable-compositing` to disable the compositing schema
  and enable the legacy schema (the shared frame buffer schema).

Note that, the compositing schema only works under MiniGUI-Processes runtime
mode.

#### Runtime configuration for compositing schema

```
[compositing_schema]
# The size of wallpaper pattern surface (the fake screen for clients).
# Optional values: <w>x<h>, full, half, quarter, octant, and empty.
# Default value is empty.
wallpaper_pattern_size=full
# wallpaper_pattern_size=half
# wallpaper_pattern_size=quarter
# wallpaper_pattern_size=octant
# wallpaper_pattern_size=empty
# wallpaper_pattern_size=32x32
compositor=my_compositor.so
```

As mentioned above, when using the compositing schema, the client processes
can not access the ultimate scan out frame buffer. However, MiniGUI provides
a graphics device context called `HDC_SCREEN` for apps. In order to provide
a backward compatibility, we implement the `HDC_SCREEN` as a special surface
which can be shared among all processes under MiniGUI-Processes runtime mode,
and one compositor can use the contents in this surface to render the wallpaper.

We call the special surface as the wallpaper pattern. You can specify the
size via the runtime configuration key `compsoting_schema.wallpaper_patter_size`.
All contents you rendered by using `HDC_SCREEN` in your applications will
appears in the shared surface ultimately. And the compositor can use the contents
in the shared surface to show a wallpaper or just ignore it.

The key in the runtime configuration `compsoiting_schema.compsoitor` specify
the shared library to load as the default compositor. If it is not specified, or
failed to load it, MiniGUI will use the built-in compositor: the fallback
compositor.

The fallback compositor implement a group of basic compositing actions. It renders
the contents of all visible main windows in the classical overlapped way in their
intrinsic z-order information. It keeps as simple as possible, so it does not
implement the following features:

- no border shadows.
- no support for complex compositing types, e.g., blurred.
- no any visual animations.

#### New APIs for compositing schema

In order to use the compositing schema under MiniGUI-Processes runtime mode,
we introduce some new APIs for the app:

- `CreateMainWindowEx2`: This function is an extension of `CreateMainWindowEx`.
  It creates a main window by using the legacy create information and the
  specified compositing type, the compositing argument, the surface type,
  the background color, and returns the handle to the new main window:
   + The compositing type: one of `CT_OPAQUE`, `CT_COLORKEY`,
     `CT_ALPHACHANNEL`, `CT_ALPHAPIXEL`, `CT_BLURRED`, or other compositing
     types defined by a customized compositor. By using this argument, you
     specify how the contents in a main window will be composited to the
     screen.
   + The compositing argument: one DWORD value. You generally pass a color
     for this argument. For example, for the compositing type `CT_COLORKEY`,
     you need to use this argument to tell the compositor the color acts
     as the key.
   + The surface type: you can specify the new main window uses a different
     surface type instead of the one same as the screen. Here the surface type
     mainly means the pixel format of the surface. For example, on a screen
     with RGB656 pixel format, if you want to use the compositing type
     `CT_ALPHAPIXEL`, you need to create a surface with type `ST_PIXEL_ARGB8888`
     or `ST_PIXEL_ARGB4444`.
   + The background color in DWORD representation. When you use a surface type
     other than `ST_PIXEL_DEFAULT`, you need this argument to pass the background
     color of the main window in a DWORD value. This is because that
     you can only pass a pixel value in the default screen surface type via
     the legacy create information structure (`MAINWINCREATE`).
- `SetMainWindowCompositing`: This function sets the compositing type and
  the compositing argument of a main window. By using this function,
  you can change the compositing type and the argument of a main window on the fly.

MiniGUI defines the following built-in compositing types:

- `CT_OPAQUE`: The main window is opaque. This is the default compositing type
  if you create a main window by calling legacy `CreateMainWindow` and
  `CreateMainWindowEx` functions.
- `CT_COLORKEY`: Use a specific color as the transparency key when composting
  the contents of the main window to the screen. You should specify
  the color along with the compositing argument in a DWORD representation.
- `CT_ALPHACHANNEL`: Use a specific alpha channel value when compositing the
  contents of the main window to the screen.
- `CT_ALPHAPIXEL`: Use the alpha component of the rendering buffer when
  composting the contents of the main window.
- `CT_BLURRED`: Apply a Gaussian blur to the contents blew of the main window.
  Generally, when using this composting type, the alpha component of the pixels
  will go into effect.

Note that MiniGUI allows a customized compositor to define new compositing types.

If you want to develop a new compositor, you may need the following new APIs:

- `ServerRegisterCompositor`: Register a compositor.
- `ServerUnregisterCompositor`: Unregister a compositor.
- `ServerSelectCompositor`: Select a compositor.
- `ServerGetCompositorOps`: Get the operations of a specific compositor.

As mentioned before, MiniGUI will try to load the default compositor defined
by the runtime configuration key `compositing_schema.compositor` first.
MiniGUI will call a stub called `__ex_compositor_get` in the shared library to
get the pointer to the compositor operation structure for the default compositor,
and select the compositor as the current compositor.

When implementing your customized compositor, you may need the following APIs
to get the z-order information and the information of a z-order node:

- `ServerGetNextZNode` or `ServerGetPrevZNode`: travels the z-order nodes.
- `ServerGetWinZNodeHeader` and `ServerReleaseWinZNodeHeader`: get/lock and
  release a z-node of a main window.
- `ServerGetPopupMenusCount`, `ServerGetPopupMenuZNodeHeader`, and
  `ServerReleasePopupMenuZNodeHeader`: get/lock and release a z-order node
  of pop-up menus.
- `ServerGetPopupMenuZNodeRegion` and `ServerGetWinZNodeRegion`:
- `ServerSetWinZNodePrivateData` and `ServerGetWinZNodePrivateData`:
- `ServerSetPopupMenuZNodePrivateData` and `ServerGetPopupMenuZNodePrivateData`:

By using the information returned by the functions above and the basic GDI
functions of MiniGUI, you can easily implement a customized compositor.

Note that:

- A compositor always runs in the server, i.e., `mginit`. A client of
  MiniGUI-Processes can not call these functions.
- A compositor always compositing the contents from z-nodes to the special DC
  called `HDC_SCREEN_SYS`. This DC is the only one represents the ultimate
  screen under compositing schema.
- You should call `SyncUpdateDC (HDC_SCREEN_SYS);` when you need to update the
  rendering result to the screen.
- You can use a third-party graphics library like Cairo, OpenGL, OpenGL ES, or
  mGPlus to render the contents in your customized compositor.

You can refer to the source code of the fallback compositor for the usage of
the functions above:

```
minigui/src/kernel/compsor-fallback.c
```

### New main window styles

In this version, we also enhanced the window manager of MiniGUI Core
to support some special main window types.

Before 5.0.0, you can create a topmost main window with the style
`WS_EX_TOPMOST` in order to show the main window above all normal main windows,
and if you use MiniGUI-Processes runtime mode, the server (`mginit`) will
always create global main windows, which are shown on other main windows
created by clients.

Since 5.0.0, we introduce a concept of z-order levels for main windows.
There are eight levels in MiniGUI from top to bottom:

- The tooltip level (`WS_EX_WINTYPE_TOOLTIP`).
- The system/global level (`WS_EX_WINTYPE_GLOBAL`).
- The screen lock level (`WS_EX_WINTYPE_SCREENLOCK`).
- The docker level (`WS_EX_WINTYPE_DOCKER`).
- The higher level (`WS_EX_WINTYPE_HIGHER`).
- The normal level (`WS_EX_WINTYPE_NORMAL`).
- The launcher level (`WS_EX_WINTYPE_LAUNCHER`).
- The desktop or wallpaper.

We use new extended styles like `WS_EX_WINTYPE_GLOBAL` to create main windows
in different levels. For historical reasons, you can still use the legacy style
`WS_EX_TOPMOST`, but MiniGUI will create a main window in the higher
level for this style.

By default, without the style `WS_EX_TOPMOST` or a style like
`WS_EX_WINTYPE_GLOBAL`, MiniGUI will create a main window in
the normal level.

The main windows in the desktop level are managed by MiniGUI.
Any MiniGUI process instance has a virtual desktop window. The desktop
window is an internal object, so no API is provided for app to create
or manage the desktop window.

Note that, under MiniGUI-Processes runtime mode, only the first client
creates the first main window in a z-order level other than higher and normal
levels can create another main window in the same z-order level. And only
the server can create a main window in the global z-order level.

This is a security design for the multi-process runtime environment.

In this version, we also introduce a new extended style called
`WS_EX_AUTOPOSITION`.

If a main window has this extended style when creating it, MiniGUI will
determine the position in the screen for the main window. If the width
or the height of the window specified in `MAINWINCREATE` structure is zero,
MiniGUI will also determine a default size for the main window.

Under the compositing schema, the compositor is responsible to calculate
the position and the size for a main window.

The new `WS_ALWAYSTOP` style can be used to pin a main window on
the top of other main windows in the same z-order level.

### Virtual window

You know that we can post or send a message to other windows which
may run in another thread under MiniGUI-Threads. The MiniGUI
messaging functions such as `PostMessage()`, `SendMessage()`,
`SendNotifyMessage()`, and the window callback procedure
provide a flexible, efficient, safe, and flexible data transfer
and synchronization mechanism for your multithreaded applications.

For example, you can send or post a message to a window from a
general purpose thread which may download a file from a remote
server under MiniGUI-Threads.

But can we use the MiniGUI messaging mechanism under
MiniGUI-Processes and MiniGUI-Standalone runtime modes for
multithreading purpose? For example, we may download a file in a
general thread and inform a window when the file is ready.

Furthermore, if we want to use the MiniGUI messaging mechanism in
a general thread to handle messages from other threads, how to do this?

The virtual window provides a solution for the requirements above.
A virtual window is a special window object which does not have
a visible window area. But after you create a virtual window in
a different thread, you can use the MiniGUI messaging mechanism
to post or send messages between the current main window thread
and the new thread.

In MiniGUI, we call a thread creating a main window as a GUI thread,
and a thread creating a virtual window as a message thread.

It is important to know the following key points about virtual
window:

- It is enabled automatically under MiniGUI-Threads runtime mode.
- It can be enabled by using the compile-time configuration option
  `--enable-virtualwindow`, or define `_MGHAVE_VIRTUAL_WINDOW` macro
  under MiniGUI-Processes and MiniGUI-Standalone runtime modes.
- You can create multiple GUI threads under MiniGUI-Threads, but you
  cannot create multiple GUI threads under MiniGUI-Processes and
  MiniGUI-Standalone runtime modes. In other words, there is only one
  GUI thread (the main thread) under MiniGUI-Processes and
  MiniGUI-Standalone runtime modes.
- Regardless of the runtime mode, you can create multiple message
  threads, and you can also create multiple virtual windows in
  one message thread.
- It is possible to create a virtual window in a GUI thread, although
  we do not encourage to do this.
- Essentially, a virtual window is a simplified main window.
  It consumes very little memory space, but provides a complete
  MiniGUI messaging mechanism for a general multithreaded app.
- When virtual window is enabled (or under MiniGUI-Threads runtime mode),
  you can use the MiniGUI messaging facilities to post or send messages
  to a window, or notify a window from a general thread.

A virtual window will get the following system messages in its life
life-cycle:

 - `MSG_CREATE`: this message will be sent to the virtual window when
   you call \a `CreateVirtualWindow` function.
 - `MSG_CLOSE`: this message will be sent to the virtual window when
   the system asks to close the virtual window.
 - `MSG_DESTROY`: this message will be sent to the virtual window when
   the system tries to destroy the virtual window, or after you
   called \a `DestroyVirtualWindow` function.
 - `MSG_IDLE`: When there is no any message in the message queue, all
   virtual windows living in the message thread will get this idle
   message.
 - `MSG_TIMER`: When a timer expired after you call `SetTimer` to
   set up a timer for a virtual window.
 - `MSG_QUIT`: quit the message loop.
 - `MSG_GETTEXT`: To query the caption of the virtual window.
 - `MSG_SETTEXT`: To set the caption of the virtual window.
 - `MSG_GETTEXTLENGTH`: To query the caption length of the virtual window.
 - `MSG_FDEVENT`: Send to the window procedure when there is a read/write/except
   event on a listened file descriptor.

You can call `DefaultVirtualWinProc` in your window procedure for a virtual
window for the default handling of the messages above.

A virtual window has the following properties:

- The additional data and the additional data 2.
- The identifier in a LINT value.
- The notification callback procedure.
- The caption.
- The local data.

Therefore, the following APIs can be called for a virtual window:

- `DefaultWindowProc`
- `GetWindowId`
- `SetWindowId`
- `GetThreadByWindow`
- `GetWindowAdditionalData`
- `SetWindowAdditionalData`
- `GetWindowAdditionalData2`
- `SetWindowAdditionalData2`
- `GetClassName`: always returns `VIRTWINDOW` for a virtual window.
- `GetWindowCallbackProc`
- `SetWindowCallbackProc`
- `GetWindowCaption`
- `SetWindowCaption`
- `GetWindowTextLength`
- `GetWindowText`
- `SetWindowText`
- `GetNotificationCallback`
- `SetNotificationCallback`
- `SetWindowLocalData`
- `GetWindowLocalData`
- `RemoveWindowLocalData`
- `RegisterEventHookWindow`
- `UnregisterEventHookWindow`
- `RegisterKeyHookWindow`
- `RegisterMouseHookWindow`
- `RegisterListenFD`
- `UnregisterListenFD`

Like a main window, when you want to create a virtual window, you call
`CreateVirtualWindow`, and when you wan to destroy a virtual window, you call
`DestroyVirtualWindow`. You must call `VirtualWindowCleanup` to cleanup the
system resource used by the virtual window after done with it, e.g., after
quitting the message loop.

### Other enhancements

#### Window identifier

Before 5.0.0, MiniGUI only provides the APIs to retrieve a control based on
the identifier. Since 5.0.0, you can calling the following APIs on a
main window or a virtual window on the basis of identifier:

- `GetWindowId`: return the identifier of a specific window.
- `SetWindowId`: set the identifier of a specific window.

Note that all main windows and/or virtual windows in a thread form a window tree.
The root window of the tree may be `HWND_DESKTOP` or the first main/virtual
window created in the thread. You can call `GetRootWindow` to retrieve the
root window of the current thread.

You can travel the window tree by calling the old API `GetNextHosted`. Since 5.0.0,
you can retrieve a hosted main window or virtual window via a specific identifier
by calling `GetHostedById` function.

#### Local data of a window

Local data of a window are some void objects represented in DWORD values, and they
are bound with different string names. In a window's life cycle, you can set, get,
or remove a local data which is bound a specific name. This provides a easy-to-use
way to manage multiple and complex objects of a window.

- `SetWindowLocalData`: set a local data.
- `GetWindowLocalData`: get a local data.
- `RemoveWindowLocalData`: remove a local data.

Note that all local data will be removed automatically when you destroy a window.

#### Hardware cursor

Under the compositing schema, MiniGUI now can use the hardware cursor to show
the mouse pointer. And you can use the following APIs to load a PNG file as
the cursor:

- `LoadCursorFromPNGFile`
- `LoadCursorFromPNGMem`

For a GAL engine which supports hardware cursors, it needs to implement the
following new methods:

- `AllocDumbSurface`/`FreeDumbSurface`: allocate/free a dumb surface.
  Note that each hardware cursor uses a specific dumb surface. Here, a `dumb`
  surface is a term introduced by the DRM driver of Linux. It means that
  the surface does not provide any hardware acceleration functions.
- `SetCursor`: Set a dumb surface as the cursor, along with the hotspot of
  of the cursor.
- `MoveCursor`: Move the cursor to a new position.

#### Loading an icon from bitmap files

As described before, you can create a main window to use a special surface type
under compositing schema. That is, the surface for the main window will have
a different pixel type from the screen. In other words, it is not compatible
with the screen. Therefore, we have to change some internal implementation
to reflect this enhancement. One of the changes is the icon of a window.

Since 5.0.0, an icon for a main window is always stored in a ARGB8888 surface,
so that it can be rendered correctly into the surface of any main window.

We also introduced some new APIs to load an icon from a PNG file or other bitmap
files:

- `LoadBitmapIconEx`, `LoadBitmapIconFromFile`, and `LoadBitmapIconFromMem`.

#### Loading system bitmaps for private surface

Another change is about the system bitmaps. The system bitmaps are often used
by a look-and-feel renderer to render the caption bar or the border of
a main window or a control. For a main window which may have a different surface
type under compositing schema, one L&F renderer can not use the system bitmaps
loaded by `GetSystemBitmapEx` function. Because the function only loads the
system bitmaps for `HDC_SCREEN`.

Therefore, we introduce a new function to load the system bitmaps:

- `GetSystemBitmapEx2`. This function will load the system bitmaps for
   the specific device context.

We have tuned the built-in look and feel renderers to use the new function
to load the system bitmaps. If you want to use a customized look and feel
renderer under compositing schema, you must change the code to use the new
function to load the system bitmaps.

#### Unified event hook functions

- `RegisterEventHookFunc`
- `RegisterEventHookWindow` and `UnregisterEventHookWindow`

#### Listening file descriptor under all runtime modes

Since 5.0.0, MiniGUI provides support for listening a file descriptor
as long as the underlying system has the `select()` system call for all
runtime modes. Now you can call `RegisterListenFD()` to register a
file descriptor to be listened, and handle `MSG_FDEVENT` in your window
callback procedure to read/write from/to the file descriptor.

Before this version, this feature only available for MiniGUI-Processes
runtime mode.

By using this feature, you can listen and handle a file descriptor in
a message thread by using the virtual window. This is a powerful mechanism
for a multithreaded application.

Note that MiniGUI no longer limits the max number of listening file descriptors,
MiniGUI will try to allocate space to manage all listening file descriptors.

#### Enhanced timer

Since 5.0.0, MiniGUI manages the timers per message thread.
Under MiniGUI-Threads runtime mode, you can set up 32 (64 on 64-bit
architecture) timers for each GUI threads. If you enabled virtual window,
you can also do this for each message thread.

The function `IsTimerInstalled` checks the timers installed for the current
thread, and the function `HaveFreeTimer` also checks the free timer slots
for the current thread.

### Other new APIs

For MiniGUI-Processes runtime mode:

- `IsServer`: Under MiniGUI-Processes runtime mode, return whether the process
   is the server (`mginit`) or a client.
- `MoveToLayer`: Called by a client to move itself to a specific layer. .
- `ServerMoveClientToLayer`: The server version of `MoveToLayer`.
- `ServerSendReplyEx`: The extended version of legacy `ServerSendRely`. It can
   transfer a file descriptor between the server and the client.
- `RegisterRequestHandlerV1`: To register a request handler in version 1. The
   request handler can handle the file descriptor received from the client.
- `GetRequestHandlerV1`: Get the registered request handler in version 1.
- `GetRequestHandlerEx`: Get the registered request handler and its version.

Global scope:

- `GetScreenRect`: Return a RECT as the screen rectangle.

For MiniGUI-Threads or when virtual window enabled:

- `GetThreadByWindow`: Return the thread identifier of a window.
- `IsWindowInThisThread`: Determine whether a window is created by the current
   thread.
- `IsVirtualWindow`: Determine whether a window is a virtual window.
- `CreateThreadForMessaging`: Create a thread for messaging.

For messaging mechanism:

- `SendPriorNotifyMessage`: Send a prior notification message. Generally, a
   notification message is put at the tail of the message list. This function
   put the notification message at the head of the list.
- `NotifyWindow`: Send a notification message to a specific window.
- `BroadcastMessageInThisThread`: Broadcast a message in the current thread.
- `PreDefVirtualWinProc`: The pre-defined window procedure for a virtual window.
- `DefaultVirtualWinProc`: The default window procedure for a virtual window.

- `GetDCEx`: A extended version of `GetDC` and `GetClientDC`.
- `GetEffectiveCDC`: Get a effective device context for painting a window.
- `GetDCInSecondarySurface`: Get a device content for a main window or a control
   in the secondary surface if the main window has set the secondary DC.
- `DWORD2PixelByWindow`: Convert a DWORD color to pixel value for a window..
- `AreRegionsIntersected`: Determine whether two regions are intersected.
- `SyncUpdateSurface`: Synchronize the update rectangles of the backing surface of
   a window to screen.
- `SyncUpdateDC`: Synchronize the update rectangles of the surface corresponding to
   a DC to screen.
- `LoadBitmapEx2`: The extended version of legacy `LoadBitmapEx` function. You can
   specify a callback to allocate the buffer for the MiniGUI bitmap loader.

- `MSG_MOVEWINDOW`: This message will be sent as a notification after calling
  `MoveWindow`.

### Changes leading to incompatibility

#### Look and feel renderer

As we mentioned before. If you want to use a customized look and feel
renderer under compositing schema, you must change the code to use the new
function `GetSystemBitmapEx2` to load the system bitmaps. For example, the
old code:

```
const BITMAP* radio_bmp;

radio_bmp =
    GetSystemBitmapEx (__mg_wnd_rdr_classic.name, SYSBMP_RADIOBUTTON);
```

should be revised:

```
const BITMAP* radio_bmp;

radio_bmp =
    GetSystemBitmapEx2 (hdc, __mg_wnd_rdr_classic.name, SYSBMP_RADIOBUTTON);
```

For the same reason, you should always call `GetWindowElementPixelEx` to
get the pixel value of a window element for a specific device context.

Fortunately, MiniGUI has passed the correct device context (`hdc`) for all
operations of a L&F renderer. So it is every easy to revise the code to use
the new function.

#### `WNDCLASS`

In order to support the private surface of a main window under compositing
schema, the `WNDCLASS` structure to register a control class also changed.

Since 5.0.0, the field `iBkColor` only available for shared frame buffer
schema. Under compositing schema, you must use `dwBkColor` field to
specify the background color for a control.

This introduces a source code incompatibility, you should change
you code with a conditional compilation statement block:

```
#ifdef _MGSCHEMA_COMPOSITING
     MyClass.dwBkColor   = RGBA_lightwhite;
#else
     MyClass.iBkColor    = PIXEL_lightwhite;
#endif
```

Note that the macros with the prefix `RGBA_` are new constants introduced for
the standard colors in DWORD representation. While the legacy macros with the
prefix `PIXEL_` give the pixel values for the screen.

#### `mgIsServer`

We make `mgIsServer` to be a macro calling `IsServer`. That is, `mgIsServer`
is no longer a global variable. Generally, this change will not break the source
code compatibility.

#### `g_rcScr`

Before version 5.0.0, `g_rcScr` is a global variable of `RECT` structure.
Since 5.0.0, we define it as a macro calling `GetScreenRect`:

```
RECT GUIAPI GetScreenRect (void);

#define g_rcScr (GetScreenRect())
```

This change breaks the source code compatibility. The compiler will complain
if you use `&g_rcScr`:

```
    RECT rc;
    CopyRect (&rc, &g_rcScr);
```

To fix it, you need to change the source code in this way:

```
    RECT rc = g_rcScr;
```

Note that the legacy `g_rcDesktop` symbol is also defined as the macro calling
`GetScreenRect()` function.

#### `HDC_SCREEN`

Some code use `HDC_SCREEN` to determine the screen resolution by calling
function `GetGDCapability`, or draw some things directly to the screen.
This will not work under MiniGUI-Processes with the compositing schema.

As described before, under MiniGUI-Processes with compositing schema,
`HDC_SCREEN` stands for a global shared surface for wallpaper pattern.
This surface is the ONLY surface that can be accessed by all processes
(including the server and all clients) under compositing schema.

This surface will have the same pixel format as the real screen.
Therefore, one app can still use `HDC_SCREEN` to create a compatible
memory DC, load bitmaps, or draw something to the surface. However,
the content in the wallpaper surface will not be reflected to
the whole screen; the compositor decides how to display the contents
in it.

On the other hand, you can configure MiniGUI to create a smaller
surface than the whole screen as the underlying surface of `HDC_SCREEN`,
and the compositor may use it as a pattern to tile the content
to the whole wallpaper.

Because of the change of `HDC_SCREEN`'s connotation, you should avoid
to use \a `GetGDCapability` to determine the screen's resolution.
Instead, you use the macro `g_rcScr` or the new function `GetScreenRect()`.

Note that, you can still use `HDC_SCREEN` under a runtime mode other than
MiniGUI-Processes, and MiniGUI-Processes with the shared frame buffer schema.

#### `WINDOWINFO` structure

The `WINDOWINFO` structure changed, all legacy members were reserved, but
the order changed. Do not assume the first field of the structure is the
position of the window.

### Deprecated APIs

The following APIs are deprecated:

- `InitVectorialFonts` and `TermVectorialFonts`, no need to call these functions.
- `GetWindowElementColor` and `GetWindowElementColorEx`,
  use `GetWindowElementPixelEx` instead.
- `SetServerEventHook`, use `RegisterEventHookFunc` instead.
- `GetMainWinThread`, use `GetThreadByWindow` instead.
- `WaitMainWindowClose`, use system `pthread_join` instead.
- `MainWindowThreadCleanup`, use `MainWindowCleanup` instead.

Note that the functions are marked as deprecated, but you can still use them.
We recommend that you no longer use the functions in new applications.

[HybridOS]: https://github.com/FMSoftCN/hybridos
[HybridOS Foundation Class Library]: https://github.com/FMSoftCN/hybridos/tree/dev/device-side/hfcl
[CSS Text Module Level 3]: https://www.w3.org/TR/css-text-3/
[CSS Writing Modes Level 3]: https://www.w3.org/TR/css-writing-modes-3/

