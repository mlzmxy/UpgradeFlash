#include "upgradework.h"

UpgradeWork::UpgradeWork(QObject *parent) :
    QThread(parent),
    upgrade_proc(new UpgradeProc()),
    file_path_name(""),
    qmsg(nullptr)
{

}

void UpgradeWork::run()
{
    UpgradeProc proc(qmsg, 0x200);
    proc.SetHexParseSettings(this->file_path_name.toStdString(), 0x318000, 0x18000);
    bool result = proc.Process();
    emit ReturnResult(result);
}

void UpgradeWork::SetFilePathName(QString file_path)
{
    this->file_path_name = file_path;
}

void UpgradeWork::setQmsg(QMsgInfo *value)
{
    qmsg = value;
}
