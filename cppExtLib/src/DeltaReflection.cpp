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

#pragma region __DeltaRefPtrContainer

	static DeltaRefPtrContainer::SysRefPtrContainerType& getOrCreateContainer()
	{
		DeltaRefPtrContainer::SysRefPtrContainerType* ptr = nullptr;

		try
		{
			ptr = DeltaPTRCast<DeltaRefPtrContainer::SysRefPtrContainerType>(DELTA_REGKEY_SYS_REGREFPTR);
		}
		catch (std::exception e)
		{
			reg::Register(DELTA_REGKEY_SYS_REGREFPTR, DeltaRefPtrContainer::SysRefPtrContainerType());
			ptr = DeltaPTRCast<DeltaRefPtrContainer::SysRefPtrContainerType>(DELTA_REGKEY_SYS_REGREFPTR);
		}

		assert(ptr!=nullptr);

		return *ptr;
	}

	// class DeltaRefPtrContainer
	bool DeltaRefPtrContainer::IsExisted(Interface* obj)
	{
		SysRefPtrContainerType& container = getOrCreateContainer();
		for (auto v : container)
		{
			if (obj == v.get())
			{
				return true;
			}
		}

		return false;
	}

	Interface* DeltaRefPtrContainer::addObject(std::shared_ptr<Interface> obj)
	{
		getOrCreateContainer().push_back(obj);
		return obj.get();
	}

	void DeltaRefPtrContainer::removeObject(Interface* obj)
	{
		SysRefPtrContainerType& container = getOrCreateContainer();

		container.erase(
			std::remove_if(container.begin(), container.end(), 
			[&obj](const std::shared_ptr<Interface>& v)
		{
			if (v.get()==obj) return true;
			else return false;
		}));
	}

#pragma endregion DeltaRefPtrContainer

}

