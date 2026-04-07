#include <libnotify/notify.h>
#include "mainwindow.h"

#include <tqmessagebox.h>
#include <mntent.h>
#include <sys/statvfs.h>
#include <tqclipboard.h>
#include <tqdatetime.h>
#include <tqfiledialog.h>
#include <tqtextstream.h>
#include <tqstylefactory.h>
#include <tqtimer.h>
#include <tqapplication.h>
#include <tqtooltip.h>
#include <tqiconset.h>
#include <tqaction.h>
#include <tqwidgetstack.h>

#include "math.h"
#include <tdeaction.h>
#include <tdeglobal.h>
#include <tdeapplication.h>
#include "diskdriveinfo.h"
#include "settings.h"
#include "aboutdialog.h"
#include "tdediskmark_png.h"
#include "refresh_png.h"
#include "refresh2_png.h"
#include "race_png.h"
#include "fly_png.h"
#include "go_png.h"

#include "global.h"
#include <cmath>

BenchmarkProgressBar* MainWindow::createProgressBar(TQWidget* parent) {
    BenchmarkProgressBar* bar = new BenchmarkProgressBar(parent ? parent : centralwidget);
    bar->setCenterIndicator(true);
    bar->setMinimumSize(TQSize(220, 61));
    return bar;
}

TQPushButton* MainWindow::createTestButton(const TQString& title, TQWidget* parent) {
    TQPushButton* btn = new TQPushButton(title, parent ? parent : centralwidget);
    btn->setMinimumSize(TQSize(91, 61));
    return btn;
}

#include <tqpixmap.h>

