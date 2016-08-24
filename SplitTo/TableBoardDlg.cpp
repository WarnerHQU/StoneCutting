// TableBoardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "TableBoardDlg.h"
#include "afxdialogex.h"
#include"Utils.h"

// CTableBoardDlg 对话框

IMPLEMENT_DYNAMIC(CTableBoardDlg, CDialogEx)

//静态成员变量需要放在cpp的类外进行初始化操作
CTableBoardDlg* CTableBoardDlg::m_TableBoard = NULL;

CTableBoardDlg::CTableBoardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTableBoardDlg::IDD, pParent)
	, m_BoardPieceLength(0)
	, m_BoardPieceWidth(0)
	, m_BoardPieceHighth(0)
	, m_BoardOutLeft(0)
	, m_BoardInLeft(0)
	, m_FeedSpeed_Horizon(0)
	, m_FeedTime_Horizon(0)
	, m_FeedLoad_Horizon(0)
	, m_FeedDis_Horizon(0)
	, m_FeedSpeed_Vertical(0)
	, m_FeedTime_Vertical(0)
	, m_FeedLoad_Vertical(0)
	, m_ToolDiamand(0)
	, m_MillCutSpeed(0)
	, m_MillRotation(0)
	, m_MillLoadPerTooth(0)
	, m_FeedLoadPerMin(0)
	, m_MillCutLoad_Horizon(0)
	, m_MillCutDepth(0)
{
	//m_TableBoard = NULL;
	//m_TempArray = new CPtrArray();

	m_ShowTips = false;
	m_TempArray = NULL;
	m_BackupArray = NULL;

	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

}

CTableBoardDlg::~CTableBoardDlg()
{
	/*
	if (m_TempArray != NULL)
		delete m_TempArray;
	if (m_BackupArray != NULL)
		delete m_BackupArray;
	
	if (m_TableBoard != NULL)
		delete m_TableBoard;*/
}

void CTableBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_BoardPieceLength);
	DDX_Text(pDX, IDC_EDIT2, m_BoardPieceWidth);
	DDX_Text(pDX, IDC_EDIT3, m_BoardPieceHighth);
	DDX_Text(pDX, IDC_EDIT4, m_BoardOutLeft);
	DDX_Text(pDX, IDC_EDIT9, m_BoardInLeft);
	DDX_Text(pDX, IDC_EDIT5, m_FeedSpeed_Horizon);
	DDX_Text(pDX, IDC_EDIT6, m_FeedTime_Horizon);
	DDX_Text(pDX, IDC_EDIT7, m_FeedLoad_Horizon);
	DDX_Text(pDX, IDC_EDIT8, m_FeedDis_Horizon);
	DDX_Text(pDX, IDC_EDIT10, m_FeedSpeed_Vertical);
	DDX_Text(pDX, IDC_EDIT11, m_FeedTime_Vertical);
	DDX_Text(pDX, IDC_EDIT12, m_FeedLoad_Vertical);
	DDX_Text(pDX, IDC_EDIT14, m_ToolDiamand);
	DDX_Text(pDX, IDC_EDIT13, m_MillCutSpeed);
	DDX_Text(pDX, IDC_EDIT16, m_MillRotation);
	DDX_Text(pDX, IDC_EDIT15, m_MillLoadPerTooth);
	DDX_Text(pDX, IDC_EDIT17, m_FeedLoadPerMin);
	DDX_Text(pDX, IDC_EDIT19, m_MillCutLoad_Horizon);
	DDX_Text(pDX, IDC_EDIT18, m_MillCutDepth);
}


BEGIN_MESSAGE_MAP(CTableBoardDlg, CDialogEx)
	ON_BN_CLICKED(IDC_TB_OPENFILE, &CTableBoardDlg::OnBnClickedTbOpenfile)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_EXTEND, &CTableBoardDlg::OnBnClickedExtend)
	ON_BN_CLICKED(IDC_SHRINK, &CTableBoardDlg::OnBnClickedShrink)
	ON_BN_CLICKED(IDC_TIPS, &CTableBoardDlg::OnBnClickedTips)
