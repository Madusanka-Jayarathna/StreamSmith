#include "libMain/public/MainApp.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    // 1. QApplication MUST be constructed first
    QApplication app(argc, argv);

    // 2. Now it is safe to construct QWidget / QMainWindow classes
    MainApp window;
    window.show();

    // 3. Start the Qt event loop
    return app.exec();
}