void MainWindow::setupUi() {
    setCaption(m_windowTitle);
    resize(840, 720);

    TQPixmap appIcon;
    appIcon.loadFromData(___icons_tdediskmark_png, ___icons_tdediskmark_png_len);
    setIcon(appIcon);
        
    centralwidget = new TQWidget(this);
    setCentralWidget(centralwidget);
        
    actionDefault = new TQAction("Default", 0, this, "actionDefault"); actionDefault->setToggleAction(true);
    actionPeak_Performance = new TQAction("Peak Performance", 0, this, "actionPeak_Performance"); actionPeak_Performance->setToggleAction(true);
    actionReal_World_Performance = new TQAction("Real World Performance", 0, this, "actionReal_World_Performance"); actionReal_World_Performance->setToggleAction(true);
    actionDemo = new TQAction("Demo", 0, this, "actionDemo"); actionDemo->setToggleAction(true);
    actionDefault_Mix = new TQAction("Default [+Mix]", 0, this, "actionDefault_Mix"); actionDefault_Mix->setToggleAction(true);
    actionPeak_Performance_Mix = new TQAction("Peak Performance [+Mix]", 0, this, "actionPeak_Performance_Mix"); actionPeak_Performance_Mix->setToggleAction(true);
    actionReal_World_Performance_Mix = new TQAction("Real World Performance [+Mix]", 0, this, "actionReal_World_Performance_Mix"); actionReal_World_Performance_Mix->setToggleAction(true);

    actionRead_Write_Mix = new TQAction("Read & Write & Mix", 0, this, "actionRead_Write_Mix"); actionRead_Write_Mix->setToggleAction(true);
    actionRead_Mix = new TQAction("Read & Mix", 0, this, "actionRead_Mix"); actionRead_Mix->setToggleAction(true);
    actionWrite_Mix = new TQAction("Write & Mix", 0, this, "actionWrite_Mix"); actionWrite_Mix->setToggleAction(true);

    actionTestData_Random = new TQAction("Random", 0, this, "actionTestData_Random"); actionTestData_Random->setToggleAction(true);
    actionTestData_Zeros = new TQAction("Zeros", 0, this, "actionTestData_Zeros"); actionTestData_Zeros->setToggleAction(true);
    actionTestData_Continuous = new TQAction("Continuous", 0, this, "actionTestData_Continuous"); actionTestData_Continuous->setToggleAction(true);

    actionPreset_Standard = new TQAction("Standard", 0, this, "actionPreset_Standard"); actionPreset_Standard->setToggleAction(true);
    actionPreset_NVMe_SSD = new TQAction("NVMe SSD", 0, this, "actionPreset_NVMe_SSD"); actionPreset_NVMe_SSD->setToggleAction(true);

    actionUse_O_DIRECT = new TQAction("Use O_DIRECT", 0, this, "actionUse_O_DIRECT"); actionUse_O_DIRECT->setToggleAction(true);
    actionFlush_Pagecache = new TQAction("Flush Pagecache", 0, this, "actionFlush_Pagecache"); actionFlush_Pagecache->setToggleAction(true);
    actionCoW_detection = new TQAction("Detect CoW", 0, this, "actionCoW_detection"); actionCoW_detection->setToggleAction(true);
    
    actionTheme_Orange = new TQAction("Racing", 0, this, "actionTheme_Orange"); actionTheme_Orange->setToggleAction(true);
    actionTheme_Blue = new TQAction("Night Fly", 0, this, "actionTheme_Blue"); actionTheme_Blue->setToggleAction(true);
    actionTheme_None = new TQAction("None", 0, this, "actionTheme_None"); actionTheme_None->setToggleAction(true);
    actionTheme_Use_Gauges = new TQAction(tr("Use Gauges"), 0, this, "actionTheme_Use_Gauges"); actionTheme_Use_Gauges->setToggleAction(true);
    actionNotifyOnCompletion = new TQAction(tr("Notify on completion"), 0, this, "actionNotifyOnCompletion"); actionNotifyOnCompletion->setToggleAction(true);

    actionCopy = new TQAction(tr("Copy"), 0, this, "actionCopy");
    actionSave = new TQAction(tr("Save"), 0, this, "actionSave");

    menubar = menuBar();
    menuFile = new TDEPopupMenu(this);
    actionCopy->addTo(menuFile); actionSave->addTo(menuFile);
    menuFile->insertSeparator();
    KStdAction::quit(this, TQT_SLOT(close()), actionCollection())->plug(menuFile);
    
    menuTest_Data = new TDEPopupMenu(this); actionTestData_Random->addTo(menuTest_Data); actionTestData_Zeros->addTo(menuTest_Data); menuTest_Data->insertSeparator(); actionTestData_Continuous->addTo(menuTest_Data);
    menuPreset = new TDEPopupMenu(this); actionPreset_Standard->addTo(menuPreset); actionPreset_NVMe_SSD->addTo(menuPreset);
    
    menuSettings = new TDEPopupMenu(this); 
    menuSettings->insertItem(tr("Test Data"), menuTest_Data);
    menuSettings->insertSeparator();
    actionPreset_Standard->addTo(menuSettings);
    actionPreset_NVMe_SSD->addTo(menuSettings);
    menuSettings->insertSeparator();
    KStdAction::preferences(this, TQT_SLOT(on_actionSettings_triggered()), actionCollection())->plug(menuSettings);
    actionNotifyOnCompletion->addTo(menuSettings);
    menuSettings->insertSeparator();
    actionUse_O_DIRECT->addTo(menuSettings);
    actionFlush_Pagecache->addTo(menuSettings);
    actionCoW_detection->addTo(menuSettings);
    
    menuProfile = new TDEPopupMenu(this); 
    actionDefault->addTo(menuProfile); 
    actionPeak_Performance->addTo(menuProfile); 
    actionReal_World_Performance->addTo(menuProfile); 
    actionDemo->addTo(menuProfile); 
    actionDefault_Mix->addTo(menuProfile); 
    actionPeak_Performance_Mix->addTo(menuProfile); 
    actionReal_World_Performance_Mix->addTo(menuProfile);
    menuProfile->insertSeparator();
    actionRead_Write_Mix->addTo(menuProfile); 
    actionRead_Mix->addTo(menuProfile); 
    actionWrite_Mix->addTo(menuProfile);

    menuTheme = new TDEPopupMenu(this);
    actionTheme_Orange->addTo(menuTheme);
    actionTheme_Blue->addTo(menuTheme);
    actionTheme_None->addTo(menuTheme);
    menuTheme->insertSeparator();
    actionTheme_Use_Gauges->addTo(menuTheme);
    
    menuHelp = new TDEPopupMenu(this);
    KStdAction::helpContents(this, TQT_SLOT(on_actionHelpContents_triggered()), actionCollection())->plug(menuHelp);
    menuHelp->insertSeparator();
    TDEAction* actionAbout = KStdAction::aboutApp(this, TQT_SLOT(on_actionAbout_triggered()), actionCollection());
    TQPixmap aboutIcon;
    aboutIcon.loadFromData(___icons_tdediskmark_png, ___icons_tdediskmark_png_len);
    actionAbout->setIconSet(TQIconSet(aboutIcon));
    actionAbout->plug(menuHelp);

    menubar->insertItem(tr("&File"), menuFile);
    menubar->insertItem(tr("&Settings"), menuSettings);
    menubar->insertItem(tr("&Profile"), menuProfile);
    menubar->insertItem(tr("&Theme"), menuTheme);
    menubar->insertItem(tr("&Help"), menuHelp);

    TQVBoxLayout *mainLayout = new TQVBoxLayout(centralwidget, 11, 6);
    
    TQHBoxLayout *topLayout = new TQHBoxLayout();
    loopsCount = new TQSpinBox(1, 9, 1, centralwidget); loopsCount->setMinimumSize(TQSize(0, 32));
    comboBox_fileSize = new TQComboBox(centralwidget); comboBox_fileSize->setMinimumSize(TQSize(0, 32));
    comboBox_Storages = new TQComboBox(centralwidget); comboBox_Storages->setMinimumSize(TQSize(0, 32));
    refreshStoragesButton = new TQPushButton("", centralwidget); refreshStoragesButton->setMinimumSize(TQSize(32, 32));
    TQPixmap refreshIcon;
    refreshIcon.loadFromData(___icons_refresh_png, ___icons_refresh_png_len);
    refreshStoragesButton->setIconSet(TQIconSet(refreshIcon));
    comboBox_ComparisonUnit = new TQComboBox(centralwidget); comboBox_ComparisonUnit->setMinimumSize(TQSize(0, 32)); comboBox_ComparisonUnit->insertItem("MB/s"); comboBox_ComparisonUnit->insertItem("GB/s"); comboBox_ComparisonUnit->insertItem("IOPS"); comboBox_ComparisonUnit->insertItem(TQString(TQChar(0xB5)) + "s");
    comboBox_MixRatio = new TQComboBox(centralwidget); comboBox_MixRatio->setMinimumSize(TQSize(0, 32));
    
    pushButton_All = createTestButton("All");
    TQPixmap raceIcon;
    raceIcon.loadFromData(___icons_race_png, ___icons_race_png_len);
    pushButton_All->setIconSet(TQIconSet(raceIcon));
    topLayout->addWidget(pushButton_All);
    topLayout->addWidget(loopsCount); topLayout->addWidget(comboBox_fileSize); topLayout->addWidget(comboBox_Storages, 1); topLayout->addWidget(refreshStoragesButton); topLayout->addWidget(comboBox_ComparisonUnit); topLayout->addWidget(comboBox_MixRatio);
    mainLayout->addLayout(topLayout);

    stackedWidget = new TQWidgetStack(centralwidget);
    mainLayout->addWidget(stackedWidget);

    TQWidget *pageGrid = new TQWidget(stackedWidget);
    m_gridLayout = new TQGridLayout(pageGrid, 5, 4, 10);
    m_gridLayout->setColStretch(0, 0);
    m_gridLayout->setColStretch(1, 1);
    m_gridLayout->setColStretch(2, 1);
    m_gridLayout->setColStretch(3, 1);
    pushButton_Test_1 = createTestButton("Test 1", pageGrid); pushButton_Test_2 = createTestButton("Test 2", pageGrid); pushButton_Test_3 = createTestButton("Test 3", pageGrid); pushButton_Test_4 = createTestButton("Test 4", pageGrid);
    label_Read = new TQLabel("Read", pageGrid); label_Read->setAlignment(TQt::AlignCenter);
    label_Write = new TQLabel("Write", pageGrid); label_Write->setAlignment(TQt::AlignCenter);
    label_Mix = new TQLabel("Mix", pageGrid); label_Mix->setAlignment(TQt::AlignCenter);

    // Helper lambda: create a TQWidgetStack containing [bar, gauge] and register in m_gaugeMap
    auto makeCell = [&](BenchmarkProgressBar*& bar, TQWidget* parent,
                        Global::BenchmarkTest tt, Global::BenchmarkIOReadWrite rw) -> TQWidgetStack* {
        bar = createProgressBar(parent);
        bar->testType = tt;
        bar->readWriteType = rw;
        TQWidgetStack* cell = new TQWidgetStack(parent);
        TQtMeterGauge* g = createGaugeForBar(bar, cell);
        cell->addWidget(bar, 0);
        cell->addWidget(g,   1);
        cell->raiseWidget(0);
        m_gaugeMap[bar] = g;
        return cell;
    };

    readBar_1  = nullptr; readBar_2  = nullptr; readBar_3  = nullptr; readBar_4  = nullptr;
    writeBar_1 = nullptr; writeBar_2 = nullptr; writeBar_3 = nullptr; writeBar_4 = nullptr;
    mixBar_1   = nullptr; mixBar_2   = nullptr; mixBar_3   = nullptr; mixBar_4   = nullptr;

    TQWidgetStack* cell_r1 = makeCell(readBar_1,  pageGrid, Global::Test_1, Global::Read);
    TQWidgetStack* cell_r2 = makeCell(readBar_2,  pageGrid, Global::Test_2, Global::Read);
    TQWidgetStack* cell_r3 = makeCell(readBar_3,  pageGrid, Global::Test_3, Global::Read);
    TQWidgetStack* cell_r4 = makeCell(readBar_4,  pageGrid, Global::Test_4, Global::Read);
    TQWidgetStack* cell_w1 = makeCell(writeBar_1, pageGrid, Global::Test_1, Global::Write);
    TQWidgetStack* cell_w2 = makeCell(writeBar_2, pageGrid, Global::Test_2, Global::Write);
    TQWidgetStack* cell_w3 = makeCell(writeBar_3, pageGrid, Global::Test_3, Global::Write);
    TQWidgetStack* cell_w4 = makeCell(writeBar_4, pageGrid, Global::Test_4, Global::Write);
    cell_m1 = makeCell(mixBar_1,   pageGrid, Global::Test_1, Global::Mix);
    cell_m2 = makeCell(mixBar_2,   pageGrid, Global::Test_2, Global::Mix);
    cell_m3 = makeCell(mixBar_3,   pageGrid, Global::Test_3, Global::Mix);
    cell_m4 = makeCell(mixBar_4,   pageGrid, Global::Test_4, Global::Mix);

    m_gridLayout->addWidget(label_Read,  0, 1); m_gridLayout->addWidget(label_Write,  0, 2); m_gridLayout->addWidget(label_Mix,  0, 3);
    m_gridLayout->addWidget(pushButton_Test_1, 1, 0); m_gridLayout->addWidget(cell_r1, 1, 1); m_gridLayout->addWidget(cell_w1, 1, 2); m_gridLayout->addWidget(cell_m1, 1, 3);
    m_gridLayout->addWidget(pushButton_Test_2, 2, 0); m_gridLayout->addWidget(cell_r2, 2, 1); m_gridLayout->addWidget(cell_w2, 2, 2); m_gridLayout->addWidget(cell_m2, 2, 3);
    m_gridLayout->addWidget(pushButton_Test_3, 3, 0); m_gridLayout->addWidget(cell_r3, 3, 1); m_gridLayout->addWidget(cell_w3, 3, 2); m_gridLayout->addWidget(cell_m3, 3, 3);
    m_gridLayout->addWidget(pushButton_Test_4, 4, 0); m_gridLayout->addWidget(cell_r4, 4, 1); m_gridLayout->addWidget(cell_w4, 4, 2); m_gridLayout->addWidget(cell_m4, 4, 3);
    stackedWidget->addWidget(pageGrid, 0);

    TQWidget *pageDemo = new TQWidget(stackedWidget);
    TQVBoxLayout *demoLayout = new TQVBoxLayout(pageDemo, 0, 10);
    
    label_Demo = new TQLabel("Demo", pageDemo);
    label_Demo->setAlignment(TQt::AlignCenter);
    demoLayout->addWidget(label_Demo);

    TQHBoxLayout *demoBarsLayout = new TQHBoxLayout();
    
    // Demo Read: bar + gauge in TQWidgetStack
    TQVBoxLayout *demoReadLayout = new TQVBoxLayout();
    label_Unit_Read_Demo = new TQLabel(tr("Read"), pageDemo);
    readBar_Demo = createProgressBar(pageDemo); readBar_Demo->isDemo = true;
    readBar_Demo->readWriteType = Global::BenchmarkIOReadWrite::Read;
    readBar_Demo->setSizePolicy(TQSizePolicy(TQSizePolicy::Expanding, TQSizePolicy::Expanding));
    demoReadStack = new TQWidgetStack(pageDemo);
    TQtMeterGauge* gDemoRead = createGaugeForBar(readBar_Demo, demoReadStack);
    gDemoRead->setRadius(150);
    demoReadStack->addWidget(readBar_Demo, 0);
    demoReadStack->addWidget(gDemoRead, 1);
    demoReadStack->raiseWidget(0);
    m_gaugeMap[readBar_Demo] = gDemoRead;
    demoReadLayout->addWidget(label_Unit_Read_Demo);
    demoReadLayout->addWidget(demoReadStack, 1);
    
    // Demo Write: bar + gauge in TQWidgetStack
    TQVBoxLayout *demoWriteLayout = new TQVBoxLayout();
    label_Unit_Write_Demo = new TQLabel(tr("Write"), pageDemo);
    writeBar_Demo = createProgressBar(pageDemo); writeBar_Demo->isDemo = true;
    writeBar_Demo->readWriteType = Global::BenchmarkIOReadWrite::Write;
    writeBar_Demo->setSizePolicy(TQSizePolicy(TQSizePolicy::Expanding, TQSizePolicy::Expanding));
    demoWriteStack = new TQWidgetStack(pageDemo);
    TQtMeterGauge* gDemoWrite = createGaugeForBar(writeBar_Demo, demoWriteStack);
    gDemoWrite->setRadius(150);
    demoWriteStack->addWidget(writeBar_Demo, 0);
    demoWriteStack->addWidget(gDemoWrite, 1);
    demoWriteStack->raiseWidget(0);
    m_gaugeMap[writeBar_Demo] = gDemoWrite;
    demoWriteLayout->addWidget(label_Unit_Write_Demo);
    demoWriteLayout->addWidget(demoWriteStack, 1);
    
    demoBarsLayout->addLayout(demoReadLayout);
    demoBarsLayout->addLayout(demoWriteLayout);
    demoLayout->addLayout(demoBarsLayout);
    stackedWidget->addWidget(pageDemo, 1);

    TQHBoxLayout *bottomLayout = new TQHBoxLayout();
    extraIcon = new TQLabel("", centralwidget);
    deviceModel = new TQLineEdit(centralwidget); deviceModel->setReadOnly(true);
    bottomLayout->addWidget(extraIcon); bottomLayout->addWidget(deviceModel, 1);
    mainLayout->addLayout(bottomLayout);

    mixWidget = new TQWidget(centralwidget); mixWidget->hide();

    connect(pushButton_Test_1, SIGNAL(clicked()), this, SLOT(on_pushButton_Test_1_clicked()));
    connect(pushButton_Test_2, SIGNAL(clicked()), this, SLOT(on_pushButton_Test_2_clicked()));
    connect(pushButton_Test_3, SIGNAL(clicked()), this, SLOT(on_pushButton_Test_3_clicked()));
    connect(pushButton_Test_4, SIGNAL(clicked()), this, SLOT(on_pushButton_Test_4_clicked()));
    connect(pushButton_All, SIGNAL(clicked()), this, SLOT(on_pushButton_All_clicked()));
    connect(comboBox_Storages, SIGNAL(activated(int)), this, SLOT(on_comboBox_Storages_activated(int)));
    connect(loopsCount, SIGNAL(valueChanged(int)), this, SLOT(on_loopsCount_valueChanged(int)));
    connect(comboBox_ComparisonUnit, SIGNAL(activated(int)), this, SLOT(on_comboBox_ComparisonUnit_activated(int)));
    connect(actionCopy, SIGNAL(activated()), this, SLOT(on_actionCopy_triggered()));
    connect(actionSave, SIGNAL(activated()), this, SLOT(on_actionSave_triggered()));
    connect(comboBox_MixRatio, SIGNAL(activated(int)), this, SLOT(on_comboBox_MixRatio_activated(int)));
    connect(refreshStoragesButton, SIGNAL(clicked()), this, SLOT(on_refreshStoragesButton_clicked()));
    connect(comboBox_fileSize, SIGNAL(activated(int)), this, SLOT(on_comboBox_fileSize_activated(int)));
    connect(actionFlush_Pagecache, SIGNAL(toggled(bool)), this, SLOT(on_actionFlush_Pagecache_triggered(bool)));
    connect(actionUse_O_DIRECT, SIGNAL(toggled(bool)), this, SLOT(on_actionUse_O_DIRECT_triggered(bool)));
    connect(actionTestData_Continuous, SIGNAL(toggled(bool)), this, SLOT(on_actionTestData_Continuous_triggered(bool)));
    connect(actionCoW_detection, SIGNAL(toggled(bool)), this, SLOT(on_actionCoW_detection_triggered(bool)));
}

