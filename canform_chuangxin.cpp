#include "canform_chuangxin.h"
#include "ui_canform_chuangxin.h"

CanForm_ChuangXin::CanForm_ChuangXin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanForm_ChuangXin),
    can_func(nullptr)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
}

CanForm_ChuangXin::~CanForm_ChuangXin()
{
    delete ui;
}

CanFunc *CanForm_ChuangXin::getCan_func() const
{
    return can_func;
}

void CanForm_ChuangXin::on_start_button_clicked()
{

}

void CanForm_ChuangXin::on_cancel_button_clicked()
{
    this->close();
}
