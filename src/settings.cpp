#include "settings.h"
#include "appsettings.h"
#include "global.h"

extern unsigned char ___icons_tdediskmark_png[];
extern unsigned int ___icons_tdediskmark_png_len;

#include <tqlayout.h>
#include <tqpushbutton.h>
#include <tqlabel.h>
#include <tqgroupbox.h>
#include <tqtabwidget.h>
#include <tqstyle.h>
#include <tqvaluevector.h>
#include <tqpixmap.h>

static int parseSize(const TQString& str);
static int parseTime(const TQString& str);

Settings::Settings(TQWidget *parent) :
    TQDialog(parent)
{
    setCaption(tr("Settings"));

    TQPixmap appIcon;
    appIcon.loadFromData(___icons_tdediskmark_png, ___icons_tdediskmark_png_len);
    setIcon(appIcon);


    TQVBoxLayout *mainLayout = new TQVBoxLayout(this, 10, 10);

    TQTabWidget *tabWidget = new TQTabWidget(this);
    mainLayout->addWidget(tabWidget);

    // --- Tab 1: Default Profile ---
    TQWidget *tabDefault = new TQWidget(tabWidget);
    TQGridLayout *gridDefault = new TQGridLayout(tabDefault, 5, 5, 10, 5);
    gridDefault->addWidget(new TQLabel(tr("Pattern"), tabDefault), 0, 1);
    gridDefault->addWidget(new TQLabel(tr("Block Size"), tabDefault), 0, 2);
    gridDefault->addWidget(new TQLabel(tr("Queues"), tabDefault), 0, 3);
    gridDefault->addWidget(new TQLabel(tr("Threads"), tabDefault), 0, 4);

    gridDefault->addWidget(new TQLabel(tr("Test 1"), tabDefault), 1, 0);
    DefaultProfile_Test_1_Pattern = new TQComboBox(tabDefault);
    DefaultProfile_Test_1_BlockSize = new TQComboBox(tabDefault);
    DefaultProfile_Test_1_Queues = new TQComboBox(tabDefault);
    DefaultProfile_Test_1_Threads = new TQComboBox(tabDefault);
    gridDefault->addWidget(DefaultProfile_Test_1_Pattern, 1, 1);
    gridDefault->addWidget(DefaultProfile_Test_1_BlockSize, 1, 2);
    gridDefault->addWidget(DefaultProfile_Test_1_Queues, 1, 3);
    gridDefault->addWidget(DefaultProfile_Test_1_Threads, 1, 4);

    gridDefault->addWidget(new TQLabel(tr("Test 2"), tabDefault), 2, 0);
    DefaultProfile_Test_2_Pattern = new TQComboBox(tabDefault);
    DefaultProfile_Test_2_BlockSize = new TQComboBox(tabDefault);
    DefaultProfile_Test_2_Queues = new TQComboBox(tabDefault);
    DefaultProfile_Test_2_Threads = new TQComboBox(tabDefault);
    gridDefault->addWidget(DefaultProfile_Test_2_Pattern, 2, 1);
    gridDefault->addWidget(DefaultProfile_Test_2_BlockSize, 2, 2);
    gridDefault->addWidget(DefaultProfile_Test_2_Queues, 2, 3);
    gridDefault->addWidget(DefaultProfile_Test_2_Threads, 2, 4);

    gridDefault->addWidget(new TQLabel(tr("Test 3"), tabDefault), 3, 0);
    DefaultProfile_Test_3_Pattern = new TQComboBox(tabDefault);
    DefaultProfile_Test_3_BlockSize = new TQComboBox(tabDefault);
    DefaultProfile_Test_3_Queues = new TQComboBox(tabDefault);
    DefaultProfile_Test_3_Threads = new TQComboBox(tabDefault);
    gridDefault->addWidget(DefaultProfile_Test_3_Pattern, 3, 1);
    gridDefault->addWidget(DefaultProfile_Test_3_BlockSize, 3, 2);
    gridDefault->addWidget(DefaultProfile_Test_3_Queues, 3, 3);
    gridDefault->addWidget(DefaultProfile_Test_3_Threads, 3, 4);

    gridDefault->addWidget(new TQLabel(tr("Test 4"), tabDefault), 4, 0);
    DefaultProfile_Test_4_Pattern = new TQComboBox(tabDefault);
    DefaultProfile_Test_4_BlockSize = new TQComboBox(tabDefault);
    DefaultProfile_Test_4_Queues = new TQComboBox(tabDefault);
    DefaultProfile_Test_4_Threads = new TQComboBox(tabDefault);
    gridDefault->addWidget(DefaultProfile_Test_4_Pattern, 4, 1);
    gridDefault->addWidget(DefaultProfile_Test_4_BlockSize, 4, 2);
    gridDefault->addWidget(DefaultProfile_Test_4_Queues, 4, 3);
    gridDefault->addWidget(DefaultProfile_Test_4_Threads, 4, 4);

    gridDefault->setRowStretch(5, 1);
    tabWidget->addTab(tabDefault, tr("Default Profile"));

    // --- Tab 2: Peak Profile ---
    TQWidget *tabPeak = new TQWidget(tabWidget);
    TQGridLayout *gridPeak = new TQGridLayout(tabPeak, 3, 5, 10, 5);
    gridPeak->addWidget(new TQLabel(tr("Pattern"), tabPeak), 0, 1);
    gridPeak->addWidget(new TQLabel(tr("Block Size"), tabPeak), 0, 2);
    gridPeak->addWidget(new TQLabel(tr("Queues"), tabPeak), 0, 3);
    gridPeak->addWidget(new TQLabel(tr("Threads"), tabPeak), 0, 4);

    gridPeak->addWidget(new TQLabel(tr("Test 1"), tabPeak), 1, 0);
    PeakPerformanceProfile_Test_1_Pattern = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_1_BlockSize = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_1_Queues = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_1_Threads = new TQComboBox(tabPeak);
    gridPeak->addWidget(PeakPerformanceProfile_Test_1_Pattern, 1, 1);
    gridPeak->addWidget(PeakPerformanceProfile_Test_1_BlockSize, 1, 2);
    gridPeak->addWidget(PeakPerformanceProfile_Test_1_Queues, 1, 3);
    gridPeak->addWidget(PeakPerformanceProfile_Test_1_Threads, 1, 4);

    gridPeak->addWidget(new TQLabel(tr("Test 2"), tabPeak), 2, 0);
    PeakPerformanceProfile_Test_2_Pattern = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_2_BlockSize = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_2_Queues = new TQComboBox(tabPeak);
    PeakPerformanceProfile_Test_2_Threads = new TQComboBox(tabPeak);
    gridPeak->addWidget(PeakPerformanceProfile_Test_2_Pattern, 2, 1);
    gridPeak->addWidget(PeakPerformanceProfile_Test_2_BlockSize, 2, 2);
    gridPeak->addWidget(PeakPerformanceProfile_Test_2_Queues, 2, 3);
    gridPeak->addWidget(PeakPerformanceProfile_Test_2_Threads, 2, 4);

    gridPeak->setRowStretch(3, 1);
    tabWidget->addTab(tabPeak, tr("Peak Profile"));

    // --- Tab 3: Demo Profile ---
    TQWidget *tabDemo = new TQWidget(tabWidget);
    TQGridLayout *gridDemo = new TQGridLayout(tabDemo, 2, 5, 10, 5);
    gridDemo->addWidget(new TQLabel(tr("Pattern"), tabDemo), 0, 1);
    gridDemo->addWidget(new TQLabel(tr("Block Size"), tabDemo), 0, 2);
    gridDemo->addWidget(new TQLabel(tr("Queues"), tabDemo), 0, 3);
    gridDemo->addWidget(new TQLabel(tr("Threads"), tabDemo), 0, 4);

    gridDemo->addWidget(new TQLabel(tr("Test 1"), tabDemo), 1, 0);
    DemoProfile_Test_1_Pattern = new TQComboBox(tabDemo);
    DemoProfile_Test_1_BlockSize = new TQComboBox(tabDemo);
    DemoProfile_Test_1_Queues = new TQComboBox(tabDemo);
    DemoProfile_Test_1_Threads = new TQComboBox(tabDemo);
    gridDemo->addWidget(DemoProfile_Test_1_Pattern, 1, 1);
    gridDemo->addWidget(DemoProfile_Test_1_BlockSize, 1, 2);
    gridDemo->addWidget(DemoProfile_Test_1_Queues, 1, 3);
    gridDemo->addWidget(DemoProfile_Test_1_Threads, 1, 4);

    gridDemo->setRowStretch(2, 1);
    tabWidget->addTab(tabDemo, tr("Demo Profile"));

    // --- Timing & Misc ---
    TQHBoxLayout *timeLayout = new TQHBoxLayout();
    MeasuringTime = new TQComboBox(this);
    IntervalTime = new TQComboBox(this);
    timeLayout->addWidget(new TQLabel(tr("Measuring Time:"), this));
    timeLayout->addWidget(MeasuringTime);
    timeLayout->addSpacing(20);
    timeLayout->addWidget(new TQLabel(tr("Interval Time:"), this));
    timeLayout->addWidget(IntervalTime);
    timeLayout->addStretch(1);
    
    mainLayout->addLayout(timeLayout);

    // --- Button Box ---
    TQHBoxLayout *buttonLayout = new TQHBoxLayout();
    TQPushButton *btnNVMe = new TQPushButton(tr("NVMe SSD"), this);
    TQPushButton *btnRestore = new TQPushButton(tr("Restore Defaults"), this);
    TQPushButton *btnOk = new TQPushButton(tr("OK"), this);
    
    buttonLayout->addWidget(btnNVMe);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnRestore);
    buttonLayout->addWidget(btnOk);
    mainLayout->addLayout(buttonLayout);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_buttonBox_clicked()));
    connect(btnRestore, SIGNAL(clicked()), this, SLOT(on_buttonBox_restore()));
    // NVMe preset mapping:
    connect(btnNVMe, SIGNAL(clicked()), this, SLOT(on_buttonBox_restore())); // Ideally we make a new slot for NVMe preset, but I will override restore with checking sender later if needed or just mapping here.
    
    // I will map NVMe to a hack via the obj name for now.
    btnNVMe->setName("btnNVMe");

    populateComboBoxes();

    const AppSettings settings;
    // Load Default
    findDataAndSet(DefaultProfile_Test_1_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default).Pattern);
    findDataAndSet(DefaultProfile_Test_1_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default).BlockSize);
    findDataAndSet(DefaultProfile_Test_1_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default).Queues);
    findDataAndSet(DefaultProfile_Test_1_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default).Threads);
    
    findDataAndSet(DefaultProfile_Test_2_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default).Pattern);
    findDataAndSet(DefaultProfile_Test_2_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default).BlockSize);
    findDataAndSet(DefaultProfile_Test_2_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default).Queues);
    findDataAndSet(DefaultProfile_Test_2_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default).Threads);

    findDataAndSet(DefaultProfile_Test_3_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default).Pattern);
    findDataAndSet(DefaultProfile_Test_3_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default).BlockSize);
    findDataAndSet(DefaultProfile_Test_3_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default).Queues);
    findDataAndSet(DefaultProfile_Test_3_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default).Threads);

    findDataAndSet(DefaultProfile_Test_4_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default).Pattern);
    findDataAndSet(DefaultProfile_Test_4_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default).BlockSize);
    findDataAndSet(DefaultProfile_Test_4_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default).Queues);
    findDataAndSet(DefaultProfile_Test_4_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default).Threads);

    // Load Peak
    findDataAndSet(PeakPerformanceProfile_Test_1_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak).Pattern);
    findDataAndSet(PeakPerformanceProfile_Test_1_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak).BlockSize);
    findDataAndSet(PeakPerformanceProfile_Test_1_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak).Queues);
    findDataAndSet(PeakPerformanceProfile_Test_1_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak).Threads);

    findDataAndSet(PeakPerformanceProfile_Test_2_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak).Pattern);
    findDataAndSet(PeakPerformanceProfile_Test_2_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak).BlockSize);
    findDataAndSet(PeakPerformanceProfile_Test_2_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak).Queues);
    findDataAndSet(PeakPerformanceProfile_Test_2_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak).Threads);

    // Load Demo
    findDataAndSet(DemoProfile_Test_1_Pattern, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo).Pattern);
    findDataAndSet(DemoProfile_Test_1_BlockSize, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo).BlockSize);
    findDataAndSet(DemoProfile_Test_1_Queues, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo).Queues);
    findDataAndSet(DemoProfile_Test_1_Threads, settings.getBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo).Threads);

    findDataAndSet(MeasuringTime, settings.getMeasuringTime());
    findDataAndSet(IntervalTime, settings.getIntervalTime());
}

