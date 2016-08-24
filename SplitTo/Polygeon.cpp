// Polygeon.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "Polygeon.h"
#include "afxdialogex.h"


// CPolygeon 对话框

IMPLEMENT_DYNAMIC(CPolygeon, CDialogEx)

CPolygeon::CPolygeon(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPolygeon::IDD, pParent)
	, m_PolygeonSideNumber(0)
	, m_PolygeonCenterX(0)
	, m_PolygeonCenterY(0)
	, m_PolygeonCenterZ(0)
	, m_PolygeonCircleRadius(0)
	, m_PolygeonBottomHigh(0)
	, m_PolygeonSideAngel(0)
	, m_PolygeonFeedSpeed(0)
	, m_PolygeonFeedTime(0)
	, m_PolygeonFeedLoad(0)
	, m_PolygeonExtendDisatance(0)
	/*, m_PolygeonCutDirection(0)*/
	, m_PolygeonSawWidth(0)
	, m_PolygeonSawDiamand(0)
	, m_PolygeonWorkBlankLength(0)
	, m_PolygeonWorkBlankWidth(0)
	, m_PolygeonWorkBlankHigh(0)
{
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	ratio = 0.0;
}

CPolygeon::~CPolygeon()
{
}

void CPolygeon::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT6, m_PolygeonSideNumber);
	DDX_Text(pDX, IDC_EDIT7, m_PolygeonCenterX);
	DDX_Text(pDX, IDC_EDIT8, m_PolygeonCenterY);
	DDX_Text(pDX, IDC_EDIT9, m_PolygeonCenterZ);
	DDX_Text(pDX, IDC_EDIT10, m_PolygeonCircleRadius);
	DDX_Text(pDX, IDC_EDIT11, m_PolygeonBottomHigh);
	DDX_Text(pDX, IDC_EDIT15, m_PolygeonSideAngel);
	DDX_Text(pDX, IDC_EDIT3, m_PolygeonFeedSpeed);
	DDX_Text(pDX, IDC_EDIT4, m_PolygeonFeedTime);
	DDX_Text(pDX, IDC_EDIT5, m_PolygeonFeedLoad);
	DDX_Text(pDX, IDC_EDIT12, m_PolygeonExtendDisatance);
	//  DDX_Radio(pDX, IDC_RADIO1, m_PolygeonCutDirection);
	DDX_Text(pDX, IDC_EDIT1, m_PolygeonSawWidth);
	DDX_Text(pDX, IDC_EDIT14, m_PolygeonSawDiamand);
	DDX_Text(pDX, IDC_EDIT2, m_PolygeonWorkBlankLength);
	DDX_Text(pDX, IDC_EDIT13, m_PolygeonWorkBlankWidth);
	DDX_Text(pDX, IDC_EDIT16, m_PolygeonWorkBlankHigh);
}


BEGIN_MESSAGE_MAP(CPolygeon, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CPolygeon::OnBnClickedShowBigger)
	ON_BN_CLICKED(IDC_BUTTON2, &CPolygeon::OnBnClickedShowSmaller)
END_MESSAGE_MAP()


// CPolygeon 消息处理程序


void CPolygeon::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CPolygeon::OnOK()
{
	UpdateData(TRUE);

	pDoc->tool.SetPolygeonNumber(m_PolygeonSideNumber);
	pDoc->tool.SetPolygeonCenter(m_PolygeonCenterX, m_PolygeonCenterY, m_PolygeonCenterZ);
	pDoc->tool.SetRadiusOfPolygeon(m_PolygeonCircleRadius);
	pDoc->tool.SetPolygeonBottomHigh(m_PolygeonBottomHigh);
	pDoc->tool.SetPolygeonBottomAngel(m_PolygeonSideAngel);

	pDoc->tool.SetPolygeonFeedSpeed(m_PolygeonFeedSpeed);
	pDoc->tool.SetPolygeonFeedTime(m_PolygeonFeedTime);
	pDoc->tool.SetPolygeonFeedLoad(m_PolygeonFeedLoad);
	pDoc->tool.SetPolygeonExtention(m_PolygeonExtendDisatance);
	//pDoc->tool.SetPolygeonCutDirection(m_PolygeonCutDirection);

	pDoc->tool.SetPolygeonSawWidth(m_PolygeonSawWidth);
	
	pDoc->tool.SetPolygeonWorkBlankLength(m_PolygeonWorkBlankLength);
	pDoc->tool.SetPolygeonWorkBlankWidth(m_PolygeonWorkBlankWidth);
	pDoc->tool.SetPolygeonWorkBlankHigh(m_PolygeonWorkBlankHigh);

	pDoc->tool.SetPolygeonPolyLine(PolygeonLine);
	pDoc->tool.RenderPolygeonInit();

	DestroyWindow();


	//CDialogEx::OnOK();
}


