#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "MainWnd.h"

//给构造函数传入参数，引入数据库
//显式给出构造函数
//MainWnd::MainWnd(){}
MainWnd::MainWnd(MySQL* pMySQL)
:m_pMySQL(pMySQL)
{

}
MainWnd::~MainWnd()
{

}


//WindowImplBase提供三个纯虚函数

//xml文件对应的目录
 CDuiString MainWnd::GetSkinFolder()
{
	 return (_T(""));
}

 //xml文件名字
 CDuiString MainWnd::GetSkinFile()
 {
	 return (_T("MainWnd - 副本.xml"));
	 //return (_T("MainWnd.xml"));
	 //return (_T("CashierManage.xml"));
 }

 //窗口类的名字，在注册窗口时必须提供(自己不用管)
 LPCTSTR MainWnd::GetWindowClassName(void) const
 {
	 return (_T("MainWnd"));
 }




void MainWnd::Notify(TNotifyUI& msg)//员工操作
{
	CDuiString strName = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		//CDuiString strBTNName = msg.pSender->GetName();
		if (strName == _T("BTN_CLOSE"))
			PostQuitMessage(0);//Close();关闭窗口
		else if (strName == _T("BTN_MIN"))
			::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);//最小化窗口
		else if (strName == _T("BTN_SELECT"))
			//MessageBox(m_hWnd, _T("SelectBTN"), _T("cashier"), IDOK);
			SelectEmployeeInfo();
		else if (strName == _T("BTN_INSERT"))
			//MessageBox(m_hWnd, _T("InsertBTN"), _T("cashier"), IDOK);
			InsertEmployeeInfo();
		else if (strName == _T("BTN_UPDATE"))
			MessageBox(m_hWnd, _T("UpdateBTN"), _T("cashier"), IDOK);
		else if (strName == _T("BTN_DELETE"))
			//MessageBox(m_hWnd, _T("DeleteBTN"), _T("cashier"), IDOK);
			DeleteEmployeeInfo();
		else if (strName == _T("BTN_SELL_RECORD"))
			MessageBox(m_hWnd, _T("SellBTN"), _T("cashier"), IDOK);
	}
	else if (msg.sType = _T("selectchanged"))//option控件只能选一个，相互之间的切换单击时就是selectchanged
	{
		/*if (strName == _T("OPTION_EMPLOYEE"))
			MessageBox(m_hWnd, _T("EMPLOYEE"), _T("cashier"), IDOK);
		else
			MessageBox(m_hWnd, _T("GOODS"), _T("cashier"), IDOK);*/

		CTabLayoutUI* pTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("tablayout"));//找控件
		if (strName == _T("OPTION_EMPLOYEE"))
			pTab->SelectItem(0);//第一个option:对员工的操作
		else if (strName == _T("OPTION_GOODS"))
			pTab->SelectItem(1);//第二个option:对商品的操作
	}
	else if (msg.sType == _T("windowinit"))
	{
		//窗口初始化期间，将一些空间初始化
#if 1
		CComboBoxUI* pGender = (CComboBoxUI*)m_PaintManager.FindControl(_T("usergender"));
		pGender->SelectItem(0);

		CComboBoxUI* pPosition = (CComboBoxUI*)m_PaintManager.FindControl(_T("position"));
		pPosition->SelectItem(0);

		CComboBoxUI* pSelect = (CComboBoxUI*)m_PaintManager.FindControl(_T("COMOB_SELECT"));
		pSelect->SelectItem(0);
#endif
	}
}



