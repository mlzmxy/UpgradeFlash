#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "upgradework.h"
#include "qmsginfo.h"
#include "usbcan/canfunc.h"

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
    void on_upgrade_local_button_clicked();

    void ShowResultMsg(bool result);

private:
    Ui::MainWindow *ui;

    UpgradeWork* upgrade_work;
    QMsgInfo* msg;
    CanFunc *can_func;
};

#endif // MAINWINDOW_H
