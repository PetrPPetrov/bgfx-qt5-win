#pragma once

#include <vector>
#include <QObject>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QTreeView>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void initializeBGFX();

private:
    bool first_show = true;
    Ui::MainWindow main_window;
};