END_MESSAGE_MAP()


// CTableBoardDlg 消息处理程序


void CTableBoardDlg::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类

	delete this;

	CDialogEx::PostNcDestroy();
}


void CTableBoardDlg::OnBnClickedTbOpenfile()
{
	// TODO:  在此添加控件通知处理程序代码
	//CUtils::OpenFile();
	CString name = _T("谢峰台面板打开对话框");
	m_TempArray=CUtils::OpenFile(this->m_hWnd,name,1100);
	//指针有内容的话就进行计算
	if (m_TempArray->GetSize())
	{
		//备份指针的内容
		m_BackupArray = m_TempArray;
		m_BoundingBox_2D = this->BoundingBox(m_TempArray);
		m_center.x = m_BoundingBox_2D.left + (m_BoundingBox_2D.right - m_BoundingBox_2D.left) / 2;
		m_center.y = m_BoundingBox_2D.bottom + (m_BoundingBox_2D.top - m_BoundingBox_2D.bottom) / 2;

		if (m_center.x < m_center.y)
		{
			ratio = m_center.x / 120;
		}
		else
		{
			ratio = m_center.y / 130;
		}
		Invalidate();
	}
	else//无内容的话，就用以前存储的值就可以。
	{
		m_TempArray = m_BackupArray;
	}
	
}


//单例
CTableBoardDlg* CTableBoardDlg::GetTableBoardInstance()
{
	if (m_TableBoard == NULL)//判断第一次调用
	{
		m_TableBoard = new CTableBoardDlg();
	}
	return m_TableBoard;
}

void CTableBoardDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	UpdateData(TRUE);


	if (m_TempArray->GetSize())
	{
		//CTableBoardProcess::GetTableProcessInstance()->RegonizeProfile(m_TempArray);
		//CTableBoardProcess::GetTableProcessInstance()->ChangeSegments(m_TempArray);
		

		CTableBoardProcess::GetTableProcessInstance()->SetBoardLength(m_BoardPieceLength);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardWidth(m_BoardPieceWidth);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardHeigh(m_BoardPieceHighth);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardInLeft(m_BoardInLeft);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardOutLeft(m_BoardOutLeft);

		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedSpeedHorizon(m_FeedSpeed_Horizon);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedTimeHorizon(m_FeedTime_Horizon);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedLoadHorizon(m_FeedLoad_Horizon);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedDisHorizon(m_FeedDis_Horizon);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedSpeedVertical(m_FeedSpeed_Vertical);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedTimeVertical(m_FeedTime_Vertical);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardFeedLoadVertical(m_FeedLoad_Vertical);

		CTableBoardProcess::GetTableProcessInstance()->SetBoardToolDiamnd(m_ToolDiamand);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardMillCutDepthHorizon(m_MillCutLoad_Horizon);
		CTableBoardProcess::GetTableProcessInstance()->SetBoardMillCutDepthVertical(m_MillCutDepth);



		CTableBoardProcess::GetTableProcessInstance()->InitData(m_TempArray);
	}
	//DestroyWindow();
	CDialogEx::OnOK();
}


void CTableBoardDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

	//DestroyWindow();//在这里不可用销毁窗口，因为是单例的，销毁后，显示不出来

	CDialogEx::OnCancel();
}


void CTableBoardDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

	CDC* pDC;
	pDC = GetDlgItem(IDC_TABLE_PICTURE)->GetDC();
	GetDlgItem(IDC_TABLE_PICTURE)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_TABLE_PICTURE)->UpdateWindow();
	GetDlgItem(IDC_TABLE_PICTURE)->GetClientRect(rec);

	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&redpen);
	CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
	CBrush* pOldBrush = pDC->SelectObject(pBrush);

	MyDraw(pDC);
	if (m_ShowTips)
	{
		SortDraw(pDC, m_TempArray);
		m_BoardPieceLength = m_BoundingBox_2D.Width();
		m_BoardPieceWidth = fabsl(m_BoundingBox_2D.Height());
		UpdateData(false);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

}

