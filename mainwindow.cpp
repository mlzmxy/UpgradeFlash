#include <QString>
#include <QFileDialog>
#include <QLibrary>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    upgrade_work(new UpgradeWork),
    msg(new QMsgInfo)
{
    ui->setupUi(this);
    this->setWindowTitle("UpgradeFlash");
    ui->textBrowser->setFontFamily("微软雅黑");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

    upgrade_work->setQmsg(msg);

    connect(msg, QMsgInfo::TextAppend, ui->textBrowser, QTextBrowser::append);
    connect(msg, QMsgInfo::ScrollValueChanged, ui->progressBar, QProgressBar::setValue);
    connect(upgrade_work, UpgradeWork::ReturnResult, this, MainWindow::ShowResultMsg);
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

    upgrade_work->start();

    this->ui->select_button->setEnabled(false);
    this->ui->upgrade_button->setEnabled(false);
    this->ui->upgrade_local_button->setEnabled(false);
}


void MainWindow::ShowResultMsg(bool result)
{
    if(result) {
        QMessageBox::information(this, tr("信息"), tr("升级成功!"), QMessageBox::Cancel);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("升级失败!"), QMessageBox::Cancel);
    }
}

