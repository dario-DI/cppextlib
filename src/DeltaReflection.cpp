#include "stdAfx.h"
#include <loki/Singleton.h>
#include <TDE/DeltaReflection.h>

namespace TDE
{
	typedef Loki::SingletonHolder<StringDeltaRegister> StringDeltaRegisterSingletonHolder;
	typedef Loki::SingletonHolder<UIntDeltaRegister> UIntDeltaRegisterSingletonHolder;

	StringDeltaRegister& StringDeltaRegisterSingleton()
	{
		return StringDeltaRegisterSingletonHolder::Instance();
	}

	UIntDeltaRegister& UIntDeltaRegisterSingleton()
	{
		return UIntDeltaRegisterSingletonHolder::Instance();
	}


#if 0
	////////////////////////////
	static int s_myInt=1;
	static int* s_ptrInt=&s_myInt;
	REGIST_DELTA2(1000, (int*)s_ptrInt)
	REGIST_DELTA2("INT", (int*)s_ptrInt)
	REGIST_DELTA( 2012, 2012, (int*)s_ptrInt)
	REGIST_DELTA( "INTSPACE1", 2012, (int*)s_ptrInt)
	REGIST_DELTA( "INTSPACE2", "myIntPtr", (int*)s_ptrInt)
	REGIST_DELTA2( _T("INT2"), (int*)s_ptrInt)
	REGIST_DELTA( _T("INTSPACE1"), 2013, (int*)s_ptrInt)
	REGIST_DELTA( _T("INTSPACE2"), _T("myIntPtr2"), (int*)s_ptrInt)
	
	int* s_refPtrInt=new int(1);
	REGIST_DELTA_REFPTR2(1001, (int*)s_refPtrInt)
	//REGIST_DELTA_REFPTR2(1002, (int*)s_refPtrInt) //will cause exception at runtime.

	////////////////////////////
	using namespace TDE;
	void Foo(int& n)
	{
		++n;// do something
	}

	REGIST_DELTA2(2013, &Foo) // register with int key. 
	REGIST_DELTA(_T("MyLibInt"), 2013, &Foo)
	REGIST_DELTA(_T("MyLib"), _T("Foo2"), &Foo)
	//REGIST_DELTA("MyLib", "Foo", &Foo) // should has runtime error with same key name

	static void test()
	{
		/////////////////////////////////////
		// test int pointer

		// ref ptr
		{
			int* myIntPtr = DeltaCast<int*>(1001);
			ASSERT( (*myIntPtr) == 1 );
			int** ptr = DeltaPTRCast<int*>(1001);
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>(1000);
			ASSERT( (*myIntPtr) == 1 );
			int** ptr = DeltaPTRCast<int*>(1000);
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>("INT");
			ASSERT( (*myIntPtr) == 2 );
			int** ptr = DeltaPTRCast<int*>("INT");
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>(2012, 2012);
			ASSERT( (*myIntPtr) == 3 );
			int** ptr = DeltaPTRCast<int*>(2012, 2012);
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>("INTSPACE1", 2012);
			ASSERT( (*myIntPtr) == 4 );
			int** ptr = DeltaPTRCast<int*>("INTSPACE1", 2012);
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>("INTSPACE2", "myIntPtr");
			ASSERT( (*myIntPtr) == 5 );
			int** ptr = DeltaPTRCast<int*>("INTSPACE2", "myIntPtr");
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		// MFC
		{
			int* myIntPtr = DeltaCast<int*>(_T("INT"));
			ASSERT( (*myIntPtr) == 6 );
			int** ptr = DeltaPTRCast<int*>(_T("INT"));
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>(_T("INTSPACE1"), 2013);
			ASSERT( (*myIntPtr) == 7 );
			int** ptr = DeltaPTRCast<int*>(_T("INTSPACE1"), 2013);
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		{
			int* myIntPtr = DeltaCast<int*>(_T("INTSPACE2"), _T("myIntPtr2"));
			ASSERT( (*myIntPtr) == 8 );
			int** ptr = DeltaPTRCast<int*>(_T("INTSPACE2"), _T("myIntPtr2"));
			ASSERT((*ptr)==myIntPtr);
			(*myIntPtr)+=1;
		}

		///////////////////////////////////////
		// test method pointer
		typedef void (*MethodType)(int&);

		int testNum=0;

		{
			MethodType method = DeltaCast<MethodType>(2013);
			method(testNum);
			ASSERT(testNum==1);
		}
		
		{
			MethodType method = DeltaCast<MethodType>(_T("MyLibInt"), 2013);
			method(testNum);
			ASSERT(testNum==2);
		}

		{
			MethodType method = DeltaCast<MethodType>(_T("MyLib"), _T("Foo2"));
			method(testNum);
			ASSERT(testNum==3);
		}
	}

	class UnitTester
	{
	public:
		UnitTester()
		{
			test();
		}
	};

	static UnitTester tester;
#endif 
}