#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "CashierWnd.h"

CCashierWnd::CCashierWnd(MySQL* pMySQL)
:m_pMySQL(pMySQL)
{

}
CCashierWnd::~CCashierWnd()
{

}

//WindowImplBase提供三个纯虚函数

//xml文件对应的目录
CDuiString CCashierWnd::GetSkinFolder()
{
	return (_T(""));
}

//xml文件名字
CDuiString CCashierWnd::GetSkinFile()
{
	//return (_T("Cashier.xml"));
	return (_T("CashierManage.xml"));
}

//窗口类的名字，在注册窗口时必须提供(自己不用管)
LPCTSTR CCashierWnd::GetWindowClassName() const
{
	return _T("CashierWnd");
}



void CCashierWnd::Notify(TNotifyUI& msg)
{
	CDuiString strName = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		//CDuiString strBTNName = msg.pSender->GetName();
		if (strName == _T("BTN_CLOSE"))
			PostQuitMessage(0);//Close();关闭窗口
		else if (strName == _T("BTN_MIN"))
			::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);//最小化窗口
		else if (strName == _T("BTN_ADD"))
			//MessageBox(m_hWnd, _T("Test BTN_ADD"), _T("cashier"), IDOK);
			AddGoodsCount();
		else if (strName == _T("BTN_SUB"))
			//MessageBox(m_hWnd, _T("Test BTN_SUB"), _T("cashier"), IDOK);
			SubGoodsCount();
		else if (strName == _T("BTN_SELECT"))
			//MessageBox(m_hWnd, _T("Test BTN_SELECT"), _T("cashier"), IDOK);
			SelectGoods();
		else if (strName == _T("BTN_COMMIT"))
			 //MessageBox(m_hWnd, _T("Test BTN_COMMIT"), _T("cashier"), IDOK);
			//还没写完
			CommitOrder();
		else if (strName == _T("BTN_CANCEL"))
			CancelOrder();
		else if (strName == _T("BTN_OK"))
			InsertGoodsList();
	}
}
void CCashierWnd::SelectGoods()
{
	//获取商品名称
	CDuiString strGoodsName = ((CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_NAME")))->GetText();

	//构造SQL语句
	string strSQL("select * from goodstable where GoodsName = '");
	strSQL += UnicodeToANSI(strGoodsName);
	strSQL += "';";

	//响应语句，修改构造函数
	vector<vector<string>> vRet = m_pMySQL->Select(strSQL);
	if (vRet.empty())
	{
		MessageBox(m_hWnd, _T("暂无此商品"), _T("cashier"), IDOK);
		return;
	}

	//将(查到的)剩余个数显示到界面编辑框中
	((CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_LEFT")))->SetText(ANSIToUnicode(vRet[0][7]));
}
void CCashierWnd::AddGoodsCount()
{
	//库存减1
	CEditUI* pGoodLeft = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_LEFT"));
	CDuiString strLeft = pGoodLeft->GetText();//拿到的是Unicode类型
	if (strLeft == _T("0"))
	{
		MessageBox(m_hWnd, _T("库存量不足"), _T("cashier"), IDOK);
		return;
	}
	int count = 0;
	count = atoi(UnicodeToANSI(strLeft).c_str());//字符串转换为数字        [去转]
	--count;
	strLeft.Format(_T("%d"), count);//将count自减之后格式化转到strCount    [回转]
	pGoodLeft->SetText(strLeft);//将strLeft写入EDIT_GOODS_LEFT控件

	//(要出售的)商品个数加1
	CEditUI* pGoodCount = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_COUNT"));
	CDuiString strCount = pGoodCount->GetText();//拿到的是Unicode类型
	
	int sellcount = atoi(UnicodeToANSI(strCount).c_str());//字符串转换为数字[去转]
	++sellcount;
	strCount.Format(_T("%d"), sellcount);//[回转]
	pGoodCount->SetText(strCount);//将strCount写入EDIT_GOODS_COUNT控件

}
void CCashierWnd::SubGoodsCount()
{
	//商品个数减去1
	CEditUI* pGoodCount = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_COUNT"));
	CDuiString strCount = pGoodCount->GetText();//拿到的是Unicode类型
	if (strCount == _T("0"))
	{
		MessageBox(m_hWnd, _T("商品个数与已经为0"), _T("cashier"), IDOK);
		return;
	}
	int count = atoi(UnicodeToANSI(strCount).c_str());//字符串转换为数字
	--count;
	strCount.Format(_T("%d"), count);//将count自增之后格式化转到strCount
	pGoodCount->SetText(strCount);//将strCount写入到控件pGoodCount

	//库存加1
	CEditUI* pGoodLeft = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_LEFT"));
	CDuiString strLeft = pGoodLeft->GetText();//拿到的是Unicode类型

	count = atoi(UnicodeToANSI(strLeft).c_str());//字符串转换为数字
	++count;
	strLeft.Format(_T("%d"), count);//将count自增之后格式化转到strLeft
	pGoodLeft->SetText(strLeft);//将strCount写入到控件pGoodCount
}

void CCashierWnd::InsertGoodsList()//点击OK之后将该（欲出售）商品加入list
{
	//1.从界面获取商品名称以及购买数量
	CEditUI* pstrGoodsName = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_NAME"));
	//CDuiString strGoodsName = pstrGoodsName->GetName();//是GetText()
	CDuiString strGoodsName = pstrGoodsName->GetText();
	CEditUI* pstrGoodsCount = (CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_COUNT")); 
	CDuiString strCount = pstrGoodsCount->GetText();

	//2.从数据库获取商品的价格以及计量单位
	string strSQL("select * from goodstable where GoodsName = '");
	strSQL += UnicodeToANSI(strGoodsName);
	strSQL += "';";
	vector<vector<string>> vRet = m_pMySQL->Select(strSQL);//响应数据库语句,用vRet接受select结果
															//后面利用vRet来进行操作
	//3.合计价格
	int count = atoi(UnicodeToANSI(strCount).c_str());
	double price = atof(vRet[0][5].c_str());//atof:字符串转为double类型
	//int price = atoi(vRet[0][5].c_str());//atoi:字符串转为int类型

	price = count * price;
	CDuiString strPrice;
	strPrice.Format(_T("%lf"), price);//double类型输出格式
	//strPrice.Format(_T("%d"), price);


	//4.将信息更新到List中
	CListTextElementUI* pItem = new CListTextElementUI;//List中的一行（元素）
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("OrderList"));
	pList->Add(pItem);//pItem表示List中的一行，下标0,1,2,3,4,...（ListCount）
	pItem->SetText(0, strGoodsName);
	pItem->SetText(1, ANSIToUnicode(vRet[0][5]));
	pItem->SetText(2, strCount);
	pItem->SetText(3, ANSIToUnicode(vRet[0][6]));
	pItem->SetText(4, strPrice);

	//5.将商品数量以及名称的编辑框清零
	((CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_NAME")))->SetText(_T(""));
	((CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_LEFT")))->SetText(_T(""));
	((CEditUI*)m_PaintManager.FindControl(_T("EDIT_GOODS_COUNT")))->SetText(_T(""));

}

void CCashierWnd::CancelOrder()//清空欲出售list中所有商品
{
	//清空所有商品
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("OrderList"));//找List控件
	pList->RemoveAll();

}

void CCashierWnd::CommitOrder()//将list中所列商品出售并更新数据库
{
	//合并总价格
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("OrderList"));
	int count = pList->GetCount();//GetCount的返回值类型是int，故不需要转换

	double totalPrice = 0;
	for (int i = 0; i < count; ++i)
	{
		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);//获取list中第i行数据
		CDuiString strPrice = (CDuiString)pItem->GetText(4);//获取该行中第5列(0,1,2,3,4)从0开始算
		totalPrice += atof(UnicodeToANSI(strPrice).c_str());//将每行中的第5列（总价格）相加
	}
	CDuiString strTotalPrice;
	strTotalPrice.Format(_T("%.02lf"), totalPrice);//将double类型转为CDuiString存入strTotalPrice
	((CEditUI*)m_PaintManager.FindControl(_T("EDIT_TOTAL")))->SetText(strTotalPrice);//将strTotalPrice写入EDIT_TOTAL控件

	//更新商品的数据库
	for (int i = 0; i < count; ++i)
	{
		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);///////获取list中每行数据到pItem，类型是CListTextElementUI*
		CDuiString strCount = pItem->GetText(2);//获取每行第3列元素（本行该出售商品数量）
		CDuiString strName = pItem->GetText(0);//获取没行第1列（本行出售商品的名称）

		//int sellcount = atoi(UnicodeToANSI(strCount).c_str());//string-->int不过这一步不需要，导致sellcount会变为\x(sellcount)

		//构造SQL语句【问题：构造strSQL命令语句时int/double也要当成ANSI字符串对待】
		string strSQL = (" update goodstable set Inventory = (Inventory - ");
		//strSQL += sellcount;
		strSQL += UnicodeToANSI(strCount);
		strSQL += ") where GoodsName='";
		strSQL += UnicodeToANSI(strName);
		strSQL += "';";
		m_pMySQL->Update(strSQL);
	}
	//插入本次销售记录

	//弹窗提示
	if (count != 0)
		MessageBox(m_hWnd, _T("出售提交成功"), _T("Cashier"), IDOK);
	else if (pList->IsVisible())
		MessageBox(m_hWnd, _T("请选择要出售的商品并加入清单"), _T("Cashier"), IDOK);
	else
		MessageBox(m_hWnd, _T("出售商品无库存"), _T("Cashier"), IDOK);
}