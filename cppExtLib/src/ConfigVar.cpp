#include <windows.h>

#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <tchar.h>
#include <iostream>
#include <sstream>

#include <cex/ConfigVar.h>
#include <cex/LuaEx.h>

/////////////////////////////////////

//		//	unicode�ļ�ͷ
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

using namespace cex;

#define CONFIG_FILE_EX ".ini"

namespace cex
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

	//LUA_BEGIN_REGIST_LIB( ConfigVarLib )
	//	LUA_REGIST_FUNC( LuaC_RegistGloable )
	//LUA_END_REGIST_OPEN_LIB( "ConfigVarLib", ConfigVarLib )

}

////////////////////////////////////////////////////////////////////////
//	class ConfigVarRWByLua
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

static std::string GetFilePath( const std::string& fileName )
{
	// fileName�������ļ�����׺������·��
	int nDot = fileName.find('.');
	if ( nDot < 0 ) return fileName;

	// fileName�������ļ�����׺
	for (int i=nDot; i>=0; --i)
	{
		if (fileName.at(i) == '\\' || fileName.at(i) == '/' )
		{
			return fileName.substr(0, i);
		}
	}

	return fileName;
}

#define CONFIGURE_DIR GetWorkPath(NULL) + "Configure\\"

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
//	ע�⣺��ӿ���ʹ�õı�����������namespace
class ConfigVarRWByLua : public IConfigVarRW
{
public:
	ConfigVarRWByLua();
	virtual ~ConfigVarRWByLua();

public:

	void RegisterLoadValueFunc(const LoadValueFunc& func)
	{
		_LoadValueFuncList.push_back( func );
	}

	const LoadValueFuncList& GetLoadValueFuncList() const 
	{ 
		return _LoadValueFuncList;
	}

	// ע��: ʹ��ǰ��Ӧʹ��TryLoadVarFromFile�����ļ��л�ȡ�ı��ı���
	VarInfoList& GetDomianVars(const DomainType& domain) { return _domainMap[domain]; }
public:

	// ע�������Ϣ
	virtual void RegistDomain( const DomainType& domain, const DomainType& name, const DomainType& value, const DomainType& comment);

	// ���±�������Ϣ.
	virtual bool UpdateVarInfo( const DomainType& varName, const DomainType& value );

	// ��ȡ���а汾��Ϣ������
	virtual DomainType GetDomainNameInVersion( const DomainType& nameWithoutVersion );


public:
	virtual void SaveVar2File(const DomainType& domain);

	virtual void TryLoadVarFromFile();
	virtual void LoadVarFromFile(const DomainType& domain);

	virtual bool IsConfigureFileExist(const DomainType& domain);

protected:

	LoadValueFuncList _LoadValueFuncList;

	Var_Domain_Map _domainMap;

