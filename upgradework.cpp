#include "upgradework.h"

#include "usbcan/canfunc_chuangxin.h"

UpgradeWork::UpgradeWork(QObject *parent) :
    QThread(parent),
    upgrade_proc(new UpgradeProc),
    can_func(nullptr),
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
    UpgradeProc proc(can_func, qmsg, sector, 0x800);
    proc.SetCanID(m_canid_cmd, m_canid_receive, m_canid_data);
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

void UpgradeWork::SetCANID(unsigned long canid_cmd, unsigned long canid_receive, unsigned long canid_data)
{
    m_canid_cmd = canid_cmd;
    m_canid_receive = canid_receive;
    m_canid_data = canid_data;
}

void UpgradeWork::setCan_func(CanFunc *value)
{
    can_func = value;
}
