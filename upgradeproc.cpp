#include <QTextBrowser>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "upgradeproc.h"
#include "crc16.h"


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
    unsigned int num_datablock_start = 0;  //数据块起始数据在vector中的位置
    unsigned int num_16bit_data = 0;  //flash数据计数
    unsigned int tmp_num_16bit_data = 0;  //备份
    unsigned short crc_value = 0;  //CRC16校验值
    int progress_bar_value = 0;
    Addr addr_origin = hex_parsing->GetOriginAddr();  //起始地址
    unsigned int addr_len = hex_parsing->GetAddrLength();  //地址长度
    Addr addr = addr_origin;  //数据地址
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
            this->ui->progressBar->setValue(++progress_bar_value);
            //发送握手指令
            can_data.at(0) = handshake;
            CanSendCmdData(handshake);
            check_flow = handshake;
            upgrade_flow = receiveWait;
            break;
        case unlockCSM:
            this->ui->textBrowser->append("等待CSM解锁-->");
            this->ui->progressBar->setValue(++progress_bar_value);
            //发送解锁指令
            can_data.at(0) = unlockCSM;
            CanSendCmdData(unlockCSM);
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case toggle:
            this->ui->textBrowser->append("等待toggle测试-->");
            this->ui->progressBar->setValue(++progress_bar_value);
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
            num_datablock_start = (data_block_num - 1) * data_block_size;
            //发送数据块信息
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
            tmp_num_16bit_data = num_16bit_data;
            can_data.at(0) = flashData;
            can_data.at(1) = 0;
            can_data.at(2) = data.at(tmp_num_16bit_data) & 0x00FF;
            can_data.at(3) = (data.at(tmp_num_16bit_data++) >> 8) & 0x00FF;
            can_data.at(4) = data.at(tmp_num_16bit_data) & 0x00FF;
            can_data.at(5) = (data.at(tmp_num_16bit_data++) >> 8) & 0x00FF;
            crc_value = CrcOf4Char(can_data.at(2), can_data.at(3), can_data.at(4), can_data.at(5));
            can_data.at(6) = crc_value & 0x00FF;
            can_data.at(7) = (crc_value >> 8) & 0x00FF;
            CanSendData();
            check_flow = flashData;
            upgrade_flow = receiveWait;
            break;
        case checkSum:
            this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据校验-->"));
            //发送校验值
            can_data.at(0) = checkSum;
            can_data.at(1) = 0;
            crc_value = CrcOfDataBlock(this->data, num_datablock_start, this->data_block_size);
            can_data.at(2) = crc_value & 0x00FF;
            can_data.at(3) = (crc_value >> 8) & 0x00FF;
            can_data.at(4) = 0;
            can_data.at(5) = 0;
            can_data.at(6) = 0;
            can_data.at(7) = 0;
            can_data.at(8) = 0;
            CanSendData();
            check_flow = checkSum;
            upgrade_flow = receiveWait;
            break;
        case program:
            this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据烧写-->"));
            //发送program指令
            can_data.at(0) = program;
            CanSendCmdData(program);
            check_flow = program;
            upgrade_flow = receiveWait;
            break;
        case verify:
            this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据烧写校验-->"));
            //发送校验指令
            can_data.at(0) = verify;
            CanSendCmdData(verify);
            check_flow = verify;
            upgrade_flow = receiveWait;
            break;
        case resetDSP:
            this->ui->textBrowser->append("发送重启DSP指令-->");
            //发送解锁指令
            can_data.at(0) = resetDSP;
            CanSendCmdData(resetDSP);
            check_flow = resetDSP;
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
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("握手失败");
                        return false;
                    }
                    upgrade_flow = unlockCSM;
                } else {
                    this->ui->textBrowser->append("握手命令回复错位 " + QString::number(check_flow));
                    return false;
                }
                break;
            case unlockCSM:
                if(check_flow == unlockCSM) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("CSM解锁成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("CSM解锁失败");
                        return false;
                    }
                    upgrade_flow = toggle;
                } else {
                    this->ui->textBrowser->append("解锁CSM命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case toggle:
                if(check_flow == toggle) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("Toggle测试成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("Toggle测试失败");
                        return false;
                    }
                    upgrade_flow = version;
                } else {
                    this->ui->textBrowser->append("Toggle测试命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case version:
                if(check_flow == version) {
                    if(0x210 == (can_data.at(1) + (can_data.at(2) >> 8))) {
                        this->ui->textBrowser->append("DSP Flash API版本正确: " + QString::number(0x210, 16));
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("DSP Flash API版本错误: " +
                                 QString::number((can_data.at(1) + (can_data.at(2) >> 8)), 16));
                        return false;
                    }
                    upgrade_flow = erase;
                } else {
                    this->ui->textBrowser->append("API版本命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case erase:
                if(check_flow == erase) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("Flash擦除成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("Flash擦除失败");
                        return false;
                    }
                    upgrade_flow = dataBlockInfo;
                } else {
                    this->ui->textBrowser->append("Flash擦除命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case dataBlockInfo:
                if(check_flow == dataBlockInfo) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("数据块信息反馈，起始地址对齐成功");
                    } else {
                        this->ui->textBrowser->append("数据块信息反馈，起始地址对齐错误");
                        return false;
                    }
                    upgrade_flow = flashData;
                } else {
                    this->ui->textBrowser->append("数据块信息命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case flashData:
                if(check_flow == flashData) {
                    if(0x55 == can_data.at(1)) {
                        num_16bit_data += 2;
                    } else {
                        this->ui->textBrowser->append(QString::number(addr.addr_32 + num_16bit_data, 16)
                                 + "," +QString::number(addr.addr_32 + num_16bit_data +1, 16)+ " 数据校验错误, 正在重新发送...");
                        //return false;
                    }
                    if((num_16bit_data - num_datablock_start) == data_block_size) {
                        this->ui->textBrowser->append("数据发送完成");
                        upgrade_flow = checkSum;
                    } else {
                        upgrade_flow = flashData;
                    }
                } else {
                    this->ui->textBrowser->append("数据信息回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case checkSum:
                if(check_flow == checkSum) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("校验成功");
                        upgrade_flow = program;
                    } else {
                        this->ui->textBrowser->append("校验失败，重新发送");
                        num_16bit_data = num_datablock_start;
                        upgrade_flow = dataBlockInfo;
                    }
                } else {
                    this->ui->textBrowser->append("块数据校验命令错位" + QString::number(check_flow));
                    return false;
                }
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

