#include "../tqtmetergauge.h"

#include <ntqapplication.h>
#include <ntqgroupbox.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqslider.h>
#include <ntqwidget.h>

#include <ntqfont.h>
#include <ntqfontmetrics.h>
#include <ntqpainter.h>
#include <ntqpen.h>

#include <ntqpixmap.h>

#include <ntqevent.h>

#include <ntqmemarray.h>

#include <string.h>

#include <math.h>

#include "tqtaapainter.h"

class TQEraseEvent;

static inline double tqtmg_deg2rad(double d) { return d * (3.14159265358979323846 / 180.0); }
static inline double tqtmg_clamp(double x, double a, double b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

class TQtMeterGaugeAA : public TQWidget {
    TQ_OBJECT
public:
    TQtMeterGaugeAA(TQWidget* parent = 0)
        : TQWidget(parent),
          m_radius(150),
          m_startScaleAngle(35),
          m_stopScaleAngle(325),
          m_minValue(0.0),
          m_maxValue(220.0),
          m_value(90.0),
          m_scaleStep(2.0),
          m_scaleGridStep(10.0),
          m_backgroundColor(TQColor(0, 0, 0)),
          m_needleColor(TQColor(0, 0, 255)),
          m_textColor(TQColor(255, 255, 255)),
          m_gridColor(TQColor(255, 255, 255)),
          m_label("speed"),
          m_unitsLabel("km/h"),
          m_drawValue(1),
          m_drawGridValues(1),
          m_valuePrecision(0),
          m_scalePrecision(0),
          m_staticValid(0)
    {
        setBackgroundMode(TQt::NoBackground);
        setWFlags(getWFlags() | TQt::WNoAutoErase);
        setMinimumSize((int)m_radius * 2 + 2, (int)m_radius * 2 + 2);
        setMaximumSize((int)m_radius * 2 + 2, (int)m_radius * 2 + 2);
        setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);
        resize(sizeHint());
    }

public:

    void setRadius(unsigned int r) {
        if (r < 45U) r = 45U;
        m_radius = r;
        m_staticValid = 0;
        resize(sizeHint());
        update();
    }

    TQSize sizeHint() const { return TQSize((int)m_radius * 2 + 2, (int)m_radius * 2 + 2); }

    void setMinValue(double v) { m_minValue = v; if (m_minValue > m_maxValue) m_maxValue = m_minValue; m_staticValid = 0; update(); }
    void setMaxValue(double v) { m_maxValue = v; if (m_maxValue < m_minValue) m_minValue = m_maxValue; m_staticValid = 0; update(); }

    void setValue(double v) {
        if (m_maxValue <= m_minValue) return;
        const double nv = tqtmg_clamp(v, m_minValue, m_maxValue);
        if (fabs(nv - m_value) < 0.0000001) return;
        m_value = nv;
        update();
    }

    void setStartScaleAngle(int a) { m_startScaleAngle = a; m_staticValid = 0; update(); }
    void setStopScaleAngle(int a) { m_stopScaleAngle = a; m_staticValid = 0; update(); }

    void setScaleStep(double s) { m_scaleStep = s; m_staticValid = 0; update(); }
    void setScaleGridStep(double s) { m_scaleGridStep = s; m_staticValid = 0; update(); }

    void setBackgroundColor(const TQColor& c) { m_backgroundColor = c; m_staticValid = 0; update(); }
    void setNeedleColor(const TQColor& c) { m_needleColor = c; update(); }
    void setTextColor(const TQColor& c) { m_textColor = c; m_staticValid = 0; update(); }
    void setGridColor(const TQColor& c) { m_gridColor = c; m_staticValid = 0; update(); }

    void setLabel(const TQString& s) { m_label = s; update(); }
    void setUnitsLabel(const TQString& s) { m_unitsLabel = s; update(); }
    void setDrawValue(int on) { m_drawValue = on; update(); }
    void setDrawGridValues(int on) { m_drawGridValues = on; m_staticValid = 0; update(); }
    void setValuePrecision(int p) { m_valuePrecision = p; update(); }
    void setScalePrecision(int p) { m_scalePrecision = p; update(); }

protected:
    void paintEvent(TQPaintEvent*)
    {
        const int r = (int)m_radius;
        const int d = r * 2;

        ensureStatic_();

        if (m_frameImg.isNull() || m_frameImg.width() != d || m_frameImg.height() != d) {
            m_frameImg = TQImage(d, d, 32);
        }

        if (!m_staticImg.isNull() && m_staticImg.width() == d && m_staticImg.height() == d) {
            const int nbytes = d * d * 4;
            memcpy(m_frameImg.bits(), m_staticImg.bits(), (size_t)nbytes);
        } else {
            m_frameImg.fill(colorGroup().brush(TQColorGroup::Background).color().rgb());
        }

        const double scaleDegree = (double)((int)m_stopScaleAngle - (int)m_startScaleAngle);
        const double rangeSpan = (m_maxValue - m_minValue);
        if (rangeSpan <= 0.0000001) {
            if (m_backPix.isNull() || m_backPix.width() != d + 2 || m_backPix.height() != d + 2) {
                m_backPix = TQPixmap(d + 2, d + 2);
            }

            {
                TQPainter bp(&m_backPix);
                bp.fillRect(0, 0, d + 2, d + 2, colorGroup().brush(TQColorGroup::Background));
                bp.drawImage(1, 1, m_frameImg);
            }

            TQPainter p(this);
            p.drawPixmap(0, 0, m_backPix);
            return;
        }

        const double margin = (double)r / 20.0;
        const double gridLabelSize = (double)r / 10.0;
        const double scaleWidth = (double)r / (10.0 + 20.0);

        const int cx = r;
        const int cy = r;

        {
            const double angle = (double)m_startScaleAngle + scaleDegree * ((m_value - m_minValue) / rangeSpan);
            const double rad = -tqtmg_deg2rad(angle);
            const double sina = sin(rad);
            const double cosa = cos(rad);

            const int x0 = cx + (int)floor((-(double)r / 10.0 * 2.0) * sina + 0.5);
            const int y0 = cy + (int)floor((-(double)r / 10.0 * 2.0) * cosa + 0.5);
            const int x1 = cx + (int)floor(((double)r - margin) * sina + 0.5);
            const int y1 = cy + (int)floor(((double)r - margin) * cosa + 0.5);

            TQtAAPainter::drawLineAA(&m_frameImg, x0, y0, x1, y1, m_needleColor, 3);

            TQtAAPainter::fillCircleAA(&m_frameImg, cx, cy, (int)((double)r / 10.0), TQColor(180, 180, 180));
            TQtAAPainter::fillCircleAA(&m_frameImg, cx, cy, (int)((double)r / 20.0), TQColor(120, 120, 120));
        }

        if (m_backPix.isNull() || m_backPix.width() != d + 2 || m_backPix.height() != d + 2) {
            m_backPix = TQPixmap(d + 2, d + 2);
        }

        {
            TQPainter bp(&m_backPix);
            bp.fillRect(0, 0, d + 2, d + 2, colorGroup().brush(TQColorGroup::Background));
            bp.drawImage(1, 1, m_frameImg);

            const double fontPixel = (gridLabelSize * 0.75);
            TQFont f = bp.font();
            f.setPixelSize((int)fontPixel);
            bp.setFont(f);
            TQFontMetrics fm(f);

            if (m_scaleGridStep > 0.0 && m_drawGridValues) {
                const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
                const double deltaRad = -tqtmg_deg2rad(scaleDegree / rangeSpan * m_scaleGridStep);
                double startRad = -tqtmg_deg2rad((double)m_startScaleAngle);
                double val = m_minValue;

                int i;
                for (i = 0; i <= stepsCount; ++i) {
                    const double ang = startRad + (double)i * deltaRad;
                    const double sina = sin(ang);
                    const double cosa = cos(ang);

                    const TQString str = TQString::number(val, 'f', m_scalePrecision);
                    const TQSize s = fm.size(TQt::SingleLine, str);

                    const double off = (double)r - gridLabelSize - margin * 3.0;
                    const int x = (int)(off * sina - (double)s.width() / 2.0);
                    const int y = (int)(off * cosa + (double)s.height() / 4.0);

                    bp.setPen(m_textColor);
                    bp.drawText(1 + x + r, 1 + y + r, str);

                    val += m_scaleGridStep;
                }
            }

            if (!m_unitsLabel.isEmpty()) {
                bp.setPen(m_textColor);
                bp.drawText(TQRect(1, 1 + (int)(margin * 3.0 + gridLabelSize * 3.0), d, r), TQt::AlignHCenter, m_unitsLabel);
            }

            if (!m_label.isEmpty()) {
                bp.setPen(m_textColor);
                bp.drawText(TQRect(1, 1 + (int)(d - margin * 3.0 - gridLabelSize * 3.0), d, r), TQt::AlignHCenter, m_label);
            }

            if (m_drawValue) {
                TQFont vf = f;
                vf.setPixelSize((int)(fontPixel * 2.0));
                vf.setBold(true);
                bp.setFont(vf);
                bp.setPen(m_textColor);
                bp.drawText(TQRect(1, 1 + (int)(d - margin - gridLabelSize * 2.0), d, r), TQt::AlignHCenter,
                            TQString::number(m_value, 'f', m_valuePrecision));
                bp.setFont(f);
            }
        }

        TQPainter p(this);
        p.drawPixmap(0, 0, m_backPix);
    }

private:
    void ensureStatic_()
    {
        const int r = (int)m_radius;
        const int d = r * 2;

        if (m_staticValid && !m_staticImg.isNull() && m_staticImg.width() == d && m_staticImg.height() == d) return;

        m_staticImg = TQImage(d, d, 32);
        m_staticImg.fill(colorGroup().brush(TQColorGroup::Background).color().rgb());

        const int cx = r;
        const int cy = r;

        const double scaleDegree = (double)((int)m_stopScaleAngle - (int)m_startScaleAngle);
        const double rangeSpan = (m_maxValue - m_minValue);
        if (rangeSpan <= 0.0000001) {
            m_staticValid = 1;
            return;
        }

        const double margin = (double)r / 20.0;
        const double gridLabelSize = (double)r / 10.0;
        const double scaleWidth = (double)r / (10.0 + 20.0);

        // Dial disk (solid fill)
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
            if (m_scaleGridStep > 0.0) {
                const int stepsCount = (int)((m_maxValue - m_minValue) / m_scaleGridStep);
                const double deltaDeg = scaleDegree / rangeSpan * m_scaleGridStep;

                int i;
                for (i = 0; i <= stepsCount; ++i) {
                    const double a = (double)m_startScaleAngle + (double)i * deltaDeg;
                    const double rad = -tqtmg_deg2rad(a);
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

                // Skip first/last to match original.
                stepsCount -= 2;

                int i;
                for (i = 0; i <= stepsCount; ++i) {
                    const double a = (double)m_startScaleAngle + (double)(i + 1) * stepDeg;
                    const double rad = -tqtmg_deg2rad(a);
                    const double sina = sin(rad);
                    const double cosa = cos(rad);

                    const int x0 = cx + (int)floor(((double)r - margin) * sina + 0.5);
                    const int y0 = cy + (int)floor(((double)r - margin) * cosa + 0.5);
                    const int x1 = cx + (int)floor(((double)r - scaleWidth - margin) * sina + 0.5);
                    const int y1 = cy + (int)floor(((double)r - scaleWidth - margin) * cosa + 0.5);

                    TQtAAPainter::drawLineAA(&m_staticImg, x0, y0, x1, y1, m_gridColor, 1);
                }
            }
        }

        m_staticValid = 1;
    }

private:
    unsigned int m_radius;
    unsigned int m_startScaleAngle;
    unsigned int m_stopScaleAngle;

    double m_minValue;
    double m_maxValue;
    double m_value;

    double m_scaleStep;
    double m_scaleGridStep;

    TQColor m_backgroundColor;
    TQColor m_needleColor;
    TQColor m_textColor;
    TQColor m_gridColor;

    TQString m_label;
    TQString m_unitsLabel;

    int m_drawValue;
    int m_drawGridValues;
    int m_valuePrecision;
    int m_scalePrecision;

    TQImage m_staticImg;
    TQImage m_frameImg;
    TQPixmap m_backPix;
    int m_staticValid;
};