	bool _bAlreadyLoadFile;
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

#ifdef _DEBUG // �鿴�Ƿ��Ѿ�����ͬ���Ƶı������������ֵ����
		VarInfoList::iterator itr = infoList.begin();
		for ( ; itr != infoList.end(); ++itr )
		{
			if ( itr->name == var.name ) { assert(false); break; }
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
		return false;// ��������ʧ��
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

	assert(false);
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
			assert( false );
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
		MessageBox( 0, _T("�����ļ����ɴ���"), _T("error"), 0 );
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

	if ( cex::LuaEx::DoFile( L, fileName.data() ) != 0 )
	{
		//assert( false );	// to see lua_error.txt
		return;
	}

	const char* s_luaReadGloable = "for i, v in pairs(LUA_TABLE) do \n" 
							"    ConfigVarLib.LuaC_RegistGloable( \"NAME_SPACE\", i, v ) \n"
							 "end \n";

	std::string luaTableName = detail::RemoveVersionString( domain );
	std::string nameSpace = luaTableName + "::";

	std::string strTable( luaTableName.c_str() );
	std::string strNameSpace( nameSpace.c_str() );
	std::string strCode(s_luaReadGloable);

	std::ostringstream oss;
	oss << "for i, v in pairs(";
	oss << strTable.c_str() << ") do \n";
	oss << "    ConfigVarLib.LuaC_RegistGloable( \"";
	oss << strNameSpace.c_str() << "\", i, v ) \n";
	oss << "end \n";

	std::string code = oss.str();

	if ( cex::LuaEx::DoString( L, code.c_str() ) != 0 )
	{
		//assert( false );	// to see lua_error.txt
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

	char* name_space = (char*)lua_tostring(L, -3);
	char* name = (char*)lua_tostring(L, -2);
	bool number = lua_toboolean(L,-1)>0;

	BOOL_VAR_REGISTER_INS->SetVar( std::string(name_space)+name, number );

	return true;
}

////////////////////////////////////
// class ConfigVarFloatType
std::string ConfigVarFloatType::Value2String(const ValueType& value)
{
	return boost::lexical_cast<std::string>(value);
}

bool ConfigVarFloatType::LoadValueFunc(lua_State* L)
{
	if ( ! lua_isnumber(L, -1) )
	{
		return false;
	}

	//luaL_checktype(L, -2, LUA_TSTRING );
	//luaL_checktype(L, -3, LUA_TSTRING );

	char* name_space = (char*)lua_tostring(L, -3);
	char* name = (char*)lua_tostring(L, -2);
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

	char* name_space = (char*)lua_tostring(L, -3);
	char* name = (char*)lua_tostring(L, -2);
	char* number = (char*)lua_tostring(L, -1);

	STRING_VAR_REGISTER_INS->SetVar( std::string(name_space)+name, number );

	return true;
}

///////////////////////////////////
//	ConfigVarInstanceFactory

static void SetVarValue2RW(const std::string& varName, const std::string& value)
{
	bool bRet = CONFIGVARRW_REGISTER_INS->UpdateVarInfo(varName, value);
	if ( bRet == false )
	{
		assert(false);
		return;
	}

	ConfigVarRWByLua::DomainType domain = detail::GetVarNameSpace(varName);
	domain = CONFIGVARRW_REGISTER_INS->GetDomainNameInVersion(domain);

	CONFIGVARRW_REGISTER_INS->SaveVar2File(domain);
	CONFIGVARRW_REGISTER_INS->LoadVarFromFile(domain);

}

void VarRegisterUtil::SetVar( const std::string& varName, bool v )
{
	BOOL_VAR_REGISTER_INS->SetVarPure(varName, v);
	SetVarValue2RW(varName, cex::ConfigVarBooleanType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, float v )
{
	FLOAT_VAR_REGISTER_INS->SetVarPure(varName, v);
	SetVarValue2RW(varName, cex::ConfigVarFloatType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, int v )
{
	FLOAT_VAR_REGISTER_INS->SetVarPure(varName, (float)v);
	SetVarValue2RW(varName, cex::ConfigVarFloatType::Value2String((float)v));
}

void VarRegisterUtil::SetVar( const std::string& varName, double v )
{
	FLOAT_VAR_REGISTER_INS->SetVarPure(varName, (float)v);
	SetVarValue2RW(varName, cex::ConfigVarFloatType::Value2String((float)v));
}

void VarRegisterUtil::SetVar( const std::string& varName, const char* v )
{
	STRING_VAR_REGISTER_INS->SetVarPure(varName, std::string(v));
	SetVarValue2RW(varName, cex::ConfigVarStringType::Value2String(v));
}

void VarRegisterUtil::SetVar( const std::string& varName, const std::string& v )
{
	STRING_VAR_REGISTER_INS->SetVarPure(varName, v);
	SetVarValue2RW(varName, cex::ConfigVarStringType::Value2String(v));
}

#pragma region configVar

////////////////////////////////////////////
//	class ConfigVar
template<typename DataType>
class ConfigVar : public IConfigVar<DataType>
{
public:
	ConfigVar()
	{
		_rw = CONFIGVARRW_REGISTER_INS;
		_rw->RegisterLoadValueFunc(boost::bind( &DataType::LoadValueFunc, _1 ));
	}

public:
	void RegistVar( 
		const ConfigDomainType& domain, const NameType& name, 
		const ValueType& value, const ConfigCommentType& comment )
	{
#ifdef _DEBUG
		Var_Map::iterator itrVar = _varMap.find( name );
		assert( itrVar == _varMap.end() );	//	a variable with the same name has been already registered.
#endif

		_varMap[name] = value;

		_rw->RegistDomain( domain, name, DataType::Value2String(value), comment );
	}

	ValueType GetVar( const NameType& varName )
	{
		_rw->TryLoadVarFromFile();

		Var_Map::const_iterator varItr = _varMap.find( varName );

		if ( varItr == _varMap.end() )
		{
#ifdef _DEBUG
			std::cout<< varName.c_str() << ": δע��ı���." << std::endl;
			assert(false);
#endif
			throw( std::invalid_argument( varName+": can not find." ) );
		}

		return varItr->second;	
	}

	void SetVar( const NameType& name, const ValueType& value )
	{
		SetVarPure(name, value);

		_rw->UpdateVarInfo( name, DataType::Value2String(value) );
	}

	void SetVarPure( const NameType& name, const ValueType& value )
	{
#ifdef _DEBUG
		Var_Map::iterator itrVar = _varMap.find( name );
		assert( itrVar != _varMap.end() );	//	a variable must has been already registered.
#endif
		_varMap[name] = value;
	}

protected:

	Var_Map _varMap;
	IConfigVarRW* _rw;
};	

#pragma endregion

typedef ConfigVar<ConfigVarBooleanType> RConfigVarBoolean;
typedef ConfigVar<ConfigVarFloatType> RConfigVarFloat;
typedef ConfigVar<ConfigVarStringType> RConfigVarString;

REGIST_DELTA_INSTANCE(IConfigVarRW, ConfigVarRWByLua)
REGIST_DELTA_INSTANCE(ConfigVarBoolean, RConfigVarBoolean)
REGIST_DELTA_INSTANCE(ConfigVarFloat, RConfigVarFloat)
REGIST_DELTA_INSTANCE(ConfigVarString, RConfigVarString)