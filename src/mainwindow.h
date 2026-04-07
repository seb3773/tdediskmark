#include <stdint.h>
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <tdemainwindow.h>
#include <tdemenubar.h>
#include <tqvaluevector.h>
#include <tqstring.h>
#include <tqaction.h>
#include <tqmenubar.h>
#include <tqpopupmenu.h>
#include <tqwidgetstack.h>
#include <tdepopupmenu.h>
#include <khelpmenu.h>
#include <kstdaction.h>
#include <tqpushbutton.h>
#include <tqspinbox.h>
#include <tqcombobox.h>
#include <tqlabel.h>
#include <tqlineedit.h>
#include <tqlayout.h>
#include <tqprogressbar.h>
#include <tqmenubar.h>
#include <tqprogressbar.h>
#include <tqpainter.h>
#include <tqcolor.h>
#include <functional>
#include <tqmap.h>
#include "../meter-gauge/tqtmetergauge_aa.h"
typedef TQtMeterGaugeAA TQtMeterGauge;

#include "benchmark.h"
#include "appsettings.h"

class BenchmarkProgressBar : public TQProgressBar {
    TQ_OBJECT
public:
    BenchmarkProgressBar(TQWidget* parent=0) : TQProgressBar(parent), MBPerSec(0), GBPerSec(0), IOPS(0), Latency(0), targetMBPerSec(0), targetGBPerSec(0), targetIOPS(0), targetLatency(0), isDemo(false), m_barColor(50, 180, 80) {
        setCenterIndicator(true);
    }
    double MBPerSec;
    double GBPerSec;
    double IOPS;
    double Latency;
    
    double targetMBPerSec;
    double targetGBPerSec;
    double targetIOPS;
    double targetLatency;
    
    bool isDemo;
    Global::BenchmarkTest testType;
    Global::BenchmarkIOReadWrite readWriteType;

    TQString customText;
    TQColor m_barColor;

    void setBarColor(const TQColor& c) { m_barColor = c; update(); }

    void setFormatText(const TQString &text) {
        customText = text;
        // Force TQt3 to invalidate its cached pixmap by toggling progress
        int p = progress();
        setProgress(p == 0 ? 1 : 0);
        setProgress(p);
    }

protected:
    bool setIndicator( TQString & indicator, int progress, int total ) {
        if (!customText.isEmpty()) {
            indicator = customText;
            return true;
        }
        return TQProgressBar::setIndicator(indicator, progress, total);
    }

    void paintEvent(TQPaintEvent*) {
        TQPainter p(this);
        TQRect wr = rect();

        // Draw sunken frame border (3D effect)
        p.setPen(colorGroup().dark());
        p.drawLine(wr.x(), wr.y(), wr.right(), wr.y());                   // top
        p.drawLine(wr.x(), wr.y(), wr.x(), wr.bottom());                  // left
        p.setPen(colorGroup().light());
        p.drawLine(wr.x(), wr.bottom(), wr.right(), wr.bottom());         // bottom
        p.drawLine(wr.right(), wr.y(), wr.right(), wr.bottom());          // right

        // Content area inside the frame
        TQRect cr(wr.x() + 2, wr.y() + 2, wr.width() - 4, wr.height() - 4);
        const int total = totalSteps();
        const int prog = progress();
        double frac = (total > 0) ? (double)prog / (double)total : 0.0;
        int w = (int)(cr.width() * frac);

        // Draw filled portion
        p.fillRect(cr.x(), cr.y(), w, cr.height(), m_barColor);
        // Draw empty portion
        p.fillRect(cr.x() + w, cr.y(), cr.width() - w, cr.height(), colorGroup().background());

        // Draw text centered
        if (centerIndicator() && !progressString().isEmpty()) {
            p.setPen(colorGroup().text());
            p.drawText(cr, TQt::AlignCenter, progressString());
        }
    }
};