CRect CTableBoardDlg::BoundingBox(CPtrArray* Array)
{
	CRect Zone;

	double Xmin, Ymin, Xmax, Ymax;//将起始元素的首位点设置为最大最小点
	if (((CLine*)Array->GetAt(0))->flag == 1)
	{
		Xmin = ((CLine*)Array->GetAt(0))->m_begin.x;
		Xmax = ((CLine*)Array->GetAt(0))->m_end.x;
		Ymin = ((CLine*)Array->GetAt(0))->m_begin.y;
		Ymax = ((CLine*)Array->GetAt(0))->m_end.y;
	}
	else if (((CArc*)Array->GetAt(0))->flag == 2)
	{
		if (((CArc*)Array->GetAt(0))->m_Tudu == 0)
		{
			CArc::Calculate(((CArc*)Array->GetAt(0))->m_center, ((CArc*)Array->GetAt(0))->m_radio, ((CArc*)Array->GetAt(0))->m_begin_angle,
				((CArc*)Array->GetAt(0))->m_end_angle, *Array, 0);
			Xmin = ((CArc*)Array->GetAt(0))->m_begin.x;
			Xmax = ((CArc*)Array->GetAt(0))->m_end.x;
			Ymin = ((CArc*)Array->GetAt(0))->m_begin.y;
			Ymax = ((CArc*)Array->GetAt(0))->m_end.y;
		}
		else
		{
			Xmin = ((CArc*)Array->GetAt(0))->m_begin.x;
			Xmax = ((CArc*)Array->GetAt(0))->m_end.x;
			Ymin = ((CArc*)Array->GetAt(0))->m_begin.y;
			Ymax = ((CArc*)Array->GetAt(0))->m_end.y;
		}
	}
	else if (((CCircle*)Array->GetAt(0))->flag == 3)
	{
		Xmin = ((CCircle*)Array->GetAt(0))->m_center.x - ((CCircle*)Array->GetAt(0))->m_radio;
		Xmax = ((CCircle*)Array->GetAt(0))->m_center.x + ((CCircle*)Array->GetAt(0))->m_radio;
		Ymin = ((CCircle*)Array->GetAt(0))->m_center.y - ((CCircle*)Array->GetAt(0))->m_radio;
		Ymax = ((CCircle*)Array->GetAt(0))->m_center.y + ((CCircle*)Array->GetAt(0))->m_radio;
	}


	/*搜索Array中的元素比较X\Y最大最小值*/
	for (int i = 0; i != Array->GetSize(); ++i)
	{

		if (((CLine*)Array->GetAt(i))->flag == 1)
		{
			if (((CLine*)Array->GetAt(i))->m_begin.x < Xmin)
				Xmin = ((CLine*)Array->GetAt(i))->m_begin.x;
			else if (((CLine*)Array->GetAt(i))->m_begin.x>Xmax)
				Xmax = ((CLine*)Array->GetAt(i))->m_begin.x;

			if (((CLine*)Array->GetAt(i))->m_end.x < Xmin)
				Xmin = ((CLine*)Array->GetAt(i))->m_end.x;
			else if (((CLine*)Array->GetAt(i))->m_end.x>Xmax)
				Xmax = ((CLine*)Array->GetAt(i))->m_end.x;

			if (((CLine*)Array->GetAt(i))->m_begin.y < Ymin)
				Ymin = ((CLine*)Array->GetAt(i))->m_begin.y;
			else if (((CLine*)Array->GetAt(i))->m_begin.y>Ymax)
				Ymax = ((CLine*)Array->GetAt(i))->m_begin.y;



			if (((CLine*)Array->GetAt(i))->m_end.y < Ymin)
				Ymin = ((CLine*)Array->GetAt(i))->m_end.y;
			else if (((CLine*)Array->GetAt(i))->m_end.y>Ymax)
				Ymax = ((CLine*)Array->GetAt(i))->m_end.y;

		}
		else if (((CArc*)Array->GetAt(i))->flag == 2)
		{
			if (((CArc*)Array->GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				CToolPath t;
				vector<CPoint3D> pt = t.ArcDiscrete(((CArc*)Array->GetAt(i))->m_center, ((CArc*)Array->GetAt(i))->m_radio, ((CArc*)Array->GetAt(i))->m_begin_angle,
					((CArc*)Array->GetAt(i))->m_end_angle, *Array, i, 80, 6000);
				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end(); ++it)
				{
					if (it->x > Xmax)
						Xmax = it->x;
					if (it->x < Xmin)
						Xmin = it->x;
					if (it->y>Ymax)
						Ymax = it->y;
					if (it->y < Ymin)
						Ymin = it->y;
				}

			}
			else//多义线段圆弧
			{
				CToolPath t;
				vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)Array->GetAt(i))->m_begin, ((CArc*)Array->GetAt(i))->m_end,
					((CArc*)Array->GetAt(i))->m_Tudu, 80, 6000);

				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end(); ++it)
				{
					if (it->x > Xmax)
						Xmax = it->x;
					if (it->x < Xmin)
						Xmin = it->x;
					if (it->y>Ymax)
						Ymax = it->y;
					if (it->y < Ymin)
						Ymin = it->y;
				}
			}
		}
	}
	Zone.left = Xmin;
	Zone.top = Ymax;
	Zone.right = Xmax;
	Zone.bottom = Ymin;
	return Zone;
}

