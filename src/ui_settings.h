#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H
#include <tqdialog.h>
#include <tqcombobox.h>
#include <tqpushbutton.h>
#include <tqlayout.h>

class Ui_Settings {
public:
    TQComboBox *MeasuringTime;
    TQComboBox *IntervalTime;

    TQComboBox *DefaultProfile_Test_1_Pattern, *DefaultProfile_Test_1_BlockSize, *DefaultProfile_Test_1_Queues, *DefaultProfile_Test_1_Threads;
    TQComboBox *DefaultProfile_Test_2_Pattern, *DefaultProfile_Test_2_BlockSize, *DefaultProfile_Test_2_Queues, *DefaultProfile_Test_2_Threads;
    TQComboBox *DefaultProfile_Test_3_Pattern, *DefaultProfile_Test_3_BlockSize, *DefaultProfile_Test_3_Queues, *DefaultProfile_Test_3_Threads;
    TQComboBox *DefaultProfile_Test_4_Pattern, *DefaultProfile_Test_4_BlockSize, *DefaultProfile_Test_4_Queues, *DefaultProfile_Test_4_Threads;

    TQComboBox *PeakPerformanceProfile_Test_1_Pattern, *PeakPerformanceProfile_Test_1_BlockSize, *PeakPerformanceProfile_Test_1_Queues, *PeakPerformanceProfile_Test_1_Threads;
    TQComboBox *PeakPerformanceProfile_Test_2_Pattern, *PeakPerformanceProfile_Test_2_BlockSize, *PeakPerformanceProfile_Test_2_Queues, *PeakPerformanceProfile_Test_2_Threads;

    TQComboBox *DemoProfile_Test_1_Pattern, *DemoProfile_Test_1_BlockSize, *DemoProfile_Test_1_Queues, *DemoProfile_Test_1_Threads;

    TQPushButton *buttonBox_Ok;
    TQPushButton *buttonBox_Restore;
    TQPushButton *buttonBox_NVMe;

    void setupUi(TQDialog *dialog) {
        MeasuringTime = new TQComboBox(dialog);
        IntervalTime = new TQComboBox(dialog);

        DefaultProfile_Test_1_Pattern = new TQComboBox(dialog);
        DefaultProfile_Test_1_BlockSize = new TQComboBox(dialog);
        DefaultProfile_Test_1_Queues = new TQComboBox(dialog);
        DefaultProfile_Test_1_Threads = new TQComboBox(dialog);

        DefaultProfile_Test_2_Pattern = new TQComboBox(dialog);
        DefaultProfile_Test_2_BlockSize = new TQComboBox(dialog);
        DefaultProfile_Test_2_Queues = new TQComboBox(dialog);
        DefaultProfile_Test_2_Threads = new TQComboBox(dialog);

        DefaultProfile_Test_3_Pattern = new TQComboBox(dialog);
        DefaultProfile_Test_3_BlockSize = new TQComboBox(dialog);
        DefaultProfile_Test_3_Queues = new TQComboBox(dialog);
        DefaultProfile_Test_3_Threads = new TQComboBox(dialog);

        DefaultProfile_Test_4_Pattern = new TQComboBox(dialog);
        DefaultProfile_Test_4_BlockSize = new TQComboBox(dialog);
        DefaultProfile_Test_4_Queues = new TQComboBox(dialog);
        DefaultProfile_Test_4_Threads = new TQComboBox(dialog);

        PeakPerformanceProfile_Test_1_Pattern = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_1_BlockSize = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_1_Queues = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_1_Threads = new TQComboBox(dialog);

        PeakPerformanceProfile_Test_2_Pattern = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_2_BlockSize = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_2_Queues = new TQComboBox(dialog);
        PeakPerformanceProfile_Test_2_Threads = new TQComboBox(dialog);

        DemoProfile_Test_1_Pattern = new TQComboBox(dialog);
        DemoProfile_Test_1_BlockSize = new TQComboBox(dialog);
        DemoProfile_Test_1_Queues = new TQComboBox(dialog);
        DemoProfile_Test_1_Threads = new TQComboBox(dialog);

        buttonBox_Ok = new TQPushButton("OK", dialog);
        buttonBox_Restore = new TQPushButton("Restore Defaults", dialog);
        buttonBox_NVMe = new TQPushButton("NVMe SSD", dialog);
    }
};
#endif
