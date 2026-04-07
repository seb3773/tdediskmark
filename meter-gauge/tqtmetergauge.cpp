#include "tqtmetergauge.h"

#include <ntqpainter.h>
#include <ntqpen.h>
#include <ntqfont.h>
#include <ntqfontmetrics.h>

#include <math.h>

static inline double tqt_deg2rad(double d) { return d * (3.14159265358979323846 / 180.0); }

static inline double tqt_clamp(double x, double a, double b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

TQtMeterGauge::TQtMeterGauge(TQWidget* parent, const char* name)
    : TQWidget(parent, name),
      m_drawValue(1),
      m_drawGridValues(1),
      m_valuePrecision(0),
      m_scalePrecision(0),
      m_currentThreshold(0),
      m_radius(100),
      m_startScaleAngle(35),
      m_stopScaleAngle(325),
      m_minValue(0.0),
      m_maxValue(220.0),
      m_value(0.0),
      m_scaleStep(2.0),
      m_scaleGridStep(10.0),
      m_backgroundColor(TQColor(0, 0, 0)),
      m_needleColor(TQColor(0, 0, 255)),
      m_textColor(TQColor(255, 255, 255)),
      m_gridColor(TQColor(255, 255, 255)),
      m_label(),
      m_unitsLabel(),
      m_ranges() {
    setBackgroundMode(PaletteBackground);
    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);
    resize(sizeHint());
}

double TQtMeterGauge::minValue() const { return m_minValue; }

void TQtMeterGauge::setMinValue(double v) {
    m_minValue = v;
    if (m_minValue > m_maxValue) m_maxValue = m_minValue;
    if (m_value < m_minValue) m_value = m_minValue;
    update();
}

double TQtMeterGauge::maxValue() const { return m_maxValue; }

void TQtMeterGauge::setMaxValue(double v) {
    m_maxValue = v;
    if (m_minValue > m_maxValue) m_minValue = m_maxValue;
    if (m_value > m_maxValue) m_value = m_maxValue;
    update();
}

double TQtMeterGauge::value() const { return m_value; }

const TQColor& TQtMeterGauge::backgroundColor() const { return m_backgroundColor; }

void TQtMeterGauge::setBackgroundColor(const TQColor& c) { m_backgroundColor = c; update(); }

const TQColor& TQtMeterGauge::needleColor() const { return m_needleColor; }

void TQtMeterGauge::setNeedleColor(const TQColor& c) { m_needleColor = c; update(); }

const TQColor& TQtMeterGauge::textColor() const { return m_textColor; }

void TQtMeterGauge::setTextColor(const TQColor& c) { m_textColor = c; update(); }

const TQColor& TQtMeterGauge::gridColor() const { return m_gridColor; }

void TQtMeterGauge::setGridColor(const TQColor& c) { m_gridColor = c; update(); }

const TQString& TQtMeterGauge::label() const { return m_label; }

void TQtMeterGauge::setLabel(const TQString& l) { m_label = l; update(); }

const TQString& TQtMeterGauge::unitsLabel() const { return m_unitsLabel; }

void TQtMeterGauge::setUnitsLabel(const TQString& l) { m_unitsLabel = l; update(); }

unsigned int TQtMeterGauge::radius() const { return m_radius; }

void TQtMeterGauge::setRadius(unsigned int r) {
    if (r < 45U) r = 45U;
    m_radius = r;
    resize(sizeHint());
    update();
}

unsigned int TQtMeterGauge::startScaleAngle() const { return m_startScaleAngle; }

void TQtMeterGauge::setStartScaleAngle(unsigned int a) { m_startScaleAngle = a; update(); }

unsigned int TQtMeterGauge::stopScaleAngle() const { return m_stopScaleAngle; }

void TQtMeterGauge::setStopScaleAngle(unsigned int a) { m_stopScaleAngle = a; update(); }

double TQtMeterGauge::scaleStep() const { return m_scaleStep; }

void TQtMeterGauge::setScaleStep(double s) { if (s >= 0.0) { m_scaleStep = s; update(); } }

