#pragma once
#include "Common.h"
//#include "mysql.h"
#include "MySQL.h"

class LogInWnd : public WindowImplBase
{
public:
	//LogInWnd();
	LogInWnd(MySQL* pMySQL);//显式给出带有参数构造函数
	~LogInWnd();

	//消息响应
	virtual void Notify(TNotifyUI& msg);

	void LogIn();

protected:
	//WindowImplBase提供三个纯虚函数

	//xml文件对应的目录
	virtual CDuiString  GetSkinFolder();

	//xml文件名字
	virtual CDuiString GetSkinFile();

	//窗口类的名字，在注册窗口时必须提供(自己不用管)
	virtual LPCTSTR GetWindowClassName() const;

private:
	MySQL* m_pMySQL;
};