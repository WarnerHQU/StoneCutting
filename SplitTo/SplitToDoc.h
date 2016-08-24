
// SplitToDoc.h : CSplitToDoc 类的接口
//


#pragma once
#include"Arc.h"
#include"OPENGL\CadBase.h"
#include"Circle.h"
#include"Line.h"
#include"fstream"
#include<iostream>
#include<string>
//#include"ToolPath.h"//2016.8.10 TableBoardProcess.h包含有ToolPath.h
#include"TableBoardProcess.h"

//#include"Orthogonal.h"
//#include"Offset.h"
//#include"LineStone.h"
typedef struct SplineData
{
	const int flag = 4;//表示Spline的标志
	CVector3D m_NormalVector;//210 220 230法向矢量
	int Type;//70曲线类型
	int Degree;//71曲线阶次
	int NodeCount;//72节点数
	int CtrlPtCount;//73控制点数
	int FitPtCount;//74拟合点数
	vector<double> NodeValue;//40节点值
	const double WeightFactor=1;//41权因子值
	double m_FitTolerence;//44拟合公差
	double m_CtrlTolerence;//43控制点公差
	double m_NodeTolerence;//42节点公差
	vector<CPoint3D> CtrlPtValue;//10 20 30控制点值 
	vector<CPoint3D> FitPtValue;//11 21 31拟合点值
	CVector3D m_BeginSlope;//起点切向量12 22 32
	CVector3D m_EndSlope;//终点切向量 13 23 33

} Spline;



class CSplitToDoc : public CDocument
{
protected: // 仅从序列化创建
	CSplitToDoc();
	DECLARE_DYNCREATE(CSplitToDoc)

// 特性
public:
	CPtrArray m_ptArray;//存储的都是空指针
	vector<string> data;
	BOOL juge;

	CToolPath tool;

	void MyDraw(COpenGLDC* pDC);//画图形
	void DrawOffset(COpenGLDC* pDC);//画偏移
	void DrawOtho(COpenGLDC* pDC);//画正交


	void ChangeToXOZ();//将存储的点都转为XOZ平面上的点
	void MyDrawXOZ(COpenGLDC* pDC);//画XOZ图形
	void ChangeToYOZ();//将存储的点都转为YOZ平面上的点
	void MyDrawYOZ(COpenGLDC* pDC);//画YOZ图形

	void ClearFlags();

	//多边形加工
public:
	BOOL m_ShowPolygeon;
	BOOL m_ShowPolygeonCuttingPath;
	BOOL m_ShowPolygeonWorkBlank;

	//正交加工
public:
	BOOL m_ShowOrthgonal;
	BOOL m_ShowOrthgonalCutting;
	BOOL m_ShowOrthgonalWorkBlank;
	//void DrawLineProcess(COpenGLDC* pDC);//画直线线条
	//void DrawArcLineProcess(COpenGLDC* pDC);//画圆弧线条
	//CPtrArray m_LineProcess;//存储位置偏移后的轮廓
	//vector<CPtrArray*> m_LineProcessList;//存储位置偏移后离散后的点形成的圆弧的数据
	//CPtrArray m_LinePocessArcArray;//存储圆弧线条的圆弧轮廓
	//BOOL InitArcLineProcess();//根据设置的圆弧的圆心半径起始角和终止角值生成模型

public://线条加工
	BOOL m_ShowLine;//显示工件
	BOOL m_ShowLineRough;//显示粗加工轨迹
	BOOL m_ShowLineWorkBlank;//显示毛坯
	BOOL m_ShowLineRoughCut;//显示裁剪后粗加工轨迹
	BOOL m_ShowLineRoughLeft;//显示粗加工余量
	BOOL m_ShowLineFinish;//显示精加工轨迹
	

public://旋转加工
	BOOL m_ShowRotate;//显示工件示意图
	BOOL m_ShowRotateRough;//显示粗加工轨迹示意图
	BOOL m_ShowRotateWorkBlank;//显示毛坯
	BOOL m_ShowRotateRoughCut;//显示粗加工轨迹裁剪示意图
	BOOL m_ShowRotateFinish;//显示精加工轨迹示意图
	//CPoint3D m_center;
	//void ChangePartToXOZ();//将部分的图素的坐标转化为XOZ平面上
	//void DrawRotate(COpenGLDC* pDC);//画旋转面
public://侧面加工
	BOOL m_ShowSide;
	BOOL m_ShowSidePiece;
	BOOL m_ShowSideRough;
	BOOL m_ShowSideRoughCut;
	BOOL m_ShowSideRoughLeft;//用于判断显示粗加工余量
	BOOL m_ShowSideFinish;//用于判断显示侧面加工精加工路径

