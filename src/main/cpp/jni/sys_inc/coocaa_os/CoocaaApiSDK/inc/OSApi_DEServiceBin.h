#ifndef __OSAPI_DESERVICE_BIN_H_
#define __OSAPI_DESERVICE_BIN_H_
#include "CoocaaDef.h"

#include "OSApiListenerController.h"
#include "OSApi_Base.h"

NS_COOCAA_BEGIN
class OSApi_DEServiceBin:public OSApi_Base
{
public:

	OSApi_DEServiceBin(OSApiListenerController* controller);
	virtual ~OSApi_DEServiceBin();

	static const std::string STARTUP;
	static const std::string NOTIFYINFO;
	static const std::string WRITEDECHANNEL;
	static const std::string READDECHANNEL;
	static const std::string SETDEVICENAME;
	static const std::string RESTART;

	void startUp(std::string devicename,std::string version,std::string devtype);
	void restart();
	void notifyInfo(std::string cmd,std::string param);
	void writeDEChannel(std::string sessionstr);
	std::string readDEChannel(std::string sessionstr);
	void setDeviceName(std::string devicename);

private:
	OSApi_DEServiceBin(const OSApi_DEServiceBin&);
	OSApi_DEServiceBin& operator = (const OSApi_DEServiceBin&);


};

NS_COOCAA_END

#endif//__OSAPI_DESERVICE_BIN_H_