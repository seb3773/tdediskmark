# aa-painter

Files:

- `tqtaapainter.h` / `tqtaapainter.cpp`: local antialiasing helpers (Wu line) for custom-painted widgets.
- `example/main.cpp`: demo comparing legacy `TQPainter::drawLine()` with Wu antialiased lines.

API:

- `TQtAAPainter`
  - `drawLineAA(TQImage*, x0, y0, x1, y1, fg, width)`
  - `drawPolylineAA(TQImage*, points, count, fg, width)`
  - `drawEllipseAA(TQImage*, cx, cy, rx, ry, fg, segments)`
  - `drawCircleAA(TQImage*, cx, cy, r, fg, segments)`
  - `fillCircleAA(TQImage*, cx, cy, r, fg)`
  - `fillPolygonAA(TQImage*, points, count, fg)`
  - `fillPolygonAA(TQImage*, points, count, fg, outlineWidth)`
  - `fillAnnularArcAA(TQImage*, cx, cy, rOuter, rInner, startDeg, spanDeg, fg, segments)`
  - `drawArcAA(TQImage*, cx, cy, r, startDeg, spanDeg, fg, segments, width)`
  - `drawLineAA(TQPainter*, x0, y0, x1, y1, fg, bg, width)`
  - `drawPolylineAA(TQPainter*, points, count, fg, bg, width)`
  - `drawEllipseAA(TQPainter*, cx, cy, rx, ry, fg, bg, segments)`
  - `drawCircleAA(TQPainter*, cx, cy, r, fg, bg, segments)`
  - `fillCircleAA(TQPainter*, cx, cy, r, fg)`

Rendering model (implementation):

- `TQImage*` overloads rasterize directly into the provided image and blend against the destination pixels.
- `TQPainter*` overloads rasterize into a small temporary `TQImage(32bpp)` covering the primitive bounding box.
  The image is pre-filled with the provided `bg` (background color), then blitted back using `TQPainter::drawImage()`.

Ellipse/circle details:

- Ellipses are approximated with a polyline (configurable `segments`).
- For `TQImage*` overloads: if `segments == 0`, the ellipse outline uses a per-pixel distance test (1px AA outline).
- The whole polyline is rasterized into a single temporary image (to avoid overwrite artifacts).

Usage:

See `aa-painter/example/main.cpp`.

Build:

- Example target: `tqtaapainter_example`

```sh
cmake -S . -B build
cmake --build build -j --target tqtaapainter_example
./build/tqtaapainter_example
```

Limitations / notes:

- For `TQPainter*` overloads, background must be a solid color (`bg`). The implementation does not read pixels from the target paint device.
- For `TQImage*` overloads, blending is done against destination pixels (recommended for composing multiple primitives).
- No alpha in inputs: output pixels are opaque RGB.
- `width` applies to stroke-based primitives (`drawLineAA`, `drawPolylineAA`, `drawArcAA`). For `width > 1`, a distance-to-segment coverage model is used.
- `fillCircleAA` draws a filled disk. The `TQImage*` overload also provides a 1px antialiased boundary.
- `fillPolygonAA` uses scanline fill. The default variant draws a thin AA outline to match `TQPainter`'s filled polygon look.
- `fillAnnularArcAA` draws a filled annular sector (ring segment). This is useful to reproduce `drawPie()`-based ring effects.
- This is intentionally local to custom widgets (no TQt3 engine patching).

Composition note:

- The `TQPainter*` overloads rasterize a temporary image and blit it back.
  If you need to compose multiple AA primitives reliably, render into a single offscreen `TQImage` using the `TQImage*` overloads and then `drawImage()` once.
- To minimize flicker in animated widgets, consider full double-buffering (compose into an offscreen `TQPixmap` and blit once per `paintEvent`).
- For very small widget sizes, AA can become less visible. A practical approach is internal supersampling:
  render at `ss=2` into a larger `TQImage` and downscale with `smoothScale()`.
