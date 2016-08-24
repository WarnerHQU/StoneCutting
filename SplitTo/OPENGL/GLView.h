
// GLView.h : CGLView 类的接口
//

#pragma once
#include"gl\GL.h"
#include"gl\GLU.h"
#include"gl\GLAUX.H"
#include"gl\glut.h"
#include"Camera.h"
#include "CadBase.h"
#define BUFFER_LENGTH 64

class COpenGLDC;
class CGLView : public CView
{
protected: // 仅从序列化创建
	CGLView();
	DECLARE_DYNCREATE(CGLView)

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	virtual void RenderScene(COpenGLDC* pDC);//用于OPENGL绘制图形
	COpenGLDC* GetGLDc();
private:

	//device context
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	COpenGLDC*	m_pGLDC;

	virtual BOOL GetBox(double& x0,double& y0,double& z0,double& x1,double& y1,double& z1);

	void	Zoom(double dScale);
	void	ZoomAll();
	void	OnViewType(UINT type);
	void	MoveView(double dpx,double dpy);
};

#ifndef _DEBUG  // GLView.cpp 中的调试版本
//inline CGLDoc* CGLView::GetDocument() const
//   { return reinterpret_cast<CGLDoc*>(m_pDocument); }
#endif

