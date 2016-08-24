#pragma once

#include"SplitToDoc.h"
#include"MainFrm.h"
// COrthgonal 对话框

class COrthgonal : public CDialogEx
{
	DECLARE_DYNAMIC(COrthgonal)

public:
	COrthgonal(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COrthgonal();

// 对话框数据
	enum { IDD = IDD_ORTHGONAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CSplitToDoc* pDoc;
public:
	// 记录正交加工零件的长度
	float m_OrthgonalWorkBlankLength;
	// 记录正交加工零件的宽度
	float m_OrthgonalWorkBlankWidth;
	// 记录正交加工零件的高度
	float m_OrthgonalWorkBlankHeigh;
	// 记录加工梯形的类别
//	int m_TrapezoidType;
	// 记录加工梯形的底角
	float m_TrapezoidAngle;
	// 记录锯片的宽度
	float m_OrthgonalSawWidth;
	// 记录梯形的形状0为梯形1为倒梯形
	int m_TrapezoidType;
	// 垂直加工模式的选择0为横切加工1为竖切加工2为正交加工
	int m_OrthgonalProcessStyle;
	// 记录Y侧梯形的短边长度
	float m_OrthgonalYShort;
	// 记录Y侧梯形长边的长度
	float m_OrthgonalYLong;
	// 记录Y侧梯形间间距
	float m_OrthgonalYGap;
	// 记录Y侧梯形的进给量
	float m_OrthgonalYFeedLoad;
	// 记录X侧梯形短边的长度
	float m_OrthgonalXShort;
	// 记录X侧梯形长边的长度
	float m_OrthgonalXLong;
	// 记录X侧梯形间的间距
	float m_OrthgonalXGap;
	// 记录X侧梯形的合成进给量
	float m_OrthgonalXFeedLoad;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
