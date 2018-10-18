#include "upgradeproc.h"
#include "crc16.h"

UpgradeProc::UpgradeProc() :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    message(new StdMessage),
    m_datablock_size(0x200),
    m_error_code(0)
{
    m_can_data.resize(8);
}

UpgradeProc::UpgradeProc(unsigned short blockSize) :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    message(new StdMessage),
    m_datablock_size(blockSize),
    m_error_code(0)
{
    m_can_data.resize(8);
}

UpgradeProc::UpgradeProc(Message* msg, EraseSector sector, unsigned short blockSize) :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    message(msg),
    sectors(sector),
    m_datablock_size(blockSize),
    m_error_code(0)
{
    m_can_data.resize(8);
}

UpgradeProc::~UpgradeProc()
{

}


bool UpgradeProc::Process()
{
    unsigned int num_datablock = 1;  //发送数据块计数
    unsigned int num_block_start = 0;  //数据块起始数据在vector中的位置
    unsigned int num_flash_data = 0;  //flash数据计数
    unsigned short crc_value = 0;  //CRC16校验值
    Addr addr_origin = hex_parsing->GetOriginAddr();  //起始地址
    Addr addr_end = hex_parsing->GetEndAddr();  //结束长度
    unsigned int addr_len = hex_parsing->GetAddrLength();
    Addr addr = addr_origin;  //数据地址
    Flow upgrade_flow = unlockCSM;
    Flow check_flow = unlockCSM;
    m_error_code = 0;

    int progress_value = 1;  //进度值
    int sum_datablock = addr_len / this->m_datablock_size;
    int progress_added = 100 / sum_datablock;  //进度增加值

    while(true) {
        switch (upgrade_flow) {
        case handshake:
            break;
        case unlockCSM:
            message->Cout("等待CSM解锁-->");
            message->ProgressValue(progress_value);
            //发送解锁指令
            if(!CanSendCmdData(unlockCSM)) {
                return false;
            }
            check_flow = unlockCSM;
            upgrade_flow = receiveWait;
            break;
        case version:
            message->Cout("检查Flash API版本-->");
            //发送version指令
            if(!CanSendCmdData(version)) {
                return false;
            }
            check_flow = version;
            upgrade_flow = receiveWait;
            break;
        case erase:
            message->Cout("等待Flash擦除-->");
            //发送擦除指令
            m_can_data.at(0) = erase;
            m_can_data.at(1) = 0;
            m_can_data.at(2) = this->sectors;
            m_can_data.at(3) = 0;
            m_can_data.at(4) = 0;
            m_can_data.at(5) = 0;
            m_can_data.at(6) = 0;
            m_can_data.at(7) = 0;
            if(!CanSendData()) {
                return false;
            }
            check_flow = erase;
            upgrade_flow = receiveWait;
            break;
        case dataBlockInfo:
            num_block_start = (num_datablock - 1) * this->m_datablock_size;
            num_flash_data = num_block_start;
            addr.addr_32 = addr_origin.addr_32 + (num_datablock - 1) * this->m_datablock_size;
            message->Cout("发送第 " + std::to_string(num_datablock) + "/" +
                          std::to_string(sum_datablock) +" 块数据信息-->");
            //发送数据块信息
            m_can_data.at(0) = dataBlockInfo;
            m_can_data.at(1) = 0;
            m_can_data.at(2) = addr.addr_16.addr_l & 0x00FF;          //地址位 7-0
            m_can_data.at(3) = (addr.addr_16.addr_l >> 8) & 0x00FF;   //地址位 15-8
            m_can_data.at(4) = addr.addr_16.addr_h & 0x00FF;          //地址位 23-16
            m_can_data.at(5) = (addr.addr_16.addr_h >> 8) & 0x00FF;   //地址位 31-24
            m_can_data.at(6) = this->m_datablock_size & 0x00FF;        //数据块大小低位
            m_can_data.at(7) = (this->m_datablock_size >> 8) & 0x00FF; //数据块大小高位
            if(!CanSendData()) {
                return false;
            }
            check_flow = dataBlockInfo;
            upgrade_flow = receiveWait;
            break;
        case flashData:
            //发送数据
            while ((num_flash_data - num_block_start) < this->m_datablock_size) {
                if((num_flash_data + 3) < m_flash_data.capacity()) {
                    m_can_data.at(0) = m_flash_data.at(num_flash_data) & 0x00FF;
                    m_can_data.at(1) = (m_flash_data.at(num_flash_data++) >> 8) & 0x00FF;
                    m_can_data.at(2) = m_flash_data.at(num_flash_data) & 0x00FF;
                    m_can_data.at(3) = (m_flash_data.at(num_flash_data++) >> 8) & 0x00FF;
                    m_can_data.at(4) = m_flash_data.at(num_flash_data) & 0x00FF;
                    m_can_data.at(5) = (m_flash_data.at(num_flash_data++) >> 8) & 0x00FF;
                    m_can_data.at(6) = m_flash_data.at(num_flash_data) & 0x00FF;
                    m_can_data.at(7) = (m_flash_data.at(num_flash_data++) >> 8) & 0x00FF;
                } else {
                    message->Cout("读取vector数据越界");
                }
                if(!CanSendFlashData()) {
                    return false;
                }
            }

            //数据发送结束后，发送一条命令帧
            if(!CanSendCmdData(flashData)) {
                return false;
            }
            check_flow = flashData;
            upgrade_flow = receiveWait;
            break;
        case checkSum:
            //发送校验值
            m_can_data.at(0) = checkSum;
            m_can_data.at(1) = 0;
            crc_value = CrcOfDataBlock(this->m_flash_data, num_block_start, this->m_datablock_size);
            m_can_data.at(2) = crc_value & 0x00FF;
            m_can_data.at(3) = (crc_value >> 8) & 0x00FF;
            m_can_data.at(4) = 0;
            m_can_data.at(5) = 0;
            m_can_data.at(6) = 0;
            m_can_data.at(7) = 0;
            if(!CanSendData()) {
                return false;
            }
            check_flow = checkSum;
            upgrade_flow = receiveWait;
            break;
        case program:
            //发送program指令
            if(!CanSendCmdData(program)) {
                return false;
            }
            check_flow = program;
            upgrade_flow = receiveWait;
            break;
        case verify:
            //发送校验指令
            if(!CanSendCmdData(verify)) {
                return false;
            }
            check_flow = verify;
            upgrade_flow = receiveWait;
            break;
        case resetDSP:
            //发送复位指令
            if(!CanSendCmdData(resetDSP)) {
                return false;
            }
            message->Cout("发送重启DSP指令-->");
            check_flow = resetDSP;
            upgrade_flow = receiveWait;
            break;
        case receiveWait:
            break;
        }

        if(CanReceiveData()) {
            switch(m_can_data.at(0)) {
            case unlockCSM:
                if(check_flow == unlockCSM) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  CSM解锁成功");
                    } else {
                        message->Cout("  CSM解锁失败");
                        return false;
                    }
                    upgrade_flow = version;
                } else {
                    message->Cout("  解锁CSM命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case version:
                if(check_flow == version) {
                    if(0x210 == (m_can_data.at(1) + (m_can_data.at(2) << 8))) {
                        message->Cout("  DSP Flash API版本正确: 0x210");
                    } else {
                        message->Cout("  DSP Flash API版本错误: " +
                                 std::to_string(m_can_data.at(1) + (m_can_data.at(2) << 8)));
                        return false;
                    }
                    upgrade_flow = erase;
                } else {
                    message->Cout("  API版本命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case erase:
                if(check_flow == erase) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  Flash擦除成功");
                        message->ProgressValue(++progress_value);
                    } else {
                        message->Cout("  Flash擦除失败");
                        return false;
                    }
                    upgrade_flow = dataBlockInfo;
                } else {
                    message->Cout("  Flash擦除命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case dataBlockInfo:
                if(check_flow == dataBlockInfo) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  起始地址对齐成功");

                    } else {
                        message->Cout("  数据块信息回复，起始地址对齐错误");
                        return false;
                    }
                    upgrade_flow = flashData;
                } else {
                    message->Cout("  数据块信息命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case flashData:
                if(check_flow == flashData) {
                    if((num_flash_data - num_block_start) != static_cast<unsigned int>(m_can_data.at(1) + (m_can_data.at(2) << 8))) {
                        message->Cout("  丢失" + std::to_string((num_flash_data - num_block_start) -
                                      static_cast<unsigned int>(m_can_data.at(2) + (m_can_data.at(3) << 8))) +"个数据, 重新发送");
                        //return false;
                        num_flash_data = num_block_start;
                        upgrade_flow = dataBlockInfo;
                    } else {
                        message->Cout("  数据发送完成");
                        upgrade_flow = checkSum;
                    }
                } else {
                    message->Cout("  数据信息回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case checkSum:
                if(check_flow == checkSum) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  数据校验成功");
                        upgrade_flow = program;
                    } else {
                        message->Cout("  校验失败，重新发送");
                        num_flash_data = num_block_start;
                        upgrade_flow = dataBlockInfo;
                    }
                } else {
                    message->Cout("  块数据校验命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case program:
                if(check_flow == program) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  Flash烧写成功");
                    } else {
                        message->Cout("  Flash烧写失败");
                        return false;
                    }
                    upgrade_flow = verify;
                } else {
                    message->Cout("  烧写命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case verify:
                if(check_flow == verify) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  Flash校验成功");
                        progress_value += progress_added;
                        message->ProgressValue(progress_value);
                    } else {
                        message->Cout("  Flash校验失败");
                        return false;
                    }
                    num_datablock++;
                    if((addr_origin.addr_32 + num_flash_data) == addr_end.addr_32) {
                        upgrade_flow = resetDSP;
                    } else {
                        upgrade_flow = dataBlockInfo;
                    }
                } else {
                    message->Cout("  Flash校验命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                break;
            case resetDSP:
                if(check_flow == resetDSP) {
                    if(0x55 == m_can_data.at(1)) {
                        message->Cout("  DSP正在复位...");
                        message->ProgressValue(100);
                    } else {
                        message->Cout("  DSP复位失败");
                        message->ProgressValue(0);
                        return false;
                    }
                } else {
                    message->Cout("  更改Flash升级标志命令回复错位" + std::to_string(check_flow));
                    return false;
                }
                return true;
            }
        }
    }
}


/**
 * @brief UpgradeProc::ParseHexFile
 *   第一步. 解析Hex文件
 *
 * @param file
 * @param origin_addr
 * @param addr_len
 * @return
 */
bool UpgradeProc::ParseHexFile(std::string file,
                               unsigned int origin_addr, unsigned int addr_len)
{
    message->Cout("解析Hex文件");
    hex_parsing->SetParameters(file, origin_addr, addr_len);
    if(hex_parsing->Convert()) {
        this->m_flash_data = hex_parsing->GetDataMap();
        message->Cout("  Hex文件解析成功");
        return true;
    } else {
        message->Cout(hex_parsing->GetErrorMsg());
        return false;
    }
}

/**
 * @brief UpgradeProc::InitCAN
 *
 *    第二步. 打开并初始化CAN设备
 *
 * @return
 */
bool UpgradeProc::InitCAN()
{
    message->Cout("打开并初始化CAN设备");
    if(can_func->OpenAndInitDevice()) {
        message->Cout("  CAN设备初始化完成");
        return true;
    } else {
        message->Cout(can_func->GetErrorMsg());
        return false;
    }
}

/**
 * @brief UpgradeProc::WaitForUpgrade
 *
 * 第三步. 等待握手
 */
void UpgradeProc::WaitForUpgrade()
{
    message->Cout("等待进入升级程序...");
    while(true) {
        //等待底层程序发送升级命令，两次握手后结束循环
        if(CanReceiveData()) {
            if(handshake == m_can_data.at(0)) {
                if(0x55 == m_can_data.at(1)) {
                    message->Cout("进入升级程序, 开始升级");
                    break;
                } else if(0 == m_can_data.at(1)) {
                    CanSendCmdData(handshake);
                }
            }
        }
    }
}

bool UpgradeProc::CanSendData()
{
    m_candata_struct.ID = 0x01A2A3A0;
    m_candata_struct.SendType = 0;
    m_candata_struct.RemoteFlag = 0;
    m_candata_struct.ExternFlag = 1;
    m_candata_struct.DataLen = 8;
    m_candata_struct.Data[0] = m_can_data.at(0);
    m_candata_struct.Data[1] = m_can_data.at(1);
    m_candata_struct.Data[2] = m_can_data.at(2);
    m_candata_struct.Data[3] = m_can_data.at(3);
    m_candata_struct.Data[4] = m_can_data.at(4);
    m_candata_struct.Data[5] = m_can_data.at(5);
    m_candata_struct.Data[6] = m_can_data.at(6);
    m_candata_struct.Data[7] = m_can_data.at(7);

    if(can_func->Transmit(Dev_Index, Can_Index_1, &m_candata_struct)) {
        return true;
    } else {
        message->Cout("CAN发送失败");
        return false;
    }
}

bool UpgradeProc::CanSendFlashData()
{
    m_candata_struct.ID = 0x01A2A3A4;
    m_candata_struct.SendType = 0;
    m_candata_struct.RemoteFlag = 0;
    m_candata_struct.ExternFlag = 1;
    m_candata_struct.DataLen = 8;
    m_candata_struct.Data[0] = m_can_data.at(0);
    m_candata_struct.Data[1] = m_can_data.at(1);
    m_candata_struct.Data[2] = m_can_data.at(2);
    m_candata_struct.Data[3] = m_can_data.at(3);
    m_candata_struct.Data[4] = m_can_data.at(4);
    m_candata_struct.Data[5] = m_can_data.at(5);
    m_candata_struct.Data[6] = m_can_data.at(6);
    m_candata_struct.Data[7] = m_can_data.at(7);

    if(can_func->Transmit(Dev_Index, Can_Index_1, &m_candata_struct)) {
        return true;
    } else {
        message->Cout("CAN发送失败");
        return false;
    }
}

bool UpgradeProc::CanSendCmdData(Flow flow)
{
    m_can_data.at(0) = flow;
    m_can_data.at(1) = 0;
    m_can_data.at(2) = 0;
    m_can_data.at(3) = 0;
    m_can_data.at(4) = 0;
    m_can_data.at(5) = 0;
    m_can_data.at(6) = 0;
    m_can_data.at(7) = 0;

    if(CanSendData())
        return true;
    return false;
}

bool UpgradeProc::CanReceiveData()
{
    if(can_func->GetReceiveNum(Dev_Index, Can_Index_1)) {
        if(can_func->Receive(Dev_Index, Can_Index_1, &m_candata_struct, 1, 5)) {
            if((0x01A2A3A1 == m_candata_struct.ID) && (8 == m_candata_struct.DataLen)) {
                m_can_data.at(0) = m_candata_struct.Data[0];
                m_can_data.at(1) = m_candata_struct.Data[1];
                m_can_data.at(2) = m_candata_struct.Data[2];
                m_can_data.at(3) = m_candata_struct.Data[3];
                m_can_data.at(4) = m_candata_struct.Data[4];
                m_can_data.at(5) = m_candata_struct.Data[5];
                m_can_data.at(6) = m_candata_struct.Data[6];
                m_can_data.at(7) = m_candata_struct.Data[7];
                return true;
            }
        }
    }
    return false;
}

