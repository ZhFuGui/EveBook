#ifndef CS2ASSIST_MISC_UTIL_H
#define CS2ASSIST_MISC_UTIL_H
#include <thread>
#include "../entity/EntityMgr.h"
#include <cmath>

namespace CS2Assist {

    BOOL ByteToChar(BYTE* byteCode, char* strCode, int codeLen);
    

    BOOL cmpStrCode(char* code, char* readStr, int cmpLen);
   

    BOOL scanGameCode(HANDLE hProcess, uint64_t beginAddr, uint64_t endAddr, char* code, int codeLen, DWORD64& retAddr); 


} // namespace CS2Assist

#endif // CS2ASSIST_MISC_UTIL_H