
// SplitToView.cpp : CSplitToView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SplitTo.h"
#endif

#include "SplitToDoc.h"
#include "SplitToView.h"
#include"MainFrm.h"
#include"Line.h"
#include"Graphic.h"
#include"GcodeGeneration.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSplitToView

IMPLEMENT_DYNCREATE(CSplitToView, CFormView)

BEGIN_MESSAGE_MAP(CSplitToView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON1, &CSplitToView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_LINEPROCESS, &CSplitToView::OnBnClickedLineprocess)
	ON_BN_CLICKED(IDC_ORTHGONAL, &CSplitToView::OnBnClickedOrthgonal)
	ON_BN_CLICKED(IDC_ROTATE_SURFACE, &CSplitToView::OnBnClickedRotateSurface)
	ON_BN_CLICKED(IDC_SIDE_SURFACE, &CSplitToView::OnBnClickedSideSurface)
	ON_BN_CLICKED(IDC_ARC_SURFACE, &CSplitToView::OnBnClickedArcSurface)
	ON_WM_ERASEBKGND()
	/*ON_COMMAND(ID_SHOW_ROUGH, &CSplitToView::OnShowRough)*/
	ON_BN_CLICKED(IDC_MACHINE_SET, &CSplitToView::OnBnClickedMachineSet)
	ON_BN_CLICKED(IDC_SAW_SET, &CSplitToView::OnBnClickedSawSet)
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON13, &CSplitToView::OnBnClickedCodeGeneration)
	ON_BN_CLICKED(IDC_TABLE_BOARD, &CSplitToView::OnBnClickedTableBoard)
END_MESSAGE_MAP()

// CSplitToView 构造/析构

CSplitToView::CSplitToView()
	: CFormView(CSplitToView::IDD)
{
	JUGE = FALSE;
}

CSplitToView::~CSplitToView()
{
}

void CSplitToView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR2, m_HorizonScroll);
}

BOOL CSplitToView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	//cs.style |= WS_MAXIMIZE;
	return CFormView::PreCreateWindow(cs);
}

void CSplitToView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	SetDlgItemInt(IDC_X_CORD, 0);
	SetDlgItemInt(IDC_Y_CORD, 0);
	SetDlgItemInt(IDC_Z_CORD, 0);
	SetDlgItemInt(IDC_A_AXIS, 0);
	SetDlgItemInt(IDC_C_AXIS, 0);

	m_HorizonScroll.SetScrollRange(1, 100);
	m_HorizonScroll.SetScrollPos(50);
	SetDlgItemInt(IDC_HSCROLL_EDIT, 50);


	//GetDlgItem(IDC_SCROLLBAR2)->EnableWindow(FALSE);//使得控件不可用

}

void CSplitToView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSplitToView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSplitToView 诊断

#ifdef _DEBUG
void CSplitToView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSplitToView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSplitToDoc* CSplitToView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSplitToDoc)));
	return (CSplitToDoc*)m_pDocument;
}
#endif //_DEBUG


// CSplitToView 消息处理程序


void CSplitToView::OnBnClickedButton1()
{
	CPolygeon* pPolygeon=new CPolygeon;
	pPolygeon->Create(IDD_POLYGEON, this);
	pPolygeon->ShowWindow(SW_SHOW);
	
}


void CSplitToView::OnBnClickedLineprocess()
{
	CLineProcess* pLineProcess = new CLineProcess;
	pLineProcess->Create(IDD_LINEPROCESS,this);
	pLineProcess->ShowWindow(SW_SHOW);
}


void CSplitToView::OnBnClickedOrthgonal()
{
	COrthgonal* pOrthgonal = new COrthgonal;
	pOrthgonal->Create(IDD_ORTHGONAL, this);
	pOrthgonal->ShowWindow(SW_SHOW);
}


void CSplitToView::OnBnClickedRotateSurface()
{
	CRotateProcess* rote = new CRotateProcess;
	rote->Create(IDD_ROTATEPROCESS);
	rote->ShowWindow(SW_SHOW);
}


void CSplitToView::OnBnClickedSideSurface()
{
	CSideProcess* pSide = new CSideProcess;
	pSide->Create(IDD_SIDEPROCESS);
	pSide->ShowWindow(SW_SHOW);
}


