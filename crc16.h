#ifndef CRC_H
#define CRC_H

#include <vector>
using std::vector;

unsigned short Crc16(const char *buf, int len);
unsigned short CrcOfDataBlock(const vector<unsigned short> &buf, unsigned int num_start, unsigned short len);

#endif // CRC_H
