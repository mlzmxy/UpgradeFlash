#include <QTextBrowser>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "upgradeproc.h"


UpgradeProc::UpgradeProc() :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    data_block_size(0x100),
    error_code(0)
{
}

UpgradeProc::UpgradeProc(Ui::MainWindow *main_ui) :
    ui(main_ui),
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    data_block_size(0x100),
    error_code(0)
{
    can_data.resize(8);
}

UpgradeProc::~UpgradeProc()
{

}


bool UpgradeProc::Process()
{
    unsigned int data_block_num = 1;  //发送数据块计数
    Addr addr = hex_parsing->GetOriginAddr();  //数据地址
    Addr addr_temp;  //备份地址
    Flow upgrade_flow = handshake;
    Flow check_flow = handshake;
    error_code = 0;

    if(can_func->IsInitialized()) {
        this->ui->textBrowser->append("CAN设备初始化完成");
        //this->ui->textBrowser->moveCursor(QTextCursor::End);
    } else {
        //error_code = ERROR_CANFUNC;
        this->ui->textBrowser->append("CAN设备初始化失败");
        return false;
    }



    while(1) {
        switch (upgrade_flow) {
        case handshake:
            this->ui->textBrowser->append("开始升级...");
            this->ui->textBrowser->append("等待握手-->");
            this->ui->progressBar->setValue(1);
            //发送握手指令
            can_data.at(0) = handshake;
            CanSendCmdData(handshake);
            check_flow = handshake;
            upgrade_flow = receiveWait;
            break;
        case unlockCSM:
            this->ui->textBrowser->append("等待CSM解锁-->");
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case toggle:
            this->ui->textBrowser->append("等待toggle测试-->");
            //发送toggle指令
            can_data.at(0) = toggle;
            CanSendCmdData(toggle);
            check_flow = toggle;
            upgrade_flow = receiveWait;
            break;
        case version:
            this->ui->textBrowser->append("检查Flash API版本-->");
            //发送version指令
            can_data.at(0) = version;
            CanSendCmdData(version);
            check_flow = version;
            upgrade_flow = receiveWait;
            break;
        case erase:
            this->ui->textBrowser->append("等待Flash擦除-->");
            //发送擦除指令
            can_data.at(0) = erase;
            CanSendCmdData(erase);
            check_flow = erase;
            upgrade_flow = receiveWait;
            break;
        case dataBlockInfo:
            this->ui->textBrowser->append(QString("发送第" + QString::number(data_block_num) +"块数据信息-->"));
            //发送解锁指令
            can_data.at(0) = dataBlockInfo;
            can_data.at(1) = 0;
            can_data.at(2) = addr.addr_16.addr_l & 0x00FF;          //地址位 7-0
            can_data.at(3) = (addr.addr_16.addr_l >> 8) & 0x00FF;   //地址位 15-8
            can_data.at(4) = addr.addr_16.addr_h & 0x00FF;          //地址位 23-16
            can_data.at(5) = (addr.addr_16.addr_h >> 8) & 0x00FF;   //地址位 31-24
            can_data.at(6) = this->data_block_size & 0x00FF;        //数据块大小低位
            can_data.at(7) = (this->data_block_size >> 8) & 0x00FF; //数据块大小高位
            CanSendData();
            check_flow = dataBlockInfo;
            upgrade_flow = receiveWait;
            break;
        case flashData:
            //发送数据
            can_data.at(0) = flashData;
            can_data.at(1) = 0;
            can_data.at(2) = flashData;
            can_data.at(3) = flashData;
            can_data.at(4) = flashData;
            can_data.at(5) = flashData;
            can_data.at(6) = flashData;
            can_data.at(7) = flashData;
            CanSendData();
            check_flow = flashData;
            upgrade_flow = receiveWait;
            break;
        case checkSum:
            this->ui->textBrowser->append("等待CSM解锁-->");
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case program:
            this->ui->textBrowser->append("等待CSM解锁-->");
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case verify:
            this->ui->textBrowser->append("等待CSM解锁-->");
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case resetDSP:
            this->ui->textBrowser->append("等待CSM解锁-->");
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case receiveWait:
            break;
        }

        if(CanReceiveData()) {
            switch(can_data.at(0)) {
            case handshake:
                if(check_flow == handshake) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("握手成功");
                        this->ui->progressBar->setValue(2);
                    } else {
                        this->ui->textBrowser->append("握手失败");
                        return false;
                    }
                    upgrade_flow = unlockCSM;
                } else {
                    //error_code = ERROR_RECMD_DISLOCATION;
                    //upgrade_flow = updateFailed;
                    this->ui->textBrowser->append("命令回复错位");
                    return false;
                }
                break;
            case unlockCSM:
                break;
            case toggle:
                break;
            case version:
                break;
            case erase:
                break;
            case dataBlockInfo:
                break;
            case flashData:
                break;
            case checkSum:
                break;
            case program:
                break;
            case verify:
                break;
            case resetDSP:
                break;
            case receiveWait:
                break;
            }
        }
    }
}

