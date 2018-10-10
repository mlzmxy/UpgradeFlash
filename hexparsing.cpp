#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "hexparsing.h"

using std::ifstream;


HexParsing::HexParsing()
    :file(""),
	len(0)
{
    addr_origin.addr_32 = 0;
	addr_end.addr_32 = 0;
	error_code = 0;
}

HexParsing::~HexParsing()
{

}

/**
 * HexParsing
 * 
 * @param file_path_name  hex文件的路径及名称
 * @param origin_address  hex文件映射的起始地址
 * @param addr_length     地址长度
 */
HexParsing::HexParsing(string file_path_name, unsigned int origin_address,
	unsigned int addr_length)
	:file(file_path_name),
	len(addr_length)
{
	addr_origin.addr_32 = origin_address;
	addr_end.addr_32 = origin_address + addr_length;
	//设置数据vector大小，并全部初始化为1
	data_vec.resize(addr_length, 0xFFFF);
	error_code = 0;
}

/**
 * Convert  转换文件
 *   打开hex文件并解析数据。
 *   
 * @return
 *   true  转换成功
 *   false 转换失败
 */
bool HexParsing::Convert()
{
	char buf[80];  //hex每一行的数据
	LineForm line_data;
	Addr addr_t = { 0 }; //32位地址

	ifstream in_file(file);
	if (!in_file.is_open()) {
		error_code = FILE_OPEN_ERROR;
		return false;
	}

	int i_char_num = 0;  //读取的行数据长度，最少为11
    unsigned int line_number = 1;  //记录行数
	int file_end_flag = 0;  //hex文件结束行(:00000001FF)标志
	in_file.getline(buf, 50);
	while (!in_file.eof()) {
		i_char_num = 0;
		while (buf[i_char_num] != '\0') {
			i_char_num++;
		}
		if (i_char_num < 11) {
			error_code = FILE_LINE_ERROR;
			error_rec = line_number;
			return false;
		}

		if (buf[0] == ':') {
			//将文件字符数据转换为整型数据
			if (CharBuffer2HexData(buf, &line_data)) {
				//数据校验
				if (CheckData(&line_data)) {
					if (0x0 == line_data.l_type) {  //映射数据
						if (!MappedData(&line_data, &addr_t)) {
							error_rec = line_number;
							return false;
						}
					}
					else if (0x04 == line_data.l_type) {  //切换高位地址
						addr_t.addr_16.addr_h = line_data.l_data[0];
					}
					else if (0x01 == line_data.l_type) {  //文件结束
						file_end_flag = 1;
						break;
					}
					else {
						error_code = FILE_LINE_TYPE_ERROR;
						error_rec = line_number;
						return false;
					}
				}
				else {
					error_code = FILE_LINE_CHECK_ERROR;
					error_rec = line_number;
					return false;
				}
			}
			else {
				error_rec = line_number;
				return false;
			}
		}
		else {
			error_code = FILE_LINE_ERROR;
			error_rec = line_number;
			return false;
		}

		in_file.getline(buf, 139);
		line_number++;
	}

	in_file.close();

	if (file_end_flag) {
		return true;
	}
	else {
		error_code = FILE_NOEND_ERROR;
		error_rec = line_number;
		return false;
	}
}

/**
 * GetErrorMsg  错误信息
 *   当Convert()函数执行失败后，可以调用此函数查看错误信息。
 *   
 * @return 错误信息描述
 */
