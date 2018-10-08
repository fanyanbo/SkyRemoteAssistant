#ifndef __DEDATA_H_
#define __DEDATA_H_
#include "CoocaaDef.h"
#include <string>
#include <vector>
#include "json.h"

NS_COOCAA_BEGIN

class DEData
{
public:

	DEData();

	DEData(const DEData& srcData);

	DEData(const std::string& deserializeStr);

	virtual ~DEData();

	DEData & operator=(const DEData & src);

	std::string getStringValue(const std::string & key);

	int getIntValue(const std::string&  key,int& result);

	int getLongValue(const std::string&  key,long& result);

	int getBooleanValue(const std::string&  key,bool& result);

	int getFloatValue(const std::string&  key,float& result);

	int getDoubleValue(const std::string &  key,double& result);

	int getDEDataListValue(const std::string&  key,  std::vector<DEData>& list);

	int  getDEData(const std::string& key,DEData& data);

	void addValue(const std::string&  key, const std::string& value);	

	void addValue(const std::string&  key,const char value[]);

	void addValue(const std::string& key, std::vector<DEData> value);

	void addValue(const  std::string&  key, const int& value);

	void addValue(const std::string&  key, const long& value);

	void addValue(const std::string& key, const bool& value);

	void addValue(const std::string&  key, const float& value);

	void addValue(const std::string&  key, const double& value);

	void addValue(const std::string& key,DEData data);

	std::string toString();

	bool isIntValue(const std::string& key);

	bool isStringValue(const std::string& key);

private:
	Json::Value getJsonValue();
	DEData(Json::Value value);
private:
	Json::Value root;  //the whole json object 
	Json::FastWriter writer;// fast json writer
	Json::Reader reader;// json reader for the serialized data 
};

NS_COOCAA_END

#endif//__DEDATA_H_