bool UpgradeProc::ParseHexFile()
{
    if(hex_parsing->Convert()) {
        this->data = hex_parsing->GetDataMap();
    } else {
        error_code = ERROR_HEXPARSING;
        return false;
    }
    return true;
}

void UpgradeProc::SetHexParseSettings(std::string file,
                                      unsigned int origin_addr, unsigned int addr_len)
{
    hex_parsing->SetFilePathName(file);
    hex_parsing->SetOrigin_Address(origin_addr);
    hex_parsing->SetAddrLength(addr_len);
    hex_parsing->Prepare();
}

bool UpgradeProc::CanSendData()
{
    can_data_struct.ID = 0x01A2A3A0;
    can_data_struct.SendType = 0;
    can_data_struct.RemoteFlag = 0;
    can_data_struct.ExternFlag = 1;
    can_data_struct.DataLen = 8;
    can_data_struct.Data[0] = can_data.at(0);
    can_data_struct.Data[1] = can_data.at(1);
    can_data_struct.Data[2] = can_data.at(2);
    can_data_struct.Data[3] = can_data.at(3);
    can_data_struct.Data[4] = can_data.at(4);
    can_data_struct.Data[5] = can_data.at(5);
    can_data_struct.Data[6] = can_data.at(6);
    can_data_struct.Data[7] = can_data.at(7);

    if(can_func->Transmit(Dev_Index, Can_Index_1, &can_data_struct)) {
        return true;
    } else {
        error_code = ERROR_CAN_SEND_FAILED;
        return false;
    }
}

bool UpgradeProc::CanSendCmdData(Flow flow)
{
    can_data.at(0) = flow;
    can_data.at(1) = 0;
    can_data.at(2) = 0;
    can_data.at(3) = 0;
    can_data.at(4) = 0;
    can_data.at(5) = 0;
    can_data.at(6) = 0;
    can_data.at(7) = 0;

    if(CanSendData())
        return true;
    return false;
}

bool UpgradeProc::CanReceiveData()
{
    if(can_func->GetReceiveNum(Dev_Index, Can_Index_1)) {
        if(can_func->Receive(Dev_Index, Can_Index_1, &can_data_struct, 1, 5)) {
            if((0x01A2A3A1 == can_data_struct.ID) && (8 == can_data_struct.DataLen)) {
                can_data.at(0) = can_data_struct.Data[0];
                can_data.at(1) = can_data_struct.Data[1];
                can_data.at(2) = can_data_struct.Data[2];
                can_data.at(3) = can_data_struct.Data[3];
                can_data.at(4) = can_data_struct.Data[4];
                can_data.at(5) = can_data_struct.Data[5];
                can_data.at(6) = can_data_struct.Data[6];
                can_data.at(7) = can_data_struct.Data[7];
                return true;
            }
        }
    }
    return false;
}

string UpgradeProc::GetErrorMsg()
{
    string msg = "";
    switch(error_code)
    {
    case ERROR_CANFUNC:
        msg = can_func->GetErrorMsg();
        break;
    case ERROR_HEXPARSING:
        msg = hex_parsing->GetErrorMsg();
        break;
    }
    return msg;
}

