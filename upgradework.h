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

signals:
    void ReturnResult(bool result);

private:
    UpgradeProc* upgrade_proc;
    QMsgInfo* qmsg;
    QString file_path_name;
    UpgradeProc::EraseSector sector;
    unsigned int origin_addr;
    unsigned int addr_len;
};

#endif // UPGRADEWORK_H
