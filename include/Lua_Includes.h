		
#ifndef _INCLUDE_LIB_LUA_LIB__123412341234
#define _INCLUDE_LIB_LUA_LIB__123412341234

#include "..\VcVerSwitchComm.h"

#if defined( _DEBUG )
    #define _LUA_LIB_  "Debug\\lua5.1"
#else
    //#define _LUA_LIB_  "3rdParty\\Lua\\lua5.1"
    #define _LUA_LIB_  "Release\\lua5.1"
#endif // _DEBUG

#if defined( _LUA_LIB_ )
    #pragma comment(lib, _LIB_PATH_  _Ver_ _LUA_LIB_ ".lib") 
    #pragma message("Automatically linking with " _LIB_PATH_ _Ver_ _LUA_LIB_ ".lib")
#endif

#endif //_INCLUDE_LIB_LUA_LIB__123412341234