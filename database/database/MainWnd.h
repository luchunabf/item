#pragma once
#include "Common.h"
//#include "mysql.h"
#include "MySQL.h"

class MainWnd : public WindowImplBase
{
public:
	//MainWnd();
	MainWnd(MySQL* pMySQL);//给构造函数传入参数，引入数据库
	~MainWnd();

	virtual void Notify(TNotifyUI& msg);

protected:
	//WindowImplBase提供三个纯虚函数

	//xml文件对应的目录
	virtual CDuiString  GetSkinFolder();
	
	//xml文件名字
	virtual CDuiString GetSkinFile();

	//窗口类的名字，在注册窗口时必须提供(自己不用管)
	virtual LPCTSTR GetWindowClassName() const;

	void SelectEmployeeInfo();

	void DeleteEmployeeInfo();

	void InsertEmployeeInfo();
private:
	MySQL* m_pMySQL;
};