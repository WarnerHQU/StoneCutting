#pragma once


// CMySpliter

class CMySpliter : public CSplitterWnd
{
	DECLARE_DYNAMIC(CMySpliter)

public:
	CMySpliter();
	virtual ~CMySpliter();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	virtual void OnInvertTracker(const CRect& rect);
};


