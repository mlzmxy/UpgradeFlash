#include <QTextBrowser>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "upgradeproc.h"
#include "crc16.h"


UpgradeProc::UpgradeProc() :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    data_block_size(0x200),
    error_code(0)
{
}

UpgradeProc::UpgradeProc(Ui::MainWindow *main_ui) :
    ui(main_ui),
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    data_block_size(0x200),
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
    unsigned short crc_value = 0;  //CRC16校验值
    int progress_bar_value = 0;  //进度条数值
    int bar_value_added = (hex_parsing->GetAddrLength() / this->data_block_size) / 90;
    Addr addr_origin = hex_parsing->GetOriginAddr();  //起始地址
    Addr addr_end = hex_parsing->GetEndAddr();  //地址长度
    Addr addr = addr_origin;  //数据地址
    Flow upgrade_flow = handshake;
    Flow check_flow = handshake;
    error_code = 0;

    if(can_func->IsInitialized()) {
        this->ui->textBrowser->append("CAN设备初始化完成");
        //this->ui->textBrowser->moveCursor(QTextCursor::End);
    } else {
        this->ui->textBrowser->append(can_func->GetErrorMsg().c_str());
        return false;
    }

    while(1) {
        switch (upgrade_flow) {
        case handshake:
            this->ui->textBrowser->append("开始升级...");
            this->ui->textBrowser->append("等待握手-->");
            this->ui->progressBar->setValue(++progress_bar_value);
            //发送握手指令
            if(!CanSendCmdData(handshake)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            check_flow = handshake;
            upgrade_flow = receiveWait;
            break;
        case unlockCSM:
            //发送解锁指令
            if(!CanSendCmdData(unlockCSM)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append("等待CSM解锁-->");
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case toggle:
            //发送toggle指令
            if(!CanSendCmdData(toggle)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append("等待toggle测试-->");
            check_flow = toggle;
            upgrade_flow = receiveWait;
            break;
        case version:
            //发送version指令
            if(!CanSendCmdData(version)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append("检查Flash API版本-->");
            check_flow = version;
            upgrade_flow = receiveWait;
            break;
        case erase:
            //发送擦除指令
            if(!CanSendCmdData(erase)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append("等待Flash擦除-->");
            check_flow = erase;
            upgrade_flow = receiveWait;
            break;
        case dataBlockInfo:
            num_datablock_start = (data_block_num - 1) * this->data_block_size;
            num_16bit_data = num_datablock_start;
            addr.addr_32 = addr_origin.addr_32 + (data_block_num - 1) * this->data_block_size;
            //发送数据块信息
            can_data.at(0) = dataBlockInfo;
            can_data.at(1) = 0;
            can_data.at(2) = addr.addr_16.addr_l & 0x00FF;          //地址位 7-0
            can_data.at(3) = (addr.addr_16.addr_l >> 8) & 0x00FF;   //地址位 15-8
            can_data.at(4) = addr.addr_16.addr_h & 0x00FF;          //地址位 23-16
            can_data.at(5) = (addr.addr_16.addr_h >> 8) & 0x00FF;   //地址位 31-24
            can_data.at(6) = this->data_block_size & 0x00FF;        //数据块大小低位
            can_data.at(7) = (this->data_block_size >> 8) & 0x00FF; //数据块大小高位
            if(!CanSendData()) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append(QString::number(this->data_block_size));
            this->ui->textBrowser->append("发送第" + QString::number(data_block_num) +"块数据信息-->");
            check_flow = dataBlockInfo;
            upgrade_flow = receiveWait;
            break;
        case flashData:
            //发送数据
            this->ui->textBrowser->append("  正在发送数据...");
            while ((num_16bit_data - num_datablock_start) < this->data_block_size) {
                if((num_16bit_data + 3) < data.capacity()) {
                    can_data.at(0) = data.at(num_16bit_data) & 0x00FF;
                    can_data.at(1) = (data.at(num_16bit_data++) >> 8) & 0x00FF;
                    can_data.at(2) = data.at(num_16bit_data) & 0x00FF;
                    can_data.at(3) = (data.at(num_16bit_data++) >> 8) & 0x00FF;
                    can_data.at(4) = data.at(num_16bit_data) & 0x00FF;
                    can_data.at(5) = (data.at(num_16bit_data++) >> 8) & 0x00FF;
                    can_data.at(6) = data.at(num_16bit_data) & 0x00FF;
                    can_data.at(7) = (data.at(num_16bit_data++) >> 8) & 0x00FF;
                } else {
                    this->ui->textBrowser->append("读取vector数据越界");
                }
                if(!CanSendFlashData()) {
                    this->ui->textBrowser->append("CAN发送失败");
                    return false;
                }
            }

            if(!CanSendCmdData(flashData)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            check_flow = flashData;
            upgrade_flow = receiveWait;
            break;
        case checkSum:
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
            if(!CanSendData()) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            //this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据校验-->"));
            check_flow = checkSum;
            upgrade_flow = receiveWait;
            break;
        case program:
            //发送program指令
            if(!CanSendCmdData(program)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            //this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据烧写-->"));
            check_flow = program;
            upgrade_flow = receiveWait;
            break;
        case verify:
            //发送校验指令
            if(!CanSendCmdData(verify)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            //this->ui->textBrowser->append(QString("第" + QString::number(data_block_num) +"块数据烧写校验-->"));
            check_flow = verify;
            upgrade_flow = receiveWait;
            break;
        case changeUpdateFlag:
            //发送更改升级标志指令
            if(!CanSendCmdData(changeUpdateFlag)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append(QString("更改Flash升级标志-->"));
            check_flow = changeUpdateFlag;
            upgrade_flow = receiveWait;
            break;
        case resetDSP:
            //发送复位指令
            if(!CanSendCmdData(resetDSP)) {
                this->ui->textBrowser->append("CAN发送失败");
                return false;
            }
            this->ui->textBrowser->append("发送重启DSP指令-->");
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
                        this->ui->textBrowser->append("  握手成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  握手失败");
                        return false;
                    }
                    upgrade_flow = unlockCSM;
                } else {
                    this->ui->textBrowser->append("  握手命令回复错位 " + QString::number(check_flow));
                    return false;
                }
                break;
            case unlockCSM:
                if(check_flow == unlockCSM) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  CSM解锁成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  CSM解锁失败");
                        return false;
                    }
                    upgrade_flow = toggle;
                } else {
                    this->ui->textBrowser->append("  解锁CSM命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case toggle:
                if(check_flow == toggle) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  Toggle测试成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  Toggle测试失败");
                        return false;
                    }
                    upgrade_flow = version;
                } else {
                    this->ui->textBrowser->append("  Toggle测试命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case version:
                if(check_flow == version) {
                    if(0x210 == (can_data.at(1) + (can_data.at(2) << 8))) {
                        this->ui->textBrowser->append("  DSP Flash API版本正确: " + QString::number(0x210, 16));
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  DSP Flash API版本错误: " +
                                 QString::number((can_data.at(1) + (can_data.at(2) << 8)), 16));
                        return false;
                    }
                    upgrade_flow = erase;
                } else {
                    this->ui->textBrowser->append("  API版本命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case erase:
                if(check_flow == erase) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  Flash擦除成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  Flash擦除失败");
                        return false;
                    }
                    upgrade_flow = dataBlockInfo;
                } else {
                    this->ui->textBrowser->append("  Flash擦除命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case dataBlockInfo:
                if(check_flow == dataBlockInfo) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  起始地址对齐成功");
                        progress_bar_value += bar_value_added;
                        this->ui->progressBar->setValue(progress_bar_value);

                    } else {
                        this->ui->textBrowser->append("  数据块信息回复，起始地址对齐错误");
                        return false;
                    }
                    upgrade_flow = flashData;
                } else {
                    this->ui->textBrowser->append("  数据块信息命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case flashData:
                if(check_flow == flashData) {
                    if((num_16bit_data - num_datablock_start) != static_cast<unsigned int>(can_data.at(1) + (can_data.at(2) << 8))) {
                        this->ui->textBrowser->append("  丢失" + QString::number((num_16bit_data - num_datablock_start) - static_cast<unsigned int>(can_data.at(2) + (can_data.at(3) << 8))) +"个数据");
                        return false;
                    } else {
                        this->ui->textBrowser->append("  数据发送完成");
                        upgrade_flow = checkSum;
                    }
                } else {
                    this->ui->textBrowser->append("  数据信息回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case checkSum:
                if(check_flow == checkSum) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  数据校验成功");
                        upgrade_flow = program;
                    } else {
                        this->ui->textBrowser->append("  校验失败，重新发送");
                        num_16bit_data = num_datablock_start;
                        upgrade_flow = dataBlockInfo;
                    }
                } else {
                    this->ui->textBrowser->append("  块数据校验命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case program:
                if(check_flow == program) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  烧写成功");
                    } else {
                        this->ui->textBrowser->append("  烧写失败");
                        return false;
                    }
                    upgrade_flow = verify;
                } else {
                    this->ui->textBrowser->append("  烧写命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case verify:
                if(check_flow == verify) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  Flash校验成功");
                        progress_bar_value += bar_value_added;
                        this->ui->progressBar->setValue(progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  Flash校验失败");
                        return false;
                    }
                    data_block_num++;
                    if((addr_origin.addr_32 + num_16bit_data) == addr_end.addr_32) {
                        upgrade_flow = changeUpdateFlag;
                    } else {
                        upgrade_flow = dataBlockInfo;
                    }
                } else {
                    this->ui->textBrowser->append("  Flash校验命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case changeUpdateFlag:
                if(check_flow == changeUpdateFlag) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  Flash升级标志更改成功");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  Flash升级标志更改失败");
                        return false;
                    }
                    upgrade_flow = resetDSP;
                } else {
                    this->ui->textBrowser->append("  更改Flash升级标志命令回复错位" + QString::number(check_flow));
                    return false;
                }
                break;
            case resetDSP:
                if(check_flow == resetDSP) {
                    if(0x55 == can_data.at(1)) {
                        this->ui->textBrowser->append("  DSP正在复位...");
                        this->ui->progressBar->setValue(++progress_bar_value);
                    } else {
                        this->ui->textBrowser->append("  DSP复位失败");
                        return false;
                    }
                } else {
                    this->ui->textBrowser->append("  更改Flash升级标志命令回复错位" + QString::number(check_flow));
                    return false;
                }
                return true;
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

bool UpgradeProc::CanSendFlashData()
{
    can_data_struct.ID = 0x01A2A3A4;
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

