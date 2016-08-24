#pragma once
#include"SplitToDoc.h"
#include"MainFrm.h"
#include "afxwin.h"

// CLineProcess 对话框

class CLineProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CLineProcess)

public:
	CLineProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLineProcess();

// 对话框数据
	enum { IDD = IDD_LINEPROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedLineProcessOpenFile();
public://图形显示画框
	CRect BoundingBox(CPtrArray& Array);//用于返回平面二维图形的最小包围矩形框
	void SortDraw(CDC* dc, CPtrArray& m_Array);
	void DrawaLabel(CDC* m_pDC, CRect Rect);
	void DrawH2Label(CDC* m_pDC, CRect Rect);
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);

private://图形显示
	int m_AddDistance;//图形太靠下了，增加个距离使其位于中间
	CPtrArray m_TempArray;//存储的都是空指针
	vector<string> m_TempData;
	CRect rec;//存储显示区域的范围
	float ratio;
	CRect PieceSize;//保存整体的工件的rect
	POINT m_CorCenter;//将图形显示设置在picture控件的中心位置
	CRect m_BoundingBox_2D;
	BOOL m_ShowTips;
	CSplitToDoc* pDoc;
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedShowBigger();
	afx_msg void OnBnClickedShowSmaller();
	afx_msg void OnBnClickedShowTips();
	// 工件的长度
	float m_PierceLong;
	// 工件的宽度
	float m_PierceWidth;
	// 工件顶的高
	float m_PierceTopHigh;
	// 工件底面的高度
	float m_PierceBottomHigh;
	// 记录设置的毛坯的长度
	float m_WorkBlankLength;
	// 记录工件的宽度
	float m_WorkBlankWidth;
	// 记录设置的毛坯的高度
	float m_WorkBlankHigh;
	// 记录粗加工水平切割进给速度
	float m_Rough_H_FeedSpeed;
	// 记录水平粗加工切割进给周期
	float m_Rough_H_FeedTime;
	// 记录水平粗加工合成进给量
	float m_Rough_H_FeedLoad;
	// 记录粗加工垂直面加工进给速度
	float m_Rough_V_FeedSpeed;
	// 记录粗加工垂直面切割进给周期
	float m_Rough_V_FeedTime;
	// 记录粗加工垂直面加工合成进给量
	float m_Rough_V_FeedLoad;
	// 记录粗加工垂直下刀距离
//	float m_Rough_VerticalDistance;
	// 记录粗加工层数
//	float m_Rough_Piles;
	// 记录粗加工余量
	float m_Rough_Left;
	// 记录精加工水平面切割速度
	float m_Finish_H_FeedSpeed;
	// 记录精加工水平面切割进给周期
	float m_Finish_H_FeedTime;
	// 记录水平面精加工合成进给量
	float m_Finish_H_FeedLoad;
	// 记录精加工垂直面切割进给速度
	float m_Finish_V_FeedSpeed;
	// 记录精加工垂直面加工进给周期
	float m_Finish_V_FeedTime;
	// 记录精加工垂直面合成进给量
	float m_Finish_V_FeedLoad;
	// 记录精加工垂直下刀距离
//	float m_Finish_VerticalDistance;
	// 记录 精加工层数
//	float m_Finish_Piles;
	// 记录线型石材线条的长度
	float m_Line_Length;
	// 记录圆弧形线条圆心横坐标
	float m_Arc_CenterX;
	// 记录圆弧形线条圆心纵坐标
	float m_Arc_CenterY;
	// 记录圆弧形线条圆心Z坐标
	float m_Arc_CenterZ;
	// 记录圆弧形线条半径,从图形的最左端点算起的半径
	float m_Arc_Radius;
	// 记录圆弧形线条起始角(与逆时针夹角)
	float m_Arc_BeginAngle;
	// 记录圆弧形线条终止角(与逆时针夹角)
	float m_Arc_EndAngle;
//	CButton m_LineRadio;
	int m_LineRadio;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedArcRadio();
	afx_msg void OnBnClickedLineRadio();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// 线条切割方向
//	int m_LineDirection;
	afx_msg void OnEnChangeEdit17();
};
