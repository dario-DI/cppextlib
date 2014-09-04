/// \file ConfigVar.h Copyright (C) Sharewin Inc.
/// \brief 全局变量配置
///
///
/// \note:
/// \author: DI
/// \time: 2011/8/30 9:46
#ifndef _TDE_CONFIGVAR_
#define _TDE_CONFIGVAR_

#if 0

namespace example_code
{
	/// 变量使用方法

	/// 获取变量
	if ( GET_bool(sw::shader_use) )
	{
		// do something
	}

	if ( GET_int( great::overview_width ) > 256 ) {}

	float fLight1_diffuse = GET_float( light1_diffuse );
	std::string str = GET_string( seismic::label2 );

	/// 界面化修改seismic命名空间下的所有变量
	TDE::ShowDlg::ShowDlgVarEdit( "seismic" );

	/// 设置某一变量的值，并保存到文件
	SET_var( great::overview_width,  150 );
	
	///-------------------------------------------------------

	/// 注册变量方法
	/// 在源文件中，添加命名空间与变量. 使用时 不 需要对文件名引用
	BEGIN_REGIST_VAR_TABLE_V( seismic, 1 )
		REGIST_bool(shader_use, true,			"着色语言开关")
		REGIST_int(overview_width, 256,			"全局视图宽")
		REGIST_string(label,  "字符串",			"提示信息")
	END_REGIST_VAR_TABLE
}

#endif

#pragma once
#include <TDE/Config>

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <TDE/DeltaReflection.h>


#if defined(REGIST_bool) || defined(REGIST_int) || defined(REGIST_float) || defined(REGIST_string) || \
	defined(GET_bool) || defined(GET_int) || defined(GET_float) || defined(GET_string) || \
	defined(SET_var)

#error the macro redefined, please modify these macro

#endif

#define VERSION_LINK_IN_VAR_TABLE_REGIST "@"

