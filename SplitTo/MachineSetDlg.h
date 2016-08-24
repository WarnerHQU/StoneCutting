#pragma once


// CMachineSetDlg 对话框

class CMachineSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMachineSetDlg)

public:
	CMachineSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMachineSetDlg();

// 对话框数据
	enum { IDD = IDD_MACHINESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};
