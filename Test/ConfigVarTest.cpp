#include "stdafx.h"
#include <cex/UnTest.h>
#include <cex/ConfigVar.h>

BEGIN_REGIST_VAR_TABLE_V( d3, 1 )
	REGIST_bool(abc, true,			"abc")
	REGIST_int(abc2, 256,			"abc2")
	REGIST_string(abc3,  "abc3",	"abc3")
END_REGIST_VAR_TABLE

CEX_TEST(ConfigVarTest)
{
	bool t = GET_bool(d3::abc);
	CEX_ASSERT_TRUE(t==true);

	int n = GET_int(d3::abc2);
	CEX_ASSERT_TRUE(n==256);

	std::string s = GET_string(d3::abc3);
	CEX_ASSERT_TRUE(s=="abc3");

	// set
	SET_var(d3::abc, false);
	t = GET_bool(d3::abc);
	CEX_ASSERT_TRUE(t==false);

	SET_var(d3::abc2, 0);
	n = GET_int(d3::abc2);
	CEX_ASSERT_TRUE(n==0);

	SET_var(d3::abc3, "str");
	s = GET_string(d3::abc3);
	CEX_ASSERT_TRUE(s=="str");

}