void CTableBoardDlg::MyDraw(CDC* pDC)
{
	if (m_TempArray != NULL)
	{
		for (int i = 0; i != m_TempArray->GetSize(); ++i)
		{
			if (((CLine*)m_TempArray->GetAt(i))->flag == 1)//直线
			{
				pDC->MoveTo(((CLine*)m_TempArray->GetAt(i))->m_begin.x / ratio, rec.Height() - ((CLine*)m_TempArray->GetAt(i))->m_begin.y / ratio);
				pDC->LineTo(((CLine*)m_TempArray->GetAt(i))->m_end.x / ratio, rec.Height() - ((CLine*)m_TempArray->GetAt(i))->m_end.y / ratio);

				CPoint3D pt;
				pt = ((CLine*)m_TempArray->GetAt(i))->m_end;

			}
			else if (((CArc*)m_TempArray->GetAt(i))->flag == 2)//圆弧
			{
				if (((CArc*)m_TempArray->GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					CToolPath t;
					vector<CPoint3D> pt = t.ArcDiscrete(((CArc*)m_TempArray->GetAt(i))->m_center, ((CArc*)m_TempArray->GetAt(i))->m_radio, ((CArc*)m_TempArray->GetAt(i))->m_begin_angle,
						((CArc*)m_TempArray->GetAt(i))->m_end_angle, *m_TempArray, i, 80, 6000);
					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
					{
						pDC->MoveTo(it->x / ratio, rec.Height() - it->y / ratio);
						pDC->LineTo((it + 1)->x / ratio, rec.Height() - (it + 1)->y / ratio);
					}
				}
				else//多义线段圆弧
				{
					CToolPath t;
					vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)m_TempArray->GetAt(i))->m_begin, ((CArc*)m_TempArray->GetAt(i))->m_end,
						((CArc*)m_TempArray->GetAt(i))->m_Tudu, 80, 6000);

					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
					{
						pDC->MoveTo(it->x / ratio, rec.Height() - it->y / ratio);
						pDC->LineTo((it + 1)->x / ratio, rec.Height() - (it + 1)->y / ratio);
					}

				}
			}
			else if (((CCircle*)m_TempArray->GetAt(i))->flag == 3)
			{
				CRect CircleRect;
				CircleRect.left = (((CCircle*)m_TempArray->GetAt(i))->m_center.x - ((CCircle*)m_TempArray->GetAt(i))->m_radio) / ratio;
				CircleRect.top = rec.Height() - (((CCircle*)m_TempArray->GetAt(i))->m_center.y + ((CCircle*)m_TempArray->GetAt(i))->m_radio) / ratio;
				CircleRect.right = (((CCircle*)m_TempArray->GetAt(i))->m_center.x + ((CCircle*)m_TempArray->GetAt(i))->m_radio) / ratio;
				CircleRect.bottom = rec.Height() - (((CCircle*)m_TempArray->GetAt(i))->m_center.y - ((CCircle*)m_TempArray->GetAt(i))->m_radio) / ratio;
				pDC->Ellipse(CircleRect);
			}
		}
	}
}


