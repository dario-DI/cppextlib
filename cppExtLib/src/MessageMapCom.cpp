#include "stdAfx.h"
#include <cex\MessageMapCom.h>

void APIENTRY ShutMessageMapCom()
{
	cex::DeltaInstance<MessageMapCom::IMessageRegist>()->ClearMessageMap();
}

namespace MessageMapCom
{
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

#pragma region ClassMessageRegist

	class  MessageRegist : public IMessageRegist
	{
	public:
		MessageRegist(void) 
		{
		}

		~MessageRegist(void) 
		{
		}

	public:
		// clear all message map
		void ClearMessageMap()
		{
			getServerMap().clear();
			getClientMap().clear();
		}

		void MessageRegist::addObserver( const server_type& server_name, const update_function& fn, MsgRecieveClass* object )
		{
			addObserverImpl( getServerMap(), _clientMap, server_name, fn, object );
		}

		//void addObserver( ComMsgType type, const update_function& fn, MsgRecieveClass* object )
		//{
		//	addObserverImpl( getServerMap(), _clientMap, type, fn, object );
		//}

		void MessageRegist::removeObserver( MsgRecieveClass* object )
		{
			removeFromMapImpl( getServerMap(), _clientMap, object );
		}		

		//template<typename SenderType MT_COMMA(1) MT_TPARAM(1) > 
		//void SendComMsg( const server_type& key, SenderType* sender MT_COMMA(1) MT_PARAM(1) )
		//{
		//	typedef SERVER_MAP::const_iterator SERVER_ITR;
		//	typedef boost::function<void(SenderType* MT_COMMA(1) MT_TARG(1) )> Method;
		//	RecurciveLimit limit;
		//	if (limit.laynum > RecurciveLimit::LIMIT_MAX)
		//	{
		//		assert(false);
		//		return;
		//	}
		//	std::pair<SERVER_ITR, SERVER_ITR> range ( getServerMap().equal_range(key) );
		//	for (SERVER_ITR itServer = range.first; itServer != range.second; ++itServer)
		//	{
		//		if (itServer->second.object == NULL) continue;
		//		Method method;
		//		try
		//		{
		//			method = boost::any_cast<Method>(itServer->second.fn);
		//		}
		//		catch (boost::bad_any_cast e)
		//		{
		//			continue;
		//		}
		//		method(sender MT_COMMA(1) MT_ARG(1) );
		//	}
		//}

	private:

		template<typename ServerKey, typename ServerValue, typename ClientKey, typename Function>
		static void addObserverImpl( std::multimap<ServerKey, ServerValue>& server, 
			std::multimap<ClientKey, ServerKey>& client, 
			const ServerKey& key, const Function& fn, ClientKey object )
		{
			typedef std::multimap<ServerKey, ServerValue> ServerMap;
			typedef std::multimap<ClientKey, ServerKey> ClientMap;

			typedef std::pair<ServerKey, ServerValue> ServerPair;
			typedef std::pair<ClientKey, ServerKey> ClientPair;

			typedef typename ServerMap::iterator SERVER_ITR;
			typedef typename ClientMap::iterator CLIENT_ITR;

			client.insert( ClientPair(object, key) );

			MessageRegist::strObserver observer(fn, object);

#if 0
			//  if there has an null client, map it to the new one
			std::pair<SERVER_ITR, SERVER_ITR> range ( server.equal_range(key) );
			for (SERVER_ITR it = range.first; it != range.second; ++it)
			{
				if (it->second.object == NULL)
				{
					it->second = observer;
					return;
				}
			}
#endif
			server.insert( ServerPair(key, observer) );
		}

		template<typename ServerKey, typename ServerValue, typename ClientKey>
		static void removeFromMapImpl( std::multimap<ServerKey, ServerValue>& server, 
			std::multimap<ClientKey, ServerKey>& client, ClientKey object )
		{
			typedef std::multimap<ServerKey, ServerValue> ServerMap;
			typedef std::multimap<ClientKey, ServerKey> ClientMap;

			typedef ServerMap::iterator SERVER_ITR;
			typedef ClientMap::iterator CLIENT_ITR;

			std::pair<CLIENT_ITR, CLIENT_ITR> rngClient ( client.equal_range(object) );

			for ( CLIENT_ITR it = rngClient.first; it != rngClient.second; ++it )
			{
				std::pair<SERVER_ITR, SERVER_ITR> range ( 
					server.equal_range( it->second ) );

#if 0
				for (SERVER_ITR itServer = range.first; 
					itServer != range.second; 
					++itServer)
				{
					if (itServer->second.object == object)
					{
						server.erase(itServer);//itServer->second = MessageRegist::strObserver(); 
						break;
					}
				}
#else
				SERVER_ITR itServer = range.first;
				while ( itServer != range.second )
				{
					if (itServer->second.object == object)
					{
						server.erase(itServer++);
					}
					else
					{
						itServer++;
					}
				}
			}
#endif
			client.erase( object );
		};

	private:

		SERVER_MAP _serverMap;		//!	消息发送者者映射的所有消息接受者
		CLIENT_MAP _clientMap;		//!	消息接收者映射的所有消息发送者者

		SERVER_MAP& getServerMap()
		{
			return _serverMap;
		}
	
		CLIENT_MAP& getClientMap()
		{
			return _clientMap;
		}
	};

	//regist singleton instance
	REGIST_DELTA_INSTANCE(IMessageRegist, MessageRegist);

#pragma endregion


// 测试消息映射
#define _TEST_MESSAGECOM_

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
		assert(msg.type==MSG_DELETE);
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
			assert(fRef==20);

			ComMsg msg;
			int nRef=0;
			::SendMessageComAny(this, 100, boost::ref(nRef), boost::cref(msg) );
			assert(nRef==1);

			// -->
			// 上面的消息发送，被推算为下面的形式。
			// 说明：如果显示定义模板参数，不需要使用boost::ref(当然加上也没有错), 这里故意省略boost::ref
			::SendMessageComAny<MessageTest, int, int&, const ComMsg&>(this, 100, nRef, msg );
			assert(nRef==2);

			// -->
			// 上面的消息发送，被继续推算为下面的形式。
			// 说明：1 由于MT_TARG_UNWAP必须为每个参数加了一个&，为了和目标函数(any类型)能够匹配，包装消息函数MT_TARG_REF也强制加上了&
			int nTempVal=100; // 第二个参数首先按值传递，所以有一个临时变量
			COM_MSG_INSTANCE_PTR->SendComMsg<MessageTest, int&, int&, const ComMsg &>\
				(MessageMapCom::hash_code(this), this, nTempVal, nRef, msg);
			assert(nRef==3);

			std::string str="ww";
			int kk = 0;
			kk = ::SendMessageComAnyR<int>(this, 2, boost::ref(str), boost::cref(ComMsg()) );
			assert(kk==100&&str=="changed");
		}
	};

	static MessageTest tester;

#endif

}
