#ifndef CANFORM_CHUANGXIN_H
#define CANFORM_CHUANGXIN_H

#include <QWidget>

#include "usbcan/canfunc_chuangxin.h"

namespace Ui {
class CanForm_ChuangXin;
}

class CanForm_ChuangXin : public QWidget
{
    Q_OBJECT

public:
    explicit CanForm_ChuangXin(QWidget *parent = nullptr);
    ~CanForm_ChuangXin();


signals:
    void ReturnOpenCanResult(CanFunc *can_func);

private slots:
    void on_start_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::CanForm_ChuangXin *ui;

    CanFunc_ChuangXin *can_func_cx;
};

#endif // CANFORM_CHUANGXIN_H