	BOOL m_IsBeginSimulation;//粗加工仿真开始判断标志
	BOOL m_IsBeginFinishSimulation;//精加工仿真开始判断标志
	//void ChangeSideProcess();
	//void DrawSide(COpenGLDC* pDC);
	//int mark;//用于存储封闭图形有几个元素
	//CPoint3D XMax;//用于存储X坐标方向上值最大的点的坐标，一般为起点
public://双截面加工
	BOOL m_ShowArc;
	BOOL m_ShowArcRough;
	BOOL m_ShowArcWorkBlank;
	BOOL m_ShowArcRoughAfterCut;
	BOOL m_ShowArcRoughLeft;
	BOOL m_ShowArcFinish;
	//void ChangeCoordinate();//转化为平行于YOZ和YOZ平面
	//void DrawSurface(COpenGLDC* pDC);

	//台面板加工
public:
	BOOL m_ShowBoard;
	BOOL m_ShowBoardWorkBlank;
	
	BOOL m_ShowBoardInSawPath;
	BOOL m_ShowBoardInMillPath;

	BOOL m_ShowBoardOutPath;
	BOOL m_ShowBoardOutCutPath;

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CSplitToDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnFileOpen();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowRoughCut();
	afx_msg void OnShowPiece();
	afx_msg void OnShowRough();
	afx_msg void OnShowSide();
	afx_msg void OnShowFinish();
	afx_msg void OnShowRoughLeft();
//	afx_msg void OnBnClickedBeginSimulation();
	afx_msg void OnBnClickedBeginSimulation();
	afx_msg void OnBnClickedBeginFinishSimulation();
	afx_msg void OnShowRotate();
	afx_msg void OnRoughRotate();
	afx_msg void OnShowRotateWorkBlank();
	afx_msg void OnShowRotateCutRough();
	afx_msg void OnShowRotateFinish();
	afx_msg void OnShowArc();
	afx_msg void OnShowArcRough();
	afx_msg void OnShowArcWorkBlank();
	afx_msg void OnShowArcRoughAfterDelete();
	afx_msg void OnShowArcRoughLeft();
	afx_msg void On32830();
	afx_msg void OnShowLineProcess();
	afx_msg void OnShowLineRough();
	afx_msg void OnShowLineWorkBlank();
//	afx_msg void OnShowLineRoughCut();
	afx_msg void OnShowLineRoughLeft();
	afx_msg void OnShowLineFinish();
	afx_msg void OnShowOrthgonal();
	afx_msg void OnShowOrthgonalCutting();
	afx_msg void OnShowOrthgonalWorkBlank();
	afx_msg void OnShowPolygeon();
	afx_msg void OnShowPolygeonWorkBlank();
	afx_msg void OnShowPolygeonCutting();
	afx_msg void OnShowBoard();
	afx_msg void OnShowBoardWorkBlank();
	afx_msg void OnShowBoardInSawPath();
	afx_msg void OnShowBoardInMillPath();
	afx_msg void OnShowBoardOutOffset();
	afx_msg void OnShowBoardOutCutting();
	afx_msg void OnDelData();
};
