#ifndef UI_ABOUT_H
#define UI_ABOUT_H
#include <tqdialog.h>
#include <tqlabel.h>

class Ui_About {
public:
    TQLabel *label_Version;
    TQLabel *label_FIO;
    TQLabel *label_Icon;

    void setupUi(TQDialog *dialog) {
        if (dialog->name() == "About") dialog->setName("About");
        dialog->resize(400, 300);
        label_Version = new TQLabel(dialog);
        label_FIO = new TQLabel(dialog);
        label_Icon = new TQLabel(dialog);

    }
};
#endif
