// Graphic.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"

#include "Graphic.h"

// CGraphic

IMPLEMENT_DYNCREATE(CGraphic, CGLView)

CGraphic::CGraphic()
{
	m_LeftButtonDown = FALSE;
	account = 0;
	initRotate();
}

CGraphic::~CGraphic()
{
	mm_Timer.KillTimer();
	mm_TimerFinish.KillTimer();
}

BEGIN_MESSAGE_MAP(CGraphic, CGLView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_FRONT_VIEW, &CGraphic::OnFrontView)
	ON_COMMAND(ID_RIGHT_VIEW, &CGraphic::OnRightView)
	ON_COMMAND(ID_TOP_VIEW, &CGraphic::OnTopView)
	ON_COMMAND(ID_BACK_VIEW, &CGraphic::OnBackView)
	ON_COMMAND(ID_LEFT_VIEW, &CGraphic::OnLeftView)
	ON_COMMAND(ID_BOTTOM_VIEW, &CGraphic::OnBottomView)
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CGraphic 绘图


// CGraphic 诊断

#ifdef _DEBUG
void CGraphic::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CGraphic::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGraphic 消息处理程序


void CGraphic::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;

	CGLView::OnLButtonDown(nFlags, point);
}


void CGraphic::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_LeftButtonDown = FALSE;

	CGLView::OnLButtonUp(nFlags, point);
}


void CGraphic::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_MidButtionPos.x = point.x;
	m_MidButtionPos.y = point.y;
	m_MidButtionPos.z = 0.0;

	CGLView::OnMButtonDown(nFlags, point);
}


void CGraphic::OnMouseMove(UINT nFlags, CPoint point)
{
	double nMoveX, nMoveY;
	if (nFlags & MK_SHIFT)
	{
		if (nFlags & MK_MBUTTON)
		{
			nMoveX = point.x - m_MidButtionPos.x;
			nMoveY = m_MidButtionPos.y - point.y;
			MoveView(nMoveX*0.0011, nMoveY*0.0011);
			m_MidButtionPos.x = point.x;
			m_MidButtionPos.y = point.y;
		}
	}

	else if (m_LeftButtonDown == TRUE)
	{
		CSize rotate = m_LeftDownPos - point;
		m_LeftDownPos = point;
		if (nFlags & MK_CONTROL)
			MoveView(-0.001*rotate.cx, 0.001*rotate.cy);
		else
		{

			m_yRotate -= rotate.cx;
			m_xRotate -= rotate.cy;
		}
		InvalidateRect(NULL, FALSE);
	}

	CGLView::OnMouseMove(nFlags, point);
}


BOOL CGraphic::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 100)
	{
		Zoom(0.9);

	}
	if (zDelta < -100)
	{
		Zoom(1.1);
	}
	Invalidate();

	return CGLView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGraphic::initRotate()
{
	m_xRotate = 0.0;
	m_yRotate = 0.0;
}

