/// \file deltaReflection.h Copyright (C) Sharewin Inc.
/// \brief 
///  any type reflection by string key, or even by unsigned int key(string hash code).
///
/// \note:
/// \author: DI
/// \time: 2011/9/20 11:02
#ifndef _TDE_DELTAREFLECTION_H_
#define _TDE_DELTAREFLECTION_H_

#if 0

// ע�ắ��
{
	void Foo(int k)
	{
		// do something
	}

	// ��cpp�ļ���ע��Foo
	REGIST_DELTA( "MyLib", "Foo", &Foo);
}

// ʹ�ú���
{
	typedef void (*MethodType)(int);

	MethodType func = DeltaCast<MethodType>("MyLib", "Foo");

	func(2);
}

#endif


#pragma once
#include <map>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include <TDE/config>
#include <TDE/MetaProgram.hpp>

// ���е�ע��key�����꣬Լ����DELTA_REGKEY_��ͷ������ͳһ�����鿴
// ����Լ�����ֶ�����ַ���key�����Ѵ�д��ʾ��

#define DELTA_REGKEY_SYS_REGREFPTR "DELTASYS_REGREFPTR"
#ifdef _WIN64
#	define DELTA_REGKEY_SYS_INSTANCE ULONG_PTR(1)
#else
#	define DELTA_REGKEY_SYS_INSTANCE 1U
#endif

namespace TDE
{
	template<typename KeyType_,
		typename ValueType_,
		typename uniqueType_=int>
	class KeyValueRegister
	{
	public:
		typedef KeyType_ KeyType;
		typedef ValueType_ ValueType;

		typedef std::map<KeyType, ValueType> TYPE_MAP;

		void Regist(const KeyType& key, const ValueType& value)
		{
			_map[key]=value;
		}

		void UnRegist(const ValueType& value)
		{
			TYPE_MAP::iterator itr = _map.begin();
			for ( ; itr != _map.end(); ++itr )
			{
				if ( itr->second == value )
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

			//MessageBox(0, key+_T(" KeyValueRegister ���Һ���ʧ��"), _T(""), MB_OK );
			//ASSERT(false);
			//TRACE( "\nDeltaReflection.h: Get(%s) failed: no such key has registered.\n", key );
			throw( std::invalid_argument( "DeltaReflection.h: KeyValueRegister: invalid key." ) );
		}

		typename const ValueType& Get(const KeyType& key) const
		{
			TYPE_MAP::iterator itr = _map.find( key );
			if ( itr != _map.end() )
			{
				return itr->second;
			}

			//ASSERT(false);
			//TRACE( "\nDeltaReflection.h: Get(%s) failed: no such key has registered.\n", key );
			throw( std::invalid_argument( "DeltaReflection.h: KeyValueRegister: invalid key." ) );
		}

		typename TYPE_MAP& Map() { return _map; }

		typename const TYPE_MAP& Map() const { return _map; }

	private:
		TYPE_MAP _map;
	};

	/// delta register
	struct UniqueTypeofDeltaRegister {};
	typedef KeyValueRegister<size_t, boost::any, UniqueTypeofDeltaRegister> UIntDeltaRegister;
	TDE_IMPORT UIntDeltaRegister& UIntDeltaRegisterSingleton();
	typedef KeyValueRegister<std::string, boost::any, UniqueTypeofDeltaRegister> StringDeltaRegister;
	TDE_IMPORT StringDeltaRegister& StringDeltaRegisterSingleton();

	namespace df 
	{
		// common type(char*, wchar_t*, std::string)
		template<typename T>
		struct TTypeTraits
		{
			typedef std::string KeyType;
			typedef StringDeltaRegister RegType;
			static RegType& Instance() { return StringDeltaRegisterSingleton(); }

			static std::string Convert(const std::string& key)
			{
				return key;
			}

			static std::string Convert(const char* key)
			{
				return std::string(key);
			}

			static std::string Convert(const wchar_t* key)
			{
				return std::string(CStringA(key).GetBuffer());
			}
		};

		// unsigned int type
		template<>
		struct TTypeTraits<size_t>
		{
			typedef size_t KeyType;
			typedef UIntDeltaRegister RegType;
			static RegType& Instance() { return UIntDeltaRegisterSingleton(); }

			static size_t Convert(size_t key)
			{
				return key;
			}
		};

		// int type
		template<>
		struct TTypeTraits<int>
		{
			typedef size_t KeyType;
			typedef UIntDeltaRegister RegType;
			static RegType& Instance() { return UIntDeltaRegisterSingleton(); }

