#pragma once
//***2016.08.20***#include"TableBoardProcess.h"
#include"SplitToDoc.h"
#include"MainFrm.h"
// CTableBoardDlg 对话框



class CTableBoardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTableBoardDlg)
//单例化
	public:
		static CTableBoardDlg* GetTableBoardInstance();
	private:
		static CTableBoardDlg* m_TableBoard;
		
		//私有内嵌类，它唯一的好处就是山吹CTableBoardDlg实例
		class CGarbo
		{
		public:
			~CGarbo()
			{
				if (CTableBoardDlg::m_TableBoard)
					delete CTableBoardDlg::m_TableBoard;
			}
		};
		//程序在结束的时候，系统会自动析构所有的全局变量。
		//事实上，系统也会析构所有的类的静态成员变量，就像这些静态成员也是全局变量一样。
		//定义一个静态变量，程序结束的时候自动调用其析构函数。
		//这样写比直接放在析构函数里面要好，不会出现错误。
		static CGarbo Garbo;

private:
	CTableBoardDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTableBoardDlg();

// 对话框数据
	enum { IDD = IDD_TABLE_BOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();

public:
	//存储图片空间的位置信息
	CRect rec;//tagRECT = {top=0 bottom=240 left=0 right=259}
	//用于控件中绘图
	void MyDraw(CDC* pDC);

	//用于获取零件的尺寸的
	CRect BoundingBox(CPtrArray* Array);
	CRect m_BoundingBox_2D;
	float ratio;
	POINT m_center;
	//画标记线
	BOOL m_ShowTips;
	void SortDraw(CDC* dc, CPtrArray* m_Array);
	void DrawaLabel(CDC* m_pDC, CRect Rect);
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);

private:
	CSplitToDoc* pDoc;

	//存储数据
	CPtrArray* m_TempArray;
	CPtrArray* m_BackupArray;//备份
	CPtrArray m_Array;
public:
	afx_msg void OnBnClickedTbOpenfile();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedExtend();
	afx_msg void OnBnClickedShrink();
	afx_msg void OnBnClickedTips();
	int m_BoardPieceLength;
	int m_BoardPieceWidth;
	// 台面板高度设置
	int m_BoardPieceHighth;
	// 台面板外侧毛坯
	float m_BoardOutLeft;
	// 台面板内侧毛坯
	float m_BoardInLeft;
	float m_FeedSpeed_Horizon;
	float m_FeedTime_Horizon;
	float m_FeedLoad_Horizon;
	// 水平进刀距离
	float m_FeedDis_Horizon;
	float m_FeedSpeed_Vertical;
	float m_FeedTime_Vertical;
	float m_FeedLoad_Vertical;
	float m_ToolDiamand;
	// 铣削速度vc
	float m_MillCutSpeed;
	// 铣刀转速n
	float m_MillRotation;
	// 每齿进给量fz
	float m_MillLoadPerTooth;
	// 每分钟进给量vf
	float m_FeedLoadPerMin;
	// 水平吃刀量
	float m_MillCutLoad_Horizon;
	// 切削深度
	float m_MillCutDepth;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