class Demo : public TQObject {
    TQ_OBJECT
public:
    Demo(TQtMeterGauge* legacy, TQtMeterGaugeAA* aa)
        : m_legacy(legacy), m_aa(aa)
    {
    }

public slots:
    void onValueChanged(int v)
    {
        const double t = (double)v / 1000.0;
        const double val = m_legacy->minValue() + t * (m_legacy->maxValue() - m_legacy->minValue());
        if (m_legacy) m_legacy->setValue(val);
        if (m_aa) m_aa->setValue(val);
    }

private:
    TQtMeterGauge* m_legacy;
    TQtMeterGaugeAA* m_aa;
};

class MainWidget : public TQWidget {
public:
    MainWidget()
        : TQWidget(0, 0, TQt::WNoAutoErase)
    {
        setBackgroundMode(TQt::NoBackground);
    }

protected:
    void eraseEvent(TQEraseEvent*)
    {
    }
};

int main(int argc, char** argv)
{
    TQApplication app(argc, argv);

    MainWidget w;
    app.setMainWidget(&w);
    TQObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    TQVBoxLayout* l = new TQVBoxLayout(&w);

    TQHBoxLayout* row = new TQHBoxLayout();
    l->addLayout(row);

    TQGroupBox* gb0 = new TQGroupBox("Legacy (TQtMeterGauge)", &w);
    TQVBoxLayout* gl0 = new TQVBoxLayout(gb0);
    TQtMeterGauge* g0 = new TQtMeterGauge(gb0);

    g0->setMinValue(0.0);
    g0->setMaxValue(220.0);
    g0->setValue(90.0);
    g0->setBackgroundColor(TQColor(0, 0, 0));
    g0->setNeedleColor(TQColor(0, 0, 255));
    g0->setTextColor(TQColor(255, 255, 255));
    g0->setGridColor(TQColor(255, 255, 255));
    g0->setLabel("speed");
    g0->setUnitsLabel("km/h");
    g0->setRadius(150);
    g0->setStartScaleAngle(35);
    g0->setStopScaleAngle(325);
    g0->setScaleStep(2.0);
    g0->setScaleGridStep(10.0);
    g0->setDrawValue(true);
    g0->setDrawGridValues(true);
    g0->setDrawValuePrecision(0);
    g0->setScaleLabelPrecision(0);

    gl0->addWidget(g0);

    TQGroupBox* gb1 = new TQGroupBox("AA (aa-painter)", &w);
    TQVBoxLayout* gl1 = new TQVBoxLayout(gb1);
    TQtMeterGaugeAA* g1 = new TQtMeterGaugeAA(gb1);

    g1->setMinValue(0.0);
    g1->setMaxValue(220.0);
    g1->setValue(90.0);
    g1->setBackgroundColor(TQColor(0, 0, 0));
    g1->setNeedleColor(TQColor(0, 0, 255));
    g1->setTextColor(TQColor(255, 255, 255));
    g1->setGridColor(TQColor(255, 255, 255));
    g1->setLabel("speed");
    g1->setUnitsLabel("km/h");
    g1->setRadius(150);
    g1->setStartScaleAngle(35);
    g1->setStopScaleAngle(325);
    g1->setScaleStep(2.0);
    g1->setScaleGridStep(10.0);
    g1->setDrawValue(true);
    g1->setDrawGridValues(true);
    g1->setValuePrecision(0);
    g1->setScalePrecision(0);

    gl1->addWidget(g1);

    row->addWidget(gb0);
    row->addWidget(gb1);

    TQSlider* s = new TQSlider(TQt::Horizontal, &w);
    s->setMinValue(0);
    s->setMaxValue(1000);
    s->setValue(409);
    l->addWidget(s);

    Demo d(g0, g1);
    TQObject::connect(s, SIGNAL(valueChanged(int)), &d, SLOT(onValueChanged(int)));

    w.show();

    return app.exec();
}

#include "meter_gauge_aa_main.moc"