			static size_t Convert(int key)
			{
				return key;
			}
		};

		// other supported key type(compatible with type of size_t or string)
		// ...

		// template cast
		template<typename DeltaType, typename KeyType>
			DeltaType TGetRegValue(
			KeyValueRegister<KeyType, boost::any, UniqueTypeofDeltaRegister>& reg,
			const KeyType& key)
		{
			boost::any& r = reg.Get(key);

			try
			{
				return boost::any_cast<DeltaType>(r);
			}
			catch (std::exception e)
			{
				//TRACE( "\nDeltaReflection.h: DeltaCast(%s) failed: type cast failed.\n", libAndValueName);
				//ASSERT(false);
				//throw(e);
				throw( std::invalid_argument( "invalid key" ) );
				//return DeltaType();
			}
		}

		template<typename DeltaType, typename KeyType>
			DeltaType* TGetRegValuePTR(
			KeyValueRegister<KeyType, boost::any, UniqueTypeofDeltaRegister>& reg,
			const KeyType& key)
		{
			boost::any& r = reg.Get(key);

			DeltaType* v = boost::any_cast<DeltaType>(&r);

			if (v != NULL)
			{
				return v;
			}
			else
			{
				throw( std::invalid_argument( "invalid key" ) );
			}
		}
	}

	template<typename DeltaType, typename KeyType>
		DeltaType DeltaCast(const KeyType& key)
	{
		return df::TGetRegValue<DeltaType>(df::TTypeTraits<KeyType>::Instance(), 
			df::TTypeTraits<KeyType>::Convert(key));
	}

	template<typename DeltaType, typename KeyType>
	DeltaType* DeltaPTRCast(const KeyType& key)
	{
		return df::TGetRegValuePTR<DeltaType>(df::TTypeTraits<KeyType>::Instance(),
			df::TTypeTraits<KeyType>::Convert(key));
	}

	template<typename DeltaType, typename libKeyType, typename valueKeyType>
	DeltaType DeltaCast(const libKeyType& libKey, const valueKeyType& valueKey)
	{
		typedef df::TTypeTraits<valueKeyType>::RegType RegType;

		RegType* valueReg = DeltaPTRCast<RegType>(libKey);

		if (valueReg==NULL)
		{ 
			throw( std::invalid_argument( "invalid lib key." ) );
		}

		return df::TGetRegValue<DeltaType>(*valueReg, 
			df::TTypeTraits<valueKeyType>::Convert(valueKey));
	}

	template<typename DeltaType, typename libKeyType, typename valueKeyType>
	DeltaType* DeltaPTRCast(const libKeyType& libKey, const valueKeyType& valueKey)
	{
		typedef df::TTypeTraits<valueKeyType>::RegType RegType;

		RegType* valueReg = DeltaPTRCast<RegType>(libKey);

		if (valueReg==NULL)
		{ 
			throw( std::invalid_argument( "invalid lib key." ) );
		}

		return df::TGetRegValuePTR<DeltaType>(*valueReg,
			df::TTypeTraits<valueKeyType>::Convert(valueKey));
	}

	namespace reg
	{
		template<typename DeltaType, typename KeyType>
		void Register(const KeyType& key, const DeltaType& value)
		{
			typedef df::TTypeTraits<KeyType> Traits;
#ifdef _DEBUG
			try
			{
				Traits::Instance().Get(Traits::Convert(key));
				ASSERT(false); // the same named value has already registered.
				return;
			}
			catch(...)
			{
			}
#endif
			Traits::Instance().Regist(Traits::Convert(key), value);
		}

		template<typename DeltaType, typename libKeyType, typename valueKeyType>
		void Register(const libKeyType& libKey, const valueKeyType& valueKey, const DeltaType& value)
		{
			typedef df::TTypeTraits<libKeyType> LibTraits;
			typedef df::TTypeTraits<valueKeyType> ValueTraits;

			// find library register
			ValueTraits::RegType* reg = NULL;
			try
			{
				reg = DeltaPTRCast<ValueTraits::RegType>(LibTraits::Convert(libKey));
			}
			catch(std::exception e)
			{
				// no register for this lib key, just register
				LibTraits::Instance().Regist(LibTraits::Convert(libKey), ValueTraits::RegType());
			}

			reg = DeltaPTRCast<ValueTraits::RegType>(LibTraits::Convert(libKey));
			ASSERT(reg != NULL);

			// register value to the lib register
#ifdef _DEBUG
			try
			{
				ValueTraits::RegType::ValueType& r = reg->Get(ValueTraits::Convert(valueKey));
				ASSERT(false); // the same named value has already registered.
				return;
			}
			catch(...)
			{
			}
#endif
			reg->Regist(ValueTraits::Convert(valueKey), value);
		}
	}

