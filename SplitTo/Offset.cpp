#include "stdafx.h"
#include "Offset.h"

#define PI 3.1415926
COffset::COffset()
{
}


COffset::~COffset()
{
}

CVector3D COffset::slope(CPoint3D PtBegin, CPoint3D PtEnd)//这里首先主要用的是XY平面上的斜率
{
	CVector3D LineSlope;
	LineSlope.dx = PtEnd.x - PtBegin.x;
	LineSlope.dy = PtEnd.y - PtBegin.y;
	LineSlope.dz = PtEnd.z - PtBegin.z;
	return LineSlope;

}
void  COffset::Polygeon(CPtrArray& Array, float distance, int number, float depth)
{
	LineOffsetPoint.clear();//清空数据
	LineIntersection.clear();//清空数据
	ExtendLine.clear();
	Line.clear();

	for (int i = 0; i < Array.GetSize(); ++i)//假设都走的是左刀补，零件形状顺时针加工，计算没有去延伸量的偏置线
	{
		CVector3D Slope = slope(((CLine*)Array.GetAt(i))->m_begin, ((CLine*)Array.GetAt(i))->m_end);
		OffsetPoint NewPt;
		double length = ((CLine*)Array.GetAt(i))->Length(((CLine*)Array.GetAt(i))->m_begin, ((CLine*)Array.GetAt(i))->m_end);//直线的长度
		double cosXY = fabs(Slope.dx) / sqrt(pow(Slope.dx, 2) + pow(Slope.dy, 2) + pow(Slope.dz, 2));//余弦值,利用直线的方向向量和X轴的单位向量的夹角算的
		double sinXY = sqrt(1 - pow(cosXY, 2));//正弦值
		OffsetPoint point;

		if (Slope.dx < 0.000001&&Slope.dx>-0.000001)
		{
			if (((CLine*)Array.GetAt(i))->m_begin.y>((CLine*)Array.GetAt(i))->m_end.y)
			{
				point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x + distance;
				point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y;
				point.begin.z = 0;
				point.end.x = ((CLine*)Array.GetAt(i))->m_end.x + distance;
				point.end.y = ((CLine*)Array.GetAt(i))->m_end.y;
				point.end.z = 0;

				NewPt.begin = point.begin - Slope.GetNormal() * 50;// 延伸
				NewPt.end = point.end + Slope.GetNormal() * 50;// 延伸

			}
			else
			{
				point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x - distance;
				point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y;
				point.begin.z = 0;
				point.end.x = ((CLine*)Array.GetAt(i))->m_end.x - distance;
				point.end.y = ((CLine*)Array.GetAt(i))->m_end.y;
				point.end.z = 0;

				NewPt.begin = point.begin + Slope.GetNormal() * 50;// 延伸
				NewPt.end = point.end - Slope.GetNormal() * 50;// 延伸

			}
		}
		else
		{
			double ratio = Slope.dy / Slope.dx;

			if (ratio > 0)//斜率为正或者斜率为0
			{
				if (((CLine*)Array.GetAt(i))->m_begin.x <= ((CLine*)Array.GetAt(i))->m_end.x)
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x - distance*sinXY;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y + distance*cosXY;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x - distance*sinXY;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y + distance*cosXY;
					point.end.z = 0;


				}
				else
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x + distance*sinXY;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y - distance*cosXY;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x + distance*sinXY;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y - distance*cosXY;
					point.end.z = 0;


				}
			}
			else if (ratio<0)//斜率为负的时候
			{
				if (((CLine*)Array.GetAt(i))->m_begin.x <= ((CLine*)Array.GetAt(i))->m_end.x)
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x + distance*sinXY;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y + distance*cosXY;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x + distance*sinXY;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y + distance*cosXY;
					point.end.z = 0;

				}
				else
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x - distance*sinXY;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y - distance*cosXY;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x - distance*sinXY;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y - distance*cosXY;
					point.end.z = 0;


				}

			}
			else
			{
				if (((CLine*)Array.GetAt(i))->m_begin.x <= ((CLine*)Array.GetAt(i))->m_end.x)
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y + distance;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y + distance;
					point.end.z = 0;

				}
				else
				{
					point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x;
					point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y - distance;
					point.begin.z = 0;
					point.end.x = ((CLine*)Array.GetAt(i))->m_end.x;
					point.end.y = ((CLine*)Array.GetAt(i))->m_end.y - distance;
					point.end.z = 0;


				}

			}

			NewPt.begin = point.begin - Slope.GetNormal() * 50;// 延伸
			NewPt.end = point.end + Slope.GetNormal() * 50;// 延伸
		}
		LineOffsetPoint.push_back(point);
		ExtendLine.push_back(NewPt);
	}
	for (int i = 0; i != number; ++i)
	{
		for (vector<OffsetPoint>::iterator it = ExtendLine.begin(); it != ExtendLine.end(); ++it)
		{
			CVector3D Dir = slope(it->end, it->begin);
			CVector3D Roat = CVector3D(Dir.dy, -1 * Dir.dx, 0);//投影到XY平面上的向量，逆时针旋转90
			//CVector3D Roat = CVector3D(-1*Dir.dy, Dir.dx, 0);//投影到XY平面上的向量，顺时针旋转90
			vector<OffsetPoint> Sum;
			OffsetPoint Pos;
			Pos.begin = it->begin + Roat.GetNormal()*distance*i;
			Pos.end = it->end + Roat.GetNormal()*distance*i;
			Pos.begin.z -= (i + 1)*depth;//z方向上的距离跟偏置距离一样成45度角
			Pos.end.z -= (i + 1)*depth;
			Sum.push_back(Pos);
			Line.push_back(Sum);
		}
	}


	//接下来就是根据两点求解直线方程并求得两条直线的交点，再将交点存储到LineInteraction里面，最后将点取出划线

	vector<CVector3D> Juge;
	CVector3D CloseGrap = CVector3D(0, 0, 0);
	for (vector<OffsetPoint>::iterator it = LineOffsetPoint.begin(); it != LineOffsetPoint.end(); ++it)//判断是否是封闭图形，矢量和为0
	{
		Juge.push_back(slope(it->begin, it->end));
	}
	for (vector<CVector3D>::iterator it = Juge.begin(); it != Juge.end(); ++it)
	{
		CloseGrap += *it;
	}
	for (vector<OffsetPoint>::iterator it = LineOffsetPoint.begin(); it != LineOffsetPoint.end(); ++it)
	{


		if (CloseGrap.GetLength() == 0)//封闭图形首尾相连
		{

			if (it == LineOffsetPoint.begin())
			{
				LineIntersection.push_back(it->begin);
				CPoint3D  point = TwoLineNot(*it, *(it + 1));
				LineIntersection.push_back(point);
			}
			else if (it == LineOffsetPoint.end() - 1)
			{

				CPoint3D point;
				point = TwoLineNot(*it, *(LineOffsetPoint.begin()));
				LineIntersection.push_back(point);

			}

			else
			{
				CPoint3D  point = TwoLineNot(*it, *(it + 1));
				LineIntersection.push_back(point);
			}
		}
		else//开口图形
		{
			if (it == LineOffsetPoint.begin())
			{
				LineIntersection.push_back(it->begin);
			}
			if (it != LineOffsetPoint.end() - 1)
			{

				CPoint3D  point = TwoLineNot(*it, *(it + 1));
				LineIntersection.push_back(point);
			}
			if (it == LineOffsetPoint.end() - 1)
				LineIntersection.push_back(it->end);

		}

	}

}

