#ifndef __OSAPI_ADAPTERSERVICE_BIN_H_
#define __OSAPI_ADAPTERSERVICE_BIN_H_
#include "CoocaaDef.h"

#include "OSApiListenerController.h"
#include "OSApi_Base.h"

NS_COOCAA_BEGIN
class OSApi_AdapterServiceBin:public OSApi_Base
{
public:

	OSApi_AdapterServiceBin(OSApiListenerController* controller);
	virtual ~OSApi_AdapterServiceBin();

	static const std::string INIT;
	static const std::string CLEAR;
	static const std::string SETIMAGEMODE;
	static const std::string GETIMAGEMODE;
	static const std::string SETVOLUME;
	static const std::string GETVOLUME;
	static const std::string SETLEDCTRL;
	static const std::string SETMUTE;
	static const std::string GETMUTE;
	static const std::string FACTORYRESET;
	static const std::string SETSTANDBY;
	static const std::string GETSTANDBY;
	static const std::string ENABLEETH;
	static const std::string GETETHSTATE;
	static const std::string SHUTDOWN;
	
	static const std::string SETAUDIOMODE;
	static const std::string GETAUDIOMODE;
	static const std::string SETSRS;
	static const std::string GETSRS;

	void init();
	void clear();
	void setImageMode(std::string value);
	std::string getImageMode();
	void setVolume(std::string value);
	std::string getVolume();
	void setLedCtrl(std::string value);
	void setMute(std::string value);
	std::string getMute();
	void factoryReset(std::string value, std::string option);
	void setStandby(std::string value);
	std::string getStandby();
	void enableEth(std::string value);
	std::string getEthState();
	void shutDown();

	void setAudioMode(std::string value);
	std::string getAudioMode();
	void setSRS(std::string value);
	std::string getSRS();

private:
	OSApi_AdapterServiceBin(const OSApi_AdapterServiceBin&);
	OSApi_AdapterServiceBin& operator = (const OSApi_AdapterServiceBin&);


};

NS_COOCAA_END

#endif//__OSAPI_ADAPTERSERVICE_BIN_H_