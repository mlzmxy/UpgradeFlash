#ifndef HEXPARSING_H
#define HEXPARSING_H

#include <string>
#include <vector>
using std::string;
using std::vector;

// HEX文件的行格式结构
typedef struct _LINE_DATA {
    unsigned short l_len;      //数据长度
    unsigned short l_addr;     //地址
    unsigned short l_type;     //记录类型
    unsigned short l_data[16]; //数据，这里最大长度为32个字节
    unsigned short l_check;    //校验
}LineForm, *PLineForm;

// 32位地址结构
typedef union _ADDR {
	unsigned int addr_32;  //完整地址
	struct {
        unsigned short addr_l;  //地址低位
        unsigned short addr_h;  //地址高位
	}addr_16;
}Addr, *PAddr;

class HexParsing
{
public:
	HexParsing();
	HexParsing(string file_path_name, unsigned int origin_address,
		unsigned int addr_length);
	~HexParsing();

	bool Convert();
    void SetParameters(string file_path_name, unsigned int origin_address,
                       unsigned int addr_length);

    vector<unsigned short> GetDataMap();
	string GetErrorMsg();
    unsigned int GetAddrLength();
    Addr GetOriginAddr();
    Addr GetEndAddr();

private:
    vector<unsigned short> m_data_vec;  //转换后的数据序列
    string m_file;                          //源文件
    Addr m_addr_origin;                     //起始地址
    Addr m_addr_end;                        //结束地址
    unsigned int m_len;                     //地址长度
    unsigned int m_error_code;              //错误类型
    unsigned int m_error_rec;               //错误记录

	bool MappedData(PLineForm data, PAddr addr);
	bool CheckData(PLineForm data);
	bool CharBuffer2HexData(char* buf, PLineForm data);
    unsigned short Char2IntByte(char d_h, char d_l);
    unsigned short Char2ShortInt(char d_4, char d_3, char d_2, char d_1);
    unsigned short Char2Int(char ch);
};


#define FILE_OPEN_ERROR         0x01  //文件打开错误
#define FILE_LINE_ERROR         0x02  //读取的行数据错误
#define LINE_DATA_LENGTH_ERROR  0x03  //数据转换错误, 数据长度超过范围
#define FILE_ILLEGAL_CHAR_ERROR 0x04  //数据转换错误, 非法字符
#define FILE_LINE_CHECK_ERROR   0x05  //行数据校验错误
#define FILE_LINE_TYPE_ERROR    0x06  //行数据类型错误
#define FILE_NOEND_ERROR        0x07  //hex文件无结束行
#define ADDRESS_OUT_OF_RANGE    0x08  //地址越界
#define ADDRESS_LENGTH_ILLEGAL  0x09  //地址长度非法
#define VECTOR_OUT_OF_RANGE     0x0A  //vector存储越界


#endif // HEXPARSING_H
