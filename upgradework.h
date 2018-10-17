#ifndef UPGRADEWORK_H
#define UPGRADEWORK_H

#include <QThread>
#include <QString>

#include "upgrade_ctrl/upgradeproc.h"

class UpgradeWork : public QThread
{
    Q_OBJECT
public:
    explicit UpgradeWork(QObject *parent = nullptr);
    virtual void run();

signals:
    void TextAppend(QString msg);
    void ScrollValueChanged(int value);
    void ReturnResult(bool tf);

private:
    UpgradeProc* upgrade_proc;
    QString file_path_name;
};

#endif // UPGRADEWORK_H
