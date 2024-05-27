#include <QApplication>
#include <QScreen>
#include <QStyle>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow main_window;

    // Center main window on desktop
    main_window.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            main_window.size(),
            application.primaryScreen()->availableGeometry()
        )
    );

    main_window.show();
    return application.exec();
}
