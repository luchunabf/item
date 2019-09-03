#define _CRT_SECURE_NO_WARNINGS 1
#include "MySQL.h"



#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")
MySQL::MySQL()
{
	// 初始化mySql
	_mySql = mysql_init(nullptr);
}
bool MySQL::ConnectMySql(const char* host, const char* user, const char* passward,
	const char* dbName, int port)
{
	// 连接mySql数据库
	if (!mysql_real_connect(_mySql, host, user, passward, dbName, port, NULL, 0))
	{
		cout << "数据库连接失败" << endl;
		return false;
	}
	/*
	c++连接mysql时，比如查询语句中含有中文，或者得到结果中含有中文，经常出现编译出错或乱码问
	题。
	VS编译器默认使用gbk编码。
	如果将mysql设置为utf-8编码，则需要先将c++中的各种中文字符串转为utf-8编码输入mysql，得到
	的结果为utf-8编码，需要转为gbk才能正常显示。转来转去很麻烦。
	*/
	mysql_query(_mySql, "set names 'gbk'");
	return true;
}
bool MySQL::SelectDB(const string& dbName)
{
	if (mysql_select_db(_mySql, dbName.c_str()))
	{
		return false;
	}
	return true;
}
bool MySQL::Insert(const string& strSql)
{
	// 执行sql语句
	if (mysql_query(_mySql, strSql.c_str()))
	{
		cout << mysql_error(_mySql) << endl;
		return false;
	}
	return true;
}
bool MySQL::Update(const string& strSQL)
{
	// 执行sql语句
	if (mysql_query(_mySql, strSQL.c_str()))
	{
		cout << mysql_error(_mySql) << endl;
		return false;
	}
	return true;
}
vector<vector<string>> MySQL::Select(const string& sql)
{
	vector<vector<string>> vRet;
	// 指定指定SQL语句
	if (mysql_query(_mySql, sql.c_str()))
	{
		string vsRet(mysql_error(_mySql));
		return vRet;
	}
	// 检索完整的数据集到客户端
	MYSQL_RES *res = mysql_store_result(_mySql);
	if (res == NULL)
	{
		return vRet;
	}
	// 用来保存结果集中行的信息
	MYSQL_ROW rows;
	// 结果集中总共有多少行数据
	int num_fields = mysql_num_fields(res);
	while (rows = mysql_fetch_row(res))
	{
		int i = 0;
		vector<string> vItem;
		vItem.resize(num_fields);
		for (i = 0; i < num_fields; i++)
		{
			vItem[i] = rows[i];
		}
		vRet.push_back(vItem);
	}
	const char* str = mysql_error(_mySql);
	mysql_free_result(res);//释放记录集
	return vRet;
}
size_t MySQL::GetCount(const string& strSQL)
{
	// 指定指定SQL语句
	if (mysql_query(_mySql, strSQL.c_str()))
	{
		return 0;
	}
	// 检索完整的数据集到客户端
	MYSQL_RES *res = mysql_store_result(_mySql);
	if (res == NULL)
	{
		return 0;
	}
	return mysql_num_fields(res);
}
bool MySQL::Delete(const string& strSQL)
{
	// 执行sql语句
	if (mysql_query(_mySql, strSQL.c_str()))
	{
		cout << mysql_error(_mySql) << endl;
		return false;
	}
	return true;
}
MySQL::~MySQL()
{
	mysql_close(_mySql);
}