MainWindow::MainWindow(TQWidget *parent)
    : TDEMainWindow(parent)
    , m_benchmark(new Benchmark)
{
    m_animationTimer = new TQTimer(this);
    connect(m_animationTimer, TQT_SIGNAL(timeout()), this, TQT_SLOT(animateProgressBars()));
    m_animationTimer->start(33); // ~30 fps easing timer


    connect(m_benchmark, TQT_SIGNAL(testRowAdvanced()), this, TQT_SLOT(snapProgressBars()));

    setupUi();

    extraIcon->hide();

    TQActionGroup *profilesGroup = new TQActionGroup(this);
    profilesGroup->add(actionDefault);
    profilesGroup->add(actionPeak_Performance);
    profilesGroup->add(actionReal_World_Performance);
    profilesGroup->add(actionDemo);
    profilesGroup->add(actionDefault_Mix);
    profilesGroup->add(actionPeak_Performance_Mix);
    profilesGroup->add(actionReal_World_Performance_Mix);
    connect(profilesGroup, SIGNAL(selected(TQAction*)), this, SLOT(profileSelected(TQAction*)));

    TQActionGroup *modesGroup = new TQActionGroup(this);
    modesGroup->add(actionRead_Write_Mix);
    modesGroup->add(actionRead_Mix);
    modesGroup->add(actionWrite_Mix);
    connect(modesGroup, SIGNAL(selected(TQAction*)), this, SLOT(modeSelected(TQAction*)));

    TQActionGroup *testDataGroup = new TQActionGroup(this);
    testDataGroup->add(actionTestData_Random);
    testDataGroup->add(actionTestData_Zeros);
    connect(testDataGroup, SIGNAL(selected(TQAction*)), this, SLOT(testDataSelected(TQAction*)));

    TQActionGroup *presetsGroup = new TQActionGroup(this);
    presetsGroup->add(actionPreset_Standard);
    presetsGroup->add(actionPreset_NVMe_SSD);
    connect(presetsGroup, SIGNAL(selected(TQAction*)), this, SLOT(presetSelected(TQAction*)));

    TQActionGroup *themeGroup = new TQActionGroup(this);
    themeGroup->add(actionTheme_Orange);
    themeGroup->add(actionTheme_Blue);
    themeGroup->add(actionTheme_None);
    connect(themeGroup, SIGNAL(selected(TQAction*)), this, SLOT(themeSelected(TQAction*)));
    // Use Gauges is a standalone toggle (not exclusive with light/dark)
    connect(actionTheme_Use_Gauges, SIGNAL(toggled(bool)), this, SLOT(on_actionUseGauges_toggled(bool)));
    connect(actionNotifyOnCompletion, SIGNAL(toggled(bool)), this, SLOT(on_actionNotifyOnCompletion_toggled(bool)));

    m_isRunningAll = false;
    m_benchmarkCancelled = false;
    m_hasThemeColors = false;
    m_pulsePhase = 0;
    m_pulseTimer = new TQTimer(this);
    connect(m_pulseTimer, SIGNAL(timeout()), this, SLOT(pulseStopButtons()));

    // testType/readWriteType for grid bars already set in makeCell above
    readBar_Demo->testType = Global::BenchmarkTest::Test_1; readBar_Demo->readWriteType = Global::BenchmarkIOReadWrite::Read;
    writeBar_Demo->testType = Global::BenchmarkTest::Test_1; writeBar_Demo->readWriteType = Global::BenchmarkIOReadWrite::Write;

    m_progressBars.push_back(readBar_1); m_progressBars.push_back(writeBar_1); m_progressBars.push_back(mixBar_1);
    m_progressBars.push_back(readBar_2); m_progressBars.push_back(writeBar_2); m_progressBars.push_back(mixBar_2);
    m_progressBars.push_back(readBar_3); m_progressBars.push_back(writeBar_3); m_progressBars.push_back(mixBar_3);
    m_progressBars.push_back(readBar_4); m_progressBars.push_back(writeBar_4); m_progressBars.push_back(mixBar_4);
    m_progressBars.push_back(readBar_Demo); m_progressBars.push_back(writeBar_Demo);

    refreshProgressBars();

    // Load settings
    const AppSettings settings;

    for (TQAction *action : { actionDefault, actionPeak_Performance, actionReal_World_Performance, actionDemo,
                             actionDefault_Mix, actionPeak_Performance_Mix, actionReal_World_Performance_Mix }) {
        Global::PerformanceProfile p = Global::PerformanceProfile::Default;
        if (action == actionPeak_Performance || action == actionPeak_Performance_Mix) p = Global::PerformanceProfile::Peak;
        else if (action == actionReal_World_Performance || action == actionReal_World_Performance_Mix) p = Global::PerformanceProfile::RealWorld;
        else if (action == actionDemo) p = Global::PerformanceProfile::Demo;

        bool m = (action == actionDefault_Mix || action == actionPeak_Performance_Mix || action == actionReal_World_Performance_Mix);

        if (p == settings.getPerformanceProfile() && m == settings.getMixedState()) {
            action->setOn(true);
            profileSelected(action);
            break;
        }
    }

    int indexMixRatio = settings.getRandomReadPercentage() / 10 - 1;

    for (int i = 1; i <= 9; i++) {
        comboBox_MixRatio->insertItem(TQString("R%1%/W%2%").arg(i * 10).arg((10 - i) * 10));
    }

    comboBox_ComparisonUnit->setCurrentItem(settings.getComparisonUnit());
    comboBox_MixRatio->setCurrentItem(indexMixRatio);

    actionTestData_Zeros->setOn(settings.getBenchmarkTestData() == Global::BenchmarkTestData::Zeros);
    // actionTestData_Continuous->setOn(settings.getContinuousGenerationState());
    actionRead_Write_Mix->setOn(settings.getBenchmarkMode() == Global::BenchmarkMode::ReadWriteMix);
    actionRead_Mix->setOn(settings.getBenchmarkMode() == Global::BenchmarkMode::ReadMix);
    actionWrite_Mix->setOn(settings.getBenchmarkMode() == Global::BenchmarkMode::WriteMix);

    actionUse_O_DIRECT->setOn(settings.getCacheBypassState());
    actionFlush_Pagecache->setOn(settings.getFlushingCacheState());
    // actionCoW_detection->setOn(settings.getCoWDetectionState());
    loopsCount->setValue(settings.getLoopsCount());

    Global::Theme savedTheme = settings.getTheme();
    actionTheme_Orange->setOn(savedTheme == Global::Theme::ThemeOrange);
    actionTheme_Blue->setOn(savedTheme == Global::Theme::ThemeBlue);
    actionTheme_None->setOn(savedTheme == Global::Theme::ThemeNone);
    actionTheme_Use_Gauges->setOn(settings.getUseGauges());
    actionNotifyOnCompletion->setOn(settings.getNotifyOnCompletion());

    applyThemeBackground();
    updateProgressBarsStyle();

    updatePresetsSelection();
    updateBenchmarkButtonsContent();

    updateFileSizeList();

    updateStoragesList();

    // Set callbacks
    connect(m_benchmark, TQT_SIGNAL(runningStateChanged(bool)), this, TQT_SLOT(benchmarkStateChanged(bool)));
    connect(m_benchmark, TQT_SIGNAL(benchmarkStatusUpdate(const TQString&)), this, TQT_SLOT(benchmarkStatusUpdate(const TQString&)));
    connect(m_benchmark, TQT_SIGNAL(failed(const TQString&)), this, TQT_SLOT(benchmarkFailed(const TQString&)));
    connect(m_benchmark, TQT_SIGNAL(resultReady(BenchmarkProgressBar*, const Benchmark::PerformanceResult&)), this, TQT_SLOT(handleResults(BenchmarkProgressBar*, const Benchmark::PerformanceResult&)));
    connect(m_benchmark, TQT_SIGNAL(currentTestRunning(Global::BenchmarkTest)), this, TQT_SLOT(onCurrentTestRunning(Global::BenchmarkTest)));

    handleDirectoryChanged("/");

    if (comboBox_Storages->count() > 1) {
        comboBox_Storages->setCurrentItem(1);
        on_comboBox_Storages_activated(1);
    }

    if (!m_benchmark->isFIODetected()) {
        TQMessageBox::critical(this, "tdeDiskMark", TQObject::tr("No FIO was found. Please install FIO before using tdeDiskMark."));
        tqApp->quit();
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(TQCloseEvent *event)
{
    if (m_benchmark->isRunning()) {
        connect(m_benchmark, TQT_SIGNAL(finished()), tqApp, TQT_SLOT(quit()));
        m_benchmark->setRunning(false);
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::changeEvent(TQEvent *event)
{
}

void MainWindow::on_refreshStoragesButton_clicked()
{
    updateStoragesList();
}

void MainWindow::updateStoragesList()
{
    TQString temp = comboBox_Storages->currentText();

    TQValueVector<TQString> permanentStoragesInList; // For simplicity, we track paths

    comboBox_Storages->clear();

    FILE *fp = setmntent("/proc/mounts", "r");
    if (fp) {
        struct mntent *mnt;
        while ((mnt = getmntent(fp)) != NULL) {
            TQString device = mnt->mnt_fsname;
            if (device.startsWith("/dev")) {
                struct statvfs st;
                if (statvfs(mnt->mnt_dir, &st) == 0) {
                    uint64_t total = (uint64_t)st.f_blocks * st.f_frsize;
                    uint64_t free = (uint64_t)st.f_bfree * st.f_frsize;
                    Global::Storage storage;
                    storage.path = mnt->mnt_dir;
                    storage.bytesTotal = total;
                    storage.bytesOccupied = total - free;
                    storage.formatedSize = formatSize(storage.bytesOccupied, storage.bytesTotal);
                    addItemToStoragesList(storage);
                }
            }
        }
        endmntent(fp);
    }

    comboBox_Storages->insertItem(tr("Add a directory"), 0);

    if (!temp.isEmpty()) {
        int foundIndex = -1;
        for (int i = 0; i < comboBox_Storages->count(); ++i) {
            if (comboBox_Storages->text(i).contains(temp)) {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex != -1) comboBox_Storages->setCurrentItem(foundIndex);
    }
    
    // Resize items popup
    resizeComboBoxItemsPopup(comboBox_Storages);
}

void MainWindow::addItemToStoragesList(const Global::Storage &storage)
{
    TQString itemText = TQString("%1 %2% (%3)")
                            .arg(storage.path)
                            .arg(storage.bytesTotal > 0 ? (int)((storage.bytesOccupied * 100) / storage.bytesTotal) : 0)
                            .arg(storage.formatedSize);

    for (int i=0; i < comboBox_Storages->count(); ++i) {
        if (comboBox_Storages->text(i) == itemText)
            return;
    }

    comboBox_Storages->insertItem(itemText);
    m_storagePaths.push_back(storage.path);
}

void MainWindow::updateFileSizeList()
{
    const AppSettings settings;

    int fileSize = settings.getFileSize();

    comboBox_fileSize->clear();

    for (int i = 16; i <= 512; i *= 2) {
        comboBox_fileSize->insertItem(TQString("%1 %2").arg(i).arg(tr("MiB")));
    }

    for (int i = 1; i <= 64; i *= 2) {
        comboBox_fileSize->insertItem(TQString("%1 %2").arg(i).arg(tr("GiB")));
    }

    /* ignore find data */
    comboBox_fileSize->setCurrentItem(6);

    resizeComboBoxItemsPopup(comboBox_fileSize);
}

void MainWindow::on_comboBox_fileSize_activated(int index)
{
    if (index != -1) {
        AppSettings settings;
        settings.setFileSize(1024);
    }
}

void MainWindow::resizeComboBoxItemsPopup(TQComboBox *)
{
}


void MainWindow::on_actionUse_O_DIRECT_triggered(bool checked)
{
    AppSettings().setCacheBypassState(checked);
}

void MainWindow::on_actionTestData_Continuous_triggered(bool checked) {
    // AppSettings().setContinuousGenerationState(checked);
}

void MainWindow::on_actionFlush_Pagecache_triggered(bool checked)
{
    AppSettings().setFlushingCacheState(checked);
}

void MainWindow::on_actionCoW_detection_triggered(bool /*checked*/)
{
    // CoW detection feature not yet implemented.
}

void MainWindow::updateBenchmarkButtonsContent()
{
    const AppSettings settings;

    Global::BenchmarkParams params;

    params = settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile());
    pushButton_Test_1->setText(Global::getBenchmarkButtonText(params));

    switch (settings.getPerformanceProfile())
    {
    case Global::PerformanceProfile::Default:
        TQToolTip::add(pushButton_Test_1, Global::getBenchmarkButtonToolTip(params));

        params = settings.getBenchmarkParams(Global::BenchmarkTest::Test_2);
        pushButton_Test_2->setText(Global::getBenchmarkButtonText(params));
        TQToolTip::add(pushButton_Test_2, Global::getBenchmarkButtonToolTip(params));

        params = settings.getBenchmarkParams(Global::BenchmarkTest::Test_3);
        pushButton_Test_3->setText(Global::getBenchmarkButtonText(params));
        TQToolTip::add(pushButton_Test_3, Global::getBenchmarkButtonToolTip(params));

        params = settings.getBenchmarkParams(Global::BenchmarkTest::Test_4);
        pushButton_Test_4->setText(Global::getBenchmarkButtonText(params));
        TQToolTip::add(pushButton_Test_4, Global::getBenchmarkButtonToolTip(params));
        break;
    case Global::PerformanceProfile::Peak:
    case Global::PerformanceProfile::RealWorld:
        TQToolTip::add(pushButton_Test_1, Global::getBenchmarkButtonToolTip(params, true).arg(tr("MB/s")));

        params = settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, settings.getPerformanceProfile());
        pushButton_Test_2->setText(Global::getBenchmarkButtonText(params));
        TQToolTip::add(pushButton_Test_2, Global::getBenchmarkButtonToolTip(params, true).arg(tr("MB/s")));

        pushButton_Test_3->setText(Global::getBenchmarkButtonText(params, tr("IOPS")));
        TQToolTip::add(pushButton_Test_3, Global::getBenchmarkButtonToolTip(params, true).arg(tr("IOPS")));

        pushButton_Test_4->setText(Global::getBenchmarkButtonText(params, tr("μs")));
        TQToolTip::add(pushButton_Test_4, Global::getBenchmarkButtonToolTip(params, true).arg(tr("μs")));
        break;
    case Global::PerformanceProfile::Demo:
        label_Demo->setText(TQString("%1 %2 %3, Q=%4, T=%5")
                                .arg(params.Pattern == Global::BenchmarkIOPattern::SEQ ? TQString("SEQ") : TQString("RND"))
                                .arg(params.BlockSize >= 1024 ? params.BlockSize / 1024 : params.BlockSize)
                                .arg(params.BlockSize >= 1024 ? tr("MiB") : tr("KiB"))
                                .arg(params.Queues).arg(params.Threads));
        break;
    }
}

void MainWindow::updatePresetsSelection()
{
    const AppSettings settings;

    auto testFunc = [&] (Global::BenchmarkTest test, Global::PerformanceProfile profile, Global::BenchmarkPreset preset) {
        return settings.getBenchmarkParams(test, profile) == settings.defaultBenchmarkParams(test, profile, preset);
    };

    Global::BenchmarkPreset preset = Global::BenchmarkPreset::Standard;
    bool testStandardPreset =
            testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default, preset) &&
            testFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default, preset) &&
            testFunc(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default, preset) &&
            testFunc(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default, preset) &&
            testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak, preset) &&
            testFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak, preset) &&
            testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo, preset);

    actionPreset_Standard->setOn(testStandardPreset);

    if (!testStandardPreset) {
        Global::BenchmarkPreset preset = Global::BenchmarkPreset::NVMe_SSD;
        bool testNVMeSSDPreset =
                testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default, preset) &&
                testFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default, preset) &&
                testFunc(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default, preset) &&
                testFunc(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default, preset) &&
                testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak, preset) &&
                testFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak, preset) &&
                testFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo, preset);

        actionPreset_NVMe_SSD->setOn(testNVMeSSDPreset);
    }
}

