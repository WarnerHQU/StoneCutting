// Orthgonal.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "Orthgonal.h"
#include "afxdialogex.h"


// COrthgonal 对话框

IMPLEMENT_DYNAMIC(COrthgonal, CDialogEx)

COrthgonal::COrthgonal(CWnd* pParent /*=NULL*/)
	: CDialogEx(COrthgonal::IDD, pParent)
	, m_OrthgonalWorkBlankLength(0)
	, m_OrthgonalWorkBlankWidth(0)
	, m_OrthgonalWorkBlankHeigh(0)
	, m_TrapezoidType(0)
	, m_TrapezoidAngle(0)
	, m_OrthgonalSawWidth(0)
	
	, m_OrthgonalProcessStyle(0)
	, m_OrthgonalYShort(0)
	, m_OrthgonalYLong(0)
	, m_OrthgonalYGap(0)
	, m_OrthgonalYFeedLoad(0)
	, m_OrthgonalXShort(0)
	, m_OrthgonalXLong(0)
	, m_OrthgonalXGap(0)
	, m_OrthgonalXFeedLoad(0)
{
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();


}

COrthgonal::~COrthgonal()
{
}

void COrthgonal::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_OrthgonalWorkBlankLength);
	DDX_Text(pDX, IDC_EDIT2, m_OrthgonalWorkBlankWidth);
	DDX_Text(pDX, IDC_EDIT3, m_OrthgonalWorkBlankHeigh);
	DDX_Text(pDX, IDC_EDIT4, m_TrapezoidAngle);
	DDX_Text(pDX, IDC_EDIT13, m_OrthgonalSawWidth);
	DDX_Radio(pDX, IDC_RADIO1, m_TrapezoidType);
	DDX_Radio(pDX, IDC_RADIO3, m_OrthgonalProcessStyle);
	DDX_Text(pDX, IDC_EDIT9, m_OrthgonalYShort);
	DDX_Text(pDX, IDC_EDIT10, m_OrthgonalYLong);
	DDX_Text(pDX, IDC_EDIT12, m_OrthgonalYGap);
	DDX_Text(pDX, IDC_EDIT15, m_OrthgonalYFeedLoad);
	DDX_Text(pDX, IDC_EDIT5, m_OrthgonalXShort);
	DDX_Text(pDX, IDC_EDIT6, m_OrthgonalXLong);
	DDX_Text(pDX, IDC_EDIT8, m_OrthgonalXGap);
	DDX_Text(pDX, IDC_EDIT14, m_OrthgonalXFeedLoad);
}


BEGIN_MESSAGE_MAP(COrthgonal, CDialogEx)
END_MESSAGE_MAP()


// COrthgonal 消息处理程序


void COrthgonal::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void COrthgonal::OnOK()
{
	UpdateData(TRUE);

	pDoc->tool.SetOrthgonalWorkBlankLength(m_OrthgonalWorkBlankLength);
	pDoc->tool.SetOrthgonalWorkBlankWidth(m_OrthgonalWorkBlankWidth);
	pDoc->tool.SetOrthgonalWorkBlankHigh(m_OrthgonalWorkBlankHeigh);

	pDoc->tool.SetOrthgonalTrapezoidType(m_TrapezoidType);
	pDoc->tool.SetOrthgonalTrapezoidAngle(m_TrapezoidAngle);
	pDoc->tool.SetOrthgonalSawWidth(m_OrthgonalSawWidth);

	pDoc->tool.SetOrthgonalProcessStyle(m_OrthgonalProcessStyle);

	pDoc->tool.SetOrthgonalYLong(m_OrthgonalYLong);
	pDoc->tool.SetOrthgonalYShort(m_OrthgonalYShort);
	pDoc->tool.SetOrthgonalYGap(m_OrthgonalYGap);
	pDoc->tool.SetOrthgonalYFeedLoad(m_OrthgonalYFeedLoad);

	pDoc->tool.SetOrthgonalXLong(m_OrthgonalXLong);
	pDoc->tool.SetOrthgonalXShort(m_OrthgonalXShort);
	pDoc->tool.SetOrthgonalXGap(m_OrthgonalXGap);
	pDoc->tool.SetOrthgonalXFeedLoad(m_OrthgonalXFeedLoad);

	DestroyWindow();
	//CDialogEx::OnOK();
}


void COrthgonal::OnCancel()
{
	
	DestroyWindow();
	//CDialogEx::OnCancel();
}


BOOL COrthgonal::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_OrthgonalWorkBlankHeigh = 100;
	m_OrthgonalWorkBlankWidth = 1000;
	m_OrthgonalWorkBlankLength = 1000;

	m_OrthgonalSawWidth = 10;
	m_TrapezoidAngle = 80;

	m_OrthgonalYLong = 100;
	m_OrthgonalYShort = 0;
	m_OrthgonalYGap = 100;
	m_OrthgonalYFeedLoad = 6000;//微米


	m_OrthgonalXLong = 100;
	m_OrthgonalXShort = 0;
	m_OrthgonalXGap = 100;
	m_OrthgonalXFeedLoad = 6000;//微米

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL COrthgonal::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);

			m_OrthgonalYShort = m_OrthgonalYLong - 2 * m_OrthgonalWorkBlankHeigh/tan(m_TrapezoidAngle*3.1415926/180);

			m_OrthgonalXShort = m_OrthgonalXLong - 2 * m_OrthgonalWorkBlankHeigh/tan(m_TrapezoidAngle*3.1415926/180);

			UpdateData(FALSE);
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
