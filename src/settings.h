#ifndef SETTINGS_H
#define SETTINGS_H

#include <tqdialog.h>
#include <tqcombobox.h>

class AppSettings;

class Settings : public TQDialog
{
    TQ_OBJECT

public:
    explicit Settings(TQWidget *parent = nullptr);
    ~Settings();

private slots:
    void on_buttonBox_clicked();
    void on_buttonBox_restore();

private:
    void findDataAndSet(TQComboBox* comboBox, int data);
    void populateComboBoxes();

    // Default Profile
    TQComboBox *DefaultProfile_Test_1_Pattern;
    TQComboBox *DefaultProfile_Test_1_BlockSize;
    TQComboBox *DefaultProfile_Test_1_Queues;
    TQComboBox *DefaultProfile_Test_1_Threads;
    
    TQComboBox *DefaultProfile_Test_2_Pattern;
    TQComboBox *DefaultProfile_Test_2_BlockSize;
    TQComboBox *DefaultProfile_Test_2_Queues;
    TQComboBox *DefaultProfile_Test_2_Threads;
    
    TQComboBox *DefaultProfile_Test_3_Pattern;
    TQComboBox *DefaultProfile_Test_3_BlockSize;
    TQComboBox *DefaultProfile_Test_3_Queues;
    TQComboBox *DefaultProfile_Test_3_Threads;
    
    TQComboBox *DefaultProfile_Test_4_Pattern;
    TQComboBox *DefaultProfile_Test_4_BlockSize;
    TQComboBox *DefaultProfile_Test_4_Queues;
    TQComboBox *DefaultProfile_Test_4_Threads;
    
    // Peak Profile
    TQComboBox *PeakPerformanceProfile_Test_1_Pattern;
    TQComboBox *PeakPerformanceProfile_Test_1_BlockSize;
    TQComboBox *PeakPerformanceProfile_Test_1_Queues;
    TQComboBox *PeakPerformanceProfile_Test_1_Threads;

    TQComboBox *PeakPerformanceProfile_Test_2_Pattern;
    TQComboBox *PeakPerformanceProfile_Test_2_BlockSize;
    TQComboBox *PeakPerformanceProfile_Test_2_Queues;
    TQComboBox *PeakPerformanceProfile_Test_2_Threads;

    // Demo Profile
    TQComboBox *DemoProfile_Test_1_Pattern;
    TQComboBox *DemoProfile_Test_1_BlockSize;
    TQComboBox *DemoProfile_Test_1_Queues;
    TQComboBox *DemoProfile_Test_1_Threads;

    // Time
    TQComboBox *MeasuringTime;
    TQComboBox *IntervalTime;
};

#endif // SETTINGS_H
