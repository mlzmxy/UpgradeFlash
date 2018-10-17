#ifndef UPGRADEPROC_H
#define UPGRADEPROC_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "hexparsing.h"
#include "canfunc.h"
#include "message.h"

class UpgradeProc
{
public:
    UpgradeProc();
    UpgradeProc(unsigned short blockSize);
    UpgradeProc(Message* msg, unsigned short blockSize);
    ~UpgradeProc();

    bool Process();
    bool ParseHexFile();
    void SetHexParseSettings(string file,unsigned int origin_addr, unsigned int addr_len);

    string GetErrorMsg();

private:
    typedef enum {  //flash升级流程
        handshake = 0x10,
        unlockCSM = 0x11,
        toggle = 0x12,
        version = 0x13,
        erase = 0x14,
        dataBlockInfo = 0x15,
        flashData = 0x16,
        checkSum = 0x17,
        program = 0x18,
        verify = 0x19,
        resetDSP = 0x1A,
        receiveWait = 0x1B
    }Flow;

    HexParsing* hex_parsing;
    CanFunc* can_func;
    Message* message;

    vector<unsigned short> m_flash_data;
    unsigned short int m_datablock_size;
    unsigned int m_error_code;

    // can struct
    VCI_CAN_OBJ m_candata_struct;  //CAN数据结构
    vector<unsigned char> m_can_data;  //CAN数据

    bool CanSendData();
    bool CanSendFlashData();
    bool CanSendCmdData(Flow flow);
    bool CanReceiveData();
};

#define ERROR_CANFUNC    0x01         //CanFunc错误
#define ERROR_HEXPARSING 0x02         //HexParsing错误
#define ERROR_CAN_SEND_FAILED  0x03   //Can消息发送失败
#define ERROR_RECMD_DISLOCATION 0x04  //命令反馈错位

#endif // UPGRADEPROC_H