string HexParsing::GetErrorMsg()
{
	string msg = "";
	switch (error_code) {
	case FILE_OPEN_ERROR:
		msg = "文件打开失败";
		break;
	case FILE_LINE_ERROR:
		msg = "第" + std::to_string(error_rec) + "行, 行数据错误";
		break;
	case LINE_DATA_LENGTH_ERROR:
		msg = "第" + std::to_string(error_rec) + "行, 数据转换错误, 数据长度超过范围";
		break;
	case FILE_ILLEGAL_CHAR_ERROR:
		msg = "第" + std::to_string(error_rec) + "行, 数据转换错误, 非法字符";
		break;
	case FILE_LINE_CHECK_ERROR:
		msg = "第" + std::to_string(error_rec) + "行, 数据校验错误";
		break;
	case FILE_LINE_TYPE_ERROR:
		msg = "第" + std::to_string(error_rec) + "行, 数据类型错误";
		break;
	case FILE_NOEND_ERROR:
		msg = "hex文件无结束行";
		break;
	case ADDRESS_OUT_OF_RANGE:
		msg = "第" + std::to_string(error_rec) + "行, 地址越界";
		break;
	case VECTOR_OUT_OF_RANGE:
		msg = "第" + std::to_string(error_rec) + "行, vector越界";
		break;
	default:
		msg = "未知错误";
		break;
	}
	return msg;
}

/**
 * GetDataMap  返回转换后的数据序列
 * 
 * @return 转换后的数据vector
 */
vector<unsigned short int> HexParsing::GetDataMap()
{
	return data_vec;
}

/**
 * MappedData  将hex文件中的数据映射到转换的数据序列中
 * 
 * @param data hex一行的数据
 * @param addr 当前行数据的地址
 * 
 * @return
 *   true
 *   false
 * 
 * @error
 *   ADDRESS_OUT_OF_RANGE 地址越界
 *   VECTOR_OUT_OF_RANGE  vector存储越界
 */
bool HexParsing::MappedData(PLineForm data, PAddr addr)
{
	addr->addr_16.addr_l = data->l_addr;
	if ((addr->addr_32 < addr_origin.addr_32)
		|| (addr->addr_32 > addr_end.addr_32)) {
		error_code = ADDRESS_OUT_OF_RANGE;
		return false;
	}

	unsigned int distance = addr->addr_32 - addr_origin.addr_32;

    for (unsigned int i = 0; i < data->l_len; ++i) {
		try {
			data_vec.at(distance + i) = data->l_data[i];
		}
		catch (const std::out_of_range& oor) {
			error_code = VECTOR_OUT_OF_RANGE;
			break;
		}
	}
	if (error_code)
		return false;
	return true;
}

/**
 * CheckData  校验数据
 * 
 *   校验值计算方式：0x100减去所有值相加的和(除去':'和校验值本身)
 * 
 * @param data  一行hex数据
 * 
 * @return
 *   true/false
 * 
 * @error
 *   FILE_LINE_CHECK_ERROR  行数据校验错误
 */
bool HexParsing::CheckData(PLineForm data)
{
	int sum = 0;
	sum += data->l_len * 2;
	sum = sum + (data->l_addr & 0xFF) + ((data->l_addr >> 8) & 0xFF);
	sum += data->l_type;
	for (int i = 0; i < data->l_len; ++i) {
		sum = sum + (data->l_data[i] & 0x00FF) + ((data->l_data[i] >> 8) & 0x00FF);
	}

	sum = (0x100 - (sum & 0x00FF)) & 0x00FF;
	if (sum == data->l_check)
		return true;
	else
		return false;
}

/**
 * CharBuffer2HexData  将字符串形式的数据转换为整型数据
 *   字符串中的数据是以十六进制的形式显示的。
 * 
 * @param buf  字符数据
 * @param data 整型数据
 * 
 * @return
 *   true/false
 * 
 * @error
 *   LINE_DATA_LENGTH_ERROR   数据转换错误, 数据长度超过范围
 *   FILE_ILLEGAL_CHAR_ERROR  数据转换错误, 非法字符
 */