void CPolygeon::OnCancel()
{
	DestroyWindow();

	//CDialogEx::OnCancel();
}


BOOL CPolygeon::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_PolygeonSideNumber = 5;
	m_PolygeonCenterX = 500;
	m_PolygeonCenterY = 500;
	m_PolygeonCenterZ = 200;
	m_PolygeonCircleRadius = 300;
	m_PolygeonBottomHigh = 0;
	m_PolygeonSideAngel = 60;

	m_PolygeonFeedSpeed = 6000;
	m_PolygeonFeedTime = 60;
	m_PolygeonExtendDisatance = 400;

	m_PolygeonSawWidth = 10;
	m_PolygeonSawDiamand = 800;

	m_PolygeonWorkBlankLength = 800;
	m_PolygeonWorkBlankWidth = 800;
	m_PolygeonWorkBlankHigh = 200;

	
	UpdateData(FALSE);
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL CPolygeon::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			vector<OffsetPoint>().swap(PolygeonLine);
			UpdateData(TRUE);

			m_PolygeonFeedLoad = m_PolygeonFeedSpeed*m_PolygeonFeedTime / 60;

			if (m_PolygeonSideNumber != 0)
			{
				m_EveryAngelOfSide = 360 / m_PolygeonSideNumber;
				//顺时针顺序,零件的准确形状
				for (int i = 0; i != m_PolygeonSideNumber; ++i)
				{
					OffsetPoint tempPolygeon;
					tempPolygeon.begin.x = m_PolygeonCenterX + m_PolygeonCircleRadius*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.y = m_PolygeonCenterY + m_PolygeonCircleRadius*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.z = m_PolygeonCenterZ;

					tempPolygeon.end.x = m_PolygeonCenterX + m_PolygeonCircleRadius*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.y = m_PolygeonCenterY + m_PolygeonCircleRadius*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.z = m_PolygeonCenterZ;

					PolygeonLine.push_back(tempPolygeon);
				}

				if (m_PolygeonCenterX < m_PolygeonCenterY)
					ratio = m_PolygeonCenterX / 50;
				else
					ratio = m_PolygeonCenterY / 50;

				InvalidateRect(rec);
			}

			

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


void CPolygeon::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

	CDC* pDC;
	pDC = GetDlgItem(IDC_POLYGEON_PICTURE)->GetDC();

	GetDlgItem(IDC_POLYGEON_PICTURE)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_POLYGEON_PICTURE)->UpdateWindow();

	GetDlgItem(IDC_POLYGEON_PICTURE)->GetClientRect(rec);



	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&redpen);
	CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
	CBrush* pOldBrush = pDC->SelectObject(pBrush);

	for (vector<OffsetPoint>::iterator it = PolygeonLine.begin(); it != PolygeonLine.end(); ++it)
	{
		pDC->MoveTo(it->begin.x/ratio,rec.Height()-it->begin.y/ratio);
		pDC->LineTo(it->end.x / ratio, rec.Height() - it->end.y / ratio);
	}


	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);


}


void CPolygeon::OnBnClickedShowBigger()
{
	if (ratio != 0)
	{
		ratio *= 0.9;

		InvalidateRect(rec);
	}
}


void CPolygeon::OnBnClickedShowSmaller()
{
	if (ratio != 0)
	{
		ratio *= 1.1;

		InvalidateRect(rec);
	}
}
