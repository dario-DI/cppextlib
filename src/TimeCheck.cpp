#include "StdAfx.h"

#include <algorithm>
#include <TDE/TimeCheck.h>


using namespace TDE;


//////////////////////////////////////////////////
//
FuncTimeAsm::FuncTimeAsm(const CString& name)
{
	_assembleTime = 0;
	_onceTimeMax = 0;
	_onceTimeMin = INT_MAX;

	_callNumber = 0;

	//_name = name;
	_name.Format( _T("%60s"), _T("") );
	_name.CopyCharsOverlapped( _name.GetBuffer(), name, name.GetLength() );

	TimeCheckManage::instance()->AddFuncTimeAsm(this);
}

FuncTimeAsm::~FuncTimeAsm()
{
	TimeCheckManage::instance()->WriteFile();
}

void FuncTimeAsm::AddTick(DWORD tick)
{
	_assembleTime += tick;
	if (_onceTimeMax < tick) _onceTimeMax = tick;
	if (_onceTimeMin > tick) _onceTimeMin = tick;

	++_callNumber;
}

void FuncTimeAsm::WriteFile(CArchive& ar)
{
	CString strTime;
	strTime.Format( _T("%d calls, All: %d ms, Max: %d ms, Min: %d ms. "), _callNumber, _assembleTime, _onceTimeMax, _onceTimeMin);

	CString str;
	str.Format( _T("%60s : %s"), _name, strTime);

	ar.WriteString( str );
}

////////////////////////////////////////////////////
//
FuncTimeAsmPxy::FuncTimeAsmPxy( FuncTimeAsm* timeAsm ) : 
_timeAsm(timeAsm)
{
	_onceTimeBegin = GetTickCount();
}

FuncTimeAsmPxy::~FuncTimeAsmPxy()
{ 
	_timeAsm->AddTick( GetTickCount() - _onceTimeBegin );
}


//////////////////////////////////////////////////////////////
//
//void TimeCheckManageExit()
//{
//	TimeCheckManage::instance()->WriteFile();
//}

static CString GetTime()
{
	CTime ttime = CTime::GetCurrentTime();
	//strDate.Format( _T("%d%2d%2d-%2dH-%d2m-%d2s"),
	//	ttime.GetYear(),ttime.GetMonth(),ttime.GetDay(), 
	//	ttime.GetHour(), ttime.GetMinute(), ttime.GetSecond() );

	return  ttime.Format(_T("%Y-%m-%d %H-%M-%S"));
}

TimeCheckManage::TimeCheckManage()
{
}

TimeCheckManage::~TimeCheckManage()
{
	WriteFile();
}

TimeCheckManage* TimeCheckManage::instance()
{
	static TimeCheckManage timeCheckManager;
	return &timeCheckManager;
}

void TimeCheckManage::AddFuncTimeAsm(FuncTimeAsm* timeAsm )
{
	_vtFuncTimeAsm.push_back( timeAsm );
}

bool TimeCheck( FuncTimeAsm* a1, FuncTimeAsm* a2 )
{
	return ((a1->GetAssemble()) > (a2->GetAssemble()));
}

void TimeCheckManage::WriteFile()
{
	//return;

	if ( _vtFuncTimeAsm.size() == 0 ) return;


	CString strZZ( _T("\r\n-------------------------------------------------------------------------------------------------------\r\n") );

	CString strTime = GetTime();

	CFile file( _T("TimeCheck")+strTime+_T(".txt"), CFile::modeCreate|CFile::modeReadWrite );
	CArchive ar(&file, CArchive::store);

	ar.WriteString( strTime );

	std::sort( _vtFuncTimeAsm.begin(), _vtFuncTimeAsm.end(), TimeCheck );

	DWORD lowestTime = _vtFuncTimeAsm.front()->GetAssemble();

	FuncAsmList::iterator it = _vtFuncTimeAsm.begin();

	for ( ; it != _vtFuncTimeAsm.end(); ++it )
	{
		ar.WriteString( strZZ );
		(*it)->WriteFile(ar);

		double rat2Lowest = (*it)->GetAssemble() * 100.0 / lowestTime;

		CString str;
		str.Format( _T("%2d"), (int)rat2Lowest );
		str += _T("%/lowest");

		ar.WriteString( str );
	}

	_vtFuncTimeAsm.clear();
}