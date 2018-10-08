#ifndef __IPCMESSAGE_H_
#define __IPCMESSAGE_H_
#include "CoocaaDef.h"
#include <string>
#include <map>
#include <list>
#include <vector>

namespace flatbuffers
{
    class FlatBufferBuilder;
}


NS_COOCAA_BEGIN

class IPCMessage
{
public:
    IPCMessage(const std::string & from, const std::string & to, const std::string & cmd);
    IPCMessage(const unsigned char* buffer, size_t dataSize);
    IPCMessage(const char* buffer, size_t dataSize);
    IPCMessage(const IPCMessage& src);
    IPCMessage();
    virtual ~IPCMessage();

    const IPCMessage& operator=(const IPCMessage& src);

    std::string getUUID();
    std::string getLsocketFrom();
    std::string getLsocketTo();
    std::string getCmd();
    int getContentValue(const std::string& key, std::string & value);
    int getContentValue(const std::string& key, std::list<std::string> & stringValueList);
    int getContentValue(const std::string& key, std::vector<std::string> & stringValueList);
    bool isNeedAck();
	bool isNeedAck() const;
    bool isEmpty();
	bool isEmpty() const;

    unsigned char* flushBuffers();
    unsigned int getBufferLength();

    void addContentValue(const std::string& key, const std::string& value);
    void addContentValue(const std::string& key, const std::list<std::string> & stringValueList);
    void addContentValue(const std::string& key, const std::vector<std::string> & stringValueList);
    void setNeedAck(bool needAck);
    void clear();
    void setLsocketFrom(std::string from);
    void setLsocketTo(std::string to);
    void setCmdString(std::string cmd);
    void setUUID(std::string uuid);

private:
    flatbuffers::FlatBufferBuilder *    m_builder;
    std::string                         m_uuid;
    std::string                         m_lsocketPathFrom;
    std::string                         m_lsocketPathTo;
    std::string                         m_cmdString;
    std::map<std::string, std::string>  m_cmdBody;
    bool                                m_needAck;
    bool                                m_dirty;

    int serialize();
    int parse(const unsigned char * flatstring, size_t dataSize);
    void makeUUID();
    void setDirty();
    void addContentValue_inner(const std::string& key, const std::string& value);
};

NS_COOCAA_END
#endif //__IPCMESSAGE_H_

