#ifndef _RWCOMM_VERSION
#define _RWCOMM_VERSION

#include "RwComm.h"

extern "C"
{

__declspec(dllexport) unsigned int RwCommGetVersion()
{
    int vers[] = {RWCOMM_VERSION_STRING};
    return RWCOMM_MAKE_VERSION(vers[0], vers[1], vers[2], vers[3]);
}

#ifndef _NODBGCHECK
__declspec(dllexport) bool RwCommIsDebug()
{
#ifdef _DEBUG
    return true;
#else
    return false;
#endif
}
#endif

};

#if !defined(_CPPRTTI)
#error Must compile with RTTI
#endif



#endif