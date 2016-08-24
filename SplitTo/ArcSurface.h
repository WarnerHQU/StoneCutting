#pragma once
#include"SplitToDoc.h"
#include"MainFrm.h"
// CArcSurface 对话框

class CArcSurface : public CDialogEx
{
	DECLARE_DYNAMIC(CArcSurface)

public:
	CArcSurface(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CArcSurface();

// 对话框数据
	enum { IDD = IDD_ARCSURFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedArcOpenfile();
public:
	void SortDraw(CDC* dc, CPtrArray& m_Array);
	CRect BoundingBox(CPtrArray& Array);//用于返回平面二维图形的最小包围矩形框
	void DrawaLabel(CDC* m_pDC, CRect Rect);
	void DrawYLabel(CDC* m_pDC, CRect Rect);
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);
private:
	CPtrArray m_TempArray;//存储的都是空指针
	vector<string> m_TempData;
	CRect rec;//存储显示区域的范围 tagRECT = {top=0 bottom=216 left=0 right=232}
	float ratio;
	CRect PieceSize;//保存整体的工件的rect
	POINT m_CorCenter;//将图形显示设置在picture控件的中心位置
	CRect m_BoundingBox_2D;
	BOOL m_ShowTips;
	//存储两个侧面的圆弧的半径，X侧面的圆弧其半径都相等
	float XRadius, YRadius;
	//
	CSplitToDoc* pDoc;
public:

public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedExtendBtn();
	afx_msg void OnBnClickedShrinkBtn();
	// 记录零件的长度
	float m_ArcPieceLong;
	// 记录零件的宽度
	float m_ArcPieceWidth;
	// 记录零件的高度
	float m_ArcPieceHigth;
	// Y侧面的圆弧半径
	float m_ArcYSideRadius;
	// 记录X侧面圆弧半径
	float m_ArcXSideRadius;
	afx_msg void OnBnClickedTipsBtn();
	// 记录设置的毛坯的长度
	float m_WorkBlankLong;
	// 记录设置的毛坯的宽度
	float m_WorkBlankWidth;
	// 记录设置的毛坯的高度
	float m_WorkBlankHeigth;
	// 记录Y侧圆弧的粗加工进给速度
	float m_Rough_YArcFeedSpeed;
	// 记录Y侧面粗加工进给周期
	float m_Rough_YArcFeedTime;
	// 记录Y侧面粗加工合成进给量
	float m_Rough_YArcFeedLoad;
	// 记录X侧面圆弧粗加工进给速度
	float m_Rough_XArcFeedSpeed;
	// 记录X侧面圆弧粗加工进给周期
	float m_Rough_XArcFeedTime;
	// 记录粗加工X侧面合成进给量
	float m_Rough_XArcFeedLoad;
	// 记录粗加工垂直下刀距离
	float m_Rough_VerticalDown;
	// 记录粗加工加工层数
	float m_Rough_NumberOfPiles;
	// 记录粗加工余量
	float m_Rough_Left;
	// 记录Y侧面圆弧精加工进给速度
	float m_Finish_YArcFeedSpeed;
	// 记录Y侧面精加工进给周期
	float m_Finish_YArcFeedTime;
	// 记录Y侧面圆弧精加工合成进给量
	float m_Finish_YArcFeedLoad;
	// 记录X侧面精加工进给速度
	float m_Finish_XArcFeedSpeed;
	// 记录X侧面精加工进给周期
	float m_Finish_XArcFeedTime;
	// 记录X侧面圆弧精加工合成进给量
	float m_Finish_XArcFeedLoad;
	// 记录精加工垂直下刀距离
	float m_Finish_VerticalDistance;
	// 记录精加工加工层数
	float m_Finish_NumberOfPile;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
