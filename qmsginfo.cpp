#include "qmsginfo.h"


QMsgInfo::QMsgInfo(QObject *parent) :
    QObject(parent)
{

}

void QMsgInfo::Cout(std::string msg)
{
    emit TextAppend(QString::fromStdString(msg));
}

void QMsgInfo::ProgressValue(int progress_value)
{
    emit ScrollValueChanged(progress_value);
}

