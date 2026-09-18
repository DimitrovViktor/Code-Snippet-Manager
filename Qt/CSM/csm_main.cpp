#include "csm.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CSM window;
    window.show();
    return app.exec();
}
