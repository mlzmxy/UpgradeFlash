#include <QString>
#include <QFileDialog>
#include <QLibrary>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

extern unsigned int flag_dll_funcs;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    upgrade_proc(new UpgradeProc),
    file_path_name(""),
    flag_updating(1)
{
    ui->setupUi(this);
    this->setWindowTitle("Upgrade Flash");
    ui->textBrowser->setFontFamily("微软雅黑");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete upgrade_proc;
}

void MainWindow::SetTextInfo(QString info)
{
    ui->textBrowser->append(info);
}

void MainWindow::on_select_button_clicked()
{
    this->file_path_name = QFileDialog::getOpenFileName(this,tr("选择升级文件"), ".", tr("Hex (*.hex)"));
    ui->textBrowser->append(file_path_name);
}

void MainWindow::on_upgrade_button_clicked()
{
    if(flag_updating) {
        upgrade_proc->SetHexParseSettings(this->file_path_name.toStdString(), 0x328000, 0x8000);
        flag_updating = 0;
    } else {
        QMessageBox::information(this, tr("信息"), tr("正在升级..."), QMessageBox::Cancel);
    }
}
