#include <QMenu>
#include <QToolBar>
#include "MainWindow.h"
#include "BGFXWidget.h"

MainWindow::MainWindow()
{
    main_window.setupUi(this);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (first_show)
    {
        initializeBGFX();
        first_show = false;
    }
}

void MainWindow::initializeBGFX()
{
    int width = main_window.centralwidget->width();
    int height = main_window.centralwidget->height();
    void* native_window_handle = reinterpret_cast<void*>(main_window.centralwidget->winId());
    main_window.centralwidget->initializeBGFX(width, height, native_window_handle);
}