void CTableBoardDlg::OnBnClickedExtend()
{
	if (ratio != 0)
	{
		ratio *= 0.9;
		InvalidateRect(rec);
	}
}


void CTableBoardDlg::OnBnClickedShrink()
{
	if (ratio != 0)
	{
		ratio /= 0.9;
		InvalidateRect(rec);
	}
}


void CTableBoardDlg::OnBnClickedTips()
{
	if (m_ShowTips)
	{
		m_ShowTips = false;
	}
	else
	{
		m_ShowTips = true;
	}
	InvalidateRect(rec);
}

void CTableBoardDlg::SortDraw(CDC* dc, CPtrArray* m_Array)
{
	CRect m_RectBoardProcess;
	m_RectBoardProcess = m_BoundingBox_2D;


	m_RectBoardProcess.left = m_RectBoardProcess.left / ratio;
	m_RectBoardProcess.right = m_RectBoardProcess.right / ratio;
	m_RectBoardProcess.top = rec.Height() - m_RectBoardProcess.top / ratio;
	m_RectBoardProcess.bottom = rec.Height() - m_RectBoardProcess.bottom / ratio;

	//画标线
	DrawaLabel(dc, m_RectBoardProcess);

	CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pPen = dc->SelectObject(&GreenPen);
	dc->Rectangle(m_RectBoardProcess);
	dc->SelectObject(pPen);

}

void CTableBoardDlg::DrawaLabel(CDC* m_pDC, CRect Rect)
{
	CFont font;
	font.CreateFont(15,                                          //   nHeight     
		0,                                                   //   nWidth     
		0,                                                   //   nEscapement   
		0,                                                   //   nOrientation     
		FW_NORMAL,                                   //   nWeight     
		FALSE,                                           //   bItalic     
		FALSE,                                           //   bUnderline     
		0,                                                   //   cStrikeOut     
		ANSI_CHARSET,                             //   nCharSet     
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision     
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision     
		DEFAULT_QUALITY,                       //   nQuality     
		DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily       
		_T("宋体"));

	CPen Pen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pOldPen = m_pDC->SelectObject(&Pen);

	CPoint p1, p2, p3, p4;
	p1.x = Rect.right + 5;
	p1.y = Rect.top;

	p2.x = Rect.right + 5;
	p2.y = Rect.bottom;

	p3.x = Rect.left;
	p3.y = Rect.top - 5;

	p4.x = Rect.right;
	p4.y = Rect.top - 5;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);
	DrawArrow(m_pDC, p3, p4, 45, 5);

	//画延伸线
	m_pDC->MoveTo(Rect.right, Rect.bottom - 1);
	m_pDC->LineTo(Rect.right + 6, Rect.bottom - 1);

	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right + 6, Rect.top);


	//标注线旁写上“H”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.right + 15, Rect.top + Rect.Height()*0.3, _T("W"));


	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left, Rect.top - 6);

	m_pDC->MoveTo(Rect.right - 1, Rect.top);
	m_pDC->LineTo(Rect.right - 1, Rect.top - 6);

	//标注线上写上“W”
	m_pDC->TextOutW(Rect.left + Rect.Width()*0.5, Rect.top - 20, _T("L"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);

}

