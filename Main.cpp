#include <QDesktopWidget>
#include <QSplashScreen>
#include <QApplication>
#include <QStringList>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include <QDebug>
#include <QtGlobal>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow main_window;

    // Center main window on desktop
    QDesktopWidget screen;
    QRect screen_rect = screen.screenGeometry(&main_window);
    QPoint position((screen_rect.width() - main_window.width()) / 2, (screen_rect.height() - main_window.height()) / 2);
    main_window.move(position);
    main_window.show();
    return application.exec();
}
