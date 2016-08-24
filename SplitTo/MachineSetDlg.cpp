// MachineSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "MachineSetDlg.h"
#include "afxdialogex.h"


// CMachineSetDlg 对话框

IMPLEMENT_DYNAMIC(CMachineSetDlg, CDialogEx)

CMachineSetDlg::CMachineSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMachineSetDlg::IDD, pParent)
{

}

CMachineSetDlg::~CMachineSetDlg()
{

}

void CMachineSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMachineSetDlg, CDialogEx)
END_MESSAGE_MAP()


// CMachineSetDlg 消息处理程序


void CMachineSetDlg::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}
