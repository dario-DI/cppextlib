#include "StdAfx.h"

#include <TDE/LuaEx.h>
#include <algorithm>
#include <Util/BaseFun.h>

#include <TDE/ConfigVar.h>

/////////////////////////////////////

//		//	unicode文件头
//#ifdef UNICODE
//		//UTL16
///*		char fx[2];
//		fx[0]=(char)0xFF;
//		fx[1]=(char)0xFE
//		file.write(fx,2);*/;
//		////UTL8
//		/*char fx[3];
//		fx[0]=(char)0xEF;
//		fx[1]=(char)0xBB;
//		fx[2]=(char)0xBF;
//		file.write(fx,3);*/
//#endif

using namespace TDE;

#define CONFIG_FILE_EX ".ini"

namespace TDE
{
/////////////////////////////////////////
//	regist functin
	static int LuaC_RegistGloable( lua_State* L )
	{
		luaL_checktype(L, -2, LUA_TSTRING );
		luaL_checktype(L, -3, LUA_TSTRING );

		const IConfigVarRW::LoadValueFuncList& funcList = CONFIGVARRW_REGISTER_INS->GetLoadValueFuncList();

		for ( size_t i=0; i<funcList.size(); ++i)
		{
			if ( funcList[i](L) )
			{
				break;
			}
		}

		return 0;
	}

	LUA_BEGIN_REGIST_LIB( ConfigVarLib )
		LUA_REGIST_FUNC( LuaC_RegistGloable )
	LUA_END_REGIST_OPEN_LIB( "ConfigVarLib", ConfigVarLib )

}

////////////////////////////////////////////////////////////////////////
//	class ConfigVarRWByLua

#define CONFIGURE_DIR Util::GetWorkPath(NULL) + "Configure\\"

namespace detail
{
	bool IsFileExist( const std::string& fileName )
	{
		std::fstream fs;
		fs.open( fileName.c_str(), std::ios::in );

		if ( !fs )
		{
			return false;
		}

		fs.close();
		return true;
	}
}

////////////////////////////////////////////
//	class ConfigVarRWByLua
//	注意：类接口中使用的变量名均包含namespace
class ConfigVarRWByLua : public IConfigVarRW
{
public:
	ConfigVarRWByLua();
	virtual ~ConfigVarRWByLua();

	// 注册变量信息
	virtual void RegistDomain( const DomainType& domain, const DomainType& name, const DomainType& value, const DomainType& comment);

	// 更新变量的信息.
	virtual bool UpdateVarInfo( const DomainType& varName, const DomainType& value );

	// 获取含有版本信息的域名
	virtual DomainType GetDomainNameInVersion( const DomainType& nameWithoutVersion );


public:
	virtual void SaveVar2File(const DomainType& domain);

	virtual void TryLoadVarFromFile();
	virtual void LoadVarFromFile(const DomainType& domain);

	virtual bool IsConfigureFileExist(const DomainType& domain);
};

ConfigVarRWByLua::ConfigVarRWByLua()
{
	_bAlreadyLoadFile = false;

	std::string strDir = CONFIGURE_DIR;

	//	create directory if not
	::CreateDirectoryA( strDir.c_str(), NULL );
}

ConfigVarRWByLua::~ConfigVarRWByLua()
{
}

namespace detail
{
	static ConfigVarRWByLua::DomainType RemoveVarNameSpace( const ConfigVarRWByLua::DomainType& varName )
	{
		std::string::size_type pos = varName.find( "::" );
		if ( pos == std::string::npos ) return varName;

		return varName.substr(pos+2);
	}

	std::string static RemoveVersionString( const std::string& domain)
	{
		std::string::size_type pos = domain.find( VERSION_LINK_IN_VAR_TABLE_REGIST );
		if ( pos == std::string::npos ) return domain;

		return domain.substr(0, pos);
	}

	static ConfigVarRWByLua::DomainType GetVarNameSpace( const ConfigVarRWByLua::DomainType& varName )
	{
		std::string::size_type pos = varName.find( "::" );
		if ( pos == std::string::npos ) return "";

		return varName.substr(0, pos);
	}
}


void ConfigVarRWByLua::RegistDomain( const DomainType& domain, const DomainType& name,	const DomainType& value, const DomainType& comment )
{
	VarInfo var(name, value, comment);

	Var_Domain_Map::iterator itrDomain = _domainMap.find( domain );

	if ( itrDomain == _domainMap.end() )
	{
		VarInfoList varList;
		varList.push_back( var );

		_domainMap[domain] = varList;
	}
	else
	{
		std::vector<VarInfo>& infoList = itrDomain->second;

#ifdef _DEBUG // 查看是否已经有相同名称的变量，如果将其值覆盖
		VarInfoList::iterator itr = infoList.begin();
		for ( ; itr != infoList.end(); ++itr )
		{
			if ( itr->name == var.name ) { ASSERT(false); break; }
		}
#endif

		infoList.push_back( var );

	}
}

