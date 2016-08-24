#pragma once
#include"SplitToDoc.h"
#include"MainFrm.h"
// CSideProcess 对话框

/*
通过求取所读取的文件的信息，判别其X\Y轴的最大最小值，并以此求取包容矩形的中心点的坐标，
并以此中心点的坐标设置成picture控件的中心点的坐标再根据相应的比例变换求取坐标点的变化
从而实现在picture图形控件上显示要加工图纸的示意图，使得软件的交互性更强
*/
/*
放控件中间，还不如设置个按钮，根据显示的图形就行调节，使其在对话框内，对ratio变量进行操作，
还要在显示图形的边上写上长宽等的标识，对于侧面加工，由于高度方向上的图形也是显示在了二维
图形中，故要对保存高度方向的数据点进行存储，并画出相应的包围区间，并进行显示，也要标出高度
方向上的h。
*/

class CSideProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CSideProcess)

public:
	CPtrArray m_TempArray;//存储的都是空指针
	vector<string> m_TempData;
	CRect BoundingBox(CPtrArray& Array);//用于返回平面二维图形的最小包围矩形框
	void  SortDraw(CDC* dc,CPtrArray& m_Array);//进行X Y与Z方向上的区分
	void DrawaLabel(CDC* m_pDC, CRect Rect);//绘制标注线，长宽高
	void DrawaLabelHeigth(CDC* m_pDC, CRect Rect);//只绘制高度
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);//画箭头
private:
	BOOL m_IsShow;
	CRect rec;//picture尺寸tagRECT = {top=0 bottom=208 left=0 right=253},设置显示区域放大20倍即长5060，宽4160
	CRect PieceSize;//保存整体的工件的rect
	CRect PieceSizeXY;
	CRect PieceSizeXZ;
	POINT m_CorCenter;//将图形显示设置在picture控件的中心位置
	float ratio;
	CRect m_BoundingBox_2D;
	
public:
	CSideProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSideProcess();

// 对话框数据
	enum { IDD = IDD_SIDEPROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedSideOpenFile();
	afx_msg void OnPaint();
	// 工件的长度	// 工件的长度
	double m_PieceLong;
	// 工件的宽度
	double m_PieceWidth;
	// 工件加工侧面图形的高度
	double m_PieceHeigth;
	afx_msg void OnBnClickedAmphsize();
	afx_msg void OnBnClickedShrink();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedShowTip();
	// 用来表示侧面余出来的尺寸
	double m_PieceExtend;
	// 进给速度F
	float m_Feed;
	// 进给周期T
	float m_Time;
	// 毛坯尺寸长度设置
	float m_LongSet;
	// 毛坯宽度设置
	float m_WidthSet;
	// 毛坯高度设置
	float m_HeigthSet;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// 进给量
	float m_Load;
	float m_Feed_VX;
	// 垂直面的插补周期
	float m_Time_VX;
	// 进给量
	float m_Load_VX;
	virtual BOOL OnInitDialog();
	// 用于表示进刀深度
	float m_DeapthOfCut;
	// 存储粗加工余量
	float m_RoughLeft;
	// 精加工进给速度
	float m_Speed_Finish;
	// 精加工插补周期
	float m_Time_Finish;
	// 精加工垂直面进给速度
	float m_Speed_Finish_VX;
	// 精加工插补周期
	float m_Time_Finish_VX;
	// 精加工合成进给量
	float m_Load_Finish;
	// 垂直面合成进给量
	float m_Load_Finish_VX;
	// 精加工水平面每次的进给深度
	float m_DepthOfCut_Finish;
	// 标注粗加工层数
	int m_Rough_Plies;
};
