#include "upgradework.h"
#include "upgrade_ctrl/qmsg.h"

UpgradeWork::UpgradeWork(QObject *parent) :
    QThread(parent),
    upgrade_proc(new UpgradeProc()),
    file_path_name("")
{

}

void UpgradeWork::run()
{
    UpgradeProc proc(new QMsg, 0x200);
    proc.SetHexParseSettings(this->file_path_name.toStdString(), 0x318000, 0x18000);
    if(proc.Process()) {

    }
}
