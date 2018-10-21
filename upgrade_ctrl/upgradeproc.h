#ifndef UPGRADEPROC_H
#define UPGRADEPROC_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "hexparsing.h"
#include "usbcan/canfunc.h"
#include "message.h"


class UpgradeProc
{
public:
    enum EraseSector{
        AppProgram = 0,
        LocalProgram = 1
    };

    UpgradeProc();
    UpgradeProc(unsigned short blockSize);
    UpgradeProc(CanFunc* canFunc, Message* msg, EraseSector sector,unsigned short blockSize);
    ~UpgradeProc();

    bool InitCAN();
    bool ParseHexFile(string file,unsigned int origin_addr, unsigned int addr_len);
    void WaitForUpgrade();
    bool Process();

    void SetCanID(unsigned long canid_cmd, unsigned long canid_receive, unsigned long canid_data);

private:
    typedef enum {  //flash升级流程
        handshake = 0x10,
        unlockCSM = 0x11,
        version = 0x12,
        erase = 0x13,
        dataBlockInfo = 0x14,
        flashData = 0x15,
        checkSum = 0x16,
        program = 0x17,
        verify = 0x18,
        resetDSP = 0x19,
        receiveWait = 0x1A
    }Flow;

    HexParsing* hex_parsing;
    CanFunc* can_func;
    Message* message;

    EraseSector sectors;
    vector<unsigned short> m_flash_data;
    unsigned short m_datablock_size;
    unsigned int m_error_code;

    // can struct
    CanMsg m_candata;  //CAN消息
    vector<unsigned char> m_can_data;  //CAN数据

    unsigned long m_canid_cmd;
    unsigned long m_canid_receive;
    unsigned long m_canid_data;

    bool CanSendData();
    bool CanSendFlashData();
    bool CanSendCmdData(Flow flow);
    bool CanReceiveData();
};

#endif // UPGRADEPROC_H
