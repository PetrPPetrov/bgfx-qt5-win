#pragma once

#include <QWidget>

#include "ui_MainWindow.h"

class MainWindow : public QWidget, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();
};
