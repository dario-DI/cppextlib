#include <boost/container/detail/Singleton.hpp>
#include <cex/DeltaReflection.h>

namespace cex
{
#pragma region keyValueRegister
	
	template<typename KeyType_,
		typename ValueType_,
		typename uniqueType_=int>
	class KeyValueRegister : public ITKeyValueRegister<KeyType_, ValueType_, uniqueType_>
	{
	public:
		typedef std::map<KeyType, ValueType> TYPE_MAP;

		void Regist(const KeyType& key, const ValueType& value)
		{
			_map[key]=value;
		}

		void UnRegist(const KeyType& key)
		{
			TYPE_MAP::iterator itr = _map.begin();
			for ( ; itr != _map.end(); ++itr )
			{
				if ( itr->first == key )
				{
					_map.erase( itr );
					return;
				}
			}
		}

		typename ValueType& Get(const KeyType& key)
		{
			TYPE_MAP::iterator itr = _map.find( key );
			if ( itr != _map.end() )
			{
				return itr->second;
			}

			//assert(false);
			//std::cout << "DeltaReflection.h: KeyValueRegister: invalid key: " << key << std::endl;
			throw( std::invalid_argument( "DeltaReflection.h: KeyValueRegister: invalid key." ) );
		}

		typename const ValueType& Get(const KeyType& key) const
		{
			TYPE_MAP::const_iterator itr = _map.find( key );
			if ( itr != _map.end() )
			{
				return itr->second;
			}

			//assert(false);
			//std::cout << "DeltaReflection.h: KeyValueRegister: invalid key: " << key << std::endl;
			throw( std::invalid_argument( "DeltaReflection.h: KeyValueRegister: invalid key." ) );
		}

	private:
		TYPE_MAP _map;
	};

	typedef KeyValueRegister<size_t, boost::any, UniqueTypeofDeltaRegister> UIntDeltaRegister;
	typedef KeyValueRegister<std::string, boost::any, UniqueTypeofDeltaRegister> StringDeltaRegister;

	CEX_API IUIntDeltaRegister& __stdcall UIntDeltaRegisterSingleton()
	{
		static UIntDeltaRegister theInstance;

		return theInstance;
	}


	CEX_API IStringDeltaRegister& __stdcall StringDeltaRegisterSingleton()
	{
		static StringDeltaRegister theInstance;

		return theInstance;
	}

	CEX_EXPORT_CLASS  std::shared_ptr<IUIntDeltaRegister> __stdcall createUIntDeltaRegister()
	{
		return std::make_shared<UIntDeltaRegister>();
	}

	CEX_EXPORT_CLASS std::shared_ptr<IStringDeltaRegister> __stdcall createStringDeltaRegister()
	{
		return std::make_shared<StringDeltaRegister>();
	}

	//typedef boost::container::container_detail::singleton_default<StringDeltaRegister> StringDeltaRegisterSingletonHolder;
	//typedef boost::container::container_detail::singleton_default<UIntDeltaRegister> UIntDeltaRegisterSingletonHolder;

#pragma endregion keyValueRegister

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

