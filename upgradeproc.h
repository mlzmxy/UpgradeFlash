#ifndef UPGRADEPROC_H
#define UPGRADEPROC_H

#include <string>
#include <vector>
using std::vector;

#include "hexparsing.h"
#include "canfunc.h"

class UpgradeProc
{
public:
    UpgradeProc();

    bool Process();
    bool LoadCanFuncs();
    bool ParseHexFile();

    void SetHexParseSettings(string file,unsigned int origin_addr, unsigned int addr_len);

private:
    vector<unsigned short int> data;

    HexParsing* hex_parsing;
    CanFunc* can_func;

    unsigned int error_code;
};

#define ERROR_CANFUNC    0x1  //CanFunc错误
#define ERROR_HEXPARSING 0x2  //HexParsing错误

#endif // UPGRADEPROC_H