void MainWindow::refreshProgressBars()
{
    

    
    for (auto const& progressBar: m_progressBars) {
        progressBar->MBPerSec = 0;
        progressBar->GBPerSec = 0;
        progressBar->IOPS = 0;
        progressBar->Latency = 0;
        progressBar->targetMBPerSec = 0;
        progressBar->targetGBPerSec = 0;
        progressBar->targetIOPS = 0;
        progressBar->targetLatency = 0;
        progressBar->setProgress(0);
        
        if (!progressBar->isDemo)
        TQToolTip::add(progressBar, 
                    Global::getToolTipTemplate().arg(
                        TQString::number(0., 'f', 3),
                        TQString::number(0., 'f', 3),
                        TQString::number(0., 'f', 3),
                        TQString::number(0., 'f', 3)
                        )
                    );
    }
}

TQString MainWindow::formatSize(uint64_t available, uint64_t total)
{
    TQStringList units;
    units << tr("Bytes") << tr("KiB") << tr("MiB") << tr("GiB") << tr("TiB") << tr("PiB");
    int i;
    double outputAvailable = available;
    double outputTotal = total;
    for (i = 0; i < units.size() - 1; i++) {
        if (outputTotal < 1024) break;
        outputAvailable = outputAvailable / 1024;
        outputTotal = outputTotal / 1024;
    }
        return TQString("%1/%2 %3").arg(TQString::number(outputAvailable, 'f', 2))
            .arg(TQString::number(outputTotal, 'f', 2)).arg(units[i]);
}

void MainWindow::on_comboBox_ComparisonUnit_activated(int index)
{
    AppSettings().setComparisonUnit(Global::ComparisonUnit(index));
    updateLabels();

    updateProgressBarsStyle();
    for (auto const& progressBar: m_progressBars) {
        updateProgressBar(progressBar);
    }
}

void MainWindow::updateLabels()
{
    label_Read->setText(Global::getComparisonLabelTemplate()
                            .arg(tr("Read"), comboBox_ComparisonUnit->currentText()));

    label_Write->setText(Global::getComparisonLabelTemplate()
                             .arg(tr("Write"), comboBox_ComparisonUnit->currentText()));

    label_Mix->setText(Global::getComparisonLabelTemplate()
                             .arg(tr("Mix"), comboBox_ComparisonUnit->currentText()));

    label_Unit_Read_Demo->setText(comboBox_ComparisonUnit->currentText());
    label_Unit_Write_Demo->setText(comboBox_ComparisonUnit->currentText());
}

TQString MainWindow::combineOutputTestResult(const BenchmarkProgressBar *progressBar, const Global::BenchmarkParams &params)
{
    

    return TQString("%1 %2 %3 (Q=%4, T=%5): %6 MB/s [ %7 IOPS] < %8 us>")
           .arg(params.Pattern == Global::BenchmarkIOPattern::SEQ ? "Sequential" : "Random")
           .arg(TQString::number(params.BlockSize >= 1024 ? params.BlockSize / 1024 : params.BlockSize).rightJustify(3, ' '))
           .arg(params.BlockSize >= 1024 ? "MiB" : "KiB")
           .arg(TQString::number(params.Queues).rightJustify(3, ' '))
           .arg(TQString::number(params.Threads).rightJustify(2, ' '))
           .arg(TQString::number(
                    progressBar->MBPerSec, 'f', 3)
                .rightJustify(9, ' '))
           .arg(TQString::number(
                    progressBar->IOPS, 'f', 1)
                .rightJustify(8, ' '))
           .arg(TQString::number(
                    progressBar->Latency, 'f', 2)
                .rightJustify(8, ' '))
           .rightJustify(Global::getOutputColumnsCount(), ' ');
}

TQString MainWindow::getTextBenchmarkResult()
{
    const AppSettings settings;

    TQStringList output;

    output << TQString("tdeDiskMark (%1): https://github.com/seb3773/tdediskmark")
              .arg(TQString("1.0"))
              .rightJustify(Global::getOutputColumnsCount(), ' ')
           << TQString("Flexible I/O Tester (%1): https://github.com/axboe/fio")
              .arg(m_benchmark->getFIOVersion())
              .rightJustify(Global::getOutputColumnsCount(), ' ')
           << TQString().leftJustify(Global::getOutputColumnsCount())
           << "* MB/s = 1,000,000 bytes/s [SATA/600 = 600,000,000 bytes/s]"
           << "* KB = 1000 bytes, KiB = 1024 bytes";

    output << TQString()
           << "[Read]";
    if (settings.getPerformanceProfile() == Global::PerformanceProfile::Demo) {
        output << combineOutputTestResult(readBar_Demo, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile()));
    }
    else {
        output << combineOutputTestResult(readBar_1, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile()));
        output << combineOutputTestResult(readBar_2, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, settings.getPerformanceProfile()));
        if (settings.getPerformanceProfile() == Global::PerformanceProfile::Default) {
            output << combineOutputTestResult(readBar_3, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, settings.getPerformanceProfile()));
            output << combineOutputTestResult(readBar_4, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, settings.getPerformanceProfile()));
        }
    }

    output << TQString()
           << "[Write]";
    if (settings.getPerformanceProfile() == Global::PerformanceProfile::Demo) {
        output << combineOutputTestResult(writeBar_Demo, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile()));
    }
    else {
        output << combineOutputTestResult(writeBar_1, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile()));
        output << combineOutputTestResult(writeBar_2, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, settings.getPerformanceProfile()));
        if (settings.getPerformanceProfile() == Global::PerformanceProfile::Default) {
            output << combineOutputTestResult(writeBar_3, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, settings.getPerformanceProfile()));
            output << combineOutputTestResult(writeBar_4, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, settings.getPerformanceProfile()));
        }
    }

    if (settings.getMixedState() && settings.getPerformanceProfile() != Global::PerformanceProfile::Demo) {
         output << TQString()
                << TQString("[Mix] Read %1%/Write %2%")
                   .arg(settings.getRandomReadPercentage())
                   .arg(100 - settings.getRandomReadPercentage())
                << combineOutputTestResult(mixBar_1, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, settings.getPerformanceProfile()))
                << combineOutputTestResult(mixBar_2, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, settings.getPerformanceProfile()));
             if (settings.getPerformanceProfile() == Global::PerformanceProfile::Default) {
                 output << combineOutputTestResult(mixBar_3, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, settings.getPerformanceProfile()));
                 output << combineOutputTestResult(mixBar_4, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, settings.getPerformanceProfile()));
             }
    }

    TQString profiles[] = { "Default", "Peak Performance", "Real World Performance", "Demo" };

    output << TQString()
           << TQString("Profile: %1%2")
              .arg(profiles[(int)settings.getPerformanceProfile()]).arg(settings.getMixedState() ? " [+Mix]" : TQString())
           << TQString("   Test: %1")
              .arg("%1 %2 (x%3) [Measure: %4 %5 / Interval: %6 %7]")
              .arg(settings.getFileSize() >= 1024 ? settings.getFileSize() / 1024 : settings.getFileSize())
              .arg(settings.getFileSize() >= 1024 ? "GiB" : "MiB")
              .arg(settings.getLoopsCount())
              .arg(settings.getMeasuringTime() >= 60 ? settings.getMeasuringTime() / 60 : settings.getMeasuringTime())
              .arg(settings.getMeasuringTime() >= 60 ? "min" : "sec")
              .arg(settings.getIntervalTime() >= 60 ? settings.getIntervalTime() / 60 : settings.getIntervalTime())
              .arg(settings.getIntervalTime() >= 60 ? "min" : "sec")
           << TQString("   Date: %1 %2")
              .arg(TQDate::currentDate().toString("yyyy-MM-dd"))
              .arg(TQTime::currentTime().toString("hh:mm:ss"))
           << TQString("     OS: %1 %2 [%3 %4]").arg("Linux").arg("")
              .arg("Kernel").arg("");

    TQString targetInfo = comboBox_Storages->currentText();
    TQString modelName = deviceModel->text();
    if (!modelName.isEmpty()) {
        targetInfo += TQString(" [%1]").arg(modelName);
    }

    output << TQString(" Target: %1")
              .arg(targetInfo);

    return output.join("\n");
}

