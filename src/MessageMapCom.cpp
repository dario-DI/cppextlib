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

// ������Ϣӳ��
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
			// �������Ϣ���ͣ�������Ϊ�������ʽ��
			// ˵���������ʾ����ģ�����������Ҫʹ��boost::ref(��Ȼ����Ҳû�д�), �������ʡ��boost::ref
			::SendMessageComAny<MessageTest, int, int&, const ComMsg&>(this, 100, nRef, msg );
			ASSERT(nRef==2);

			// -->
			// �������Ϣ���ͣ�����������Ϊ�������ʽ��
			// ˵����1 ����MT_TARG_UNWAP����Ϊÿ����������һ��&��Ϊ�˺�Ŀ�꺯��(any����)�ܹ�ƥ�䣬��װ��Ϣ����MT_TARG_REFҲǿ�Ƽ�����&
			int nTempVal=100; // �ڶ����������Ȱ�ֵ���ݣ�������һ����ʱ����
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
