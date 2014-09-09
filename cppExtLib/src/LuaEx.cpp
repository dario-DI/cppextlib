#include "stdafx.h"
#include <stdio.h>
#include <sstream>
#include <shellapi.h>
#include <cex/LuaEx.h>


//load lua libary
#pragma comment(lib, _3RDPARTY_PATH_  _LUA_LIB_ ".lib") 
#pragma message("Automatically linking with " _3RDPARTY_PATH_ _LUA_LIB_ ".lib")

using namespace cex;

//#pragma comment(lib, "lua5.1.lib")

#define LUA_ENABLE_ERROR_REPORT

namespace cex
{
	class GetGlobaleAndPop
	{
	public:
		GetGlobaleAndPop( lua_State* L, LPCSTR str ) : _L(L)
		{
			lua_getglobal(_L, str);	//	����ֵ����ջ��
		}

		~GetGlobaleAndPop()
		{
			lua_pop( _L, 1 );	//	������ֵ��ջ, �ָ�ջ��Ԫ��
		}

	protected:

		lua_State* _L;

	};

	static std::string GetWorkPath(HMODULE hModule)
	{
		CHAR fileName[MAX_PATH+1];

		GetModuleFileNameA(hModule, fileName, MAX_PATH);

		std::string strFile( fileName );

		int pos=0;

		//	�������һ�� '\' ����
		for(size_t i=0; i<strFile.length(); ++i)
		{
			if(strFile.at(i)=='\\')
			{
				pos=i;
			}
		}

		std::string strPath( strFile.begin(), strFile.begin() + pos + 1 );

		return strPath;
	}

	void error (lua_State *L, const char *fmt, ...)
	{
#ifdef LUA_ENABLE_ERROR_REPORT
		std::string errfile = GetWorkPath(0) + LUA_ERROR_FILE_NAME;
		FILE* file = NULL;
		fopen_s(&file, errfile.data(), "w+" );
		assert( file );

		va_list argp;
		va_start(argp, fmt);
		vfprintf(file, fmt, argp);
		va_end(argp);

		fclose( file );

		//lua_close(L);
		//exit(EXIT_FAILURE);
#endif
	}

	//
	//	void Configcex()
	//	{
	//		const char* filename = "cexConfig.con";
	//
	//		lua_State* L = Util::LuaStateEx::::get();
	//		if ( L == NULL ) return;
	//
	//		if ( luaL_loadfile( L, filename ) ||
	//			lua_pcall( L, 0, 0, 0 ) != 0 //	���Ͷ����ı�
	//			)
	//		{
	//			assert(false);
	//			TRACE( _T("can not open file %s"), lua_tostring( L, -1 ) );
	//		}
	//
	//		lua_getglobal( L, "overview_width" );
	//		lua_getglobal( L, "overview_height" );
	//
	//		if ( !lua_isnumber( L, -2 ) ||
	//			 !lua_isnumber( L, -1 )
	//			)
	//		{
	//			assert( false );
	//		}
	//
	//		OVERVIEW_WIDTH = (int) lua_tonumber( L, -2 );
	//
	//		OVERVIEW_HEIGHT = (int) lua_tonumber( L, -1 );
	//	}
	//
	//	int luaC_messageBox( lua_State* L )
	//	{
	//		char message[256] = "";
	//
	//		int n = lua_gettop( L );
	//
	//		if ( lua_isstring( L, 1 ) )
	//		{
	//			strcpy( message, lua_tostring( L, 1 ) );
	//		}
	//
	//		MessageBoxA( NULL, message, "Lua Test", MB_OK );
	//
	//		return 0;
	//	}
	//
	//void RegistLuaCFun()
	//{
	//	lua_State* L = Util::LuaStateEx::get();

	//	//lua_register( L, "luaC_messageBox", luaC_messageBox );
	//	lua_register( L, "luaC_messageBoxEx", luaC_messageBoxEx );
	//}

	int luaC_messageBoxEx( lua_State* L )
	{
		HWND    hWnd;
		char       lpText[256] = "";
		char       lpCaption[256] = "";
		UINT      uType;


		// ������
		LuaEx::GetField( L, "hWnd",        &hWnd,        GTC_LP);
		LuaEx::GetField( L,  "lpText",       &lpText,       GTC_STRING);
		LuaEx::GetField( L,  "lpCaption", &lpCaption, GTC_STRING);
		LuaEx::GetField( L,  "uType",       &uType,        GTC_INT);

		// ִ���߼�
		MessageBoxA( hWnd, lpText, lpCaption, uType);

		// ����ֵѹջ

		// ����ѹջ�����ĸ���
		return 0;
	}

