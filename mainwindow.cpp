#include <QString>
#include <QFileDialog>
#include <QLibrary>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("UpgradeFlash");
    ui->textBrowser->setFontFamily("微软雅黑");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTextInfo(QString info)
{
    ui->textBrowser->append(info);
}

void MainWindow::on_select_button_clicked()
{
    QString file_path_name = QFileDialog::getOpenFileName(this,tr("选择升级文件"), ".", tr("Hex (*.hex)"));
    ui->textBrowser->append(file_path_name);
}

void MainWindow::on_upgrade_button_clicked()
{
//    UpgradeProc proc(new QMsg, 0x200);
//    proc.SetHexParseSettings(this->file_path_name.toStdString(), 0x318000, 0x18000);
//    if(proc.Process()) {
//        QMessageBox::information(this, tr("信息"), tr("升级成功!"), QMessageBox::Cancel);
//    } else {
//        QMessageBox::warning(this, tr("错误"), upgrade_proc->GetErrorMsg().c_str(), QMessageBox::Cancel);
//    }

    this->ui->select_button->setEnabled(false);
    this->ui->upgrade_button->setEnabled(false);
}
