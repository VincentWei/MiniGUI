# Release Notes

## Version 4.9.0

The MiniGUI development team announces the availability of MiniGUI 4.9.0.
This is a unstable release to test some new and exciting features.
We recommend that you test this version and report any bugs and
incompatibilities in

<https://github.com/VincentWei/minigui/tree/dev-4-1>

### What's new in this version

In the development of version 4.9.x, we introduced the following new and
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
   - Support for hardware cursors under compositing schema. MiniGUI now can
     utilize the hardware cursors if your graphics device support it.
     You can also load a cursor from a PNG file.
   - Support for loading icon from a bitmap file. You can now load an icon
     from a bitmap file such as a PNG file.
   - Unified the message hook functions for all runtime modes. MiniGUI now
     provides the consistent message hook functions for all runtime modes.
   - Use the update regions for cumulative updating the screen. This will
     emilinate the flickers due to the frequently redrawning of controls.
* ADJUSTMENTS:
   - `g_rcScr` now is defined a macro calling function `GetScreenRect()`.
   - `mgIsServer` now is define a macro calling function `IsServer()`.
* CLEANUP:
   - Cleaned up a lot of internal symbols (the external functions and
     global variables) in order to avoid name polution.
   - Refactored the code for the following modules: z-order management,
     message queue, event/message handling, and part of graphics abstract
     layer.

The following new features will be developed in the successive versions of
4.9.x:

- Enhance DRM engine to support MiniGUI-Processes runtime mode and the
  compositing schema.
- Enhance other frequently-used GAL engines to support compositing schema.
- Provide a customized compositor which provides window animation effects as
  an example in `mg-demos`.

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

MiniGUI Core implements the default compositor. But you can implement
your own compositor by writing your own server, i.e., `mginit`. You
can also implement your own compositor in a shared library which
can be loaded by MiniGUI Core dynamically.

By enabling the compositing schema, MiniGUI now provides a better
implementation for multi-process environment:

- Easy to implement advanced user interfaces with rounded corners,
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

Usage:

- Use `--enable-compositing` to enable the compositing
  schema when you configure the runtime mode of MiniGUI as
  MiniGUI-Processes (`--with-runmode=procs`).
  Use `--disable-compositing` to enable
  the legacy schema (the shared frame buffer schema).

In order to use the compositing schema on Linux-based system, we introduce
some new APIs for your new apps.

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

#### Compositing types

You can call the following function to set the compositing type and argument
of a main window:

- `CreateMainWindowEx2` creates a main window with a specific compositing type
  and argument.
- `SetMainWindowCompositing` changes the compositing type and argument of a
  main window.

MiniGUI provides you the following compositing type:

- `CT_OPAQUE`: The main window is opaque. This is the default compositing type
  if you create a main window by calling `CreateMainWindow` and
  `CreateMainWindowEx`.
- `CT_COLORKEY`: Use a specific color as the transparency key when composting
  the contents of the main window to the screen. You should specify
  the color along with the compositing argument in a RGBA quadruple value.
- `CT_ALPHACHANNEL`: Use a specific alpha channel value when compositing the
  contents of the main window to the screen.
- `CT_ALPHAPIXEL`: Use the alpha component of the rendering buffer when
  composting the contents of the main window.
- `CT_BLURRED`: Apply a Gaussian blur to the contents blew of the main window.

### New main window types

In this version, we also enhanced the window manager of MiniGUI Core
to support some special main window types.

Before 5.0.0, you can create a topmost main window with the style
`WS_EX_TOPMOST` in order to show the main window above all normal main windows,
and if you use MiniGUI-Processes runtime mode, the server (`mginit`) will
always create global main windows, which are shown on other main windows
created by clients.

Since 5.0.0, we introduce a concept of z-order levels for main windows.
There are eight levels in MiniGUI from top to bottom:

- The tooltip level. 
- The system/global level.
- The screen lock level.
- The docker level.
- The higher level.
- The normal level.
- The launcher level.
- The desktop or wallpaper.

We use new styles like `WS_EX_WINTYPE_GLOBAL` to create main windows in
different levels. For historical reasons, you can still use the style
`WS_EX_TOPMOST`, but MiniGUI will create a main window in the higher
level for this style.

By default, without the style `WS_EX_TOPMOST` or a style like
`WS_EX_WINTYPE_GLOBAL`, MiniGUI will create a main window in
the normal level.

