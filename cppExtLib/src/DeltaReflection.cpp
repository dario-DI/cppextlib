#include "stdAfx.h"
#include <loki/Singleton.h>
#include <cex/DeltaReflection.h>

namespace cex
{
	typedef Loki::SingletonHolder<StringDeltaRegister> StringDeltaRegisterSingletonHolder;
	typedef Loki::SingletonHolder<UIntDeltaRegister> UIntDeltaRegisterSingletonHolder;

	StringDeltaRegister& APIENTRY StringDeltaRegisterSingleton()
	{
		return StringDeltaRegisterSingletonHolder::Instance();
	}

	UIntDeltaRegister& APIENTRY UIntDeltaRegisterSingleton()
	{
		return UIntDeltaRegisterSingletonHolder::Instance();
	}
}