void MainWindow::on_actionCopy_triggered()
{
    TQClipboard *clipboard = TQApplication::clipboard();
    clipboard->setText(getTextBenchmarkResult());
}

void MainWindow::on_actionSave_triggered()
{
    TQString fileName = TQFileDialog::getSaveFileName(TQString("KDM_%1%2.txt").arg(TQDate::currentDate().toString("yyyyMMdd")).arg(TQTime::currentTime().toString("hhmmss")), TQString(), this);
    if (!fileName.isEmpty()) {
        TQFile file(fileName);
        if (file.open(IO_WriteOnly)) {
            TQTextStream out(&file);
            out << getTextBenchmarkResult();
            file.close();
        }
    }
}

void MainWindow::on_loopsCount_valueChanged(int arg1)
{
    AppSettings settings;
    settings.setLoopsCount(arg1);
}

void MainWindow::on_comboBox_MixRatio_activated(int index)
{
    AppSettings settings;
    settings.setRandomReadPercentage((index + 1) * 10.f);
}

void MainWindow::on_comboBox_Storages_activated(int index)
{
    // It is expected that at index 0 without item data there will be a field to add a directory
    if (index == 0 && comboBox_Storages->text(index) == tr("Add a directory")) {
        TQString dir = TQFileDialog::getExistingDirectory(TQDir::homeDirPath(), this, 0, "Select Directory", true, false);
        if (!dir.isNull()) {
            int foundIndex = -1;
            for (int i=0; i < comboBox_Storages->count(); ++i) {
                if (comboBox_Storages->text(i).contains(dir)) foundIndex = i;
            }
            if (foundIndex == -1) {
                

                struct statvfs st;
                uint64_t bT = 0, bO = 0;
                if (statvfs(dir.local8Bit().data(), &st) == 0) {
                    bT = (uint64_t)st.f_blocks * st.f_frsize;
                    bO = bT - ((uint64_t)st.f_bfree * st.f_frsize);
                }

                Global::Storage storage {
                    .path = dir,
                    .bytesTotal = static_cast<long long>(bT),
                    .bytesOccupied = static_cast<long long>(bO),
                    .formatedSize = formatSize(bO, bT),
                    .permanentInList = true
                };

                addItemToStoragesList(storage);
                resizeComboBoxItemsPopup(comboBox_Storages);

                comboBox_Storages->setCurrentItem(comboBox_Storages->count() - 1);
            }
            else {
                comboBox_Storages->setCurrentItem(foundIndex);
            }

            return;
        }

        comboBox_Storages->setCurrentItem(1);
    }
    else {
        m_benchmark->setDir(comboBox_Storages->text(index).section(" ", 0, 0));
        
        TQString dirPath = comboBox_Storages->text(index).section(" ", 0, 0);
        TQString blockDevice = "";
        FILE *fp = setmntent("/proc/mounts", "r");
        if (fp) {
            struct mntent *mnt;
            int max_match_len = -1;
            while ((mnt = getmntent(fp)) != NULL) {
                TQString mountDir = mnt->mnt_dir;
                if (dirPath.startsWith(mountDir)) {
                    if ((int)mountDir.length() > max_match_len) {
                        max_match_len = mountDir.length();
                        blockDevice = mnt->mnt_fsname;
                    }
                }
            }
            endmntent(fp);
        }
        
        if (!blockDevice.isEmpty() && blockDevice.startsWith("/dev/")) {
            deviceModel->setText(DiskDriveInfo::Instance().getModelName(blockDevice));
        } else {
            deviceModel->setText("");
        }
    }
}

void MainWindow::localeSelected(TQAction* act)
{
    AppSettings settings;

    

    // settings.setLocale(TQString("en"));
}

void MainWindow::profileSelected(TQAction* act)
{
    AppSettings settings;

    bool isMixed = (act == actionDefault_Mix || act == actionPeak_Performance_Mix || act == actionReal_World_Performance_Mix);
    Global::PerformanceProfile prof = Global::PerformanceProfile::Default;
    if (act == actionPeak_Performance || act == actionPeak_Performance_Mix) prof = Global::PerformanceProfile::Peak;
    else if (act == actionReal_World_Performance || act == actionReal_World_Performance_Mix) prof = Global::PerformanceProfile::RealWorld;
    else if (act == actionDemo) prof = Global::PerformanceProfile::Demo;

    settings.setMixedState(isMixed);

    if (isMixed) {
        label_Mix->show();
        cell_m1->show();
        cell_m2->show();
        cell_m3->show();
        cell_m4->show();
        comboBox_MixRatio->show();
        m_gridLayout->setColStretch(3, 1);
    } else {
        label_Mix->hide();
        cell_m1->hide();
        cell_m2->hide();
        cell_m3->hide();
        cell_m4->hide();
        comboBox_MixRatio->hide();
        m_gridLayout->setColStretch(3, 0);
    }

    switch (prof)
    {
    case Global::PerformanceProfile::Default:
        m_windowTitle = "tdeDiskMark";
        comboBox_ComparisonUnit->show();
        break;
    case Global::PerformanceProfile::Peak:
        m_windowTitle = "tdeDiskMark <PEAK>";
        comboBox_ComparisonUnit->setCurrentItem(0);
        comboBox_ComparisonUnit->hide();
        break;
    case Global::PerformanceProfile::RealWorld:
        m_windowTitle = "tdeDiskMark <REAL>";
        comboBox_ComparisonUnit->setCurrentItem(0);
        comboBox_ComparisonUnit->hide();
        break;
    case Global::PerformanceProfile::Demo:
        m_windowTitle = "tdeDiskMark <DEMO>";
        comboBox_ComparisonUnit->show();
        break;
    }

    setCaption(m_windowTitle);

    settings.setPerformanceProfile(prof);
    stackedWidget->raiseWidget(prof == Global::PerformanceProfile::Demo ? 1 : 0);

    



    adjustSize();

    refreshProgressBars();
    updateBenchmarkButtonsContent();

    updateProgressBarsStyle();
}

void MainWindow::modeSelected(TQAction* act)
{
    if (act == actionRead_Mix) AppSettings().setBenchmarkMode(Global::BenchmarkMode::ReadMix);
    else if (act == actionWrite_Mix) AppSettings().setBenchmarkMode(Global::BenchmarkMode::WriteMix);
    else AppSettings().setBenchmarkMode(Global::BenchmarkMode::ReadWriteMix);
}

void MainWindow::testDataSelected(TQAction* act)
{
    if (act == actionTestData_Zeros) AppSettings().setBenchmarkTestData(Global::BenchmarkTestData::Zeros);
    else AppSettings().setBenchmarkTestData(Global::BenchmarkTestData::Random);
}

void MainWindow::presetSelected(TQAction* act)
{
    AppSettings settings;

    Global::BenchmarkPreset preset = (act == actionPreset_NVMe_SSD) ? Global::BenchmarkPreset::NVMe_SSD : Global::BenchmarkPreset::Standard;

    auto updateFunc = [&] (Global::BenchmarkTest test, Global::PerformanceProfile profile) {
        settings.setBenchmarkParams(test, profile, settings.defaultBenchmarkParams(test, profile, preset));
    };

    updateFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default);
    updateFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default);
    updateFunc(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default);
    updateFunc(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default);

    updateFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak);
    updateFunc(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak);

    updateFunc(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo);

    updateBenchmarkButtonsContent();
}


void MainWindow::updateProgressBarsStyle()
{
    const AppSettings settings;
    bool useGauges = settings.getUseGauges();

    Global::ComparisonUnit defaultUnit = settings.getComparisonUnit();

    // Each bar lives in a TQWidgetStack (page 0=bar, page 1=gauge)
    for (auto bar : m_progressBars) {
        TQWidgetStack* stack = dynamic_cast<TQWidgetStack*>(bar->parentWidget());
        if (stack) {
            stack->raiseWidget(useGauges ? 1 : 0);
        }
        // Apply gauge style
        TQtMeterGauge* g = m_gaugeMap[bar];
        if (g) {
            Global::ComparisonUnit unit = defaultUnit;
            switch (settings.getPerformanceProfile()) {
            case Global::PerformanceProfile::Peak:
            case Global::PerformanceProfile::RealWorld:
                if (bar == readBar_3 || bar == writeBar_3 || bar == mixBar_3)
                    unit = Global::ComparisonUnit::IOPS;
                else if (bar == readBar_4 || bar == writeBar_4 || bar == mixBar_4)
                    unit = Global::ComparisonUnit::Latency;
                break;
            default:
                break;
            }
            applyGaugeStyleForUnit(g, unit);
            updateGauge(bar);
            g->repaint(true);
        }
    }
}


