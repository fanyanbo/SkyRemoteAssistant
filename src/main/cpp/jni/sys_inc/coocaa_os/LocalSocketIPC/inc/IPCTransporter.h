#ifndef __IPCTRANSPORTER_H_
#define __IPCTRANSPORTER_H_
#include "CoocaaDef.h"
#include "IIPCHandler.h"

#include <string>
#include <vector>
#include <algorithm>

NS_COOCAA_BEGIN
class IPCTransporter:public IIPCHandler
{
public:
	IPCTransporter(std::string lSocketPath);
	virtual ~IPCTransporter();

	void setCmdReceiver(IIPCMessageReceiver* p);

	std::string getSyncLoSocketPath();
	std::string getASyncLoSocketPath();

protected:
	IIPCMessageReceiver* _msgReceiver;
	std::string _localpath_sync;
	std::string _localpath_async;

	static std::string convertasyncpath(std::string const& str);
	static std::string convertsyncpath(std::string const& str);

	static std::string unconverpath(std::string const& str);
private:
	static std::string convertpath(std::string const& str, std::string const& delimiter);
	static void unconverpath(std::string const& str, std::string const& delimiter, std::vector<std::string>& dest);
};

NS_COOCAA_END
#endif //__IPCTRANSPORTER_H_