#pragma once

#include"SplitToDoc.h"
#include"MainFrm.h"
// CRotateProcess 对话框

class CRotateProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CRotateProcess)

public:
	CRect BoundingBox(CPtrArray& Array);//用于返回平面二维图形的最小包围矩形框
	void  SortDraw(CDC* dc, CPtrArray& m_Array);//进行圆与非圆的区分
	void DrawaLabel(CDC* m_pDC, CRect Rect);//绘制侧面标注线
	void DrawCircleLabel(CDC* m_pDC, CRect Rect);//绘制底面圆形标注线
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);//画箭头
private:
	CPtrArray m_TempArray;//存储的都是空指针
	vector<string> m_TempData;
	CRect rec;//{top=0 bottom=219 left=0 right=228}	存储显示区域的尺寸
	float ratio;
	CRect PieceSize;//保存整体的工件的rect
	POINT m_CorCenter;//将图形显示设置在picture控件的中心位置
	CRect m_BoundingBox_2D;
	BOOL m_ShowTips;

public:
	CRotateProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRotateProcess();

// 对话框数据
	enum { IDD = IDD_ROTATEPROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedRotateOpenFile();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedShowBigeer();
	afx_msg void OnBnClickedShowSmaller();
	afx_msg void OnBnClickedTips();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// 记录底面的长度(即圆的直径)
	float m_underside_Long;
	// 记录底面的宽度(即圆的直径)
	float m_underside_Width;
	// 记录侧面形状的高度
	float m_Highth;
	// 记录侧面凸出的长度(相对于底面凸出的部分)
	float m_raised;
	// 记录毛坯的长度
	float m_workBlank_Long;
	// 记录所设置毛坯的宽度
	float m_workBlank_Width;
	// 记录所设置毛坯的高度
	float m_workBlank_Heighth;
	// 记录进给的速度
	float m_FeedSpeed;
	// 记录进给的周期
	float m_FeedTime;
	// 记录合成进给量
	float m_FeedLoad;
	// 记录每层进给的距离
	float m_FeedDistance;
	// 记录粗加工进给层数
	float m_RoughPlies;
	// 记录粗加工余量
	float m_RoughLeft;
	// 记录垂直方向的进给速度
	float m_FeedSpeed_VX;
	// 记录垂直方向上的插补周期
	float m_FeedTime_VX;
	// 记录垂直方向的粗加工合成进给量
	float m_FeedLoad_VX;
	// 记录精加工水平进给速度
	float m_FeedSpeed_Finish;
	// 记录精加工进给周期
	float m_FeedTime_Finish;
	// 记录精加工水平方向上的合成进给量
	float m_FeedLoad_Finish;
	// 记录精加工水平方向上每层的切割量
	float m_FeedDistance_Finish;
	// 记录精加工垂直方向上的进给速度
	float m_FeedSpeed_VX_Finish;
	// 记录精加工垂直方向上的进给周期
	float m_FeedTime_VX_Finish;
	// 记录精加工垂直方向上的合成进给量
	float m_FeedLoad_VX_Finish;
	virtual BOOL OnInitDialog();
};