	int Print( lua_State* L )
	{
		luaL_checktype(L, -1, LUA_TSTRING );
		lua_setglobal( L, "s_strPrint" );
		return 0;
	}

	//LUA_DECL_WRAPPER( cex::cexApp*, luacexWrapper )


	LUA_BEGIN_REGIST_LIB( GlobalLib )
		LUA_REGIST_FUNC( Print )
		LUA_END_REGIST_OPEN_LIB1( GlobalLib )

		//static const struct luaL_reg cexlib [] = 
		//{
		//	{"luaC_messageBoxEx", luaC_messageBoxEx},
		//	//{"GetApp", GetcexApp},
		//	//{"SetViewScale", SetcexViewSclae},
		//	{"LuaC_RegistGloable", LuaC_RegistGloable},
		//	{NULL, NULL}  /* sentinel */
		//};

		//extern "C" int _declspec(dllexport) TL_FunctionExport(lua_State *L)
		//{ 
		//	luaL_openlib(L, "cex", mylibs, 0);
		//	return 1;

		//	//	call in lua as --assert( package.loadlib ("MFC_OSGud.dll", "TL_FunctionExport") ) ();
		//	//	��LuaStateEx::get()��ʹ��luaL_openlib(_L, "cex", mylibs, 0)������
		//	//	���ַ�ʽ����Ҫ�޸�LuaConf.h�е� LUA_COMPAT_LOADLIB ����
		//}

		void testLua()
	{
		//Configcex();
		LuaStateAutoPtr luaState(new LuaState);

		char* cexMethod = "C:\\Users\\xl\\Desktop\\lua\\program\\cexMethod.lua";

		if ( LuaEx::DoFile(luaState->get(), cexMethod ) != 0 )
		{
			assert( false );	// to see LUA_ERROR_FILE_NAME
		}

		char* lueTest = "C:\\Users\\xl\\Desktop\\lua\\program\\testCFun.lua";

		if ( LuaEx::DoFile(luaState->get(),  lueTest ) != 0 )
		{
			assert( false );	// to see LUA_ERROR_FILE_NAME
		}

	}
}
////////////////////////////////////////////////////////
int LuaEx::DoFile( lua_State* L, LPCSTR str )
{
	int nResult = luaL_dofile(L, str);

	if ( nResult != 0 )
	{
		error(L, "do file faild: %s", lua_tostring(L, -1));
	}

	return nResult;
}

int LuaEx::DoString( lua_State* L,  LPCSTR str )
{
	int nResult = luaL_dostring(L, str);

	if ( nResult != 0 )
	{
		error(L, "do string faild!\r\ncode:\r\n%s\r\nerror info:\r\n%s", str, lua_tostring(L, -1));
	}

	return nResult;
}

LPSTR LuaEx::LoadString( lua_State* L,  LPCSTR str )
{
	GetGlobaleAndPop makeGet(L, str);

	if (lua_isstring(L, -1))
	{
		return (LPSTR)lua_tostring(L, -1);
	}
	return NULL;
}

int LuaEx::LoadInteger( lua_State* L,  LPCSTR str )
{
	GetGlobaleAndPop makeGet(L, str);

	if (lua_isnumber(L, -1))
	{
		return (int)lua_tointeger(L, -1);
	}
	return NULL;
}

double LuaEx::LoadDouble( lua_State* L,  LPCSTR str )
{
	GetGlobaleAndPop makeGet(L, str);

	if (lua_isnumber(L, -1))
	{
		return (double)lua_tonumber(L, -1);
	}
	return 0.0;

}

bool LuaEx::LoadBoolean( lua_State* L,  LPCSTR str )
{
	GetGlobaleAndPop makeGet(L, str);

	BOOST_ASSERT( sizeof(double) == 0 );

	if (lua_isboolean(L, -1))
	{
		return lua_toboolean(L, -1) != 0;
	}
	return false;
}

