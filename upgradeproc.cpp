#include <QTextBrowser>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "upgradeproc.h"

UpgradeProc::UpgradeProc() :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    error_code(0)
{
}

UpgradeProc::UpgradeProc(Ui::MainWindow *main_ui) :
    ui(main_ui),
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    error_code(0)
{
    can_data.resize(8);
}

UpgradeProc::~UpgradeProc()
{

}


bool UpgradeProc::Process()
{
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
            this->ui->textBrowser->append("发送握手指令-->");
            this->ui->progressBar->setValue(1);
            //发送握手指令
            can_data.at(0) = handshake;
            CanSendCmdData(handshake);
            check_flow = handshake;
            upgrade_flow = receiveWait;
            break;
        case unlockCSM:
            break;
        case toggle:
            break;
        case version:
            break;
        case erase:
            break;
        case dataBlockSize:
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
            case dataBlockSize:
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
    can_data_struct.Data[0] = data.at(0);
    can_data_struct.Data[1] = data.at(1);
    can_data_struct.Data[2] = data.at(2);
    can_data_struct.Data[3] = data.at(3);
    can_data_struct.Data[4] = data.at(4);
    can_data_struct.Data[5] = data.at(5);
    can_data_struct.Data[6] = data.at(6);
    can_data_struct.Data[7] = data.at(7);

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

