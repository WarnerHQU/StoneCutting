#pragma once
#include<vector>
#include"OPENGL\CadBase.h"
#include"Arc.h"
#include<math.h>
using namespace std;
class CLine
{
public:
	CLine(void);
	~CLine(void);
	CPoint3D m_begin;
	CPoint3D m_end;
	UINT flag;
     float sum;
public:
	
	static double Length(CPoint3D m_begin, CPoint3D m_end);

//方法
public:
	static CPoint3D IntersectOfLine(const CPoint3D& pt0, const CVector3D& vec0,const CPoint3D& pt1, const CVector3D& vec1);//直线与直线的交点
	static CPoint3D IntersectOfArc(CPoint3D& LineBegin, CPoint3D& LineEnd, CPoint3D& center, float radio,int mark, /*float beginAngle, float endAngle,*/ 
		                       CPtrArray& XFarray, int index); //用于判断先直线情况下的直线与圆弧的交点问题

public://时间分割法插补
	static vector<CPoint3D>  LineTDMInterpolation(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F);//XOY起始点，终止点，插补周期ms以及进给速度mm/min
	static vector<CPoint3D>  LineTDMInterpolationXZ(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F);
	static vector<CPoint3D>  LineTDMInterpolationYZ(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F);
};