#include "canform_chuangxin.h"
#include "ui_canform_chuangxin.h"

#include <QMessageBox>

CanForm_ChuangXin::CanForm_ChuangXin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanForm_ChuangXin),
    can_func_cx(new CanFunc_ChuangXin)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    ui->combo_dev->addItem(tr("0"));
    ui->combo_dev->addItem(tr("1"));

    ui->combo_ind->addItem(tr("0"));
    ui->combo_ind->addItem(tr("1"));

    ui->combo_Timing->addItem(tr("1000K"));
    ui->combo_Timing->addItem(tr("800K"));
    ui->combo_Timing->addItem(tr("500K"));
    ui->combo_Timing->addItem(tr("250K"));
    ui->combo_Timing->addItem(tr("125K"));
    ui->combo_Timing->addItem(tr("100K"));
    ui->combo_Timing->addItem(tr("50K"));
    ui->combo_Timing->addItem(tr("20K"));
    ui->combo_Timing->addItem(tr("10K"));
}

CanForm_ChuangXin::~CanForm_ChuangXin()
{
    delete ui;
    delete can_func_cx;
}

void CanForm_ChuangXin::on_start_button_clicked()
{
    unsigned long dev = ui->combo_dev->currentIndex();
    unsigned long ind = ui->combo_ind->currentIndex();
    CanFunc_ChuangXin::TIMING timing = CanFunc_ChuangXin::TIMING_250K;
    switch (ui->combo_Timing->currentIndex()) {
    case 0:
        timing = CanFunc_ChuangXin::TIMING_1000K;
        break;
    case 1:
        timing = CanFunc_ChuangXin::TIMING_800K;
        break;
    case 2:
        timing = CanFunc_ChuangXin::TIMING_500K;
        break;
    case 3:
        timing = CanFunc_ChuangXin::TIMING_250K;
        break;
    case 4:
        timing = CanFunc_ChuangXin::TIMING_125K;
        break;
    case 5:
        timing = CanFunc_ChuangXin::TIMING_100K;
        break;
    case 6:
        timing = CanFunc_ChuangXin::TIMING_50K;
        break;
    case 7:
        timing = CanFunc_ChuangXin::TIMING_20K;
        break;
    case 8:
        timing = CanFunc_ChuangXin::TIMING_10K;
        break;
    default:
        break;
    }
    can_func_cx->SetCanConfig(dev, ind, timing);
    if(can_func_cx->OpenAndInitDevice()) {
        emit ReturnOpenCanResult(can_func_cx);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("设备打开失败!"), QMessageBox::Cancel);
    }

    this->hide();
}

void CanForm_ChuangXin::on_cancel_button_clicked()
{
    this->close();
}
