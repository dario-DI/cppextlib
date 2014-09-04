/// \file MessageMapCom.h Copyright (C) Sharewin Inc.
/// \brief brief-description
///
///
/// \note:
/// \author: DI
/// \time: 2012/3/15 14:22

#if 0

//!	通用消息映射使用方法
//! 与windows消息映射相似
/*! \ingroup core
 */
namespace exampleCode_send_message
{
	void TestClass::Test()
	{
		/// SendMessageCom发送 ComMsg 类型的消息函数, param 1: 消息发送者指针, param 2: ComMsg类型消息
		::SendMessageCom( this, ComMsg(MSG_DATA, "load", NULL) );

		/// SendMessageComAny发送 任意参数类型的 消息，参数个数为1~8个，第一个参数必须为发送者指针。
		::SendMessageComAny( this, 2.0f，CPoint(125,231) );

		/// 注意: 参数为引用(&)使用boost::ref(), 常引用(const&)使用boost::cref()
		float fRefValue=2.0; 
		::SendMessageComAny( this, boost::ref(fRefValue), boost::cref( CPoint(125,231) ) );

		/// SendMessageComAnyR发送 包含返回值的 任意参数类型的 消息，当消息响应函数返回值为默认值时，消息继续遍历循环；否则，停止循环并返回。
		BOOL bRet = ::SendMessageComAnyR<BOOL>( this, MSG_ADD );
		CPoint pt = ::SendMessageComAnyR<CPoint>( this, boost::ref(fRefValue), boost::cref( CPoint(125,231) ) );
	}
}

namespace exampleCode_recieve_message
{
	/// 接收消息使用方法
	/// 类头文件
	class MyClass
	{
	public:
		DECL_COM_MESSAGE_MAP( MyClass )	//	声明消息映射列表
			//...
	private:

		// 声明消息的自定义相应函数
		void OnSeismicDataLineMsg( CSeismicDataLine* data, const ComMsg& msg );//接收CSeismicDataLine的ComMsg类型消息
		void OnSingleWellDataMsg( CSingleWellData* data, int& refValue ); //接收CSingleWellData的int &类型消息
		BOOL OnFaultMsg( CSeismicFault* data, const CPoint& point );//接收CSeismicFault的CPoint const& 类型含返回值消息

		//...
	};

	/// 类实现文件(.cpp) 
	/// 实现消息映射列表
	BEGIN_COM_MESSAGE_MAP(MyClass)
		ON_COM_MSG( OnSeismicDataLineMsg, CSeismicDataLine ) //接收CSeismicDataLine的消息
		ON_COM_MSG( OnSingleWellDataMsg, CSingleWellData)//接收CSingleWellData的消息
		ON_COM_MSG( OnFaultMsg, CSeismicFault)//接收CSeismicFault的CPoint类型含返回值消息
	END_COM_MESSAGE_MAP

	///	实现自定义消息响应函数
	void MyClass::OnSeismicDataLineMsg( SeismicData::CSeismicDataLine* data, const ComMsg& msg )
	{
		//	add code to handle this massage 
		switch ( msg.type )
		{
		case MSG_COLOR:
			//	add your code here
			data->DoSomething();
		}

		//	add your code here
	}

	BOOL OnFaultMsg( CSeismicFault* data, const CPoint& point )
	{
		///...
		return FALSE; // return false消息继续循环；true, 消息停止循环
	}
}

#endif // define! 0

#ifndef _TDE_MESSAGE_MAP_H_
#define _TDE_MESSAGE_MAP_H_ 1

#pragma once
#include <TDE/config>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <map>

#include <TDE/MetaProgram.hpp>

#define ADD_COM_MSG_OBSERVER( key, method, reciever_ptr ) \
	MessageMapCom::MessageRegist::instance()->addObserver(key, method, reciever_ptr );

#define DECL_COM_MESSAGE_MAP(class_name) public: \
	MessageMapCom::TMessageRegistPxy<class_name> ___comMessageMapProxy; \
	void ___registComMessageMap();

