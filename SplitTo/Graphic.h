#pragma once

#include"OPENGL\GLView.h"
#include"OPENGL\OpenGLDC.h"
#include"SplitToDoc.h"
#include"MainFrm.h"
#include"SplitToView.h"
#include"Polygeon.h"
#include"HighResolutionTimer.h"
#include"TableBoardProcess.h"
// CGraphic 视图

class CGraphic : public CGLView
{
	DECLARE_DYNCREATE(CGraphic)


//多媒体定时器
public:
	CHighResolutionTimer mm_Timer;
	CHighResolutionTimer mm_TimerFinish;
	static void TimerCallback(DWORD dwUser);
	static void TimerCallbackFinish(DWORD dwUser);
	CSplitToView* pSplitView;
	void SetRoughCordValue(CSplitToDoc* pDoc,int index);//设置粗加工坐标的显示
	void SetFinishCordValue(CSplitToDoc* pDoc, int index);//设置精加工坐标的显示
public:
	LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
protected:
	short account;

protected:
	CGraphic();           // 动态创建所使用的受保护的构造函数
	virtual ~CGraphic();

public:
	CSplitToDoc* GetDocument() const;
public:
	
	void initRotate();
	CPoint3D      m_MidButtionPos;
	CPoint     m_LeftDownPos;
	float m_xRotate;
	float m_yRotate;
	bool m_LeftButtonDown;
	virtual void RenderScene(COpenGLDC* pDC);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFrontView();
	afx_msg void OnRightView();
	afx_msg void OnTopView();
//	afx_msg void OnUpdateFrontView(CCmdUI *pCmdUI);
	afx_msg void OnBackView();
	afx_msg void OnLeftView();
	afx_msg void OnBottomView();
//	afx_msg void OnBnClickedPolygeon();
	virtual void OnInitialUpdate();
//	afx_msg void OnBnClickedBeginSimulation();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


