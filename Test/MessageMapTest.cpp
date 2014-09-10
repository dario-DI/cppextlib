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
	// �������Ϣ���ͣ�������Ϊ�������ʽ��
	// ˵���������ʾ����ģ�����������Ҫʹ��boost::ref(��Ȼ����Ҳû�д�), �������ʡ��boost::ref
	::SendMessageComAny<MessageSender, int, int&, const ComMsg&>(&sender, 100, nRef, msg );
	assert(nRef==2);

	// -->
	// �������Ϣ���ͣ�����������Ϊ�������ʽ��
	// ˵����1 ����MT_TARG_UNWAP����Ϊÿ����������һ��&��Ϊ�˺�Ŀ�꺯��(any����)�ܹ�ƥ�䣬��װ��Ϣ����MT_TARG_REFҲǿ�Ƽ�����&
	int nTempVal=100; // �ڶ����������Ȱ�ֵ���ݣ�������һ����ʱ����
	COM_MSG_INSTANCE_PTR->SendComMsg<MessageSender, int&, int&, const ComMsg &>\
		(MessageMapCom::hash_code(&sender), &sender, nTempVal, nRef, msg);
	assert(nRef==3);

	std::string str="ww";
	int kk = 0;
	kk = ::SendMessageComAnyR<int>(&sender, 2, boost::ref(str), boost::cref(msg) );
	assert(kk==100&&str=="changed");
}