void MainWindow::themeSelected(TQAction* act)
{
    Global::Theme theme = Global::Theme::ThemeOrange;
    if (act == actionTheme_Orange) theme = Global::Theme::ThemeOrange;
    else if (act == actionTheme_Blue) theme = Global::Theme::ThemeBlue;
    else if (act == actionTheme_None) theme = Global::Theme::ThemeNone;

    AppSettings settings;
    settings.setTheme(theme);

    applyThemeBackground();
    updateProgressBarsStyle();
}

void MainWindow::on_actionUseGauges_toggled(bool checked)
{
    AppSettings settings;
    settings.setUseGauges(checked);
    updateProgressBarsStyle();
}

void MainWindow::on_actionNotifyOnCompletion_toggled(bool checked)
{
    AppSettings settings;
    settings.setNotifyOnCompletion(checked);
}

void MainWindow::applyThemeBackground()
{
    const AppSettings settings;
    Global::Theme theme = settings.getTheme();

    if (theme == Global::Theme::ThemeNone) {
        m_hasThemeColors = false;
        centralwidget->unsetPalette();
        centralwidget->setBackgroundMode(TQt::PaletteBackground);
        centralwidget->update();

        // Icons for None theme
        TQPixmap goIcon;
        goIcon.loadFromData(___icons_go_png, ___icons_go_png_len);
        pushButton_All->setIconSet(TQIconSet(goIcon));
        TQPixmap refIcon;
        refIcon.loadFromData(___icons_refresh_png, ___icons_refresh_png_len);
        refreshStoragesButton->setIconSet(TQIconSet(refIcon));

        applyThemeColors();
        return;
    }

    m_hasThemeColors = true;
    TQColor top, bottom;
    if (theme == Global::Theme::ThemeOrange) {
        top    = TQColor(0xda, 0xc5, 0x57);
        bottom = TQColor(0xda, 0x54, 0x50);
        m_textColor1 = TQColor(0x00, 0x00, 0x00);
        m_textColor2 = TQColor(0xEE, 0xEE, 0xEE);
        m_bgColor1   = TQColor(0xec, 0xff, 0x18);
        m_bgColor2   = TQColor(0xda, 0x5e, 0x50);
        m_bgColor3   = TQColor(0xda, 0xb2, 0x55);

        TQPixmap rIcon;
        rIcon.loadFromData(___icons_race_png, ___icons_race_png_len);
        pushButton_All->setIconSet(TQIconSet(rIcon));
        TQPixmap r2Icon;
        r2Icon.loadFromData(___icons_refresh2_png, ___icons_refresh2_png_len);
        refreshStoragesButton->setIconSet(TQIconSet(r2Icon));
    } else { // ThemeBlue
        top    = TQColor(0x4c, 0x5d, 0x6b);
        bottom = TQColor(0x95, 0xab, 0xb7);
        m_textColor1 = TQColor(0xEE, 0xEE, 0xEE);
        m_textColor2 = TQColor(0x00, 0x00, 0x00);
        m_bgColor1   = TQColor(0x00, 0x3e, 0x50);
        m_bgColor2   = TQColor(0x90, 0xa6, 0xb2);
        m_bgColor3   = TQColor(0x4e, 0x5f, 0x6d);

        TQPixmap fIcon;
        fIcon.loadFromData(___icons_fly_png, ___icons_fly_png_len);
        pushButton_All->setIconSet(TQIconSet(fIcon));
        TQPixmap refIcon;
        refIcon.loadFromData(___icons_refresh_png, ___icons_refresh_png_len);
        refreshStoragesButton->setIconSet(TQIconSet(refIcon));
    }

    int h = centralwidget->height();
    if (h < 1) h = 720;
    TQPixmap bgPix(1, h);
    TQPainter p(&bgPix);
    for (int y = 0; y < h; y++) {
        double t = (double)y / (double)(h - 1);
        int r = (int)(top.red()   + t * (bottom.red()   - top.red()));
        int g = (int)(top.green() + t * (bottom.green() - top.green()));
        int b = (int)(top.blue()  + t * (bottom.blue()  - top.blue()));
        p.setPen(TQColor(r, g, b));
        p.drawPoint(0, y);
    }
    p.end();

    centralwidget->setPaletteBackgroundPixmap(bgPix);
    centralwidget->update();
    applyThemeColors();
}

void MainWindow::applyThemeColors()
{
    // Test buttons (color1/bg1) — not during Stop pulse
    TQPushButton* testBtns[] = { pushButton_Test_1, pushButton_Test_2, pushButton_Test_3, pushButton_Test_4 };

    if (m_hasThemeColors) {
        for (int i = 0; i < 4; i++) {
            testBtns[i]->setPaletteForegroundColor(m_textColor1);
            testBtns[i]->setPaletteBackgroundColor(m_bgColor1);
        }

        // All button + top bar controls (color2/bg2)
        pushButton_All->setPaletteForegroundColor(m_textColor2);
        pushButton_All->setPaletteBackgroundColor(m_bgColor2);
        loopsCount->setPaletteForegroundColor(m_textColor2);
        loopsCount->setPaletteBackgroundColor(m_bgColor2);
        comboBox_fileSize->setPaletteForegroundColor(m_textColor2);
        comboBox_fileSize->setPaletteBackgroundColor(m_bgColor2);
        comboBox_Storages->setPaletteForegroundColor(m_textColor2);
        comboBox_Storages->setPaletteBackgroundColor(m_bgColor2);
        refreshStoragesButton->setPaletteForegroundColor(m_textColor2);
        refreshStoragesButton->setPaletteBackgroundColor(m_bgColor2);
        comboBox_ComparisonUnit->setPaletteForegroundColor(m_textColor2);
        comboBox_ComparisonUnit->setPaletteBackgroundColor(m_bgColor2);
        comboBox_MixRatio->setPaletteForegroundColor(m_textColor2);
        comboBox_MixRatio->setPaletteBackgroundColor(m_bgColor2);

        label_Read->setPaletteForegroundColor(m_textColor1);
        label_Read->setPaletteBackgroundColor(m_bgColor3);
        label_Write->setPaletteForegroundColor(m_textColor1);
        label_Write->setPaletteBackgroundColor(m_bgColor3);
        label_Mix->setPaletteForegroundColor(m_textColor1);
        label_Mix->setPaletteBackgroundColor(m_bgColor3);
    } else {
        // ThemeNone: reset all to system defaults
        for (int i = 0; i < 4; i++) {
            testBtns[i]->unsetPalette();
        }
        pushButton_All->unsetPalette();
        loopsCount->unsetPalette();
        comboBox_fileSize->unsetPalette();
        comboBox_Storages->unsetPalette();
        refreshStoragesButton->unsetPalette();
        comboBox_ComparisonUnit->unsetPalette();
        comboBox_MixRatio->unsetPalette();
        label_Read->unsetPalette();
        label_Write->unsetPalette();
        label_Mix->unsetPalette();
    }
}

TQtMeterGauge* MainWindow::createGaugeForBar(BenchmarkProgressBar* bar, TQWidget* parent)
{
    TQtMeterGauge* g = new TQtMeterGauge(parent);
    g->setMinValue(0.0);
    g->setMaxValue(100.0);  // Log scale: 0-100
    g->setValue(0.0);
    g->setRadius(80);
    g->setLabel("");
    g->setDrawValue(false);
    g->hide();
    m_gaugeMap[bar] = g;
    return g;
}

void MainWindow::applyGaugeStyleForUnit(TQtMeterGauge* g, Global::ComparisonUnit unit)
{
    if (!g) return;
    g->clearThresholdRanges();

    switch (unit) {
    case Global::ComparisonUnit::IOPS:
        // ===== "Light" style (like example: light bg, red needle, green/yellow/red arc) =====
        g->setBackgroundColor(TQColor(245, 245, 245));
        g->setNeedleColor(TQColor(180, 20, 20));
        g->setTextColor(TQColor(10, 10, 10));
        g->setGridColor(TQColor(60, 60, 60));
        g->setUnitsLabel("IOPS");
        g->setStartScaleAngle(45);
        g->setStopScaleAngle(315);
        g->setScaleStep(1.0);
        g->setScaleGridStep(10.0);
        g->setDrawGridValues(true);
        g->setDrawValuePrecision(0);
        g->setScaleLabelPrecision(0);
        g->setThresholdRange(0.0,  70.0, 0, TQColor(0, 160, 0));
        g->setThresholdRange(70.0, 90.0, 1, TQColor(255, 170, 0));
        g->setThresholdRange(90.0, 100.1, 2, TQColor(220, 0, 0));
        break;

    case Global::ComparisonUnit::Latency:
        // ===== "Narrow sweep" style (like example: dark bg, white needle, narrow arc) =====
        g->setBackgroundColor(TQColor(0, 0, 0));
        g->setNeedleColor(TQColor(255, 255, 255));
        g->setTextColor(TQColor(255, 255, 255));
        g->setGridColor(TQColor(255, 255, 255));
        g->setUnitsLabel(TQString(TQChar(0xB5)) + "s");
        g->setStartScaleAngle(120);
        g->setStopScaleAngle(240);
        g->setScaleStep(5.0);
        g->setScaleGridStep(20.0);
        g->setDrawGridValues(true);
        g->setDrawValuePrecision(0);
        g->setScaleLabelPrecision(0);
        g->setThresholdRange(0.0,  60.0, 0, TQColor(0, 160, 0));
        g->setThresholdRange(60.0, 80.0, 1, TQColor(255, 170, 0));
        g->setThresholdRange(80.0, 100.1, 2, TQColor(255, 0, 0));
        break;

    default: // MB/s, GB/s
        // ===== "Classic" style (like example: dark bg, blue needle, full sweep, grid values) =====
        g->setBackgroundColor(TQColor(0, 0, 0));
        g->setNeedleColor(TQColor(0, 0, 255));
        g->setTextColor(TQColor(255, 255, 255));
        g->setGridColor(TQColor(255, 255, 255));
        g->setUnitsLabel(unit == Global::ComparisonUnit::GBPerSec ? "GB/s" : "MB/s");
        g->setStartScaleAngle(35);
        g->setStopScaleAngle(325);
        g->setScaleStep(2.0);
        g->setScaleGridStep(10.0);
        g->setDrawGridValues(true);
        g->setDrawValuePrecision(0);
        g->setScaleLabelPrecision(0);
        g->setThresholdRange(0.0,  55.0, 0);
        g->setThresholdRange(55.0, 75.0, 1, TQColor(255, 255, 0));
        g->setThresholdRange(75.0, 100.1, 2, TQColor(255, 0, 0));
        break;
    }
}

void MainWindow::rebuildGauges()
{
    // Gauges are now created in setupUi, nothing to rebuild
}

