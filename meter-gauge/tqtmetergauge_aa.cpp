#include "tqtmetergauge_aa.h"

#include "tqtaapainter.h"

#include <ntqevent.h>
#include <ntqfont.h>
#include <ntqfontmetrics.h>
#include <ntqpainter.h>
#include <ntqpen.h>
#include <ntqpixmap.h>
#include <ntqregion.h>
#include <ntqbitmap.h>

#include <math.h>
#include <string.h>

static inline double tqtmg_aa_deg2rad(double d) { return d * (3.14159265358979323846 / 180.0); }

static inline double tqtmg_aa_clamp(double x, double a, double b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

TQtMeterGaugeAA::TQtMeterGaugeAA(TQWidget* parent, const char* name)
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
      m_ranges(),
      m_staticImg(),
      m_frameImg(),
      m_staticValid(0),
      m_circleMask(),
      m_circleMaskDiameter(-1)
{
    setBackgroundMode(TQt::PaletteBackground);
    setBackgroundOrigin(TQWidget::WindowOrigin);

    // Default to WNoAutoErase=ON for impenetrable 50fps flicker protection.
    // It will be temporarily disabled during tab switches to allow X11 native hardware wiping of layout ghosts.
    setWFlags(getWFlags() | TQt::WNoAutoErase);
    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);
    resize(sizeHint());
}

double TQtMeterGaugeAA::minValue() const { return m_minValue; }

void TQtMeterGaugeAA::setMinValue(double v)
{
    m_minValue = v;
    if (m_minValue > m_maxValue) m_maxValue = m_minValue;
    if (m_value < m_minValue) m_value = m_minValue;
    invalidateStatic_();
    update();
}

double TQtMeterGaugeAA::maxValue() const { return m_maxValue; }

void TQtMeterGaugeAA::setMaxValue(double v)
{
    m_maxValue = v;
    if (m_minValue > m_maxValue) m_minValue = m_maxValue;
    if (m_value > m_maxValue) m_value = m_maxValue;
    invalidateStatic_();
    update();
}

double TQtMeterGaugeAA::value() const { return m_value; }

const TQColor& TQtMeterGaugeAA::backgroundColor() const { return m_backgroundColor; }

void TQtMeterGaugeAA::setBackgroundColor(const TQColor& c)
{
    m_backgroundColor = c;
    invalidateStatic_();
    update();
}

const TQColor& TQtMeterGaugeAA::needleColor() const { return m_needleColor; }

void TQtMeterGaugeAA::setNeedleColor(const TQColor& c)
{
    m_needleColor = c;
    update();
}

const TQColor& TQtMeterGaugeAA::textColor() const { return m_textColor; }

void TQtMeterGaugeAA::setTextColor(const TQColor& c)
{
    m_textColor = c;
    invalidateStatic_();
    update();
}

const TQColor& TQtMeterGaugeAA::gridColor() const { return m_gridColor; }

void TQtMeterGaugeAA::setGridColor(const TQColor& c)
{
    m_gridColor = c;
    invalidateStatic_();
    update();
}

const TQString& TQtMeterGaugeAA::label() const { return m_label; }

void TQtMeterGaugeAA::setLabel(const TQString& l)
{
    m_label = l;
    update();
}

const TQString& TQtMeterGaugeAA::unitsLabel() const { return m_unitsLabel; }

void TQtMeterGaugeAA::setUnitsLabel(const TQString& l)
{
    m_unitsLabel = l;
    update();
}

unsigned int TQtMeterGaugeAA::radius() const { return m_radius; }

void TQtMeterGaugeAA::setRadius(unsigned int r)
{
    if (r < 45U) r = 45U;
    m_radius = r;
    m_circleMaskDiameter = -1; // invalidate mask cache when radius changes
    invalidateStatic_();
    resize(sizeHint());
    update();
}

unsigned int TQtMeterGaugeAA::startScaleAngle() const { return m_startScaleAngle; }

void TQtMeterGaugeAA::setStartScaleAngle(unsigned int a)
{
    m_startScaleAngle = a;
    invalidateStatic_();
    update();
}

unsigned int TQtMeterGaugeAA::stopScaleAngle() const { return m_stopScaleAngle; }