#define BEGIN_COM_MESSAGE_MAP(class_name) \
	void class_name::___registComMessageMap() { \
	typedef class_name ThisClass; 

#define ON_COM_MSG( method, serverType ) \
	ADD_COM_MSG_OBSERVER( \
	MessageMapCom::hash_code( (serverType*)0 ), \
	MessageMapCom::detail::ClassMethod( &ThisClass::method, this, (serverType*)0 ), \
	this );

#define ON_COM_FUNCTION(method) \
	ADD_COM_MSG_OBSERVER( \
	MessageMapCom::hash_code( &ThisClass::method ), \
	MessageMapCom::detail::ClassMethod( &ThisClass::method, this ), \
	this );

#define END_COM_MESSAGE_MAP }

	
//!	公共消息类型, 可以使用其它自定义消息类型
/*! \ingroup core
 */
enum ComMsgType 
{
	MSG_NULL=0,
	MSG_MOUSE,
	MSG_KEYBOARD,
	MSG_REFRESH,
	MSG_ADD,
	MSG_REMOVE,
	MSG_DELETE,
	MSG_PAINT,
	MSG_STATUS,
	MSG_STYLE,
	MSG_OPEN,
	MSG_CLOSE,
	MSG_SAVE,
	MSG_LOAD,
	MSG_CREATE,
	MSG_DESTORY,
	MSG_FOUCS,
	MSG_CHECK,
	MSG_TIME,
	MSG_COMMAND,
	MSG_WATCH,
	MSG_MODEL,
	MSG_DIALOG,
	MSG_DO,
	MSG_UNDO,
	MSG_CUT,
	MSG_PASTE,
	MSG_COPY,
	MSG_SIZE,
	MSG_SCALE,
	MSG_DATA,
	MSG_COLOR,
	MSG_DEPTH,
	MSG_LENGTH,
	MSG_PLAY,
	MSG_START,
	MSG_STOP,
	MSG_FRONT,
	MSG_BACK,
	MSG_BEGIN,
	MSG_END,
	MSG_TOP,
	MSG_BOTTOM,
	MSG_LEFT,
	MSG_RIGHT,
	MSG_CUSTOM,
	MSG_VALID,
	MSG_USER = 2000
};

//!	消息描述类型
/*! \ingroup core
 */
struct strComMsg
{
	UINT type;			///	消息类型	
	std::string info;	///	消息字符串描述
	void* param;		///	消息参数

	strComMsg() : type(0), param(0) {}
	strComMsg(UINT t) : type(t), param(0) {}
	strComMsg(UINT t, const std::string& s) : type(t), info(s), param(0) {}
	strComMsg(UINT t, const std::string& s, void* p) : type(t), info(s), param(p) {}

	strComMsg( const std::string& s ) : type(0), info(s), param(0) {}

};

typedef strComMsg ComMsg;

/// 向没有返回值的响应函数发送消息
template<typename T> void SendMessageCom( T* ptr, UINT type ) { return ::SendMessageCom( ptr, ComMsg(type) ); }
template<typename T> void SendMessageCom( T* ptr, UINT type, const std::string& info ) { return ::SendMessageCom( ptr, ComMsg(type, info) ); }
template<typename T> void SendMessageCom( T* ptr, UINT type, const std::string& info, void* param ) { return ::SendMessageCom( ptr, ComMsg(type, info, param) ); }
template<typename T> void SendMessageCom( T* ptr, const ComMsg& msg ) { ::SendMessageComAny<T,ComMsg const&>(ptr, msg); }


/// 泛型实参(取消wrap_reference包装,并增加引用) boost::unwrap_reference<T1>::type& P1...
#define MT_TARG_UNWAP_FN(N) typename boost::remove_reference<boost::unwrap_reference<P##N>::type>::type&
#define MT_TARG_UNWAP(N) MT_CHR(MT_CHAR_, N)(MT_TARG_UNWAP_FN)

/// 泛型实参(引用) boost::unwrap_reference<T1>::type& P1...
#define MT_TARG_REF_FN(N) typename boost::remove_reference<P##N>::type&
#define MT_TARG_REF(N) MT_CHR(MT_CHAR_, N)(MT_TARG_REF_FN)

#define MT_SENDMSGCOM_R_IMPL(N) \
	template<typename RetType, typename SenderType MT_COMMA(N) MT_TPARAM(N) >\
	RetType SendMessageComAnyR( SenderType* ptr MT_COMMA(N) MT_PARAM(N) )\
	{\
		return MessageMapCom::MessageRegist::instance()->SendComMsgR<RetType,SenderType MT_COMMA(N) MT_TARG_UNWAP(N)>( MessageMapCom::hash_code(ptr), ptr MT_COMMA(N) MT_ARG(N) );\
	}

/// 消息发送函数实现(含返回值)
MT_EXPRESSION_IMPL_0_7( MT_SENDMSGCOM_R_IMPL )

#define MT_SENDMSGCOM_IMPL(N) \
	template<typename SenderType MT_COMMA(N) MT_TPARAM(N) >\
	void SendMessageComAny( SenderType* ptr MT_COMMA(N) MT_PARAM(N) )\
	{\
		return MessageMapCom::MessageRegist::instance()->SendComMsg<SenderType MT_COMMA(N) MT_TARG_UNWAP(N)>( MessageMapCom::hash_code(ptr), ptr MT_COMMA(N) MT_ARG(N) );\
	}

/// 消息发送函数实现(无返回值)
MT_EXPRESSION_IMPL_0_7( MT_SENDMSGCOM_IMPL )


#ifdef _DEBUG
#define Compatible_Type_Check(type, CompatibleType) while(0) { CompatibleType* comType=NULL; type* t=comType; }
#else
#define Compatible_Type_Check(type, CompatibleType)
#endif // #ifdef _DEBUG


/// 由于发送消息时使用boost::ref(),boost::cref(), 消息类型将是boost::remove_reference<P>::type&。所以对于普通类型也都将有引用。
/// 所以对响应函数增加包装，消息参数没有加引用的都加上引用。引用后的参数最终匹配目标函数参数，不影响最终结果。

/// 返回兼容boost::function<fnRetType(serverType*, ...)>函数的boost::function<fnRetType(serverCompatibleType*, ...)>
#define MT_ClassMethod_Compatible_IMPL(N) \
	template<typename fnRetType, typename classType, typename serverType, typename serverCompatibleType MT_COMMA(N) MT_TPARAM(N) > \
	boost::function<fnRetType(serverCompatibleType* MT_COMMA(N) MT_TARG_REF(N) )> \
		ClassMethod(fnRetType(classType::*fn)(serverType* MT_COMMA(N) MT_TARG(N) ), classType* ptr, serverCompatibleType* dummy) \
	{\
		Compatible_Type_Check(serverType, serverCompatibleType) \
		return boost::bind(fn, ptr MT_COMMA( MT_INCR(N) ) MT_PLACEHOLDER( MT_INCR(N) ));\
	}

/// 返回boost::function<fnRetType(serverType*, ...)>
#define  MT_ClassMethod_IMPL(N) \
	template<typename fnRetType, typename classType, typename serverType MT_COMMA(N) MT_TPARAM(N) > \
		boost::function<fnRetType(serverType* MT_COMMA(N) MT_TARG_REF(N) )> \
		ClassMethod(fnRetType(classType::*fn)(serverType* MT_COMMA(N) MT_TARG(N) ), classType* ptr) \
	{\
		return ClassMethod( fn, ptr, (serverType*)0 ); \
	}

	namespace MessageMapCom
	{
		namespace detail
		{
			/// 返回兼容boost::function<fnRetType(serverType*, ...)>函数的boost::function<fnRetType(serverCompatibleType*, ...)>
			MT_EXPRESSION_IMPL_0_7( MT_ClassMethod_Compatible_IMPL )
			
			/// 返回boost::function<fnRetType(serverType*, ...)>
			MT_EXPRESSION_IMPL_0_7( MT_ClassMethod_IMPL )
		}
	}


/// 消息发送函数实现(含返回值)
#define MT_SENDCOMMSG_R_IMPL(N) \
	template<typename RetType, typename SenderType MT_COMMA(N) MT_TPARAM(N) > \
	RetType SendComMsgR( const server_type& key, SenderType* sender MT_COMMA(N) MT_PARAM(N) )\
	{\
		typedef SERVER_MAP::const_iterator SERVER_ITR;\
		typedef boost::function<RetType(SenderType* MT_COMMA(N) MT_TARG(N) )> Method;\
		RecurciveLimit limit;\
		if (limit.laynum > RecurciveLimit::LIMIT_MAX)\
		{\
			ASSERT(false); \
			return RetType();\
		}\
		std::pair<SERVER_ITR, SERVER_ITR> range ( _serverMap.equal_range(key) );\
		RetType bRet = RetType();\
		for (SERVER_ITR itServer = range.first; itServer != range.second; ++itServer)\
		{\
			if (itServer->second.object == NULL) continue;\
			Method method;\
			try\
			{\
				method = boost::any_cast<Method>(itServer->second.fn);\
			}\
			catch (boost::bad_any_cast e)\
			{\
				continue;\
			}\
			bRet = method(sender MT_COMMA(N) MT_ARG(N) );\
			if ( bRet != RetType() ) return bRet;\
		}\
		return bRet;\
	}

/// 无返回值发送消息函数
#define MT_SENDCOMMSG_IMPL(N) \
	template<typename SenderType MT_COMMA(N) MT_TPARAM(N) > \
	void SendComMsg( const server_type& key, SenderType* sender MT_COMMA(N) MT_PARAM(N) )\
	{\
		typedef SERVER_MAP::const_iterator SERVER_ITR;\
		typedef boost::function<void(SenderType* MT_COMMA(N) MT_TARG(N) )> Method;\
		RecurciveLimit limit;\
		if (limit.laynum > RecurciveLimit::LIMIT_MAX)\
		{\
			ASSERT(false);\
			return;\
		}\
		std::pair<SERVER_ITR, SERVER_ITR> range ( _serverMap.equal_range(key) );\
		for (SERVER_ITR itServer = range.first; itServer != range.second; ++itServer)\
		{\
			if (itServer->second.object == NULL) continue;\
			Method method;\
			try\
			{\
				method = boost::any_cast<Method>(itServer->second.fn);\
			}\
			catch (boost::bad_any_cast e)\
			{\
				continue;\
			}\
			method(sender MT_COMMA(N) MT_ARG(N) );\
		}\
	}

/// 关闭消息映射. 用于减少程序退出时间.
TDE_IMPORT void ShutMessageMapCom();

template<typename ValueType>
ValueType unthrow_any_cast(const boost::any& operand)
{
	typedef BOOST_DEDUCED_TYPENAME boost::remove_reference<ValueType>::type nonref;
	const nonref * result = boost::any_cast<nonref>(&operand);
	if(!result) return 0;
	return *result;
}

namespace MessageMapCom
{
	template<typename T>
	size_t hash_code(T obj)
	{
		return hash_code( typeid(obj) );
	}

	template<typename classType, typename serverType>
	size_t hash_code( void(classType::*fn)(serverType*, ComMsg const&) )
	{
		return hash_code( typeid( (serverType*)0 ) );
	}

	TDE_IMPORT size_t hash_code( const type_info& info );

	class TDE_IMPORT RecurciveLimit
	{
	public:
		RecurciveLimit();

		~RecurciveLimit();

		int laynum;

	public:
		static int LIMIT_MAX;

		static int& GetCounter();

		static int AddLay();

		static int RemoveLay();

	};

	class TDE_IMPORT MessageRegist
	{
	public:
		typedef boost::any update_function;
		typedef size_t server_type;

		typedef void MsgRecieveClass; // any

		struct strObserver
		{
			update_function fn;
			MsgRecieveClass* object;

			strObserver() : fn(0), object(0) {}
			strObserver( const update_function& f, MsgRecieveClass* p ) : 
			fn(f), object(p)
			{
			}
		};

		//	message method of string
		typedef std::multimap<server_type, strObserver> SERVER_MAP;
		typedef std::multimap<MsgRecieveClass*, server_type> CLIENT_MAP;

		MessageRegist(void) 
		{
		}

		~MessageRegist(void) 
		{
		}

		static MessageRegist* instance(void);

		// clear all message map
		void ClearMessageMap();

	public:
		
		void addObserver( const server_type& server_name, const update_function& fn, MsgRecieveClass* object );

		void addObserver( ComMsgType type, const update_function& fn, MsgRecieveClass* object );

		void removeObserver( MsgRecieveClass* object );

		//template<typename SenderType MT_COMMA(1) MT_TPARAM(1) > 
		//void SendComMsg( const server_type& key, SenderType* sender MT_COMMA(1) MT_PARAM(1) )
		//{
		//	typedef SERVER_MAP::const_iterator SERVER_ITR;
		//	typedef boost::function<void(SenderType* MT_COMMA(1) MT_TARG(1) )> Method;
		//	RecurciveLimit limit;
		//	if (limit.laynum > RecurciveLimit::LIMIT_MAX)
		//	{
		//		ASSERT(false);
		//		return;
		//	}
		//	std::pair<SERVER_ITR, SERVER_ITR> range ( _serverMap.equal_range(key) );
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

		/// 消息发送函数实现(含返回值)
		MT_EXPRESSION_IMPL_0_7( MT_SENDCOMMSG_R_IMPL )

		/// 消息发送函数实现(无返回值)
		MT_EXPRESSION_IMPL_0_7( MT_SENDCOMMSG_IMPL )

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
	};

	template<typename T>
	class TMessageRegistPxy
	{
	public:
		typedef T client;
        
		TMessageRegistPxy()
		{
			_client = (client*) ((char*)this - offsetof(client, ___comMessageMapProxy));
			
			_client->___registComMessageMap();
		}

		~TMessageRegistPxy()
		{
			MessageRegist::instance()->removeObserver( _client );
		}

		client* get() { return _client; }

	private:

		client* _client;
	};

}

#undef MT_TARG_UNWAP_FN
#undef MT_TARG_UNWAP
#undef MT_TARG_REF_FN
#undef MT_TARG_REF

#undef MT_SENDMSGCOM_R_IMPL
#undef MT_SENDMSGCOM_IMPL

#undef Compatible_Type_Check
#undef MT_ClassMethod_Compatible_IMPL
#undef MT_ClassMethod_IMPL

#undef MT_SENDCOMMSG_R_IMPL
#undef MT_SENDCOMMSG_IMPL


#endif