void MainWnd::SelectEmployeeInfo()
{
	//vector<vector<string>> vRet1 = m_pMySQL->Select(strSQL);//用vRet来接受select的查询结果
	//if (vRet1.empty())
	//	return;

	CComboBoxUI* pCombo = (CComboBoxUI*)m_PaintManager.FindControl(_T("COMOB_SELECT"));
	CDuiString strStyle = pCombo->GetText();

	//构造数据库语句
	string strSQL("select * from employee ");

	if (strStyle == _T("无"))
		strSQL += ";";
	else if (strStyle == _T("姓名"))
	{
		strSQL += "where name = '";
		CEditUI* pstrName = (CEditUI*)m_PaintManager.FindControl(_T("username"));
		//CDuiString strName = pstrName->GetName();//获取控件名字
		CDuiString strName = pstrName->GetText();//区别是什么呀？//获取控件内容
		if (strName.IsEmpty())
		{
			MessageBox(m_hWnd, _T("请输入查询用户的名字"), _T("Cashier"), IDOK);
			return;
		}
		strSQL += UnicodeToANSI(strName);
		strSQL += "';";
	}													//edit是要输入文字，是gettext()
														//combo是要选择文字，所以是gettext()，得到的是combo选择选项中的文字；
	else if (strStyle == _T("性别"))					//而getname 此时获得的是combo默认的文本？还是没用呢？
	{													//还不确定对不对？需验证
		CComboBoxUI* pGender = (CComboBoxUI*)m_PaintManager.FindControl(_T("usergender"));
		CDuiString strGender = pGender->GetText();/////不能是getname
		if (strGender.IsEmpty())
		{
			MessageBox(m_hWnd, _T("请输入性别"), _T("Cashier"), IDOK);
			return;
		}

		strSQL += "where gender = '";
		
		//if (strGender == _T("男"))
		//{
		strSQL += UnicodeToANSI(strGender);
		strSQL += "';";
		//}
	}
	else if (strStyle == _T("薪资"))
	{
		CEditUI* pSalary = (CEditUI*)m_PaintManager.FindControl(_T("usersalary"));
		CDuiString strSalary = pSalary->GetText();
		//判空？
		if (strSalary.IsEmpty())
		{
			MessageBox(m_hWnd, _T("请输入薪资"), _T("Cashier"), IDOK);
			return;
		}
		strSQL += "where salary = '";
		
		strSQL += UnicodeToANSI(strSalary);//////这里到底需不需要转换--->需要
		strSQL += "';";

	}

	
	
	//先将原来的list清空
	CListUI* pListUI = (CListUI*)m_PaintManager.FindControl(_T("ListEmployeeInfo"));
	pListUI->RemoveAll();

	vector<vector<string>> vRet = m_pMySQL->Select(strSQL);//响应strSQL数据库语句，将结果保存到vRet中
	if (vRet.empty())
	{
		return;
	}

	//然后根据select查询结果插入到list
	for (size_t i = 0; i < vRet.size(); ++i)
	{
		vector<string>& strItem = vRet[i];//vRet[i]就是所查找到的一行数据
		CListTextElementUI* pData = new CListTextElementUI;
		pListUI->Add(pData);//每次入循环添加一行
		pData->SetAttribute(_T("align"), _T("center"));//文字对齐方式：居中

		//插入该行中的每一个元素（跟数据库对应）
		pData->SetText(0, ANSIToUnicode(strItem[1]));
		pData->SetText(1, ANSIToUnicode(strItem[2]));
		pData->SetText(2, ANSIToUnicode(strItem[3]));
		pData->SetText(3, ANSIToUnicode(strItem[5]));
		pData->SetText(4, ANSIToUnicode(strItem[6]));
		pData->SetText(5, ANSIToUnicode(strItem[7]));
	}

}

//不管是插入还是删除，先管数据库的插入删除，再管list
void MainWnd::DeleteEmployeeInfo()
{
	//1.获取当前选中
	CListUI* pListUI = (CListUI*)m_PaintManager.FindControl(_T("ListEmployeeInfo"));
	//pListUI->RemoveAll();
	int lineNo = pListUI->GetCurSel();//获取当前选中项
	CListTextElementUI* pRow = (CListTextElementUI*)pListUI->GetItemAt(lineNo);//获取当前选中整行
	CDuiString strName = pRow->GetText(0);//获取该行第一列元素(姓名)  //	【问题：删除时未获取到姓名（第一列），导致删除失败】

																
	//2.从数据库中将该员工的信息删除

	////构造SQL命令
	//string strSQL("delete from employee where id = ");
	//strSQL += lineNo;
	string strSQL("delete from employee where name = '");
	strSQL += UnicodeToANSI(strName);
	strSQL += ("';");


	
	//从数据库中将该条记录删除
	m_pMySQL->Delete(strSQL);//问题：数据库命令构造中已经有了delete，为什么封装数据库的时候还需要分Delete(strSQL)呢？-->只是调用了mysql_query并封装了一层判断错误mysql_error而已
	

	//3.从List中移除
	pListUI->RemoveAt(lineNo);//RemoveAt(行号):移除整行
}

