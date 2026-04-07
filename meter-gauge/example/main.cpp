#include "../tqtmetergauge.h"

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqslider.h>
#include <ntqwidget.h>

class Demo : public TQObject {
    TQ_OBJECT
public:
    Demo(TQtMeterGauge* a, TQtMeterGauge* b, TQtMeterGauge* c, TQtMeterGauge* d)
        : m0(a), m1(b), m2(c), m3(d) {}

public slots:
    void onValueChanged(int v) {
        const double t = (double)v / 1000.0;

        if (m0) m0->setValue(m0->minValue() + t * (m0->maxValue() - m0->minValue()));
        if (m1) m1->setValue(m1->minValue() + t * (m1->maxValue() - m1->minValue()));
        if (m2) m2->setValue(m2->minValue() + t * (m2->maxValue() - m2->minValue()));
        if (m3) m3->setValue(m3->minValue() + t * (m3->maxValue() - m3->minValue()));
    }

private:
    TQtMeterGauge* m0;
    TQtMeterGauge* m1;
    TQtMeterGauge* m2;
    TQtMeterGauge* m3;
};

int main(int argc, char** argv) {
    TQApplication app(argc, argv);

    TQWidget w;
    app.setMainWidget(&w);
    TQObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    TQVBoxLayout* l = new TQVBoxLayout(&w);

    TQGridLayout* grid = new TQGridLayout(2, 2, 8);
    l->addLayout(grid);

    TQWidget* cell0 = new TQWidget(&w);
    TQVBoxLayout* c0 = new TQVBoxLayout(cell0);
    c0->addWidget(new TQLabel("Classic", cell0));
    TQtMeterGauge* m0 = new TQtMeterGauge(cell0);
    m0->setMinValue(0.0);
    m0->setMaxValue(220.0);
    m0->setValue(90.0);
    m0->setBackgroundColor(TQColor(0, 0, 0));
    m0->setNeedleColor(TQColor(0, 0, 255));
    m0->setTextColor(TQColor(255, 255, 255));
    m0->setGridColor(TQColor(255, 255, 255));
    m0->setLabel("speed");
    m0->setUnitsLabel("km/h");
    m0->setRadius(150);
    m0->setStartScaleAngle(35);
    m0->setStopScaleAngle(325);
    m0->setScaleStep(2.0);
    m0->setScaleGridStep(10.0);
    m0->setDrawValue(true);
    m0->setDrawGridValues(true);
    m0->setDrawValuePrecision(0);
    m0->setScaleLabelPrecision(0);
    m0->setThresholdRange(0.0, 110.0, 0);
    m0->setThresholdRange(110.0, 130.0, 1, TQColor(255, 255, 0));
    m0->setThresholdRange(130.0, 220.1, 2, TQColor(255, 0, 0));
    c0->addWidget(m0);

    TQWidget* cell1 = new TQWidget(&w);
    TQVBoxLayout* c1 = new TQVBoxLayout(cell1);
    c1->addWidget(new TQLabel("Light", cell1));
    TQtMeterGauge* m1 = new TQtMeterGauge(cell1);
    m1->setMinValue(0.0);
    m1->setMaxValue(100.0);
    m1->setValue(40.0);
    m1->setBackgroundColor(TQColor(245, 245, 245));
    m1->setNeedleColor(TQColor(180, 20, 20));
    m1->setTextColor(TQColor(10, 10, 10));
    m1->setGridColor(TQColor(60, 60, 60));
    m1->setLabel("load");
    m1->setUnitsLabel("%");
    m1->setRadius(150);
    m1->setStartScaleAngle(45);
    m1->setStopScaleAngle(315);
    m1->setScaleStep(1.0);
    m1->setScaleGridStep(10.0);
    m1->setDrawValue(true);
    m1->setDrawGridValues(true);
    m1->setDrawValuePrecision(0);
    m1->setScaleLabelPrecision(0);
    m1->setThresholdRange(0.0, 70.0, 0, TQColor(0, 160, 0));
    m1->setThresholdRange(70.0, 90.0, 1, TQColor(255, 170, 0));
    m1->setThresholdRange(90.0, 100.1, 2, TQColor(220, 0, 0));
    c1->addWidget(m1);

    TQWidget* cell2 = new TQWidget(&w);
    TQVBoxLayout* c2 = new TQVBoxLayout(cell2);
    c2->addWidget(new TQLabel("Minimal", cell2));
    TQtMeterGauge* m2 = new TQtMeterGauge(cell2);
    m2->setMinValue(-50.0);
    m2->setMaxValue(50.0);
    m2->setValue(0.0);
    m2->setBackgroundColor(TQColor(15, 15, 15));
    m2->setNeedleColor(TQColor(0, 200, 255));
    m2->setTextColor(TQColor(220, 220, 220));
    m2->setGridColor(TQColor(220, 220, 220));
    m2->setLabel(TQString());
    m2->setUnitsLabel(TQString());
    m2->setRadius(150);
    m2->setStartScaleAngle(60);
    m2->setStopScaleAngle(300);
    m2->setScaleStep(5.0);
    m2->setScaleGridStep(0.0);
    m2->setDrawValue(false);
    m2->setDrawGridValues(false);
    m2->setThresholdRange(-50.0, 50.1, 0, TQColor(80, 80, 80));
    c2->addWidget(m2);

    TQWidget* cell3 = new TQWidget(&w);
    TQVBoxLayout* c3 = new TQVBoxLayout(cell3);
    c3->addWidget(new TQLabel("Narrow sweep", cell3));
    TQtMeterGauge* m3 = new TQtMeterGauge(cell3);
    m3->setMinValue(0.0);
    m3->setMaxValue(5000.0);
    m3->setValue(1800.0);
    m3->setBackgroundColor(TQColor(0, 0, 0));
    m3->setNeedleColor(TQColor(255, 255, 255));
    m3->setTextColor(TQColor(255, 255, 255));
    m3->setGridColor(TQColor(255, 255, 255));
    m3->setLabel("rpm");
    m3->setUnitsLabel(TQString());
    m3->setRadius(150);
    m3->setStartScaleAngle(120);
    m3->setStopScaleAngle(240);
    m3->setScaleStep(250.0);
    m3->setScaleGridStep(1000.0);
    m3->setDrawValue(true);
    m3->setDrawGridValues(true);
    m3->setDrawValuePrecision(0);
    m3->setScaleLabelPrecision(0);
    m3->setThresholdRange(0.0, 3000.0, 0, TQColor(0, 160, 0));
    m3->setThresholdRange(3000.0, 4000.0, 1, TQColor(255, 170, 0));
    m3->setThresholdRange(4000.0, 5000.1, 2, TQColor(255, 0, 0));
    c3->addWidget(m3);

    grid->addWidget(cell0, 0, 0);
    grid->addWidget(cell1, 0, 1);
    grid->addWidget(cell2, 1, 0);
    grid->addWidget(cell3, 1, 1);

    TQSlider* s = new TQSlider(TQt::Horizontal, &w);
    s->setMinValue(0);
    s->setMaxValue(1000);
    s->setValue(409);

    Demo d(m0, m1, m2, m3);
    TQObject::connect(s, SIGNAL(valueChanged(int)), &d, SLOT(onValueChanged(int)));

    l->addWidget(s);

    w.show();
    return app.exec();
}

#include "meter_gauge_main.moc"
