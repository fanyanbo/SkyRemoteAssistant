#ifndef __OSAPI_PUSHSERVICE_BIN_API_H_
#define __OSAPI_PUSHSERVICE_BIN_API_H_

#include "CoocaaDef.h"
#include "OSApiListenerController.h"
#include "OSApi_Base.h"
#include "CooCaaLog.h"
NS_COOCAA_BEGIN

class OSApi_PushServiceBin:public OSApi_Base
{
public:

	OSApi_PushServiceBin(OSApiListenerController* controller);
	virtual ~OSApi_PushServiceBin();

	static const std::string STARTUP;
	static const std::string CLEANUP;
	static const std::string REGISTID;
	static const std::string REGISTCHANNEL;
	static const std::string UNREGISTCHANNEL;
	static const std::string NETEVENTCHANGED;

	bool startUp(std::string srcpkgname, std::string mac, std::string barcode,std::string _apram);
	bool cleanUp();

	bool registerID(std::string appidpkgname,std::string appid,std::string token);
	bool registerChannel(std::string appidpkgname,std::string appid,std::string token,std::string channel);
	bool unregisterChannel(std::string appidpkgname,std::string appid,std::string token,std::string channel);
	bool netEventChanged();
};

NS_COOCAA_END
#endif//__OSAPI_PUSHSERVICE_BIN_API_H_
