#ifndef UPGRADEWORK_H
#define UPGRADEWORK_H

#include <QThread>
#include <QString>

#include "qmsginfo.h"
#include "upgrade_ctrl/upgradeproc.h"

class UpgradeWork : public QThread
{
    Q_OBJECT
public:
    explicit UpgradeWork(QObject *parent = nullptr);
    ~UpgradeWork();
    virtual void run();

    void SetFilePathName(QString file_path);

    void setQmsg(QMsgInfo *value);

    void setOrigin_addr(unsigned int value);

    void setAddr_len(unsigned int value);

    void setSector(const UpgradeProc::EraseSector &value);

    void SetCANID(unsigned long canid_cmd, unsigned long canid_receive, unsigned long canid_data);

signals:
    void ReturnResult(bool result);

private:
    UpgradeProc* upgrade_proc;
    QMsgInfo* qmsg;
    QString file_path_name;
    UpgradeProc::EraseSector sector;
    unsigned int origin_addr;
    unsigned int addr_len;

    unsigned long m_canid_cmd;
    unsigned long m_canid_receive;
    unsigned long m_canid_data;
};

#endif // UPGRADEWORK_H