double TQtMeterGauge::scaleGridStep() const { return m_scaleGridStep; }

void TQtMeterGauge::setScaleGridStep(double s) { if (s >= 0.0) { m_scaleGridStep = s; update(); } }

bool TQtMeterGauge::drawValue() const { return m_drawValue != 0; }

void TQtMeterGauge::setDrawValue(bool on) { m_drawValue = on ? 1 : 0; update(); }

bool TQtMeterGauge::drawGridValues() const { return m_drawGridValues != 0; }

void TQtMeterGauge::setDrawGridValues(bool on) { m_drawGridValues = on ? 1 : 0; update(); }

int TQtMeterGauge::drawValuePrecision() const { return m_valuePrecision; }

void TQtMeterGauge::setDrawValuePrecision(int p) { if (p >= 0) { m_valuePrecision = p; update(); } }

int TQtMeterGauge::scaleLabelPrecision() const { return m_scalePrecision; }

void TQtMeterGauge::setScaleLabelPrecision(int p) { if (p >= 0) { m_scalePrecision = p; update(); } }

void TQtMeterGauge::setThresholdRange(double start, double stop, int thresholdIndex, const TQColor& color) {
    RangeData rd;
    rd.start = start;
    rd.stop = stop;
    rd.idx = thresholdIndex;
    rd.color = color;
    rd.hasColor = color.isValid() ? 1 : 0;
    m_ranges.append(rd);
    update();
}

void TQtMeterGauge::clearThresholdRanges() {
    m_ranges.clear();
    update();
}

TQSize TQtMeterGauge::minimumSizeHint() const { return TQSize((int)m_radius * 2 + 2, (int)m_radius * 2 + 2); }

TQSize TQtMeterGauge::sizeHint() const { return minimumSizeHint(); }

void TQtMeterGauge::setValue(double v) {
    if (m_maxValue <= m_minValue) return;
    const double nv = tqt_clamp(v, m_minValue, m_maxValue);
    if (fabs(nv - m_value) < 0.0000001) return;
    m_value = nv;
    update();
    emit valueChanged(m_value);

    const int ti = thresholdIndexForValue(m_value);
    if (ti != m_currentThreshold) {
        m_currentThreshold = ti;
        emit thresholdFired(ti);
    }
}

void TQtMeterGauge::setValueInt(int v) {
    setValue((double)v);
}

int TQtMeterGauge::thresholdIndexForValue(double v) const {
    for (TQValueList<RangeData>::ConstIterator it = m_ranges.begin(); it != m_ranges.end(); ++it) {
        const RangeData& rd = *it;
        if ((v > rd.start || fabs(v - rd.start) < 0.000001) && v < rd.stop) return rd.idx;
    }
    return 0;
}

