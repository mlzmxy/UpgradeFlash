#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "upgradeproc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetTextInfo(QString info);

private slots:
    void on_select_button_clicked();

    void on_upgrade_button_clicked();

private:
    Ui::MainWindow *ui;

    UpgradeProc* upgrade_proc;
    QString file_path_name;
    unsigned int flag_updating;
};

#endif // MAINWINDOW_H