void MainWindow::updateGauge(BenchmarkProgressBar* bar)
{
    TQtMeterGauge* g = m_gaugeMap[bar];
    if (!g) return;

    const AppSettings settings;

    Global::ComparisonUnit comparisonUnit = Global::ComparisonUnit::MBPerSec;
    switch (settings.getPerformanceProfile()) {
    case Global::PerformanceProfile::Peak:
    case Global::PerformanceProfile::RealWorld:
        if (bar == readBar_3 || bar == writeBar_3 || bar == mixBar_3) {
            comparisonUnit = Global::ComparisonUnit::IOPS;
        } else if (bar == readBar_4 || bar == writeBar_4 || bar == mixBar_4) {
            comparisonUnit = Global::ComparisonUnit::Latency;
        }
        break;
    default:
        comparisonUnit = settings.getComparisonUnit();
        break;
    }

    double value = 0;
    TQString valueStr;
    switch (comparisonUnit) {
    case Global::ComparisonUnit::MBPerSec:
        value = bar->MBPerSec;
        g->setValue(value <= 0.1 ? 0 : 16.666666666666 * log10(value * 10));
        valueStr = (value >= 1000000.0) ? TQString::number((int)value) : TQString::number(value, 'f', 1);
        break;
    case Global::ComparisonUnit::GBPerSec:
        value = bar->GBPerSec;
        g->setValue(value <= 0.0001 ? 0 : 16.666666666666 * log10(value * 10000));
        valueStr = TQString::number(value, 'f', 3);
        break;
    case Global::ComparisonUnit::IOPS:
        value = bar->IOPS;
        g->setValue(value <= 0.1 ? 0 : 16.666666666666 * log10(value * 10));
        valueStr = (value >= 1000000.0) ? TQString::number((int)value) : TQString::number(value, 'f', 1);
        break;
    case Global::ComparisonUnit::Latency:
        value = bar->Latency;
        g->setValue(value <= 0.0000000001 ? 0 : 100 - 16.666666666666 * log10(value));
        valueStr = TQString::number(value, 'f', 2);
        break;
    }
    g->setLabel(value <= 0.001 ? "" : valueStr);
}

void MainWindow::benchmarkStateChanged(bool state)
{
    if (state) {
        m_baseDeviceModelText = deviceModel->text();
        menubar->setEnabled(false);
        loopsCount->setEnabled(false);
        comboBox_fileSize->setEnabled(false);
        comboBox_Storages->setEnabled(false);
        refreshStoragesButton->setEnabled(false);
        comboBox_ComparisonUnit->setEnabled(false);
        comboBox_MixRatio->setEnabled(false);
        
        pushButton_All->setEnabled(false);
        pushButton_Test_1->setEnabled(false);
        pushButton_Test_2->setEnabled(false);
        pushButton_Test_3->setEnabled(false);
        pushButton_Test_4->setEnabled(false);

        // If launched with All, All button is immediately active as Stop
        if (m_isRunningAll) {
            pushButton_All->setEnabled(true);
            pushButton_All->setText(tr("Stop"));
        }
        m_pulsePhase = 0;
        m_benchmarkCancelled = false;
        m_pulseTimer->start(50);
    }
    else {
        deviceModel->setText(m_baseDeviceModelText);
        setCaption(m_windowTitle);
        m_isRunningAll = false;
        pushButton_All->setEnabled(true);
        pushButton_Test_1->setEnabled(true);
        pushButton_Test_2->setEnabled(true);
        pushButton_Test_3->setEnabled(true);
        pushButton_Test_4->setEnabled(true);
        menubar->setEnabled(true);
        loopsCount->setEnabled(true);
        comboBox_fileSize->setEnabled(true);
        comboBox_Storages->setEnabled(true);
        refreshStoragesButton->setEnabled(true);
        comboBox_ComparisonUnit->setEnabled(true);
        comboBox_MixRatio->setEnabled(true);
        pushButton_All->setText(tr("All"));
        m_pulseTimer->stop();
        applyThemeColors();
        updateBenchmarkButtonsContent();

        // Send desktop notification on normal completion (not on explicit user-initiated Stop).
        // Notification is ALWAYS sent on normal completion, regardless of the menu toggle.
        if (!m_benchmarkCancelled) {
            const char *iconPath = "/tmp/tdediskmark.png";
            if (access(iconPath, F_OK) != 0) {
                FILE *f = fopen(iconPath, "wb");
                if (f) {
                    fwrite(___icons_tdediskmark_png, 1, ___icons_tdediskmark_png_len, f);
                    fclose(f);
                }
            }
            if (notify_init("tdeDiskMark")) {
                NotifyNotification *n = notify_notification_new(
                    "tdeDiskMark",
                    "Benchmark completed.",
                    iconPath);
                notify_notification_set_timeout(n, 5000);
                notify_notification_show(n, NULL);
                g_object_unref(G_OBJECT(n));
                notify_uninit();
            }
        }
    }
}

void MainWindow::onCurrentTestRunning(Global::BenchmarkTest test)
{
    updateBenchmarkButtonsContent(); // reset button labels to profile names
    applyThemeColors();              // wipe any residual red from previous test

    // Deterministic rule — exactly one test button becomes Stop, all others disabled:
    pushButton_Test_1->setEnabled(false);
    pushButton_Test_2->setEnabled(false);
    pushButton_Test_3->setEnabled(false);
    pushButton_Test_4->setEnabled(false);

    switch (test) {
    case Global::BenchmarkTest::Test_1: pushButton_Test_1->setEnabled(true); pushButton_Test_1->setText(tr("Stop")); break;
    case Global::BenchmarkTest::Test_2: pushButton_Test_2->setEnabled(true); pushButton_Test_2->setText(tr("Stop")); break;
    case Global::BenchmarkTest::Test_3: pushButton_Test_3->setEnabled(true); pushButton_Test_3->setText(tr("Stop")); break;
    case Global::BenchmarkTest::Test_4: pushButton_Test_4->setEnabled(true); pushButton_Test_4->setText(tr("Stop")); break;
    }

    // All button: Stop only if the series was started with All, otherwise stays disabled
    if (m_isRunningAll) {
        pushButton_All->setEnabled(true);
        pushButton_All->setText(tr("Stop"));
    } else {
        pushButton_All->setEnabled(false);
    }
}