void MainWnd::InsertEmployeeInfo()
{
	//1.从界面获取员工信息
	CEditUI* pstrname = (CEditUI*)m_PaintManager.FindControl(_T("username"));
	CDuiString strName = pstrname->GetText();

	CComboBoxUI* pstrGender = (CComboBoxUI*)m_PaintManager.FindControl(_T("usergender"));
	CDuiString strGender = pstrGender->GetText();
	CDuiString strBirthday = ((CEditUI*)m_PaintManager.FindControl(_T("userbirthday")))->GetText();//合二为一，注意括号，要把前面的所有的括起来
	CDuiString strPosition = ((CComboBoxUI*)m_PaintManager.FindControl(_T("position")))->GetText();//合二为一，注意括号，要把前面的所有的括起来
	CDuiString strTel = ((CEditUI*)m_PaintManager.FindControl(_T("telphone")))->GetText();//合二为一，注意括号，要把前面的所有的括起来
	CDuiString strSalary = ((CEditUI*)m_PaintManager.FindControl(_T("usersalary")))->GetText();//合二为一，注意括号，要把前面的所有的括起来

	CListUI* pListUI = (CListUI*)m_PaintManager.FindControl(_T("ListEmployeeInfo"));
	//pListUI->GetCount();

	char szCount[32] = { 0 };
	_itoa(pListUI->GetCount() + 3, szCount, 10);

	//2.构造SQL命令,将员工信息插入数据库                                   ///////////////【问题：插入语句写错，导致插入一行NULL】
	/*string strSQL("insert into employee values(id = ");
	strSQL += szCount;
	strSQL += ",name = '";
	strSQL += UnicodeToANSI(strName);
	strSQL += "',gender = '";
	strSQL += UnicodeToANSI(strGender);
	strSQL += "', birthday = '";
	strSQL += UnicodeToANSI(strBirthday);
	strSQL += "', password = '123',position = '";
	strSQL += UnicodeToANSI(strPosition);
	strSQL += "', telphone = '";
	strSQL += UnicodeToANSI(strTel);
	strSQL += "', salary = ";
	strSQL += UnicodeToANSI(strSalary);
	strSQL += ");";*/

	//修改mysql命令语句
	string strSQL("insert into employee values( ");
	strSQL += szCount;
	strSQL += ",'";
	strSQL += UnicodeToANSI(strName);
	strSQL += "', '";
	strSQL += UnicodeToANSI(strGender);
	strSQL += "',  '";
	strSQL += UnicodeToANSI(strBirthday);
	strSQL += "',  '123', '";
	strSQL += UnicodeToANSI(strPosition);
	strSQL += "',  '";
	strSQL += UnicodeToANSI(strTel);
	strSQL += "',  ";
	strSQL += UnicodeToANSI(strSalary);
	strSQL += ");";

	//响应SQL命令
	m_pMySQL->Insert(strSQL);


	//3.将该员工的信息插入到List
	CListTextElementUI* pItem = new CListTextElementUI;//获取list中整行信息
	//CListUI* pListUI = (CListUI*)m_PaintManager.FindControl(_T("ListEmployeeInfo"));
	pListUI->Add(pItem);//list中添加一行
	pItem->SetText(0, strName);
	pItem->SetText(1, strGender);
	pItem->SetText(2, strBirthday);
	pItem->SetText(3, strPosition);
	pItem->SetText(4, strTel);
	pItem->SetText(5, strSalary);

}
