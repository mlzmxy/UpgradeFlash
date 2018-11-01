#ifndef CANFUNC_H
#define CANFUNC_H

#include <windows.h>

#include <string>
using std::string;

// Can消息结构体
typedef struct _CANMSG {
	unsigned long id;
	unsigned int datalen;
	unsigned char data[8];
}CanMsg, *PCanMsg;

class CanFunc
{
public:
	virtual ~CanFunc();
	virtual bool OpenAndInitDevice() = 0;
	virtual bool Transmit(PCanMsg data) = 0;
	virtual unsigned long GetReceiveNum() = 0;
	virtual bool ReceiveData(PCanMsg data) = 0;

	virtual string GetErrorMsg() = 0;
};

#endif // CANFUNC_H