void MainWindow::pulseStopButtons()
{
    m_pulsePhase++;
    double t = sin((double)m_pulsePhase * 0.08) * 0.5 + 0.5; // 0..1 oscillation
    int r = (int)(150 + t * 80);  // 150..230
    int g = (int)(30  + t * 30);  // 30..60
    int b = (int)(30  + t * 30);  // 30..60
    TQColor c(r, g, b);

    // Only pulse buttons that are actually labeled "Stop" — all others must stay grayed out
    TQPushButton* btns[] = { pushButton_All, pushButton_Test_1, pushButton_Test_2, pushButton_Test_3, pushButton_Test_4 };
    for (int i = 0; i < 5; i++) {
        if (!btns[i]->isEnabled()) continue;
        
        bool shouldPulse = false;
        if (btns[i] == pushButton_All) {
            if (m_isRunningAll) shouldPulse = true;
        } else {
            if (btns[i]->text() == tr("Stop") || btns[i]->text() == "Stop") {
                shouldPulse = true;
            }
        }
        
        if (shouldPulse) {
            btns[i]->setPaletteBackgroundColor(c);
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionHelpContents_triggered()
{
    kapp->invokeHelp();
}


void MainWindow::on_actionSettings_triggered()
{
    Settings dialog(this);
    if (dialog.exec() == TQDialog::Accepted) {
        updateBenchmarkButtonsContent();
        
        AppSettings settings;
        if (settings.getMixedState()) {
            label_Mix->show();
            cell_m1->show();
            cell_m2->show();
            cell_m3->show();
            cell_m4->show();
            comboBox_MixRatio->show();
            m_gridLayout->setColStretch(3, 1);
        } else {
            label_Mix->hide();
            cell_m1->hide();
            cell_m2->hide();
            cell_m3->hide();
            cell_m4->hide();
            comboBox_MixRatio->hide();
            m_gridLayout->setColStretch(3, 0);
        }
    }
}

void MainWindow::defineBenchmark(std::function<void()> bodyFunc)
{
    AppSettings settings;

    if (m_benchmark->isRunning()) {
        benchmarkStatusUpdate(tr("Stopping..."));
        pushButton_All->setEnabled(false);
        pushButton_Test_1->setEnabled(false);
        pushButton_Test_2->setEnabled(false);
        pushButton_Test_3->setEnabled(false);
        pushButton_Test_4->setEnabled(false);
        m_benchmarkCancelled = true;
        m_benchmark->setRunning(false);
    }
    else {
        if (m_benchmark->getBenchmarkFile().isNull()) {
            TQMessageBox::critical(this, tr("Not available"), tr("Directory is not specified."));
        }
        else {
            bodyFunc();
        }
    }
}

void MainWindow::benchmarkFailed(const TQString &error)
{
    TQMessageBox::critical(this, tr("Benchmark Failed"), error);
}

void MainWindow::benchmarkStatusUpdate(const TQString &name)
{
    TQString prefix = name.startsWith("Waiting") || name.startsWith(tr("Preparation")) ? "" : tr("Benchmarking: ");
    deviceModel->setText(TQString("%1  -  %2%3").arg(m_baseDeviceModelText, prefix, name));
}

void MainWindow::handleResults(BenchmarkProgressBar *progressBar, const Benchmark::PerformanceResult &result)
{
    if (!progressBar) return;
    
    // Set ONLY target values — animation timer eases current → target (both text + bar)
    progressBar->targetMBPerSec = result.Bandwidth;
    progressBar->targetGBPerSec = result.Bandwidth / 1000.0;
    progressBar->targetIOPS     = result.IOPS;
    progressBar->targetLatency  = result.Latency;

    if (!progressBar->isDemo) {
        TQToolTip::add(progressBar, 
                    Global::getToolTipTemplate().arg(
                        TQString::number(result.Bandwidth, 'f', 3),
                        TQString::number(result.Bandwidth / 1000.0, 'f', 3),
                        TQString::number(result.IOPS, 'f', 3),
                        TQString::number(result.Latency, 'f', 3)
                        )
                    );
    }

}

#include <cmath>

void MainWindow::animateProgressBars()
{
    float easing = 0.15f;
    for (auto* bar : m_progressBars) {
        bool changed = false;

        if (std::abs(bar->targetMBPerSec - bar->MBPerSec) > 0.00001) {
            bar->MBPerSec += (bar->targetMBPerSec - bar->MBPerSec) * easing;
            if (std::abs(bar->targetMBPerSec - bar->MBPerSec) < 0.001) bar->MBPerSec = bar->targetMBPerSec;
            changed = true;
        } else if (bar->MBPerSec != bar->targetMBPerSec) {
            bar->MBPerSec = bar->targetMBPerSec;
            changed = true;
        }

        if (std::abs(bar->targetGBPerSec - bar->GBPerSec) > 0.00001) {
            bar->GBPerSec += (bar->targetGBPerSec - bar->GBPerSec) * easing;
            if (std::abs(bar->targetGBPerSec - bar->GBPerSec) < 0.001) bar->GBPerSec = bar->targetGBPerSec;
            changed = true;
        } else if (bar->GBPerSec != bar->targetGBPerSec) {
            bar->GBPerSec = bar->targetGBPerSec;
            changed = true;
        }

        if (std::abs(bar->targetIOPS - bar->IOPS) > 0.00001) {
            bar->IOPS += (bar->targetIOPS - bar->IOPS) * easing;
            if (std::abs(bar->targetIOPS - bar->IOPS) < 0.01) bar->IOPS = bar->targetIOPS;
            changed = true;
        } else if (bar->IOPS != bar->targetIOPS) {
            bar->IOPS = bar->targetIOPS;
            changed = true;
        }

        if (std::abs(bar->targetLatency - bar->Latency) > 0.00001) {
            bar->Latency += (bar->targetLatency - bar->Latency) * easing;
            if (std::abs(bar->targetLatency - bar->Latency) < 0.001) bar->Latency = bar->targetLatency;
            changed = true;
        } else if (bar->Latency != bar->targetLatency) {
            bar->Latency = bar->targetLatency;
            changed = true;
        }

        if (changed) {
            updateProgressBar(bar);
            updateGauge(bar);
        }
    }
}

void MainWindow::snapProgressBars()
{
    for (auto* bar : m_progressBars) {
        if (!bar) continue;
        bar->MBPerSec = bar->targetMBPerSec;
        bar->GBPerSec = bar->targetGBPerSec;
        bar->IOPS = bar->targetIOPS;
        bar->Latency = bar->targetLatency;
        updateProgressBar(bar);
        updateGauge(bar);
    }
}

void MainWindow::updateProgressBar(BenchmarkProgressBar *progressBar)
{
    const AppSettings settings;

    

    float score = progressBar->MBPerSec;

    float value;

    Global::ComparisonUnit comparisonUnit = Global::ComparisonUnit::MBPerSec;

    switch (settings.getPerformanceProfile()) {
    case Global::PerformanceProfile::Peak:
    case Global::PerformanceProfile::RealWorld:
        if (progressBar == readBar_3 || progressBar == writeBar_3 || progressBar == mixBar_3) {
            comparisonUnit = Global::ComparisonUnit::IOPS;
        }
        else if (progressBar == readBar_4 || progressBar == writeBar_4 || progressBar == mixBar_4) {
            comparisonUnit = Global::ComparisonUnit::Latency;
        }
        break;
    default:
        comparisonUnit = settings.getComparisonUnit();
        break;
    }

    
    switch (comparisonUnit) {
    case Global::ComparisonUnit::MBPerSec:
        progressBar->setFormatText(score >= 1000000.0 ? TQString::number((int)score) : TQString::number(score, 'f', progressBar->isDemo ? 1 : 2));
        break;
    case Global::ComparisonUnit::GBPerSec:
        value = progressBar->GBPerSec;
        progressBar->setFormatText(TQString::number(value, 'f', progressBar->isDemo ? 1 : 3));
        break;
    case Global::ComparisonUnit::IOPS:
        value = progressBar->IOPS;
        progressBar->setFormatText(value >= 1000000.0 ? TQString::number((int)value) : TQString::number(value, 'f', progressBar->isDemo ? 0 : 2));
        break;
    case Global::ComparisonUnit::Latency:
        value = progressBar->Latency;
        progressBar->setFormatText(value >= 1000000.0 ? TQString::number((int)value) : TQString::number(value, 'f', progressBar->isDemo ? 1 : 2));
        break;
    }

    if (comparisonUnit == Global::ComparisonUnit::Latency) {
        progressBar->setProgress(value <= 0.0000000001 ? 0 : 100 - 16.666666666666 * log10(value));
    }
    else {
        progressBar->setProgress(score <= 0.1 ? 0 : 16.666666666666 * log10(score * 10));
    }

    // Color the progress bar based on unit type
    switch (comparisonUnit) {
    case Global::ComparisonUnit::IOPS:
        progressBar->setBarColor(TQColor(40, 120, 220));
        break;
    case Global::ComparisonUnit::Latency:
        progressBar->setBarColor(TQColor(220, 140, 40));
        break;
    default:
        progressBar->setBarColor(TQColor(50, 180, 80));
        break;
    }
}


void MainWindow::handleDirectoryChanged(const TQString &newDir)
{
    struct statvfs st;
    uint64_t bT = 0, bO = 0;
    if (statvfs(newDir.local8Bit().data(), &st) == 0) {
        bT = (uint64_t)st.f_blocks * st.f_frsize;
        bO = bT - ((uint64_t)st.f_bfree * st.f_frsize);
    }

    Global::Storage storage {
        .path = newDir,
        .bytesTotal = static_cast<long long>(bT),
        .bytesOccupied = static_cast<long long>(bO),
        .formatedSize = formatSize(bO, bT),
        .permanentInList = true
    };

    addItemToStoragesList(storage);
    resizeComboBoxItemsPopup(comboBox_Storages);
}

bool MainWindow::runCombinedRandomTest()
{
    const AppSettings settings;

    if (settings.getPerformanceProfile() == Global::PerformanceProfile::Peak || settings.getPerformanceProfile() == Global::PerformanceProfile::RealWorld) {
        TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > set;

        TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_2); rd.push_back(readBar_3); rd.push_back(readBar_4);
        TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_2); wr.push_back(writeBar_3); wr.push_back(writeBar_4);
        set.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Read), rd));
        set.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Write), wr));

        if (settings.getMixedState()) {
            TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBar_2); mx.push_back(mixBar_3); mx.push_back(mixBar_4);
            set.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Mix), mx));
        }

        m_benchmark->runBenchmark(set);
        return true;
    }

    return false;
}

void MainWindow::on_pushButton_Test_1_clicked() {
    const AppSettings settings;
    TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > tests;
    
    TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_1);
    TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_1);
    
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Read), rd));
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Write), wr));
    
    if (settings.getMixedState()) {
        TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBar_1);
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Mix), mx));
    }
    
    defineBenchmark([this, tests] { m_benchmark->runBenchmark(tests); });
}

void MainWindow::on_pushButton_Test_2_clicked() {
    defineBenchmark([this] {
        if (runCombinedRandomTest()) return;
        const AppSettings settings;
        TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > tests;
    
    TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_2);
    TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_2);
    
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Read), rd));
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Write), wr));
    
    if (settings.getMixedState()) {
        TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBar_2);
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Mix), mx));
    }
    
        m_benchmark->runBenchmark(tests);
    });
}

void MainWindow::on_pushButton_Test_3_clicked() {
    defineBenchmark([this] {
        if (runCombinedRandomTest()) return;
        const AppSettings settings;
        TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > tests;
    
    TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_3);
    TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_3);
    
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_3, Global::BenchmarkIOReadWrite::Read), rd));
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_3, Global::BenchmarkIOReadWrite::Write), wr));
    
    if (settings.getMixedState()) {
        TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBar_3);
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_3, Global::BenchmarkIOReadWrite::Mix), mx));
    }
    
        m_benchmark->runBenchmark(tests);
    });
}

void MainWindow::on_pushButton_Test_4_clicked() {
    defineBenchmark([this] {
        if (runCombinedRandomTest()) return;
        const AppSettings settings;
        TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > tests;
    
    TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_4);
    TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_4);
    
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_4, Global::BenchmarkIOReadWrite::Read), rd));
    tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_4, Global::BenchmarkIOReadWrite::Write), wr));
    
    if (settings.getMixedState()) {
        TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBar_4);
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_4, Global::BenchmarkIOReadWrite::Mix), mx));
    }
    
        m_benchmark->runBenchmark(tests);
    });
}

void MainWindow::on_pushButton_All_clicked() {
    m_isRunningAll = true;
    const AppSettings settings;
    TQValueList<std::pair<std::pair<Global::BenchmarkTest, Global::BenchmarkIOReadWrite>, TQValueVector<BenchmarkProgressBar*> > > tests;

    if (settings.getPerformanceProfile() == Global::PerformanceProfile::Demo) {
        TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBar_Demo);
        TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBar_Demo);
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Read), rd));
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Write), wr));
    } else if (settings.getPerformanceProfile() == Global::PerformanceProfile::Default) {
        // All reads first, then all writes (matching original KDiskMark order)
        BenchmarkProgressBar* readBars[]  = { readBar_1,  readBar_2,  readBar_3,  readBar_4  };
        BenchmarkProgressBar* writeBars[] = { writeBar_1, writeBar_2, writeBar_3, writeBar_4 };
        BenchmarkProgressBar* mixBars[]   = { mixBar_1,   mixBar_2,   mixBar_3,   mixBar_4   };

        for (int t = 0; t < 4; ++t) {
            Global::BenchmarkTest test = static_cast<Global::BenchmarkTest>(t);
            TQValueVector<BenchmarkProgressBar*> rd; rd.push_back(readBars[t]);
            tests.push_back(std::make_pair(std::make_pair(test, Global::BenchmarkIOReadWrite::Read), rd));
        }
        for (int t = 0; t < 4; ++t) {
            Global::BenchmarkTest test = static_cast<Global::BenchmarkTest>(t);
            TQValueVector<BenchmarkProgressBar*> wr; wr.push_back(writeBars[t]);
            tests.push_back(std::make_pair(std::make_pair(test, Global::BenchmarkIOReadWrite::Write), wr));
        }
        if (settings.getMixedState()) {
            for (int t = 0; t < 4; ++t) {
                Global::BenchmarkTest test = static_cast<Global::BenchmarkTest>(t);
                TQValueVector<BenchmarkProgressBar*> mx; mx.push_back(mixBars[t]);
                tests.push_back(std::make_pair(std::make_pair(test, Global::BenchmarkIOReadWrite::Mix), mx));
            }
        }
    } else {
        // Peak / RealWorld: Test_1 on bar_1 alone, Test_2 on bars 2+3+4
        TQValueVector<BenchmarkProgressBar*> rd1; rd1.push_back(readBar_1);
        TQValueVector<BenchmarkProgressBar*> rd2; rd2.push_back(readBar_2); rd2.push_back(readBar_3); rd2.push_back(readBar_4);
        TQValueVector<BenchmarkProgressBar*> wr1; wr1.push_back(writeBar_1);
        TQValueVector<BenchmarkProgressBar*> wr2; wr2.push_back(writeBar_2); wr2.push_back(writeBar_3); wr2.push_back(writeBar_4);

        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Read), rd1));
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Read), rd2));
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Write), wr1));
        tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Write), wr2));

        if (settings.getMixedState()) {
            TQValueVector<BenchmarkProgressBar*> mx1; mx1.push_back(mixBar_1);
            TQValueVector<BenchmarkProgressBar*> mx2; mx2.push_back(mixBar_2); mx2.push_back(mixBar_3); mx2.push_back(mixBar_4);
            tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_1, Global::BenchmarkIOReadWrite::Mix), mx1));
            tests.push_back(std::make_pair(std::make_pair(Global::BenchmarkTest::Test_2, Global::BenchmarkIOReadWrite::Mix), mx2));
        }
    }
    
    defineBenchmark([this, tests] { m_benchmark->runBenchmark(tests); });
}