void TQtMeterGauge::paintEvent(TQPaintEvent*) {
    TQPainter p(this);
    p.fillRect(rect(), colorGroup().brush(TQColorGroup::Background));

    const int r = (int)m_radius;
    const int d = r * 2;

    // Center the gauge circle within the widget area
    const int ox = (width() - d - 2) / 2;
    const int oy = (height() - d - 2) / 2;
    p.translate(1 + (ox > 0 ? ox : 0), 1 + (oy > 0 ? oy : 0));

    const TQRect rectf(0, 0, d, d);

    {
        p.setPen(m_backgroundColor);
        p.setBrush(m_backgroundColor);
        p.drawEllipse(rectf);
    }

    const double scaleDegree = (double)((int)m_stopScaleAngle - (int)m_startScaleAngle);
    const double rangeSpan = (m_maxValue - m_minValue);
    if (rangeSpan <= 0.0000001) return;

    const double margin = (double)r / 20.0;
    const double gridLabelSize = (double)r / 10.0;
    const double scaleWidth = (double)r / (10.0 + 20.0);
    const double fontPixel = gridLabelSize * 0.75;

    const double m = margin + scaleWidth * 0.5;
    const int ax = (int)floor((double)rectf.x() + m + 0.5);
    const int ay = (int)floor((double)rectf.y() + m + 0.5);
    const int aw = (int)floor((double)rectf.width() - m * 2.0 + 0.5);
    const int ah = (int)floor((double)rectf.height() - m * 2.0 + 0.5);
    const TQRect arcRect(ax, ay, aw, ah);

    {
        for (TQValueList<RangeData>::ConstIterator it = m_ranges.begin(); it != m_ranges.end(); ++it) {
            const RangeData& rd = *it;
            if (!rd.hasColor) continue;
            p.setPen(TQPen(rd.color, (int)scaleWidth));
            const double a0 = (double)m_startScaleAngle + scaleDegree * ((rd.start - m_minValue) / rangeSpan);
            const double a1 = (double)m_startScaleAngle + scaleDegree * ((rd.stop - m_minValue) / rangeSpan);
            const double span = a1 - a0;
            p.drawArc(arcRect, (int)((-90.0 - a0) * 16.0), (int)(-span * 16.0));
        }
    }

    {
        p.setPen(m_textColor);
        const int sx = (int)floor((double)rectf.x() + margin + 0.5);
        const int sy = (int)floor((double)rectf.y() + margin + 0.5);
        const int sw = (int)floor((double)rectf.width() - margin * 2.0 + 0.5);
        const int sh = (int)floor((double)rectf.height() - margin * 2.0 + 0.5);
        const TQRect scaleRect(sx, sy, sw, sh);
        p.drawArc(scaleRect, (int)((-90.0 - (double)m_startScaleAngle) * 16.0), (int)(-scaleDegree * 16.0));
    }

    {
        TQFont f = p.font();
        f.setPixelSize((int)fontPixel);
        p.setFont(f);
        TQFontMetrics fm(f);

        if (m_scaleGridStep > 0.0 && m_drawGridValues) {
            double startRad = -tqt_deg2rad((double)m_startScaleAngle);
            const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
            const double fullSteps = ((m_maxValue - m_minValue) / m_scaleGridStep);
            const double deltaRad = -tqt_deg2rad(scaleDegree / rangeSpan * m_scaleGridStep);
            double val = m_minValue;

            for (int i = 0; i <= stepsCount; ++i) {
                const double ang = startRad + (double)i * deltaRad;
                const double sina = sin(ang);
                const double cosa = cos(ang);

                const TQString str = TQString::number(val, 'f', m_scalePrecision);
                const TQSize s = fm.size(TQt::SingleLine, str);

                const double off = (double)r - gridLabelSize - margin * 3.0;
                const int x = (int)(off * sina - (double)s.width() / 2.0);
                const int y = (int)(off * cosa + (double)s.height() / 4.0);

                p.setPen(m_textColor);
                p.drawText(x + r, y + r, str);

                val += m_scaleGridStep;
            }

            if (fabs(fullSteps - (double)stepsCount) > 0.000001) {
                const double rad = -tqt_deg2rad((double)m_startScaleAngle + scaleDegree);
                const double sina = sin(rad);
                const double cosa = cos(rad);

                const TQString str = TQString::number(m_maxValue, 'f', m_scalePrecision);
                const TQSize s = fm.size(TQt::SingleLine, str);
                const double off = (double)r - gridLabelSize - margin * 3.0;
                const int x = (int)(off * sina - (double)s.width() / 2.0);
                const int y = (int)(off * cosa + (double)s.height() / 4.0);

                p.drawText(x + r, y + r, str);
            }
        }

        if (!m_unitsLabel.isEmpty()) {
            p.setPen(m_textColor);
            p.drawText(TQRect(0, (int)(margin * 3.0 + gridLabelSize * 3.0), d, r), TQt::AlignHCenter, m_unitsLabel);
        }

        if (!m_label.isEmpty()) {
            TQFont lf = f;
            lf.setPixelSize((int)(fontPixel * 3.5));
            lf.setBold(true);
            p.setFont(lf);
            p.setPen(m_textColor);
            p.drawText(TQRect(0, (int)(d - margin * 2.0 - gridLabelSize * 3.0), d, r), TQt::AlignHCenter, m_label);
            p.setFont(f);
        }

        if (m_drawValue) {
            TQFont vf = f;
            vf.setPixelSize((int)(fontPixel * 2.0));
            vf.setBold(true);
            p.setFont(vf);
            p.setPen(m_textColor);
            p.drawText(TQRect(0, (int)(d - margin - gridLabelSize * 2.0), d, r), TQt::AlignHCenter,
                       TQString::number(m_value, 'f', m_valuePrecision));
            p.setFont(f);
        }
    }

    {
        const double needleR = (double)r / 10.0;
        p.save();
        p.translate(r, r);

        const double angle = (double)m_startScaleAngle + scaleDegree * ((m_value - m_minValue) / rangeSpan);
        p.rotate(angle);

        TQPen pen(m_needleColor);
        pen.setWidth((int)((double)r / 75.0) + 1);
        pen.setCapStyle(TQt::RoundCap);
        pen.setJoinStyle(TQt::RoundJoin);
        p.setPen(pen);
        p.drawLine(0, (int)((double)r - margin), 0, (int)(-(needleR * 2.0)));

        p.setBrush(m_backgroundColor);
        p.setPen(NoPen);
        p.drawEllipse((int)(-needleR), (int)(-needleR), (int)(needleR * 2.0), (int)(needleR * 2.0));
        p.restore();

        p.save();
        p.translate(r, r);
        p.setPen(NoPen);
        p.setBrush(TQColor(180, 180, 180));
        p.drawEllipse((int)(-needleR), (int)(-needleR), (int)(needleR * 2.0), (int)(needleR * 2.0));
        p.setBrush(TQColor(120, 120, 120));
        p.drawEllipse((int)(-needleR / 2.0), (int)(-needleR / 2.0), (int)(needleR), (int)(needleR));
        p.restore();
    }

    {
        p.save();
        p.translate(r, r);
        p.rotate((double)m_startScaleAngle);

        if (m_scaleGridStep > 0.0) {
            const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
            const double scaleStepDeg = scaleDegree / rangeSpan * m_scaleGridStep;
            p.setPen(TQPen(m_gridColor, 1));

            for (int i = 0; i <= stepsCount; ++i) {
                p.drawLine(0, (int)((double)r - margin), 0, (int)((double)r - gridLabelSize - margin));
                p.rotate(scaleStepDeg);
            }

            const double fullSteps = (m_maxValue - m_minValue) / m_scaleGridStep;
            if (fabs(fullSteps - (double)stepsCount) > 0.000001) {
                p.restore();
                p.save();
                p.translate(r, r);
                p.rotate((double)m_startScaleAngle + scaleDegree);
                p.setPen(TQPen(m_gridColor, 1));
                p.drawLine(0, (int)((double)r - margin), 0, (int)((double)r - gridLabelSize - margin));
            }
        } else {
            p.setPen(TQPen(m_gridColor, 1));
            p.drawLine(0, (int)((double)r - margin), 0, (int)((double)r - gridLabelSize - margin));
            p.rotate(scaleDegree);
            p.drawLine(0, (int)((double)r - margin), 0, (int)((double)r - gridLabelSize - margin));
        }

        p.restore();

        if (m_scaleStep > 0.0) {
            p.save();
            p.setPen(TQPen(m_gridColor, 1));
            p.translate(r, r);
            p.rotate((double)m_startScaleAngle);

            int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleStep);
            if (stepsCount < 2) stepsCount = 2;
            const double stepDeg = scaleDegree / (double)stepsCount;

            stepsCount -= 2;
            p.rotate(stepDeg);

            for (int i = 0; i <= stepsCount; ++i) {
                p.drawLine(0, (int)((double)r - margin), 0, (int)((double)r - scaleWidth - margin));
                p.rotate(stepDeg);
            }

            p.restore();
        }
    }
}
