#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "LogIn.h"
#include "MainWnd.h"
#include "CashierWnd.h"


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	
	MySQL mysql;
	mysql.ConnectMySql("localhost", "root", "123", "item1");

	//LogInWnd´°¿Ú
	CPaintManagerUI::SetInstance(hInstance);
	LogInWnd duiFrame(&mysql);
	duiFrame.Create(NULL, _T("LOGINWND"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();


	return 0;
}