The main windows in the desktop level are managed by MiniGUI.
Any MiniGUI process instance has a virtual desktop window. The desktop
window is an internal object, so no API is provided for app to create
or manage the desktop window.

### Virtual window

You know that we can post or send a message to other windows which
may run in another thread under MiniGUI-Threads. The MiniGUI
messaging functions such as `PostMessage()`, `SendMessage()`,
`SendNotifyMessage()`, and the window callback procedure
provide a flexible, efficient, safe, and flexible data transfer
and synchronization mechanism for your multithreaded applications.

But if we want to use the MiniGUI messaging mechanism for a general
thread without a main window, how to do this?

Furthermore, can we use the MiniGUI messaging mechanism under
MiniGUI-Processes and MiniGUI-Standalone runtime modes for
multithreading purpose?

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
 - `MSG_GETTEXT`:
 - `MSG_SETTEXT`:
 - `MSG_GETTEXTLENGTH`:

### Other enhancements

#### Hardware cursor

Under the compositing schema, MiniGUI now can use the hardware cursor to show
the cursor. And you can use the following APIs to load a PNG file as the cursor:

- `LoadCursorFromPNGFile`
- `LoadCursorFromPNGMem`

#### Loading an icon from bitmap files

#### Unified event hook functions

#### Listening file descriptor under all runtime modes

#### Enhanced timer 

### Other new APIs

- `ServerMoveClientToLayer`
- `MoveToLayer`
- `LoadBitmapEx2`
- `ServerSendReplyEx`
- `RegisterRequestHandlerV1`
- `GetRequestHandlerV1`
- `GetRequestHandlerEx`
- `IsServer`
- `GetScreenRect`

### Changes leading to incompatibility

- We make `mgIsServer` to be a macro calling `IsServer`.
- We make `g_rcScr` to be a macro calling `GetScreenRect`.

## Version 4.0.4

The MiniGUI development team announces the availability of MiniGUI 4.0.4.
We strongly recommend that you use this version for any new MiniGUI
apps, especially if the new features of MiniGUI 4.0 are must for your
new apps.

Please report any bugs and incompatibilities in

<https://github.com/VincentWei/minigui>

### What's new in this version

In this version, we mainly enhanced the DRM engine of MiniGUI in order
to integrate MiniGUI with Mesa and Cairo, for example, to implement EGL
for MiniGUI platform.

* MODIFICATIONS:
   - Change the name of old `dri` NEWGAL engine to `drm`.
   - Change the configuration option of DRM engine to `--enable-videodrm`.
   - Change the macro for DRM engine to `_MGGAL_DRM`.
   - Change the runtime configuration section for DRM engine to `drm`.
* ENHANCEMENTS:
   - New APIs for GPU integration, such as `IsMemDC`, `IsScreenDC`, `IsWindowDC`, `GetVideoHandle`, and `drmGetDeviceFD`.
   - Add new operation for DRM engine: `create_buffer_from_prime_fd`.
   - Use `dlopen` to load the external DRM driver.
   - Add a new runtime configuration key for DRM engine `drm.exdriver` to define the external DRM driver.

## Version 4.0.2

The MiniGUI development team announces the availability of MiniGUI 4.0.2.
We strongly recommend that you use this version for any new MiniGUI
apps, especially if the new features of MiniGUI 4.0 are must for your
new apps.

Please report any bugs and incompatibilities in

<https://github.com/VincentWei/minigui>

### What's new in this version

* ENHANCEMENTS:
   - Support for RTEMS operating system
   - Basic support for RT-Thread and FreeRTOS operating systems.
* BUGFIXES:
   - Fixed errors in makefile.ng files.
* CLEANUP:
   - `README.md` for correct markdown tags.

## Version 4.0.1

The MiniGUI development team announces the availability of MiniGUI 4.0.1.
We strongly recommend that you use this version for any new MiniGUI
apps, especially if the new features of MiniGUI 4.0 are must for your
new apps.

Please report any bugs and incompatibilities in

<https://github.com/VincentWei/minigui>

### What's new in this version

This version mainly fixed some minor issues:

- Use FVN algorithm to implement `Str2Key`.
- Tune some API description.
- Tune format string to use '%z' instead of '%l' or '%ll' for `size_t` values.

