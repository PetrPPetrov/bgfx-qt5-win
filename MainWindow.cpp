#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QTextEdit>
#include "MainWindow.h"
#include "BGFXWidget.h"

MainWindow::MainWindow()
{
    main_window.setupUi(this);

    QDockWidget* dock = new QDockWidget(tr("Model Tree"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    QTextEdit* internal_widget = new QTextEdit(dock);
    internal_widget->setText("Node name");
    dock->setWidget(internal_widget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
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
