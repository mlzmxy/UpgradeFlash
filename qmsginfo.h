#ifndef QMSGINFO_H
#define QMSGINFO_H

#include <QObject>

#include <string>
using std::string;

#include "upgrade_ctrl/message.h"

class QMsgInfo : public QObject, public Message
{
    Q_OBJECT
public:
    explicit QMsgInfo(QObject *parent = nullptr);
    virtual void Cout(string msg);
    virtual void ProgressValue(int progress_value);

signals:
    void TextAppend(QString msg);
    void ScrollValueChanged(int value);
};

#endif // QMSGINFO_H
