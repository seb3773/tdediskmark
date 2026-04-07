# meter-gauge

Files:

- `tqtmetergauge.h` / `tqtmetergauge.cpp`: `TQtMeterGauge` (TQt3) analog meter gauge widget.
- `example/main.cpp`: demo showing 4 gauge configurations driven by a slider.

Widget:

`TQtMeterGauge` draws a circular dial with:

- optional colored threshold arc segments
- main arc (scale sweep)
- major/minor tick marks
- optional grid numeric labels along the arc
- optional central value readout
- optional top/bottom labels (`unitsLabel`, `label`)
- a rotating needle + center cap

Value model:

- Range: `[minValue..maxValue]` (double)
- `setValue(double)` clamps input to range and emits:
  - `valueChanged(double currentValue)`
  - `thresholdFired(int thresholdIndex)` when the threshold index changes

Thresholds:

- Configure with `setThresholdRange(start, stop, thresholdIndex, color)`.
- Internally stored as a list; `thresholdIndexForValue()` returns the first range where:
  - `v >= start` (with epsilon) and `v < stop`
- If a `color` is valid, an arc segment is drawn with pen width derived from the gauge radius.

Geometry / sizing:

- `radius` controls the widget size: `sizeHint()` is `(2*radius+2, 2*radius+2)`.
- `setRadius()` clamps to at least 45.
- Scale sweep is defined by angles in degrees:
  - `startScaleAngle`, `stopScaleAngle`

Scale / ticks:

- `scaleGridStep` controls *grid tick marks* and *grid value labels*.
  - If `scaleGridStep > 0` and `drawGridValues == true`, numeric labels are placed on the arc.
- `scaleStep` controls *minor ticks* distribution (computed as `(max-min)/scaleStep`, minimum 2).

Rendering notes (implementation):

- Uses `drawArc()` with Qt’s 1/16th degree units and a `(-90 - angle)` transform to map angles.
- Needle angle is interpolated linearly across the sweep using current value.
- Text sizes are derived from radius (`gridLabelSize`, `fontPixel`).

Usage:

See `meter-gauge/example/main.cpp` for complete setups. Typical:

```cpp
TQtMeterGauge* g = new TQtMeterGauge(parent);

g->setMinValue(0.0);
g->setMaxValue(100.0);
g->setValue(40.0);

g->setUnitsLabel("%");
g->setLabel("load");

g->setRadius(150);
g->setStartScaleAngle(45);
g->setStopScaleAngle(315);

g->setScaleGridStep(10.0);
g->setScaleStep(1.0);

g->setThresholdRange(0.0, 70.0, 0, TQColor(0, 160, 0));
```

Build:

- Example target: `tqtmeter_gauge_example`

```sh
cmake -S . -B build
cmake --build build -j --target tqtmeter_gauge_example
./build/tqtmeter_gauge_example
```

Limitations / notes:

- Threshold selection depends on insertion order; overlapping ranges will pick the first match.
- No anti-aliasing tuning is exposed; rendering relies on TQt3 painter defaults.
- The widget size policy is fixed and based on `radius`.
