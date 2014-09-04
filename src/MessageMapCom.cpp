#include "stdAfx.h"
#include <TDE\MessageMapCom.h>

void ShutMessageMapCom()
{
	MessageMapCom::MessageRegist::instance()->ClearMessageMap();
}

namespace MessageMapCom
{
	int RecurciveLimit::LIMIT_MAX = 12;

	size_t hash_code( const type_info& info )
	{
		// hash name() to size_t value by pseudorandomizing transform
		const char *_Keyval = info.name();
		size_t _Val = 2166136261U;
		size_t _First = 0;
		size_t _Last = _CSTD strlen(_Keyval);
		size_t _Stride = 1 + _Last / 10;

		for(; _First < _Last; _First += _Stride)
			_Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
		return (_Val);
	}

	//// BKDR Hash Function
	//unsigned int BKDRHash(char *str)
	//{
	//	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	//	unsigned int hash = 0;

	//	while (*str)
	//	{
	//		hash = hash * seed + (*str++);
	//	}

	//	return (hash & 0x7FFFFFFF);
	//}

	MessageRegist* MessageRegist::instance(void)
	{
		static MessageRegist regist;
		return &regist;
	}

	void MessageRegist::addObserver( const server_type& server_name, const update_function& fn, MsgRecieveClass* object )
	{
		addObserverImpl( _serverMap, _clientMap, server_name, fn, object );
	}

	void MessageRegist::removeObserver( MsgRecieveClass* object )
	{
		removeFromMapImpl( _serverMap, _clientMap, object );
	}

	void MessageRegist::ClearMessageMap()
	{
		_serverMap.clear();
		_clientMap.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// class RecurciveLimit

	RecurciveLimit::RecurciveLimit()
	{
		laynum = AddLay();
	}

	RecurciveLimit::~RecurciveLimit()
	{
		RemoveLay();
	}

	int& RecurciveLimit::GetCounter()
	{
		static int counter=0;
		return counter;
	}

	int RecurciveLimit::AddLay()
	{
		return ++GetCounter();
	}

	int RecurciveLimit::RemoveLay()
	{
		return --GetCounter();
	}

// 测试消息映射
//#define _TEST_MESSAGECOM_

#ifdef _TEST_MESSAGECOM_
	
	class MessageTest;

	// test messagemap
	class MessageReciever
	{
	public:
		DECL_COM_MESSAGE_MAP(MessageReciever);

		void OnComMsg(MessageTest* node, const ComMsg& msg);
		void OnAnyMsg(MessageTest* node, int v1, int& v2, const ComMsg& msg);
		int OnAnyMsgR(MessageTest* node, int v1, std::string& v2, const ComMsg& msg);
	};

	BEGIN_COM_MESSAGE_MAP(MessageReciever)
		ON_COM_MSG( OnComMsg, MessageTest )
		ON_COM_MSG( OnAnyMsg, MessageTest )
		ON_COM_MSG( OnAnyMsgR, MessageTest )
	END_COM_MESSAGE_MAP

	void MessageReciever::OnComMsg(MessageTest* node, const ComMsg& msg)
	{
		ASSERT(msg.type==MSG_DELETE);
		float* fRef = (float*)msg.param;
		*fRef = 20; 
	}

	void MessageReciever::OnAnyMsg(MessageTest* node, int v1, int& v2, const ComMsg& msg)
	{
		++v2;
	}

	int MessageReciever::OnAnyMsgR(MessageTest* node, int v1, std::string& v2, const ComMsg& msg)
	{
		v2="changed";
		return 100;
	}

	class MessageTest
	{
	public:
		MessageTest()
		{
			MessageReciever reciever;

			float fRef=0;
			::SendMessageCom(this, ComMsg(MSG_DELETE, "", &fRef) );
			ASSERT(fRef==20);

			ComMsg msg;
			int nRef=0;
			::SendMessageComAny(this, 100, boost::ref(nRef), boost::cref(msg) );
			ASSERT(nRef==1);

			// -->
			// 上面的消息发送，被推算为下面的形式。
			// 说明：如果显示定义模板参数，不需要使用boost::ref(当然加上也没有错), 这里故意省略boost::ref
			::SendMessageComAny<MessageTest, int, int&, const ComMsg&>(this, 100, nRef, msg );
			ASSERT(nRef==2);

			// -->
			// 上面的消息发送，被继续推算为下面的形式。
			// 说明：1 由于MT_TARG_UNWAP必须为每个参数加了一个&，为了和目标函数(any类型)能够匹配，包装消息函数MT_TARG_REF也强制加上了&
			int nTempVal=100; // 第二个参数首先按值传递，所以有一个临时变量
			MessageRegist::instance()->SendComMsg<MessageTest, int&, int&, const ComMsg &>\
				(MessageMapCom::hash_code(this), this, nTempVal, nRef, msg);
			ASSERT(nRef==3);

			std::string str="ww";
			int kk = 0;
			kk = ::SendMessageComAnyR<int>(this, 2, boost::ref(str), boost::cref(ComMsg()) );
			ASSERT(kk==100&&str=="changed");
		}
	};

	static MessageTest tester;

#endif

}