void CGraphic::RenderScene(COpenGLDC* pDC)
{
	
	glRotatef(m_yRotate, 0.0f, 1.0f, 0.0f); // Rock Z
	glRotatef(m_xRotate, 1.0f, 0.0f, 0.0f); // Roll X
	
	pDC->DrawCoord();
	//分屏的视类中访问文档类
	glColor3f(0.1f, 0.5f, 1.0f);

	//CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	/*CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	CSplitToDoc* pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();*/

	
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pDocTemplate->GetFirstDocPosition();
	CSplitToDoc* pDoc = (CSplitToDoc*)pDocTemplate->GetNextDoc(pos);

	//获取视类指针
	CView* pView;
	pos = pDoc->GetFirstViewPosition();
    pView = pDoc->GetNextView(pos);
	pSplitView = (CSplitToView*)pView;
	
	

	COLORREF old_clr;
	pDC->GetMaterialColor(old_clr);
	pDC->SetMaterialColor(RGB(0, 0, 255));

	//CTableBoardProcess::GetTableProcessInstance()->RenderTableBoard(pDC);
	//glutWireTeapot(500);
	//pDoc->MyDraw(pDC);
	//pDoc->MyDrawXOZ(pDC);
	//pDoc->MyDrawYOZ(pDC);
	pDC->SetMaterialColor(RGB(255, 0, 0));

	//pDoc->DrawLineProcess(pDC);
	//pDoc->DrawArcLineProcess(pDC);

	//pDoc->DrawOffset(pDC);
	//pDoc->DrawOtho(pDC);

	//pDoc->DrawRotate(pDC);
	//pDoc->DrawSide(pDC);
	static int i = 0;//粗加工标志
	static int m = 0;//精加工标志
	if (pDoc->m_IsBeginSimulation)//粗加工仿真
	{
		
		if (pDoc->tool.RoughSimulationPoints.size() != 0)
		{
             SetRoughCordValue(pDoc, i);//放前面，以免重复调用

			if (i < pDoc->tool.RoughSimulationPoints.size())
			{
				pDC->SetMaterialColor(RGB(255, 255, 0));//黄色
				for (int j = 0; j< i; ++j)
				{
					if (i < pDoc->tool.RoughSimulationPoints.size() - 1)
					{
						pDC->DrawLine(pDoc->tool.RoughSimulationPoints[j], pDoc->tool.RoughSimulationPoints[j + 1]);
						
					}
					else
						break;
				}


				//显示最前面的点的位置红点
				pDC->SetMaterialColor(RGB(255, 0, 0));
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.RoughSimulationPoints[i]);
				++i;
			}
			else
			{
				i = 0;

			}
		}
	
	}
	else if (pDoc->m_IsBeginFinishSimulation)//精加工仿真
	{
		if (pDoc->tool.FinishSimulationPoints.size() != 0)
		{
			SetFinishCordValue(pDoc, m);

			if (m< pDoc->tool.FinishSimulationPoints.size())
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				for (int  k = 0; k< m; ++k)
				{
					if (k < pDoc->tool.FinishSimulationPoints.size() - 1)
						pDC->DrawLine(pDoc->tool.FinishSimulationPoints[k], pDoc->tool.FinishSimulationPoints[k + 1]);
					else
					{
						break;
					}
				}

				//用来显示最前面点的位置绿点
				pDC->SetMaterialColor(RGB(0,255, 0));
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.FinishSimulationPoints[m]);
				++m;
			}

			else
			{
				m = 0;
				
			}
		}
	}
	else
	{
		i = 0; 
		m = 0;
		
		if (pDoc->tool.GetArray().IsEmpty() == 0)//侧面加工
		{
			if (pDoc->m_ShowSide)
			{
				pDoc->tool.RenderSide(pDC);
			}
			if (pDoc->m_ShowSideRough)
			{
				pDC->SetMaterialColor(RGB(0, 255, 0));
				pDoc->tool.RenderSideRough(pDC);
			}
			if (pDoc->m_ShowSidePiece)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.GetCenterPoint());
				pDC->DrawRectangleFrame(pDoc->tool.GetCenterPoint(), pDoc->tool.GetLongth(),
					pDoc->tool.GetWidth(), pDoc->tool.GetHeigth());
			}
			if (pDoc->m_ShowSideRoughCut)
			{
				pDC->SetMaterialColor(RGB(255, 255, 0));//黄色
				pDoc->tool.RenderSideRoughCut(pDC);
			}
			if (pDoc->m_ShowSideRoughLeft)
			{
				pDC->SetMaterialColor(RGB(255, 128, 128));//粉色
				pDoc->tool.RenderSideRoughLeft(pDC);
			}
			if (pDoc->m_ShowSideFinish)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDoc->tool.RenderSideFinish(pDC);
			}
          

		}
		if (pDoc->tool.GetRotateArray().IsEmpty() == 0)
		{
			if (pDoc->m_ShowRotate)
			{
				pDoc->tool.RenderRotate(pDC);
			}
			if (pDoc->m_ShowRotateWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0,255));//毛坯
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.m_RotateRoughWorkBlankCenter);
				pDC->DrawRectangleFrame(pDoc->tool.m_RotateRoughWorkBlankCenter, pDoc->tool.GetRotateBlankLongth(),
					                pDoc->tool.GetRotateBlankWidth(), pDoc->tool.GetRotateBlankHeighth());
			}
			if (pDoc->m_ShowRotateRough)
			{
				pDC->SetMaterialColor(RGB(0, 255, 0));//绿色
				pDoc->tool.RenderRoughRotate(pDC);
			}
			if (pDoc->m_ShowRotateRoughCut)
			{
				pDC->SetMaterialColor(RGB(255, 255, 0));//黄色
				//pDC->SetMaterialColor(RGB(255,73, 77));
				pDoc->tool.RenderRoughCutRotate(pDC);
			}
			if (pDoc->m_ShowRotateFinish)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDoc->tool.RenderFinishRotate(pDC);
			}
			
		}
		if (pDoc->tool.GetArcArray().IsEmpty() == 0)
		{
			if (pDoc->m_ShowArc)
			{
				pDoc->tool.RenderArc(pDC);
			}
			if (pDoc->m_ShowArcRough)
			{
				pDC->SetMaterialColor(RGB(0, 255, 0));//绿色
				pDoc->tool.RenderRoughArc(pDC);
			}
			if (pDoc->m_ShowArcWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.GetArcWorkBlankCenter(pDoc->tool.GetArcWorkBlankHeigth()));
				pDC->DrawRectangleFrame(pDoc->tool.GetArcWorkBlankCenter(pDoc->tool.GetArcWorkBlankHeigth()), pDoc->tool.GetArcWorkBlankLongth(),
					pDoc->tool.GetArcWorkBlankWidth(), pDoc->tool.GetArcWorkBlankHeigth());
			}
			if (pDoc->m_ShowArcRoughAfterCut)
			{
				pDC->SetMaterialColor(RGB(255, 255, 0));//黄色
				pDoc->tool.RenderArcRoughCut(pDC);
			}
			if (pDoc->m_ShowArcRoughLeft)
			{
				pDC->SetMaterialColor(RGB(255, 128, 128));//粉色
				pDoc->tool.RenderArcRoughLeft(pDC);
			}
			if (pDoc->m_ShowArcFinish)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDoc->tool.RenderArcFinish(pDC);
			}
		}
		if (pDoc->tool.GetLineArray().IsEmpty() == 0)
		{
			if (pDoc->m_ShowLine)
			{
				pDoc->tool.LineRender(pDC);
			}
			if (pDoc->m_ShowLineWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				glPointSize(5);
				if (!pDoc->tool.GetLineStyle())//直线
				{
					pDC->DrawPoint(pDoc->tool.GetLineProcessWorkBlankCenter());
					pDC->DrawRectangleFrame(pDoc->tool.GetLineProcessWorkBlankCenter(), pDoc->tool.GetLineWorkBlankWidth(),
						pDoc->tool.GetLineWorkBlankLength(), pDoc->tool.GetLineWorkBlankHigh());
				}
				else//圆弧
				{
					pDC->DrawArcFrame(pDoc->tool.GetLineArcShapeArray(), pDoc->tool.GetLineProcessWorkBlankCenter(),
						pDoc->tool.GetLineWorkBlankWidth(), pDoc->tool.GetLineWorkBlankHigh());
				}
			}
			if (pDoc->m_ShowLineRoughLeft)
			{
				pDC->SetMaterialColor(RGB(255, 128, 128));//粉色
				pDoc->tool.LineRoughLeftRender(pDC);
			}
			if (pDoc->m_ShowLineRough)
			{
				pDC->SetMaterialColor(RGB(0, 255, 0));//绿色
				pDoc->tool.LineRoughRender(pDC);
			}
			if (pDoc->m_ShowLineFinish)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDoc->tool.LineFinishRender(pDC);
			}
		}
		if (pDoc ->tool.RenderOrthgonalInit())
		{
			if (pDoc->m_ShowOrthgonal)
			{
				pDoc->tool.RenderOrthgonal(pDC);
			}
			if (pDoc->m_ShowOrthgonalCutting)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色

				pDoc->tool.RenderOrthgonalCuttingPath(pDC);

			}
			if (pDoc->m_ShowOrthgonalWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				glPointSize(5);
				pDC->DrawPoint(pDoc->tool.GetOrthgonalWorkBlankCenter(pDoc->tool.GetOrthgonalWorkBlankLength(), pDoc->tool.GetOrthgonalWorkBlankWidth(),
					pDoc->tool.GetOrthgonalWorkBlankHigh()));
				pDC->DrawRectangleFrame(pDoc->tool.GetOrthgonalWorkBlankCenter(pDoc->tool.GetOrthgonalWorkBlankLength(), pDoc->tool.GetOrthgonalWorkBlankWidth(),
					pDoc->tool.GetOrthgonalWorkBlankHigh()), pDoc->tool.GetOrthgonalWorkBlankLength(), pDoc->tool.GetOrthgonalWorkBlankWidth(),
					pDoc->tool.GetOrthgonalWorkBlankHigh());
			}
		}
		if (pDoc->tool.GetPolygeonPolyLine().size() != 0)
		{
			if (pDoc->m_ShowPolygeon)
			{
				pDoc->tool.RenderPolygeon(pDC);
			}
			if (pDoc->m_ShowPolygeonWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				glPointSize(5);
				pDC->DrawPoint(CPoint3D(pDoc->tool.GetPolygeonCenter().x, pDoc->tool.GetPolygeonCenter().y, 
					(pDoc->tool.GetPolygeonCenter().z - pDoc->tool.GetPolygeonBottomHigh())*0.5 + pDoc->tool.GetPolygeonBottomHigh()));
				pDC->DrawRectangleFrame(CPoint3D(pDoc->tool.GetPolygeonCenter().x, pDoc->tool.GetPolygeonCenter().y,
					(pDoc->tool.GetPolygeonCenter().z - pDoc->tool.GetPolygeonBottomHigh())*0.5 + pDoc->tool.GetPolygeonBottomHigh()),
					pDoc->tool.GetPolygeonWorkBlankLength(), pDoc->tool.GetPolygeonWorkBlankWidth(),pDoc->tool.GetPolygeonWorkBlankHigh());
			}
			if (pDoc->m_ShowPolygeonCuttingPath)
			{
				/*pDC->SetMaterialColor(RGB(0, 255, 0));*/
				pDoc->tool.RenderPolygeonCuttingPath(pDC);
			}
		}

		if (CTableBoardProcess::GetTableProcessInstance()->GetBoardArray()!=NULL)
		{
			pDoc->tool.RoughSimulationPoints = CTableBoardProcess::GetTableProcessInstance()->m_BoardRoughSimulation;
			pDoc->tool.FinishSimulationPoints = CTableBoardProcess::GetTableProcessInstance()->m_BoardFinishSimulation;

			if (pDoc->m_ShowBoard)
			{
				CTableBoardProcess::GetTableProcessInstance()->RenderTableBoard(pDC);
			}
			if (pDoc->m_ShowBoardWorkBlank)
			{
				pDC->SetMaterialColor(RGB(0, 0, 255));
				CTableBoardProcess::GetTableProcessInstance()->RenderTableBlank(pDC);
			}
			if (pDoc->m_ShowBoardOutPath)
			{
				pDC->SetMaterialColor(RGB(0, 255, 0));//绿色
				CTableBoardProcess::GetTableProcessInstance()->RenderTableOutPath(pDC);
			}
			if (pDoc->m_ShowBoardInSawPath)
			{
				//橙色
				pDC->SetMaterialColor(RGB(255,97,0));
				CTableBoardProcess::GetTableProcessInstance()->RenderTableSawPath(pDC);
			}
			if (pDoc->m_ShowBoardInMillPath)
			{
				//浅石板灰
				pDC->SetMaterialColor(RGB(119,136,153));
				CTableBoardProcess::GetTableProcessInstance()->RenderTableMillPath(pDC);
			}
		}

        Invalidate();   
	}

	
	//pDoc->DrawSurface(pDC);
	pDC->SetMaterialColor(old_clr);
	
}