void CTableBoardDlg::DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length)
{
	theta = 3.1415926*theta / 180;//转换为弧度
	double Px, Py, P1x, P1y, P2x, P2y;
	//以P2为原点得到向量P2P1（P）
	Px = p1.x - p2.x;
	Py = p1.y - p2.y;
	//向量P旋转theta角得到向量P1
	P1x = Px*cos(theta) - Py*sin(theta);
	P1y = Px*sin(theta) + Py*cos(theta);
	//向量P旋转-theta角得到向量P2
	P2x = Px*cos(-theta) - Py*sin(-theta);
	P2y = Px*sin(-theta) + Py*cos(-theta);
	//伸缩向量至制定长度
	double x1, x2;
	x1 = sqrt(P1x*P1x + P1y*P1y);
	P1x = P1x*length / x1;
	P1y = P1y*length / x1;
	x2 = sqrt(P2x*P2x + P2y*P2y);
	P2x = P2x*length / x2;
	P2y = P2y*length / x2;
	//平移变量到直线的末端
	P1x = P1x + p2.x;
	P1y = P1y + p2.y;
	P2x = P2x + p2.x;
	P2y = P2y + p2.y;

	CPen pen, pen1, *oldpen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pen1.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	oldpen = m_pDC->SelectObject(&pen);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(p2.x, p2.y);
	m_pDC->SelectObject(&pen1);
	m_pDC->MoveTo(p2.x, p2.y);
	m_pDC->LineTo(P1x, P1y);
	m_pDC->MoveTo(p2.x, p2.y);
	m_pDC->LineTo(P2x, P2y);
	m_pDC->SelectObject(oldpen);


	//以P1为原点得到向量P1P2（P）
	Px = p2.x - p1.x;
	Py = p2.y - p1.y;
	//向量P旋转theta角得到向量P1
	P1x = Px*cos(theta) - Py*sin(theta);
	P1y = Px*sin(theta) + Py*cos(theta);
	//向量P旋转-theta角得到向量P2
	P2x = Px*cos(-theta) - Py*sin(-theta);
	P2y = Px*sin(-theta) + Py*cos(-theta);
	//伸缩向量至制定长度
	x1 = sqrt(P1x*P1x + P1y*P1y);
	P1x = P1x*length / x1;
	P1y = P1y*length / x1;
	x2 = sqrt(P2x*P2x + P2y*P2y);
	P2x = P2x*length / x2;
	P2y = P2y*length / x2;
	//平移变量到直线的首端
	P1x = P1x + p1.x;
	P1y = P1y + p1.y;
	P2x = P2x + p1.x;
	P2y = P2y + p1.y;


	oldpen = m_pDC->SelectObject(&pen1);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(P1x, P1y);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(P2x, P2y);
	m_pDC->SelectObject(oldpen);

}

BOOL CTableBoardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_FeedSpeed_Horizon = 6000;
	m_FeedTime_Horizon = 60;
	m_FeedSpeed_Vertical = 6000;
	m_FeedTime_Vertical = 60;
	m_BoardInLeft = 20;
	m_BoardOutLeft = 20;
	m_BoardPieceHighth = 100;

	m_FeedDis_Horizon = 30;
	
	m_ToolDiamand = 20;
	m_MillCutDepth = 10;
	m_MillCutLoad_Horizon = 30;//得与锯片的每层切割距离相同

	m_MillRotation = 300;
	m_MillLoadPerTooth = 0.07;//mm/z

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL CTableBoardDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);

			m_FeedLoad_Horizon = m_FeedSpeed_Horizon*m_FeedTime_Horizon / 60;
			m_FeedLoad_Vertical = m_FeedSpeed_Vertical*m_FeedTime_Vertical / 60;

			m_MillCutLoad_Horizon =m_FeedDis_Horizon;
			m_MillCutSpeed = (3.1415926*m_ToolDiamand*m_MillRotation) / 1000;
			//假定6齿
			m_FeedLoadPerMin = m_MillLoadPerTooth * 6 * m_MillRotation;

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
