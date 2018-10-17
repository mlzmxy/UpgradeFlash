#ifndef QMSG_H
#define QMSG_H

#include "message.h"

class QMsg : public Message
{
public:
    QMsg();

    virtual void Cout(string msg);

};

#endif // QMSG_H
