#pragma once


// CSawSetDlg 对话框

class CSawSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSawSetDlg)

public:
	CSawSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSawSetDlg();
	void DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length);

// 对话框数据
	enum { IDD = IDD_SAWSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnPaint();

	// 存储锯片的直径(mm)
	float m_Diamond;
	// 存储锯片的厚度
	float m_SawThickness;
	// 存储锯片切割时的延伸长度
	float m_SawExtend;
	virtual BOOL OnInitDialog();
};