void LuaEx::GetField( lua_State* L,  LPCSTR key, void* ret, int type_flag )
{
	//	�˺�����lua�е���C�����б���ӵ��ã���˲����Ѿ�������������ջ

	lua_pushstring( L, key );	//	��keyֵ��ջ

	lua_gettable( L, -2 );		//	���ں�������ֻ��һ��������һ��table��(��lua����), ����-2λ��Ϊtable��

	//	��gettable֮��keyֵ��ջ������ֵ������ջ��

	switch ( type_flag )
	{
	case GTC_LP:
		/*if ( lua_ispointer(L, -1) == false )
		{
		assert( false );
		}*/
		(*(LPVOID*)ret) = (LPVOID)lua_topointer(L, -1);
		break;

	case GTC_INT:
		if ( lua_isnumber(L, -1) == false )
		{
			assert( false );
		}
		(*(int*)ret) = (int)lua_tointeger(L, -1);
		break;

	case GTC_DOUBLE:
		if ( lua_isnumber(L, -1) == false )
		{
			assert( false );
		}
		(*(double*)ret) = (double)lua_tonumber(L, -1);
		break;

	case GTC_BOOL:
		if ( lua_isboolean(L, -1) == false )
		{
			assert( false );
		}
		(*(bool*)ret) = lua_toboolean(L, -1) != 0;
		break;

	case GTC_STRING:
		strcpy_s( (char*)ret, sizeof(ret), lua_tostring(L, -1) );
		break;

	default:
		break;
	}

	lua_pop( L, 1 );	//	������ֵ��ջ, �ָ�ջ��Ԫ��
}

void LuaEx::Setfenv( lua_State* L, LPCSTR newGlobal )
{
	//lua_getglobal( L, "_G" );

	//lua_getglobal( L, newGlobal );

	//lua_setmetatable(L, -2);

	//const char* code = 
	//	"setmetatable(newgt, {__index=_G})\n"
	//	"setfenv(0, newgt)";

	///	setfenv(1, newgt) �������ã���setfenv(0, newgt)����

	std::ostringstream oss;
	oss << "setmetatable(newgt, {__index=_G})\n";
	oss << "setfenv(0, ";
	oss << newGlobal;
	oss << ");";

	std::string strCode = oss.str();

	int r = DoString( L, strCode.c_str() );

	assert( r == 0 );	//	to see LUA_ERROR_FILE_NAME
}

void LuaEx::ShowErrorInfo(int nType)
{
	ShellExecuteA(NULL, "open", LUA_ERROR_FILE_NAME, NULL, NULL, SW_SHOWNORMAL);
}

#pragma region LuaState
LuaState::LuaState()
{
	InitialObject( "_G" );
}

LuaState::LuaState(const std::string& lib)
{
	InitialObject( lib );
}

void LuaState::InitialObject(const std::string& lib)
{
	_L = lua_open();
	assert( _L != NULL );

	luaL_openlibs(_L); //�������л���lua��

	cex::DeltaInstance<ILuaCLibRegister>()->OpenLib( _L, lib );
}

LuaState::~LuaState()
{
	Close();
}

lua_State* LuaState::get()
{
	return _L;
}

void LuaState::Close()
{
	if ( _L != NULL )
	{
		lua_close(_L);
		_L = NULL;
	}
}

std::vector<std::string> LuaState::GetFunctionName( const luaL_reg* reg )
{
	std::vector<std::string> vtName;

	while ( reg != NULL && (*reg).name != NULL )
	{
		vtName.push_back( std::string( (*reg).name ) );
		++reg;
	}

	return vtName;
}

#pragma endregion

////////////////////////////////////////////////////////////////
class CLuaCLibRegister : public ILuaCLibRegister
{
public:
	void AddRegist( const std::string& lib, const LuaRegistLibFunc::FuncType& func )
	{
		_map[lib] += func;
	}

	void OpenLib( lua_State* L, const std::string& lib )
	{
		_map["_G"]( L );

		_map[lib]( L );
	}

private:

	FUNC_MAP _map;
};

namespace cex
{
	ILuaCLibRegister* __stdcall getOrCreateLuaCLibRegister()
	{
		try
		{
			cex::DeltaCast<ILuaCLibRegister*>(DELTA_REGKEY_SYS_INSTANCE,
				typeid(ILuaCLibRegister*).name());
		}
		catch(std::exception e)
		{
			cex::DeltaObjInstanceRegistProxy<ILuaCLibRegister, CLuaCLibRegister> temp;
		}

		return cex::DeltaInstance<ILuaCLibRegister>();
	}
}
