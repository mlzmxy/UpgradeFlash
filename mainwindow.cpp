#include <QString>
#include <QFileDialog>
#include <QLibrary>
#include <QMessageBox>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "usbcan/canfunc_chuangxin.h"
#include "usbcan/canfunc_puchuang.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    upgrade_work(new UpgradeWork),
    msg(new QMsgInfo)
{
    ui->setupUi(this);
    this->setWindowTitle("UpgradeFlash for 二次配电 v0.12");
    ui->textBrowser->setFontFamily("微软雅黑");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

    //CAN设备 combo
    ui->comboBoxCan->addItem(tr("创芯CANalyst-II"));
    ui->comboBoxCan->addItem(tr("普创CANUSB-II"));

    ui->comboBox->addItem(tr("高压母线盒"));
    ui->comboBox->addItem(tr("低压母线盒"));
    ui->comboBox->addItem(tr("综合控制盒"));
    ui->comboBox->addItem(tr("直流配电终端1"));
    ui->comboBox->addItem(tr("直流配电终端2"));
    ui->comboBox->addItem(tr("直流配电终端3"));
    ui->comboBox->addItem(tr("直流配电终端4"));

    upgrade_work->setQmsg(msg);

    connect(msg, &QMsgInfo::TextAppend, ui->textBrowser, &QTextBrowser::append);
    connect(msg, &QMsgInfo::ScrollValueChanged, ui->progressBar, &QProgressBar::setValue);
    connect(upgrade_work, &UpgradeWork::ReturnResult, this, &MainWindow::ShowResultMsg);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete msg;
    upgrade_work->quit();
    upgrade_work->wait();
    delete upgrade_work;
}

void MainWindow::SetTextInfo(QString info)
{
    ui->textBrowser->append(info);
}

void MainWindow::on_select_button_clicked()
{
    QString file_path_name = QFileDialog::getOpenFileName(this,tr("选择升级文件"), ".", tr("Hex (*.hex)"));
    this->upgrade_work->SetFilePathName(file_path_name);
    ui->textBrowser->append(file_path_name);
}

void MainWindow::on_upgrade_button_clicked()
{
    upgrade_work->setSector(UpgradeProc::AppProgram);
    upgrade_work->setOrigin_addr(0x318000);
    upgrade_work->setAddr_len(0x18000);

    switch (ui->comboBoxCan->currentIndex()) {
    case 0:
        upgrade_work->setCan_func(new CanFunc_ChuangXin);
        break;
    case 1:
        upgrade_work->setCan_func(new CanFunc_PuChuang);
        break;
    }
/*
    switch (ui->comboBox->currentIndex()) {
    case 0:
        ui->textBrowser->append("高压母线盒应用程序升级");
        upgrade_work->SetCANID(0x01A2A100, 0x01A2A101, 0x01A2A104);
        break;
    case 1:
        ui->textBrowser->append("低压母线盒应用程序升级");
        upgrade_work->SetCANID(0x01A2A300, 0x01A2A301, 0x01A2A304);
        break;
    case 2:
        ui->textBrowser->append("综合控制盒应用程序升级");
        upgrade_work->SetCANID(0x01A2A000, 0x01A2A001, 0x01A2A004);
        break;
    case 3:
        ui->textBrowser->append("直流配电终端1应用程序升级");
        upgrade_work->SetCANID(0x01A2A510, 0x01A2A511, 0x01A2A514);
        break;
    case 4:
        ui->textBrowser->append("直流配电终端2应用程序升级");
        upgrade_work->SetCANID(0x01A2A520, 0x01A2A521, 0x01A2A524);
        break;
    case 5:
        ui->textBrowser->append("直流配电终端3应用程序升级");
        upgrade_work->SetCANID(0x01A2A530, 0x01A2A531, 0x01A2A534);
        break;
    case 6:
        ui->textBrowser->append("直流配电终端4应用程序升级");
        upgrade_work->SetCANID(0x01A2A540, 0x01A2A541, 0x01A2A544);
        break;
    default:
        break;
    }
*/
    upgrade_work->SetCANID(0x01A2A3A0, 0x01A2A3A1, 0x01A2A3A4);  //测试
    upgrade_work->start();

    this->ui->select_button->setEnabled(false);
    this->ui->upgrade_button->setEnabled(false);
    this->ui->upgrade_local_button->setEnabled(false);
}

void MainWindow::on_upgrade_local_button_clicked()
{
    upgrade_work->setSector(UpgradeProc::LocalProgram);
    upgrade_work->setOrigin_addr(0x338000);
    upgrade_work->setAddr_len(0x8000);

    switch (ui->comboBoxCan->currentIndex()) {
    case 0:
        upgrade_work->setCan_func(new CanFunc_ChuangXin);
        break;
    case 1:
        upgrade_work->setCan_func(new CanFunc_PuChuang);
        break;
    }
/*
    switch (ui->comboBox->currentIndex()) {
    case 0:
        ui->textBrowser->append("高压母线盒底层程序升级");
        upgrade_work->SetCANID(0x01A2A100, 0x01A2A101, 0x01A2A104);
        break;
    case 1:
        ui->textBrowser->append("低压母线盒底层程序升级");
        upgrade_work->SetCANID(0x01A2A300, 0x01A2A301, 0x01A2A304);
        break;
    case 2:
        ui->textBrowser->append("综合控制盒底层程序升级");
        upgrade_work->SetCANID(0x01A2A000, 0x01A2A001, 0x01A2A004);
        break;
    case 3:
        ui->textBrowser->append("直流配电终端1底层程序升级");
        upgrade_work->SetCANID(0x01A2A510, 0x01A2A511, 0x01A2A514);
        break;
    case 4:
        ui->textBrowser->append("直流配电终端2底层程序升级");
        upgrade_work->SetCANID(0x01A2A520, 0x01A2A521, 0x01A2A524);
        break;
    case 5:
        ui->textBrowser->append("直流配电终端3底层程序升级");
        upgrade_work->SetCANID(0x01A2A530, 0x01A2A531, 0x01A2A534);
        break;
    case 6:
        ui->textBrowser->append("直流配电终端4底层程序升级");
        upgrade_work->SetCANID(0x01A2A540, 0x01A2A541, 0x01A2A544);
        break;
    default:
        break;
    }
*/
    upgrade_work->SetCANID(0x01A2A3A0, 0x01A2A3A1, 0x01A2A3A4);  //测试
    upgrade_work->start();

    this->ui->select_button->setEnabled(false);
    this->ui->upgrade_button->setEnabled(false);
    this->ui->upgrade_local_button->setEnabled(false);
}


void MainWindow::ShowResultMsg(bool result)
{
    if(result) {
        QMessageBox::information(this, tr("信息"), tr("升级成功!"), QMessageBox::Yes);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("升级失败!"), QMessageBox::Cancel);
    }
}


