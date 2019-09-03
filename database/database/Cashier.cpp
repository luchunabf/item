#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
using namespace std;
#include <WinSock2.h>
#include "mysql.h"
#include "MySQL.h"

#if 0
int main()//这里可以通过测试，只不过win32下不能编译而已
{
	MySQL mysql;
	mysql.ConnectMySql("localhost", "root", "123", "item1");
	vector<vector<string>> vvRet = mysql.Select("select * from employee;");
	mysql.Insert("insert into employee values(9, 'jim','nan','1995-01-08','000000','casher','15010000009',8000.00);");
	mysql.Select("select * from employee;");
	return 0;
}


#endif