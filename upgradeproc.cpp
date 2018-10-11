
#include "upgradeproc.h"

UpgradeProc::UpgradeProc() :
    hex_parsing(new HexParsing),
    can_func(new CanFunc),
    error_code(0)
{

}

bool UpgradeProc::Process()
{

}

bool UpgradeProc::LoadCanFuncs()
{
    if(can_func->LoadDllFuncs()) {
        return true;
    } else {
        error_code = ERROR_CANFUNC;
        return false;
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