void TQtMeterGaugeAA::setStopScaleAngle(unsigned int a)
{
    m_stopScaleAngle = a;
    invalidateStatic_();
    update();
}

double TQtMeterGaugeAA::scaleStep() const { return m_scaleStep; }

void TQtMeterGaugeAA::setScaleStep(double s)
{
    if (s >= 0.0) {
        m_scaleStep = s;
        invalidateStatic_();
        update();
    }
}

double TQtMeterGaugeAA::scaleGridStep() const { return m_scaleGridStep; }

void TQtMeterGaugeAA::setScaleGridStep(double s)
{
    if (s >= 0.0) {
        m_scaleGridStep = s;
        invalidateStatic_();
        update();
    }
}

bool TQtMeterGaugeAA::drawValue() const { return m_drawValue != 0; }

void TQtMeterGaugeAA::setDrawValue(bool on)
{
    m_drawValue = on ? 1 : 0;
    update();
}

bool TQtMeterGaugeAA::drawGridValues() const { return m_drawGridValues != 0; }

void TQtMeterGaugeAA::setDrawGridValues(bool on)
{
    m_drawGridValues = on ? 1 : 0;
    invalidateStatic_();
    update();
}

int TQtMeterGaugeAA::drawValuePrecision() const { return m_valuePrecision; }

void TQtMeterGaugeAA::setDrawValuePrecision(int p)
{
    if (p >= 0) {
        m_valuePrecision = p;
        update();
    }
}

int TQtMeterGaugeAA::scaleLabelPrecision() const { return m_scalePrecision; }

void TQtMeterGaugeAA::setScaleLabelPrecision(int p)
{
    if (p >= 0) {
        m_scalePrecision = p;
        invalidateStatic_();
        update();
    }
}

void TQtMeterGaugeAA::setThresholdRange(double start, double stop, int thresholdIndex, const TQColor& color)
{
    RangeData rd;
    rd.start = start;
    rd.stop = stop;
    rd.idx = thresholdIndex;
    rd.color = color;
    rd.hasColor = color.isValid() ? 1 : 0;
    m_ranges.append(rd);
    invalidateStatic_();
    update();
}

void TQtMeterGaugeAA::clearThresholdRanges()
{
    m_ranges.clear();
    invalidateStatic_();
    update();
}

TQSize TQtMeterGaugeAA::minimumSizeHint() const
{
    return TQSize((int)m_radius * 2 + 2, (int)m_radius * 2 + 2);
}

TQSize TQtMeterGaugeAA::sizeHint() const { return minimumSizeHint(); }

