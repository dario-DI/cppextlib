// UnitTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#define USING_CEX_LIB "../debug/cppExtLib"

#include <cex/config>
#include <cex/UnTest.h>

int _tmain(int argc, _TCHAR* argv[])
{
	CEX_RUN_ALL_TESTS();

	//CEX_RUN_TEST("DeltaReflectTest")
	//CEX_RUN_TEST("MessageMapTest")

	system("pause");

	return 0;
}