void CGraphic::OnFrontView()
{
	OnViewType(VIEW_FRONT);

}


void CGraphic::OnRightView()
{
	OnViewType(VIEW_RIGHT);
}


void CGraphic::OnTopView()
{
	OnViewType(VIEW_TOP);
}


void CGraphic::OnBackView()
{
	OnViewType(VIEW_BACK);
}


void CGraphic::OnLeftView()
{
	OnViewType(VIEW_LEFT);
}


void CGraphic::OnBottomView()
{
	OnViewType(VIEW_BOTTOM);
}



void CGraphic::OnInitialUpdate()
{
	CGLView::OnInitialUpdate();
	

}


void CGraphic::TimerCallback(DWORD dwUser)
{
	CGraphic *pThis = (CGraphic *)dwUser;
	
	pThis->RenderScene(pThis->m_pGLDC);
	
	pThis->Invalidate();
}

void CGraphic::TimerCallbackFinish(DWORD dwUser)
{
	
	CGraphic *pThis = (CGraphic *)dwUser;

	pThis->RenderScene(pThis->m_pGLDC);

	pThis->Invalidate();
}

void CGraphic::SetRoughCordValue(CSplitToDoc* pDoc, int index)
{
	if (pDoc->tool.RoughSimulationPoints.size() != 0)
	{
		if (index < pDoc->tool.RoughSimulationPoints.size())
		{
			//设置坐标
			CString str;
			str.Format(_T("%7.2f"), pDoc->tool.RoughSimulationPoints[index].x);
			pSplitView->GetDlgItem(IDC_X_CORD)->SetWindowTextW(str);
			str.Format(_T("%7.2f"), pDoc->tool.RoughSimulationPoints[index].y);
			pSplitView->GetDlgItem(IDC_Y_CORD)->SetWindowTextW(str);
			str.Format(_T("%7.2f"), pDoc->tool.RoughSimulationPoints[index].z);
			pSplitView->GetDlgItem(IDC_Z_CORD)->SetWindowTextW(str);
		}
		else
		{
			pSplitView->GetDlgItem(IDC_X_CORD)->SetWindowTextW(_T("0.00"));
			pSplitView->GetDlgItem(IDC_Y_CORD)->SetWindowTextW(_T("0.00"));
			pSplitView->GetDlgItem(IDC_Z_CORD)->SetWindowTextW(_T("0.00"));
		}
		
	}
}