void TQtMeterGaugeAA::setValue(double v)
{
    if (m_maxValue <= m_minValue) return;

    const double nv = tqtmg_aa_clamp(v, m_minValue, m_maxValue);
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

void TQtMeterGaugeAA::setValueInt(int v)
{
    setValue((double)v);
}

int TQtMeterGaugeAA::thresholdIndexForValue(double v) const
{
    for (TQValueList<RangeData>::ConstIterator it = m_ranges.begin(); it != m_ranges.end(); ++it) {
        const RangeData& rd = *it;
        if ((v > rd.start || fabs(v - rd.start) < 0.000001) && v < rd.stop) return rd.idx;
    }
    return 0;
}

void TQtMeterGaugeAA::invalidateStatic_()
{
    m_staticValid = 0;
}

void TQtMeterGaugeAA::ensureStatic_()
{
    const int r = (int)m_radius;
    const int d = r * 2;

    if (m_staticValid && !m_staticImg.isNull() && m_staticImg.width() == d && m_staticImg.height() == d) return;

    m_staticImg = TQImage(d, d, 32);
    m_staticImg.setAlphaBuffer(true);
    m_staticImg.fill(0); // 0 = fully transparent ARGB

    const double scaleDegree = (double)((int)m_stopScaleAngle - (int)m_startScaleAngle);
    const double rangeSpan = (m_maxValue - m_minValue);
    if (rangeSpan <= 0.0000001) {
        m_staticValid = 1;
        return;
    }

    const double margin = (double)r / 20.0;
    const double gridLabelSize = (double)r / 10.0;
    const double scaleWidth = (double)r / (10.0 + 20.0);

    const int cx = r;
    const int cy = r;

    {
        const int rr = r - 1;
        int y;
        for (y = -rr; y <= rr; ++y) {
            const int yy = cy + y;
            const int xspan = (int)floor(sqrt((double)(rr * rr - y * y)) + 0.5);
            const int x0 = cx - xspan;
            const int x1 = cx + xspan;

            int x;
            for (x = x0; x <= x1; ++x) {
                if ((unsigned int)x >= (unsigned int)m_staticImg.width()) continue;
                if ((unsigned int)yy >= (unsigned int)m_staticImg.height()) continue;
                m_staticImg.setPixel(x, yy, m_backgroundColor.rgb());
            }
        }
    }

    TQtAAPainter::drawCircleAA(&m_staticImg, cx, cy, r - 1, m_gridColor, 0);

    {
        const int rr = (int)((double)r - margin - scaleWidth * 0.5);
        TQtAAPainter::drawArcAA(&m_staticImg, cx, cy, rr, (double)m_startScaleAngle, scaleDegree, m_textColor, 0, 1);
    }

    {
        for (TQValueList<RangeData>::ConstIterator it = m_ranges.begin(); it != m_ranges.end(); ++it) {
            const RangeData& rd = *it;
            if (!rd.hasColor) continue;

            const double a0 = (double)m_startScaleAngle + scaleDegree * ((rd.start - m_minValue) / rangeSpan);
            const double a1 = (double)m_startScaleAngle + scaleDegree * ((rd.stop - m_minValue) / rangeSpan);
            const double span = a1 - a0;

            const int rr = (int)((double)r - margin - scaleWidth * 0.5);
            int w = (int)(scaleWidth + 0.5);
            if (w < 1) w = 1;
            TQtAAPainter::drawArcAA(&m_staticImg, cx, cy, rr, a0, span, rd.color, 0, w);
        }
    }

    {
        if (m_scaleGridStep > 0.0) {
            const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
            const double deltaDeg = scaleDegree / rangeSpan * m_scaleGridStep;

            int i;
            for (i = 0; i <= stepsCount; ++i) {
                const double a = (double)m_startScaleAngle + (double)i * deltaDeg;
                const double rad = -tqtmg_aa_deg2rad(a);
                const double sina = sin(rad);
                const double cosa = cos(rad);

                const int x0 = cx + (int)floor(((double)r - margin) * sina + 0.5);
                const int y0 = cy + (int)floor(((double)r - margin) * cosa + 0.5);
                const int x1 = cx + (int)floor(((double)r - gridLabelSize - margin) * sina + 0.5);
                const int y1 = cy + (int)floor(((double)r - gridLabelSize - margin) * cosa + 0.5);

                TQtAAPainter::drawLineAA(&m_staticImg, x0, y0, x1, y1, m_gridColor, 1);
            }
        }
    }

    {
        if (m_scaleStep > 0.0) {
            int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleStep);
            if (stepsCount < 2) stepsCount = 2;

            const double stepDeg = scaleDegree / (double)stepsCount;
            stepsCount -= 2;

            int i;
            for (i = 0; i <= stepsCount; ++i) {
                const double a = (double)m_startScaleAngle + (double)(i + 1) * stepDeg;
                const double rad = -tqtmg_aa_deg2rad(a);
                const double sina = sin(rad);
                const double cosa = cos(rad);

                const int x0 = cx + (int)floor(((double)r - margin) * sina + 0.5);
                const int y0 = cy + (int)floor(((double)r - margin) * cosa + 0.5);
                const int x1 = cx + (int)floor(((double)r - scaleWidth - margin) * sina + 0.5);
                const int y1 = cy + (int)floor(((double)r - scaleWidth - margin) * cosa + 0.5);

                TQtAAPainter::drawLineAA(&m_staticImg, x0, y0, x1, y1, m_gridColor, 1);
            }
        }

        // --- NEW: Bake the 50 heavy text drawing operations directly into the static RAM Image ONCE! ---
        // Must use native depth TQPixmap, NOT 32-bit, because on older X11 composite servers depth 32 is undefined!
        TQPixmap txtBakePix(d, d);
        txtBakePix.fill(TQt::black); // Native fill
        
        {
            TQPainter tp(&txtBakePix);
            const double fontPixel = (double)r / 7.5;
            TQFont f = tp.font();
            f.setPixelSize((int)fontPixel);
            tp.setFont(f);
            TQFontMetrics fm(f);
            
            if (m_scaleGridStep > 0.0 && m_drawGridValues) {
                double startRad = -tqtmg_aa_deg2rad((double)m_startScaleAngle);
                const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
                const double fullSteps = ((m_maxValue - m_minValue) / m_scaleGridStep);
                const double deltaRad = -tqtmg_aa_deg2rad(scaleDegree / rangeSpan * m_scaleGridStep);
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

                    tp.setPen(m_textColor);
                    tp.drawText(x + r, y + r, str);

                    val += m_scaleGridStep;
                }

                if (fabs(fullSteps - (double)stepsCount) > 0.000001) {
                    const double rad = -tqtmg_aa_deg2rad((double)m_startScaleAngle + scaleDegree);
                    const double sina = sin(rad);
                    const double cosa = cos(rad);

                    const TQString str = TQString::number(m_maxValue, 'f', m_scalePrecision);
                    const TQSize s = fm.size(TQt::SingleLine, str);

                    const double off = (double)r - gridLabelSize - margin * 3.0;
                    const int x = (int)(off * sina - (double)s.width() / 2.0);
                    const int y = (int)(off * cosa + (double)s.height() / 4.0);

                    tp.setPen(m_textColor);
                    tp.drawText(x + r, y + r, str);
                }
            }
        }
        
        // Blend rasterized text onto the static background map without alpha loss!
        TQImage txtImg = txtBakePix.convertToImage();
        for (int y = 0; y < txtImg.height(); ++y) {
            for (int x = 0; x < txtImg.width(); ++x) {
                TQRgb pOld = txtImg.pixel(x, y);
                // If it isn't pure black, copy it directly onto the static gauge buffer
                if (pOld != tqRgb(0, 0, 0)) {
                    m_staticImg.setPixel(x, y, pOld);
                }
            }
        }
    }

    m_staticValid = 1;
}

