#ifndef CRC_H
#define CRC_H

#include <vector>
using std::vector;

unsigned short crc16(const char *buf, int len);
unsigned short CrcOf4Char(const unsigned char ch1, const unsigned char ch2, const unsigned char ch3, const unsigned char ch4);
unsigned short CrcOfDataBlock(const vector<unsigned short> &buf, unsigned int num_start, unsigned short len);

#endif // CRC_H
