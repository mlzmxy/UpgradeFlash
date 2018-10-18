#include "upgradework.h"

UpgradeWork::UpgradeWork(QObject *parent) :
    QThread(parent),
    upgrade_proc(new UpgradeProc()),
    qmsg(nullptr),
    file_path_name("")
{

}

UpgradeWork::~UpgradeWork()
{
}

void UpgradeWork::run()
{
    bool result = 0;
    UpgradeProc proc(qmsg, sector, 0x800);
    if(proc.ParseHexFile(this->file_path_name.toStdString(), origin_addr, addr_len)){
        if(proc.InitCAN()) {
            proc.WaitForUpgrade();
            result = proc.Process();
        }
    }
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

void UpgradeWork::setOrigin_addr(unsigned int value)
{
    origin_addr = value;
}

void UpgradeWork::setAddr_len(unsigned int value)
{
    addr_len = value;
}

void UpgradeWork::setSector(const UpgradeProc::EraseSector &value)
{
    sector = value;
}