void CSplitToView::OnBnClickedArcSurface()
{
	CArcSurface* pArc = new CArcSurface;
	pArc->Create(IDD_ARCSURFACE);
	pArc->ShowWindow(SW_SHOW);
	
}


BOOL CSplitToView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return CFormView::OnEraseBkgnd(pDC);

}





void CSplitToView::OnBnClickedMachineSet()
{
	CMachineSetDlg* pMachine = new CMachineSetDlg;
	pMachine->Create(IDD_MACHINESET);
	pMachine->ShowWindow(SW_SHOW);
}


void CSplitToView::OnBnClickedSawSet()
{
	CSawSetDlg* pSaw = new CSawSetDlg;
	pSaw->Create(IDD_SAWSET);
	pSaw->ShowWindow(SW_SHOW);
}


void CSplitToView::OnMouseMove(UINT nFlags, CPoint point)
{
	

	CFormView::OnMouseMove(nFlags, point);
}


void CSplitToView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSplitToDoc* pDoc = GetDocument();
	CView* pView;
	POSITION POS = pDoc->GetFirstViewPosition();
	while (POS != NULL)
	{
		pView = pDoc->GetNextView(POS);
		if (!pView->IsKindOf(RUNTIME_CLASS(CGraphic)));
		break;
	}
	CGraphic* pGraphicView = (CGraphic*)pDoc->GetNextView(POS);


	int pos = m_HorizonScroll.GetScrollPos();    // 获取水平滚动条当前位置   

	switch (nSBCode)
	{
		// 如果向左滚动一列，则pos减1   
	case SB_LINEUP:
		pos -= 1;
		break;
		// 如果向右滚动一列，则pos加1   
	case SB_LINEDOWN:
		pos += 1;
		break;
		// 如果向左滚动一页，则pos减10   
	case SB_PAGEUP:
		pos -= 10;
		break;
		// 如果向右滚动一页，则pos加10   
	case SB_PAGEDOWN:
		pos += 10;
		break;
		// 如果滚动到最左端，则pos为1   
	case SB_TOP:
		pos = 1;
		break;
		// 如果滚动到最右端，则pos为100   
	case SB_BOTTOM:
		pos = 100;
		break;
		// 如果拖动滚动块滚动到指定位置，则pos赋值为nPos的值   
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
		// 下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，所以在此处设置编辑框中显示数值   
	default:
		SetDlgItemInt(IDC_HSCROLL_EDIT,pos);
		return;
	}

	// 设置滚动块位置   
	m_HorizonScroll.SetScrollPos(pos);

	//设置速度的变化，通过重新定义定时器进行
	if (pDoc->m_IsBeginSimulation)
	{
		pGraphicView->mm_Timer.CreateTimer((DWORD)pGraphicView, 20 * 50 / pos, pGraphicView->TimerCallback);
		
	}
	
	if (pDoc->m_IsBeginFinishSimulation)
	{	
		pGraphicView->mm_TimerFinish.CreateTimer((DWORD)pGraphicView, 10 * 50 / pos, pGraphicView->TimerCallbackFinish);
	}
	
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSplitToView::OnTimer(UINT_PTR nIDEvent)//跟不上多媒体定时器的速度
{
	

	CFormView::OnTimer(nIDEvent);
}


void CSplitToView::OnBnClickedCodeGeneration()
{
	CSplitToDoc* pDoc = GetDocument();
	CGcodeGeneration Gcode(&pDoc->tool);
	Gcode.Init();
	Gcode.OutPutNcCode();
}


void CSplitToView::OnBnClickedTableBoard()
{
	// TODO:  在此添加控件通知处理程序代码
	//CTableBoardDlg* pTable = new CTableBoardDlg();
	CTableBoardDlg* pTable = CTableBoardDlg::GetTableBoardInstance();

	//判断对话框的句柄是否为空，空则创建对话框，
	//非空则不创建对话框，则保证了对话框只有一个，实现单一性
	
	if (!pTable->m_hWnd)
	{
		pTable->Create(IDD_TABLE_BOARD);
		pTable->ShowWindow(SW_SHOW);
	}
	else
	{
		pTable->ShowWindow(SW_SHOW);
	}
}
