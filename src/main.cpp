#include "mainwindow.h"
#include <tdeapplication.h>
#include <tdeaboutdata.h>
#include <tdecmdlineargs.h>
#include "appsettings.h"

int main(int argc, char *argv[])
{
    TDEAboutData about("tdediskmark", "tdeDiskMark", "3.2.0");
    TDECmdLineArgs::init(argc, argv, &about);
    TDEApplication app;

    AppSettings().setupLocalization();

    MainWindow *w = new MainWindow();
    app.setMainWidget(w);
    w->setFixedSize(w->size());
    w->show();

    return app.exec();
}
