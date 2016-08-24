#pragma once
#include"Finish.h"
class COffset : public CFinish
{
public:
	COffset();
	~COffset();

	// 多边形偏置操作
public:
	CVector3D slope(CPoint3D PtBegin, CPoint3D PtEnd);
	vector<CPoint3D> LineIntersection;//存储偏移后交点的坐标
	vector<OffsetPoint> LineOffsetPoint;//存储偏移后直线两点的坐标
	vector<OffsetPoint> ExtendLine;//存储第一层偏置伸长后的点位置
	vector<vector<OffsetPoint>> Line;//存储各层上面点的数据
	void Polygeon(CPtrArray& Array, float distance, int number, float depth);
	CPoint3D TwoLineNot(OffsetPoint One, OffsetPoint Another);

	static void DrawOffset(COpenGLDC* pDC, CPtrArray m_ptArray);
};