bool ConfigVarRWByLua::UpdateVarInfo( const DomainType& varName, const DomainType& value )
{
	ConfigVarRWByLua::DomainType domain = detail::GetVarNameSpace(varName);
	ConfigVarRWByLua::DomainType name = varName;

	domain = GetDomainNameInVersion(domain);

	Var_Domain_Map::iterator itrDomain = _domainMap.find( domain );
	if ( itrDomain == _domainMap.end() )
	{
		return false;// 查找域名失败
	}

	VarInfoList& infoList = itrDomain->second;

	for (size_t i=0; i<infoList.size(); ++i)
	{
		if ( infoList[i].name != name ) continue;

		infoList[i].value = value;
		return true;
	}
	
	return false;
}

bool ConfigVarRWByLua::IsConfigureFileExist(const DomainType& domain)
{
	std::string fileName = CONFIGURE_DIR + domain + CONFIG_FILE_EX;

	return detail::IsFileExist( fileName );
}

ConfigVarRWByLua::DomainType ConfigVarRWByLua::GetDomainNameInVersion( const DomainType& nameWithoutVersion )
{
	DomainType domianName = nameWithoutVersion;

	Var_Domain_Map::iterator itr = _domainMap.begin();
	for ( ; itr!=_domainMap.end(); ++itr )
	{
		DomainType str = detail::RemoveVersionString(itr->first);
		if ( domianName != str ) continue;

		return itr->first;
	}

	ASSERT(false);
	return "";
}

void ConfigVarRWByLua::SaveVar2File(const DomainType& domain)
{
	try
	{
		Var_Domain_Map::iterator itr = _domainMap.find( domain );
		if ( itr == _domainMap.end() ) return;

		std::string fileName = CONFIGURE_DIR + domain + CONFIG_FILE_EX;

		std::ofstream ofs( fileName.c_str() );
		if ( !ofs )
		{
			ASSERT( false );
			return;
		}

		//	wirte head
		ofs << "-- " << domain << " config.";
		ofs << std::endl;
		ofs << "-- You can modify the values, and restart program to make it effective.";
		ofs << std::endl << std::endl;

		std::string strTableName = detail::RemoveVersionString(domain);
		ofs << strTableName << " =" << std::endl;
		ofs << "{" << std::endl;

		VarInfoList::iterator listBeg = itr->second.begin();
		VarInfoList::iterator listEnd = itr->second.end();

		VarInfoList::iterator listItr = listBeg;

		for ( ; listItr != listEnd; ++listItr )
		{
			ofs << "	";			// a tap
			ofs << detail::RemoveVarNameSpace(listItr->name);
			ofs << "=";
			ofs << listItr->value;
			ofs << ";";

			//	wirte comment
			if ( listItr->comment.length() > 0 )
			{
				ofs << "	--";		//	for comment
				ofs << listItr->comment;
			}

			ofs << std::endl;
		}

		//	write tail
		ofs << "}";
		ofs << std::endl;

		ofs.close();
	}
	catch(...)
	{
		MessageBox( 0, _T("配置文件生成错误"), _T("error"), 0 );
	}
}

void ConfigVarRWByLua::TryLoadVarFromFile()
{
	if ( _bAlreadyLoadFile ) return;
	
	Var_Domain_Map::iterator itr = _domainMap.begin();

	for ( ; itr != _domainMap.end(); ++itr )
	{
		LoadVarFromFile( itr->first );
	}

	_bAlreadyLoadFile = true;
}

void ConfigVarRWByLua::LoadVarFromFile(const DomainType& domain)
{
	std::string fileName = CONFIGURE_DIR + domain + CONFIG_FILE_EX;

    LuaStateAutoPtr luaState( new LuaState("ConfigVarLib") );

	lua_State* L = luaState->get();

	if ( TDE::LuaEx::DoFile( L, fileName.data() ) != 0 )
	{
		//ASSERT( false );	// to see lua_error.txt
		return;
	}

	char* s_luaReadGloable = "for i, v in pairs(LUA_TABLE) do \n" 
							"    ConfigVarLib.LuaC_RegistGloable( \"NAME_SPACE\", i, v ) \n"
							 "end \n";

	std::string luaTableName = detail::RemoveVersionString( domain );
	std::string nameSpace = luaTableName + "::";

	CStringA strTable( luaTableName.c_str() );
	CStringA strNameSpace( nameSpace.c_str() );
	CStringA strCode(s_luaReadGloable);

	strCode.Replace( "LUA_TABLE", strTable.GetBuffer(0) );
	strTable.ReleaseBuffer();

	strCode.Replace( "NAME_SPACE", strNameSpace.GetBuffer(0) );
	strNameSpace.ReleaseBuffer();

	std::string code( strCode.GetBuffer(0) );
	strCode.ReleaseBuffer();

	if ( TDE::LuaEx::DoString( L, code.c_str() ) != 0 )
	{
		//ASSERT( false );	// to see lua_error.txt
	}
}

