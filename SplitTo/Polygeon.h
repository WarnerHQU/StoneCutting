#pragma once
#include"SplitToDoc.h"
#include"MainFrm.h"

// CPolygeon 对话框

class CPolygeon : public CDialogEx
{
	DECLARE_DYNAMIC(CPolygeon)

public:
	CPolygeon(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPolygeon();

// 对话框数据
	enum { IDD = IDD_POLYGEON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CSplitToDoc* pDoc;
	vector<OffsetPoint> PolygeonLine;//存储多边形的边
	float m_EveryAngelOfSide;//每条边所对应的圆心角
	CRect rec;//存储图像控件的尺寸信息
	float ratio;//设置比例

public:
	// 记录多边形边数
	int m_PolygeonSideNumber;
	// 记录多边形中心点X坐标
	float m_PolygeonCenterX;
	// 记录多边形中心点的Y坐标
	float m_PolygeonCenterY;
	// 记录多边形中心点的Z坐标
	float m_PolygeonCenterZ;
	// 记录多边形外接圆的半径
	float m_PolygeonCircleRadius;
	// 记录多边形底面的高度
	float m_PolygeonBottomHigh;
	// 记录多边形侧边与底面的夹角
	float m_PolygeonSideAngel;
	// 记录多边形加工中的进给速度
	float m_PolygeonFeedSpeed;
	// 记录多边形加工中的进给周期
	float m_PolygeonFeedTime;
	// 记录多边形加工中的合成进给量
	float m_PolygeonFeedLoad;
	// 记录多边形加工中两边的延伸长度
	float m_PolygeonExtendDisatance;
	// 记录多边形加工中的刀补方向
//	int m_PolygeonCutDirection;
	// 记录多边形加工中锯片的宽度
	float m_PolygeonSawWidth;
	// 记录多边形加工中锯片的直径
	float m_PolygeonSawDiamand;
	// 记录多边形加工中毛坯的长度
	float m_PolygeonWorkBlankLength;
	// 记录多边形加工中毛坯的宽度值
	float m_PolygeonWorkBlankWidth;
	// 记录多边形加工中毛坯的高度
	float m_PolygeonWorkBlankHigh;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedShowBigger();
	afx_msg void OnBnClickedShowSmaller();
};
