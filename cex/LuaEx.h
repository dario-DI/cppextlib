/// \file LuaEx.h Copyright (C) Sharewin Inc.
/// \brief lua脚本扩展
///
///
/// \note:
/// \author: DI
/// \time: 2011/8/12 10:30
#pragma once

extern "C"
{
#include <cex/lua/lua.h>
#include <cex/lua/lauxlib.h>
#include <cex/lua/lualib.h>
}

#include <string>
#include <vector>
#include <map>
#include <cex/config>
#include <cex/Delegate.hpp>

//#include <boost/shared_ptr.hpp>

#ifndef GTC_LP
#define GTC_LP	0
#define GTC_INT		1<<1
#define	GTC_DOUBLE	1<<2
#define	GTC_BOOL	1<<3
#define	GTC_STRING	1<<4
#endif

#define LUA_ERROR_FILE_NAME "lua_error.txt"

namespace cex
{
#define LUA_OPEN_USER_LIB( spaceName, lib ) \
	cex::CLuaRegistLibProxy s_LuaRegistLibProxy_##lib(spaceName, (luaL_reg*)lib);

#define LUA_BEGIN_REGIST_LIB( lib ) \
	static const struct luaL_reg lib [] = {

#define LUA_REGIST_FUNC( function ) {#function, function},

#define LUA_REGIST_FUNC2( newName, function ) {#newName, function},

#define LUA_END_REGIST_LIB \
	{NULL, NULL} };

#define LUA_END_REGIST_OPEN_LIB( nameSpace, lib ) \
	LUA_END_REGIST_LIB \
	LUA_OPEN_USER_LIB( nameSpace, lib )

#define LUA_END_REGIST_OPEN_LIB1( lib ) \
	LUA_END_REGIST_OPEN_LIB( "_G", lib )

#define LUA_END_REGIST_OPEN_LIB2( nameSpace, lib ) \
	LUA_END_REGIST_OPEN_LIB( nameSpace, lib )

	template<typename T>
	struct luaUserdataWrapper
	{
		typedef T type;

		luaUserdataWrapper() {}
		luaUserdataWrapper(const type& d) : data(d) {}

		type data; 
	};

#define LUA_DECL_WRAPPER( type_c, type_wrapper ) \
	typedef cex::luaUserdataWrapper<type_c> type_wrapper;

	class CEX_IMPORT LuaEx
	{
	public:
		/** make do file. */
		static int DoFile( lua_State* L, LPCSTR str );

		/** make do string. */
		static int DoString( lua_State* L, LPCSTR str );

		/** make getGlobale string in lua globale code. */
		static LPSTR LoadString( lua_State* L, LPCSTR str ); 

		/** make getGlobale int in lua globale code. */
		static int LoadInteger( lua_State* L, LPCSTR str );

		/** make getGlobale number in lua globale code. */
		static double LoadDouble( lua_State* L, LPCSTR str );

		/** make getGlobale bool in lua globale code. */
		static bool LoadBoolean( lua_State* L, LPCSTR str );

		/**	make lua_gettable of the table at -1. */
		static void GetField( lua_State* L, LPCSTR key, void* ret, int type_flag );


		/** 设置全局环境. */
		static void Setfenv( lua_State* L, LPCSTR newGlobal );


		///	显示错误信息, nType:显示方式. 
		static void ShowErrorInfo(int nType=0);
	};

	class CEX_IMPORT LuaState
	{
	public:

		LuaState();

		LuaState(const std::string& lib);

		~LuaState();

		lua_State* get();

		///	获取包含在reg库中所有变量名称
		static std::vector<std::string> GetFunctionName( const luaL_reg* reg );

	protected:

		void InitialObject(const std::string& lib);

		void Close();

		lua_State* _L;
	};

	//typedef boost::shared_ptr<LuaState> LuaStateAutoPtr;
	typedef std::auto_ptr<LuaState> LuaStateAutoPtr;

	typedef cex::CDelegate( void(lua_State*) )  LuaRegistLibFunc;

	class CEX_IMPORT CLuaCLibRegister
	{
	public:
		typedef std::map<std::string, LuaRegistLibFunc> FUNC_MAP;

		static CLuaCLibRegister* Instance();

		void AddRegist( const std::string& lib, const LuaRegistLibFunc::FuncType& func );

		void OpenLib( lua_State* L, const std::string& lib );

	private:

		CLuaCLibRegister() {}

		FUNC_MAP _map;
	};

	class CEX_IMPORT CLuaRegistLibProxy
	{
	public:
		CLuaRegistLibProxy(const std::string& spaceName, luaL_reg* reg ) :
		  _spaceName(spaceName), _reg(reg)
		  {
			  assert( reg != NULL );
			  CLuaCLibRegister::Instance()->AddRegist( 
				  spaceName,
				  boost::bind( &CLuaRegistLibProxy::RegistFunction, this, _1 ) );
		  }

		void RegistFunction( lua_State* L )
		{
			luaL_openlib(L, _spaceName.c_str(), _reg, 0);	//	载入自定义库
		}

	protected:
		std::string _spaceName;
		luaL_reg* _reg;

	};

	//void testLua();
}