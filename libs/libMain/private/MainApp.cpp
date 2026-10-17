#include "libMain/public/MainApp.h"
#include "libMainView/public/MainView.h"
#include <QApplication>

int
MainApp::run(int& argc, char* argv[]) {
    QApplication app(argc, argv);
    MainView window;
    window.show();
    return app.exec();
}