#include "stdafx.h"

#include <cex/MessageMapCom.h>
#include <cex/UnTest.h>

using namespace cex;

class MessageSender
{
};

class MessageReciever
{
public:
	DECL_COM_MESSAGE_MAP(MessageReciever);

	void OnComMsg(MessageSender* node, const ComMsg& msg);
	void OnAnyMsg(MessageSender* node, int v1, int& v2, const ComMsg& msg);
	int OnAnyMsgR(MessageSender* node, int v1, std::string& v2, const ComMsg& msg);
};

BEGIN_COM_MESSAGE_MAP(MessageReciever)
	ON_COM_MSG( OnComMsg, MessageSender )
	ON_COM_MSG( OnAnyMsg, MessageSender )
	ON_COM_MSG( OnAnyMsgR, MessageSender )
END_COM_MESSAGE_MAP

void MessageReciever::OnComMsg(MessageSender* node, const ComMsg& msg)
{
	assert(msg.type==MSG_DELETE);
	float* fRef = (float*)msg.param;
	*fRef = 20; 
}

void MessageReciever::OnAnyMsg(MessageSender* node, int v1, int& v2, const ComMsg& msg)
{
	++v2;
}

int MessageReciever::OnAnyMsgR(MessageSender* node, int v1, std::string& v2, const ComMsg& msg)
{
	v2="changed";
	return 100;
}


CEX_TEST(MessageMapTest)
{
	MessageSender sender;
	MessageReciever reciever;

	float fRef=0;
	::SendMessageCom(&sender, ComMsg(MSG_DELETE, "", &fRef) );
	assert(fRef==20);

	ComMsg msg;
	int nRef=0;
	::SendMessageComAny(&sender, 100, boost::ref(nRef), boost::cref(msg) );
	assert(nRef==1);

	// -->
	// 上面的消息发送，被推算为下面的形式。
	// 说明：如果显示定义模板参数，不需要使用boost::ref(当然加上也没有错), 这里故意省略boost::ref
	::SendMessageComAny<MessageSender, int, int&, const ComMsg&>(&sender, 100, nRef, msg );
	assert(nRef==2);

	// -->
	// 上面的消息发送，被继续推算为下面的形式。
	// 说明：1 由于MT_TARG_UNWAP必须为每个参数加了一个&，为了和目标函数(any类型)能够匹配，包装消息函数MT_TARG_REF也强制加上了&
	int nTempVal=100; // 第二个参数首先按值传递，所以有一个临时变量
	COM_MSG_INSTANCE_PTR->SendComMsg<MessageSender, int&, int&, const ComMsg &>\
		(MessageMapCom::hash_code(&sender), &sender, nTempVal, nRef, msg);
	assert(nRef==3);

	std::string str="ww";
	int kk = 0;
	kk = ::SendMessageComAnyR<int>(&sender, 2, boost::ref(str), boost::cref(msg) );
	assert(kk==100&&str=="changed");
}
