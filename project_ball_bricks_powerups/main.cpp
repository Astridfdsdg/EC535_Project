#include <QApplication>
#include "GameWidget.h"
#include "MainMenu.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainMenu w;
    w.setWindowTitle("Brick Breaker");
    w.setCursor(Qt::BlankCursor);
    w.showFullScreen();           // fill the LCD
    return app.exec();
}
