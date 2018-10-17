#include "qmsg.h"

#include <string>

#include <QDebug>

QMsg::QMsg()
{

}

void QMsg::Cout(std::string msg)
{
    qDebug() << msg.c_str();
}
