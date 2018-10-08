#ifndef __IPCTRANSPORTER_FACTORY_H_
#define __IPCTRANSPORTER_FACTORY_H_

#include "CoocaaDef.h"
#include <memory>
#include <string>

NS_COOCAA_BEGIN

class IPCTransporter;
class IPCTransporterFactory
{
public:
	static std::shared_ptr<IPCTransporter> createTransporter(std::string lsocketpath);
};

NS_COOCAA_END

#endif//__IPCTRANSPORTER_FACTORY_H_