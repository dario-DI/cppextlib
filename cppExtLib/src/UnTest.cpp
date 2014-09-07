#include "stdAfx.h"
#include <cex/UnTest.h>

namespace cex
{
	class UTRegister : public IUnitTestMethodRegister
	{
	public:
		~UTRegister() {}

		void addCase(const std::string& caseName, const TestFun& fun)
		{
			_vtCase.push_back(caseMethod(caseName, fun));
		}

		void run()
		{
			for each (auto var in _vtCase)
			{
				std::cout<<"------------------------------------------"<<std::endl;
				std::cout<<"start case: "<< var.caseName << std::endl;

				if (var.method != NULL)
				{
					var.method();
				}

				std::cout<<"the end of case: "<< var.caseName << std::endl;
				std::cout<<"------------------------------------------"<<std::endl;
			}
		}

	private:
		struct caseMethod
		{
			std::string caseName;
			TestFun method;

			caseMethod(const std::string& name, const TestFun& m)
			{
				caseName = name;
				method = m;
			}
		};

		std::vector<caseMethod> _vtCase;
	};

	REGIST_DELTA_INSTANCE(IUnitTestMethodRegister, UTRegister);
}