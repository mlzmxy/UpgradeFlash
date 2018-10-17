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
    virtual void run();

    void SetFilePathName(QString file_path);

    void setQmsg(QMsgInfo *value);

signals:
    void ReturnResult(bool result);

private:
    UpgradeProc* upgrade_proc;
    QString file_path_name;
    QMsgInfo* qmsg;
};

#endif // UPGRADEWORK_H