CPoint3D  COffset::TwoLineNot(OffsetPoint One, OffsetPoint Another)//已知两条直线求其交点坐标但是缺少水平和竖直的直线的交点的求解
{
	CPoint3D point;//存储交点
	double X, Y, x, y;
	double A, B, a, b;
	if (slope(One.begin, One.end).dx != 0 && slope(Another.begin, Another.end).dx != 0)//两条直线都有斜率
	{
		A = slope(One.begin, One.end).dy / slope(One.begin, One.end).dx;
		a = slope(Another.begin, Another.end).dy / slope(Another.begin, Another.end).dx;
		B = (One.end.x*One.begin.y - One.begin.x*One.end.y) / (One.end.x - One.begin.x);
		b = (Another.end.x*Another.begin.y - Another.begin.x * Another.end.y) / (Another.end.x - Another.begin.x);
		point.x = (b - B) / (A - a);
		point.y = (A*b - a*B) / (A - a);
		point.z = 0;
	}
	else if (slope(One.begin, One.end).dx == 0 && slope(Another.begin, Another.end).dx != 0)//第二条直线斜率无穷大
	{
		point.x = One.begin.x;
		a = slope(Another.begin, Another.end).dy / slope(Another.begin, Another.end).dx;
		b = (Another.end.x*Another.begin.y - Another.begin.x * Another.end.y) / (Another.end.x - Another.begin.x);
		point.y = a*One.begin.x + b;

	}
	else if (slope(One.begin, One.end).dx != 0 && slope(Another.begin, Another.end).dx == 0)//第一条直线斜率无穷大
	{
		point.x = Another.begin.x;
		A = slope(One.begin, One.end).dy / slope(One.begin, One.end).dx;
		B = (One.end.x*One.begin.y - One.begin.x*One.end.y) / (One.end.x - One.begin.x);
		point.y = A*Another.begin.x + B;
	}
	else
	{
		AfxMessageBox(_T("两条直线平行！"));
	}
	return point;
}

