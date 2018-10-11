#ifndef UPGRADEPROC_H
#define UPGRADEPROC_H

#include <QMainWindow>

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "hexparsing.h"
#include "canfunc.h"

namespace Ui {
class MainWindow;
}

class UpgradeProc
{
public:
    UpgradeProc();
    UpgradeProc(Ui::MainWindow *main_ui);
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
        dataBlockSize = 0x15,
        flashData = 0x16,
        checkSum = 0x17,
        program = 0x18,
        verify = 0x19,
        resetDSP = 0x1A,
        receiveWait = 0x1B
    }Flow;

    Ui::MainWindow *ui;

    vector<unsigned short int> data;
    HexParsing* hex_parsing;
    CanFunc* can_func;
    unsigned int error_code;

    // can struct
    VCI_CAN_OBJ can_data_struct;  //CAN数据结构
    vector<unsigned char> can_data;  //CAN数据

    bool CanSendData();
    bool CanSendCmdData(Flow flow);
    bool CanReceiveData();
};

#define ERROR_CANFUNC    0x01         //CanFunc错误
#define ERROR_HEXPARSING 0x02         //HexParsing错误
#define ERROR_CAN_SEND_FAILED  0x03   //Can消息发送失败
#define ERROR_RECMD_DISLOCATION 0x04  //命令反馈错位

#endif // UPGRADEPROC_H