void TQtMeterGaugeAA::paintEvent(TQPaintEvent*)
{
    // Re-lock WNoAutoErase immediately after any background-exposing event completes.
    // This utterly truncates the XServer native clearing gap directly after Tab Switches,
    // firmly securing the 50fps needle sweeping loop against all forms of Screen Tearing!
    if ((getWFlags() & TQt::WNoAutoErase) == 0) {
        setWFlags(getWFlags() | TQt::WNoAutoErase);
    }
    const int r = (int)m_radius;
    const int d = r * 2;

    ensureStatic_();

    if (m_frameImg.isNull() || m_frameImg.width() != d || m_frameImg.height() != d) {
        m_frameImg = TQImage(d, d, 32);
        m_frameImg.setAlphaBuffer(true);
    }

    if (!m_staticImg.isNull() && m_staticImg.width() == d && m_staticImg.height() == d) {
        const int nbytes = d * d * 4;
        memcpy(m_frameImg.bits(), m_staticImg.bits(), (size_t)nbytes);
    } else {
        m_frameImg.fill(0); // transparent fallback
    }

    const double scaleDegree = (double)((int)m_stopScaleAngle - (int)m_startScaleAngle);
    const double rangeSpan = (m_maxValue - m_minValue);

    // Calculate offset to draw precisely centered in the assigned layout allocated space
    int dx = (width() - (d + 2)) / 2;
    int dy = (height() - (d + 2)) / 2;
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;

    TQPainter p(this);
    
    // Fallback if range is 0
    if (rangeSpan <= 0.0000001) {
        p.drawImage(dx + 1, dy + 1, m_frameImg);
        return;
    }

    const double margin = (double)r / 20.0;
    const double gridLabelSize = (double)r / 10.0;
    const int cx = r;
    const int cy = r;

    // Draw the generated gauge frame (transparent corners, black core, needle)
    {
        const double angle = (double)m_startScaleAngle + scaleDegree * ((m_value - m_minValue) / rangeSpan);
        const double rad = -tqtmg_aa_deg2rad(angle);
        const double sina = sin(rad);
        const double cosa = cos(rad);

        const double needleLength = (double)r - gridLabelSize - margin * 1.5;
        const double needlePinRadius = (double)r / 10.0;
        const int x0 = cx;
        const int y0 = cy;
        const int x1 = cx + (int)floor(needleLength * sina + 0.5);
        const int y1 = cy + (int)floor(needleLength * cosa + 0.5);
        const int w = (r > 60) ? 3 : 2;

        // Origin tail line to perfectly center the pivot point
        const int tx = cx + (int)floor((-(needlePinRadius * 2.0)) * sina + 0.5);
        const int ty = cy + (int)floor((-(needlePinRadius * 2.0)) * cosa + 0.5);

        TQtAAPainter::drawLineAA(&m_frameImg, tx, ty, x1, y1, m_needleColor, w);
        TQtAAPainter::fillCircleAA(&m_frameImg, cx, cy, (int)needlePinRadius, TQColor(180, 180, 180));
        TQtAAPainter::fillCircleAA(&m_frameImg, cx, cy, (int)(needlePinRadius / 2.0), TQColor(120, 120, 120));
    }
    
    // Construct local ultra-fast double buffer purely encompassing the inner circular diameter safely mapped bounding it off layout glitches!
    TQPixmap gaugePix(d, d);
    {
        TQPainter bp(&gaugePix);
        
        // Ensure pure solid blackness interior via primitive native layout painting inside the mask domain
        bp.fillRect(0, 0, d, d, m_backgroundColor);

        // Blit transparent TQImage frame and text elements natively to the buffer
        bp.drawImage(0, 0, m_frameImg);

        const double fontPixel = (double)r / 7.5;
        TQFont f = bp.font();
        f.setPixelSize((int)fontPixel);

        if (!m_unitsLabel.isEmpty()) {
            TQFont fUnit = f;
            fUnit.setPixelSize((int)((double)r / 6.0));
            bp.setFont(fUnit);
            bp.setPen(m_textColor);
            bp.drawText(TQRect(0, (int)(margin * 2.0 + gridLabelSize * 2.0), d, (int)((double)r / 3.0)), TQt::AlignHCenter, m_unitsLabel);
            bp.setFont(f);
        }

        if (!m_label.isEmpty()) {
            TQFont fLabel = f;
            fLabel.setPixelSize((int)((double)r / 4.0));
            fLabel.setBold(true);
            bp.setFont(fLabel);
            bp.setPen(m_textColor);
            bp.drawText(TQRect(0, (int)r + (int)(margin * 2.5), d, (int)((double)r / 2.0)), TQt::AlignHCenter, m_label);
            bp.setFont(f);
        }

        if (m_drawValue) {
            TQFont vf = f;
            vf.setPixelSize((int)((double)r / 4.0));
            vf.setBold(true);
            bp.setFont(vf);
            bp.setPen(m_textColor);
            bp.drawText(TQRect(0, (int)(d - margin - gridLabelSize * 1.5), d, r), TQt::AlignHCenter,
                        TQString::number(m_value, 'f', m_valuePrecision));
            bp.setFont(f);
        }
    }

    // Circle mask: recompute only when diameter has changed (cached between frames)
    if (m_circleMaskDiameter != d) {
        m_circleMask = TQBitmap(d, d);
        m_circleMask.fill(color0);
        TQPainter mp(&m_circleMask);
        mp.setBrush(color1);
        mp.setPen(color1);
        mp.drawEllipse(0, 0, d, d);
        m_circleMaskDiameter = d;
    }
    gaugePix.setMask(m_circleMask);

    // Blit final masked gauge atop the native TDE-erased background
    p.drawPixmap(dx + 1, dy + 1, gaugePix);
}

void TQtMeterGaugeAA::resizeEvent(TQResizeEvent*)
{
    // Resizing scales correctly implicitly via native parent propagation
}

void TQtMeterGaugeAA::showEvent(TQShowEvent *e)
{
    TQWidget::showEvent(e);
    // Explicitly unseal WNoAutoErase ONLY during tab switch mappings!
    // This allows the ensuing XExpose hardware interrupt to physically clear and push the NightFly 
    // parent gradient directly over any legacy progress bar ghosts synchronously!
    setWFlags(getWFlags() & ~TQt::WNoAutoErase);
}