////////////////////////////////////
// class ConfigVarBooleanType
std::string ConfigVarBooleanType::Value2String(const ValueType& value)
{
	std::string strNum = value==true? "true" : "false";

	return strNum;
}

bool ConfigVarBooleanType::LoadValueFunc(lua_State* L)
{
	if ( ! lua_isboolean(L, -1) )
	{
		return false;
	}

	//luaL_checktype(L, -2, LUA_TSTRING );
	//luaL_checktype(L, -3, LUA_TSTRING );

	LPSTR name_space = (LPSTR)lua_tostring(L, -3);
	LPSTR name = (LPSTR)lua_tostring(L, -2);
	bool number = lua_toboolean(L,-1)>0;

	BOOL_VAR_REGISTER_INS->SetVar( std::string(name_space)+name, number );

	return true;
}

////////////////////////////////////
// class ConfigVarFloatType
std::string ConfigVarFloatType::Value2String(const ValueType& value)
{
	CStringA cNum;

#ifndef _use_integer
	int integer = (int)value;
	float fract = value - integer;
	if ( fract == 0.0 )
	{
		cNum.Format( "%.0f", value );
	}
	else
	{
		cNum.Format( "%.3f", value );
	}
#else
	cNum.Format( "%.3f", value );
#endif

	std::string strNum( cNum.GetBuffer(0) );
	cNum.ReleaseBuffer();

	return strNum;
}

bool ConfigVarFloatType::LoadValueFunc(lua_State* L)
{
	if ( ! lua_isnumber(L, -1) )
	{
		return false;
	}

	//luaL_checktype(L, -2, LUA_TSTRING );
	//luaL_checktype(L, -3, LUA_TSTRING );

	LPSTR name_space = (LPSTR)lua_tostring(L, -3);
	LPSTR name = (LPSTR)lua_tostring(L, -2);
	float number = (float)lua_tonumber(L, -1);

	FLOAT_VAR_REGISTER_INS->SetVar( std::string(name_space)+name, number );

	return true;
}

////////////////////////////////////
// class ConfigVarStringType
std::string ConfigVarStringType::Value2String(const ValueType& value)
{
	std::string strNum = "\"";
	strNum += value;
	strNum += "\"";

	return strNum;
}

bool ConfigVarStringType::LoadValueFunc(lua_State* L)
{
	if ( ! lua_isstring(L, -1) )
	{
		return false;
	}

	//luaL_checktype(L, -2, LUA_TSTRING );
	//luaL_checktype(L, -3, LUA_TSTRING );

	LPSTR name_space = (LPSTR)lua_tostring(L, -3);
	LPSTR name = (LPSTR)lua_tostring(L, -2);
	LPSTR number = (LPSTR)lua_tostring(L, -1);

	STRING_VAR_REGISTER_INS->SetVar( std::string(name_space)+name, number );

	return true;
}

///////////////////////////////////
//	ConfigVarInstanceFactory

void VarRegisterUtil::SetVarValueAndSave(const std::string& varName, const std::string& value)
{
	bool bRet = CONFIGVARRW_REGISTER_INS->UpdateVarInfo(varName, value);
	if ( bRet == false )
	{
		ASSERT(false);
		return;
	}

	ConfigVarRWByLua::DomainType domain = detail::GetVarNameSpace(varName);
	domain = CONFIGVARRW_REGISTER_INS->GetDomainNameInVersion(domain);

	CONFIGVARRW_REGISTER_INS->SaveVar2File(domain);
	CONFIGVARRW_REGISTER_INS->LoadVarFromFile(domain);

}

void VarRegisterUtil::SetVar( const std::string& varName, bool v )
{
	SetVarValueAndSave(varName, TDE::ConfigVarBooleanType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, float v )
{
	SetVarValueAndSave(varName, TDE::ConfigVarFloatType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, int v )
{
	SetVarValueAndSave(varName, TDE::ConfigVarFloatType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, double v )
{
	SetVarValueAndSave(varName, TDE::ConfigVarFloatType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, const std::string& v )
{
	SetVarValueAndSave(varName, TDE::ConfigVarStringType::Value2String(v));
}

REGIST_DELTA_INSTANCE(IConfigVarRW, ConfigVarRWByLua)
REGIST_DELTA_INSTANCE(ConfigVarBoolean, ConfigVarBoolean)
REGIST_DELTA_INSTANCE(ConfigVarFloat, ConfigVarFloat)
REGIST_DELTA_INSTANCE(ConfigVarString, ConfigVarString)