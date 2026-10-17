# Default DRM damage-bounded updates

## Scope

The built-in DRM shadow-to-scanout path now copies the accumulated dirty
bounding rectangle for each scanout buffer. Row copies use pixel width times
BytesPerPixel, with independent source and destination pitches and offsets.
The copy is clipped to both surfaces and row capacity; pitch padding is not
copied. This requires no libdrmdrivers.so or GPU support.

Each of the real/flip buffers starts with full-screen pending damage. Every
shadow update is added to both buffers; copying clears only the destination's
pending damage. This ensures a buffer receives changes made while the other
buffer was displayed. Multiple disjoint changes are conservatively merged to
one bounding rectangle, so widely separated updates may still cost a full frame.
External-driver and compositing-schema copies retain their conservative range;
software-cursor composition is not optimized in this phase.

Page-flip submission no longer changes curr_buff immediately. A completion
event changes ownership. While a flip is pending neither buffer is copied:
the async updater polls at its next update iteration; the synchronous path
waits up to one second for an event and preserves damage if it cannot proceed.
Submission failures also retain dirty work for retry. Timeout/error recovery
beyond retaining damage is not introduced here. DirtyFB support is no longer
a prerequisite for a synchronous page-flip request. Synchronous page flips
require double buffering with a shadow surface: a single-buffer screen is
drawn directly and must not flip to an unpopulated alternate buffer.

## Local validation

Run from any directory, supplying the MiniGUI repository path if needed:

```sh
python3 scripts/test-drm-damage.py
python3 scripts/test-drm-lease-lock.py
```

The test extracts the actual production copy, update and flip helper functions
and compiles them with mocked surface/DRM interfaces, AddressSanitizer and
UndefinedBehaviorSanitizer. It checks initial buffer population, 40 alternating
frames with disjoint updates, source/destination pitch and offset handling,
padding/guard preservation, clipping, pending ownership and submission failure.
It also extracts `DRM_SyncUpdate` to check that single-buffer rendering never
submits the untouched flip buffer, while double-buffer rendering still flips.
PRIME synchronization uses an explicit mock fd and ioctl stub; no real device
ioctl is issued.
It does not compile the complete Linux driver or validate kernel event behavior.
Both tests passed on the local macOS host with ASan/UBSan on 2026-10-17.
The lease test checks source-level acquire/release ordering and compiles the
production drawing macros and file-lock helpers. A separate child process
confirms that a compositor-style reader is excluded until the writer releases
the surface lock. This does not exercise an actual GPU fence or compositor.
Linux DRM headers/toolchain were not
available for a target driver build. No xGUI build/test pipeline was started.

## RK3566 acceptance (pending)

Build MiniGUI using the existing board toolchain/configuration; deploy it with
xGUI using the default Skia raster path. Preserve the previous library for A/B
comparison. Record the MiniGUI commit and schema/double-buffer configuration.

- Check initial screen, small animated regions, distant simultaneous updates,
  alternating updates, scrolling in both directions, and idle after updates.
- Verify no stale alternating frames, torn regions or cursor trails; test both
  synchronous and asynchronous updater configurations actually supported by
  the product.
- Exercise repeated startup/shutdown, display suspend/resume and failure paths.
  Hardware event completion and recovery remain device acceptance requirements.
- Compare CPU usage and frame/input latency against the previous library using
  the same xGUI build, scene and gestures. Full-screen scrolling may still
  invalidate most of the display; no proportional speedup is promised there.

At 1024x600/32bpp, the previous per-update payload was 2,457,600 bytes. Once both
buffers are initialized, a stationary 100x100 dirty rectangle requires 40,000
pixel bytes per copy, provided accumulated damage remains within that rectangle.
These are byte-count examples, not board performance measurements.

The mock test checks initial buffer population, alternating-frame contents,
pitch/offset handling, clipping and flip ownership. Updated Linux target
compilation and RK3566 measurements remain pending.
