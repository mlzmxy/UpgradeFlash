#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "canform_chuangxin.h"

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

private slots:
    void on_select_button_clicked();
    void on_upgrade_button_clicked();
    void on_upgrade_local_button_clicked();

    void ShowResultMsg(bool result);
    void OpenCanDevice_ChuangXin();
    void OpenCanDevice_PuChuang();
    void ResultOfOpenCanDevice(CanFunc* can);

private:
    Ui::MainWindow *ui;
    CanForm_ChuangXin *can_form_chuangxin;

    UpgradeWork* upgrade_work;
    CanFunc* can_func;
    QMsgInfo* msg;

    //menu actions
    QAction *menu_can_chuangxin;
    QAction *menu_can_puchuang;
};

#endif // MAINWINDOW_H