class MainWindow : public TDEMainWindow
{
    TQ_OBJECT

public:
    MainWindow(TQWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Test_1_clicked();
    void on_pushButton_Test_2_clicked();
    void on_pushButton_Test_3_clicked();
    void on_pushButton_Test_4_clicked();
    void on_pushButton_All_clicked();

    void on_actionAbout_triggered();
    void on_actionHelpContents_triggered();
    void on_actionSettings_triggered();

    void on_comboBox_Storages_activated(int index);
    void on_loopsCount_valueChanged(int arg1);
    void on_comboBox_ComparisonUnit_activated(int index);
    void on_actionCopy_triggered();
    void on_actionSave_triggered();
    void on_comboBox_MixRatio_activated(int index);
    
    void animateProgressBars();
    void snapProgressBars();
    void on_refreshStoragesButton_clicked();
    void on_comboBox_fileSize_activated(int index);

    void on_actionFlush_Pagecache_triggered(bool checked);
    void on_actionUse_O_DIRECT_triggered(bool checked);
    void on_actionTestData_Continuous_triggered(bool checked);
    void on_actionCoW_detection_triggered(bool checked);
    void on_actionUseGauges_toggled(bool checked);
    void on_actionNotifyOnCompletion_toggled(bool checked);

public slots:
    void handleDirectoryChanged(const TQString &newDir);
    void benchmarkStatusUpdate(const TQString &name);
    void benchmarkFailed(const TQString &error);
    void handleResults(BenchmarkProgressBar *progressBar, const Benchmark::PerformanceResult &result);
    void onCurrentTestRunning(Global::BenchmarkTest test);
    
    void localeSelected(TQAction* act);
    void profileSelected(TQAction* act);
    void modeSelected(TQAction* act);
    void testDataSelected(TQAction* act);
    void presetSelected(TQAction* act);
    void themeSelected(TQAction* act);

    void benchmarkStateChanged(bool state);
    void pulseStopButtons();


protected:
    virtual void changeEvent(TQEvent * event);
    virtual void closeEvent(TQCloseEvent *event);

private:
    Benchmark *m_benchmark;
    TQValueVector<BenchmarkProgressBar*> m_progressBars;

    TQTimer* m_animationTimer;
    TQValueVector<TQString> m_storagePaths;
    TQString m_windowTitle;
    bool m_isRunningAll;
    TQString m_baseDeviceModelText;

    // --- UI Pointers ---
    TQWidget *centralwidget;
    
    // Menus
    KMenuBar *menubar;
    TDEPopupMenu *menuFile;
    TDEPopupMenu *menuSettings;
    TDEPopupMenu *menuProfile;
    TDEPopupMenu *menuTest_Data;
    TDEPopupMenu *menuPreset;
    TDEPopupMenu *menuTheme;
    TDEPopupMenu *menuHelp;

    // Actions
    TQAction *actionCopy;
    TQAction *actionSave;
    
    TQAction *actionDefault;
    TQAction *actionPeak_Performance;
    TQAction *actionReal_World_Performance;
    TQAction *actionDemo;
    TQAction *actionDefault_Mix;
    TQAction *actionPeak_Performance_Mix;
    TQAction *actionReal_World_Performance_Mix;
    
    TQAction *actionRead_Write_Mix;
    TQAction *actionRead_Mix;
    TQAction *actionWrite_Mix;

    TQAction *actionTestData_Random;
    TQAction *actionTestData_Zeros;
    TQAction *actionTestData_Continuous;

    TQAction *actionPreset_Standard;
    TQAction *actionPreset_NVMe_SSD;

    TQAction *actionUse_O_DIRECT;
    TQAction *actionFlush_Pagecache;
    TQAction *actionCoW_detection;

    TQAction *actionTheme_Orange;
    TQAction *actionTheme_Blue;
    TQAction *actionTheme_None;
    TQAction *actionTheme_Use_Gauges;
    TQAction *actionNotifyOnCompletion;

    // Top Control Bar
    TQSpinBox *loopsCount;
    TQComboBox *comboBox_fileSize;
    TQComboBox *comboBox_Storages;
    TQPushButton *refreshStoragesButton;
    TQComboBox *comboBox_ComparisonUnit;
    TQComboBox *comboBox_MixRatio;

    // Benchmark Triggers
    TQPushButton *pushButton_All;
    TQPushButton *pushButton_Test_1;
    TQPushButton *pushButton_Test_2;
    TQPushButton *pushButton_Test_3;
    TQPushButton *pushButton_Test_4;

    // Read Widgets
    TQLabel *label_Read;
    BenchmarkProgressBar *readBar_1;
    BenchmarkProgressBar *readBar_2;
    BenchmarkProgressBar *readBar_3;
    BenchmarkProgressBar *readBar_4;

    // Write Widgets
    TQLabel *label_Write;
    BenchmarkProgressBar *writeBar_1;
    BenchmarkProgressBar *writeBar_2;
    BenchmarkProgressBar *writeBar_3;
    BenchmarkProgressBar *writeBar_4;

    // Mix Widgets
    TQWidget *mixWidget;
    TQLabel *label_Mix;
    BenchmarkProgressBar *mixBar_1;
    BenchmarkProgressBar *mixBar_2;
    BenchmarkProgressBar *mixBar_3;
    BenchmarkProgressBar *mixBar_4;

    // Mix column stacks (to show/hide entire column)
    TQWidgetStack *cell_m1, *cell_m2, *cell_m3, *cell_m4;

    // Grid layout reference (for dynamic column stretch)
    TQGridLayout *m_gridLayout;

    // Demo Widgets (Off-grid but instantiated)
    TQWidgetStack *stackedWidget;
    TQLabel *label_Demo;
    TQLabel *label_Unit_Read_Demo;
    TQLabel *label_Unit_Write_Demo;
    BenchmarkProgressBar *readBar_Demo;
    BenchmarkProgressBar *writeBar_Demo;
    TQWidgetStack *demoReadStack;
    TQWidgetStack *demoWriteStack;

    // Bottom Bar
    TQLabel *extraIcon;
    TQLineEdit *deviceModel;

    // --- Private Methods ---
    void setupUi(); // Hand-written C++ layout builder
    void updateFileSizeList();
    void updateStoragesList();
    void addItemToStoragesList(const Global::Storage &storage);
    void defineBenchmark(std::function<void()> bodyFunc);
    TQString formatSize(uint64_t available, uint64_t total);
    TQString getTextBenchmarkResult();
    void updateBenchmarkButtonsContent();
    void updatePresetsSelection();
    void refreshProgressBars();
    void updateProgressBar(BenchmarkProgressBar *progressBar);
    void updateLabels();
    bool runCombinedRandomTest();
    TQString combineOutputTestResult(const BenchmarkProgressBar *progressBar, const Global::BenchmarkParams &params);
    void resizeComboBoxItemsPopup(TQComboBox *combobox);
    void updateProgressBarsStyle();
    void rebuildGauges();
    TQtMeterGauge* createGaugeForBar(BenchmarkProgressBar* bar, TQWidget* parent);
    void updateGauge(BenchmarkProgressBar* bar);
    void applyGaugeStyleForUnit(TQtMeterGauge* g, Global::ComparisonUnit unit);
    void applyThemeBackground();
    void applyThemeColors();

    TQMap<BenchmarkProgressBar*, TQtMeterGauge*> m_gaugeMap;
    
    BenchmarkProgressBar* createProgressBar(TQWidget* parent = 0);
    TQPushButton* createTestButton(const TQString& title, TQWidget* parent = 0);

    TQTimer *m_pulseTimer;
    int m_pulsePhase;
    bool m_benchmarkCancelled;

    // Theme colors
    bool m_hasThemeColors;
    TQColor m_textColor1, m_textColor2;
    TQColor m_bgColor1, m_bgColor2, m_bgColor3;
};

#endif // MAINWINDOW_H
