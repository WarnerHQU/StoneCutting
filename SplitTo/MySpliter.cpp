// MySpliter.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "MySpliter.h"


// CMySpliter

IMPLEMENT_DYNAMIC(CMySpliter, CSplitterWnd)

CMySpliter::CMySpliter()
{

}

CMySpliter::~CMySpliter()
{
}


BEGIN_MESSAGE_MAP(CMySpliter, CSplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CMySpliter 消息处理程序




void CMySpliter::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//CSplitterWnd::OnLButtonDown(nFlags, point);
	return;
}


void CMySpliter::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//CSplitterWnd::OnMouseMove(nFlags, point);
	CWnd::OnMouseMove(nFlags, point);
}


BOOL CMySpliter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
	return FALSE;
}


void CMySpliter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{
	//if (pDC == NULL)
	//{
	//	RedrawWindow(rect, NULL, RDW_INVALIDATE | RDW_NOCHILDREN);
	//	return;
	//}
	//ASSERT_VALID(pDC);
	//CRect rc = rect;
	//switch (nType)
	//{
	//case splitBorder:
	//	//重画分割窗口边界,使之为红色 
	//	pDC->Draw3dRect(rc, RGB(255, 0, 0), RGB(255, 0, 0));
	//	rc.InflateRect(-CX_BORDER, -CY_BORDER);
	//	pDC->Draw3dRect(rc, RGB(255, 0, 0), RGB(255, 0, 0));
	//	return;
	//case splitBox:
	//	pDC->Draw3dRect(rc, RGB(0, 0, 0), RGB(0, 0, 0));
	//	rc.InflateRect(-CX_BORDER, -CY_BORDER);
	//	pDC->Draw3dRect(rc, RGB(0, 0, 0), RGB(0, 0, 0));
	//	rc.InflateRect(-CX_BORDER, -CY_BORDER);
	//	pDC->FillSolidRect(rc, RGB(0, 0, 0));
	//	pDC->Draw3dRect(rc, RGB(0, 0, 0), RGB(0, 0, 0));
	//	return;
	//case splitBar:
	//	//重画分割条，使之为绿色  
	//	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	//	rc.InflateRect(-5, -5);
	//	pDC->Draw3dRect(rc, RGB(255, 0, 0), RGB(255, 0, 0));
	//	return;
	//default:
	//	ASSERT(FALSE);
	//}
	//pDC->FillSolidRect(rc, RGB(0, 0, 255));


	CSplitterWnd::OnDrawSplitter(pDC, nType, rect);
}


void CMySpliter::OnInvertTracker(const CRect& rect)
{
	// TODO:  在此添加专用代码和/或调用基类

	CSplitterWnd::OnInvertTracker(rect);
}
