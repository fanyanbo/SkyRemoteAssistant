#ifndef _SQLITE_USAGE_H_
#define _SQLITE_USAGE_H_

#include "CoocaaDef.h"
#include "sqlite3.h"
#include <string>
#include <thread>

NS_COOCAA_BEGIN

typedef int (*QueryCallback) (void *para, int n_column, char **column_value, char **column_name);  

typedef enum _SQLITE_DATATYPE  
{  
	//整形
	SQLITE_DATATYPE_INTEGER = SQLITE_INTEGER,  
	//浮点形
	SQLITE_DATATYPE_FLOAT  = SQLITE_FLOAT,  
	//字符串
	SQLITE_DATATYPE_TEXT  = SQLITE_TEXT,  
	//二进制
	SQLITE_DATATYPE_BLOB = SQLITE_BLOB,  
	//空
	SQLITE_DATATYPE_NULL= SQLITE_NULL,  
}SQLITE_DATATYPE;

class SQLiteUsage;

class SQLiteDataReader
{
public:
	SQLiteDataReader(sqlite3_stmt *pStmt);
	~SQLiteDataReader();

	// 读取一行数据   
	bool read();  

	// 关闭Reader，读取结束后调用   
	void close();  

	// 总的列数   
	int columnCount(void);  

	// 获取某列的名称    
	std::string getName(int nCol);  

	// 获取某列的数据类型   
	SQLITE_DATATYPE getDataType(int nCol);  

	// 获取某列的值(字符串)   
	std::string getStringValue(int nCol);  

	// 获取某列的值(整形)   
	int getIntValue(int nCol);  

	// 获取某列的值(长整形)   
	long getInt64Value(int nCol);

	// 获取某列的值(浮点形)   
	double getFloatValue(int nCol);  

	// 获取某列的值(二进制数据)   
	const unsigned char* getBlobValue(int nCol, int &nLen);  

private: 
	sqlite3_stmt *m_pStmt;
};

class SQLiteCommand
{
public:
	SQLiteCommand(SQLiteUsage* pSqlite);
	SQLiteCommand(SQLiteUsage* pSqlite,std::string lpSql);  
	~SQLiteCommand();

	bool setCommandText(std::string lpSql); 
	// 绑定参数（index为要绑定参数的序号，从1开始）  
	bool bindParam(int index, std::string szValue);
	bool bindParam(int index, const int nValue);  
	bool bindParam(int index, const double dValue);  
	bool bindParam(int index, const unsigned char* blobValue, int nLen);  
	// 执行命令  
	bool execute();
	// 清除命令（命令不再使用时需调用该接口清除）  
	void clear();

private:
	SQLiteUsage *m_pSqlite;  
	sqlite3_stmt *m_pStmt;  
};


class SQLiteUsage
{
public:

	friend class SQLiteCommand;

	SQLiteUsage();
	~SQLiteUsage();

	bool open(std::string dbfile);

	void close();

	//执行非查询操作
	bool execute(std::string sqlstr);
	bool execute(SQLiteCommand* pCmd);  

	SQLiteDataReader executeWithResult(std::string lpSql);  
	//回调方式
	bool executeWithResult(std::string lpSql,QueryCallback querycb); 

	//开始事务 
	bool beginTransaction();
	//提交事务 
	bool commitTransaction();
	//回滚事务
	bool roolbackTransaction();

	std::string getLastErrorMsg();

private:  
	sqlite3 *m_db;  
};

NS_COOCAA_END

#endif//_SQLITE_USAGE_H_