#ifndef __OSAPI_PUSHSERVICE_SHELL_API_H_
#define __OSAPI_PUSHSERVICE_SHELL_API_H_
#include "CoocaaDef.h"
#include <string>
#include "OSApiListenerController.h"
#include "OSApi_Base.h"
NS_COOCAA_BEGIN
class OSApi_PushServiceShell:public OSApi_Base
{
public:
	OSApi_PushServiceShell(OSApiListenerController* controller);
	virtual ~OSApi_PushServiceShell();



	static const std::string PUSH_ID;
	static const std::string CHANNEL;
	static const std::string UN_CHANNEL;
	static const std::string MSG;
	static const std::string INFO_DEVICE;
	static const std::string PUSH_STATE;
	static const std::string PUSH_RESTART;

	void notifyPushID(std::string pkg, std::string pushid, std::string serviceid);
	void notifyChannel(std::string pkg, std::string channel, std::string serviceid);
	void notifyUnChannel(std::string pkg, std::string channel, std::string serviceid);
	void notifyPushMsg(std::string pkg,std::string msgcontent);
	void notifyPushBinState(int stateCode, std::string codestr);
	std::string info_getDevice();
	void notifyStarted();


private :
	OSApi_PushServiceShell(const OSApi_PushServiceShell&);
	OSApi_PushServiceShell& operator =(const OSApi_PushServiceShell&);
};
NS_COOCAA_END
#endif//__OSAPI_PUSHSERVICE_SHELL_API_H_