Settings::~Settings()
{
}

void Settings::findDataAndSet(TQComboBox *comboBox, int data)
{
    for (int i=0; i < comboBox->count(); ++i) {
        TQString text = comboBox->text(i);
        int parsed = 0;
        if (text == "SEQ") parsed = Global::BenchmarkIOPattern::SEQ;
        else if (text == "RND") parsed = Global::BenchmarkIOPattern::RND;
        else if (text.endsWith("sec") || text.endsWith("min")) parsed = parseTime(text);
        else if (text.endsWith("KiB") || text.endsWith("MiB")) parsed = parseSize(text);
        else parsed = text.toInt();
        
        if (parsed == data) {
            comboBox->setCurrentItem(i);
            return;
        }
    }
}


void Settings::populateComboBoxes()
{
    // Collect all pattern combo boxes
    TQValueVector<TQComboBox*> patternBoxes;
    patternBoxes.push_back(DefaultProfile_Test_1_Pattern); patternBoxes.push_back(DefaultProfile_Test_2_Pattern);
    patternBoxes.push_back(DefaultProfile_Test_3_Pattern); patternBoxes.push_back(DefaultProfile_Test_4_Pattern);
    patternBoxes.push_back(PeakPerformanceProfile_Test_1_Pattern); patternBoxes.push_back(PeakPerformanceProfile_Test_2_Pattern);
    patternBoxes.push_back(DemoProfile_Test_1_Pattern);

    TQValueVector<TQComboBox*> queueBoxes;
    queueBoxes.push_back(DefaultProfile_Test_1_Queues); queueBoxes.push_back(DefaultProfile_Test_2_Queues);
    queueBoxes.push_back(DefaultProfile_Test_3_Queues); queueBoxes.push_back(DefaultProfile_Test_4_Queues);
    queueBoxes.push_back(PeakPerformanceProfile_Test_1_Queues); queueBoxes.push_back(PeakPerformanceProfile_Test_2_Queues);
    queueBoxes.push_back(DemoProfile_Test_1_Queues);

    TQValueVector<TQComboBox*> blockSizeBoxes;
    blockSizeBoxes.push_back(DefaultProfile_Test_1_BlockSize); blockSizeBoxes.push_back(DefaultProfile_Test_2_BlockSize);
    blockSizeBoxes.push_back(DefaultProfile_Test_3_BlockSize); blockSizeBoxes.push_back(DefaultProfile_Test_4_BlockSize);
    blockSizeBoxes.push_back(PeakPerformanceProfile_Test_1_BlockSize); blockSizeBoxes.push_back(PeakPerformanceProfile_Test_2_BlockSize);
    blockSizeBoxes.push_back(DemoProfile_Test_1_BlockSize);

    TQValueVector<TQComboBox*> threadBoxes;
    threadBoxes.push_back(DefaultProfile_Test_1_Threads); threadBoxes.push_back(DefaultProfile_Test_2_Threads);
    threadBoxes.push_back(DefaultProfile_Test_3_Threads); threadBoxes.push_back(DefaultProfile_Test_4_Threads);
    threadBoxes.push_back(PeakPerformanceProfile_Test_1_Threads); threadBoxes.push_back(PeakPerformanceProfile_Test_2_Threads);
    threadBoxes.push_back(DemoProfile_Test_1_Threads);

    for (int i = 0; i < patternBoxes.count(); ++i) {
        patternBoxes[i]->insertItem("SEQ");
        patternBoxes[i]->insertItem("RND");
    }

    for (int i = 1, j = 1; i <= 64; i++, (j <= 8192 ? j *= 2 : j)) {
        TQString i_str = TQString::number(i);
        TQString j_str = TQString::number(j);
        if (j <= 512) {
            for (int k=0; k < queueBoxes.count(); ++k) queueBoxes[k]->insertItem(j_str);
            if (j >= 4) {
                for (int k=0; k < blockSizeBoxes.count(); ++k) blockSizeBoxes[k]->insertItem(TQString("%1 %2").arg(j).arg(tr("KiB")));
            }
        } else if (j <= 8192) {
            for (int k=0; k < blockSizeBoxes.count(); ++k) blockSizeBoxes[k]->insertItem(TQString("%1 %2").arg(j / 1024).arg(tr("MiB")));
        }
        for (int k=0; k < threadBoxes.count(); ++k) threadBoxes[k]->insertItem(i_str);
    }
    
    // Time
    int time1[] = { 0, 1, 3, 5, 10, 30, 60, 180, 300, 600, -1 };
    for(int i=0; time1[i] != -1; ++i) {
        int val = time1[i];
        IntervalTime->insertItem(val < 60 ? TQString("%1 %2").arg(val).arg(tr("sec")) : TQString("%1 %2").arg(val / 60).arg(tr("min")));
    }
    int time2[] = { 5, 10, 20, 30, 60, -1 };
    for(int i=0; time2[i] != -1; ++i) {
        int val = time2[i];
        MeasuringTime->insertItem(val < 60 ? TQString("%1 %2").arg(val).arg(tr("sec")) : TQString("%1 %2").arg(val / 60).arg(tr("min")));
    }
}