void CGraphic::SetFinishCordValue(CSplitToDoc* pDoc, int index)
{
	if (pDoc->tool.FinishSimulationPoints.size() != 0)
	{
		if (index < pDoc->tool.FinishSimulationPoints.size())
		{
			//设置坐标
			CString str;
			str.Format(_T("%7.2f"), pDoc->tool.FinishSimulationPoints[index].x);
			pSplitView->GetDlgItem(IDC_X_CORD)->SetWindowTextW(str);
			str.Format(_T("%7.2f"), pDoc->tool.FinishSimulationPoints[index].y);
			pSplitView->GetDlgItem(IDC_Y_CORD)->SetWindowTextW(str);
			str.Format(_T("%7.2f"), pDoc->tool.FinishSimulationPoints[index].z);
			pSplitView->GetDlgItem(IDC_Z_CORD)->SetWindowTextW(str);
		}
		else
		{
			pSplitView->GetDlgItem(IDC_X_CORD)->SetWindowTextW(_T("0.00"));
			pSplitView->GetDlgItem(IDC_Y_CORD)->SetWindowTextW(_T("0.00"));
			pSplitView->GetDlgItem(IDC_Z_CORD)->SetWindowTextW(_T("0.00"));
		}

	}
}

//BOOL CGraphic::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO:  在此添加消息处理程序代码和/或调用默认值
//	return TRUE;
//	//return CGLView::OnEraseBkgnd(pDC);
//}
