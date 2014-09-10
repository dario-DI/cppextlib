#include <cex/UnTest.h>
#include <iostream>

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
				std::cout<<"Start case: "<< var.caseName << std::endl;

				if (var.method != NULL)
				{
					var.method();
				}

				std::cout<<"The end of case: "<< var.caseName << std::endl;
			}
		}

		void run(const std::string& caseName)
		{
			std::cout<<"------------------------------------------"<<std::endl;
			std::cout<<"Start case: "<< caseName << std::endl;

			for (size_t i=0; i < _vtCase.size(); ++i)
			{
				if (_vtCase[i].caseName == caseName)
				{
					if (_vtCase[i].method != NULL)
					{
						_vtCase[i].method();
					}

					break;
				}
			}

			std::cout<<"The end of case: "<< caseName << std::endl;
		}

		void run(size_t index)
		{
			if (index >= _vtCase.size())
			{
				return;
			}

			std::cout<<"------------------------------------------"<<std::endl;
			std::cout<<"Start case: "<< index << std::endl;

			if (_vtCase[index].method != NULL)
			{
				_vtCase[index].method();
			}

			std::cout<<"The end of case: "<< index << std::endl;
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