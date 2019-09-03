#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "LogIn.h"
#include "MainWnd.h"
#include "CashierWnd.h"

LogInWnd::LogInWnd(MySQL* pMySQL)
:m_pMySQL(pMySQL)
{

}
LogInWnd::~LogInWnd()
{}


//xml文件目录
CDuiString LogInWnd::GetSkinFolder()
{
	return _T("");
}

//xml文件的名字
CDuiString LogInWnd::GetSkinFile()
{
	return _T("LogInWnd.xml");
}


//窗口类的名字，在注册窗口时必须提供(自己不用管)
LPCTSTR LogInWnd::GetWindowClassName(void) const
{
	return _T("LogInWnd");
}


void LogInWnd::Notify(TNotifyUI& msg)
{
	CDuiString strName = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		//CDuiString strBTNName = msg.pSender->GetName();
		if (strName == _T("BTN_CLOSE"))
			PostQuitMessage(0);//Close();关闭窗口
		else if (strName == _T("BTN_MIN"))
			::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);//最小化窗口
		else if (strName == _T("BTN_LOGIN"))
			//MessageBox(m_hWnd, _T("LOGIN"), _T("cashier"), IDOK);
			LogIn();
	}
}

void LogInWnd::LogIn()
{
	//从编辑框获取用户名以及密码
	CEditUI* pEditUerName = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_USER_NAME"));
	//CDuiString strUerName = pEditUerName->GetText();
	//CDuiString strUerName = _T("李四")/*pEditUerName->GetText()*/;
	CDuiString strUerName = pEditUerName->GetText();


	CEditUI* pEditPassword = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_USER_PASSWORD"));
	//CDuiString strUserPassword = pEditPassword->GetText();
	//CDuiString strUserPassword = _T("123")/*pEditPassword->GetText()*/;
	CDuiString strUserPassword = pEditPassword->GetText();


	if (strUerName.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入用户名"), _T("cashier"), IDOK);
		return;
	}
	if (strUserPassword.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入用户密码"), _T("cashier"), IDOK);
		return;
	}
	//查询数据库，检测该用户是否存在
	string strSQL("select * from employee where Name = '");
	strSQL += UnicodeToANSI(strUerName);
	strSQL += "';";

	//将查询结果用vRet接收
	vector<vector<string>> vRet = m_pMySQL->Select(strSQL);
	if (vRet.empty())
	{
		MessageBox(m_hWnd, _T("用户名错误"), _T("cashier"), IDOK);
		return;
	}
	string userpassword = UnicodeToANSI(strUserPassword);
	if (userpassword != vRet[0][4])
	{
		MessageBox(m_hWnd, _T("用户密码错误"), _T("cashier"), IDOK);
		return;
	}

	//隐藏登录窗口
	ShowWindow(false);

	if (vRet[0][5] == "管理员")
	{
		//创建主窗口(用户存在的话)
		MainWnd mainWnd(m_pMySQL);
		mainWnd.Create(NULL, _T("MainWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		mainWnd.CenterWindow();
		mainWnd.ShowModal();
	}
	//else if (vRet[0][5] == "售货员")
	else
	{
		CCashierWnd cashWnd(m_pMySQL);
		cashWnd.Create(NULL, _T("CashierWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		cashWnd.CenterWindow();
		cashWnd.ShowModal();
	}


}
















#if 0//这里进行了一次简单测试
void LogInWnd::LogIn()
{
	//从编辑框中获取用户名以及密码
	CEditUI* pEditUserName = (CEditUI*)m_PaintManager.FindControl(_T("u"));////////
	CDuiString StrUserName = pEditUserName->GetText();

	CEditUI* pEditPassword = (CEditUI*)m_PaintManager.FindControl(_T("u"));////////
	CDuiString StrUserPassword = pEditPassword->GetText();

	if (StrUserName.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入用户名"), _T("cashier"), IDOK);
		return;
	}
	if (StrUserPassword.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入密码"), _T("cashier"), IDOK);
		return;
	}



	//查询数据库，检测该用户是否存在
	//string
	string strSQL("select * from employee where name = '");

	//ASCII UNICODE

	//strSQL += StrUserName;
	strSQL += UnicodeToANSI(StrUserName);
	strSQL += "';";

	//strSQL = "select * from employee;";测试了一下
	vector<vector<string>> vRet =  m_pMySQL->Select(strSQL);
	if (vRet.empty())
	{
		MessageBox(m_hWnd, _T("用户名错误"), _T("Cashier"), IDOK);
		return;
	}
	string userpassword = UnicodeToANSI(StrUserPassword);//判断密码是否匹配
	if(userpassword != vRet[0][4])
	{
		MessageBox(m_hWnd, _T("用户密码错误"), _T("Cashier"), IDOK);
		return;
	}



	//隐藏登录窗口
	ShowWindow(false);
	if (0)
	{
		//创建主窗口
		MainWnd mainWnd;
		mainWnd.Create(NULL, _T("MainWnd"), UI_WNDSTYLE_FRAME)
	}
}
#endif
