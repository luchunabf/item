#pragma once
#include <iostream>
#include <WinSock2.h>
#include <mysql.h>
#include <string>
#include <vector>
using namespace std;

#pragma comment(lib, "libmysql.lib")


class MySQL
{
public:
	MySQL();
	bool ConnectMySql(const char* host, // 主机名称
		const char* user, // 用户名
		const char* passward, // 密码
		const char* dbName, // 数据库名
		int port = 3306); // 端口号：默认为3306
	~MySQL();
	bool Insert(const string& strSQL);
	bool Delete(const string& strSQL);
	bool Update(const string& strSQL);
	size_t GetCount(const string& strSQL);
	vector<vector<string>> Select(const string& strSQL);
	// 切换数据库
	bool SelectDB(const string& daName);
private:
	MYSQL* _mySql; // mysql连接的实例对象
	std::string _dbName;
	vector<string> _tables;
};