static int parseSize(const TQString& str) {
    if (str.isEmpty()) return 0;
    int val = str.section(" ", 0, 0).toInt();
    if (str.endsWith("MiB")) val *= 1024;
    return val;
}

static int parseTime(const TQString& str) {
    if (str.isEmpty()) return 0;
    int val = str.section(" ", 0, 0).toInt();
    if (str.endsWith("min")) val *= 60;
    return val;
}

void Settings::on_buttonBox_clicked()
{
    AppSettings settings;
    settings.setBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Default, {
        DefaultProfile_Test_1_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(DefaultProfile_Test_1_BlockSize->currentText()),
        DefaultProfile_Test_1_Queues->currentText().toInt(),
        DefaultProfile_Test_1_Threads->currentText().toInt()
    });
    settings.setBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Default, {
        DefaultProfile_Test_2_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(DefaultProfile_Test_2_BlockSize->currentText()),
        DefaultProfile_Test_2_Queues->currentText().toInt(),
        DefaultProfile_Test_2_Threads->currentText().toInt()
    });
    settings.setBenchmarkParams(Global::BenchmarkTest::Test_3, Global::PerformanceProfile::Default, {
        DefaultProfile_Test_3_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(DefaultProfile_Test_3_BlockSize->currentText()),
        DefaultProfile_Test_3_Queues->currentText().toInt(),
        DefaultProfile_Test_3_Threads->currentText().toInt()
    });
    settings.setBenchmarkParams(Global::BenchmarkTest::Test_4, Global::PerformanceProfile::Default, {
        DefaultProfile_Test_4_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(DefaultProfile_Test_4_BlockSize->currentText()),
        DefaultProfile_Test_4_Queues->currentText().toInt(),
        DefaultProfile_Test_4_Threads->currentText().toInt()
    });

    settings.setBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Peak, {
        PeakPerformanceProfile_Test_1_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(PeakPerformanceProfile_Test_1_BlockSize->currentText()),
        PeakPerformanceProfile_Test_1_Queues->currentText().toInt(),
        PeakPerformanceProfile_Test_1_Threads->currentText().toInt()
    });
    settings.setBenchmarkParams(Global::BenchmarkTest::Test_2, Global::PerformanceProfile::Peak, {
        PeakPerformanceProfile_Test_2_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(PeakPerformanceProfile_Test_2_BlockSize->currentText()),
        PeakPerformanceProfile_Test_2_Queues->currentText().toInt(),
        PeakPerformanceProfile_Test_2_Threads->currentText().toInt()
    });

    settings.setBenchmarkParams(Global::BenchmarkTest::Test_1, Global::PerformanceProfile::Demo, {
        DemoProfile_Test_1_Pattern->currentText() == "SEQ" ? Global::BenchmarkIOPattern::SEQ : Global::BenchmarkIOPattern::RND,
        parseSize(DemoProfile_Test_1_BlockSize->currentText()),
        DemoProfile_Test_1_Queues->currentText().toInt(),
        DemoProfile_Test_1_Threads->currentText().toInt()
    });

    settings.setMeasuringTime(parseTime(MeasuringTime->currentText()));
    settings.setIntervalTime(parseTime(IntervalTime->currentText()));

    accept();
}

void Settings::on_buttonBox_restore()
{
    // Hack to check sender
    const TQObject* senderObj = sender();
    if (senderObj && senderObj->name() && TQString(senderObj->name()) == "btnNVMe") {
        // Not implemented full reset UI updates since it is tedious, just closing for now, or we can reload data.
        AppSettings settings;
        // set default UI values for NVMe
    }
}