bool HexParsing::CharBuffer2HexData(char* buf, PLineForm data)
{
	data->l_len = Char2IntByte(buf[1], buf[2]);  //数据长度
	if (data->l_len > 0x20) {
		error_code = LINE_DATA_LENGTH_ERROR;
		return false;
	}
	data->l_len = data->l_len / 2;  //按16位(2字节)长度计算
	data->l_addr = Char2ShortInt(buf[3], buf[4], buf[5], buf[6]);  //地址
	data->l_type = Char2IntByte(buf[7], buf[8]);  //记录类型

	int j = 9;  //第10个值开始为数据
	// 转换数据
	for (int i = 0; i < data->l_len; ++i) {
		data->l_data[i] = Char2ShortInt(buf[j], buf[j + 1], buf[j + 2], buf[j + 3]);
		j += 4;
	}
	// 转换校验值
	data->l_check = Char2IntByte(buf[j], buf[j + 1]);

	if (error_code)
		return false;  //出现非法字符
	else
		return true;
}

/**
 * Char2ShortInt  char --> short int(16 bit)
 *   将4个字符转换为长度为16位的short int
 * 
 * @param d_4  15-12
 * @param d_3  11-8
 * @param d_2  7-4
 * @param d_1  3-0
 * 
 * @return 16位的short int
 */
unsigned short int HexParsing::Char2ShortInt(char d_4, char d_3, char d_2, char d_1)
{
	return (Char2Int(d_4) << 12 | Char2Int(d_3) << 8 | Char2Int(d_2) << 4 | Char2Int(d_1));
}

/**
 * Char2IntByte  char --> short int(8 bit)
 *   将2个字符转换为长度为8位的int(short int低8位)
 * 
 * @param d_h  7-4
 * @param d_l  3-0
 * 
 * @return short int 低8位有效
 */
unsigned short int HexParsing::Char2IntByte(char d_h, char d_l)
{
	return (Char2Int(d_h) << 4 | Char2Int(d_l));
}

/**
 * Char2Int  将单个字符转换为与之对应的int值
 * 
 * @param ch 字符
 * 
 * @return 转换后的int值，若转换失败则返回0
 * 
 * @error
 *   FILE_ILLEGAL_CHAR_ERROR  数据转换错误, 非法字符
 */
unsigned short int HexParsing::Char2Int(char ch)
{
	unsigned short int t;
	switch (ch) {
	case '0':
		t = 0x0;
		break;
	case '1':
		t = 0x1;
		break;
	case '2':
		t = 0x2;
		break;
	case '3':
		t = 0x3;
		break;
	case '4':
		t = 0x4;
		break;
	case '5':
		t = 0x5;
		break;
	case '6':
		t = 0x6;
		break;
	case '7':
		t = 0x7;
		break;
	case '8':
		t = 0x8;
		break;
	case '9':
		t = 0x9;
		break;
	case 'A':
		t = 0xA;
		break;
	case 'B':
		t = 0xB;
		break;
	case 'C':
		t = 0xC;
		break;
	case 'D':
		t = 0xD;
		break;
	case 'E':
		t = 0xE;
		break;
	case 'F':
		t = 0xF;
		break;
	default:
		error_code = FILE_ILLEGAL_CHAR_ERROR;
		t = 0;
	}
	return t;
}

/**
 * Prepare  转换准备
 *   当使用默认构造函数创建对象时，在执行Convert()函数之前需要设置：
 *     1. SetFilePathName(...)
 *     2. SetOrigin_Address(...)
 *     3. SetAddrLength(...)
 *     4. Prepare()
 *   如果采用带参数的构造函数，则不需要进行上述设置。
 */
void HexParsing::Prepare()
{
	addr_end.addr_32 = this->addr_origin.addr_32 + this->len;
	//设置数据vector大小，并全部初始化为1
	data_vec.resize(this->len, 0xFFFF);
}

/**
 * SetFilePathName  设置hex文件的路径及名称
 */
void HexParsing::SetFilePathName(string file_path_name)
{
    this->file = file_path_name;
}

/**
 * SetOrigin_Address  设置转换数据的起始地址
 */
void HexParsing::SetOrigin_Address(unsigned int origin_address)
{
    this->addr_origin.addr_32 = origin_address;
}

/**
 * SetAddrLength  设置地址长度
 */
void HexParsing::SetAddrLength(unsigned int addr_length)
{
    this->len = addr_length;
}
