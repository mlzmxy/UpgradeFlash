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
    upgrade_proc(new UpgradeProc(ui)),
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
    upgrade_proc->SetHexParseSettings(this->file_path_name.toStdString(), 0x328000, 0x8000);
    if(upgrade_proc->ParseHexFile()) {
        ui->textBrowser->append(tr("Hex文件解析成功"));
        if(upgrade_proc->Process()) {
            QMessageBox::information(this, tr("信息"), tr("升级成功!"), QMessageBox::Cancel);
            ui->progressBar->setValue(100);
        } else {
            QMessageBox::warning(this, tr("错误"), tr("升级失败!"), QMessageBox::Cancel);
        }
    } else {
        QMessageBox::warning(this, tr("错误"), upgrade_proc->GetErrorMsg().c_str(), QMessageBox::Cancel);
    }
}
