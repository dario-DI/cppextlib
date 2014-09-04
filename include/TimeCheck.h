/// \file TimeCheck.h Copyright (C) Sharewin Inc.
/// \brief ��ʱͳ�ƹ���
///
///
/// \note:
/// \author: DI
/// \time: 2010-12-9

#if 0

/**
����(������)�����ʱͳ��

������TIMECHECK, TIMECHECK1

˵����
��TIMECHECK�������ڵĺ�����,
����رպ�TimeCheck.txt ������ʱ���ٵ��Ⱥ�����б���ʾ���к�����

ע�⣺��ʱͳ�ƽ���_DEBUG����û�ж���DISABLE_TIME_CHECKʱ����Ч��
*/

//#define DISABLE_TIME_CHECK // ȡ�����Ժ�
//#define TIME_CHECK_ALLWAYS // ����ʹ�ò��Ժ�
#include <TDE/TimeCheck.h>

void fun( para )
{
	TIMECHECK;
	
	// your code
	...
}

//Ҳ������ĳ����������ʹ�ã��ڴ������������ʱ,Ҳ����ӡ���ļ��С�

void fun( para )
{
	for ( ... )
	{
		TIMECHECK;

		for ( ... )
		{
			//	Ϊ���������ӱ�ǩ,��������ͬһ�������в�ͬ��TimeCheck
			TIMECHECK1( -2rdFor );
		}

		// your code
		...
	}	
}

#endif

#pragma once

#include <vector>
#include <TDE/Config>

namespace TDE
{
#ifdef TIMECHECK
#error redefined TIMECHECK
#endif

#if (defined(_DEBUG) && !defined(DISABLE_TIME_CHECK)) || defined(TIME_CHECK_ALLWAYS)

#define TIMECHECK \
	static TDE::FuncTimeAsm timeCheckObjectX( CString(__FUNCTION__) ); \
	TDE::FuncTimeAsmPxy timeCheckObjectXPxy( &timeCheckObjectX ); 
#define TIMECHECK1( name ) \
	static TDE::FuncTimeAsm timeCheckObjectX( CString(__FUNCTION__)+CString("_" #name)); \
	TDE::FuncTimeAsmPxy timeCheckObjectXPxy( &timeCheckObjectX );

#else

#define TIMECHECK
#define TIMECHECK1( name )

#endif

	class FuncTimeAsmPxy;

	class TDE_IMPORT FuncTimeAsm
	{
	public:
		FuncTimeAsm( const CString& name );
		~FuncTimeAsm();
	public:
	
		void WriteFile(CArchive& ar);

		DWORD GetAssemble() { return _assembleTime; }

		void AddTick(DWORD tick);

	private:
		DWORD _assembleTime;

		DWORD _onceTimeMax;

		DWORD _onceTimeMin;

		DWORD _callNumber;

		CString _name;
	};

	class TDE_IMPORT FuncTimeAsmPxy
	{
	public:
		FuncTimeAsmPxy( FuncTimeAsm* timeAsm );

		~FuncTimeAsmPxy();

	private:
		FuncTimeAsm* _timeAsm;

		DWORD _onceTimeBegin;
	};

	class TDE_IMPORT TimeCheckManage
	{
	public:
		~TimeCheckManage();

		static TimeCheckManage* instance();

		void AddFuncTimeAsm( FuncTimeAsm* timeAsm );

		void WriteFile();

	private:
		TimeCheckManage();

		typedef std::vector<FuncTimeAsm*> FuncAsmList;
		FuncAsmList _vtFuncTimeAsm;

	};
}
