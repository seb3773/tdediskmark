#ifndef TQTMETERGAUGE_H
#define TQTMETERGAUGE_H

#include <ntqwidget.h>
#include <ntqcolor.h>
#include <ntqstring.h>

#include <ntqvaluelist.h>

class TQtMeterGauge : public TQWidget {
    TQ_OBJECT
public:
    TQtMeterGauge(TQWidget* parent = 0, const char* name = 0);

    double minValue() const;
    void setMinValue(double v);

    double maxValue() const;
    void setMaxValue(double v);

    double value() const;

    const TQColor& backgroundColor() const;
    void setBackgroundColor(const TQColor& c);

    const TQColor& needleColor() const;
    void setNeedleColor(const TQColor& c);

    const TQColor& textColor() const;
    void setTextColor(const TQColor& c);

    const TQColor& gridColor() const;
    void setGridColor(const TQColor& c);

    const TQString& label() const;
    void setLabel(const TQString& l);

    const TQString& unitsLabel() const;
    void setUnitsLabel(const TQString& l);

    unsigned int radius() const;
    void setRadius(unsigned int r);

    unsigned int startScaleAngle() const;
    void setStartScaleAngle(unsigned int a);

    unsigned int stopScaleAngle() const;
    void setStopScaleAngle(unsigned int a);

    double scaleStep() const;
    void setScaleStep(double s);

    double scaleGridStep() const;
    void setScaleGridStep(double s);

    bool drawValue() const;
    void setDrawValue(bool on);

    bool drawGridValues() const;
    void setDrawGridValues(bool on);

    int drawValuePrecision() const;
    void setDrawValuePrecision(int p);

    int scaleLabelPrecision() const;
    void setScaleLabelPrecision(int p);

    void setThresholdRange(double start, double stop, int thresholdIndex, const TQColor& color = TQColor());
    void clearThresholdRanges();

    TQSize minimumSizeHint() const;
    TQSize sizeHint() const;

signals:
    void valueChanged(double currentValue);
    void thresholdFired(int thresholdIndex);

public slots:
    void setValue(double v);
    void setValueInt(int v);

protected:
    void paintEvent(TQPaintEvent*);

private:
    struct RangeData {
        double start;
        double stop;
        int idx;
        TQColor color;
        int hasColor;
    };

    int thresholdIndexForValue(double v) const;

private:
    int m_drawValue;
    int m_drawGridValues;
    int m_valuePrecision;
    int m_scalePrecision;
    int m_currentThreshold;

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

    TQValueList<RangeData> m_ranges;
};

#endif
