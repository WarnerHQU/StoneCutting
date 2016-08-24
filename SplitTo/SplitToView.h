
// SplitToView.h : CSplitToView 类的接口
//

#pragma once

#include "resource.h"
#include"OPENGL\OpenGLDC.h"
#include"Polygeon.h"
#include"LineProcess.h"
#include"Orthgonal.h"
#include"RotateProcess.h"
#include"SideProcess.h"
#include"ArcSurface.h"
#include"SawSetDlg.h"
#include"TableBoardDlg.h"
#include"MachineSetDlg.h"
#include "afxwin.h"

class CSplitToView : public CFormView
{


protected: // 仅从序列化创建
	CSplitToView();
	DECLARE_DYNCREATE(CSplitToView)

public:
	enum{ IDD = IDD_SPLITTO_FORM };


// 特性
public:
	CSplitToDoc* GetDocument() const;

	
// 操作
public:
	BOOL JUGE;
	
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CSplitToView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnClickedButton1();
//	afx_msg void OnBnClickedPolygeon();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedLineprocess();
	afx_msg void OnBnClickedOrthgonal();
	afx_msg void OnBnClickedRotateSurface();
	afx_msg void OnBnClickedSideSurface();
	afx_msg void OnBnClickedArcSurface();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnShowRough();
	afx_msg void OnBnClickedMachineSet();
	afx_msg void OnBnClickedSawSet();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// 水平控制栏
	CScrollBar m_HorizonScroll;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCodeGeneration();
	afx_msg void OnBnClickedTableBoard();
};

#ifndef _DEBUG  // SplitToView.cpp 中的调试版本
inline CSplitToDoc* CSplitToView::GetDocument() const
   { return reinterpret_cast<CSplitToDoc*>(m_pDocument); }
#endif