#define BEGIN_REGIST_VAR_TABLE_V( domain_name, version ) \
	BEGIN_REGIST_VAR_TABLE(TDE::ConfigDomainType(#domain_name)+"::", TDE::ConfigDomainType(#domain_name)+VERSION_LINK_IN_VAR_TABLE_REGIST+#version )

/// domain_name: 在文件操作ConfigVarRW中，即文件读取、写入时起作用。每一个var_name_space对应一个文件。
/// var_name_space: var_name_space决定了domain_name。当var_name_space是""时，使用GLOBAL_VAR_DOMAIN作为domain_name。
///					当使用版本号时，var_name_space将与版本号一起决定domain_name。
///	var_name:	var_name_space将作与变量融合为一个整体:"var_name_space::var_name"，作为一个变量名字符串使用。
#define BEGIN_REGIST_VAR_TABLE(var_name_space, config_file_name) namespace{ \
	class CRegistVarProxy { public: CRegistVarProxy(); }; \
	static CRegistVarProxy s_CRegistVarProxy; \
	CRegistVarProxy::CRegistVarProxy() { \
	TDE::ConfigDomainType strVarNameSpace = var_name_space; \
	TDE::ConfigDomainType domainName = config_file_name; \
	TDE::IConfigVarRW* pRW = CONFIGVARRW_REGISTER_INS; \
	bool bEnableRegistDomainInfo = !pRW->IsConfigureFileExist( domainName );

#define CONFIGVARRW_REGISTER_INS TDE::DeltaInstance<TDE::IConfigVarRW>()
#define BOOL_VAR_REGISTER_INS TDE::DeltaInstance<TDE::ConfigVarBoolean>()
#define FLOAT_VAR_REGISTER_INS TDE::DeltaInstance<TDE::ConfigVarFloat>()
#define STRING_VAR_REGISTER_INS TDE::DeltaInstance<TDE::ConfigVarString>()

#define VAR_IN_NAME_SPACE( var ) strVarNameSpace+#var

#define	REGIST_bool( var, value, info ) \
	BOOL_VAR_REGISTER_INS->RegistVar( domainName, VAR_IN_NAME_SPACE(var), value, info );

#define	REGIST_int( var, value, info ) \
	FLOAT_VAR_REGISTER_INS->RegistVar( domainName, VAR_IN_NAME_SPACE(var), (int)value, info );

#define	REGIST_float( var, value, info ) \
	FLOAT_VAR_REGISTER_INS->RegistVar( domainName, VAR_IN_NAME_SPACE(var), value, info );

#define	REGIST_string( var, value, info ) \
	STRING_VAR_REGISTER_INS->RegistVar( domainName, VAR_IN_NAME_SPACE(var), value, info );

#define END_REGIST_VAR_TABLE \
	if ( bEnableRegistDomainInfo ) { pRW->SaveVar2File(domainName); } } }

//#define VAR_USE_NAMESPACE(name_space) \
//	static char* __decl_var_name_space() { return #name_space; }
//
//template<typename T>
//std::string __get_var_name_space(const T& valueName)
//{
//	__if_exists( __decl_var_name_space )
//	{
//		return __decl_var_name_space() + "::" + valueName;
//	}
//
//	return valueName;
//}
//
//#define VAR_OUT_NAME_SPACE( var ) __get_var_name_space(#var)
#define VAR_OUT_NAME_SPACE( var ) #var

#define GET_bool( var ) \
	BOOL_VAR_REGISTER_INS->GetVar( VAR_OUT_NAME_SPACE(var) )

#define GET_int( var ) \
	(int)FLOAT_VAR_REGISTER_INS->GetVar( VAR_OUT_NAME_SPACE(var) )

#define GET_float( var ) \
	FLOAT_VAR_REGISTER_INS->GetVar( VAR_OUT_NAME_SPACE(var) )

#define GET_string( var ) \
	STRING_VAR_REGISTER_INS->GetVar( VAR_OUT_NAME_SPACE(var) )

#define  SET_var( var, value ) \
	TDE::VarRegisterUtil::SetVar( #var, value )

////////////////////////////////////////////////////////////////////////////
struct lua_State;

namespace TDE
{
	typedef std::string		ConfigDomainType;
	typedef std::string		ConfigCommentType;
	class IConfigVarRW;

	////////////////////////////////////////////
	//	class ConfigVar
	template<typename DataType>
	class ConfigVar
	{
	public:
		typedef std::string		NameType;
		typedef typename DataType::ValueType	ValueType;
		typedef typename DataType::VarInfo		VarInfo;

		typedef std::map< NameType, ValueType > Var_Map;

		typedef DataType DataType;

		ConfigVar()
		{
			_rw = CONFIGVARRW_REGISTER_INS;
			_rw->RegisterLoadValueFunc(boost::bind( &DataType::LoadValueFunc, _1 ));
		}

		~ConfigVar()
		{
		}

	public:
		void RegistVar( 
			const ConfigDomainType& domain, const NameType& name, 
			const ValueType& value, const ConfigCommentType& comment )
		{
#ifdef _DEBUG
			Var_Map::iterator itrVar = _varMap.find( name );
			ASSERT( itrVar == _varMap.end() );	//	a variable with the same name has been already registered.
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
				AfxMessageBox( CString(varName.c_str()) + _T(": 未注册的变量.") );
#endif
				throw( std::invalid_argument( varName+": can not find." ) );
			}

			return varItr->second;	
		}

		void SetVar( const NameType& name, const ValueType& value )
		{
#ifdef _DEBUG
			Var_Map::iterator itrVar = _varMap.find( name );
			ASSERT( itrVar != _varMap.end() );	//	a variable must has been already registered.
#endif
			_varMap[name] = value;

			_rw->UpdateVarInfo( name, DataType::Value2String(value) );
		}

	protected:

		Var_Map _varMap;
		IConfigVarRW* _rw;
	};	


	////////////////////////////////////////////
	//	class ConfigVarRWByLua
	//	注意：类接口中使用的变量名均包含namespace
	class TDE_IMPORT IConfigVarRW
	{
	public:
		IConfigVarRW() {}
		virtual ~IConfigVarRW()=0 {}

		typedef std::string DomainType;

		typedef boost::function<bool(lua_State*)> LoadValueFunc;
		typedef std::vector<LoadValueFunc> LoadValueFuncList;

		struct VarInfo
		{
			DomainType name;
			DomainType value;
			DomainType comment;

			VarInfo() {}

			VarInfo(const DomainType& n, const DomainType& v, const DomainType& c) :
			name(n), value(v), comment(c)
			{}
		};

		typedef std::vector<VarInfo> VarInfoList;
		typedef std::map<DomainType, VarInfoList > Var_Domain_Map;

		void RegisterLoadValueFunc(const LoadValueFunc& func)
		{
			_LoadValueFuncList.push_back( func );
		}

		const LoadValueFuncList& GetLoadValueFuncList() const 
		{ 
			return _LoadValueFuncList;
		}

		// 注意: 使用前，应使用TryLoadVarFromFile，从文件中获取改变后的变量
		VarInfoList& GetDomianVars(const DomainType& domain) { return _domainMap[domain]; }

		// 注册变量信息
		virtual void RegistDomain( const DomainType& domain, const DomainType& name, const DomainType& value, const DomainType& comment)=0;

		// 更新变量的信息.
		virtual bool UpdateVarInfo( const DomainType& varName, const DomainType& value )=0;

		// 获取含有版本信息的域名
		virtual DomainType GetDomainNameInVersion( const DomainType& nameWithoutVersion )=0;


	public:
		virtual void SaveVar2File(const DomainType& domain)=0;

		virtual void TryLoadVarFromFile()=0;
		virtual void LoadVarFromFile(const DomainType& domain)=0;

		virtual bool IsConfigureFileExist(const DomainType& domain)=0;

	protected:

		LoadValueFuncList _LoadValueFuncList;

		Var_Domain_Map _domainMap;

		bool _bAlreadyLoadFile;

	};

	/////////////////////////////////////////////////////
	//	class ConfigVarTypeBase
	template<typename valueType>
	class ConfigVarTypeBase
	{
	public:
		typedef ConfigVarTypeBase<valueType> ThisType;

		typedef std::string	VarType;
		typedef valueType	ValueType;

		//	变量配置保存的信息
		struct VarInfo
		{
			VarType name;
			ValueType value;
			ConfigCommentType comment;

			VarInfo() : value(0) {}

			VarInfo(const VarType& n, ValueType num, const ConfigCommentType& c ) : 
			name(n), value(num), comment(c) 
			{}

		};
	};

	/// bool型变量
	class TDE_IMPORT ConfigVarBooleanType : public ConfigVarTypeBase<bool>
	{
	public:
		static std::string Value2String(const ValueType& value);
		static bool LoadValueFunc(lua_State* L);
	};

	/// float/int型变量
	class TDE_IMPORT ConfigVarFloatType : public ConfigVarTypeBase<float>
	{
	public:
		static std::string Value2String(const ValueType& value);
		static bool LoadValueFunc(lua_State* L);
	};

	/// string型变量
	class TDE_IMPORT ConfigVarStringType : public ConfigVarTypeBase<std::string>
	{
	public:
		static std::string Value2String(const ValueType& value);
		static bool LoadValueFunc(lua_State* L);
	};

	typedef ConfigVar<ConfigVarBooleanType> ConfigVarBoolean;
	typedef ConfigVar<ConfigVarFloatType> ConfigVarFloat;
	typedef ConfigVar<ConfigVarStringType> ConfigVarString;


	class TDE_IMPORT VarRegisterUtil
	{
	public:
		/// 设置变量值，并保存到文件中
		static void SetVarValueAndSave(const std::string& varName, const std::string& value);

		/// SET_var
		static void SetVar( const std::string& varName, bool v);

		static void SetVar( const std::string& varName, float v);

		static void SetVar( const std::string& varName, int v);

		static void SetVar( const std::string& varName, double v);

		static void SetVar( const std::string& varName, const std::string& v);
	};

}

#endif
