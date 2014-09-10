#include "stdAfx.h"
#include <boost/container/detail/Singleton.hpp>
#include <cex/DeltaReflection.h>

namespace cex
{
	typedef boost::container::container_detail::singleton_default<StringDeltaRegister> StringDeltaRegisterSingletonHolder;
	typedef boost::container::container_detail::singleton_default<UIntDeltaRegister> UIntDeltaRegisterSingletonHolder;

	StringDeltaRegister& __stdcall StringDeltaRegisterSingleton()
	{
		return StringDeltaRegisterSingletonHolder::instance();
	}

	UIntDeltaRegister& __stdcall UIntDeltaRegisterSingleton()
	{
		return UIntDeltaRegisterSingletonHolder::instance();
	}
}

