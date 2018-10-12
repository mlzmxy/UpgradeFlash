#ifndef CRC_H
#define CRC_H

unsigned short crc16(const char *buf, int len);
unsigned short CrcOf4Char(const char ch1, const char ch2, const char ch3, const char ch4);
unsigned short CrcOfDataBlock(const vector<unsigned short> &buf, unsigned int num_start, unsigned short len);

#endif // CRC_H