	class DeltaRegisterProxy
	{
	public:
		template<typename DeltaType, typename KeyType>
			DeltaRegisterProxy(const KeyType& key, const DeltaType& value)
		{
			reg::Register(key, value);
		}

		template<typename DeltaType, typename LibKeyType, typename ValueKeyType>
			DeltaRegisterProxy(const LibKeyType& libKey, const ValueKeyType& valueKey, const DeltaType& value)
		{
			reg::Register(libKey, valueKey, value);
		}
	};

	class DeltaRegisterRefPtrProxy
	{
	public:
		typedef std::vector<boost::any> DeltaSysRefPtrContainer;

		DeltaSysRefPtrContainer* getOrCreateDeltaSysRefPtrContainer()
		{
			try
			{
				return DeltaPTRCast<DeltaSysRefPtrContainer>(DELTA_REGKEY_SYS_REGREFPTR);
			}
			catch(std::exception e)
			{
				// no register for this lib key, just register
				StringDeltaRegisterSingleton().Regist(DELTA_REGKEY_SYS_REGREFPTR, DeltaSysRefPtrContainer());
				return DeltaPTRCast<DeltaSysRefPtrContainer>(DELTA_REGKEY_SYS_REGREFPTR);
			}
		}

		bool IsExisted(boost::any& v)
		{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
			DeltaSysRefPtrContainer* container = getOrCreateDeltaSysRefPtrContainer();
			DeltaSysRefPtrContainer::iterator itr =container->begin();

			boost::any::holder<char*>* ptrv = reinterpret_cast<
				boost::any::holder<char*>*>(v.content);

			for (; itr!=container->end(); ++itr)
			{
				boost::any::holder<char*>* ptr2 = reinterpret_cast<
					boost::any::holder<char*>*>(itr->content);

				if (ptrv->held == ptr2->held)
				{
					return true;
				}
			}

			return false;
#else
			return false;
#endif
		}

		// ref ptr
		template<typename DeltaType, typename KeyType>
			DeltaRegisterRefPtrProxy(const KeyType& key, DeltaType* value)
		{
			reg::Register(key, value);

			boost::any anyValue = boost::shared_ptr<DeltaType>(value);

#ifdef _DEBUG
			if (IsExisted(anyValue))
			{
				ASSERT(false); // same pointer register. it will issues exception at the end of program.
			}
#endif

			getOrCreateDeltaSysRefPtrContainer()->push_back(anyValue);
		}

		// ref ptr
		template<typename DeltaType, typename LibKeyType, typename ValueKeyType>
			DeltaRegisterRefPtrProxy(const LibKeyType& libKey, const ValueKeyType& valueKey, DeltaType* value)
		{
			reg::Register(libKey, valueKey, value);

			boost::any anyValue = boost::shared_ptr<DeltaType>(value);

#ifdef _DEBUG
			if (IsExisted(anyValue))
			{
				ASSERT(false); // same pointer register. it will issues exception at the end of program.
			}
#endif

			getOrCreateDeltaSysRefPtrContainer()->push_back(anyValue);
		}
	};

#define REGIST_DELTA2(key, value) \
	static TDE::DeltaRegisterProxy MT_SEQUENCE_NAME(key, value);

#define REGIST_DELTA(libKey, valueKey, value) \
	static TDE::DeltaRegisterProxy MT_SEQUENCE_NAME(libKey, valueKey, value);

#define REGIST_DELTA_REFPTR2(key, value) \
	static TDE::DeltaRegisterRefPtrProxy MT_SEQUENCE_NAME(key, value);

#define REGIST_DELTA_REFPTR(libKey, valueKey, value) \
	static TDE::DeltaRegisterRefPtrProxy MT_SEQUENCE_NAME(libKey, valueKey, value);

#define REGIST_DELTA_INSTANCE(IType, RType) \
	REGIST_DELTA_REFPTR(DELTA_REGKEY_SYS_INSTANCE, typeid(IType*).name(), (IType*)new RType())

	template<typename T>
		T* DeltaInstance()
	{
		return DeltaCast<T*>(DELTA_REGKEY_SYS_INSTANCE, typeid(T*).name());
	}

}

#endif