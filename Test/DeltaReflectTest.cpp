#include "stdafx.h"

#include <cex/DeltaReflection.h>
#include <cex/UnTest.h>

using namespace cex;

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

class Test0 : public Interface
{
};

class Test1 : public Test0
{
public:
	int v;
	Test1() : v(0) {}
};

REGIST_DELTA_CREATOR(Test0, Test1)

class Test2 : public Interface
{
public:
	int v;
	Test2() : v(0) {}
};

REGIST_DELTA_INSTANCE(Test2, Test2);


void Foo(int& n)
{
	++n;// do something
}

REGIST_DELTA2(2013, &Foo) // register with int key. 
REGIST_DELTA(_T("MyLibInt"), 2013, &Foo)
REGIST_DELTA(_T("MyLib"), _T("Foo2"), &Foo)
//REGIST_DELTA("MyLib", "Foo", &Foo) // should has runtime error with same key name

CEX_TEST(DeltaReflectTest)
{
	{
		int* myIntPtr = DeltaCast<int*>(1000);
		assert( (*myIntPtr) == 1 );
		int** ptr = DeltaPTRCast<int*>(1000);
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>("INT");
		assert( (*myIntPtr) == 2 );
		int** ptr = DeltaPTRCast<int*>("INT");
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>(2012, 2012);
		assert( (*myIntPtr) == 3 );
		int** ptr = DeltaPTRCast<int*>(2012, 2012);
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>("INTSPACE1", 2012);
		assert( (*myIntPtr) == 4 );
		int** ptr = DeltaPTRCast<int*>("INTSPACE1", 2012);
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>("INTSPACE2", "myIntPtr");
		assert( (*myIntPtr) == 5 );
		int** ptr = DeltaPTRCast<int*>("INTSPACE2", "myIntPtr");
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	// MFC
	{
		int* myIntPtr = DeltaCast<int*>(_T("INT"));
		assert( (*myIntPtr) == 6 );
		int** ptr = DeltaPTRCast<int*>(_T("INT"));
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>(_T("INTSPACE1"), 2013);
		assert( (*myIntPtr) == 7 );
		int** ptr = DeltaPTRCast<int*>(_T("INTSPACE1"), 2013);
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	{
		int* myIntPtr = DeltaCast<int*>(_T("INTSPACE2"), _T("myIntPtr2"));
		assert( (*myIntPtr) == 8 );
		int** ptr = DeltaPTRCast<int*>(_T("INTSPACE2"), _T("myIntPtr2"));
		assert((*ptr)==myIntPtr);
		(*myIntPtr)+=1;
	}

	///////////////////////////////////////
	// test method pointer
	typedef void (*MethodType)(int&);

	int testNum=0;

	{
		MethodType method = DeltaCast<MethodType>(2013);
		method(testNum);
		assert(testNum==1);
	}

	{
		MethodType method = DeltaCast<MethodType>(_T("MyLibInt"), 2013);
		method(testNum);
		assert(testNum==2);
	}

	{
		MethodType method = DeltaCast<MethodType>(_T("MyLib"), _T("Foo2"));
		method(testNum);
		assert(testNum==3);
	}

	// test creator
	{
		Test0* test0 = DeltaCreate<Test0>();
		Test1* test1 = dynamic_cast<Test1*>(test0);
		assert(test1!=nullptr);
		test1->v = 9;

		Test0* test01 = DeltaCreate<Test0>();
		test1 = dynamic_cast<Test1*>(test01);
		assert(test1!=nullptr);
		test1->v = 8;

		DeltaDestory(test0);
		DeltaDestory(test01);

		// test ref create
		boost::shared_ptr<Test0> test00;
		DeltaCreateRef(test00);
		test1 = dynamic_cast<Test1*>(test00.get());
		assert(test1!=nullptr);

		boost::shared_ptr<Test0> test001;
		DeltaCreateRef(test001);
		test1 = dynamic_cast<Test1*>(test001.get());
		assert(test1!=nullptr);


		// test instance
		Test2* test2 = DeltaInstance<Test2>();
		assert(test2!=nullptr);
		test2->v = 2;
	}
	
	

	
}