## Version 4.0.0

The MiniGUI development team announces the availability of MiniGUI 4.0.0.
We strongly recommend that you use this version for any new MiniGUI
apps, especially if the new features of MiniGUI 4.0.0 are must for your
new apps.

Please report any bugs and incompatibilities in

<https://github.com/VincentWei/minigui>

### What's new in this version

In this version, we mainly enhanced and tuned the APIs related to text
rendering in order that MiniGUI can handle complex writing systems
(scripts) like Arabic and Indic:

* MiniGUI now provides complete APIs for Unicode characters processing.
  These APIs conform to Unicode 12.0, including the Unicode
  Bidirectional Algorithm (UAX #9), Unicode Line Breaking Algorithm
  (UAX #14), Unicode Normalization Forms (UAX #15), Unicode Script Property
  (UAX #24), Unicode Text Segmentation (UAX #29), Unicode Vertical
  Text Layout (UAX #50), and so on.

* MiniGUI also provides new APIs to lay out, shape, and render glyphs
  from complex and mixed scripts, such as Arabic, Thai, and Indic.
  The new APIs also conform to the specifications of [CSS Text Module Level 3]
  and [CSS Writing Modes Level 3].

* We tuned and optimized MiniGUI's logical and device font interfaces to
  support the new features above.

* MiniGUI now is enhanced to support input events which may be generated
  by input devices other than standard mouse (or single-touch panel) and
  keyboard, such as multi-touch panel (gesture), joystick, tablet tool,
  and switch. In MiniGUI 4.0, we introduce `MSG_EXIN_XXX` messages to support
  the input events from devices other than standard mouse and keyboard. We
  call these messages as 'extra input messages'.

Another important features of this version are the new following engines:

* The NEWGAL engine of `dri` to support modern DRM-driven graphics cards. By using
  `dri` engine, one MiniGUI app can now use the hardware-accelerated graphics
  rendering for 2D/3D graphics.
* The IAL engine of `libinput` to support all modern input devices including
  mouse, keyboard, joystick, switch, multi-touch panel, gesture, tablet tool,
  and table pad.
* The enhanced IAL engine of `random` to generate extra input messages
  automatically for testing.

At last, we introduced a Slice Memory Allocator for fast concurrent memory
chunk allocation.

The following sections will describe these new features in detail.

#### New APIs conforming Unicode 12.0

* New types:
    1. `Uchar32`: the Unicode code point value of a Unicode character.
    1. `Achar32`: the abstract character index value under a certain
        charset/encoding. Under Unicode charset or encodings, the
        abstract character index value will be identical to the Unicode
        code point, i.e., Achar32 is equivalent to Uchar32 under this
        situation.
    1. `Glyph32`: the glyph index value in a device font. Note that
        a Glyph32 value is always bound to a specific logfont object.

* New functions to determine the Unicode character properties:
    1. `UCharGetCategory` for getting the general category of
        a Unicode character.
    1. `UCharGetBreakType` for getting the breaking type of
        a Unicode character.
    1. `UStrGetBreaks` for getting the breaking types of
        a Unicode string.
    1. `UCharGetBidiType` for getting the bidi type of
        a Unicode character.
    1. `UStrGetBidiTypes` for getting the bidi types of a Unicode
        character string.
    1. `UCharGetBracketType` for getting the bracketed character of a
        Uchar32 character.
    1. `UStrGetBracketTypes` for getting the bracketed characters of a
        Uchar32 string.
    1. `UCharGetMirror` for getting the mirrored character of a Uchar32
        character.
    1. `UCharGetJoiningType` for getting the joining type of a Uchar32
        character.
    1. `UStrGetJoiningTypes` for getting the joining types of a Uchar32
        string.
    1. `UBidiGetParagraphDir` for getting the base paragraph direction
        of a single paragraph.
    1. `UBidiGetParagraphEmbeddingLevels` for getting the bidi embedding
        levels of a paragraph.
    1. `UBidiReorderLine`, `UBidiShapeMirroring`, `UBidiJoinArabic`,
        ` UBidiShapeArabic`, and `UBidiShape` for doing bidi-aware
        mirroring, joining, and shaping.
    1. `UCharGetScriptType` for getting the script type of a Uchar32
        character.

MiniGUI also provides some utilities/helpers for Unicode character
conversion, such as from lower case to upper case, single width to
full width. Please see MiniGUI API reference document for the detailed
description.

#### New APIs for mixed scripts

To lay out, shape, and render a text in mixed scripts, you should call
`GetUCharsUntilParagraphBoundary` function first to convert
a multi-byte string to a Unicode string under the specified white space
rule, breaking rule, and transformation rule. For example, converting a
general C string in UTF-8 or GB18030 to a Uchar32 string by calling this
function. You can call `CreateLogFontForMChar2UChar` function to create
a dummy logfont object for this purpose in order to expense a minimal memory.

If the text is in simple scripts, like Latin or Chinese, you can call
`GetGlyphsExtentFromUChars` function to lay out the paragraph. This function
returns a glyph string which can fit in a line with the specified
maximal extent and rendering flags. After this, you call
`DrawGlyphStringEx` function to draw the glyph string to the
specific position of a DC.

If the text is in complex and/or mixed scripts, like Arabic, Thai,
and Indic, you should create a TEXTRUNS object first by calling
`CreateTextRuns` function, then initialize the shaping engine for
laying out the text.

MiniGUI provides two types of shaping engine. One is the basic
shaping engine. The corresponding function is `InitBasicShapingEngine`.
The other is called complex shaping engine, which is based on HarfBuzz.
The corresponding function is `InitComplexShapingEngine`. The latter
one can give you a better shaping result.

After this, you should call `CreateLayout` to create a layout object
for laying out the text, then call `LayoutNextLine` to lay out the lines
one by one.

You can render the laid out lines by calling `DrawLayoutLine` function.

Finally, you call `DestroyLayout` and `DestroyTextRuns` to destroy
the layout object and text runs object.

Before rendering the glyphs laid out, you can also call `GetLayoutLineRect`
to get the line rectangle, or call `CalcLayoutBoundingRect` to get
the bounding rectangle of one paragraph.

These new APIs provide a very flexible implementation for your apps
to process the complex scripts. The implementation is derived from
LGPL'd Pango, but we optimize and simplify the original implementation
in the following respects:

* We split the layout process into two stages. We get the text runs
  (Pango items) in the first stage, and the text runs will keep as
  constants for subsequent different layouts. In the second stage,
  we create a layout object for a set of specific layout parameters,
  and generates the lines one by one for the caller. This is useful
  for an app like browser, it can reuse the text runs if the output
  width or height changed, and it is no need to re-generate the text
  runs because of the size change of the output rectangle.

* We use MiniGUI's fontname for the font attributes of text, and leave
  the font selection and the glyph generating to MiniGUI's logfont
  module. In this way, we simplify the layout process greatly.

* We always use Uchar32 string for the whole layout process. So the
  code and the structures are clearer than original implementation.

* We provide two shaping engines for rendering the text. One is a
  basic shaping engine and other is the complex shaping engine based
  on HarfBuzz. The former can be used for some simple applications.

#### Enhanced logical font APIs

The styles of LOGFONT changed.

* Add new rendering style:
  1. `FS_RENDER_ANY`: Not specified.
  1. `FS_RENDER_MONO`:
  1. `FS_RENDER_GREY`:
  1. `FS_RENDER_SUBPIXEL`:
* Some old styles are deprecated:
  1. `FS_WEIGHT_BOOK`; use `FS_RENDER_GREY` instead.
  1. `FS_WEIGHT_SUBPIXEL`; use `FS_RENDER_SUBPIXEL` instead.
* Introduced or redefined the weight styles to follow OpenType specification:
  1. `FONT_WEIGHT_ANY`: Not specified.
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
  1. `FONT_DECORATE_STRUCKOUT`: glyphs are over-struck.
  1. `FONT_DECORATE_US`: Both `FONT_DECORATE_UNDERLINE`
and `FONT_DECORATE_STRUCKOUT`.
  1. `FONT_DECORATE_OUTLINE`: Outline (hollow) glyphs.
  1. `FONT_DECORATE_REVERSE`: Reserved for future. Glyphs have their
foreground and background reversed.
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

Since version 4.0.0, you can specify up to 7 family names for a logfont name,
such as:

    ttf-Courier,宋体,Naskh,SansSerif-rrncns-U-16-UTF-8

In this way, you can specify a logfont to use multiple devfonts
to render a complex text. This is useful when different glyphs are
contained in different font files. It is well known that, a font is
often designed for a particular language/script or a few similar
languages/scripts.

Since 4.0.0, the previous width field of a logfont name is used for
the glyph orientation:

- 'U': Glyphs stand upright (default).
- 'S': Glyphs are rotated 90 degrees clockwise (sideways).
- 'D': Glyphs are upside-down.
- 'L': Glyphs are rotated 90 degrees counter-clockwise (sideways left).

#### Support for Linux DRI/DRM

In order to support modern graphics card or GPU, we introduced a
new NEWGAL engine of `dri`. The developer can use this engine to
run MiniGUI apps on a Linux box on which the DRI
(Direct Rendering Infrastructure) is enabled.

The `dri` engine uses `libdrm` developed by Free Desktop project:

<https://dri.freedesktop.org/wiki/>

Libdrm is a user-space library implements the Direct Rendering Manager.
MiniGUI mainly uses this library to support the dumb frame buffer
(no hardware acceleration). However, you can write a driver for your
graphics card or GPU to implement the hardware accelerated features.

To avoid modifying the MiniGUI source code when supporting a new GPU,
the `dri` engine has adopted a scalable design:

* You can directly use the `dri` engine to run MiniGUI on a GPU
which supports dumb frame buffer.
* When you want to take advantage of the hardware acceleration of
your GPU, you can write some code for your GPU as a sub driver
of `dri` engine outside MiniGUI.

In this situation, you need to configure MiniGUI with the following
option:

    --with-targetname=external

and implement the sub driver in your MiniGUI apps.

The header file `<minigui/exstubs.h>` defines the operators (a set of
callback functions) you need to implement for your GPU externally.

As an example, we implement the sub driver for `i915` graphics chard
in `mg-tests/dri-engine/`. Please refer to `mg-tests` repository.

To exploit the GPU's accelerated rendering capabilities, a MiniGUI app
can use `cairo` and/or `OpenGL ES` to assist in rendering 2D/3D graphics
when using the `dri` engine. We will provide some samples in `mg-tests`
or `mg-samples` for this purpose.

Note that for `dri` engine, we introduce a new section in MiniGUI runtime
configuration:

```
[dri]
defaultmode=1024x768-32bpp
pixelformat=XR24
device=/dev/dri/card0
dpi=96
```

You can use the key `dri.device` to specify your DRI device.

You can use the key `dri.pixelformat` to specify the DRM pixel format for the
screen. We use DRM fourcc code to defined the pixel format of the screen
surface. For more information, please see `<drm/drm_fourcc.h>` header file.
Note that only 8/16/24/32 bpp RGB formats are supported. For example, `XR24`
means `X8R8G8B8` pixel format.

Currently, the `dri` NEWGAL engine does not provide support for
MiniGUI-Processes run-time mode. We will enhance this in the subsequent
version of MiniGUI.

Also note that when you use the hardware accelerated sub driver, MiniGUI app
may need the root privilege to call `drmSetMaster` to set the video mode.
However, under MiniGUI-Processes run-time mode, only the server (`mginit`) will
need this privilege when you use the future `dri` engine.

#### Extra input messages

In MiniGUI 4.0.0, we introduce the extra input messages to support modern
input devices including multiple touch panel, gesture, tablet tool, and
table pad.

The extra input messages have the prefix `MSG_EXIN_`. If a MiniGUI app
want to handle these extra input events such as gestures, you need
to handle the `MSG_EXIN_XXX` messages in the app. For examples, please
refer to `mg-tests/extra-input/`.

Currently, there are two built-in IAL engines which can generates
the extra input messages:

* The IAL engine of `libinput` to support all modern input devices on a
Linux box. This engine runs on `libinput` introduced by Free Desktop project.

* The enhanced IAL engine of `random` to generate extra input messages
  automatically for testing.

You can also write your own IAL engines to generate the extra messages.
Please see the implementation of `libinput` and `random` engines for
the details.

For `libinput` engine, we introduce a new section in MiniGUI runtime
configuration:

```
[libinput]
seat=seat0
```

The key `libinput.seat` specifies the seat identifier, the default
is `seat0`.

For `random` engine, we introduce a new section in MiniGUI runtime
configuration:

```
[random]
logfile=events.out
eventtypes=mouse-keyboard-button-gesture-stouch
minkeycode=1
maxkeycode=128
minbtncode=0x100
maxbtncode=0x1ff
```

The MiniGUI runtime configuration key `random.logfile` specifies
the log file which will store the input events generated by this engine.
If MiniGUI failed to open the log file, the log feature will be disabled.

The MiniGUI runtime configuration key `random.eventtypes` specifies
the input event types which will be generated by this IAL engine,
in the following pattern:

    <event-type>[-<event-type>]*

The `<event-type>` can be one of the following values:

 - `mouse`: mouse.
 - `keyboard`: keyboard.
 - `button`: buttons.
 - `single_touch`: touch pen or single touch panel.
 - `multi_touch`: multiple touch panel.
 - `gesture`: gesture.
 - `tablet_tool`: tablet tool.
 - `tablet_pad`: tablet pad.
 - `switch`: switch.

The MiniGUI ETC key `random.minkeycode` specifies the minimal key code
which can be generated by the engine if `keyboard` is included.

The MiniGUI ETC key `random.maxkeycode` specifies the maximal key code
which can be generated by the engine if `keyboard` is included.

The MiniGUI ETC key `random.minbtncode` specifies the minimal button code
which can be generated by the engine if `button` is included.

The MiniGUI ETC key `random.maxbtncode` specifies the maximal key code
which can be generated by the engine if `button` is included.

For invalid `random.eventtyps`, the engine uses `mouse` as default.

For invalid `random.minkeycode`, and/or `random.maxkeycode` key values,
the engine uses `SCANCODE_ESCAPE`, and `SCANCODE_MICMUTE` respectively.

For invalid `random.minbtncode`, and/or `random.maxbtncode` key values, use
`0x100` (`BTN_MISC` defined by Linux kernel), and `0x2ff` (`KEY_MAX` defined by
Linux kernel) respectively.

This engine maintains a state machine for each input event type, and
generates a reasonable event sequence for each type. If and only if
an event sequence finished or cancelled, the engine switch to another
event type randomly.

Note that currently, the following event types (in `random` engine)
are not implemented:

 - `multi_touch`
 - `tablet_tool`
 - `tablet_pad`
 - `switch`

#### Slice allocator

MiniGUI now provides an efficient way to allocate groups of equal-sized
chunks of memory.

Memory slices provide a space-efficient and multi-processing scalable
way to allocate equal-sized pieces of memory, just like the
MiniGUI's block data heap. Relative to the standard malloc function
and block data heap, this allocator can avoid excessive memory-waste,
scalability and performance problems.

The following APIs are introduced:

* `mg_slice_alloc` to allocate a given size trunk of memory.
* `mg_slice_free` to free a given size trunk of memory.
* `mg_slice_new` to allocate a memory for a given structure.
* `mg_slice_delete` to free a memory for a given structure.

We use the slice allocator when laying out the text in complex scripts.

Note that this implementation is derived from LGPL'd glib.

#### Other enhancements

* A new BITMAP type `BMP_TYPE_REPLACEKEY` was added. When `bmType` of
a BITMAP object has this bit set, any pixel which is equal to
`bmColorKey` will be replaced by `bmColorRep`.

* More key scancodes are defined for modern keyboard, and the `NR_KEYS`
is re-defined to be 250.

* Support for libPNG 1.6.x.

* Support for Ubuntu 18.04 LTS.

* Support for GCC 7.

#### Backward compatibility issues

In MiniGUI 4.0.0, we changed some unreasonable APIs which were introduced
in early versions. There are also other changes broke the backward
compatibility. This section gives you a summary about these changes.

* Rename `UChar32` to `Uchar32` and `UChar16` to `Uchar16` in order to
  avoid the conflict with typedef of UChar32 in the system header
  `<unicode/umachine.h>`.
* Rename `mg_FT_LcdFilter` to `FT2LCDFilter` in order to follow MiniGUI
  naming rules.
* Redefine `Uchar32` and `Glyph32` as `Uint32` instead of `int`.

In early versions, we did not significantly distinguish between
characters and glyphs. This will lead to some confusion. Therefore,
we introduce a new type called `Achar32`, which is the character's
index value under a certain charset/encoding. While the type `Glyph32`
is the index value of a glyph in a font.

In order to reflect the correct character and glyph concepts,
the following functions are removed:

  * GetGlyphType, use GetACharType instead.
  * GetGlyphShape, use GetShapedAChar instead.
  * GetGlyphBIDIType, use GetACharBidiType

The names of the following functions are changed:

  * BIDIGetTextLogicalGlyphs -> BIDIGetTextLogicalAChars
  * BIDIGetTextVisualGlyphs -> BIDIGetTextVisualAChars
  * BIDILogAChars2VisGlyphs -> BIDILogAChars2VisAChars
  * BIDILogAChars2VisGlyphsEx -> BIDILogAChars2VisACharsEx

The following functions are deprecated, you should use
the new Unicode version instead:

  * BIDIGetTextLogicalAChars
  * BIDIGetTextVisualAChars
  * BIDILogAChars2VisAChars
  * BIDILogAChars2VisACharsEx
  * BIDIGetTextRangesLog2Vis
  * BIDIGetLogicalEmbedLevelsEx

The following functions are deprecated:

  * GetGlyphBitmap, use GetGlyphInfo instead.

The fields `height` and `descent` have been removed from GLYPHINFO structure.
You should get the font metrics information by calling `GetFontMetrics` function
if you want to get the height and descent data of one font.

The the basic glyph type and break type have been removed from GLYPHINFO
structure. You should use `GetACharType` instead.

#### Deprecated features

Support for FreeType1 was removed.

You should always use FreeType2 to support vector fonts, such as TrueType
fonts (TTF), TrueType collections (TTC), OpenType fonts (OTF, both TrueType
and CFF variants), OpenType collections (OTC), and Type 1 fonts (PFA and PFB).

## Version 3.2.2

The MiniGUI development team announces the availability of MiniGUI 3.2.2.
All users of MiniGUI are recommended strongly to use this version.
Please report any bugs and incompatibilities in

https://github.com/VincentWei/minigui

### What's new in this version

* ENHANCEMENTS:
  1. New API: `SyncUpdateDC`. You can use this function to synchronize
    the update rectangles of a surface to screen, if the surface
    represents the shadow frame buffer of the screen.
    * configure option: `--enable-syncupdate`
    * macro: `_MGUSE_SYNC_UPDATE`
  1. New API: `UpdateInvalidClient`. You can use this function to update
    the invalid client region of a window instantly.
  1. Use different colors for the output of `_DBG_PRINTF` and `_ERR_PRINTF`.
  1. Add `__mg_save_jpg` function for storing MYBITMAP as JPEG file (10km).
  1. Modified logic for checking JPEG format (10km).
  1. Support BIDI for UNICODE charsets and cleanup the implementation.
    * New API: `GetGlyphBIDIType` to get the glyph type in BIDI.
    * New DC attribute: BIDI flag.
  1. Add a new key `dpi` for NEWGAL engine to define the DPI of the screen.
     If it is absent, use 96 as the default DPI.
  1. Add an item for `GetGDCapability` to return DPI of the DC.
  1. New API `InitSlaveScreenEx` to specify the DPI of slave screen.
     Define `InitSlaveScreen` as an inline function calling `InitSlaveScreenEx`.
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
  1. New header for CommLCD NEWGAL engine and COMM IAL engine:
    `<minigui/exstubs.h>`.

* BUGFIXING:
  1. handle `PNG_COLOR_TYPE_GRAY_ALPHA` color type of PNG files.
  1. Fix a bug to free a null pointer (ReleaseDC).
  1. No need to make the pitch of FT2 monobitmap is single-byte aligned.
     This bug may generate dirty dots for monobitmap glyph from TTF.
  1. Skip null pixels for SUBPIXEL glyphs. This bug will always show background
     pixels of one SUBPIXEL glyph.
  1. Fix the bug of wrong bounding box handling for SUBPIXEL rendering of glyph.
  1. Tune cache implementation of FreeType2 font engine.
    * Enable cache for rotated LOGFONT.

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

<https://github.com/VincentWei/minigui>.

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


[HybridOS]: https://github.com/FMSoftCN/hybridos
[HybridOS Foundation Class Library]: https://github.com/FMSoftCN/hybridos/tree/dev/device-side/hfcl
[CSS Text Module Level 3]: https://www.w3.org/TR/css-text-3/
[CSS Writing Modes Level 3]: https://www.w3.org/TR/css-writing-modes-3/
