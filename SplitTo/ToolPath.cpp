#include "stdafx.h"
#include "ToolPath.h"
#include"Line.h"
#include<math.h>

#define PI 3.1415926
CToolPath::CToolPath()
{
	//侧面加工
	mark = 0;
	m_FeedSpeed = 0.0;	m_FeedTime = 0.0;
	m_FeedSpeed_VX = 0.0; m_FeedTime_VX = 0.0;

	m_FeedSpeed_Finish = 0.0; m_FeedTime_Finish = 0.0;
	m_FeedSpeed_VX_Finish = 0.0; m_FeedTime_VX_Finish = 0.0;

	m_Piece_Long = 0.0;
	m_Piece_Width = 0.0;
	m_Piece_Heigth = 0.0;

    m_RoughLeft = 0.0;//粗加工余量初始化
	m_DepthOfCut = 0.0;
	m_DepthOfCut_Finish = 0.0;//精加工水平进给量
	m_Rough_Plies = 0;


	//旋转加工
	m_Rotate_FeedSpeed = 0.0;
	m_Rotate_FeedTime = 0.0;
	m_Rotate_FeedSpeed_VX = 0.0;
	m_Rotate_FeedTime_VX = 0.0;
	m_Rotate_RoughCutOfDepth = 0.0;
	m_Rotate_RoughPlies = 0.0;
	m_Rotate_RoughLeft = 0.0;

	m_Rotate_FeedSpeed_Finish = 0.0;
	m_Rotate_FeedTime_Finish = 0.0;
	m_Rotate_FeedSpeed_VX_Finish = 0.0;
	m_Rotate_FeedTime_VX_Finish = 0.0;
	m_Rotate_FinishCutOfDepth = 0.0;


	m_Rotate_WorkBlank_Longth = 0.0;
	m_Rotate_WorkBlank_Width = 0.0;
	m_Rotate_WorkBlank_Heighth = 0.0;

	//多边形加工
	m_EveryAngelOfSide=0.0;
	m_PolygeonPiles = 0.0;
}


CToolPath::~CToolPath()
{
	
}

void CToolPath::DeleteAllKindData()
{
	//侧面加工
	m_ptArray.RemoveAll();
	//旋转加工
	m_Rotate_Array.RemoveAll();
	//双圆弧曲面
	m_ArcArray.RemoveAll();
	//线条加工
	m_LineArray.RemoveAll();
	//正交加工
	SetOrthgonalTrapezoidType(2);
	//多边形加工
	vector<OffsetPoint>().swap(m_PolygeonLines);
	
}

//多边形切割加工*****原先所写的polygeon感觉无用，但是可以参考下***
CVector3D CToolPath::slope(CPoint3D PtBegin, CPoint3D PtEnd)//这里首先主要用的是XY平面上的斜率
{
	CVector3D LineSlope;
	LineSlope.dx = PtEnd.x - PtBegin.x;
	LineSlope.dy = PtEnd.y - PtBegin.y;
	LineSlope.dz = PtEnd.z - PtBegin.z;
	return LineSlope;

}
void  CToolPath::Polygeon(CPtrArray& Array, float distance, int number, float depth)
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
				//point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x - distance;
				point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y;
				point.begin.z = 0;
				point.end.x = ((CLine*)Array.GetAt(i))->m_end.x + distance;
				//point.end.x = ((CLine*)Array.GetAt(i))->m_end.x - distance;
				point.end.y = ((CLine*)Array.GetAt(i))->m_end.y;
				point.end.z = 0;

				NewPt.begin = point.begin - Slope.GetNormal() * 50;// 延伸
				NewPt.end = point.end + Slope.GetNormal() * 50;// 延伸

			}
			else
			{
				point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x - distance;
				//point.begin.x = ((CLine*)Array.GetAt(i))->m_begin.x + distance;
				point.begin.y = ((CLine*)Array.GetAt(i))->m_begin.y;
				point.begin.z = 0;
				point.end.x = ((CLine*)Array.GetAt(i))->m_end.x - distance;
				//point.end.x = ((CLine*)Array.GetAt(i))->m_end.x +distance;
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
		vector<OffsetPoint> Sum;
		for (vector<OffsetPoint>::iterator it = ExtendLine.begin(); it != ExtendLine.end(); ++it)
		{
			CVector3D Dir = slope(it->end, it->begin);
			CVector3D Roat = CVector3D(Dir.dy, -1 * Dir.dx, 0);//投影到XY平面上的向量，逆时针旋转90
			//CVector3D Roat = CVector3D(-1*Dir.dy, Dir.dx, 0);//投影到XY平面上的向量，顺时针旋转90
			/*vector<OffsetPoint> Sum;*/
			OffsetPoint Pos;
			Pos.begin = it->begin + Roat.GetNormal()*distance*i;
			Pos.end = it->end + Roat.GetNormal()*distance*i;
			Pos.begin.z -= (i + 1)*depth;//z方向上的距离跟偏置距离一样成45度角
			Pos.end.z -= (i + 1)*depth;
			Sum.push_back(Pos);
			/*Line.push_back(Sum);*/
		}
		Line.push_back(Sum);
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

CPoint3D  CToolPath::TwoLineNot(OffsetPoint One, OffsetPoint Another)//已知两条直线求其交点坐标但是缺少水平和竖直的直线的交点的求解
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

void CToolPath::RenderPolygeonInit()
{
	//m_NumberOfPolygeon = 5;
	m_EveryAngelOfSide = 360 / m_NumberOfPolygeon;//每条边所对应的圆心角
	m_PolygeonFeedLoad *= 0.001;//微米转为毫米
	// m_PolygeonCenter = CPoint3D(500, 500, 200);
	// m_RadiusOfPolygeon = 300;
	// m_PolygeonSawWidth = 10;
	 //m_PolygeonFeedLoad = 6;
	// m_PolygeonBottomAngel = 60;
	// m_PolygeonBottomHigh = 0;

	////顺时针顺序,零件的准确形状
	//for (int i = 0; i != m_NumberOfPolygeon; ++i)
	//{
	//	OffsetPoint tempPolygeon;
	//	tempPolygeon.begin.x = m_PolygeonCenter.x + m_RadiusOfPolygeon*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
	//	tempPolygeon.begin.y = m_PolygeonCenter.y + m_RadiusOfPolygeon*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
	//	tempPolygeon.begin.z = m_PolygeonCenter.z;

	//	tempPolygeon.end.x = m_PolygeonCenter.x + m_RadiusOfPolygeon*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
	//	tempPolygeon.end.y = m_PolygeonCenter.y + m_RadiusOfPolygeon*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
	//	tempPolygeon.end.z = m_PolygeonCenter.z;

	//	m_PolygeonLines.push_back(tempPolygeon);
	//}

	 m_PolygeonPiles = (m_PolygeonCenter.z - m_PolygeonBottomHigh) / (m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180));
	// m_LeftDistance = int(m_PolygeonCenter.z - m_PolygeonBottomHigh) % int(m_PolygeonFeedLoad* sin(m_PolygeonBottomAngel*PI / 180));


	vector<vector<OffsetPoint>>().swap(m_OffsetPolygeonLines);
	//偏置多边形
	
	for (float j = 0; j < m_PolygeonPiles; ++j)
	{
		vector<OffsetPoint> tempPoly;
		if (m_PolygeonPiles - j < 0.9)//最后一个除不尽的,得有两个
		{
			for (int i = 0; i != m_NumberOfPolygeon; ++i)
			{
				OffsetPoint tempPolygeon;
				tempPolygeon.begin.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.z = m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

				tempPolygeon.end.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.z = m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

				tempPoly.push_back(tempPolygeon);
			}

			for (int i = 0; i != m_NumberOfPolygeon; ++i)
			{
				OffsetPoint tempPolygeon;
				tempPolygeon.begin.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + (m_PolygeonCenter.z-m_PolygeonBottomHigh)/ tan(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + (m_PolygeonCenter.z - m_PolygeonBottomHigh) / tan(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.z = m_PolygeonBottomHigh;

				tempPolygeon.end.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + (m_PolygeonCenter.z - m_PolygeonBottomHigh) / tan(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + (m_PolygeonCenter.z - m_PolygeonBottomHigh) / tan(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.z =m_PolygeonBottomHigh;

				tempPoly.push_back(tempPolygeon);
			}
		}
		else//非最后一个
		{
			for (int i = 0; i != m_NumberOfPolygeon; ++i)
			{
				OffsetPoint tempPolygeon;
				tempPolygeon.begin.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
				tempPolygeon.begin.z = m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

				tempPolygeon.end.x = m_PolygeonCenter.x + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.y = m_PolygeonCenter.y + (m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
				tempPolygeon.end.z = m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

				tempPoly.push_back(tempPolygeon);
			}
		}

		m_OffsetPolygeonLines.push_back(tempPoly);
	}

	//加上锯片宽度后的偏置多边形以及延伸后的锯片轨迹

}

void CToolPath::RenderPolygeon(COpenGLDC* pDC)
{
		for (vector<OffsetPoint>::iterator it = m_PolygeonLines.begin(); it != m_PolygeonLines.end(); ++it)
		{
			pDC->DrawLine(it->begin, it->end);
		}
		
		for (vector<vector<OffsetPoint>>::iterator it = m_OffsetPolygeonLines.begin(); it != m_OffsetPolygeonLines.end(); ++it)
		{
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				pDC->DrawLine(its->begin, its->end);
			}
		}
	
}

void CToolPath::RenderPolygeonCuttingPathInit()//对m_OffsetPolygeonLines中的线进行延伸并且注意加上刀具的宽度
{
	if (m_OffsetPolygeonLines.size() != 0)
	{
		vector<vector<OffsetPoint>>().swap(m_PolygeonCuttingLines);//清空数据
		//加上刀具的宽度，对于XOY平面，其对应的外接圆的半径增长0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180)，高度增长0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180)
		for (float j = 0; j < m_PolygeonPiles; ++j)
		{
			vector<OffsetPoint> tempPoly;
			if (m_PolygeonPiles - j < 0.9)//最后一个除不尽的,得有两个
			{
				for (int i = 0; i != m_NumberOfPolygeon; ++i)
				{
					OffsetPoint tempPolygeon;
					tempPolygeon.begin.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

					tempPolygeon.end.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

					tempPoly.push_back(tempPolygeon);
				}

				for (int i = 0; i != m_NumberOfPolygeon; ++i)
				{
					OffsetPoint tempPolygeon;
					tempPolygeon.begin.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonBottomHigh;

					tempPolygeon.end.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonBottomHigh;

					tempPoly.push_back(tempPolygeon);
				}

			}
			else
			{
				for (int i = 0; i != m_NumberOfPolygeon; ++i)
				{
					OffsetPoint tempPolygeon;
					tempPolygeon.begin.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI/180)+m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*i)*PI / 180);
					tempPolygeon.begin.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

					tempPolygeon.end.x = m_PolygeonCenter.x + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*cos((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.y = m_PolygeonCenter.y + (0.5*m_PolygeonSawWidth*sin(m_PolygeonBottomAngel*PI / 180) + m_RadiusOfPolygeon + j*m_PolygeonFeedLoad * cos(m_PolygeonBottomAngel*PI / 180))*sin((90 - m_EveryAngelOfSide*(i + 1))*PI / 180);
					tempPolygeon.end.z = 0.5*m_PolygeonSawWidth*cos(m_PolygeonBottomAngel*PI / 180) + m_PolygeonCenter.z - j*m_PolygeonFeedLoad * sin(m_PolygeonBottomAngel*PI / 180);

					tempPoly.push_back(tempPolygeon);
				}

			}


			m_PolygeonCuttingLines.push_back(tempPoly);
		}


		//对线段进行延伸
		for (vector<vector<OffsetPoint>>::iterator it = m_PolygeonCuttingLines.begin(); it != m_PolygeonCuttingLines.end(); ++it)
		{
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				CVector3D Slope = slope(its->begin,its->end);
				its->begin = its->begin-Slope.GetNormal()*m_PolygeonExtendDistance;
				its->end = its->end + Slope.GetNormal()*m_PolygeonExtendDistance;
			}
		}

		//对上述线段进行处理
		vector<vector<OffsetPoint>>().swap(m_PolygeonSequentialLines);
		vector<vector<OffsetPoint>> m_tempPolygeonSequentialLines;
		for (int i = 0; i != m_NumberOfPolygeon; ++i)
		{
			vector<OffsetPoint> tempPoly;
			for (vector<vector<OffsetPoint>>::iterator it = m_PolygeonCuttingLines.begin(); it != m_PolygeonCuttingLines.end(); ++it)
			{
				OffsetPoint tempPolygeon;
				tempPolygeon.begin = (*it)[i].begin;
				tempPolygeon.end= (*it)[i].end;

				tempPoly.push_back(tempPolygeon);
			}
			m_tempPolygeonSequentialLines.push_back(tempPoly);
		}

		//对线段的首尾进行变换
		for (vector<vector<OffsetPoint>>::iterator it = m_tempPolygeonSequentialLines.begin(); it != m_tempPolygeonSequentialLines.end(); ++it)
		{
			BOOL Direction = TRUE;
			vector<OffsetPoint> tempPoly;
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				OffsetPoint tempPolygeon;
				if (Direction)
				{
					tempPolygeon.begin = its->begin;
					tempPolygeon.end = its->end;
				}
				else
				{
					tempPolygeon.begin = its->end;
					tempPolygeon.end = its->begin;
				}

				Direction = !Direction;
				tempPoly.push_back(tempPolygeon);
			}
			m_PolygeonSequentialLines.push_back(tempPoly);
		}

		//存入粗精加工仿真点中
		
		vector<vector<vector<CPoint3D>>>().swap(m_tempPolygeonSimulationPts);
		vector<CPoint3D>().swap(FinishSimulationPoints);
		vector<CPoint3D>().swap(RoughSimulationPoints);
		for (vector<vector<OffsetPoint>>::iterator it = m_PolygeonSequentialLines.begin(); it != m_PolygeonSequentialLines.end(); ++it)
		{
			vector<vector<CPoint3D>> m_TSPT;
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				vector<CPoint3D> m_TSPTS = CLine::LineTDMInterpolation(its->begin, its->end, GetPolygeonFeedTime(), GetPolygeonFeedSpeed());

				if (its == it->begin())
				{
					m_TSPTS.insert(m_TSPTS.begin(), CPoint3D(its->begin.x, its->begin.y, its->begin.z + 300));
				}
				else if (its == it->end() - 1)
				{
					if (it->size()%2==0)//偶数个
					{
						m_TSPTS.insert(m_TSPTS.end(), it->begin()->begin);
						m_TSPTS.insert(m_TSPTS.end(), CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));
					}
					else//奇数个
					{
						m_TSPTS.insert(m_TSPTS.end(), it->begin()->end);
						m_TSPTS.insert(m_TSPTS.end(), CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));
					}
				}
				m_TSPT.push_back(m_TSPTS);
				vector<CPoint3D>().swap(m_TSPTS);

			}
			m_tempPolygeonSimulationPts.push_back(m_TSPT);
			vector<vector<CPoint3D>>().swap(m_TSPT);
		}

		for (vector<vector<vector<CPoint3D>>>::iterator it = m_tempPolygeonSimulationPts.begin(); it != m_tempPolygeonSimulationPts.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					RoughSimulationPoints.push_back(*itss);
					FinishSimulationPoints.push_back(*itss);
				}
			}
		}

	}
}

void CToolPath::RenderPolygeonCuttingPath(COpenGLDC* pDC)
{
	/*for (vector<vector<OffsetPoint>>::iterator it = m_PolygeonCuttingLines.begin(); it != m_PolygeonCuttingLines.end(); ++it)
	{
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->DrawLine(its->begin, its->end);
		}
	}*/

	for (vector<vector<OffsetPoint>>::iterator it = m_PolygeonSequentialLines.begin(); it != m_PolygeonSequentialLines.end(); ++it)
	{
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色

			pDC->DrawLine(its->begin, its->end);

			if (its != it->end() - 1)//非每条边的最后一层
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDC->DrawLine(its->end, (its + 1)->begin);

				if (it == m_PolygeonSequentialLines.begin() && its == it->begin())//第一个元素需要画线
				{
					pDC->DrawLine(CPoint3D(its->begin.x, its->begin.y, its->begin.z + 300), its->begin);
				}
			}
			else//每条边的最后一层
			{
				if (it->size() % 2 == 0)//偶数个
				{
					pDC->DrawLine(its->end, it->begin()->begin);//接头

					pDC->SetMaterialColor(RGB(255, 128, 128));//GOO移动线

					pDC->DrawLine(it->begin()->begin, CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));
					if (it != m_PolygeonSequentialLines.end() - 1)//非最后一条边的最后一条线
					{
						pDC->DrawLine(CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);
					}
				}
				else//奇数个
				{
					pDC->DrawLine(its->end, it->begin()->end);//接尾

					pDC->SetMaterialColor(RGB(255, 128, 128));//GOO移动线

					pDC->DrawLine(it->begin()->end, CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));

					if (it != m_PolygeonSequentialLines.end() - 1)//非最后一条边的最后一条线
					{
						pDC->DrawLine(CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);

					}
				}
				
			}
		}
		
	}
}

//正交切割

vector<vector<OffsetPoint>> CToolPath::Horizon(float ToolWidth, float Angle, float Depth, float Height, float Width
	, float Long, float Gap, float PartLength)//YOZ 梯形
{
	vector<vector<OffsetPoint>> HorizonPos;
	int num = Calculate(Width, Gap, PartLength)*2;
	int CutNum = Height / (Depth*sin(Angle*PI/180)) + 2;
	for (int i = 1; i != num+1; ++i)
	{
		if (i % 2 != 0)//i奇数
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				//vector<OffsetPoint> Pos;
				pt.begin.x = 0;
				pt.end.x =Long;
				pt.begin.y = (PartLength + Gap)*(i - 1) / 2 + Height*cos(Angle*PI / 180) / sin(Angle*PI / 180) - 0.5*ToolWidth*sin(Angle*PI / 180) - j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height + 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.y = pt.begin.y;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				//HorizonPos.push_back(Pos);
			}
			HorizonPos.push_back(Pos);
		}
		else
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				//vector<OffsetPoint> Pos;
				pt.begin.x = 0;
				pt.end.x = Long;
				pt.begin.y = (PartLength + Gap)*(i - 2) / 2 + PartLength - Height*cos(Angle*PI / 180) / sin(Angle*PI / 180)
					+ 0.5*ToolWidth*sin(Angle*PI / 180) + j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height + 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.y = pt.begin.y;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				//HorizonPos.push_back(Pos);
			}
			HorizonPos.push_back(Pos);
		}
	}

	return HorizonPos;
}

vector<vector<OffsetPoint>> CToolPath::Vertical(float ToolWidth, float Angle, float Depth, float Height, float Width
	, float Long, float Gap, float PartLength)//XOZ 梯形
{
	vector<vector<OffsetPoint>> VerticalPos;
	int num = Calculate(Long, Gap, PartLength) * 2;
	int CutNum = Height / (Depth*sin(Angle*PI / 180)) + 2;
	for (int i = 1; i != num + 1; ++i)
	{
		if (i % 2 != 0)//i奇数
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				//vector<OffsetPoint> Pos;
				pt.begin.y = 0;
				pt.end.y = Width;
				pt.begin.x = (PartLength + Gap)*(i - 1) / 2 + Height*cos(Angle*PI / 180) / sin(Angle*PI / 180) - 0.5*ToolWidth*sin(Angle*PI / 180) - j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height + 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.x = pt.begin.x;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				//VerticalPos.push_back(Pos);
			}
			VerticalPos.push_back(Pos);
		}
		else
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				/*vector<OffsetPoint> Pos;*/
				pt.begin.y = 0;
				pt.end.y = Width;
				pt.begin.x = (PartLength + Gap)*(i - 2) / 2 + PartLength - Height*cos(Angle*PI / 180) / sin(Angle*PI / 180)
					+ 0.5*ToolWidth*sin(Angle*PI / 180) + j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height + 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.x = pt.begin.x;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				/*VerticalPos.push_back(Pos);*/
			}
			VerticalPos.push_back(Pos);
		}
	}

	return VerticalPos;
}

vector<vector<OffsetPoint>> CToolPath::HorizonRT(float ToolWidth, float Angle, float Depth, float Height, float Width
	, float Long, float Gap, float PartLength)
{
	vector<vector<OffsetPoint>> HorizonPosRT;
	int num = Calculate(Width, Gap, PartLength) * 2;
	int CutNum = Height / (Depth*sin(Angle*PI / 180)) + 2;
	for (int i = 1; i != num + 1; ++i)
	{
		if (i % 2 != 0)//i奇数
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				/*vector<OffsetPoint> Pos;*/
				pt.begin.x = 0;
				pt.end.x = Long;
				pt.begin.y = (PartLength + Gap)*(i - 1) / 2 - 0.5*ToolWidth*sin(Angle*PI / 180) + j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height - 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.y = pt.begin.y;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				/*HorizonPosRT.push_back(Pos);*/
			}
			HorizonPosRT.push_back(Pos);
		}
		else
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				/*vector<OffsetPoint> Pos;*/
				pt.begin.x = 0;
				pt.end.x = Long;
				pt.begin.y = (PartLength + Gap)*(i - 2) / 2 + PartLength + 0.5*ToolWidth*sin(Angle*PI / 180) - j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height - 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.y = pt.begin.y;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				/*HorizonPosRT.push_back(Pos);*/
			}
			HorizonPosRT.push_back(Pos);
		}
	}

	return HorizonPosRT;
}

vector<vector<OffsetPoint>> CToolPath::VerticalRT(float ToolWidth, float Angle, float Depth, float Height, float Width
	, float Long, float Gap, float PartLength)
{
	vector<vector<OffsetPoint>> VerticalPosRT;
	int num = Calculate(Long, Gap, PartLength) * 2;
	int CutNum = Height / (Depth*sin(Angle*PI / 180)) + 2;
	for (int i = 1; i != num + 1; ++i)
	{
		if (i % 2 != 0)//i奇数
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				/*vector<OffsetPoint> Pos;*/
				pt.begin.y = 0;
				pt.end.y = Width;
				pt.begin.x = (PartLength + Gap)*(i - 1) / 2 - 0.5*ToolWidth*sin(Angle*PI / 180) + j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height - 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.x = pt.begin.x;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				/*VerticalPosRT.push_back(Pos);*/
			}
			VerticalPosRT.push_back(Pos);
		}
		else
		{
			vector<OffsetPoint> Pos;
			for (int j = 0; j != CutNum; ++j)
			{
				OffsetPoint pt;
				/*vector<OffsetPoint> Pos;*/
				pt.begin.y = 0;
				pt.end.y = Width;
				pt.begin.x = (PartLength + Gap)*(i - 2) / 2 + PartLength + 0.5*ToolWidth*sin(Angle*PI / 180) - j*Depth*cos(Angle*PI / 180);
				pt.begin.z = Height - 0.5*ToolWidth*cos(Angle*PI / 180) - j*Depth*sin(Angle*PI / 180);
				pt.end.x= pt.begin.x;
				pt.end.z = pt.begin.z;
				Pos.push_back(pt);
				/*VerticalPosRT.push_back(Pos);*/
			}
			VerticalPosRT.push_back(Pos);
		}
	}

	return VerticalPosRT;
}

int CToolPath::Calculate(float Long, float Gap, float PartLength)
{
	
	return Long / (Gap + PartLength);
}

CPoint3D CToolPath::GetOrthgonalWorkBlankCenter(float Length, float Width, float High)
{
	CPoint3D tempPoint;

	tempPoint.z = High / 2;
	tempPoint.x = Length / 2;
	tempPoint.y = Width / 2;

	return tempPoint;
}


BOOL CToolPath::RenderOrthgonalInit()
{
	if (GetOrthgonalTrapezoidType()==0)//正梯形
	{
		if (GetOrthgonalProcessStyle() == 0)//横切，沿着X方向，Y侧参数可设置
		{
			NormalTrapezoid = Horizon(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalYFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalYGap(), GetOrthgonalYLong());
		}
		else if (GetOrthgonalProcessStyle() == 1)//竖切,沿着Y方向，X侧参数可设置
		{
			NormalTrapezoid = Vertical(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalXFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalXGap(), GetOrthgonalXLong());
		}
		else if (GetOrthgonalProcessStyle() == 2)//正交，沿着X方向，Y侧参数可设置；沿着Y方向，X侧参数可设置
		{
			NormalTrapezoid = Horizon(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalYFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalYGap(), GetOrthgonalYLong());
			vector<vector<OffsetPoint>> tempTrapezoid = Vertical(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalXFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalXGap(), GetOrthgonalXLong());
			for (vector<vector<OffsetPoint>>::iterator it = tempTrapezoid.begin(); it != tempTrapezoid.end(); ++it)
			{
				NormalTrapezoid.insert(NormalTrapezoid.end(), *it);//尾部插入数据
			}
		}

		if (NormalTrapezoid.size() != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (GetOrthgonalTrapezoidType() == 1)//倒梯形
	{
		if (GetOrthgonalProcessStyle() == 0)//横切，沿着X方向，Y侧参数可设置
		{
			ReverseTrapezoid = HorizonRT(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalYFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalYGap(), GetOrthgonalYLong());
		}
		else if (GetOrthgonalProcessStyle() == 1)//竖切,沿着Y方向，X侧参数可设置
		{
			ReverseTrapezoid = VerticalRT(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalXFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalXGap(), GetOrthgonalXLong());
		}
		else if (GetOrthgonalProcessStyle() == 2)//正交，沿着X方向，Y侧参数可设置；沿着Y方向，X侧参数可设置
		{
			ReverseTrapezoid = HorizonRT(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalYFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalYGap(), GetOrthgonalYLong());
			vector<vector<OffsetPoint>> tempTrapezoid = VerticalRT(GetOrthgonalSawWidth(), GetOrthgonalTrapezoidAngle(), GetOrthgonalXFeedLoad() / 1000,
				GetOrthgonalWorkBlankHigh(), GetOrthgonalWorkBlankWidth(), GetOrthgonalWorkBlankLength(), GetOrthgonalXGap(), GetOrthgonalXLong());
			for (vector<vector<OffsetPoint>>::iterator it = tempTrapezoid.begin(); it != tempTrapezoid.end(); ++it)
			{
				ReverseTrapezoid.insert(ReverseTrapezoid.end(), *it);//尾部插入数据
			}
		}

		if (ReverseTrapezoid.size() != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}
}

void CToolPath::RenderOrthgonal(COpenGLDC* pDC)
{
		if (!GetOrthgonalTrapezoidType())//正梯形
		{
			for (vector<vector<OffsetPoint>>::iterator it = NormalTrapezoid.begin(); it != NormalTrapezoid.end(); ++it)
			{
				pDC->SetMaterialColor(RGB(255, 0, 0));
				for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
				{
					pDC->DrawLine(its->begin, its->end);
				}
				
				pDC->SetMaterialColor(RGB(0, 255, 0));
				pDC->DrawLine(it->begin()->begin, (it->end() - 1)->begin);
				pDC->DrawLine(it->begin()->end, (it->end() - 1)->end);
			}
		}
		else//倒梯形
		{
			for (vector<vector<OffsetPoint>>::iterator it = ReverseTrapezoid.begin(); it != ReverseTrapezoid.end(); ++it)
			{
				pDC->SetMaterialColor(RGB(255, 0, 0));
				for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
				{
					pDC->DrawLine(its->begin, its->end);
				}
				pDC->SetMaterialColor(RGB(0, 255, 0));
				pDC->DrawLine(it->begin()->begin, (it->end() - 1)->begin);
				pDC->DrawLine(it->begin()->end, (it->end() - 1)->end);
			}
		}
}

void CToolPath::RenderOrthgonalCuttingPathInit()
{
	vector<vector<OffsetPoint>>().swap(NormalTrapezoidCut);
	vector<vector<OffsetPoint>>().swap(ReverseTrapezoidCut);
	if (GetOrthgonalTrapezoidType() == 0)//正梯形
	{
		for (vector<vector<OffsetPoint>>::iterator it = NormalTrapezoid.begin(); it != NormalTrapezoid.end(); ++it)
		{
			vector<OffsetPoint> tempOff;
			BOOL Direction = TRUE;
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				OffsetPoint temp;
				if (Direction)
				{
					temp.begin = its->begin;
					temp.end = its->end;
				}
				else
				{
					temp.begin = its->end;
					temp.end = its->begin;
				}
				tempOff.push_back(temp);
				Direction = !Direction;
			}
			NormalTrapezoidCut.push_back(tempOff);
			vector<OffsetPoint>().swap(tempOff);
		}
	}
	else if (GetOrthgonalTrapezoidType() == 1)//倒梯形
	{
		for (vector<vector<OffsetPoint>>::iterator it = ReverseTrapezoid.begin(); it != ReverseTrapezoid.end(); ++it)
		{
			vector<OffsetPoint> tempOff;
			BOOL Direction = TRUE;
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				OffsetPoint temp;
				if (Direction)
				{
					temp.begin = its->begin;
					temp.end = its->end;
				}
				else
				{
					temp.begin = its->end;
					temp.end = its->begin;
				}
				tempOff.push_back(temp);
				Direction = !Direction;
			}
			ReverseTrapezoidCut.push_back(tempOff);
			vector<OffsetPoint>().swap(tempOff);
		}
	}

	vector<CPoint3D>().swap(FinishSimulationPoints);
	vector<CPoint3D>().swap(RoughSimulationPoints);
	//做精加工的仿真点
	vector<vector<vector<CPoint3D>>>().swap(m_TempNormalFinish);
	vector<vector<vector<CPoint3D>>>().swap(m_TempReverseFinish);

	//对正梯形点的处理
	for (vector<vector<OffsetPoint>>::iterator it = NormalTrapezoidCut.begin(); it != NormalTrapezoidCut.end(); ++it)
	{
		vector<vector<CPoint3D>> m_TempNPiles;
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<CPoint3D> m_TempNPilesPts;
			if (GetOrthgonalProcessStyle() == 0 || GetOrthgonalProcessStyle() == 2)//横切以及正交教工都设置为横切加工的参数
			{
				m_TempNPilesPts = CLine::LineTDMInterpolation(its->begin, its->end, 60, GetOrthgonalYFeedLoad());
			}
			else if (GetOrthgonalProcessStyle() == 1)//纵切加工设置参数为纵切加工
			{
				m_TempNPilesPts = CLine::LineTDMInterpolation(its->begin, its->end, 60, GetOrthgonalXFeedLoad());
			}

			if (its == it->begin())//第一条直线
			{
				m_TempNPilesPts.insert(m_TempNPilesPts.begin(), CPoint3D(its->begin.x, its->begin.y, its->begin.z + 300));
			}
			else if (its == it->end() - 1)//最后一条直线
			{
				if (it->size() % 2 == 0)
				{
					m_TempNPilesPts.insert(m_TempNPilesPts.end(), CPoint3D(it->begin()->begin));
					m_TempNPilesPts.insert(m_TempNPilesPts.end(), CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));
				}
				else
				{
					m_TempNPilesPts.insert(m_TempNPilesPts.end(), CPoint3D(it->begin()->end));
					m_TempNPilesPts.insert(m_TempNPilesPts.end(), CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));
				}
			}
			m_TempNPiles.push_back(m_TempNPilesPts);
			vector<CPoint3D>().swap(m_TempNPilesPts);
		}
			
		m_TempNormalFinish.push_back(m_TempNPiles);
		vector<vector<CPoint3D>>().swap(m_TempNPiles);
	}

	//对倒梯形进行点的处理
	for (vector<vector<OffsetPoint>>::iterator it = ReverseTrapezoidCut.begin(); it != ReverseTrapezoidCut.end(); ++it)
	{
		vector<vector<CPoint3D>> m_TempRPiles;
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<CPoint3D> m_TempRPilesPts;
			if (GetOrthgonalProcessStyle() == 0 || GetOrthgonalProcessStyle() == 2)//横切以及正交教工都设置为横切加工的参数
			{
				m_TempRPilesPts = CLine::LineTDMInterpolation(its->begin, its->end, 60, GetOrthgonalYFeedLoad());
			}
			else if (GetOrthgonalProcessStyle() == 1)//纵切加工设置参数为纵切加工
			{
				m_TempRPilesPts = CLine::LineTDMInterpolation(its->begin, its->end, 60, GetOrthgonalXFeedLoad());
			}

			if (its == it->begin())//第一条直线
			{
				m_TempRPilesPts.insert(m_TempRPilesPts.begin(), CPoint3D(its->begin.x, its->begin.y, its->begin.z + 300));
			}
			else if (its == it->end() - 1)//最后一条直线
			{
				if (it->size() % 2 == 0)
				{
					m_TempRPilesPts.insert(m_TempRPilesPts.end(), CPoint3D(it->begin()->begin));
					m_TempRPilesPts.insert(m_TempRPilesPts.end(), CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));
				}
				else
				{
					m_TempRPilesPts.insert(m_TempRPilesPts.end(), CPoint3D(it->begin()->end));
					m_TempRPilesPts.insert(m_TempRPilesPts.end(), CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));
				}
			}


			m_TempRPiles.push_back(m_TempRPilesPts);
			vector<CPoint3D>().swap(m_TempRPilesPts);
		}
		m_TempReverseFinish.push_back(m_TempRPiles);
		vector<vector<CPoint3D>>().swap(m_TempRPiles);
	}

	//将正梯形点存入粗精加工仿真点中
	for (vector<vector<vector<CPoint3D>>>::iterator it = m_TempNormalFinish.begin(); it != m_TempNormalFinish.end(); ++it)
	{
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				FinishSimulationPoints.push_back(*itss);
				RoughSimulationPoints.push_back(*itss);
			}
		}
	}
	//将倒梯形的点存入粗精加工仿真点中
	for (vector<vector<vector<CPoint3D>>>::iterator it = m_TempReverseFinish.begin(); it != m_TempReverseFinish.end(); ++it)
	{
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				FinishSimulationPoints.push_back(*itss);
				RoughSimulationPoints.push_back(*itss);
			}
		}
	}

}
void CToolPath::RenderOrthgonalCuttingPath(COpenGLDC* pDC)
{
	for (vector<vector<OffsetPoint>>::iterator it = NormalTrapezoidCut.begin(); it != NormalTrapezoidCut.end(); ++it)
	{
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
			pDC->DrawLine(its->begin, its->end);
			if (its != it->end() - 1)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDC->DrawLine(its->end, (its + 1)->begin);
				if (it == NormalTrapezoidCut.begin() && its == it->begin())
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(CPoint3D(its->begin.x,its->begin.y,its->begin.z+300),its->begin);
				}
			}
			else//最后一个
			{
				if (it->size() % 2 == 0)//总共偶数个
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(its->end, it->begin()->begin);
					pDC->DrawLine(it->begin()->begin, CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));//设置300为锯片旋转预留空间
					if (it != NormalTrapezoidCut.end() - 1)
					{
						pDC->DrawLine(CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);
					}
				}
				else//总共有奇数个
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(its->end, it->begin()->end);
					pDC->DrawLine(it->begin()->end, CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));
					if (it != NormalTrapezoidCut.end() - 1)
					{
						pDC->DrawLine(CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);
					}

				}
			}
		}
	}
	for (vector<vector<OffsetPoint>>::iterator it = ReverseTrapezoidCut.begin(); it != ReverseTrapezoidCut.end(); ++it)
	{
		for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
			pDC->DrawLine(its->begin, its->end);
			if (its != it->end() - 1)
			{
				pDC->SetMaterialColor(RGB(148, 0, 211));//深紫罗兰色
				pDC->DrawLine(its->end, (its + 1)->begin);
				if (it == ReverseTrapezoidCut.begin() && its == it->begin())
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(CPoint3D(its->begin.x, its->begin.y, its->begin.z + 300), its->begin);
				}
			}
			else//最后一个
			{
				if (it->size() % 2 == 0)//总共偶数个
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(its->end, it->begin()->begin);
					pDC->DrawLine(it->begin()->begin, CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300));
					if (it != ReverseTrapezoidCut.end() - 1)
					{
						pDC->DrawLine(CPoint3D(it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);
					}
				}
				else//总共有奇数个
				{
					pDC->SetMaterialColor(RGB(255, 128, 128));
					pDC->DrawLine(its->end, it->begin()->end);
					pDC->DrawLine(it->begin()->end, CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300));
					if (it != ReverseTrapezoidCut.end() - 1)
					{
						pDC->DrawLine(CPoint3D(it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300),
							CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300));
						pDC->DrawLine(CPoint3D((it + 1)->begin()->begin.x, (it + 1)->begin()->begin.y, (it + 1)->begin()->begin.z + 300),
							(it + 1)->begin()->begin);
					}
				}
			}
		}
	}

}

//线条加工

vector<CPoint3D> CToolPath::LineDiscrete(CPoint3D m_Begin, CPoint3D m_End, float Inter, float F)
{
	LineDotPos.clear();
	vector<CPoint3D> point = CLine::LineTDMInterpolation(m_Begin, m_End, Inter, F);

	Opposite = Reverse(LineDotPos, 100);

	return point;
}

vector<CPoint3D> CToolPath::ArcDiscrete(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F)
{
	ArcDotPos.clear();
	
	vector<CPoint3D> point = CArc::ArcTDMInterpolation(center, radio, beginAngle, endAngle, XFarray, index,Inter, F);
	

	Opposite = Reverse(ArcDotPos, 100);
	
	return point;
}

vector<CPoint3D> CToolPath::PolyArcDiscrete(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F)
{
	float m_DisBeEnd = sqrt(pow((begin.x - end.x), 2) + pow((begin.y - end.y), 2) + pow((begin.z - end.z), 2));//计算弦长

	CPoint3D m_LineCenter;
	m_LineCenter.x = (begin.x + end.x) / 2;//弦中点坐标
	m_LineCenter.y = (begin.y + end.y) / 2;//弦中点坐标
	m_LineCenter.z = (begin.z + end.z) / 2;//弦中点坐标

	float m_CenterAng = 4 * atan(fabs(TuduAngle));//计算包角

	m_radio = 0.5*m_DisBeEnd / sin(0.5*m_CenterAng);//计算半径


	float h = sqrt(pow(m_radio, 2) - pow(m_DisBeEnd*0.5, 2));//计算交点离弦心的距离

	if (TuduAngle <= 0.0)//其中起点在终点的右边，顺时针凸度值为负*******XY坐标平面
	{
		if (TuduAngle>-1)
		{

			if ((begin.x<end.x) && (begin.y>end.y))//圆弧相对圆心在第一象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x<end.x&&begin.y<end.y)//圆弧相对圆心在第二象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y<end.y)//圆弧相对圆心在第三象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y>end.y)//圆弧相对圆心在第四象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.y == end.y)//两点纵坐标相等的时候
			{
				if (begin.x <= end.x)//起始点在第二象限，终止点在第一象限
				{
					m_center.y = m_LineCenter.y - h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
				else//起始点在第四象限，终止点在第三象限
				{
					m_center.y = m_LineCenter.y + h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}

			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.y <= end.y)//起始点在第一象限，终止点在第四象限
				{
					m_center.x = m_LineCenter.x + h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
				else//起始点在第三象限，终止点在第二象限
				{
					m_center.x = m_LineCenter.x - h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
			}


		}
		else if (TuduAngle == -1)
		{
			m_center.x = m_LineCenter.x;
			m_center.y = m_LineCenter.y;
			m_center.z = 0;
		}
		else
		{
			if ((begin.x<end.x) && (begin.y>end.y))//圆弧起点2，终点4
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x<end.x&&begin.y<end.y)//圆弧起点3，终点1
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y<end.y)//圆弧起点4，终点2
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y>end.y)//圆弧起点1，终点第三
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}

			else if (begin.y == end.y)//两点纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.y = m_LineCenter.y + h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
				else
				{
					m_center.y = m_LineCenter.y - h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}

			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.y <= end.y)
				{
					m_center.x = m_LineCenter.x - h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
				else
				{
					m_center.x = m_LineCenter.x + h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
			}
		}

	}

	else//起点在终点的左边，逆时针
	{
		if (TuduAngle<1)
		{
			if ((begin.x>end.x) && (begin.y<end.y))//圆弧相对圆心在第一象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y>end.y)//圆弧相对圆心在第二象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x<end.x&&begin.y>end.y)//圆弧相对圆心在第三象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x<end.x&&begin.y<end.y)//圆弧相对圆心在第四象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.y == end.y)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.y = m_LineCenter.y + h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
				else
				{
					m_center.y = m_LineCenter.y - h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.y <= end.y)//起始点在第4象限，终止点在第1象限
				{
					m_center.x = m_LineCenter.x - h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
				else//起始点在第2象限，终止点在第3象限
				{
					m_center.x = m_LineCenter.x + h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
			}
		}
		else if (TuduAngle == 1)
		{
			m_center.x = m_LineCenter.x;
			m_center.y = m_LineCenter.y;
			m_center.z = 0;
		}
		else
		{
			if ((begin.x<end.x) && (begin.y>end.y))//圆弧起点2，终点4
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x<end.x&&begin.y<end.y)//圆弧起点3，终点1
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y - h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y<end.y)//圆弧起点4，终点2
			{
				m_center.x = m_LineCenter.x + h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.x>end.x&&begin.y>end.y)//圆弧起点1，终点第三
			{
				m_center.x = m_LineCenter.x - h*abs(begin.y - end.y) / m_DisBeEnd;
				m_center.y = m_LineCenter.y + h*abs(begin.x - end.x) / m_DisBeEnd;
				m_center.z = 0;
			}
			else if (begin.y == end.y)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.y = m_LineCenter.y - h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
				else
				{
					m_center.y = m_LineCenter.y + h;
					m_center.x = m_LineCenter.x;
					m_center.z = 0;
				}
			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.y <= end.y)
				{
					m_center.x = m_LineCenter.x + h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
				else
				{
					m_center.x = m_LineCenter.x - h;
					m_center.y = m_LineCenter.y;
					m_center.z = 0;
				}
			}
		}

	}

	CVector3D CCXY(1, 0, 0);
	CVector3D CS(begin.x - m_center.x, begin.y - m_center.y);
	CVector3D CE(end.x - m_center.x, end.y - m_center.y);

	m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
	m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;

	if ((begin.x>m_center.x) && (begin.y>m_center.y))//圆弧起点相对圆心在第一象限
	{
		if ((end.x>m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y<m_center.y))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x<m_center.x&&begin.y>m_center.y)//圆弧起点相对圆心在第二象限
	{
		if ((end.x>m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y<m_center.y))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x<m_center.x&&begin.y<m_center.y)//圆弧起点相对圆心在第三象限
	{
		if ((end.x>m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y<m_center.y))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x>m_center.x&&begin.y<m_center.y)//圆弧起点相对圆心在第四象限
	{
		if ((end.x>m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y>m_center.y))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.y<m_center.y))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXY) / (CS.GetLength()*CCXY.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXY) / (CE.GetLength()*CCXY.GetLength())) * 360 / PI;
		}
	}
	
	
	CPoint3D temp;
	vector<CPoint3D> ArcTDMPos;
	CPoint3D ptBegin;
	CPoint3D ptEnd;

	float m_ptOffsetX;
	float m_ptOffsetY;

	float m_Juge;

	//float feed = F*Inter / 60;//单位是微米级的
	float feed = 0.001*F*Inter / 60;//单位是毫米级的
	float data;

	BOOL Direction;
	if (TuduAngle > 0)//逆时针凸度值为正
		Direction = TRUE;
	else
		Direction = FALSE;

	
	if (!Direction)//逆时针为TRUE，顺时针为FALSE
	{
		m_ptOffsetX = begin.x - m_center.x;
		m_ptOffsetY = begin.y - m_center.y;
		
	}
	else
	{
		m_ptOffsetX = begin.x - m_center.x;
		m_ptOffsetY = begin.y - m_center.y;
	}

	ArcTDMPos.push_back(begin);

	if (!Direction)//Direcrtion为False时候是顺时针，这里是求顺时针的插补Go2
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetY;//插补起点Y坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetX >= 0 && m_ptOffsetY >= 0)
				m_Juge = 1;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY >= 0)
				m_Juge = 2;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY <= 0)
				m_Juge = 3;
			else if (m_ptOffsetX >= 0 && m_ptOffsetY <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetX) <= fabs(m_ptOffsetY))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y0 - y1;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

				
					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;


				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化

					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第二种方法B
				{
					float x1;
					float y1;

					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio > y1*y1)
						x1 = -sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(m_radio*m_radio - x1*x1);
					//zy = y0 - y1;
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y0 - y1;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = -sqrt(m_radio*m_radio - y1*y1);
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第二种方法B
				{
					float x1;
					float y1;


					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					//判断下以免出现无穷小的情况
					if (m_radio*m_radio > y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					//x1 = sqrt(fabs(radio*radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) /m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
			}

			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.x - end.x, 2) + pow(temp.y - end.y, 2)) > data);
	}
	else
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetY;//插补起点Y坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetX >= 0 && m_ptOffsetY >= 0)
				m_Juge = 1;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY >= 0)
				m_Juge = 2;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY <= 0)
				m_Juge = 3;
			else if (m_ptOffsetX >= 0 && m_ptOffsetY <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetX) <= fabs(m_ptOffsetY))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					
					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;


				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第二种方法A
				{
					float x1;
					float y1;

					zx = -0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;


					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.y = y1 + m_center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
			}
			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.x - end.x, 2) + pow(temp.y - end.y, 2))>data);

	}

	ArcTDMPos.push_back(end);
	Opposite = Reverse(ArcTDMPos, 100);
	return ArcTDMPos;
}

vector<CPoint3D> CToolPath::CircleDiscrete(CPoint3D center, float radius, CPoint3D EnterPoint, float Inter, float F)
{
	vector<CPoint3D> pt = CCircle::CircleTDMInterpolation(center, radius, EnterPoint, Inter, F);

	return pt;
}

vector<LinePos> CToolPath::Reverse(vector<CPoint3D> start,double width)
{
	vector<CPoint3D> end;
	LinePos temp;
	CPoint3D pt;
	vector<LinePos> line;
	for (vector<CPoint3D>::iterator it = start.begin(); it != start.end(); ++it)
	{
		pt.x = it->x;
		pt.y = it->y;
		pt.z = width;
		temp.begin = *it;
		temp.end = pt;
		line.push_back(temp);
	}
	return line;
}

//后添加
void CToolPath::ChangeToXOZ(CPtrArray& Array)
{
	for (int i = 0; i != Array.GetSize(); ++i)
	{
		if (((CLine*)Array.GetAt(i))->flag == 1)
		{
			CPoint3D temp;
			CPoint3D temp1;
			temp.x = ((CLine*)Array.GetAt(i))->m_begin.x;
			temp.y = 0;
			temp.z = ((CLine*)Array.GetAt(i))->m_begin.y;
			((CLine*)Array.GetAt(i))->m_begin = temp;

			temp1.x = ((CLine*)Array.GetAt(i))->m_end.x;
			temp1.y = 0;
			temp1.z = ((CLine*)Array.GetAt(i))->m_end.y;
			((CLine*)Array.GetAt(i))->m_end = temp1;


		}
		else if (((CArc*)Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{

				CPoint3D center;
				center.x = ((CArc*)Array.GetAt(i))->m_center.x;
				center.y = 0;
				center.z = ((CArc*)Array.GetAt(i))->m_center.y;
				((CArc*)Array.GetAt(i))->m_center = center;
			}
			else
			{
				CPoint3D temp;
				CPoint3D temp1;
				temp.x = ((CArc*)Array.GetAt(i))->m_begin.x;
				temp.y = 0;
				temp.z = ((CArc*)Array.GetAt(i))->m_begin.y;
				((CArc*)Array.GetAt(i))->m_begin = temp;

				temp1.x = ((CArc*)Array.GetAt(i))->m_end.x;
				temp1.y = 0;
				temp1.z = ((CArc*)Array.GetAt(i))->m_end.y;
				((CArc*)Array.GetAt(i))->m_end = temp1;
			}
		}
		else if (((CCircle*)Array.GetAt(i))->flag == 3)
		{
			CPoint3D center;
			center.x = ((CCircle*)Array.GetAt(i))->m_center.x;
			center.y = 0;
			center.z = ((CCircle*)Array.GetAt(i))->m_center.y;
			((CCircle*)Array.GetAt(i))->m_center = center;
		}

	}
}
void CToolPath::ChangeToYOZ(CPtrArray& Array)
{
	for (int i = 0; i != Array.GetSize(); ++i)
	{
		if (((CLine*)Array.GetAt(i))->flag == 1)
		{
			CPoint3D temp;
			CPoint3D temp1;
			temp.y = ((CLine*)Array.GetAt(i))->m_begin.x;
			temp.x = 0;
			temp.z = ((CLine*)Array.GetAt(i))->m_begin.y;
			((CLine*)Array.GetAt(i))->m_begin = temp;

			temp1.y = ((CLine*)Array.GetAt(i))->m_end.x;
			temp1.x = 0;
			temp1.z = ((CLine*)Array.GetAt(i))->m_end.y;
			((CLine*)Array.GetAt(i))->m_end = temp1;


		}
		else if (((CArc*)Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{

				CPoint3D center;
				center.y = ((CArc*)Array.GetAt(i))->m_center.x;
				center.x = 0;
				center.z = ((CArc*)Array.GetAt(i))->m_center.y;
				((CArc*)Array.GetAt(i))->m_center = center;
			}
			else
			{
				CPoint3D temp;
				CPoint3D temp1;
				temp.y = ((CArc*)Array.GetAt(i))->m_begin.x;
				temp.x = 0;
				temp.z = ((CArc*)Array.GetAt(i))->m_begin.y;
				((CArc*)Array.GetAt(i))->m_begin = temp;

				temp1.y = ((CArc*)Array.GetAt(i))->m_end.x;
				temp1.x = 0;
				temp1.z = ((CArc*)Array.GetAt(i))->m_end.y;
				((CArc*)Array.GetAt(i))->m_end = temp1;
			}
		}
		else if (((CCircle*)Array.GetAt(i))->flag == 3)
		{
			CPoint3D center;
			center.y = ((CCircle*)Array.GetAt(i))->m_center.x;
			center.x = 0;
			center.z = ((CCircle*)Array.GetAt(i))->m_center.y;
			((CCircle*)Array.GetAt(i))->m_center = center;
		}

	}
}

void CToolPath::LineRenerInit()
{
	if (GetLineStyle())//TRUE为圆弧
	{
		//记录圆心处+X半径的坐标点
		CPoint3D m_MidlePt;
		m_MidlePt.x = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x + ((CArc*)GetLineArcShapeArray().GetAt(0))->m_radio;
		m_MidlePt.y = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y;
		m_MidlePt.z = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.z;

		//转到XOZ平面
		ChangeToXOZ(m_LineArray);
		//平移
		CVector3D m_OffsetDirection;
		for (int i = 0; i != m_LineArray.GetSize(); ++i)
		{
			if (i == 0)
			{
				if (((CLine*)m_LineArray.GetAt(i))->flag == 1)
				{
					m_OffsetDirection.dx = m_MidlePt.x - ((CLine*)m_LineArray.GetAt(i))->m_begin.x;
					m_OffsetDirection.dy = m_MidlePt.y - ((CLine*)m_LineArray.GetAt(i))->m_begin.y;
					m_OffsetDirection.dz = 0;

				}
				else if (((CArc*)m_LineArray.GetAt(i))->flag == 2)
				{
					CArc::CalculateXZ(((CArc*)m_LineArray.GetAt(i))->m_center, ((CArc*)m_LineArray.GetAt(i))->m_radio, ((CArc*)m_LineArray.GetAt(i))->m_begin_angle,
						((CArc*)m_LineArray.GetAt(i))->m_end_angle, m_LineArray, i);
					m_OffsetDirection.dx = m_MidlePt.x - ((CArc*)m_LineArray.GetAt(i))->m_begin.x;
					m_OffsetDirection.dy=m_MidlePt.y - ((CArc*)m_LineArray.GetAt(i))->m_begin.y;
					m_OffsetDirection.dz = 0;
				}
			}

			if (((CLine*)m_LineArray.GetAt(i))->flag == 1)
			{
				((CLine*)m_LineArray.GetAt(i))->m_begin += m_OffsetDirection;
				((CLine*)m_LineArray.GetAt(i))->m_end += m_OffsetDirection;
			}
			else if (((CArc*)m_LineArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_LineArray.GetAt(i))->m_Tudu == 0)
				{
					((CArc*)m_LineArray.GetAt(i))->m_center += m_OffsetDirection;
				}
				else
				{
					((CArc*)m_LineArray.GetAt(i))->m_begin += m_OffsetDirection;
					((CArc*)m_LineArray.GetAt(i))->m_end += m_OffsetDirection;
				}
			}


		}

		//离散
		vector<CPtrArray*>().swap(m_LineArrayArcList);//清空数据
		for (int i = 0; i != m_LineArray.GetSize(); ++i)
		{
			if (((CLine*)m_LineArray.GetAt(i))->flag == 1)
			{
				vector<CPoint3D> m_tempPt = CLine::LineTDMInterpolationXZ(((CLine*)m_LineArray.GetAt(i))->m_begin, ((CLine*)m_LineArray.GetAt(i))->m_end, 
					GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed()/*60, 1500*/);
				for (vector<CPoint3D>::iterator it = m_tempPt.begin(); it != m_tempPt.end(); ++it)
				{
					float m_tempRadius = CLine::Length(*it, CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x, 
						                              ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y,it->z));

					CPtrArray* m_tempArray = new CPtrArray;
					CArc* pArc = new CArc;
					m_tempArray->Add(pArc);
					pArc->m_radio = m_tempRadius;
					pArc->m_center = CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x,
						((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y, it->z);
					pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_begin_angle;
					pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_end_angle;
					pArc->m_Tudu = 0;

					m_LineArrayArcList.push_back(m_tempArray);
				}
			}
			else if (((CArc*)m_LineArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_LineArray.GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					vector<CPoint3D> m_tempPt = CArc::ArcTDMInterpolationXZ(((CArc*)m_LineArray.GetAt(i))->m_center, ((CArc*)m_LineArray.GetAt(i))->m_radio,
						((CArc*)m_LineArray.GetAt(i))->m_begin_angle, ((CArc*)m_LineArray.GetAt(i))->m_end_angle, m_LineArray, i, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed() /*60, 1000*/);
					for (vector<CPoint3D>::iterator it = m_tempPt.begin(); it != m_tempPt.end(); ++it)
					{
						float m_tempRadius = CLine::Length(*it, CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x,
							((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y, it->z));

						CPtrArray* m_tempArray = new CPtrArray;
						CArc* pArc = new CArc;
						m_tempArray->Add(pArc);
						pArc->m_radio = m_tempRadius;
						pArc->m_center = CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x,
							((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y, it->z);
						pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_begin_angle;
						pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_end_angle;
						pArc->m_Tudu = 0;

						m_LineArrayArcList.push_back(m_tempArray);
					}
				}
				else
				{
					vector<CPoint3D> m_tempPt = PolyArcDiscreteXZ(((CArc*)m_LineArray.GetAt(i))->m_begin, ((CArc*)m_LineArray.GetAt(i))->m_end,
						((CArc*)m_LineArray.GetAt(i))->m_Tudu, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed()/* 60, 1000*/);
					for (vector<CPoint3D>::iterator it = m_tempPt.begin(); it != m_tempPt.end(); ++it)
					{
						float m_tempRadius = CLine::Length(*it, CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x,
							((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y, it->z));

						CPtrArray* m_tempArray = new CPtrArray;
						CArc* pArc = new CArc;
						m_tempArray->Add(pArc);
						pArc->m_radio = m_tempRadius;
						pArc->m_center = CPoint3D(((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x,
							((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.y, it->z);
						pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_begin_angle;
						pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0)) ->m_end_angle;
						pArc->m_Tudu = 0;

						m_LineArrayArcList.push_back(m_tempArray);
					}
				}
			}
		}

		//点的离散化
		vector<vector<CPoint3D>>().swap(m_LineArcProcessPts);
		for (vector<CPtrArray*>::iterator it = m_LineArrayArcList.begin(); it != m_LineArrayArcList.end(); ++it)
		{
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)
					{
						vector<CPoint3D> tempPts = CArc::ArcTDMInterpolation(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio,
							((CArc*)(*it)->GetAt(i))->m_begin_angle, ((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, 60, 6000);
						m_LineArcProcessPts.push_back(tempPts);
						vector<CPoint3D>().swap(tempPts);
					}
				}
			}
		}

	}
	else//直线线条,横切与纵切差一个旋转90度
	{
		ChangeToXOZ(m_LineArray);
		vector<LinePos>().swap(m_LineProcess);
		for (int i = 0; i != m_LineArray.GetSize(); ++i)
		{
			if (((CLine*)m_LineArray.GetAt(i))->flag == 1)
			{
				vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_LineArray.GetAt(i))->m_begin, ((CLine*)m_LineArray.GetAt(i))->m_end,
					GetLineFinishHFeedTime(),GetLineFinishHFeedSpeed()/*80, 600*/);

				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end() - 1; ++it)
				{
					
					CPoint3D tempPt = *it;
					//tempPt.y = 200;
					tempPt.y = GetLineWorkBlankLength();
					LinePos tempLinePos;
					tempLinePos.begin = *it;
					tempLinePos.end = tempPt;
					m_LineProcess.push_back(tempLinePos);
					
				}


			}
			else if (((CArc*)m_LineArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_LineArray.GetAt(i))->m_Tudu == 0)//普通圆弧
				{

					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_LineArray.GetAt(i))->m_center, ((CArc*)m_LineArray.GetAt(i))->m_radio, ((CArc*)m_LineArray.GetAt(i))->m_begin_angle,
						((CArc*)m_LineArray.GetAt(i))->m_end_angle, m_LineArray, i, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed()/* 80, 600*/);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end() - 1; ++it)
					{
						CPoint3D tempPt = *it;
						//tempPt.y = 200;
						tempPt.y = GetLineWorkBlankLength();
						LinePos tempLinePos;
						tempLinePos.begin = *it;
						tempLinePos.end = tempPt;
						m_LineProcess.push_back(tempLinePos);
					}
				}
				else//多线段圆弧
				{
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_LineArray.GetAt(i))->m_begin, ((CArc*)m_LineArray.GetAt(i))->m_end,
						((CArc*)m_LineArray.GetAt(i))->m_Tudu, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed()/*80, 600*/);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end() - 1; ++it)
					{
						CPoint3D tempPt = *it;
						//tempPt.y = 200;
						tempPt.y = GetLineWorkBlankLength();

						LinePos tempLinePos;
						tempLinePos.begin = *it;
						tempLinePos.end = tempPt;
						m_LineProcess.push_back(tempLinePos);
					}
				}
			}

		}
	}
	
}
void CToolPath::LineRender(COpenGLDC* pDC)
{
	if (!GetLineStyle())
	{
		for (vector<LinePos>::iterator it = m_LineProcess.begin(); it != m_LineProcess.end()-1; ++it)
		{
			pDC->DrawLine(it->begin, it->end);
			pDC->DrawLine(it->begin, (it + 1)->begin);//隔壁点也连接起来显示轮廓
		}
	}
	else
	{
		for (vector<vector<CPoint3D>>::iterator it = m_LineArcProcessPts.begin(); it != m_LineArcProcessPts.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end() - 1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}
		/*for (vector<CPtrArray*>::iterator it = m_LineArrayArcList.begin(); it != m_LineArrayArcList.end(); ++it)
		{
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					((CArc*)(*it)->GetAt(i))->GLDrawArc(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_begin_angle, ((CArc*)(*it)->GetAt(i))->m_end_angle,
						0.001, ((CArc*)(*it)->GetAt(i))->m_radio, **it, i);
				}
			}
		}*/
	}
}

//获取直线型线条的毛坯中点
CPoint3D CToolPath::GetLineProcessWorkBlankCenter()
{
	CPoint3D center;
	float maxX, minX, maxZ, minZ;
	center.y = GetLineWorkBlankLength() / 2;
	//XOZ平面内的最大最小值

	if (!GetLineStyle())
	{
		for (vector<LinePos>::iterator it = m_LineProcess.begin(); it != m_LineProcess.end(); ++it)
		{
			if (it == m_LineProcess.begin())
			{
				maxX = minX = it->begin.x;
				maxZ = minZ = it->begin.z;
			}
			else
			{
				if (it->begin.x > maxX)
					maxX = it->begin.x;
				if (it->begin.z > maxZ)
					maxZ = it->begin.z;
				if (it->begin.x < minX)
					minX = it->begin.x;
				if (it->begin.z < minZ)
					minZ = it->begin.z;
			}
		}
	}
	else
	{
		for (vector<vector<CPoint3D>>::iterator it = m_LineArcProcessPts.begin(); it != m_LineArcProcessPts.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
			{
				if (it == m_LineArcProcessPts.begin()&&its == it->begin())
				{
					maxX = minX = its->x;
					maxZ = minZ = its->z;
				}
				else
				{
					if (its->x > maxX)
						maxX = its->x;
					if (its->z > maxZ)
						maxZ = its->z;
					if (its->x < minX)
						minX = its->x;
					if (its->z < minZ)
						minZ = its->z;
				}
			}
		}
	}
	center.x = minX + (maxX - minX) / 2;
	center.z = minZ + (maxZ - minZ) / 2;
	return center;
}
BOOL CToolPath::BeInXZLineWorkBlank(CPoint3D point, CPoint3D center, float Width, float High)
{
	CRect tempRect;
	/*tempRect.top = center.z + High / 2;
	tempRect.bottom = center.z - High / 2;*/
	tempRect.top =  High ;
	tempRect.bottom = 0;
	tempRect.left = center.x - Width / 2;
	tempRect.right = center.x + Width / 2;

	if (point.x >= tempRect.left&&point.x <= tempRect.right&&point.z >= tempRect.bottom&&point.z <= tempRect.top)
	{
		return TRUE;
	}
	else
		return FALSE;

}
//直线线条粗加工初始化
void CToolPath::LineRoughRenderInit()
{
	if (m_LineRoughLeftPts.size() != 0)
	{
		
			vector<CPoint3D>().swap(m_LineRoughPts);
			for (vector<CPoint3D>::iterator it = m_LineRoughLeftPts.begin(); it != m_LineRoughLeftPts.end(); ++it)
			{
				//BOOL IsInside = BeInXZLineWorkBlank(*it, GetLineProcessWorkBlankCenter(), GetLineWorkBlankWidth(), GetLineWorkBlankHigh());
				//if (IsInside)
				//{
					m_LineRoughPts.push_back(*it);
				//}
			}
			vector<LinePos>().swap(m_LineRoughVSegment);
			for (vector<CPoint3D>::iterator it = m_LineRoughPts.begin(); it != m_LineRoughPts.end(); ++it)
			{
				LinePos tempLinePos;
				CPoint3D tempBeginPt;
				tempBeginPt = *it;
				tempBeginPt.z = GetLineWorkBlankHigh();
				tempLinePos.begin = tempBeginPt;
				tempLinePos.end = *it;
				m_LineRoughVSegment.push_back(tempLinePos);
			}
			//将首尾点的连线进行离散，沿着V方向
			vector<vector<CPoint3D>>().swap(m_LineRoughVSegmentDisPts);//清除数据
			for (vector<LinePos>::iterator it = m_LineRoughVSegment.begin(); it != m_LineRoughVSegment.end(); ++it)
			{
				vector<CPoint3D> tempDisPts = CLine::LineTDMInterpolationXZ(it->begin, it->end, GetLineRoughVFeedTime(), GetLineRoughVFeedSpeed());
				m_LineRoughVSegmentDisPts.push_back(tempDisPts);
				vector<CPoint3D>().swap(tempDisPts);
			}

			if (!GetLineStyle())//直线线条
			{
			//根据零件的长度将离散后的点进行连接
			vector<vector<LinePos>>().swap(m_LineRoughVSegmentDisLine);
			for (vector<vector<CPoint3D>>::iterator it = m_LineRoughVSegmentDisPts.begin(); it != m_LineRoughVSegmentDisPts.end(); ++it)
			{
				vector<LinePos> tempDisLine;
				for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
				{
					LinePos tempPos;
					tempPos.begin = *its;
					tempPos.end = *its;
					tempPos.end.y = GetLineWorkBlankLength();
					tempDisLine.push_back(tempPos);
				}
				m_LineRoughVSegmentDisLine.push_back(tempDisLine);
				vector<LinePos>().swap(tempDisLine);
			}

			//将点离散后存入粗加工仿真点中
			vector<CPoint3D>().swap(RoughSimulationPoints);
			for (vector<vector<LinePos>>::iterator it = m_LineRoughVSegmentDisLine.begin(); it != m_LineRoughVSegmentDisLine.end(); ++it)
			{
				BOOL Direction = TRUE;
				for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
				{
					vector<CPoint3D> tempPts;
					if (Direction)//正常顺序
					{
						tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineRoughHFeedTime(), GetLineRoughHFeedSpeed());
					}
					else
					{
						tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineRoughHFeedTime(), GetLineRoughHFeedSpeed());
					}

					for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
					{
						RoughSimulationPoints.push_back(*itss);
					}

					if (its == it->end() - 1)//最后一个
					{
						CPoint3D temp;
						if (Direction)
						{
							temp = its->end;
							temp.z = GetLineWorkBlankHigh();

						}
						else
						{
							temp = its->begin;
							temp.z = GetLineWorkBlankHigh();
						}
						RoughSimulationPoints.push_back(temp);
					}
					Direction = !Direction;
				}
			}
		}
		else//圆弧线条
		{
			vector<vector<CPtrArray*>>().swap(m_LineRoughVSegmentDisArc);
			for (vector<vector<CPoint3D>>::iterator it = m_LineRoughVSegmentDisPts.begin(); it != m_LineRoughVSegmentDisPts.end(); ++it)
			{
				vector<CPtrArray*> tempArcArray;
				for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
				{
					CPtrArray* OneTempArcArray = new CPtrArray;
					CArc* pArc = new CArc;
					OneTempArcArray->Add(pArc);
					pArc->m_center = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center;
					pArc->m_center.z = its->z;
					pArc->m_radio = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_radio + fabs(its->x - ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x - ((CArc*)GetLineArcShapeArray().GetAt(0))->m_radio);
					pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_end_angle;

					tempArcArray.push_back(OneTempArcArray);
					/*(*OneTempArcArray).Copy(GetLineArcShapeArray());
					((CArc*)OneTempArcArray->GetAt(0))->m_center.z = its->z;
					((CArc*)OneTempArcArray->GetAt(0))->m_radio += fabs(its->x - ((CArc*)OneTempArcArray->GetAt(0))->m_center.x);*/
				}
				m_LineRoughVSegmentDisArc.push_back(tempArcArray);
				vector<CPtrArray*>().swap(tempArcArray);
			}
			vector<vector<vector<CPoint3D>>>().swap(m_LineRoughVSegmentDisArcDisPts);//点清空
			for (vector<vector<CPtrArray*>>::iterator it = m_LineRoughVSegmentDisArc.begin(); it != m_LineRoughVSegmentDisArc.end(); ++it)
			{
				vector<vector<CPoint3D>> m_tempLineRoughVSegmentDisArcDisPts;
				for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
				{
					vector<CPoint3D> m_tempLineArcDisPt;
					for (int i = 0; i != (*its)->GetSize(); ++i)
					{
						if (((CArc*)(*its)->GetAt(i))->flag == 2)
						{
							if (((CArc*)(*its)->GetAt(i))->m_Tudu == 0)
							{
								m_tempLineArcDisPt = CArc::ArcTDMInterpolation(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_radio,
									((CArc*)(*its)->GetAt(i))->m_begin_angle, ((CArc*)(*its)->GetAt(i))->m_end_angle, **its, i,
									GetLineRoughHFeedTime(),GetLineRoughHFeedSpeed());
							}
						}
					}
					m_tempLineRoughVSegmentDisArcDisPts.push_back(m_tempLineArcDisPt);
					vector<CPoint3D>().swap(m_tempLineArcDisPt);
				}
				m_LineRoughVSegmentDisArcDisPts.push_back(m_tempLineRoughVSegmentDisArcDisPts);
				vector<vector<CPoint3D>>().swap(m_tempLineRoughVSegmentDisArcDisPts);
			}

			vector<CPoint3D>().swap(RoughSimulationPoints);
			for (vector<vector<vector<CPoint3D>>>::iterator it = m_LineRoughVSegmentDisArcDisPts.begin(); it != m_LineRoughVSegmentDisArcDisPts.end(); ++it)
			{
				BOOL Direction = TRUE;
				for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if (Direction)//正向
					{
						for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
						{
							RoughSimulationPoints.push_back(*itss);
						}
					}
					else//逆向
					{
						for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						{
							RoughSimulationPoints.push_back(*itss);
						}
					}
					if (its == it->end() - 1)//同一层最后一个圆弧
					{
						if (Direction)
						{
							RoughSimulationPoints.push_back(CPoint3D((its->end() - 1)->x, (its->end() - 1)->y, GetLineWorkBlankHigh()));

						}
						else
						{
							RoughSimulationPoints.push_back(CPoint3D((its->begin())->x, (its->begin())->y, GetLineWorkBlankHigh()));
						}
					}

					Direction = !Direction;
				}


			}

		}

	}
}
void CToolPath::LineRoughRender(COpenGLDC* pDC)
{
	/*if (m_LineRoughPts.size() != 0)
	{
		for (vector<CPoint3D>::iterator it = m_LineRoughPts.begin(); it != m_LineRoughPts.end() - 1; ++it)
		{
			pDC->DrawLine(*it, *(it + 1));
		}
	}*/
	for (vector<LinePos>::iterator it = m_LineRoughVSegment.begin(); it != m_LineRoughVSegment.end(); ++it)
	{
		pDC->DrawLine(it->begin, it->end);
	}
	glPointSize(2);
	for (vector<vector<CPoint3D>>::iterator it = m_LineRoughVSegmentDisPts.begin(); it != m_LineRoughVSegmentDisPts.end(); ++it)
	{
		for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->DrawPoint(*its);
		}
	}
	if (!GetLineStyle())//直线
	{
		for (vector<vector<LinePos>>::iterator it = m_LineRoughVSegmentDisLine.begin(); it != m_LineRoughVSegmentDisLine.end(); ++it)
		{
			BOOL Direction = TRUE;
			for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
			{
				pDC->DrawLine(its->begin, its->end);

				if (its == it->end() - 1)
				{
					pDC->SetMaterialColor(RGB(0, 0, 255));
					if (Direction)
					{
						pDC->DrawLine(its->end, CPoint3D(its->end.x, its->end.y, GetLineWorkBlankHigh() /*+ 10*/));//高度上伸出来些
					}
					else
					{
						pDC->DrawLine(its->begin, CPoint3D(its->begin.x, its->begin.y, GetLineWorkBlankHigh()/* + 10*/));//高度上伸出来些
					}
				}
				else
				{
					pDC->SetMaterialColor(RGB(0, 255, 0));
					if (Direction)
					{
						pDC->DrawLine(its->end, (its + 1)->end);
					}
					else
					{
						pDC->DrawLine(its->begin, (its + 1)->begin);
					}
				}
				Direction = !Direction;
			}
		}
	}
	else//圆弧
	{
		/*for (vector<vector<CPtrArray*>>::iterator it = m_LineRoughVSegmentDisArc.begin(); it != m_LineRoughVSegmentDisArc.end(); ++it)
		{
			for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (int i = 0; i != (*its)->GetSize(); ++i)
				{
					if (((CArc*)(*its)->GetAt(i))->flag == 2)
					{
						if (((CArc*)(*its)->GetAt(i))->m_Tudu == 0)
						{
							((CArc*)(*its)->GetAt(i))->GLDrawArc(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_begin_angle,
								((CArc*)(*its)->GetAt(i))->m_end_angle, 0.001, ((CArc*)(*its)->GetAt(i))->m_radio, **its, i);
						}
					}
				}
			}
		}*/
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_LineRoughVSegmentDisArcDisPts.begin(); it != m_LineRoughVSegmentDisArcDisPts.end(); ++it)
		{
			BOOL Direction = TRUE;
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end() - 1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}

				if (its == it->end() - 1)//最后一个圆弧
				{
					if (Direction)
					{
						pDC->DrawLine(*(its->end() - 1), CPoint3D((its->end() - 1)->x, (its->end() - 1)->y, GetLineWorkBlankHigh()));
					
					}
					else
					{
						pDC->DrawLine(*(its->begin()), CPoint3D((its->begin())->x, (its->begin())->y, GetLineWorkBlankHigh()));
					}
				}
				else//非最后一个，上一个的终点接下一个
				{
					if (Direction)//上一个的终点接下一个的终点
					{
						pDC->DrawLine(*(its->end() - 1), *((its + 1)->end() - 1));
					}
					else//上一个的起点接下一个的起点
					{
						pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
					}
				}
				Direction = !Direction;
			}

			if (it == m_LineRoughVSegmentDisArcDisPts.end() - 1)//最后一组圆弧
			{

			}
			else
			{
				if (Direction)
				{
					pDC->DrawLine(*(it->begin()->begin()), *((it + 1)->begin()->begin()));
					//pDC->DrawLine(*(it->begin())->begin(), *((it + 1)->begin())->begin());
				}
				else
				{
					pDC->DrawLine(*(it->begin()->end() - 1), *((it + 1)->begin()->begin()));
					//pDC->DrawLine(*(it->begin())->end() - 1, *((it + 1)->begin->begin()));
				}
			}

		}
	}

}

//直线线条粗加工余量初始化及显示,整体向Z方向平移粗加工余量，后根据毛坯距离粗加工余量表面距离按照V方向进行插补形成粗加工轨迹线，
  //接着根据粗加工余量距离工件表面距离按照精加工V方向进行插补计算生成精加工轨迹线0526
void CToolPath::LineRoughLeftRenderInit()
{
	m_LineRoughLeft.RemoveAll();
	vector<CPoint3D>().swap(m_LineRoughLeftPts);//清零
	for (int i = 0; i != m_LineArray.GetSize(); ++i)
	{
		if (((CLine*)m_LineArray.GetAt(i))->flag == 1)
		{
			CLine* pLine = new CLine;
			m_LineRoughLeft.Add(pLine);
			pLine->m_begin = ((CLine*)m_LineArray.GetAt(i))->m_begin;
			pLine->m_end = ((CLine*)m_LineArray.GetAt(i))->m_end;
			/*pLine->m_begin.x += GetLineRoughLeft();
			pLine->m_end.x += GetLineRoughLeft();*/
			pLine->m_begin.z += GetLineRoughLeft();
			pLine->m_end.z += GetLineRoughLeft();
		}
		else if (((CArc*)m_LineArray.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_LineArray.GetAt(i))->m_Tudu == 0)
			{
				CArc* pArc = new CArc;
				m_LineRoughLeft.Add(pArc);
				pArc->m_center = ((CArc*)m_LineArray.GetAt(i))->m_center;
				pArc->m_radio = ((CArc*)m_LineArray.GetAt(i))->m_radio;
				pArc->m_begin_angle = ((CArc*)m_LineArray.GetAt(i))->m_begin_angle;
				pArc->m_end_angle = ((CArc*)m_LineArray.GetAt(i))->m_end_angle;
				pArc->m_Tudu = 0;
				/*pArc->m_center.x += GetLineRoughLeft();*/
				pArc->m_center.z += GetLineRoughLeft();
			}
			else
			{
				CArc* pArc = new CArc;
				m_LineRoughLeft.Add(pArc);
				pArc->m_begin = ((CArc*)m_LineArray.GetAt(i))->m_begin;
				pArc->m_end = ((CArc*)m_LineArray.GetAt(i))->m_end;
				pArc->m_Tudu = ((CArc*)m_LineArray.GetAt(i))->m_Tudu;
				pArc->m_begin.z += GetLineRoughLeft();
				pArc->m_end.z += GetLineRoughLeft();
			}
		}
	}
	//将粗加工余量进行离散,按照设置的粗加工值
	for (int i = 0; i != m_LineRoughLeft.GetSize(); ++i)
	{
		if (((CLine*)m_LineRoughLeft.GetAt(i))->flag == 1)
		{
			vector<CPoint3D> tempRoughLeftPt = CLine::LineTDMInterpolationXZ(((CLine*)m_LineRoughLeft.GetAt(i))->m_begin,
				((CLine*)m_LineRoughLeft.GetAt(i))->m_end, GetLineRoughHFeedTime(), GetLineRoughHFeedSpeed());
			for (vector<CPoint3D>::iterator it = tempRoughLeftPt.begin(); it != tempRoughLeftPt.end(); ++it)
			{
				m_LineRoughLeftPts.push_back(*it);
			}
			vector<CPoint3D>().swap(tempRoughLeftPt);
		}
		else if (((CArc*)m_LineRoughLeft.GetAt(i))->flag == 2)
		{
			vector<CPoint3D> tempRoughLeftPt;
			if (((CArc*)m_LineRoughLeft.GetAt(i))->m_Tudu == 0)
			{
				tempRoughLeftPt = CArc::ArcTDMInterpolationXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_center, ((CArc*)m_LineRoughLeft.GetAt(i))->m_radio,
					((CArc*)m_LineRoughLeft.GetAt(i))->m_begin_angle, ((CArc*)m_LineRoughLeft.GetAt(i))->m_end_angle, m_LineRoughLeft, i, GetLineRoughHFeedTime(),
					GetLineRoughHFeedSpeed());
			}
			else
			{
				tempRoughLeftPt = PolyArcDiscreteXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_begin, ((CArc*)m_LineRoughLeft.GetAt(i))->m_end,
					((CArc*)m_LineRoughLeft.GetAt(i))->m_Tudu, GetLineRoughHFeedTime(), GetLineRoughHFeedSpeed());
			}
			for (vector<CPoint3D>::iterator it = tempRoughLeftPt.begin(); it != tempRoughLeftPt.end(); ++it)
			{
				m_LineRoughLeftPts.push_back(*it);
			}
			vector<CPoint3D>().swap(tempRoughLeftPt);
		}
	}

	if (!GetLineStyle())//直线
	{
		vector<LinePos>().swap(m_LineRoughLeftLines);//清零
		for (vector<CPoint3D>::iterator it = m_LineRoughLeftPts.begin(); it != m_LineRoughLeftPts.end(); ++it)
		{
			LinePos tempPos;
			tempPos.begin = *it;
			tempPos.end = *it;
			tempPos.end.y = GetLineWorkBlankLength();
			m_LineRoughLeftLines.push_back(tempPos);
		}
	}
	else//圆弧
	{
		//由离散后的点求出其粗加工余量圆弧，并离散保存
		vector<CPtrArray*>().swap(m_LineRoughLeftArc);
		for (vector<CPoint3D>::iterator it = m_LineRoughLeftPts.begin(); it != m_LineRoughLeftPts.end(); ++it)
		{
			CPtrArray* tempArcArray = new CPtrArray;
			CArc* pArc = new CArc;
			tempArcArray->Add(pArc);

			pArc->m_center = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center;
			pArc->m_center.z = it->z;
			pArc->m_radio = fabs(it->x - ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x);
			pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_begin_angle;
			pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_end_angle;

			m_LineRoughLeftArc.push_back(tempArcArray);
		}

		//圆弧的离散
		vector<vector<CPoint3D>>().swap(m_LineRoughLeftArcDisPts);
		for (vector<CPtrArray*>::iterator it = m_LineRoughLeftArc.begin(); it != m_LineRoughLeftArc.end(); ++it)
		{
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)
					{
						vector<CPoint3D> m_tempLineArc = CArc::ArcTDMInterpolation(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio,
							((CArc*)(*it)->GetAt(i))->m_begin_angle, ((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i,
							GetLineRoughHFeedTime(), GetLineRoughHFeedSpeed());
						m_LineRoughLeftArcDisPts.push_back(m_tempLineArc);
						vector<CPoint3D>().swap(m_tempLineArc);
					}
				}
			}
		}
	}



}
void CToolPath::LineRoughLeftRender(COpenGLDC* pDC)
{
	for (int i = 0; i != m_LineRoughLeft.GetSize(); ++i)
	{
		if (((CLine*)m_LineRoughLeft.GetAt(i))->flag==1)
		{
			pDC->DrawLine(((CLine*)m_LineRoughLeft.GetAt(i))->m_begin, ((CLine*)m_LineRoughLeft.GetAt(i))->m_end);
		}
		else if (((CArc*)m_LineRoughLeft.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_LineRoughLeft.GetAt(i))->m_Tudu == 0)
			{
				((CArc*)m_LineRoughLeft.GetAt(i))->GLDrawArcXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_center, ((CArc*)m_LineRoughLeft.GetAt(i))->m_begin_angle, ((CArc*)m_LineRoughLeft.GetAt(i))->m_end_angle,
					0.001, ((CArc*)m_LineRoughLeft.GetAt(i))->m_radio, m_LineRoughLeft, i);
			}
			else
			{
				((CArc*)m_LineRoughLeft.GetAt(i))->PolyCalculateXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_begin, ((CArc*)m_LineRoughLeft.GetAt(i))->m_end,
					((CArc*)m_LineRoughLeft.GetAt(i))->m_Tudu);
				if (((CArc*)m_LineRoughLeft.GetAt(i))->m_Tudu > 0)
				{
					((CArc*)m_LineRoughLeft.GetAt(i))->PolyDrawNArcXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_center, ((CArc*)m_LineRoughLeft.GetAt(i))->m_begin_angle,
						((CArc*)m_LineRoughLeft.GetAt(i))->m_end_angle, 0.001, ((CArc*)m_LineRoughLeft.GetAt(i))->m_radio);
				}
				else
				{
					((CArc*)m_LineRoughLeft.GetAt(i))->PolyDrawSArcXZ(((CArc*)m_LineRoughLeft.GetAt(i))->m_center, ((CArc*)m_LineRoughLeft.GetAt(i))->m_begin_angle,
						((CArc*)m_LineRoughLeft.GetAt(i))->m_end_angle, 0.001, ((CArc*)m_LineRoughLeft.GetAt(i))->m_radio);
				}
			}
		}
	}
	if (!GetLineStyle())
	{
		for (vector<LinePos>::iterator it = m_LineRoughLeftLines.begin(); it != m_LineRoughLeftLines.end(); ++it)
		{
			pDC->DrawLine(it->begin, it->end);
		}
	}
	else
	{
		for (vector<vector<CPoint3D>>::iterator it = m_LineRoughLeftArcDisPts.begin(); it != m_LineRoughLeftArcDisPts.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end() - 1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}
	}
}

//精加工
void CToolPath::LineFinishRenderInit()
{
	if (!GetLineStyle())//直线
	{
		vector<LinePos>().swap(m_LineFinishVSegment);
		for (vector<LinePos>::iterator it = m_LineProcess.begin(); it != m_LineProcess.end(); ++it)
		{
			LinePos tempPos;
			tempPos.end = it->begin;
			tempPos.begin = it->begin;
			tempPos.begin.z += GetLineRoughLeft();
			m_LineFinishVSegment.push_back(tempPos);
		}
		vector<vector<CPoint3D>>().swap(m_LineFinishVSegmentDisPts);
		for (vector<LinePos>::iterator it = m_LineFinishVSegment.begin(); it != m_LineFinishVSegment.end();++it)
		{
			vector<CPoint3D> tempPts = CLine::LineTDMInterpolationXZ(it->begin, it->end, 
			GetLineFinishVFeedTime(), GetLineFinishVFeedSpeed());
			m_LineFinishVSegmentDisPts.push_back(tempPts);
			vector<CPoint3D>().swap(tempPts);
		}
		vector<vector<LinePos>>().swap(m_LineFinishVSegmentDisLine);
		for (vector<vector<CPoint3D>>::iterator it = m_LineFinishVSegmentDisPts.begin(); it != m_LineFinishVSegmentDisPts.end(); ++it)
		{
			vector<LinePos> tempDisLine;
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
			{
				LinePos tempPos;
				tempPos.begin = *its;
				tempPos.end = *its;
				tempPos.end.y = GetLineWorkBlankLength();
				tempDisLine.push_back(tempPos);
			}
			m_LineFinishVSegmentDisLine.push_back(tempDisLine);
			vector<LinePos>().swap(tempDisLine);
		}//之后还要对其所有线进行精加工H方向的插补计算

		
		//精加工有问题，精加工应该是第一刀下刀的时候是按照垂直方向设置的距离分几刀下刀切割到零件需要的尺寸，然后就是按照零件的尺寸进行切割，无须分成几刀下刀
		vector<CPoint3D>().swap(FinishSimulationPoints);//仿真点清零
		//for (vector<vector<LinePos>>::iterator it = m_LineFinishVSegmentDisLine.begin(); it != m_LineFinishVSegmentDisLine.end(); ++it)
		//{
		//	BOOL Direction = TRUE;
		//	for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
		//	{	
		//		if (its == it->end() - 1)//最后一个
		//		{
		//			vector<CPoint3D> tempPts;
		//			if (Direction)//正常顺序
		//			{
		//				tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
		//			}
		//			else
		//			{
		//				tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
		//			}
		//			for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
		//			{
		//				FinishSimulationPoints.push_back(*itss);
		//			}

		//			CPoint3D temp;
		//			if (Direction)
		//			{
		//				//temp = its->end;
		//				//temp.z = GetLineWorkBlankHigh();

		//			}
		//			else
		//			{
		//				//temp = its->begin;
		//				//temp.z = GetLineWorkBlankHigh();
		//			}
		//			FinishSimulationPoints.push_back(temp);
		//		}
		//		else
		//		{
		//			vector<CPoint3D> tempPts;
		//			if (Direction)//正常顺序
		//			{
		//				tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
		//			}
		//			else
		//			{
		//				tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
		//			}
		//			for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
		//			{
		//				FinishSimulationPoints.push_back(*itss);
		//			}
		//		}
		//		Direction = !Direction;
		//	}
		//}

		for (vector<vector<LinePos>>::iterator it = m_LineFinishVSegmentDisLine.begin(); it != m_LineFinishVSegmentDisLine.begin() + 1; ++it)
		{
			//只要第一列的元素,第一刀下刀
			BOOL Direction = TRUE;
			for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
			{
				vector<CPoint3D> tempPts;
				if (Direction)//正常顺序
				{
					tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineFinishVFeedTime(), GetLineFinishVFeedSpeed());
				}
				else
				{
					tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineFinishVFeedTime(), GetLineFinishVFeedSpeed());
				}
				for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
				{
					FinishSimulationPoints.push_back(*itss);
				}
				Direction = !Direction;
			}

			Direction = TRUE;
			if (*(FinishSimulationPoints.end() - 1) == m_LineProcess.begin()->begin)//若已提取的精加工点的最后一点与零件轮廓第一条线的起点相等
			{
				for (vector<LinePos>::iterator its = m_LineProcess.begin() + 1; its != m_LineProcess.end(); ++its)
				{
					vector<CPoint3D> tempPts;
					if (Direction)
					{
						tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
					}
					else
					{
						tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
					}
					for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
					{
						FinishSimulationPoints.push_back(*itss);
					}
					Direction = !Direction;
				}
			}
			else if (*(FinishSimulationPoints.end() - 1) == m_LineProcess.begin()->end)
			{
				for (vector<LinePos>::iterator its = m_LineProcess.begin() + 1; its != m_LineProcess.end(); ++its)
				{
					vector<CPoint3D> tempPts;
					if (Direction)
					{
						tempPts = CLine::LineTDMInterpolation(its->end, its->begin, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
					}
					else
					{
						tempPts = CLine::LineTDMInterpolation(its->begin, its->end, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
					}
					for (vector<CPoint3D>::iterator itss = tempPts.begin(); itss != tempPts.end(); ++itss)
					{
						FinishSimulationPoints.push_back(*itss);
					}
					Direction = !Direction;
				}
			}
			FinishSimulationPoints.push_back(CPoint3D((FinishSimulationPoints.end() - 1)->x, (FinishSimulationPoints.end() - 1)->y, GetLineWorkBlankHigh()));

		}
	}
	else//圆弧线条
	{
		vector<LinePos>().swap(m_LineFinishVSegment);
		vector<CPoint3D>().swap(m_LineArcSurfacePts);
		for (int i = 0; i != GetLineArray().GetSize(); ++i)
		{
			if (((CLine*)GetLineArray().GetAt(i))->flag == 1)
			{
				vector<CPoint3D> m_tempLineArc = CLine::LineTDMInterpolationXZ(((CLine*)GetLineArray().GetAt(i))->m_begin,
					((CLine*)GetLineArray().GetAt(i))->m_end, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
				for (vector<CPoint3D>::iterator it = m_tempLineArc.begin(); it != m_tempLineArc.end(); ++it)
				{
					m_LineArcSurfacePts.push_back(*it);
				}
				vector<CPoint3D>().swap(m_tempLineArc);
			}
			else if (((CArc*)GetLineArray().GetAt(i))->flag == 2)
			{
				vector<CPoint3D> m_tempLineArc;

				if (((CArc*)GetLineArray().GetAt(i))->m_Tudu == 0)
				{
					m_tempLineArc = CArc::ArcTDMInterpolationXZ(((CArc*)GetLineArray().GetAt(i))->m_center, ((CArc*)GetLineArray().GetAt(i))->m_radio,
						((CArc*)GetLineArray().GetAt(i))->m_begin_angle, ((CArc*)GetLineArray().GetAt(i))->m_end_angle, GetLineArray(), i, 
						GetLineFinishHFeedTime(),GetLineFinishHFeedSpeed());
				}
				else
				{
					m_tempLineArc = PolyArcDiscreteXZ(((CArc*)GetLineArray().GetAt(i))->m_begin, ((CArc*)GetLineArray().GetAt(i))->m_end,
						((CArc*)GetLineArray().GetAt(i))->m_Tudu, GetLineFinishHFeedTime(), GetLineFinishHFeedSpeed());
				}
				for (vector<CPoint3D>::iterator it = m_tempLineArc.begin(); it != m_tempLineArc.end(); ++it)
				{
					m_LineArcSurfacePts.push_back(*it);
				}
				vector<CPoint3D>().swap(m_tempLineArc);
			}
		}

		for (vector<CPoint3D>::iterator it = m_LineArcSurfacePts.begin(); it != m_LineArcSurfacePts.end(); ++it)
		{
			LinePos tempPos;
			tempPos.begin = *it;
			tempPos.begin.z += GetLineRoughLeft();
			tempPos.end = *it;
			m_LineFinishVSegment.push_back(tempPos);
		}
		
		vector<CPtrArray*>().swap(m_LineFinishVSegmentDisArc);
		
		for (vector<LinePos>::iterator it = m_LineFinishVSegment.begin(); it != m_LineFinishVSegment.begin() + 1; ++it)
		{
			vector<CPoint3D> m_tempLineArc = CLine::LineTDMInterpolationXZ(it->begin, it->end, 
				                                        GetLineFinishVFeedTime(), GetLineFinishVFeedSpeed());
			for (vector<CPoint3D>::iterator its = m_tempLineArc.begin(); its != m_tempLineArc.end(); ++its)
			{
				CPtrArray* tempArcArray = new CPtrArray;
				CArc* pArc = new CArc;
				tempArcArray->Add(pArc);

				pArc->m_center = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center;
				pArc->m_center.z = its->z;
				pArc->m_radio = fabs(its->x - ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x);
				pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_begin_angle;
				pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_end_angle;

				m_LineFinishVSegmentDisArc.push_back(tempArcArray);
				
			}
		}
		
		//表面每个点对应的圆弧的生成
		for (vector<CPoint3D>::iterator it = m_LineArcSurfacePts.begin() + 1; it != m_LineArcSurfacePts.end(); ++it)
		{
			CPtrArray* tempArcArray = new CPtrArray;
			CArc* pArc = new CArc;
			tempArcArray->Add(pArc);

			pArc->m_center = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center;
			pArc->m_center.z = it->z;
			pArc->m_radio = fabs(it->x - ((CArc*)GetLineArcShapeArray().GetAt(0))->m_center.x);
			pArc->m_begin_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_begin_angle;
			pArc->m_end_angle = ((CArc*)GetLineArcShapeArray().GetAt(0))->m_end_angle;

			m_LineFinishVSegmentDisArc.push_back(tempArcArray);
		}
		
		//圆弧的离散
		vector<vector<CPoint3D>>().swap(m_LineFinishVSegmentDisArcDisPts);
		for (vector<CPtrArray*>::iterator it = m_LineFinishVSegmentDisArc.begin(); it!= m_LineFinishVSegmentDisArc.end(); ++it)
		{
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (((CArc*)(*it) ->GetAt(i))->flag==2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)
					{
						vector<CPoint3D> m_tempLineArc = CArc::ArcTDMInterpolation(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio,
							((CArc*)(*it)->GetAt(i))->m_begin_angle, ((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, 
							    GetLineFinishHFeedTime(),GetLineFinishHFeedSpeed());
						m_LineFinishVSegmentDisArcDisPts.push_back(m_tempLineArc);
						vector<CPoint3D>().swap(m_tempLineArc);
					}
				}
			}
		}

		//存入精加工点
		vector<CPoint3D>().swap(FinishSimulationPoints);
		BOOL Direction = TRUE;
		for (vector<vector<CPoint3D>>::iterator it = m_LineFinishVSegmentDisArcDisPts.begin(); it != m_LineFinishVSegmentDisArcDisPts.end(); ++it)
		{
			if (Direction)
			{
				for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
				{
					FinishSimulationPoints.push_back(*its);
				}
			}
			else
			{
				for (vector<CPoint3D>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
				{
					FinishSimulationPoints.push_back(*its);
				}
			}
			Direction = !Direction;
		}
		
		


	}
}
void CToolPath::LineFinishRender(COpenGLDC* pDC)//直接用粗精加工点显示即可
{
	/*for (vector<LinePos>::iterator it = m_LineFinishVSegment.begin(); it != m_LineFinishVSegment.end(); ++it)
	{
		pDC->DrawLine(it->begin, it->end);
	}*/

	/*glPointSize(2);
	for (vector<vector<CPoint3D>>::iterator it = m_LineFinishVSegmentDisPts.begin(); it != m_LineFinishVSegmentDisPts.end(); ++it)
	{
		for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
		{
			pDC->DrawPoint(*its);
		}
	}*/

	//for (vector<vector<LinePos>>::iterator it = m_LineFinishVSegmentDisLine.begin(); it != m_LineFinishVSegmentDisLine.end(); ++it)
	//{
	//	BOOL Direction = TRUE;
	//	for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
	//	{
	//		pDC->DrawLine(its->begin, its->end);
	//		if (its == it->end() - 1)
	//		{
	//			//pDC->SetMaterialColor(RGB(0, 0, 255));
	//			if (Direction)
	//			{
	//				pDC->DrawLine(its->end, CPoint3D(its->end.x, its->end.y, GetLineWorkBlankHigh() /*+ 10*/));//高度上伸出来些
	//			}
	//			else
	//			{
	//				pDC->DrawLine(its->begin, CPoint3D(its->begin.x, its->begin.y, GetLineWorkBlankHigh() /*+ 10*/));//高度上伸出来些
	//			}
	//		}
	//		else
	//		{
	//			if (Direction)
	//			{
	//				pDC->DrawLine(its->end, (its + 1)->end);
	//			}
	//			else
	//			{
	//				pDC->DrawLine(its->begin, (its + 1)->begin);
	//			}
	//		}
	//		Direction = !Direction;
	//	}
	//}
	if (!GetLineStyle())
	{
		for (vector<CPoint3D>::iterator it = FinishSimulationPoints.begin(); it != FinishSimulationPoints.end() - 1; ++it)
		{
			pDC->DrawLine(*it, *(it + 1));
		}
	}
	else
	{
		/*for (vector<vector<CPoint3D>>::iterator it = m_LineFinishVSegmentDisArcDisPts.begin(); it!=m_LineFinishVSegmentDisArcDisPts.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end()-1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}*/
		for (vector<CPoint3D>::iterator it = FinishSimulationPoints.begin(); it != FinishSimulationPoints.end() - 1; ++it)
		{
			pDC->DrawLine(*it, *(it + 1));
		}
	}
}

//旋转加工
vector<CPoint3D> CToolPath::LineDiscreteXZ(CPoint3D m_Begin, CPoint3D m_End, float Inter, float F)
{
	LineDotPos.clear();
	vector<CPoint3D> point = CLine::LineTDMInterpolationXZ(m_Begin, m_End, Inter, F);

	Opposite = Reverse(LineDotPos, 100);

	return point;
}

vector<CPoint3D> CToolPath::ArcDiscreteXZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F)
{
	ArcDotPos.clear();

	vector<CPoint3D> point = CArc::ArcTDMInterpolationXZ(center, radio, beginAngle, endAngle, XFarray, index, Inter, F);


	Opposite = Reverse(ArcDotPos, 100);

	return point;
}
vector<CPoint3D> CToolPath::PolyArcDiscreteXZ(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F)
{
	float m_DisBeEnd = sqrt(pow((begin.x - end.x), 2) + pow((begin.y - end.y), 2) + pow((begin.z - end.z), 2));//计算弦长

	CPoint3D m_LineCenter;
	m_LineCenter.x = (begin.x + end.x) / 2;//弦中点坐标
	m_LineCenter.y = (begin.y + end.y) / 2;//弦中点坐标
	m_LineCenter.z = (begin.z + end.z) / 2;//弦中点坐标

	float m_CenterAng = 4 * atan(fabs(TuduAngle));//计算包角

	m_radio = 0.5*m_DisBeEnd / sin(0.5*m_CenterAng);//计算半径


	float h = sqrt(pow(m_radio, 2) - pow(m_DisBeEnd*0.5, 2));//计算交点离弦心的距离

	if (TuduAngle <= 0.0)//其中起点在终点的右边，顺时针凸度值为负*******XY坐标平面
	{
		if (TuduAngle>-1)
		{

			if ((begin.x<end.x) && (begin.z>end.z))//圆弧相对圆心在第一象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x<end.x&&begin.z<end.z)//圆弧相对圆心在第二象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z<end.z)//圆弧相对圆心在第三象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z>end.z)//圆弧相对圆心在第四象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.z == end.z)//两点纵坐标相等的时候
			{
				if (begin.x <= end.x)//起始点在第二象限，终止点在第一象限
				{
					m_center.z = m_LineCenter.z - h;
					m_center.x = m_LineCenter.x;
					
				}
				else//起始点在第四象限，终止点在第三象限
				{
					m_center.z = m_LineCenter.z + h;
					m_center.x = m_LineCenter.x;
					
				}

			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)//起始点在第一象限，终止点在第四象限
				{
					m_center.x = m_LineCenter.x + h;
					m_center.z = m_LineCenter.z;
					
				}
				else//起始点在第三象限，终止点在第二象限
				{
					m_center.x = m_LineCenter.x - h;
					m_center.z = m_LineCenter.z;
					
				}
			}


		}
		else if (TuduAngle == -1)
		{
			m_center.x = m_LineCenter.x;
			m_center.z = m_LineCenter.z;
			
		}
		else
		{
			if ((begin.x<end.x) && (begin.z>end.z))//圆弧起点2，终点4
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x<end.x&&begin.z<end.z)//圆弧起点3，终点1
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z<end.z)//圆弧起点4，终点2
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z>end.z)//圆弧起点1，终点第三
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}

			else if (begin.z == end.z)//两点纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.z = m_LineCenter.z + h;
					m_center.x = m_LineCenter.x;
					
				}
				else
				{
					m_center.z = m_LineCenter.z - h;
					m_center.x = m_LineCenter.x;
					
				}

			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)
				{
					m_center.x = m_LineCenter.x - h;
					m_center.z = m_LineCenter.z;
					
				}
				else
				{
					m_center.x = m_LineCenter.x + h;
					m_center.z = m_LineCenter.z;
					
				}
			}
		}

	}

	else//起点在终点的左边，逆时针
	{
		if (TuduAngle<1)
		{
			if ((begin.x>end.x) && (begin.z<end.z))//圆弧相对圆心在第一象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z>end.z)//圆弧相对圆心在第二象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x<end.x&&begin.z>end.z)//圆弧相对圆心在第三象限
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x<end.x&&begin.z<end.z)//圆弧相对圆心在第四象限
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.z == end.z)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.z = m_LineCenter.z + h;
					m_center.x = m_LineCenter.x;
					
				}
				else
				{
					m_center.z = m_LineCenter.z - h;
					m_center.x = m_LineCenter.x;
					
				}
			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)//起始点在第4象限，终止点在第1象限
				{
					m_center.x = m_LineCenter.x - h;
					m_center.z = m_LineCenter.z;
					
				}
				else//起始点在第2象限，终止点在第3象限
				{
					m_center.x = m_LineCenter.x + h;
					m_center.z = m_LineCenter.z;
					
				}
			}
		}
		else if (TuduAngle == 1)
		{
			m_center.x = m_LineCenter.x;
			m_center.z = m_LineCenter.z;
			
		}
		else
		{
			if ((begin.x<end.x) && (begin.z>end.z))//圆弧起点2，终点4
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x<end.x&&begin.z<end.z)//圆弧起点3，终点1
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z<end.z)//圆弧起点4，终点2
			{
				m_center.x = m_LineCenter.x + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.x>end.x&&begin.z>end.z)//圆弧起点1，终点第三
			{
				m_center.x = m_LineCenter.x - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.x - end.x) / m_DisBeEnd;
				
			}
			else if (begin.z == end.z)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.x <= end.x)
				{
					m_center.z = m_LineCenter.z - h;
					m_center.x = m_LineCenter.x;
					
				}
				else
				{
					m_center.z = m_LineCenter.z + h;
					m_center.x = m_LineCenter.x;
					
				}
			}
			else if (begin.x == end.x)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)
				{
					m_center.x = m_LineCenter.x + h;
					m_center.z = m_LineCenter.z;
					
				}
				else
				{
					m_center.x = m_LineCenter.x - h;
					m_center.z = m_LineCenter.z;
					
				}
			}
		}

	}

	m_center.y = begin.y;

	CVector3D CCXZ(1, 0, 0);
	CVector3D CS(begin.x - m_center.x, begin.z - m_center.z);
	CVector3D CE(end.x - m_center.x, end.z - m_center.z);

	m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
	m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;

	if ((begin.x>m_center.x) && (begin.z>m_center.z))//圆弧起点相对圆心在第一象限
	{
		if ((end.x>m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x<m_center.x&&begin.z>m_center.z)//圆弧起点相对圆心在第二象限
	{
		if ((end.x>m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x<m_center.x&&begin.z<m_center.z)//圆弧起点相对圆心在第三象限
	{
		if ((end.x>m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.x>m_center.x&&begin.z<m_center.z)//圆弧起点相对圆心在第四象限
	{
		if ((end.x>m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else if ((end.x<m_center.x) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCXZ) / (CS.GetLength()*CCXZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCXZ) / (CE.GetLength()*CCXZ.GetLength())) * 360 / PI;
		}
	}


	CPoint3D temp;
	vector<CPoint3D> ArcTDMPos;
	CPoint3D ptBegin;
	CPoint3D ptEnd;

	float m_ptOffsetX;
	float m_ptOffsetZ;

	float m_Juge;

	//float feed = F*Inter / 60;//单位是微米级的
	float feed =0.001* F*Inter / 60;//单位是毫米级的
	float data;

	BOOL Direction;
	if (TuduAngle > 0)//逆时针凸度值为正
		Direction = TRUE;
	else
		Direction = FALSE;


	if (!Direction)//逆时针为TRUE，顺时针为FALSE
	{
		m_ptOffsetX = begin.x - m_center.x;
		m_ptOffsetZ = begin.z - m_center.z;

	}
	else
	{
		m_ptOffsetX = begin.x - m_center.x;
		m_ptOffsetZ = begin.z - m_center.z;
	}

	ArcTDMPos.push_back(begin);

	if (!Direction)//Direcrtion为False时候是顺时针，这里是求顺时针的插补Go2
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetZ;//插补起点Y坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetX >= 0 && m_ptOffsetZ >= 0)
				m_Juge = 1;
			else if (m_ptOffsetX <= 0 && m_ptOffsetZ >= 0)
				m_Juge = 2;
			else if (m_ptOffsetX <= 0 && m_ptOffsetZ <= 0)
				m_Juge = 3;
			else if (m_ptOffsetX >= 0 && m_ptOffsetZ <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetX) <= fabs(m_ptOffsetZ))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y0 - y1;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;


				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化

					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第二种方法B
				{
					float x1;
					float y1;

					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio > y1*y1)
						x1 = -sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(m_radio*m_radio - x1*x1);
					//zy = y0 - y1;
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y0 - y1;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = -sqrt(m_radio*m_radio - y1*y1);
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第二种方法B
				{
					float x1;
					float y1;


					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					//判断下以免出现无穷小的情况
					if (m_radio*m_radio > y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					//x1 = sqrt(fabs(radio*radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
			}

			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.x - end.x, 2) + pow(temp.z - end.z, 2)) > data);
	}
	else
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetZ;//插补起点Z坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetX >= 0 && m_ptOffsetZ >= 0)
				m_Juge = 1;
			else if (m_ptOffsetX <= 0 && m_ptOffsetZ >= 0)
				m_Juge = 2;
			else if (m_ptOffsetX <= 0 && m_ptOffsetZ <= 0)
				m_Juge = 3;
			else if (m_ptOffsetX >= 0 && m_ptOffsetZ <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetX) <= fabs(m_ptOffsetZ))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;


				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第二种方法A
				{
					float x1;
					float y1;

					zx = -0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetZ))//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;


					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + m_center.x;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetZ = y0;

				}
			}
			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.x - end.x, 2) + pow(temp.z - end.z, 2))>data);

	}

	ArcTDMPos.push_back(end);

	for (vector<CPoint3D>::iterator it = ArcTDMPos.begin(); it != ArcTDMPos.end(); ++it)
	{
		it->y =begin.y;
	}

	Opposite = Reverse(ArcTDMPos, 100);
	return ArcTDMPos;
}

BOOL CToolPath::InitRotateRender()
{
	if (m_Rotate_Array.IsEmpty() == 0)//非空才计算
	{

		CVector3D Translation;
		CPoint3D EnterPoint;//圆半径上的大值点

		//进行坐标的一些转换
		for (int i = 0; i != m_Rotate_Array.GetSize(); ++i)
		{
			if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
			{
				CPoint3D temp;
				CPoint3D temp1;
				temp.x = ((CLine*)m_Rotate_Array.GetAt(i))->m_begin.x;
				temp.y = 0;
				temp.z = ((CLine*)m_Rotate_Array.GetAt(i))->m_begin.y;
				((CLine*)m_Rotate_Array.GetAt(i))->m_begin = temp;

				if (i == 1)
				{

					Translation.dx = EnterPoint.x - ((CLine*)m_Rotate_Array.GetAt(i))->m_begin.x;
					Translation.dy = EnterPoint.y - ((CLine*)m_Rotate_Array.GetAt(i))->m_begin.y;
					Translation.dz = EnterPoint.z - ((CLine*)m_Rotate_Array.GetAt(i))->m_begin.z;
				}

				temp1.x = ((CLine*)m_Rotate_Array.GetAt(i))->m_end.x;
				temp1.y = 0;
				temp1.z = ((CLine*)m_Rotate_Array.GetAt(i))->m_end.y;
				((CLine*)m_Rotate_Array.GetAt(i))->m_end = temp1;


			}
			else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)//普通圆弧
				{

					CPoint3D center;
					center.x = ((CArc*)m_Rotate_Array.GetAt(i))->m_center.x;
					center.y = 0;
					center.z = ((CArc*)m_Rotate_Array.GetAt(i))->m_center.y;
					((CArc*)m_Rotate_Array.GetAt(i))->m_center = center;
				}
				else
				{
					CPoint3D temp;
					CPoint3D temp1;
					temp.x = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin.x;
					temp.y = 0;
					temp.z = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin.y;
					((CArc*)m_Rotate_Array.GetAt(i))->m_begin = temp;

					temp1.x = ((CArc*)m_Rotate_Array.GetAt(i))->m_end.x;
					temp1.y = 0;
					temp1.z = ((CArc*)m_Rotate_Array.GetAt(i))->m_end.y;
					((CArc*)m_Rotate_Array.GetAt(i))->m_end = temp1;
				}
			}
			else if (((CCircle*)m_Rotate_Array.GetAt(i))->flag == 3)
			{
				CPoint3D center;
				center.x = ((CCircle*)m_Rotate_Array.GetAt(i))->m_center.x;
				center.z = 0;
				center.y = ((CCircle*)m_Rotate_Array.GetAt(i))->m_center.y;
				((CCircle*)m_Rotate_Array.GetAt(i))->m_center = center;

				EnterPoint.x = center.x + ((CCircle*)m_Rotate_Array.GetAt(i))->m_radio;
				EnterPoint.y = center.y;
				EnterPoint.z = 0;

				m_RotateCenter = center;
			}

		}

		for (int i = 0; i != m_Rotate_Array.GetSize(); ++i)
		{
			if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
			{
				((CLine*)m_Rotate_Array.GetAt(i))->m_begin += Translation;
				((CLine*)m_Rotate_Array.GetAt(i))->m_end += Translation;

			}
			else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)
				{
					((CArc*)m_Rotate_Array.GetAt(i))->m_center += Translation;
				}
				else
				{
					((CArc*)m_Rotate_Array.GetAt(i))->m_begin += Translation;
					((CArc*)m_Rotate_Array.GetAt(i))->m_end += Translation;

				}
			}
			else if (((CCircle*)m_Rotate_Array.GetAt(i))->flag == 3)
			{

			}
		}

		//离散成点
		//在XOZ平面上画直线和圆弧，在XOY平面上画圆
		vector<vector<CPoint3D>>().swap(RotateWorkPiecePoints);

		for (int i = 0; i != m_Rotate_Array.GetSize(); i++)
		{
			if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
			{
				/*vector<CPoint3D> point =LineDiscreteXZ(((CLine*)m_Rotate_Array.GetAt(i))->m_begin, ((CLine*)m_Rotate_Array.GetAt(i))->m_end, 8, 100);*/
				vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_Rotate_Array.GetAt(i))->m_begin, ((CLine*)m_Rotate_Array.GetAt(i))->m_end, m_Rotate_FeedTime_VX, m_Rotate_FeedSpeed_VX);

				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
				{
					m_RotateCenter.z = it->z;
					float radius = sqrt(pow(it->x - m_RotateCenter.x, 2) + pow(it->y - m_RotateCenter.y, 2));
					/*vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, 8,50);*/
					vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, m_Rotate_FeedTime, m_Rotate_FeedSpeed);


					RotateWorkPiecePoints.push_back(Cpoint);
					vector<CPoint3D>().swap(Cpoint);
				}

			}
			else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					
					/*vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_center, ((CArc*)m_Rotate_Array.GetAt(i))->m_radio, ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle, m_Rotate_Array, i, 8, 100);*/
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_center, ((CArc*)m_Rotate_Array.GetAt(i))->m_radio, ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle, m_Rotate_Array, i, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						m_RotateCenter.z = it->z;
						float radius = sqrt(pow(it->x - m_RotateCenter.x, 2) + pow(it->y - m_RotateCenter.y, 2));
						
						/*vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, 8, 50);*/
						vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

						RotateWorkPiecePoints.push_back(Cpoint);
						vector<CPoint3D>().swap(Cpoint);
					}


				}
				else//多线段圆弧********2016.1.1这里多线段的圆弧如果想要显示则需要在toolpath里面对polyarcediscrete进行XOZ平面上的变化*************
				{
					((CArc*)m_Rotate_Array.GetAt(i))->PolyCalculateXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
						((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu);

					if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu>0)//逆时针
					{
						/*vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
							((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, 8, 100);*/
						vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
							((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							m_RotateCenter.z = it->z;
							float radius = sqrt(pow(it->x - m_RotateCenter.x, 2) + pow(it->y - m_RotateCenter.y, 2));
							/*vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, 8, 50);*/
							vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

							RotateWorkPiecePoints.push_back(Cpoint);
							vector<CPoint3D>().swap(Cpoint);
						}


					}
					else//顺时针
					{
						
						/*vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
							((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, 8, 100);*/
						vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
							((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, m_Rotate_FeedTime, m_Rotate_FeedSpeed);


						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							m_RotateCenter.z = it->z;
							float radius = sqrt(pow(it->x - m_RotateCenter.x, 2) + pow(it->y - m_RotateCenter.y, 2));
							/*vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, 8,50);*/
							vector<CPoint3D> Cpoint = CircleDiscrete(m_RotateCenter, radius, *it, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

							RotateWorkPiecePoints.push_back(Cpoint);
							vector<CPoint3D>().swap(Cpoint);
						}


					}

				}
			}
			else if (((CCircle*)m_Rotate_Array.GetAt(i))->flag == 3)
			{
				((CCircle*)m_Rotate_Array.GetAt(i))->GLDrawCircle(((CCircle*)m_Rotate_Array.GetAt(i))->m_center, ((CCircle*)m_Rotate_Array.GetAt(i))->m_radio);
			}

		}

		return TRUE;
	}
	else 
		return FALSE;
}
void CToolPath::RenderRotate(COpenGLDC* pDC)
{
	if (m_Rotate_Array.IsEmpty() == 0)
	{
		if (RotateWorkPiecePoints.size() != 0)
		{
			for (vector<vector<CPoint3D>>::iterator it = RotateWorkPiecePoints.begin(); it != RotateWorkPiecePoints.end(); ++it)
			{
				for (vector<CPoint3D>::iterator its = it->begin(); its != it->end()-1; ++its)
				{
					pDC->DrawLine(*its, *(its + 1));
				}
			}
		}
	}
}
BOOL CToolPath::InitRotateRough()
{
	if (m_Rotate_Array.IsEmpty() == 0)
	{
		vector<CPtrArray*>().swap(m_RotateRoughArrayList);
		vector<vector<vector<CPoint3D>>>().swap(m_RotateRoughPoints);
		for (int j = GetRotateRoughPlies(); j >= 0; --j)
		{
			CPtrArray* NewArray = new CPtrArray;
			for (int i = 0; i != m_Rotate_Array.GetSize(); ++i)
			{
				if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
				{
					CLine* templine = new CLine;
					NewArray->Add(templine);
					templine->flag = 1;
					templine->m_begin = ((CLine*)m_Rotate_Array.GetAt(i))->m_begin;
					templine->m_end = ((CLine*)m_Rotate_Array.GetAt(i))->m_end;
				}
				else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
				{
					CArc* temparc = new CArc;
					NewArray->Add(temparc);
					if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)
					{
						temparc->flag = 2;
						temparc->m_Tudu = 0;
						temparc->m_center = ((CArc*)m_Rotate_Array.GetAt(i))->m_center;
						temparc->m_begin_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_Rotate_Array.GetAt(i))->m_radio;
						temparc->m_begin = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_Rotate_Array.GetAt(i))->m_end;
					}
					else
					{

						temparc->flag = 2;
						temparc->m_Tudu = ((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu;
						temparc->m_begin = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_Rotate_Array.GetAt(i))->m_end;
						temparc->m_begin_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_Rotate_Array.GetAt(i))->m_radio;
						temparc->m_center = ((CArc*)m_Rotate_Array.GetAt(i))->m_center;
					}
				}
				else if (((CCircle*)m_Rotate_Array.GetAt(i))->flag == 3)
				{
					CCircle* tempCircle = new CCircle;
					NewArray->Add(tempCircle);
					tempCircle->flag = 3;
					tempCircle->m_center = ((CCircle*)m_Rotate_Array.GetAt(i))->m_center;
					tempCircle->m_radio = ((CCircle*)m_Rotate_Array.GetAt(i))->m_radio;
				}

			}

			//对XOZ平面上的点进行坐标的变换，使其由内而外地往外扩
			for (int n = 1; n != NewArray->GetSize(); ++n)
			{
				if (((CLine*)NewArray->GetAt(n))->flag == 1)
				{
					((CLine*)NewArray->GetAt(n))->m_begin.x = ((CLine*)NewArray->GetAt(n))->m_begin.x + m_Rotate_RoughCutOfDepth * j + m_Rotate_RoughLeft;//这里得加上粗加工余量值和每步的切深
					((CLine*)NewArray->GetAt(n))->m_end.x = ((CLine*)NewArray->GetAt(n))->m_end.x + m_Rotate_RoughCutOfDepth * j + m_Rotate_RoughLeft;
				}
				else if (((CArc*)NewArray->GetAt(n))->flag == 2)
				{
					if (((CArc*)NewArray->GetAt(n))->m_Tudu == 0)
					{
						((CArc*)NewArray->GetAt(n))->m_center.x = ((CArc*)NewArray->GetAt(n))->m_center.x + m_Rotate_RoughCutOfDepth * j + m_Rotate_RoughLeft;
					}
					else
					{
						((CArc*)NewArray->GetAt(n))->m_begin.x = ((CArc*)NewArray->GetAt(n))->m_begin.x + m_Rotate_RoughCutOfDepth * j + m_Rotate_RoughLeft;
						((CArc*)NewArray->GetAt(n))->m_end.x = ((CArc*)NewArray->GetAt(n))->m_end.x + m_Rotate_RoughCutOfDepth * j + m_Rotate_RoughLeft;
						((CArc*)NewArray->GetAt(n))->PolyCalculateXZ(((CArc*)NewArray->GetAt(n))->m_begin, ((CArc*)NewArray->GetAt(n))->m_end,
							((CArc*)NewArray->GetAt(n))->m_Tudu);
					}
				}
			}


			m_RotateRoughArrayList.push_back(NewArray);

		}
		//对每层进行点的离散并存储起来
		//vector<vector<vector<CPoint3D>>>().swap(m_RotateRoughPoints);
		for (vector<CPtrArray*>::iterator it = m_RotateRoughArrayList.begin(); it != m_RotateRoughArrayList.end(); ++it)
		{
			vector<vector<CPoint3D>> m_OneFacePoints;
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				
				if (((CLine*)(*it)->GetAt(i))->flag == 1)//XOZ直线
				{
					vector<CPoint3D> point = LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end,
						                                      m_Rotate_FeedTime_VX,m_Rotate_FeedSpeed_VX);
					for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
					{
						m_RotateCenter.z = its->z;
						float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));
						vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

						m_OneFacePoints.push_back(m_tempPoints);
						vector<CPoint3D>().swap(m_tempPoints);

					}
				}
				else if (((CArc*)(*it)->GetAt(i))->flag==2)//XOZ圆弧
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
					{
						vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
							((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, m_Rotate_FeedTime_VX, m_Rotate_FeedSpeed_VX);

						for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
						{
							m_RotateCenter.z = its->z;
							float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));

							vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

							m_OneFacePoints.push_back(m_tempPoints);
							vector<CPoint3D>().swap(m_tempPoints);
						}
					}
					else//多线段圆弧
					{
						((CArc*)(*it)->GetAt(i))->PolyCalculateXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,((CArc*)(*it)->GetAt(i))->m_Tudu);
						if (((CArc*)(*it)->GetAt(i))->m_Tudu > 0)//逆时针
						{
							vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, m_Rotate_FeedTime_VX, m_Rotate_FeedSpeed_VX);

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								m_RotateCenter.z = its->z;
								float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));
								vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

								m_OneFacePoints.push_back(m_tempPoints);
								vector<CPoint3D>().swap(m_tempPoints);

							}
						}
						else//顺时针
						{
							vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, m_Rotate_FeedTime_VX, m_Rotate_FeedSpeed_VX);

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								m_RotateCenter.z = its->z;
								float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));
								vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

								m_OneFacePoints.push_back(m_tempPoints);
								vector<CPoint3D>().swap(m_tempPoints);
							}
						}
					}
				}
				
			}
			m_RotateRoughPoints.push_back(m_OneFacePoints);
			vector<vector<CPoint3D>>().swap(m_OneFacePoints);
		}


		return TRUE;
	}

	else
		return FALSE;
}
void CToolPath::RenderRoughRotate(COpenGLDC* pDC)
{
	if (m_RotateRoughPoints.size() != 0)
	{
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateRoughPoints.begin(); it != m_RotateRoughPoints.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}
			}
		}
	}
}

BOOL CToolPath::InitRotateWorkBlank()
{
	//离散比较出X  Z最大值点并求与第二个图素起点的差值，进而可以求出其包围盒
	if (m_Rotate_WorkBlank_Heighth != 0 && m_Rotate_WorkBlank_Longth != 0 && m_Rotate_WorkBlank_Width != 0)
	{
		vector<CPoint3D> m_AllPoints;
		for (int i = 1; i != m_Rotate_Array.GetSize(); ++i)
		{
			if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
			{
				/*vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_Rotate_Array.GetAt(i))->m_begin, ((CLine*)m_Rotate_Array.GetAt(i))->m_end, 8, 100);*/
				vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_Rotate_Array.GetAt(i))->m_begin, ((CLine*)m_Rotate_Array.GetAt(i))->m_end, m_Rotate_FeedTime,m_Rotate_FeedSpeed);
				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
				{
					m_AllPoints.push_back(*it);
				}
			}
			else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)
				{
					/*vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_center, ((CArc*)m_Rotate_Array.GetAt(i))->m_radio, ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle, m_Rotate_Array, i, 8, 100);*/
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_center, ((CArc*)m_Rotate_Array.GetAt(i))->m_radio, ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle, m_Rotate_Array, i, m_Rotate_FeedTime, m_Rotate_FeedSpeed);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						m_AllPoints.push_back(*it);
					}
				}
				else
				{
					((CArc*)m_Rotate_Array.GetAt(i))->PolyCalculateXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
						((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu);

					/*vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
						((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, 8, 100);*/
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Rotate_Array.GetAt(i))->m_begin, ((CArc*)m_Rotate_Array.GetAt(i))->m_end,
						((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu, m_Rotate_FeedTime, m_Rotate_FeedSpeed);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						m_AllPoints.push_back(*it);
					}

				}
			}
		}
		//比较大小，求出X最大值点和Z值最大值,分别存储在X和Z的位置
		CPoint3D m_Compare= m_AllPoints[0];
		for (vector<CPoint3D>::iterator it = m_AllPoints.begin(); it != m_AllPoints.end(); ++it)
		{
			if (m_Compare.x <= it->x)
				m_Compare.x = it->x;
			if (m_Compare.z <= it->z)
				m_Compare.z = it->z;
		}
		float m_WorkBlankRadius = m_Compare.x - ((CCircle*)m_Rotate_Array.GetAt(0))->m_center.x;
		m_RotateRoughWorkBlankCenter.x = ((CCircle*)m_Rotate_Array.GetAt(0))->m_center.x;
		m_RotateRoughWorkBlankCenter.y = ((CCircle*)m_Rotate_Array.GetAt(0))->m_center.y;
		m_RotateRoughWorkBlankCenter.z = m_Compare.z / 2;


		return TRUE;
	}
	else
		return FALSE;
}
void CToolPath::RenderRotateWorkBlank(COpenGLDC* pDC)
{
	//可用pDC中的DrawRectangleFrame来画出矩形
}

string CToolPath::BeInRotateWorkBlank(CPoint3D pt)//不足是无法判断Z方向上的
{
	double Maxx, Maxy, Maxz, Minx, Miny, Minz;
	Maxx = m_RotateRoughWorkBlankCenter.x + m_Rotate_WorkBlank_Longth / 2;
	Minx = m_RotateRoughWorkBlankCenter.x - m_Rotate_WorkBlank_Longth / 2;
	Maxy = m_RotateRoughWorkBlankCenter.y + m_Rotate_WorkBlank_Width / 2;
	Miny = m_RotateRoughWorkBlankCenter.y - m_Rotate_WorkBlank_Width / 2;
	Maxz = m_RotateRoughWorkBlankCenter.z + m_Rotate_WorkBlank_Heighth / 2;
	Minz = m_RotateRoughWorkBlankCenter.z - m_Rotate_WorkBlank_Heighth / 2;

	if (pt.x >= Minx&&pt.x <= Maxx)
	{
		if (pt.y >= Maxy)
		{

			return "T";
		}
		else if (pt.y <= Miny)
		{

			return "B";
		}
		else
		{

			return "In";
		}

	}
	else if (pt.x < Minx)
	{
		if (pt.y >= Maxy)
		{

			return "LT";
		}
		else if (pt.y <= Miny)
		{

			return "LB";
		}
		else
		{

			return "L";
		}
	}
	else
	{
		if (pt.y >= Maxy)
		{

			return "RT";
		}
		else if (pt.y <= Miny)
		{

			return "RB";
		}
		else
		{

			return "R";

		}
	}
}

BOOL CToolPath::InitRotateRoughCut()
{
	if (m_RotateRoughPoints.size() != 0)
	{
		vector<vector<vector<CPoint3D>>>().swap(m_RotateRoughCutPoints);
		InitRotateWorkBlank();//初始化中心点的值
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateRoughPoints.begin(); it != m_RotateRoughPoints.end(); ++it)
		{
			vector<vector<CPoint3D>> m_OneFacePoints;
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				vector<CPoint3D> temppoints;
				float z;//存储高度
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					CPoint3D temp;
					string JUGE;

					z = itss->z;

					JUGE = BeInRotateWorkBlank(*itss);
					temp = *itss;

					if (JUGE == "In")
					{
						temppoints.push_back(*itss);
					}
					else if (JUGE == "L")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x - m_Rotate_WorkBlank_Longth / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "R")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x + m_Rotate_WorkBlank_Longth / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "T")
					{
						temp.y = m_RotateRoughWorkBlankCenter.y + m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "B")
					{
						temp.y = m_RotateRoughWorkBlankCenter.y - m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "LT")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x - m_Rotate_WorkBlank_Longth / 2;
						temp.y = m_RotateRoughWorkBlankCenter.y + m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "LB")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x - m_Rotate_WorkBlank_Longth / 2;
						temp.y = m_RotateRoughWorkBlankCenter.y - m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "RT")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x + m_Rotate_WorkBlank_Longth / 2;
						temp.y = m_RotateRoughWorkBlankCenter.y + m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "RB")
					{
						temp.x = m_RotateRoughWorkBlankCenter.x + m_Rotate_WorkBlank_Longth / 2;
						temp.y = m_RotateRoughWorkBlankCenter.y - m_Rotate_WorkBlank_Width / 2;
						temppoints.push_back(temp);
					}
				}
				
				//每一圈首尾插入抬刀点，这边假设在零点位置
				temppoints.insert(temppoints.begin(), CPoint3D(2000, 500, z));
				temppoints.insert(temppoints.end(), CPoint3D(2000, 500, z));

				m_OneFacePoints.push_back(temppoints);
				vector<CPoint3D>().swap(temppoints);
			}

			m_RotateRoughCutPoints.push_back(m_OneFacePoints);
			vector<vector<CPoint3D>>().swap(m_OneFacePoints);
		}

		//将粗加工的仿真点进行排序
		vector<CPoint3D>().swap(RoughSimulationPoints);
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateRoughCutPoints.begin(); it != m_RotateRoughCutPoints.end(); ++it)
		{
			BOOL Direction = TRUE;
			for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
			{
				if (Direction)
				{
					for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
					{
						RoughSimulationPoints.push_back(*itss);
					}
				}
				else
				{
					for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
					{
						RoughSimulationPoints.push_back(*itss);
					}
				}
				Direction = !Direction;
			}
		}

		return TRUE;
	}
	else
		return FALSE;
}
void CToolPath::RenderRoughCutRotate(COpenGLDC* pDC)
{
	if (m_RotateRoughCutPoints.size() != 0)
	{
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateRoughCutPoints.begin(); it != m_RotateRoughCutPoints.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}

				//用于上下线起始点的连接
				if (its < it->end() - 1)
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
			}
		}
	}
}
BOOL CToolPath::InitRotateFinish()
{
	if (m_Rotate_RoughLeft != 0 && m_Rotate_FinishCutOfDepth != 0)
	{
		if (m_Rotate_Array.IsEmpty()==0)
		{
			vector<CPtrArray*>().swap(m_RotateFinsihArrayList);
			vector<vector<vector<CPoint3D>>>().swap(m_RotateFinishPoints);
			int n = m_Rotate_RoughLeft / m_Rotate_FinishCutOfDepth;
			for (int j = n; j >= 0; --j)
			{
				CPtrArray* NewArray = new CPtrArray;
				for (int i = 0; i != m_Rotate_Array.GetSize(); ++i)
				{
					if (((CLine*)m_Rotate_Array.GetAt(i))->flag == 1)
					{
						CLine* templine = new CLine;
						NewArray->Add(templine);
						templine->flag = 1;
						templine->m_begin = ((CLine*)m_Rotate_Array.GetAt(i))->m_begin;
						templine->m_end = ((CLine*)m_Rotate_Array.GetAt(i))->m_end;
					}
					else if (((CArc*)m_Rotate_Array.GetAt(i))->flag == 2)
					{
						CArc* temparc = new CArc;
						NewArray->Add(temparc);
						if (((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu == 0)
						{
							temparc->flag = 2;
							temparc->m_Tudu = 0;
							temparc->m_center = ((CArc*)m_Rotate_Array.GetAt(i))->m_center;
							temparc->m_begin_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle;
							temparc->m_end_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle;
							temparc->m_radio = ((CArc*)m_Rotate_Array.GetAt(i))->m_radio;
							temparc->m_begin = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin;
							temparc->m_end = ((CArc*)m_Rotate_Array.GetAt(i))->m_end;
						}
						else
						{

							temparc->flag = 2;
							temparc->m_Tudu = ((CArc*)m_Rotate_Array.GetAt(i))->m_Tudu;
							temparc->m_begin = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin;
							temparc->m_end = ((CArc*)m_Rotate_Array.GetAt(i))->m_end;
							temparc->m_begin_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_begin_angle;
							temparc->m_end_angle = ((CArc*)m_Rotate_Array.GetAt(i))->m_end_angle;
							temparc->m_radio = ((CArc*)m_Rotate_Array.GetAt(i))->m_radio;
							temparc->m_center = ((CArc*)m_Rotate_Array.GetAt(i))->m_center;
						}
					}
					else if (((CCircle*)m_Rotate_Array.GetAt(i))->flag == 3)
					{
						CCircle* tempCircle = new CCircle;
						NewArray->Add(tempCircle);
						tempCircle->flag = 3;
						tempCircle->m_center = ((CCircle*)m_Rotate_Array.GetAt(i))->m_center;
						tempCircle->m_radio = ((CCircle*)m_Rotate_Array.GetAt(i))->m_radio;
					}

				}

				//对XOZ平面上的点进行坐标的变换，使其由内而外地往外扩
				for (int i = 1; i != NewArray->GetSize(); ++i)
				{
					if (((CLine*)NewArray->GetAt(i))->flag == 1)
					{
						((CLine*)NewArray->GetAt(i))->m_begin.x = ((CLine*)NewArray->GetAt(i))->m_begin.x + m_Rotate_FinishCutOfDepth * j;//这里得加上每步的切深
						((CLine*)NewArray->GetAt(i))->m_end.x = ((CLine*)NewArray->GetAt(i))->m_end.x + m_Rotate_FinishCutOfDepth * j;
					}
					else if (((CArc*)NewArray->GetAt(i))->flag == 2)
					{
						if (((CArc*)NewArray->GetAt(i))->m_Tudu == 0)
						{
							((CArc*)NewArray->GetAt(i))->m_center.x = ((CArc*)NewArray->GetAt(i))->m_center.x + m_Rotate_FinishCutOfDepth * j;
						}
						else
						{
							((CArc*)NewArray->GetAt(i))->m_begin.x = ((CArc*)NewArray->GetAt(i))->m_begin.x + m_Rotate_FinishCutOfDepth * j;
							((CArc*)NewArray->GetAt(i))->m_end.x = ((CArc*)NewArray->GetAt(i))->m_end.x + m_Rotate_FinishCutOfDepth * j;
							((CArc*)NewArray->GetAt(i))->PolyCalculateXZ(((CArc*)NewArray->GetAt(i))->m_begin, ((CArc*)NewArray->GetAt(i))->m_end,
								((CArc*)NewArray->GetAt(i))->m_Tudu);
						}
					}
				}

				m_RotateFinsihArrayList.push_back(NewArray);
			}

			//对分层后进行点的离散化
			for (vector<CPtrArray*>::iterator it = m_RotateFinsihArrayList.begin(); it != m_RotateFinsihArrayList.end(); ++it)
			{
				vector<vector<CPoint3D>> m_OneFacePoints;
				for (int i = 0; i != (*it)->GetSize(); ++i)
				{
					if (((CLine*)(*it)->GetAt(i))->flag == 1)//直线
					{
						vector<CPoint3D> point = LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end,
							m_Rotate_FeedTime_VX_Finish, m_Rotate_FeedSpeed_VX_Finish);
						for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
						{
							m_RotateCenter.z = its->z;
							float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));
							vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime_Finish, m_Rotate_FeedSpeed_Finish);
							
							//每一圈首尾插入抬刀点，这边假设在（2000，500，圆心高度）位置
							m_tempPoints.insert(m_tempPoints.begin(), CPoint3D(2000, 500, m_RotateCenter.z));
							m_tempPoints.insert(m_tempPoints.end(), CPoint3D(2000, 500, m_RotateCenter.z));


							m_OneFacePoints.push_back(m_tempPoints);
							vector<CPoint3D>().swap(m_tempPoints);

						}
					}
					else if (((CArc*)(*it)->GetAt(i))->flag == 2)
					{
						if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
						{
							vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
								((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, m_Rotate_FeedTime_VX_Finish, m_Rotate_FeedSpeed_VX_Finish);

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								m_RotateCenter.z = its->z;
								float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));

								vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime_Finish, m_Rotate_FeedSpeed_Finish);

								//每一圈首尾插入抬刀点，这边假设在（2000，500，圆心高度）位置
								m_tempPoints.insert(m_tempPoints.begin(), CPoint3D(2000, 500, m_RotateCenter.z));
								m_tempPoints.insert(m_tempPoints.end(), CPoint3D(2000, 500, m_RotateCenter.z));

								m_OneFacePoints.push_back(m_tempPoints);
								vector<CPoint3D>().swap(m_tempPoints);
							}
						}
						else//多线段圆弧
						{
							((CArc*)(*it)->GetAt(i))->PolyCalculateXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end, ((CArc*)(*it)->GetAt(i))->m_Tudu);

							vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, m_Rotate_FeedTime_VX_Finish, m_Rotate_FeedSpeed_VX_Finish);

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								m_RotateCenter.z = its->z;
								float radius = sqrt(pow(its->x - m_RotateCenter.x, 2) + pow(its->y - m_RotateCenter.y, 2));
								vector<CPoint3D> m_tempPoints = CircleDiscrete(m_RotateCenter, radius, *its, m_Rotate_FeedTime_Finish, m_Rotate_FeedSpeed_Finish);

								//每一圈首尾插入抬刀点，这边假设在（2000，500，圆心高度）位置
								m_tempPoints.insert(m_tempPoints.begin(), CPoint3D(2000, 500, m_RotateCenter.z));
								m_tempPoints.insert(m_tempPoints.end(), CPoint3D(2000, 500, m_RotateCenter.z));

								m_OneFacePoints.push_back(m_tempPoints);
								vector<CPoint3D>().swap(m_tempPoints);

							}
						}
					}
				}
				m_RotateFinishPoints.push_back(m_OneFacePoints);
				vector<vector<CPoint3D>>().swap(m_OneFacePoints);
			}

			//对精加工仿真点进行排序
			vector<CPoint3D>().swap(FinishSimulationPoints);
			for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateFinishPoints.begin(); it != m_RotateFinishPoints.end(); ++it)
			{
				BOOL Direction = TRUE;
				for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
				{
					if (Direction)
					{
						for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
						{
							FinishSimulationPoints.push_back(*itss);
						}
					}
					else
					{
						for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						{
							FinishSimulationPoints.push_back(*itss);
						}
					}
					Direction = !Direction;
				}
			}

               return TRUE;

		}
	}
	else
		return FALSE;
}
void CToolPath::RenderFinishRotate(COpenGLDC* pDC)
{
	if (m_RotateFinishPoints.size() != 0)
	{
		for (vector<vector<vector<CPoint3D>>>::iterator it = m_RotateFinishPoints.begin(); it != m_RotateFinishPoints.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}
				//用于上下线起始点的连接
				if (its < it->end() - 1)
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
			}
		}
	}
}
//侧面加工
vector<CPoint3D> CToolPath::OffsetAlgorithm(CPtrArray& XFarray, int mark, double offset/*必须是点到直线的距离*/)//侧边只能是垂直于X轴的直线
{
	vector<CPoint3D> pt;//存储离散后的点
	vector<OffsetPoint> points;//存储每个图素偏移后的起始点和终点
	for (int i = 0; i != mark+1; ++i)
	{
		if (((CLine*)XFarray.GetAt(i))->flag == 1)
		{
			OffsetPoint temp;
			CPoint3D Not;

			if (i == 0)//考虑第一个元素的情况
			{
				CVector3D vec1;
				CVector3D vec2;
				CVector3D Roat1;
				CVector3D Roat2;
				

				if (((CLine*)XFarray.GetAt(mark))->flag == 1)//最后一个元素为直线
				{

					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					vec2 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);
					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset, vec1,
						((CLine*)XFarray.GetAt(mark))->m_begin + Roat2.GetNormal()*offset, vec2);

					temp.begin = Not;

				}

				else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素为圆弧
				{
					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					CPoint3D tempBegin;
					CPoint3D tempEnd;
					tempBegin = ((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset;
					tempEnd = ((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset;


					if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(tempBegin,tempEnd, ((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio+offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin, ((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

						Not = CLine::IntersectOfArc(tempBegin, tempEnd, ((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
					}
					temp.begin = Not;
				}


				if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素为直线
				{
					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					vec2 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);
					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset, vec1, ((CLine*)XFarray.GetAt(1))->m_begin + Roat2.GetNormal()*offset, vec2);
				}
				else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素为圆弧
				{
					if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio+offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin, ((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio+offset, mark, XFarray, i);
					}


				}


				temp.end = Not;



				points.push_back(temp);

			}
			else if (i == mark)//考虑最后一个元素的情况
			{
				
				if (((CLine*)XFarray.GetAt(mark))->flag == 1 && ((CLine*)XFarray.GetAt(0))->flag == 1)//最后一个元素和第一个元素都是直线
				{
					temp.begin = points[mark - 1].end ;
					temp.end = points[0].begin ;
				}
				else if (((CLine*)XFarray.GetAt(mark))->flag == 1 && ((CArc*)XFarray.GetAt(0))->flag == 2)//最后一个元素为直线，第一个元素为圆弧
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;
				}
				
				points.push_back(temp);
			}
			else
			{
				CVector3D vec1;
				vec1 = ((CLine*)XFarray.GetAt(i))->m_end - ((CLine*)XFarray.GetAt(i))->m_begin;
				CVector3D Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
				

				if (((CLine*)XFarray.GetAt(i+1))->flag == 1)//直线的下一个元素是直线
				{
					CVector3D vec2;
					vec2 = ((CLine*)XFarray.GetAt(i + 1))->m_end - ((CLine*)XFarray.GetAt(i + 1))->m_begin;
					CVector3D Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);

					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset, vec1,
						((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat2.GetNormal()*offset, vec2);
					temp.begin = points[i - 1].end;
					temp.end = Not;

					points.push_back(temp);
				}

				else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2) //直线的下一个元素是圆弧
				{
					if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(i+1))->PolyCalculate(((CArc*)XFarray.GetAt(i+1))->m_begin, ((CArc*)XFarray.GetAt(i+1))->m_end, ((CArc*)XFarray.GetAt(i+1))->m_Tudu);

						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
					}
					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);
				}

			}
			

		}
		else if (((CArc*)XFarray.GetAt(i))->flag == 2)
		{
			OffsetPoint temp;
			CPoint3D Not;

			if (((CArc*)XFarray.GetAt(i))->m_Tudu == 0)
			{
				((CArc*)XFarray.GetAt(i))->Calculate(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio, 
					                                  ((CArc*)XFarray.GetAt(i)) ->m_begin_angle, ((CArc*)XFarray.GetAt(i)) ->m_end_angle, XFarray, i);//计算起始点及终止点

				if (i == 0)
				{
					if (((CLine*)XFarray.GetAt(mark))->flag==1 )//最后一个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset, 
							((CLine*)XFarray.GetAt(mark))->m_begin+Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(mark))->m_end+Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset,
								                       ((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio+offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin,
								                                        ((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio+offset, mark, XFarray, i);
						}
					}

					temp.begin = Not;

					if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset,
							((CLine*)XFarray.GetAt(1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio+offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin,
								((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio+offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio+offset, mark, XFarray, i);
						}
					}

					temp.end = Not;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					points.push_back(temp);
				}
				else
				{


					if (((CLine*)XFarray.GetAt(i+1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(i+1))->m_end - ((CLine*)XFarray.GetAt(i+1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);


						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio+offset,
							((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(i + 1))->PolyCalculate(((CArc*)XFarray.GetAt(i + 1))->m_begin,
								((CArc*)XFarray.GetAt(i + 1))->m_end, ((CArc*)XFarray.GetAt(i + 1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
					}
					
					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);

				}
			}
			else//多线段圆弧
			{

				((CArc*)XFarray.GetAt(i))->PolyCalculate(((CArc*)XFarray.GetAt(i))->m_begin, ((CArc*)XFarray.GetAt(i))->m_end,
					                                     ((CArc*)XFarray.GetAt(i))->m_Tudu);


				if (i == 0)
				{
					if (((CLine*)XFarray.GetAt(mark))->flag == 1)//最后一个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(mark))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(mark))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin,
								((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.begin = Not;

					if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin,
								((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.end = Not;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					points.push_back(temp);
				}
				else
				{


					if (((CLine*)XFarray.GetAt(i + 1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(i + 1))->m_end - ((CLine*)XFarray.GetAt(i + 1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);


						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
							((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(i + 1))->PolyCalculate(((CArc*)XFarray.GetAt(i + 1))->m_begin,
								((CArc*)XFarray.GetAt(i + 1))->m_end, ((CArc*)XFarray.GetAt(i + 1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
					}
					
					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);

				}

			}



		}
		
	}

	for (int i = 0; i!= mark + 1; ++i)
	{
		if (((CLine*)XFarray.GetAt(i))->flag == 1)
		{
			/*vector<CPoint3D> discrete = CLine::LineTDMInterpolation(points[i].begin, points[i].end, 8,200);*/

			vector<CPoint3D> discrete = CLine::LineTDMInterpolation(points[i].begin, points[i].end, m_FeedTime, m_FeedSpeed);
			for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
			{
				pt.push_back(*its);
			}
		}
		else if (((CArc*)XFarray.GetAt(i))->flag == 2)//需要重写个圆弧插补的方程或者求取其起始角与终止角
		{
			if (((CArc*)XFarray.GetAt(i))->m_Tudu == 0)
			{
				BOOL Direction = CArc::Calculate(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio, ((CArc*)XFarray.GetAt(i))->m_begin_angle,
					((CArc*)XFarray.GetAt(i))->m_end_angle, XFarray, i);

				vector<CPoint3D> discrete = CArc::ArcTDMInterpolation(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset, Direction,
					points[i].begin, points[i].end, m_FeedTime,m_FeedSpeed);

				for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
				{
					pt.push_back(*its);
				}
			}
			else
			{
				BOOL Direction;
				if (((CArc*)XFarray.GetAt(i))->m_Tudu>0)
				{
					Direction = TRUE;
				}
				else
				{
					Direction = FALSE;
				}

				vector<CPoint3D> discrete = CArc::ArcTDMInterpolation(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset, Direction,
					points[i].begin, points[i].end, m_FeedTime, m_FeedSpeed);
				for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
				{
					pt.push_back(*its);
				}
			}
		}

	}


	return pt;
}
vector<CPoint3D> CToolPath::FinishOffsetAlgorithm(CPtrArray& XFarray, int mark, double offset)//与上面不同的地方主要是水平面的插补的速度和周期能够实现按钮的控制
{
	vector<CPoint3D> pt;//存储离散后的点
	vector<OffsetPoint> points;//存储每个图素偏移后的起始点和终点
	for (int i = 0; i != mark + 1; ++i)
	{
		if (((CLine*)XFarray.GetAt(i))->flag == 1)
		{
			OffsetPoint temp;
			CPoint3D Not;

			if (i == 0)//考虑第一个元素的情况
			{
				CVector3D vec1;
				CVector3D vec2;
				CVector3D Roat1;
				CVector3D Roat2;


				if (((CLine*)XFarray.GetAt(mark))->flag == 1)//最后一个元素为直线
				{

					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					vec2 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);
					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset, vec1,
						((CLine*)XFarray.GetAt(mark))->m_begin + Roat2.GetNormal()*offset, vec2);

					temp.begin = Not;

				}

				else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素为圆弧
				{
					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					CPoint3D tempBegin;
					CPoint3D tempEnd;
					tempBegin = ((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset;
					tempEnd = ((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset;


					if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(tempBegin, tempEnd, ((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin, ((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

						Not = CLine::IntersectOfArc(tempBegin, tempEnd, ((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
					}
					temp.begin = Not;
				}


				if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素为直线
				{
					vec1 = ((CLine*)XFarray.GetAt(0))->m_end - ((CLine*)XFarray.GetAt(0))->m_begin;
					vec2 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
					Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);
					Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);
					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset, vec1, ((CLine*)XFarray.GetAt(1))->m_begin + Roat2.GetNormal()*offset, vec2);
				}
				else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素为圆弧
				{
					if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin, ((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(0))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(0))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
					}


				}


				temp.end = Not;



				points.push_back(temp);

			}
			else if (i == mark)//考虑最后一个元素的情况
			{

				if (((CLine*)XFarray.GetAt(mark))->flag == 1 && ((CLine*)XFarray.GetAt(0))->flag == 1)//最后一个元素和第一个元素都是直线
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;
				}
				else if (((CLine*)XFarray.GetAt(mark))->flag == 1 && ((CArc*)XFarray.GetAt(0))->flag == 2)//最后一个元素为直线，第一个元素为圆弧
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;
				}

				points.push_back(temp);
			}
			else
			{
				CVector3D vec1;
				vec1 = ((CLine*)XFarray.GetAt(i))->m_end - ((CLine*)XFarray.GetAt(i))->m_begin;
				CVector3D Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);


				if (((CLine*)XFarray.GetAt(i + 1))->flag == 1)//直线的下一个元素是直线
				{
					CVector3D vec2;
					vec2 = ((CLine*)XFarray.GetAt(i + 1))->m_end - ((CLine*)XFarray.GetAt(i + 1))->m_begin;
					CVector3D Roat2 = CVector3D(-1 * vec2.dy, vec2.dx, vec2.dz);

					Not = CLine::IntersectOfLine(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset, vec1,
						((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat2.GetNormal()*offset, vec2);
					temp.begin = points[i - 1].end;
					temp.end = Not;

					points.push_back(temp);
				}

				else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2) //直线的下一个元素是圆弧
				{
					if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
					{
						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
					}
					else//多线段圆弧
					{
						((CArc*)XFarray.GetAt(i + 1))->PolyCalculate(((CArc*)XFarray.GetAt(i + 1))->m_begin, ((CArc*)XFarray.GetAt(i + 1))->m_end, ((CArc*)XFarray.GetAt(i + 1))->m_Tudu);

						Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
							((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
					}
					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);
				}

			}


		}
		else if (((CArc*)XFarray.GetAt(i))->flag == 2)
		{
			OffsetPoint temp;
			CPoint3D Not;

			if (((CArc*)XFarray.GetAt(i))->m_Tudu == 0)
			{
				((CArc*)XFarray.GetAt(i))->Calculate(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio,
					((CArc*)XFarray.GetAt(i))->m_begin_angle, ((CArc*)XFarray.GetAt(i))->m_end_angle, XFarray, i);//计算起始点及终止点

				if (i == 0)
				{
					if (((CLine*)XFarray.GetAt(mark))->flag == 1)//最后一个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(mark))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(mark))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin,
								((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.begin = Not;

					if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin,
								((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.end = Not;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					points.push_back(temp);
				}
				else
				{


					if (((CLine*)XFarray.GetAt(i + 1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(i + 1))->m_end - ((CLine*)XFarray.GetAt(i + 1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);


						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
							((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(i + 1))->PolyCalculate(((CArc*)XFarray.GetAt(i + 1))->m_begin,
								((CArc*)XFarray.GetAt(i + 1))->m_end, ((CArc*)XFarray.GetAt(i + 1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);

				}
			}
			else//多线段圆弧
			{

				((CArc*)XFarray.GetAt(i))->PolyCalculate(((CArc*)XFarray.GetAt(i))->m_begin, ((CArc*)XFarray.GetAt(i))->m_end,
					((CArc*)XFarray.GetAt(i))->m_Tudu);


				if (i == 0)
				{
					if (((CLine*)XFarray.GetAt(mark))->flag == 1)//最后一个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(mark))->m_end - ((CLine*)XFarray.GetAt(mark))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(mark))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(mark))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(mark))->flag == 2)//最后一个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(mark))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(mark))->PolyCalculate(((CArc*)XFarray.GetAt(mark))->m_begin,
								((CArc*)XFarray.GetAt(mark))->m_end, ((CArc*)XFarray.GetAt(mark))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(mark))->m_center, ((CArc*)XFarray.GetAt(mark))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.begin = Not;

					if (((CLine*)XFarray.GetAt(1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(1))->m_end - ((CLine*)XFarray.GetAt(1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);

						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
							((CLine*)XFarray.GetAt(1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(1))->PolyCalculate(((CArc*)XFarray.GetAt(1))->m_begin,
								((CArc*)XFarray.GetAt(1))->m_end, ((CArc*)XFarray.GetAt(1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(0))->m_center, ((CArc*)XFarray.GetAt(0))->m_radio + offset,
								((CArc*)XFarray.GetAt(1))->m_center, ((CArc*)XFarray.GetAt(1))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.end = Not;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					points.push_back(temp);
				}
				else
				{


					if (((CLine*)XFarray.GetAt(i + 1))->flag == 1)//第二个元素是直线
					{
						CVector3D vec1;
						CVector3D Roat1;
						vec1 = ((CLine*)XFarray.GetAt(i + 1))->m_end - ((CLine*)XFarray.GetAt(i + 1))->m_begin;
						Roat1 = CVector3D(-1 * vec1.dy, vec1.dx, vec1.dz);


						Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
							((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
					}
					else if (((CArc*)XFarray.GetAt(i + 1))->flag == 2)//第二个元素是圆弧
					{
						if (((CArc*)XFarray.GetAt(i + 1))->m_Tudu == 0)//普通圆弧
						{
							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
						else//多线段圆弧*****需要计算圆心半径********
						{
							((CArc*)XFarray.GetAt(i + 1))->PolyCalculate(((CArc*)XFarray.GetAt(i + 1))->m_begin,
								((CArc*)XFarray.GetAt(i + 1))->m_end, ((CArc*)XFarray.GetAt(i + 1))->m_Tudu);

							Not = CArc::IntersectOfArc(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);
						}
					}

					temp.begin = points[i - 1].end;
					temp.end = Not;
					points.push_back(temp);

				}

			}



		}

	}

	for (int i = 0; i != mark + 1; ++i)
	{
		if (((CLine*)XFarray.GetAt(i))->flag == 1)
		{
			/*vector<CPoint3D> discrete = CLine::LineTDMInterpolation(points[i].begin, points[i].end, 8,200);*/

			vector<CPoint3D> discrete = CLine::LineTDMInterpolation(points[i].begin, points[i].end, m_FeedTime_Finish, m_FeedSpeed_Finish);
			for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
			{
				pt.push_back(*its);
			}
		}
		else if (((CArc*)XFarray.GetAt(i))->flag == 2)//需要重写个圆弧插补的方程或者求取其起始角与终止角
		{
			if (((CArc*)XFarray.GetAt(i))->m_Tudu == 0)
			{
				BOOL Direction = CArc::Calculate(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio, ((CArc*)XFarray.GetAt(i))->m_begin_angle,
					((CArc*)XFarray.GetAt(i))->m_end_angle, XFarray, i);

				vector<CPoint3D> discrete = CArc::ArcTDMInterpolation(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset, Direction,
					points[i].begin, points[i].end, m_FeedTime_Finish, m_FeedSpeed_Finish);

				for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
				{
					pt.push_back(*its);
				}
			}
			else
			{
				BOOL Direction;
				if (((CArc*)XFarray.GetAt(i))->m_Tudu>0)
				{
					Direction = TRUE;
				}
				else
				{
					Direction = FALSE;
				}

				vector<CPoint3D> discrete = CArc::ArcTDMInterpolation(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset, Direction,
					points[i].begin, points[i].end, m_FeedTime_Finish, m_FeedSpeed_Finish);
				for (vector<CPoint3D>::iterator its = discrete.begin(); its != discrete.end(); ++its)
				{
					pt.push_back(*its);
				}
			}
		}

	}


	return pt;
}


void CToolPath::ChangeSideProcess()
{
	if (m_ptArray.GetSize() != 0)//在里面有数据的时候才进行计算
	{
		CVector3D IsSealing;//判断是否闭合
		CVector3D Translation;//用于存储偏移量

		for (int i = 0; i != m_ptArray.GetSize(); ++i)
		{
			if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
			{
				CVector3D temp;
				CPoint3D  tempMax;
				temp.dx = ((CLine*)m_ptArray.GetAt(i))->m_end.x - ((CLine*)m_ptArray.GetAt(i))->m_begin.x;
				temp.dy = ((CLine*)m_ptArray.GetAt(i))->m_end.y - ((CLine*)m_ptArray.GetAt(i))->m_begin.y;
				temp.dz = ((CLine*)m_ptArray.GetAt(i))->m_end.z - ((CLine*)m_ptArray.GetAt(i))->m_begin.z;

				IsSealing += temp;

				if (((CLine*)m_ptArray.GetAt(i))->m_end.x >= ((CLine*)m_ptArray.GetAt(i))->m_begin.x)
					tempMax = ((CLine*)m_ptArray.GetAt(i))->m_end;
				else
					tempMax = ((CLine*)m_ptArray.GetAt(i))->m_begin;

				if (tempMax.x >= XMax.x)
					XMax = tempMax;

			}
			else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
			{
				CVector3D temp;
				CPoint3D  tempMax;
				if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
				{

					((CArc*)m_ptArray.GetAt(i))->Calculate(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle, m_ptArray, i);

					temp.dx = ((CArc*)m_ptArray.GetAt(i))->m_end.x - ((CArc*)m_ptArray.GetAt(i))->m_begin.x;
					temp.dy = ((CArc*)m_ptArray.GetAt(i))->m_end.y - ((CArc*)m_ptArray.GetAt(i))->m_begin.y;
					temp.dz = ((CArc*)m_ptArray.GetAt(i))->m_end.z - ((CArc*)m_ptArray.GetAt(i))->m_begin.z;
				}
				else
				{
					temp.dx = ((CArc*)m_ptArray.GetAt(i))->m_end.x - ((CArc*)m_ptArray.GetAt(i))->m_begin.x;
					temp.dy = ((CArc*)m_ptArray.GetAt(i))->m_end.y - ((CArc*)m_ptArray.GetAt(i))->m_begin.y;
					temp.dz = ((CArc*)m_ptArray.GetAt(i))->m_end.z - ((CArc*)m_ptArray.GetAt(i))->m_begin.z;
				}

				IsSealing += temp;


				if (((CArc*)m_ptArray.GetAt(i))->m_end.x >= ((CArc*)m_ptArray.GetAt(i))->m_begin.x)
					tempMax = ((CArc*)m_ptArray.GetAt(i))->m_end;
				else
					tempMax = ((CArc*)m_ptArray.GetAt(i))->m_begin;

				if (tempMax.x >= XMax.x)
					XMax = tempMax;
			}
			else if (((CCircle*)m_ptArray.GetAt(i))->flag == 3)//圆的话则无须考虑，这里的侧面加工就是底面不是圆形的时候的加工
			{

			}

			if (IsSealing.GetLength() < 1.0)//如果该向量的长度等于0则表示其是闭合的
			{
				mark = i;
				break;//等于零的时候则跳出循环
			}

		}



		for (int number = mark + 1; number != m_ptArray.GetSize(); ++number)
		{
			if (((CLine*)m_ptArray.GetAt(number))->flag == 1)
			{
				CPoint3D temp;
				temp.x = ((CLine*)m_ptArray.GetAt(number))->m_begin.x;
				temp.y = 0;
				temp.z = ((CLine*)m_ptArray.GetAt(number))->m_begin.y;
				((CLine*)m_ptArray.GetAt(number))->m_begin = temp;

				temp.x = ((CLine*)m_ptArray.GetAt(number))->m_end.x;
				temp.y = 0;
				temp.z = ((CLine*)m_ptArray.GetAt(number))->m_end.y;
				((CLine*)m_ptArray.GetAt(number))->m_end = temp;

				if (number == mark + 1)
				{
					Translation.dx = XMax.x - ((CLine*)m_ptArray.GetAt(number))->m_begin.x;
					Translation.dy = XMax.y - ((CLine*)m_ptArray.GetAt(number))->m_begin.y;
					Translation.dz = XMax.z - ((CLine*)m_ptArray.GetAt(number))->m_begin.z;

				}

			}
			else if (((CArc*)m_ptArray.GetAt(number))->flag == 2)
			{
				if (((CArc*)m_ptArray.GetAt(number))->m_Tudu == 0)//普通圆弧
				{
					((CArc*)m_ptArray.GetAt(number))->Calculate(((CArc*)m_ptArray.GetAt(number))->m_center, ((CArc*)m_ptArray.GetAt(number))->m_radio, ((CArc*)m_ptArray.GetAt(number))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(number))->m_end_angle, m_ptArray, number);

					CPoint3D center;
					center.x = ((CArc*)m_ptArray.GetAt(number))->m_center.x;
					center.y = 0;
					center.z = ((CArc*)m_ptArray.GetAt(number))->m_center.y;
					((CArc*)m_ptArray.GetAt(number))->m_center = center;


					if (number == mark + 1)
					{
						Translation.dx = XMax.x - ((CArc*)m_ptArray.GetAt(number))->m_begin.x;
						Translation.dy = XMax.y - ((CArc*)m_ptArray.GetAt(number))->m_begin.y;
						Translation.dz = XMax.z - ((CArc*)m_ptArray.GetAt(number))->m_begin.z;

					}
				}
				else
				{
					CPoint3D temp;
					CPoint3D temp1;
					temp.x = ((CArc*)m_ptArray.GetAt(number))->m_begin.x;
					temp.y = 0;
					temp.z = ((CArc*)m_ptArray.GetAt(number))->m_begin.y;
					((CArc*)m_ptArray.GetAt(number))->m_begin = temp;

					temp1.x = ((CArc*)m_ptArray.GetAt(number))->m_end.x;
					temp1.y = 0;
					temp1.z = ((CArc*)m_ptArray.GetAt(number))->m_end.y;
					((CArc*)m_ptArray.GetAt(number))->m_end = temp1;

					if (number == mark + 1)
					{
						Translation.dx = XMax.x - ((CArc*)m_ptArray.GetAt(number))->m_begin.x;
						Translation.dy = XMax.y - ((CArc*)m_ptArray.GetAt(number))->m_begin.y;
						Translation.dz = XMax.z - ((CArc*)m_ptArray.GetAt(number))->m_begin.z;

					}

				}
			}
			else if (((CCircle*)m_ptArray.GetAt(number))->flag == 3)
			{

			}
		}

		m_Translation = Translation;

		for (int i = mark + 1; i != m_ptArray.GetSize(); ++i)
		{
			if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
			{
				((CLine*)m_ptArray.GetAt(i))->m_begin += Translation;
				((CLine*)m_ptArray.GetAt(i))->m_end += Translation;

			}
			else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
				{
					((CArc*)m_ptArray.GetAt(i))->m_center += Translation;
				}
				else
				{
					((CArc*)m_ptArray.GetAt(i))->m_begin += Translation;
					((CArc*)m_ptArray.GetAt(i))->m_end += Translation;

				}
			}
			else if (((CCircle*)m_ptArray.GetAt(i))->flag == 3)
			{

			}
		}

	}
}

BOOL CToolPath::RenderSideInit()
{
	if (m_ptArray.IsEmpty() == 0)
	{
		vector<vector<CPoint3D>>().swap(SidePoint);
		
		for (int i = mark + 1; i != m_ptArray.GetSize(); ++i)
		{
			if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
			{
				vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end, m_FeedTime_VX, m_FeedSpeed_VX);
				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
				{
					double offset;
					offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
					vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);

					for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
					{
						its->z = it->z;
					}

					SidePoint.push_back(OffsetPoint);
					vector<CPoint3D>().swap(OffsetPoint);
				}
			}
			else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
				{
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle,m_ptArray, i, m_FeedTime_VX, m_FeedSpeed_VX);
					vector<CPoint3D> OffsetPoint;

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						double offset;
						offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
						OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
						for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
						{
							its->z = it->z;

						}

						SidePoint.push_back(OffsetPoint);
						vector<CPoint3D>().swap(OffsetPoint);
					}
				}
				else
				{
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
						((CArc*)m_ptArray.GetAt(i))->m_Tudu, m_FeedTime_VX, m_FeedSpeed_VX);
					vector<CPoint3D> OffsetPoint;

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						double offset;
						offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
						OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
						for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
						{
							its->z = it->z;

						}

						SidePoint.push_back(OffsetPoint);
						vector<CPoint3D>().swap(OffsetPoint);
					}
				}
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

void CToolPath::RenderSide( COpenGLDC* pDC)
{
	if (SidePoint.size() != 0)
	{

		for (vector<vector<CPoint3D>>::iterator it = SidePoint.begin(); it != SidePoint.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end() - 1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}
	}

}
CPoint3D CToolPath::GetCenterPoint()
{
	CPoint3D center;//存储中心点
	if (this->m_ptArray.IsEmpty() == 0)
	{
		double Minx, Miny, Minz, Maxx, Maxy, Maxz,Mine,Maxe;//用来存放XYZ三个方向的最大最小值
		Minx = ((CLine*)this->m_ptArray.GetAt(0))->m_begin.x;
		Miny = ((CLine*)this->m_ptArray.GetAt(0))->m_begin.y;
		Maxx = ((CLine*)this->m_ptArray.GetAt(0))->m_end.x;
		Maxy = ((CLine*)this->m_ptArray.GetAt(0))->m_end.y;


		Minz  =Maxz = 0.0;
		Maxe = ((CLine*)this->m_ptArray.GetAt(mark + 1))->m_end.x;
		Mine = ((CLine*)this->m_ptArray.GetAt(mark + 1))->m_begin.x;

		for (int i = 0; i != this->m_ptArray.GetSize(); ++i)
		{
			if (((CLine*)this->m_ptArray.GetAt(i))->flag == 1)
			{
				if (((CLine*)this->m_ptArray.GetAt(i))->m_begin.z == 0 && ((CLine*)this->m_ptArray.GetAt(i))->m_end.z == 0)
				{
					vector<CPoint3D> point = LineDiscrete(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end,
						                                   m_FeedTime, m_FeedSpeed);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						if (it->x >= Maxx)
							Maxx = it->x;
						if (it->x <= Minx)
							Minx = it->x;
						if (it->y >= Maxy)
							Maxy = it->y;
						if (it->y <= Miny)
							Miny = it->y;

					}
				}
				else
				{
					vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end,
						                                     m_FeedTime_VX, m_FeedSpeed_VX);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						double offset;
						offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
						vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);

						for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
						{
							its->z = it->z;
                        if (its->x >= Maxx)
							Maxx = its->x;
						if (its->x <= Minx)
							Minx = its->x;
						if (its->z >= Maxz)
							Maxz = its->z;
						if (its->z <= Minz)
							Minz = its->z;
						}
						

					}
				}

			}

			else if (((CArc*)this->m_ptArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)this->m_ptArray.GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					if (((CArc*)this->m_ptArray.GetAt(i))->m_center.z == 0.0)
					{
						vector<CPoint3D> point = ArcDiscrete(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, m_ptArray, i, m_FeedTime, m_FeedSpeed);
						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							if (it->x >= Maxx)
								Maxx = it->x;
							if (it->x <= Minx)
								Minx = it->x;
							if (it->y >= Maxy)
								Maxy = it->y;
							if (it->y <= Miny)
								Miny = it->y;
						}
					}
					else
					{
						vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, m_ptArray, i, m_FeedTime_VX, m_FeedSpeed_VX);
						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							double offset;
							offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
							vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
							for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
							{
								its->z = it->z;
								if (its->x >= Maxx)
									Maxx = its->x;
								if (its->x <= Minx)
									Minx = its->x;
								if (its->z >= Maxz)
									Maxz = its->z;
								if (its->z <= Minz)
									Minz = its->z;
							}
						}
					}
				}
				else//多线段圆弧
				{
					if (((CArc*)m_ptArray.GetAt(i))->m_begin.z == 0.0 && ((CArc*)m_ptArray.GetAt(i))->m_end.z == 0.0)
					{
						vector<CPoint3D> point = PolyArcDiscrete(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
							((CArc*)m_ptArray.GetAt(i))->m_Tudu, m_FeedTime, m_FeedSpeed);
						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							if (it->x >= Maxx)
								Maxx = it->x;
							if (it->x <= Minx)
								Minx = it->x;
							if (it->y >= Maxy)
								Maxy = it->y;
							if (it->y <= Miny)
								Miny = it->y;
						}

					}
					else
					{
						vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
							((CArc*)m_ptArray.GetAt(i))->m_Tudu, m_FeedTime_VX, m_FeedSpeed_VX);

						for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
						{
							double offset;
							offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
							vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
							for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end(); ++its)
							{
								its->z = it->z;

								if (its->x >= Maxx)
									Maxx = its->x;
								if (its->x <= Minx)
									Minx = its->x;
								if (its->z >= Maxz)
									Maxz = its->z;
								if (its->z <= Minz)
									Minz = its->z;
							}

						}
					}
				}
			}


		}

		center.x = Minx + (Maxx - Minx) / 2;
		center.y = Miny + (Maxy - Miny) / 2;
		center.z = Minz + (Maxz - Minz) / 2;
		
	}
	m_RectCenter = center;
	return center;
}

string CToolPath::BeInBox(CPoint3D pt)
{
	double Maxx, Maxy, Maxz, Minx, Miny, Minz;
	Maxx = m_RectCenter.x + m_Piece_Long / 2;
	Minx = m_RectCenter.x - m_Piece_Long / 2;
	Maxy = m_RectCenter.y + m_Piece_Width / 2;
	Miny = m_RectCenter.y - m_Piece_Width / 2;
	Maxz = m_RectCenter.z + m_Piece_Heigth / 2;
	Minz = m_RectCenter.z - m_Piece_Heigth / 2;

	if (pt.x >= Minx&&pt.x <= Maxx)
	{
		if (pt.y>=Maxy)
		{

			return "T";
		}
		else if (pt.y <= Miny)
		{

			return "B";
		}
		else
		{

			return "In";
		}

	}
	else if (pt.x < Minx)
	{
		if (pt.y>=Maxy)
		{

			return "LT";
		}
		else if (pt.y <= Miny)
		{

			return "LB";
		}
		else
		{

			return "L";
		}
	}
	else
	{
		if (pt.y >= Maxy)
		{

			return "RT";
		}
		else if (pt.y <= Miny)
		{

			return "RB";
		}
		else
		{

			return "R";

		}
	}
		
}

void CToolPath::RoughInit()
{
	//NewArray.RemoveAll();
	
	if (m_ptArray.IsEmpty() == 0)//非空的情况下
	{
        vector<CPtrArray*>().swap(NewArrayList);//清零
		vector<vector<vector<CPoint3D>>>().swap(ArrayPoint);//清零
		/*这里先用一个局部变量的CPtrArray* NewArray存储了m_ptArray里面的指针，这样就不会改变m_ptArray里面的数据了*/
		/*j!=-1会导致粗加工进刀100mm,切两层时，在生成NC代码时，加工路径不正确。但是粗加工的轨迹层数是正确的。
		j!=0会导致粗加工进刀100mm,切两层时，在生成NC代码时，加工路径正确。但是粗加工的轨迹层数是不正确的。*/
		for (int j = GetRoughPlies(); j !=-1; --j)//进行分层存储
		{
			CPtrArray* NewArray = new CPtrArray;
			for (int i = 0; i != m_ptArray.GetSize(); ++i)
			{
				if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
				{
					CLine* templine = new CLine;
					NewArray->Add(templine);
					templine->flag = 1;
					templine->m_begin = ((CLine*)m_ptArray.GetAt(i))->m_begin;
					templine->m_end = ((CLine*)m_ptArray.GetAt(i))->m_end;
				}
				else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
				{
					CArc* temparc = new CArc;
					NewArray->Add(temparc);
					if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
					{
						temparc->flag = 2;
						temparc->m_Tudu = 0;
						temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
						temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
						temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
					}
					else
					{

						temparc->flag = 2;
						temparc->m_Tudu = ((CArc*)m_ptArray.GetAt(i))->m_Tudu;
						temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
						temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
						temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
					}
				}

			}


			for (int i = mark + 1; i != NewArray->GetSize(); ++i)
			{
				if (((CLine*)NewArray->GetAt(i))->flag == 1)
				{
					((CLine*)NewArray->GetAt(i))->m_begin.x = ((CLine*)NewArray->GetAt(i))->m_begin.x+m_DepthOfCut * j + m_RoughLeft;//这里得加上粗加工余量值
					((CLine*)NewArray->GetAt(i))->m_end.x = ((CLine*)NewArray->GetAt(i))->m_end.x+m_DepthOfCut * j + m_RoughLeft;
				}
				else if (((CArc*)NewArray->GetAt(i))->flag == 2)
				{
					if (((CArc*)NewArray->GetAt(i))->m_Tudu == 0)
					{
						((CArc*)NewArray->GetAt(i))->m_center.x = ((CArc*)NewArray->GetAt(i))->m_center.x+ m_DepthOfCut* j + m_RoughLeft;
					}
					else
					{
						((CArc*)NewArray->GetAt(i))->m_begin.x = ((CArc*)NewArray->GetAt(i))->m_begin.x+ m_DepthOfCut* j + m_RoughLeft;
						((CArc*)NewArray->GetAt(i))->m_end.x = ((CArc*)NewArray->GetAt(i))->m_end.x+ m_DepthOfCut * j + m_RoughLeft;
						((CArc*)NewArray->GetAt(i))->PolyCalculateXZ(((CArc*)NewArray->GetAt(i))->m_begin, ((CArc*)NewArray->GetAt(i))->m_end,
							((CArc*)NewArray->GetAt(i))->m_Tudu);
					}
				}
			}


			NewArrayList.push_back(NewArray);
			//NewArray->RemoveAll();
			//delete NewArray;
		}

		
		//需要将轮廓进行离散化并且存储起来
		for (vector<CPtrArray*>::iterator it = NewArrayList.begin(); it != NewArrayList.end(); ++it)
		{
			vector<vector<CPoint3D>> OneFacePt;
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				
				if (((CLine*)(*it)->GetAt(i))->flag == 1)
				{
					if (i>mark)
					{
					vector<CPoint3D> point = LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end, m_FeedTime_VX, m_FeedSpeed_VX);
					vector<CPoint3D> OffsetPoint;


					for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
					{
					    double offset;
					    offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
					    OffsetPoint = OffsetAlgorithm(*(*it), mark, offset);

						////在最前面/后面插入个抬刀点
						//OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
						//OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

					    for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end(); ++itss)
					    {
					       itss->z = its->z;

					     }

						OneFacePt.push_back(OffsetPoint);
						vector<CPoint3D>().swap(OffsetPoint);
					 }

					
				  }
					
				}
				else if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
					{
						if (((CArc*)(*it)->GetAt(i))->m_center.z != 0)//XZ平面
						{
							vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
								((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, m_FeedTime_VX, m_FeedSpeed_VX);
							vector<CPoint3D> OffsetPoint;

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								double offset;
								offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
								OffsetPoint = OffsetAlgorithm(**it, mark, offset);

								////在最前面/后面插入个抬刀点
								//OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
								//OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

								for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end() ; ++itss)
								{
									itss->z = its->z;
		
								}

								OneFacePt.push_back(OffsetPoint);
								vector<CPoint3D>().swap(OffsetPoint);
							}

						}
					}
					else//多线段圆弧
					{
						if (((CArc*)(*it)->GetAt(i))->m_begin.z == 0 && ((CArc*)(*it)->GetAt(i))->m_end.z == 0)//XY平面
						{
						
						}
						else//XZ平面
						{
							vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, m_FeedTime_VX, m_FeedSpeed_VX);
							vector<CPoint3D> OffsetPoint;

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								double offset;
								offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
								OffsetPoint = OffsetAlgorithm(**it, mark, offset);

								////在最前面/后面插入个抬刀点
								//OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
								//OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

								for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end(); ++itss)
								{
									itss->z = its->z;

								}

								OneFacePt.push_back(OffsetPoint);
								vector<CPoint3D>().swap(OffsetPoint);
							}



						}
					}
				}

				
			}

			
                ArrayPoint.push_back(OneFacePt);
				vector<vector<CPoint3D>>().swap(OneFacePt);

		}

	


	}
}
void CToolPath::RenderSideRough(COpenGLDC* pDC)
{
	/*粗加工轨迹可以先从内向外按照进刀深度进行扩，底面图形按照进刀深度进行扩，而侧面上的图形保持不变。
	对离散后的点进行判断，如果其在毛坯的矩形范围内，则走插补的路径，如果在矩形外面则走直线，减少空行程。*/

	//可行
	if (ArrayPoint.size() != 0)
	{

		for (vector<vector<vector<CPoint3D>>>::iterator it = ArrayPoint.begin(); it != ArrayPoint.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end() - 1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}
			}
		}
	}
}

void CToolPath::DeletePointOutOfBox()
{
	if (ArrayPoint.size() != 0)
	{
		vector<vector<vector<CPoint3D>>>().swap(ADArrayPoint);

		GetCenterPoint();
		for (vector<vector<vector<CPoint3D>>>::iterator it = ArrayPoint.begin(); it != ArrayPoint.end(); ++it)
		{
			vector<vector<CPoint3D>> pts;
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				vector<CPoint3D> temppoints;

				float z;//设置个点去获取纵坐标

				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					z = itss->z;//获取纵坐标

					CPoint3D temp;
					string JUGE;
					JUGE = BeInBox(*itss);
					temp = *itss;

					if (JUGE=="In")
					{
						temppoints.push_back(*itss);
					}
					else if (JUGE =="L")
					{
						temp.x = m_RectCenter.x - m_Piece_Long / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "R")
					{
						temp.x = m_RectCenter.x + m_Piece_Long / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "T")
					{
						temp.y = m_RectCenter.y + m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "B")
					{
						temp.y = m_RectCenter.y - m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "LT")
					{
						temp.x = m_RectCenter.x - m_Piece_Long / 2;
						temp.y = m_RectCenter.y + m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "LB")
					{
						temp.x = m_RectCenter.x - m_Piece_Long / 2;
						temp.y = m_RectCenter.y - m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "RT")
					{
						temp.x = m_RectCenter.x + m_Piece_Long / 2;
						temp.y = m_RectCenter.y + m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
					else if (JUGE == "RB")
					{
						temp.x = m_RectCenter.x + m_Piece_Long / 2;
						temp.y = m_RectCenter.y - m_Piece_Width / 2;
						temppoints.push_back(temp);
					}
				}

				//在首尾插入抬刀点
				temppoints.insert(temppoints.begin(), CPoint3D(0, 0, z));
				temppoints.insert(temppoints.end(), CPoint3D(0, 0, z));
				
				pts.push_back(temppoints);
				vector<CPoint3D>().swap(temppoints);
			}

			ADArrayPoint.push_back(pts);
			vector<vector<CPoint3D>>().swap(pts);
		}

		vector<CPoint3D>().swap(RoughSimulationPoints);
		for (vector<vector<vector<CPoint3D>>>::iterator it = ADArrayPoint.begin(); it != ADArrayPoint.end(); ++it)
		{
			BOOL Direction = TRUE;
			for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
			{
				if (Direction)//正序输出
				{
					for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
						RoughSimulationPoints.push_back(*itss);
				}
				else//对点进行倒叙输出要注意的是倒叙迭代器reverse_iterator
				{
					for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						RoughSimulationPoints.push_back(*itss);

				}
				Direction = !Direction;
			}
		}


	}
}

void CToolPath::RenderSideRoughCut(COpenGLDC* pDC)
{
	if (ADArrayPoint.size() != 0)
	{
		for (vector<vector<vector<CPoint3D>>>::iterator it = ADArrayPoint.begin(); it != ADArrayPoint.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}

				//用于上下线起始点的连接
				if (its < it->end() - 1)
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));

			}
		}
	}
}

void CToolPath::InitSideRoughLeft()
{
     if (m_ptArray.IsEmpty() == 0)//只要有初始的图形数据就可以进行粗加工余量的生成
	  {
		 vector<vector<CPoint3D>>().swap(RoughLeftPoints);
		 CPtrArray* tempArray = new CPtrArray;

		 for (int i = 0; i != m_ptArray.GetSize(); ++i)
		 {
			 if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
			 {
				 CLine* templine = new CLine;
				 tempArray->Add(templine);
				 templine->flag = 1;
				 templine->m_begin = ((CLine*)m_ptArray.GetAt(i))->m_begin;
				 templine->m_end = ((CLine*)m_ptArray.GetAt(i))->m_end;
			 }
			 else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
			 {
				 CArc* temparc = new CArc;
				 tempArray->Add(temparc);
				 if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
				 {
					 temparc->flag = 2;
					 temparc->m_Tudu = 0;
					 temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
					 temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
					 temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
					 temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
					 temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
					 temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
				 }
				 else
				 {

					 temparc->flag = 2;
					 temparc->m_Tudu = ((CArc*)m_ptArray.GetAt(i))->m_Tudu;
					 temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
					 temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
					 temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
					 temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
					 temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
					 temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
				 }
			 }
		 }

		 for (int i = mark + 1; i != tempArray->GetSize(); ++i)
		 {
			 if (((CLine*)tempArray->GetAt(i))->flag == 1)
			 {
				 ((CLine*)tempArray->GetAt(i))->m_begin.x += m_RoughLeft;
				 ((CLine*)tempArray->GetAt(i))->m_end.x += m_RoughLeft;
			 }
			 else if (((CArc*)tempArray->GetAt(i))->flag == 2)
			 {
				 if (((CArc*)tempArray->GetAt(i))->m_Tudu == 0)
				 {
					 ((CArc*)tempArray->GetAt(i))->m_center.x +=m_RoughLeft;
				 }
				 else
				 {
					 ((CArc*)tempArray->GetAt(i))->m_begin.x += m_RoughLeft;
					 ((CArc*)tempArray->GetAt(i))->m_end.x += m_RoughLeft;
					 ((CArc*)tempArray->GetAt(i))->PolyCalculateXZ(((CArc*)tempArray->GetAt(i))->m_begin, ((CArc*)tempArray->GetAt(i))->m_end,
						 ((CArc*)tempArray->GetAt(i))->m_Tudu);
				 }
			 }
		 }

		 for (int i = 0; i != tempArray->GetSize(); ++i)
		 {
			 vector<CPoint3D> tempPoints;
			 if (((CLine*)tempArray->GetAt(i))->flag == 1)
			 {
				 if (i > mark)
				 {
					 vector<CPoint3D> point = LineDiscreteXZ(((CLine*)tempArray->GetAt(i))->m_begin, ((CLine*)tempArray->GetAt(i))->m_end,
						                                     m_FeedTime_VX, m_FeedSpeed_VX);

					 for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
					 {
						 double offset;
						 offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
						 tempPoints = OffsetAlgorithm(*tempArray, mark, offset);

						 for (vector<CPoint3D>::iterator itss = tempPoints.begin(); itss !=tempPoints.end(); ++itss)
						 {
							 itss->z = its->z;

						 }

						 RoughLeftPoints.push_back(tempPoints);
					 }


				 }
			 }
			 else if (((CArc*)tempArray->GetAt(i))->flag == 2)
			 {
				 if (((CArc*)tempArray->GetAt(i))->m_Tudu == 0)
				 {
					 if (i > mark)
					 {
							 vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)tempArray->GetAt(i))->m_center, ((CArc*)tempArray->GetAt(i))->m_radio, ((CArc*)tempArray->GetAt(i))->m_begin_angle,
								 ((CArc*)tempArray->GetAt(i))->m_end_angle, *tempArray, i, m_FeedTime_VX, m_FeedSpeed_VX);

							 for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							 {
								 double offset;
								 offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
								 tempPoints = OffsetAlgorithm(*tempArray, mark, offset);
								 for (vector<CPoint3D>::iterator itss = tempPoints.begin(); itss != tempPoints.end(); ++itss)
								 {
									 itss->z = its->z;

								 }

								 RoughLeftPoints.push_back(tempPoints);

						     }
					 }

				 }
				 else
				 {
					 if (i > mark)
					 {
						 vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)tempArray->GetAt(i))->m_begin, ((CArc*)tempArray->GetAt(i))->m_end,
							 ((CArc*)tempArray->GetAt(i))->m_Tudu, m_FeedTime_VX, m_FeedSpeed_VX);

						 for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
						 {
							 double offset;
							 offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
							 tempPoints = OffsetAlgorithm(*tempArray, mark, offset);
							 for (vector<CPoint3D>::iterator itss = tempPoints.begin(); itss != tempPoints.end(); ++itss)
							 {
								 itss->z = its->z;

							 }

							 RoughLeftPoints.push_back(tempPoints);
						 }

					  }

				  }
			 }

			 
			 vector<CPoint3D>().swap(tempPoints);
		 }

		
	  }
}

void CToolPath::RenderSideRoughLeft(COpenGLDC* pDC)
{
	if (RoughLeftPoints.size() != 0)
	{

		for (vector<vector<CPoint3D>>::iterator it = RoughLeftPoints.begin(); it != RoughLeftPoints.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end() - 1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}
	}
}

void CToolPath::InitSideFinish()//精加工初始化
{
	if (m_RoughLeft!=0&&m_DepthOfCut_Finish!=0)//粗加工余量里面的点不为空
	{
		vector<CPtrArray*>().swap(FinishArrayList);
		vector<vector<vector<CPoint3D>>>().swap(FinishPoints);
		int n = m_RoughLeft / m_DepthOfCut_Finish;

		for (int j =n; j>= 0; --j)//设置方向
		{
			CPtrArray* NewArray = new  CPtrArray;
			for (int i = 0; i != m_ptArray.GetSize(); ++i)
			{

				if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
				{
					CLine* templine = new CLine;
					NewArray->Add(templine);
					templine->flag = 1;
					templine->m_begin = ((CLine*)m_ptArray.GetAt(i))->m_begin;
					templine->m_end = ((CLine*)m_ptArray.GetAt(i))->m_end;
				}
				else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
				{
					CArc* temparc = new CArc;
					NewArray->Add(temparc);
					if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)
					{
						temparc->flag = 2;
						temparc->m_Tudu = 0;
						temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
						temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
						temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
					}
					else
					{

						temparc->flag = 2;
						temparc->m_Tudu = ((CArc*)m_ptArray.GetAt(i))->m_Tudu;
						temparc->m_begin = ((CArc*)m_ptArray.GetAt(i))->m_begin;
						temparc->m_end = ((CArc*)m_ptArray.GetAt(i))->m_end;
						temparc->m_begin_angle = ((CArc*)m_ptArray.GetAt(i))->m_begin_angle;
						temparc->m_end_angle = ((CArc*)m_ptArray.GetAt(i))->m_end_angle;
						temparc->m_radio = ((CArc*)m_ptArray.GetAt(i))->m_radio;
						temparc->m_center = ((CArc*)m_ptArray.GetAt(i))->m_center;
					}
				}

			}

			for (int i = mark + 1; i != NewArray->GetSize(); ++i)
			{
				if (((CLine*)NewArray->GetAt(i))->flag == 1)
				{
					((CLine*)NewArray->GetAt(i))->m_begin.x = ((CLine*)NewArray->GetAt(i))->m_begin.x + m_DepthOfCut_Finish * j ;
					((CLine*)NewArray->GetAt(i))->m_end.x = ((CLine*)NewArray->GetAt(i))->m_end.x + m_DepthOfCut_Finish * j ;
				}
				else if (((CArc*)NewArray->GetAt(i))->flag == 2)
				{
					if (((CArc*)NewArray->GetAt(i))->m_Tudu == 0)
					{
						((CArc*)NewArray->GetAt(i))->m_center.x = ((CArc*)NewArray->GetAt(i))->m_center.x + m_DepthOfCut_Finish* j ;
					}
					else
					{
						((CArc*)NewArray->GetAt(i))->m_begin.x = ((CArc*)NewArray->GetAt(i))->m_begin.x + m_DepthOfCut_Finish* j ;
						((CArc*)NewArray->GetAt(i))->m_end.x = ((CArc*)NewArray->GetAt(i))->m_end.x + m_DepthOfCut_Finish * j;
						((CArc*)NewArray->GetAt(i))->PolyCalculateXZ(((CArc*)NewArray->GetAt(i))->m_begin, ((CArc*)NewArray->GetAt(i))->m_end,
							((CArc*)NewArray->GetAt(i))->m_Tudu);
					}
				}
			}


			FinishArrayList.push_back(NewArray);
			//NewArray->RemoveAll();
		}

		for (vector<CPtrArray*>::iterator it = FinishArrayList.begin(); it != FinishArrayList.end(); ++it)
		{
			vector<vector<CPoint3D>> OneFacePt;
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{

				if (((CLine*)(*it)->GetAt(i))->flag == 1)
				{
					if (i>mark)
					{
						vector<CPoint3D> point = LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end, m_FeedTime_VX_Finish, m_FeedSpeed_VX_Finish);
						vector<CPoint3D> OffsetPoint;

						for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
						{
							double offset;
							offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
							OffsetPoint = FinishOffsetAlgorithm(*(*it), mark, offset);

							//在最前面/后面插入个抬刀点
							OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
							OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

							for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end(); ++itss)
							{
								itss->z = its->z;

							}

							OneFacePt.push_back(OffsetPoint);
							vector<CPoint3D>().swap(OffsetPoint);
						}


					}

				}
				else if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
					{
						if (((CArc*)(*it)->GetAt(i))->m_center.z != 0)//XZ平面
						{
							vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
								((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, m_FeedTime_VX_Finish, m_FeedSpeed_VX_Finish);
							vector<CPoint3D> OffsetPoint;

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								double offset;
								offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
								OffsetPoint = FinishOffsetAlgorithm(**it, mark, offset);

								//在最前面/后面插入个抬刀点
								OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
								OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

								for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end(); ++itss)
								{
									itss->z = its->z;

								}

								OneFacePt.push_back(OffsetPoint);
								vector<CPoint3D>().swap(OffsetPoint);
							}

						}
					}
					else//多线段圆弧
					{
						if (((CArc*)(*it)->GetAt(i))->m_begin.z == 0 && ((CArc*)(*it)->GetAt(i))->m_end.z == 0)//XY平面
						{

						}
						else//XZ平面
						{
							vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, m_FeedTime_VX_Finish, m_FeedSpeed_VX_Finish);
							vector<CPoint3D> OffsetPoint;

							for (vector<CPoint3D>::iterator its = point.begin(); its != point.end(); ++its)
							{
								double offset;
								offset = sqrt(pow(its->x - XMax.x, 2) + pow(its->y - XMax.y, 2));
								OffsetPoint = FinishOffsetAlgorithm(**it, mark, offset);

								//在最前面/后面插入个抬刀点
								OffsetPoint.insert(OffsetPoint.begin(), CPoint3D(0, 0, 0));
								OffsetPoint.insert(OffsetPoint.end(), CPoint3D(0, 0, 0));

								for (vector<CPoint3D>::iterator itss = OffsetPoint.begin(); itss != OffsetPoint.end(); ++itss)
								{
									itss->z = its->z;

								}

								OneFacePt.push_back(OffsetPoint);
								vector<CPoint3D>().swap(OffsetPoint);
							}



						}
					}
				}


			}
			FinishPoints.push_back(OneFacePt);
			vector<vector<CPoint3D>>().swap(OneFacePt);

		}

		vector<CPoint3D>().swap(FinishSimulationPoints);
		for (vector<vector<vector<CPoint3D>>>::iterator it =FinishPoints.begin(); it != FinishPoints.end(); ++it)//正确的层数顺序，由外向内
		{
			BOOL Direction = TRUE;
			for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)//需要倒叙才能实现由上到下运动
			{

				//需要改变方向才能实现方向的变化

				if (Direction)//正序输出
				{
					for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)//
						FinishSimulationPoints.push_back(*itss);
				}
				else//对点进行倒叙输出要注意的是倒叙迭代器reverse_iterator
				{
					for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						FinishSimulationPoints.push_back(*itss);

				}
				Direction = !Direction;
			}
		}

	}
}

void CToolPath::RenderSideFinish(COpenGLDC* pDC)
{
	if (FinishPoints.size() != 0)
	{

		for (vector<vector<vector<CPoint3D>>>::iterator it = FinishPoints.begin(); it != FinishPoints.end(); ++it)
		{
			for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end() - 1; ++itss)
				{
					pDC->DrawLine(*itss, *(itss + 1));
				}

				//用于上下线起始点的连接
				if (its < it->end() - 1)
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
			}
		}
	}
}



//圆弧曲面加工
vector<CPoint3D> CToolPath::ArcDiscreteYZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F)
{
	ArcDotPos.clear();

	vector<CPoint3D> point = CArc::ArcTDMInterpolationYZ(center, radio, beginAngle, endAngle, XFarray, index, Inter, F);

	return point;
}
vector<CPoint3D> CToolPath::PolyArcDiscreteYZ(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F)
{
	float m_DisBeEnd = sqrt(pow((begin.x - end.x), 2) + pow((begin.y - end.y), 2) + pow((begin.z - end.z), 2));//计算弦长

	CPoint3D m_LineCenter;
	m_LineCenter.x = (begin.x + end.x) / 2;//弦中点坐标
	m_LineCenter.y = (begin.y + end.y) / 2;//弦中点坐标
	m_LineCenter.z = (begin.z + end.z) / 2;//弦中点坐标

	float m_CenterAng = 4 * atan(fabs(TuduAngle));//计算包角

	m_radio = 0.5*m_DisBeEnd / sin(0.5*m_CenterAng);//计算半径


	float h = sqrt(pow(m_radio, 2) - pow(m_DisBeEnd*0.5, 2));//计算交点离弦心的距离

	if (TuduAngle <= 0.0)//其中起点在终点的右边，顺时针凸度值为负*******YZ坐标平面
	{
		if (TuduAngle>-1)
		{

			if ((begin.y<end.y) && (begin.z>end.z))//圆弧相对圆心在第一象限
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y<end.y&&begin.z<end.z)//圆弧相对圆心在第二象限
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z<end.z)//圆弧相对圆心在第三象限
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z>end.z)//圆弧相对圆心在第四象限
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.z == end.z)//两点纵坐标相等的时候
			{
				if (begin.y <= end.y)//起始点在第二象限，终止点在第一象限
				{
					m_center.z = m_LineCenter.z - h;
					m_center.y = m_LineCenter.y;

				}
				else//起始点在第四象限，终止点在第三象限
				{
					m_center.z = m_LineCenter.z + h;
					m_center.y = m_LineCenter.y;

				}

			}
			else if (begin.y == end.y)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)//起始点在第一象限，终止点在第四象限
				{
					m_center.y = m_LineCenter.y + h;
					m_center.z = m_LineCenter.z;

				}
				else//起始点在第三象限，终止点在第二象限
				{
					m_center.y = m_LineCenter.y - h;
					m_center.z = m_LineCenter.z;

				}
			}


		}
		else if (TuduAngle == -1)
		{
			m_center.y = m_LineCenter.y;
			m_center.z = m_LineCenter.z;

		}
		else
		{
			if ((begin.y<end.y) && (begin.z>end.z))//圆弧起点2，终点4
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y<end.y&&begin.z<end.z)//圆弧起点3，终点1
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z<end.z)//圆弧起点4，终点2
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z>end.z)//圆弧起点1，终点第三
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}

			else if (begin.z == end.z)//两点纵坐标相等的时候
			{
				if (begin.y <= end.y)
				{
					m_center.z = m_LineCenter.z + h;
					m_center.y = m_LineCenter.y;

				}
				else
				{
					m_center.z = m_LineCenter.z - h;
					m_center.y = m_LineCenter.y;

				}

			}
			else if (begin.y == end.y)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)
				{
					m_center.y = m_LineCenter.y - h;
					m_center.z = m_LineCenter.z;

				}
				else
				{
					m_center.y = m_LineCenter.y + h;
					m_center.z = m_LineCenter.z;

				}
			}
		}

	}

	else//起点在终点的左边，逆时针
	{
		if (TuduAngle<1)
		{
			if ((begin.y>end.y) && (begin.z<end.z))//圆弧相对圆心在第一象限
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z>end.z)//圆弧相对圆心在第二象限
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y<end.y&&begin.z>end.z)//圆弧相对圆心在第三象限
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y<end.y&&begin.z<end.z)//圆弧相对圆心在第四象限
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.z == end.z)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.y <= end.y)
				{
					m_center.z = m_LineCenter.z + h;
					m_center.y = m_LineCenter.y;

				}
				else
				{
					m_center.z = m_LineCenter.z - h;
					m_center.y = m_LineCenter.y;

				}
			}
			else if (begin.y == end.y)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)//起始点在第4象限，终止点在第1象限
				{
					m_center.y = m_LineCenter.y - h;
					m_center.z = m_LineCenter.z;

				}
				else//起始点在第2象限，终止点在第3象限
				{
					m_center.y = m_LineCenter.y + h;
					m_center.z = m_LineCenter.z;

				}
			}
		}
		else if (TuduAngle == 1)
		{
			m_center.y = m_LineCenter.y;
			m_center.z = m_LineCenter.z;

		}
		else
		{
			if ((begin.y<end.y) && (begin.z>end.z))//圆弧起点2，终点4
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y<end.y&&begin.z<end.z)//圆弧起点3，终点1
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z - h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z<end.z)//圆弧起点4，终点2
			{
				m_center.y = m_LineCenter.y + h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.y>end.y&&begin.z>end.z)//圆弧起点1，终点第三
			{
				m_center.y = m_LineCenter.y - h*abs(begin.z - end.z) / m_DisBeEnd;
				m_center.z = m_LineCenter.z + h*abs(begin.y - end.y) / m_DisBeEnd;

			}
			else if (begin.z == end.z)//起始点和终止点的纵坐标相等的时候
			{
				if (begin.y <= end.y)
				{
					m_center.z = m_LineCenter.z - h;
					m_center.y = m_LineCenter.y;

				}
				else
				{
					m_center.z = m_LineCenter.z + h;
					m_center.y = m_LineCenter.y;

				}
			}
			else if (begin.y == end.y)//起始点和终止点的横坐标相等的时候
			{
				if (begin.z <= end.z)
				{
					m_center.y = m_LineCenter.y + h;
					m_center.z = m_LineCenter.z;

				}
				else
				{
					m_center.y = m_LineCenter.y - h;
					m_center.z = m_LineCenter.z;

				}
			}
		}

	}

	m_center.x = begin.x;

	CVector3D CCYZ(0, 1, 0);
	CVector3D CS(0,begin.y - m_center.y, begin.z - m_center.z);
	CVector3D CE(0,end.y - m_center.y, end.z - m_center.z);

	m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
	m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;

	if ((begin.y>m_center.y) && (begin.z>m_center.z))//圆弧起点相对圆心在第一象限
	{
		if ((end.y>m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.y<m_center.y&&begin.z>m_center.z)//圆弧起点相对圆心在第二象限
	{
		if ((end.y>m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.y<m_center.y&&begin.z<m_center.z)//圆弧起点相对圆心在第三象限
	{
		if ((end.y>m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
	}
	else if (begin.y>m_center.y&&begin.z<m_center.z)//圆弧起点相对圆心在第四象限
	{
		if ((end.y>m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第一象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z>m_center.z))//圆弧终点相对圆心在第2象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else if ((end.y<m_center.y) && (end.z<m_center.z))//圆弧终点相对圆心在第3象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
		else//圆弧终点相对圆心在第4象限
		{
			m_begin_angle = 360 - 0.5*acos((CS | CCYZ) / (CS.GetLength()*CCYZ.GetLength())) * 360 / PI;
			m_end_angle = 360 - 0.5*acos((CE | CCYZ) / (CE.GetLength()*CCYZ.GetLength())) * 360 / PI;
		}
	}


	CPoint3D temp;
	vector<CPoint3D> ArcTDMPos;
	CPoint3D ptBegin;
	CPoint3D ptEnd;

	float m_ptOffsetY;
	float m_ptOffsetZ;

	float m_Juge;

	//float feed = F*Inter / 60;//单位是微米级的
	float feed = 0.001*F*Inter / 60;//单位是毫米级的

	float data;

	BOOL Direction;
	if (TuduAngle > 0)//逆时针凸度值为正
		Direction = TRUE;
	else
		Direction = FALSE;


	if (!Direction)//逆时针为TRUE，顺时针为FALSE
	{
		m_ptOffsetY = begin.y - m_center.y;
		m_ptOffsetZ = begin.z - m_center.z;

	}
	else
	{
		m_ptOffsetY = begin.y - m_center.y;
		m_ptOffsetZ = begin.z - m_center.z;
	}

	ArcTDMPos.push_back(begin);

	if (!Direction)//Direcrtion为False时候是顺时针，这里是求顺时针的插补Go2
	{
		float x0 = m_ptOffsetY;//插补起点X坐标
		float y0 = m_ptOffsetZ;//插补起点Y坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetY>= 0 && m_ptOffsetZ >= 0)
				m_Juge = 1;
			else if (m_ptOffsetY <= 0 && m_ptOffsetZ >= 0)
				m_Juge = 2;
			else if (m_ptOffsetY <= 0 && m_ptOffsetZ <= 0)
				m_Juge = 3;
			else if (m_ptOffsetY >= 0 && m_ptOffsetZ <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetY) <= fabs(m_ptOffsetZ))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y0 - y1;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;


				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化

					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第二种方法B
				{
					float x1;
					float y1;

					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio > y1*y1)
						x1 = -sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(m_radio*m_radio - x1*x1);
					//zy = y0 - y1;
					zy = y1 - y0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y0 - y1;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
				else//用第二种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 - zy;
					x1 = -sqrt(m_radio*m_radio - y1*y1);
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第二种方法B
				{
					float x1;
					float y1;


					zy = -0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					//判断下以免出现无穷小的情况
					if (m_radio*m_radio > y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					//x1 = sqrt(fabs(radio*radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = -sqrt(m_radio*m_radio - x1*x1);
					zy = y1 - y0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
			}

			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.y - end.y, 2) + pow(temp.z - end.z, 2)) > data);
	}
	else
	{
		float x0 = m_ptOffsetY;//插补起点X坐标
		float y0 = m_ptOffsetZ;//插补起点Z坐标
		float zx = feed*y0 / m_radio;//起始的增量
		float zy = feed*x0 / m_radio;//起始的增量

		do{
			if (m_ptOffsetY >= 0 && m_ptOffsetZ >= 0)
				m_Juge = 1;
			else if (m_ptOffsetY <= 0 && m_ptOffsetZ >= 0)
				m_Juge = 2;
			else if (m_ptOffsetY <= 0 && m_ptOffsetZ <= 0)
				m_Juge = 3;
			else if (m_ptOffsetY >= 0 && m_ptOffsetZ <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内
			{

				if (fabs(m_ptOffsetY) <= fabs(m_ptOffsetZ))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = sqrt(m_radio*m_radio - y1*y1);
					else
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;


				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);


					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
			}
			else if (m_Juge == 2)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第二种方法A
				{
					float x1;
					float y1;

					zx = -0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 + zx;
					y1 = sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
			}
			else if (m_Juge == 3)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					if (m_radio*m_radio >y1*y1)
						x1 = -sqrt(fabs(m_radio*m_radio - y1*y1));
					else
						x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
				else//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}

			}
			else if (m_Juge == 4)
			{
				if (fabs(m_ptOffsetY) < fabs(m_ptOffsetZ))//用第二种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / m_radio;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(m_radio*m_radio - x1*x1));
					zy = y1 - y0;


					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;
				}
				else//用第一种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / m_radio;
					y1 = y0 + zy;
					x1 = sqrt(fabs(m_radio*m_radio - y1*y1));
					zx = x1 - x0;

					temp.y = x1 + m_center.y;
					temp.z = y1 + m_center.z;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetY = x0;
					m_ptOffsetZ = y0;

				}
			}
			data = sqrt(pow(zx, 2) + pow(zy, 2));
		} while (sqrt(pow(temp.y - end.y, 2) + pow(temp.z - end.z, 2))>data);

	}

	ArcTDMPos.push_back(end);

	for (vector<CPoint3D>::iterator it = ArcTDMPos.begin(); it != ArcTDMPos.end(); ++it)
	{
		it->x = begin.x;
	}

	Opposite = Reverse(ArcTDMPos, 100);
	return ArcTDMPos;
}
//零件显示及初始化
void CToolPath::RenderArcInit()//固定最后一个元素为Y侧面
{
	for (int i = 0; i != m_ArcArray.GetSize(); ++i)
	{
		if (((CArc*)m_ArcArray.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_ArcArray.GetAt(i))->m_Tudu == 0)//普通圆弧
			{

				if (i != m_ArcArray.GetSize() - 1)//非最后一个元素转到XOZ平面
				{
					CPoint3D center;
					center.x = ((CArc*)m_ArcArray.GetAt(i))->m_center.x;
					center.y = 0;
					center.z = ((CArc*)m_ArcArray.GetAt(i))->m_center.y;
					((CArc*)m_ArcArray.GetAt(i))->m_center = center;
				}
				else if (i == m_ArcArray.GetSize() - 1)
				{
					CPoint3D center;
					CVector3D Direction;

					center.y = ((CArc*)m_ArcArray.GetAt(i))->m_center.x;
					center.x = 0;
					center.z = ((CArc*)m_ArcArray.GetAt(i))->m_center.y;
					((CArc*)m_ArcArray.GetAt(i))->m_center = center;

					((CArc*)m_ArcArray.GetAt(i))->CalculateYZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_radio, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle - 270,
						((CArc*)m_ArcArray.GetAt(i))->m_end_angle + 90, m_ArcArray, i);

					//求出来的圆弧为顺时针圆弧，后期都是假定Yz平面的圆弧为逆时针圆弧，故进行转换2016.0522
					CPoint3D pt1, pt2;
					pt1 = ((CArc*)m_ArcArray.GetAt(i))->m_begin;
					pt2 = ((CArc*)m_ArcArray.GetAt(i))->m_end;
					

					if (((CArc*)m_ArcArray.GetAt(0))->m_Tudu == 0)
					{
						((CArc*)m_ArcArray.GetAt(0))->CalculateXZ(((CArc*)m_ArcArray.GetAt(0))->m_center, ((CArc*)m_ArcArray.GetAt(0))->m_radio, ((CArc*)m_ArcArray.GetAt(0))->m_begin_angle,
							((CArc*)m_ArcArray.GetAt(0))->m_end_angle, m_ArcArray, 0);
					}


					Direction.dx = ((CArc*)m_ArcArray.GetAt(0))->m_begin.x - ((CArc*)m_ArcArray.GetAt(i))->m_begin.x;
					Direction.dy = ((CArc*)m_ArcArray.GetAt(0))->m_begin.y - ((CArc*)m_ArcArray.GetAt(i))->m_begin.y;
					Direction.dz = ((CArc*)m_ArcArray.GetAt(0))->m_begin.z - ((CArc*)m_ArcArray.GetAt(i))->m_begin.z;

					((CArc*)m_ArcArray.GetAt(i))->m_center += Direction;
				}
			}
			else//多线段圆弧
			{
				if (i != m_ArcArray.GetSize() - 1)//转换到XOZ平面
				{
					CPoint3D temp;
					temp.x = ((CArc*)m_ArcArray.GetAt(i))->m_begin.x;
					temp.y = 0;
					temp.z = ((CArc*)m_ArcArray.GetAt(i))->m_begin.y;

					((CArc*)m_ArcArray.GetAt(i))->m_begin = temp;

					temp.x = ((CArc*)m_ArcArray.GetAt(i))->m_end.x;
					temp.y = 0;
					temp.z = ((CArc*)m_ArcArray.GetAt(i))->m_end.y;

					((CArc*)m_ArcArray.GetAt(i))->m_end = temp;

				}
				else if (i == m_ArcArray.GetSize() - 1)//转换到与YOZ平面平行的面上
				{
					CPoint3D temp1, temp2;
					CVector3D Direction, vec1, vec2;
					CVector3D Rotate1, Rotate2;


					temp1.x = 0;
					temp1.y = ((CArc*)m_ArcArray.GetAt(i))->m_begin.x;
					temp1.z = ((CArc*)m_ArcArray.GetAt(i))->m_begin.y;

					((CArc*)m_ArcArray.GetAt(i))->m_begin = temp1;

					temp2.x = 0;
					temp2.y = ((CArc*)m_ArcArray.GetAt(i))->m_end.x;
					temp2.z = ((CArc*)m_ArcArray.GetAt(i))->m_end.y;

					((CArc*)m_ArcArray.GetAt(i))->m_end = temp2;

					if (((CArc*)m_ArcArray.GetAt(0))->m_Tudu == 0)
					{
						((CArc*)m_ArcArray.GetAt(0))->CalculateXZ(((CArc*)m_ArcArray.GetAt(0))->m_center, ((CArc*)m_ArcArray.GetAt(0))->m_radio, ((CArc*)m_ArcArray.GetAt(0))->m_begin_angle,
							((CArc*)m_ArcArray.GetAt(0))->m_end_angle, m_ArcArray, 0);
					}

					((CArc*)m_ArcArray.GetAt(i))->PolyCalculateYZ(((CArc*)m_ArcArray.GetAt(i))->m_begin, ((CArc*)m_ArcArray.GetAt(i))->m_end, ((CArc*)m_ArcArray.GetAt(i))->m_Tudu);

					vec1 = ((CArc*)m_ArcArray.GetAt(i))->m_begin - ((CArc*)m_ArcArray.GetAt(i))->m_center;
					vec2 = ((CArc*)m_ArcArray.GetAt(i))->m_end - ((CArc*)m_ArcArray.GetAt(i))->m_center;
					Rotate1 = CVector3D(vec1.dx, -1 * vec1.dz, vec1.dy);//起点逆时针旋转90度
					Rotate2 = CVector3D(vec2.dx, -1 * vec2.dz, vec2.dy);//终点逆时针旋转90度

					((CArc*)m_ArcArray.GetAt(i))->m_begin = ((CArc*)m_ArcArray.GetAt(i))->m_center + Rotate1;//旋转90度后的起点的位置坐标
					((CArc*)m_ArcArray.GetAt(i))->m_end = ((CArc*)m_ArcArray.GetAt(i))->m_center + Rotate2;//旋转90度后的终点的位置坐标

					//旋转后的起点相对于第一个元素的偏移量
					Direction.dx = ((CArc*)m_ArcArray.GetAt(0))->m_begin.x - ((CArc*)m_ArcArray.GetAt(i))->m_begin.x;
					Direction.dy = ((CArc*)m_ArcArray.GetAt(0))->m_begin.y - ((CArc*)m_ArcArray.GetAt(i))->m_begin.y;
					Direction.dz = ((CArc*)m_ArcArray.GetAt(0))->m_begin.z - ((CArc*)m_ArcArray.GetAt(i))->m_begin.z;

					//使最后一个元素的起点与第一个元素的起点相连接
					((CArc*)m_ArcArray.GetAt(i))->m_begin += Direction;
					((CArc*)m_ArcArray.GetAt(i))->m_end += Direction;

				}

			}
		}
	}

	vector<vector<CPoint3D>>().swap(m_ArcPiecePoints);//清空数据

	for (int i = 0; i != m_ArcArray.GetSize(); ++i)
	{
		if (((CArc*)m_ArcArray.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_ArcArray.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				if (((CArc*)m_ArcArray.GetAt(i))->m_center.y == 0)//XOZ平面
				{
					vector<CPoint3D> point =ArcDiscreteXZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_radio, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ArcArray.GetAt(i))->m_end_angle, m_ArcArray, i, 60, 6000);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->CalculateYZ(((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_center, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_radio,
								((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin_angle - 270, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_end_angle + 90, m_ArcArray, m_ArcArray.GetSize() - 1);
						}

						Direction = *it - ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin;

						vector<CPoint3D> point2 = ArcDiscreteYZ(((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_center + Direction, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_radio, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin_angle - 270,
							((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_end_angle + 90, m_ArcArray, m_ArcArray.GetSize() - 1, 60, 3000);
						m_ArcPiecePoints.push_back(point2);
					}
				}
				
			}
			else//多线段圆弧
			{

				if (((CArc*)m_ArcArray.GetAt(i))->m_begin.y == 0 && ((CArc*)m_ArcArray.GetAt(i))->m_end.y == 0)//XOZ平面
				{
					//((CArc*)m_ArcArray.GetAt(i))->PolyCalculateXZ(((CArc*)m_ArcArray.GetAt(i))->m_begin, ((CArc*)m_ArcArray.GetAt(i))->m_end,
						//((CArc*)m_ArcArray.GetAt(i))->m_Tudu);

					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_ArcArray.GetAt(i))->m_begin, ((CArc*)m_ArcArray.GetAt(i))->m_end,
						((CArc*)m_ArcArray.GetAt(i))->m_Tudu, 60, 6000/*8, 300*/);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->CalculateYZ(((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_center, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_radio, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin_angle - 270,
								((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_end_angle + 90, m_ArcArray, m_ArcArray.GetSize() - 1);
						}
						Direction = *it - ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin;
						vector<CPoint3D> point2 =PolyArcDiscreteYZ(((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_begin + Direction, ((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_end + Direction,
							((CArc*)m_ArcArray.GetAt(m_ArcArray.GetSize() - 1))->m_Tudu, 60, 6000);
						m_ArcPiecePoints.push_back(point2);
					}
				}
			}
		}
	}
}
void CToolPath::RenderArc(COpenGLDC* pDC)
{
	if (m_ArcPiecePoints.size() != 0)
	{
		for (vector<vector<CPoint3D>>::iterator it = m_ArcPiecePoints.begin(); it!=m_ArcPiecePoints.end(); ++it)
		{
			for (vector<CPoint3D>::iterator its = it->begin(); its != it->end()-1; ++its)
			{
				pDC->DrawLine(*its, *(its + 1));
			}
		}

		for (int i = 0; i != m_ArcArray.GetSize(); ++i)
		{
			if (((CArc*)m_ArcArray.GetAt(i))->flag == 2)
			{
				if (((CArc*)m_ArcArray.GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					if (((CArc*)m_ArcArray.GetAt(i))->m_center.y == 0)//XOZ平面
					{
						((CArc*)m_ArcArray.GetAt(i))->GLDrawArcXZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ArcArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio, m_ArcArray, i);
					}
					else
					{
						((CArc*)m_ArcArray.GetAt(i))->GLDrawArcYZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle - 270,
							((CArc*)m_ArcArray.GetAt(i))->m_end_angle + 90, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio, m_ArcArray, i);

					}
				}
				else//多线段圆弧
				{
					if (((CArc*)m_ArcArray.GetAt(i))->m_begin.y == 0 && ((CArc*)m_ArcArray.GetAt(i))->m_end.y == 0)//XOZ平面
					{
						((CArc*)m_ArcArray.GetAt(i))->PolyCalculateXZ(((CArc*)m_ArcArray.GetAt(i))->m_begin, ((CArc*)m_ArcArray.GetAt(i))->m_end,
							((CArc*)m_ArcArray.GetAt(i))->m_Tudu);

						if (((CArc*)m_ArcArray.GetAt(i))->m_Tudu < 0)//顺时针
						{
							((CArc*)m_ArcArray.GetAt(i))->PolyDrawSArcXZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
								((CArc*)m_ArcArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio);
						}
						else//逆时针
						{
							((CArc*)m_ArcArray.GetAt(i))->PolyDrawNArcXZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
								((CArc*)m_ArcArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio);
						}
					}
					else
					{
						((CArc*)m_ArcArray.GetAt(i))->PolyCalculateYZ(((CArc*)m_ArcArray.GetAt(i))->m_begin, ((CArc*)m_ArcArray.GetAt(i))->m_end,
							((CArc*)m_ArcArray.GetAt(i))->m_Tudu);


						if (((CArc*)m_ArcArray.GetAt(i))->m_Tudu < 0)//顺时针
						{
							((CArc*)m_ArcArray.GetAt(i))->PolyDrawSArcYZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
								((CArc*)m_ArcArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio);
						}
						else//逆时针
						{
							((CArc*)m_ArcArray.GetAt(i))->PolyDrawNArcYZ(((CArc*)m_ArcArray.GetAt(i))->m_center, ((CArc*)m_ArcArray.GetAt(i))->m_begin_angle,
								((CArc*)m_ArcArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ArcArray.GetAt(i))->m_radio);
						}
					}
				}
			}
		}
	}
}
//每一层的离散及显示
vector<vector<CPoint3D>> CToolPath::ArcDiscrete(CPtrArray& m_Array)//离散步长可以在里面设置
{
	vector<vector<CPoint3D>> m_tempPoints;

	for (int i = 0; i != m_Array.GetSize(); ++i)
	{
		if (((CArc*)m_Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				if (((CArc*)m_Array.GetAt(i))->m_center.y == 0)//XOZ平面
				{
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_radio, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Array.GetAt(i))->m_end_angle, m_Array, i, GetXArcRoughFeedTime(),GetXArcRoughFeedSpeed()/*60, 6000*/);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->CalculateYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio,
								((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1);
						}

						Direction = *it - ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin;

						vector<CPoint3D> point2 = ArcDiscreteYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center + Direction, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270,
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1, GetYArcRoughFeedTime(),GetYArcRoughFeedSpeed()/*60, 3000*/);
						m_tempPoints.push_back(point2);
					}
				}

			}
			else//多线段圆弧
			{

				if (((CArc*)m_Array.GetAt(i))->m_begin.y == 0 && ((CArc*)m_Array.GetAt(i))->m_end.y == 0)//XOZ平面
				{
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Array.GetAt(i))->m_begin, ((CArc*)m_Array.GetAt(i))->m_end,
						((CArc*)m_Array.GetAt(i))->m_Tudu, GetXArcRoughFeedTime(), GetXArcRoughFeedSpeed()/*, 60, 6000*//*8, 300*/);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->CalculateYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270,
								((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1);
						}
						Direction = *it - ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin;
						vector<CPoint3D> point2 = PolyArcDiscreteYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin + Direction, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end + Direction,
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu, GetYArcRoughFeedTime(), GetYArcRoughFeedSpeed()/*60, 6000*/);
						m_tempPoints.push_back(point2);
					}
				}
			}
		}
	}

	return m_tempPoints;
}
void CToolPath::RenderArcSection(CPtrArray& m_Array, COpenGLDC* pDC)
{
	for (int i = 0; i != m_Array.GetSize(); ++i)
	{
		if (((CArc*)m_Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				if (((CArc*)m_Array.GetAt(i))->m_center.y == 0)//XOZ平面
				{
					((CArc*)m_Array.GetAt(i))->GLDrawArcXZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Array.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio, m_Array, i);
				}
				else
				{
					((CArc*)m_Array.GetAt(i))->GLDrawArcYZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle - 270,
						((CArc*)m_Array.GetAt(i))->m_end_angle + 90, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio, m_Array, i);

				}
			}
			else//多线段圆弧
			{
				if (((CArc*)m_Array.GetAt(i))->m_begin.y == 0 && ((CArc*)m_Array.GetAt(i))->m_end.y == 0)//XOZ平面
				{
					((CArc*)m_Array.GetAt(i))->PolyCalculateXZ(((CArc*)m_Array.GetAt(i))->m_begin, ((CArc*)m_Array.GetAt(i))->m_end,
						((CArc*)m_Array.GetAt(i))->m_Tudu);

					if (((CArc*)m_Array.GetAt(i))->m_Tudu < 0)//顺时针
					{
						((CArc*)m_Array.GetAt(i))->PolyDrawSArcXZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
							((CArc*)m_Array.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio);
					}
					else//逆时针
					{
						((CArc*)m_Array.GetAt(i))->PolyDrawNArcXZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
							((CArc*)m_Array.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio);
					}
				}
				else
				{
					((CArc*)m_Array.GetAt(i))->PolyCalculateYZ(((CArc*)m_Array.GetAt(i))->m_begin, ((CArc*)m_Array.GetAt(i))->m_end,
						((CArc*)m_Array.GetAt(i))->m_Tudu);


					if (((CArc*)m_Array.GetAt(i))->m_Tudu < 0)//顺时针
					{
						((CArc*)m_Array.GetAt(i))->PolyDrawSArcYZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
							((CArc*)m_Array.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio);
					}
					else//逆时针
					{
						((CArc*)m_Array.GetAt(i))->PolyDrawNArcYZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
							((CArc*)m_Array.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_Array.GetAt(i))->m_radio);
					}
				}
			}
		}
	}
}
//粗加工显示及初始化
void CToolPath::RenderRoughArcInit()
{
	//从上往下
	if (m_ArcArray.IsEmpty() == 0)
	{
		vector<CPtrArray*>().swap(m_ArcRoughArrayList);
		for (int i = GetArcRoughPiles(); i != -1; --i)
		{
			CPtrArray* temparray = new CPtrArray;
			for (int j = 0; j != m_ArcArray.GetSize(); ++j)
			{
				if (((CArc*)m_ArcArray.GetAt(j))->flag == 2)
				{
					if (((CArc*)m_ArcArray.GetAt(j))->m_Tudu == 0)//普通圆弧
					{
						CArc* pArc = new CArc;
						temparray->Add(pArc);
						pArc->m_center = ((CArc*)m_ArcArray.GetAt(j))->m_center;
						pArc->m_center.z = ((CArc*)m_ArcArray.GetAt(j))->m_center.z + i*GetArcRoughVerticalDistance() + GetArcRoughLeft();
						pArc->m_radio = ((CArc*)m_ArcArray.GetAt(j))->m_radio;
						pArc->m_begin_angle = ((CArc*)m_ArcArray.GetAt(j))->m_begin_angle;
						pArc->m_end_angle = ((CArc*)m_ArcArray.GetAt(j))->m_end_angle;
						pArc->flag = 2;
						pArc->m_Tudu = 0;

					}
					else//多线段圆弧
					{
						CArc* pArc = new CArc;
						temparray->Add(pArc);
						pArc->m_begin = ((CArc*)m_ArcArray.GetAt(j))->m_begin;
						pArc->m_begin.z = ((CArc*)m_ArcArray.GetAt(j))->m_begin.z+ i*GetArcRoughVerticalDistance() + GetArcRoughLeft();
						pArc->m_end = ((CArc*)m_ArcArray.GetAt(j))->m_end;
						pArc->m_end.z = ((CArc*)m_ArcArray.GetAt(j))->m_end.z + i*GetArcRoughVerticalDistance() + GetArcRoughLeft();

						pArc->m_Tudu = ((CArc*)m_ArcArray.GetAt(j))->m_Tudu;
						pArc->flag = 2;
					}
				}
			}

			m_ArcRoughArrayList.push_back(temparray);
		}

		vector<vector<vector<CPoint3D>>>().swap(m_ArcRoughPoints);
		//CPoint3D m_arctempcenter = GetArcWorkBlankCenter(GetArcWorkBlankHeigth());//获取毛坯的中点
		for (vector<CPtrArray*>::iterator it = m_ArcRoughArrayList.begin(); it != m_ArcRoughArrayList.end(); ++it)
		{
			vector<vector<CPoint3D>> m_tempArcRoughPt;
			m_tempArcRoughPt = ArcDiscrete(**it);
			
			vector<CPoint3D> m_LeftPt, m_DownPt;
			CPoint3D Pt1, Pt2;//设置下刀点，抬刀点,在粗加工，精加工以及粗加工裁剪的初始化都要设计这部分内容
			
			Pt1.x = m_tempArcRoughPt.begin()->begin()->x;//起点上方
			Pt1.y = m_tempArcRoughPt.begin()->begin()->y;
			Pt1.z = 2500;
			m_DownPt.push_back(Pt1);
			Pt1.x = m_tempArcRoughPt.begin()->begin()->x;//起点
			Pt1.y = m_tempArcRoughPt.begin()->begin()->y;
			Pt1.z = m_tempArcRoughPt.begin()->begin() ->z;
			m_DownPt.push_back(Pt1);
			


			if (m_tempArcRoughPt.size() % 2 != 0)
			{
				Pt2.x = ((m_tempArcRoughPt.end() - 1)->end() - 1)->x;//终点
				Pt2.y = ((m_tempArcRoughPt.end() - 1)->end() - 1)->y;
				Pt2.z = ((m_tempArcRoughPt.end() - 1)->end() - 1)->z;
				m_LeftPt.push_back(Pt2);
				Pt2.x = ((m_tempArcRoughPt.end() - 1)->end() - 1)->x;//终点上方
				Pt2.y = ((m_tempArcRoughPt.end() - 1)->end() - 1)->y;
				Pt2.z = 2500;
				m_LeftPt.push_back(Pt2);
			}
			else
			{
				
				Pt2.x = ((m_tempArcRoughPt.end() - 1)->begin())->x;//终点
				Pt2.y = ((m_tempArcRoughPt.end() - 1)->begin())->y;
				Pt2.z = ((m_tempArcRoughPt.end() - 1)->begin())->z;
				m_LeftPt.push_back(Pt2);
				Pt2.x = ((m_tempArcRoughPt.end() - 1)->begin())->x;//终点上方
				Pt2.y = ((m_tempArcRoughPt.end() - 1)->begin())->y;
				Pt2.z = 2500;
				m_LeftPt.push_back(Pt2);
			}

			//插入抬刀点，下刀点
			m_tempArcRoughPt.insert(m_tempArcRoughPt.begin(), m_DownPt);
			m_tempArcRoughPt.insert(m_tempArcRoughPt.end(),m_LeftPt );

			m_ArcRoughPoints.push_back(m_tempArcRoughPt);
			vector<vector<CPoint3D>>().swap(m_tempArcRoughPt);
		}
	}
}
void CToolPath::RenderRoughArc(COpenGLDC* pDC)
{
	for (vector<vector<vector<CPoint3D>>>::iterator it = m_ArcRoughPoints.begin(); it != m_ArcRoughPoints.end(); ++it)
	{
		
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
			{
					pDC->DrawLine(*itss, *(itss + 1));
			}
			
		}
		//绘制每段之间的连线
		BOOL direction = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin()+1; its != it->end(); ++its)//去除首尾两段的抬刀和退刀
		{
			if (direction)//终点对终点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its==it->end()-2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->end() - 1), *((its + 1)->end()-1));
				}
			}
			else//起点对起点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its == it->end() - 2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
				}
			}
			direction = !direction;
		}

	}
	//绘制侧面轮廓线
	/*for (vector<CPtrArray*>::iterator it = m_ArcRoughArrayList.begin(); it != m_ArcRoughArrayList.end(); ++it)
	{
		RenderArcSection(**it, pDC);
	}*/
}

//获取中心点
CPoint3D CToolPath::GetArcWorkBlankCenter(float higth)
{
	CPoint3D tempcenter;
	CPoint3D WorkCenter;
	double XMAX, YMAX, ZMAX,XMIN,YMIN,ZMIN;//存储三个方向上的最大值点
	
	for (vector<vector<CPoint3D>>::iterator it = m_ArcPiecePoints.begin(); it != m_ArcPiecePoints.end(); ++it)
	{
		for (vector<CPoint3D>::iterator its = it->begin(); its != it->end(); ++its)
		{
			if (it == m_ArcPiecePoints.begin() && its == it->begin())//将最大最小值点设为第一个元素的点
			{
				XMAX = XMIN = its->x;
				YMAX = YMIN = its->y;
				ZMAX = ZMIN = its->z;
			}
			else
			{
				if (its->x >= XMAX)
				{
					XMAX = its->x;
				}
				if (its->x <= XMIN)
				{
					XMIN = its->x;
				}
				if (its->y >= YMAX)
				{
					  YMAX=its->y;
				}
				if (its->y <= YMIN)
				{
					YMIN = its->y;
				}
				if (its->z >= ZMAX)
				{
					ZMAX = its->z;
				}
				if (its->z <= ZMIN)
				{
					ZMIN = its->z;
				}
			}

		}
	}

	WorkCenter.x = XMIN + (XMAX - XMIN) / 2;
	WorkCenter.y = YMIN + (YMAX - YMIN) / 2;
	WorkCenter.z = ZMIN + (ZMAX - ZMIN) / 2;


	tempcenter.x = WorkCenter.x;
	tempcenter.y = WorkCenter.y;
	tempcenter.z = higth / 2;


	return tempcenter;
}


//粗加工裁剪
void CToolPath::RenderArcRoughCutInit()
{
	vector<vector<CPtrArray*>>().swap(m_ArcsYSide);//清除数据
	for (vector<CPtrArray*>::iterator it = m_ArcRoughArrayList.begin(); it != m_ArcRoughArrayList.end(); ++it)
	{
		vector<CPoint3D> m_Arctemppts;//临时存储点,将每一层除了最后一个圆弧的元素进行离散，并存储点值
		vector<CPtrArray*> m_tempYArc;
		for (int i = 0; i != (*it)->GetSize() - 1; ++i)
		{
			if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
					((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, GetXArcRoughFeedTime(), GetXArcRoughFeedSpeed()/*60, 6000*/);
				for (vector<CPoint3D>::iterator pt = point.begin(); pt != point.end(); ++pt)
				{
					m_Arctemppts.push_back(*pt);
				}
			}
			else//多线段圆弧
			{
				vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
					((CArc*)(*it)->GetAt(i))->m_Tudu, GetXArcRoughFeedTime(), GetXArcRoughFeedSpeed()/*60, 6000*/);
				for (vector<CPoint3D>::iterator pt = point.begin(); pt != point.end(); ++pt)
				{
					m_Arctemppts.push_back(*pt);
				}
			}
		}

		for (vector<CPoint3D>::iterator its = m_Arctemppts.begin(); its != m_Arctemppts.end(); ++its)
		{
			CVector3D Direction;
			CPtrArray* tempArc = new CPtrArray;
			CArc* pArc = new CArc;
			tempArc->Add(pArc);
			if (((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu == 0)
			{
				((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->CalculateYZ(((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_center, ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_radio, ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin_angle - 270,
					((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end_angle + 90, **it, (*it)->GetSize() - 1);
			}
			Direction = *its - ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin;

			if (((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu == 0)//普通圆弧
			{
				pArc->m_begin_angle = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin_angle;
				pArc->m_end_angle = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end_angle;
				pArc->m_center = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_center + Direction;
				pArc->m_radio = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_radio;
				pArc->m_Tudu = 0;
				pArc->flag = 2;
			}
			else
			{
				pArc->m_begin = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin + Direction;
				pArc->m_end = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end + Direction;
				pArc->m_Tudu = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu;
				pArc->flag = 2;
			}

			m_tempYArc.push_back(tempArc);
		}

		m_ArcsYSide.push_back(m_tempYArc);
		vector<CPtrArray*>().swap(m_tempYArc);
	}

	
	//将m_ArcsYSide里的每条圆弧进行判断与毛坯矩形的交点问题，如有交点，求解器交点，并且将元素按顺序存储好，进行点的离散后，进行显示
	vector<vector<vector<CPoint3D>>>().swap(m_ArcRoughCutPoints);//清空数据
	for (vector<vector<CPtrArray*>>::iterator it = m_ArcsYSide.begin(); it != m_ArcsYSide.end(); ++it)
	{
		vector<vector<CPoint3D>> m_tempArcRoughCutpt;

		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<CPoint3D> m_tempSingleArcRoughCutpt;
			for (int i = 0; i != (*its)->GetSize(); ++i)
			{
				if (((CArc*)(*its)->GetAt(i))->flag == 2)//判断为圆
				{
					if (((CArc*)(*its)->GetAt(i))->m_Tudu == 0)
					{
						m_tempSingleArcRoughCutpt = ArcDiscreteYZ(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_radio,
							((CArc*)(*its)->GetAt(i))->m_begin_angle-270, ((CArc*)(*its)->GetAt(i))->m_end_angle+90, **its, i, GetYArcRoughFeedTime(), GetYArcRoughFeedSpeed());
						
					}
					else
					{
						m_tempSingleArcRoughCutpt = PolyArcDiscreteYZ(((CArc*)(*its)->GetAt(i))->m_begin, ((CArc*)(*its)->GetAt(i))->m_end,
							((CArc*)(*its)->GetAt(i))->m_Tudu, GetYArcRoughFeedTime(), GetYArcRoughFeedSpeed());
					}

					float m_jugeHeight = GetArcWorkBlankHeigth();
					//比毛坯矩形高的点，其Z坐标都设为毛坯矩形的高
					for (vector<CPoint3D>::iterator pts = m_tempSingleArcRoughCutpt.begin(); pts != m_tempSingleArcRoughCutpt.end(); ++pts)
					{
						if (pts->z >= m_jugeHeight)
						{
							pts->z = m_jugeHeight;
						}
					}

				}
			}


			m_tempArcRoughCutpt.push_back(m_tempSingleArcRoughCutpt);
			vector<CPoint3D>().swap(m_tempSingleArcRoughCutpt);
		}

		//设置抬刀点和下刀点
		vector<CPoint3D> m_LeftPt, m_DownPt;
		CPoint3D Pt1, Pt2;//设置下刀点，抬刀点

		Pt1.x = m_tempArcRoughCutpt.begin()->begin()->x;//起点上方
		Pt1.y = m_tempArcRoughCutpt.begin()->begin()->y;
		Pt1.z = 2500;
		m_DownPt.push_back(Pt1);
		Pt1.x = m_tempArcRoughCutpt.begin()->begin()->x;//起点
		Pt1.y = m_tempArcRoughCutpt.begin()->begin()->y;
		Pt1.z = m_tempArcRoughCutpt.begin()->begin()->z;
		m_DownPt.push_back(Pt1);

		if (it->size() % 2 != 0)
		{
			Pt2.x = ((m_tempArcRoughCutpt.end() - 1)->end() - 1)->x;//终点
			Pt2.y = ((m_tempArcRoughCutpt.end() - 1)->end() - 1)->y;
			Pt2.z = ((m_tempArcRoughCutpt.end() - 1)->end() - 1)->z;
			m_LeftPt.push_back(Pt2);
			Pt2.x = ((m_tempArcRoughCutpt.end() - 1)->end() - 1)->x;//终点上方
			Pt2.y = ((m_tempArcRoughCutpt.end() - 1)->end() - 1)->y;
			Pt2.z = 2500;
			m_LeftPt.push_back(Pt2);
		}
		else
		{

			Pt2.x = ((m_tempArcRoughCutpt.end() - 1)->begin())->x;//终点
			Pt2.y = ((m_tempArcRoughCutpt.end() - 1)->begin())->y;
			Pt2.z = ((m_tempArcRoughCutpt.end() - 1)->begin())->z;
			m_LeftPt.push_back(Pt2);
			Pt2.x = ((m_tempArcRoughCutpt.end() - 1)->begin())->x;//终点上方
			Pt2.y = ((m_tempArcRoughCutpt.end() - 1)->begin())->y;
			Pt2.z = 2500;
			m_LeftPt.push_back(Pt2);
		}

		//插入抬刀点，下刀点
		m_tempArcRoughCutpt.insert(m_tempArcRoughCutpt.begin(), m_DownPt);
		m_tempArcRoughCutpt.insert(m_tempArcRoughCutpt.end(), m_LeftPt);

		m_ArcRoughCutPoints.push_back(m_tempArcRoughCutpt);
		vector<vector<CPoint3D>>().swap(m_tempArcRoughCutpt);
	}

	//将粗加工裁剪点存储到粗加工仿真点中
	vector<CPoint3D>().swap(RoughSimulationPoints);//清零
	RoughSimulationPoints = SetArcRoughSimulationPoints(m_ArcRoughCutPoints);
}

void CToolPath::RenderArcRoughCut(COpenGLDC* pDC)
{
	//显示每条的Y侧圆弧，略卡
	/*for (vector<vector<CPtrArray*>>::iterator it = m_ArcsYSide.begin(); it != m_ArcsYSide.end(); ++it)
	{
		
		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (int i = 0; i != (*its)->GetSize(); ++i)
			{
				if (((CArc*)(*its)->GetAt(i))->m_Tudu == 0)
				{
					((CArc*)(*its)->GetAt(i))->GLDrawArcYZ(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_begin_angle - 270, ((CArc*)(*its)->GetAt(i))->m_end_angle + 90, 
						                                    0.0001, ((CArc*)(*its)->GetAt(i))->m_radio,**its, i);
				}
				else
				{
					((CArc*)(*its)->GetAt(i))->PolyCalculateYZ(((CArc*)(*its)->GetAt(i))->m_begin, ((CArc*)(*its)->GetAt(i))->m_end, ((CArc*)(*its)->GetAt(i))->m_Tudu);
					if (((CArc*)(*its)->GetAt(i))->m_Tudu < 0)
					{
						((CArc*)(*its)->GetAt(i))->PolyDrawSArcYZ(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_begin_angle, ((CArc*)(*its)->GetAt(i))->m_end_angle, 0.0001, ((CArc*)(*its)->GetAt(i))->m_radio);
					}
					else
					{
						((CArc*)(*its)->GetAt(i))->PolyDrawNArcYZ(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_begin_angle, ((CArc*)(*its)->GetAt(i))->m_end_angle, 0.0001, ((CArc*)(*its)->GetAt(i))->m_radio);
					}
				}
			}
			
		}
	}*/

	for (vector<vector<vector<CPoint3D>>>::iterator it = m_ArcRoughCutPoints.begin(); it != m_ArcRoughCutPoints.end(); ++it)
	{
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
			{
				pDC->DrawLine(*itss, *(itss + 1));
			}
		}


		//绘制每段之间的连线
		BOOL direction = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin() + 1; its != it->end(); ++its)//去除首尾两段的抬刀和退刀
		{
			if (direction)//终点对终点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its == it->end() - 2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->end() - 1), *((its + 1)->end() - 1));
				}
			}
			else//起点对起点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its == it->end() - 2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
				}
			}
			direction = !direction;
		}


	}

}

//粗加工余量
void CToolPath::RenderArcRoughLeftInit()
{
	if (m_ArcArray.IsEmpty() == 0)
	{
		m_ArcRoughLeftArray.RemoveAll();
		vector<vector<CPoint3D>>().swap(m_ArcRoughLeftPoints);
		for (int j = 0; j != m_ArcArray.GetSize(); ++j)
		{
			if (((CArc*)m_ArcArray.GetAt(j))->flag == 2)
			{
				if (((CArc*)m_ArcArray.GetAt(j))->m_Tudu == 0)//普通圆弧
				{
					CArc* pArc = new CArc;
					m_ArcRoughLeftArray.Add(pArc);
					pArc->m_center = ((CArc*)m_ArcArray.GetAt(j))->m_center;
					pArc->m_center.z = ((CArc*)m_ArcArray.GetAt(j))->m_center.z  + GetArcRoughLeft();
					pArc->m_radio = ((CArc*)m_ArcArray.GetAt(j))->m_radio;
					pArc->m_begin_angle = ((CArc*)m_ArcArray.GetAt(j))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)m_ArcArray.GetAt(j))->m_end_angle;
					pArc->flag = 2;
					pArc->m_Tudu = 0;

				}
				else//多线段圆弧
				{
					CArc* pArc = new CArc;
					m_ArcRoughLeftArray.Add(pArc);
					pArc->m_begin = ((CArc*)m_ArcArray.GetAt(j))->m_begin;
					pArc->m_begin.z = ((CArc*)m_ArcArray.GetAt(j))->m_begin.z + GetArcRoughLeft();
					pArc->m_end = ((CArc*)m_ArcArray.GetAt(j))->m_end;
					pArc->m_end.z = ((CArc*)m_ArcArray.GetAt(j))->m_end.z  + GetArcRoughLeft();

					pArc->m_Tudu = ((CArc*)m_ArcArray.GetAt(j))->m_Tudu;
					pArc->flag = 2;
				}
			}
		}

		m_ArcRoughLeftPoints = ArcDiscrete(m_ArcRoughLeftArray);
	}
}
void CToolPath::RenderArcRoughLeft(COpenGLDC* pDC)
{
	
	for (vector<vector<CPoint3D>>::iterator it = m_ArcRoughLeftPoints.begin(); it != m_ArcRoughLeftPoints.end(); ++it)
	{
		for (vector<CPoint3D>::iterator its = it->begin(); its != it->end()-1; ++its)
		{
			pDC->DrawLine(*its, *(its + 1));
		}
	}

	RenderArcSection(m_ArcRoughLeftArray, pDC);
	
}

//精加工初始化及显示
vector<vector<CPoint3D>> CToolPath::ArcFinishDiscrete(CPtrArray& m_Array)
{
	vector<vector<CPoint3D>> m_tempPoints;

	for (int i = 0; i != m_Array.GetSize(); ++i)
	{
		if (((CArc*)m_Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				if (((CArc*)m_Array.GetAt(i))->m_center.y == 0)//XOZ平面
				{
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_radio, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
						((CArc*)m_Array.GetAt(i))->m_end_angle, m_Array, i, GetXArcFinishFeedTime(), GetXArcFinishFeedSpeed()/*60, 6000*/);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->CalculateYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio,
								((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1);
						}

						Direction = *it - ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin;

						vector<CPoint3D> point2 = ArcDiscreteYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center + Direction, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270,
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1, GetYArcFinishFeedTime(), GetYArcFinishFeedSpeed()/*60, 3000*/);
						m_tempPoints.push_back(point2);
					}
				}

			}
			else//多线段圆弧
			{

				if (((CArc*)m_Array.GetAt(i))->m_begin.y == 0 && ((CArc*)m_Array.GetAt(i))->m_end.y == 0)//XOZ平面
				{
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_Array.GetAt(i))->m_begin, ((CArc*)m_Array.GetAt(i))->m_end,
						((CArc*)m_Array.GetAt(i))->m_Tudu, GetXArcFinishFeedTime(), GetXArcFinishFeedSpeed()/*, 60, 6000*//*8, 300*/);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						CVector3D Direction;
						if (((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu == 0)
						{
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->CalculateYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_center, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_radio, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin_angle - 270,
								((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end_angle + 90, m_Array, m_Array.GetSize() - 1);
						}
						Direction = *it - ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin;
						vector<CPoint3D> point2 = PolyArcDiscreteYZ(((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_begin + Direction, ((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_end + Direction,
							((CArc*)m_Array.GetAt(m_Array.GetSize() - 1))->m_Tudu, GetYArcFinishFeedTime(), GetYArcFinishFeedSpeed()/*60, 6000*/);
						m_tempPoints.push_back(point2);
					}
				}
			}
		}
	}

	return m_tempPoints;
}
void CToolPath::RenderArcFinishInit()
{
	if (m_ArcArray.IsEmpty() == 0)
	{
		vector<CPtrArray*>().swap(m_ArcFinishArrayList);
		for (int i = GetArcFinishPiles(); i != -1; --i)
		{
			CPtrArray* temparray = new CPtrArray;
			for (int j = 0; j != m_ArcArray.GetSize(); ++j)
			{
				if (((CArc*)m_ArcArray.GetAt(j))->flag == 2)
				{
					if (((CArc*)m_ArcArray.GetAt(j))->m_Tudu == 0)//普通圆弧
					{
						CArc* pArc = new CArc;
						temparray->Add(pArc);
						pArc->m_center = ((CArc*)m_ArcArray.GetAt(j))->m_center;
						pArc->m_center.z = ((CArc*)m_ArcArray.GetAt(j))->m_center.z + i*GetArcFinishVerticalDistance();
						pArc->m_radio = ((CArc*)m_ArcArray.GetAt(j))->m_radio;
						pArc->m_begin_angle = ((CArc*)m_ArcArray.GetAt(j))->m_begin_angle;
						pArc->m_end_angle = ((CArc*)m_ArcArray.GetAt(j))->m_end_angle;
						pArc->flag = 2;
						pArc->m_Tudu = 0;

					}
					else//多线段圆弧
					{
						CArc* pArc = new CArc;
						temparray->Add(pArc);
						pArc->m_begin = ((CArc*)m_ArcArray.GetAt(j))->m_begin;
						pArc->m_begin.z = ((CArc*)m_ArcArray.GetAt(j))->m_begin.z + i*GetArcFinishVerticalDistance();
						pArc->m_end = ((CArc*)m_ArcArray.GetAt(j))->m_end;
						pArc->m_end.z = ((CArc*)m_ArcArray.GetAt(j))->m_end.z + i*GetArcFinishVerticalDistance();

						pArc->m_Tudu = ((CArc*)m_ArcArray.GetAt(j))->m_Tudu;
						pArc->flag = 2;
					}
				}
			}

			m_ArcFinishArrayList.push_back(temparray);
			//temparray->RemoveAll();
		}

		//将精加工的Y侧面轮廓提取出来
		vector<vector<CPtrArray*>>().swap(m_ArcFinishYSide);
		for (vector<CPtrArray*>::iterator it = m_ArcFinishArrayList.begin(); it != m_ArcFinishArrayList.end(); ++it)
		{
			vector<CPoint3D> m_Arctemppts;//临时存储点,将每一层除了最后一个圆弧的元素进行离散，并存储点值
			vector<CPtrArray*> m_tempYArc;
			for (int i = 0; i != (*it)->GetSize() - 1; ++i)
			{
				if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
						((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, GetXArcFinishFeedTime(), GetXArcFinishFeedSpeed()/*60, 6000*/);
					for (vector<CPoint3D>::iterator pt = point.begin(); pt != point.end(); ++pt)
					{
						m_Arctemppts.push_back(*pt);
					}
				}
				else//多线段圆弧
				{
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
						((CArc*)(*it)->GetAt(i))->m_Tudu, GetXArcFinishFeedTime(), GetXArcFinishFeedSpeed()/*60, 6000*/);
					for (vector<CPoint3D>::iterator pt = point.begin(); pt != point.end(); ++pt)
					{
						m_Arctemppts.push_back(*pt);
					}
				}
			}

			for (vector<CPoint3D>::iterator its = m_Arctemppts.begin(); its != m_Arctemppts.end(); ++its)
			{
				CVector3D Direction;
				CPtrArray* tempArc = new CPtrArray;
				CArc* pArc = new CArc;
				tempArc->Add(pArc);
				if (((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu == 0)
				{
					((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->CalculateYZ(((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_center, ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_radio, ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin_angle - 270,
						((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end_angle + 90, **it, (*it)->GetSize() - 1);
				}
				Direction = *its - ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin;

				if (((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu == 0)//普通圆弧
				{
					pArc->m_begin_angle = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end_angle;
					pArc->m_center = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_center + Direction;
					pArc->m_radio = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_radio;
					pArc->m_Tudu = 0;
					pArc->flag = 2;
				}
				else
				{
					pArc->m_begin = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_begin + Direction;
					pArc->m_end = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_end + Direction;
					pArc->m_Tudu = ((CArc*)(*it)->GetAt((*it)->GetSize() - 1))->m_Tudu;
					pArc->flag = 2;
				}

				m_tempYArc.push_back(tempArc);
			}

			m_ArcFinishYSide.push_back(m_tempYArc);
			vector<CPtrArray*>().swap(m_tempYArc);
		}

		vector<vector<vector<CPoint3D>>>().swap(m_ArcFinishPoints);
		for (vector<CPtrArray*>::iterator it = m_ArcFinishArrayList.begin(); it != m_ArcFinishArrayList.end(); ++it)
		{
			vector<vector<CPoint3D>> m_tempArcFinishPt;
			m_tempArcFinishPt = ArcFinishDiscrete(**it);

			vector<CPoint3D> m_LeftPt, m_DownPt;
			CPoint3D Pt1, Pt2;//设置下刀点，抬刀点,在粗加工，精加工以及粗加工裁剪的初始化都要设计这部分内容

			Pt1.x = m_tempArcFinishPt.begin()->begin()->x;//起点上方
			Pt1.y = m_tempArcFinishPt.begin()->begin()->y;
			Pt1.z = 2500;
			m_DownPt.push_back(Pt1);
			Pt1.x = m_tempArcFinishPt.begin()->begin()->x;//起点
			Pt1.y = m_tempArcFinishPt.begin()->begin()->y;
			Pt1.z = m_tempArcFinishPt.begin()->begin()->z;
			m_DownPt.push_back(Pt1);

			if (m_tempArcFinishPt.size() % 2 != 0)
			{
				Pt2.x = ((m_tempArcFinishPt.end() - 1)->end() - 1)->x;//终点
				Pt2.y = ((m_tempArcFinishPt.end() - 1)->end() - 1)->y;
				Pt2.z = ((m_tempArcFinishPt.end() - 1)->end() - 1)->z;
				m_LeftPt.push_back(Pt2);
				Pt2.x = ((m_tempArcFinishPt.end() - 1)->end() - 1)->x;//终点上方
				Pt2.y = ((m_tempArcFinishPt.end() - 1)->end() - 1)->y;
				Pt2.z = 2500;
				m_LeftPt.push_back(Pt2);
			}
			else
			{

				Pt2.x = ((m_tempArcFinishPt.end() - 1)->begin())->x;//终点
				Pt2.y = ((m_tempArcFinishPt.end() - 1)->begin())->y;
				Pt2.z = ((m_tempArcFinishPt.end() - 1)->begin())->z;
				m_LeftPt.push_back(Pt2);
				Pt2.x = ((m_tempArcFinishPt.end() - 1)->begin())->x;//终点上方
				Pt2.y = ((m_tempArcFinishPt.end() - 1)->begin())->y;
				Pt2.z = 2500;
				m_LeftPt.push_back(Pt2);
			}

			//插入抬刀点，下刀点
			m_tempArcFinishPt.insert(m_tempArcFinishPt.begin(), m_DownPt);
			m_tempArcFinishPt.insert(m_tempArcFinishPt.end(), m_LeftPt);


			m_ArcFinishPoints.push_back(m_tempArcFinishPt);
			vector<vector<CPoint3D>>().swap(m_tempArcFinishPt);
		}


		//设置仿真点
		vector<CPoint3D>().swap(FinishSimulationPoints);//清零
		FinishSimulationPoints = SetArcFinishSimulationPoints(m_ArcFinishPoints);
	}
}
void CToolPath::RenderArcFinish(COpenGLDC* pDC)
{
	for (vector<vector<vector<CPoint3D>>>::iterator it = m_ArcFinishPoints.begin(); it != m_ArcFinishPoints.end(); ++it)
	{
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end() - 1; ++itss)
			{
				pDC->DrawLine(*itss, *(itss + 1));
			}
		}

		//绘制每段之间的连线
		BOOL direction = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin() + 1; its != it->end(); ++its)//去除首尾两段的抬刀和退刀
		{
			if (direction)//终点对终点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its == it->end() - 2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->end() - 1), *((its + 1)->end() - 1));
				}
			}
			else//起点对起点
			{
				if (its == it->end() - 1)//最后一个元素
				{
					pDC->DrawLine(*(its->end() - 1), *(it->begin()->begin()));
				}
				else if (its == it->end() - 2)//倒数第二个元素
				{

				}
				else//其他元素终点对终点
				{
					pDC->DrawLine(*(its->begin()), *((its + 1)->begin()));
				}
			}
			direction = !direction;
		}
	}
}

//设置仿真点
vector<CPoint3D> CToolPath::SetArcRoughSimulationPoints(vector<vector<vector<CPoint3D>>> RoughPoints)
{
	vector<CPoint3D> TempRoughPoints;
	for (vector<vector<vector<CPoint3D>>>::iterator it = RoughPoints.begin(); it != RoughPoints.end(); ++it)
	{
		BOOL m_pointDirection = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			if (its == it->begin())//第一个元素为直线
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					TempRoughPoints.push_back(*itss);
				}
			}
			else if (its == it->end() - 1)//最后一个元素亦为直线
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					TempRoughPoints.push_back(*itss);
				}
				TempRoughPoints.push_back(*(it->begin()->begin()));//最后一点为该层的起始点
			}
			else//中间的元素
			{
				if (m_pointDirection)//顺序
				{
					for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
					{
						TempRoughPoints.push_back(*itss);
					}
				}
				else//反序
				{
					for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
					{
						TempRoughPoints.push_back(*itss);
					}
				}
				m_pointDirection = !m_pointDirection;
			}

		}
	}


	return TempRoughPoints;
}

vector<CPoint3D> CToolPath::SetArcFinishSimulationPoints(vector<vector<vector<CPoint3D>>> FinishPoints)
{
	vector<CPoint3D> TempFinishPoints;
	for (vector<vector<vector<CPoint3D>>>::iterator it = FinishPoints.begin(); it != FinishPoints.end(); ++it)
	{
		BOOL m_pointDirection = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			if (its == it->begin())//第一个元素为直线
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					TempFinishPoints.push_back(*itss);
				}
			}
			else if (its == it->end() - 1)//最后一个元素亦为直线
			{
				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					TempFinishPoints.push_back(*itss);
				}
				TempFinishPoints.push_back(*(it->begin()->begin()));//最后一点为该层的起始点
			}
			else//中间的元素
			{
				if (m_pointDirection)//顺序
				{
					for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)
					{
						TempFinishPoints.push_back(*itss);
					}
				}
				else//反序
				{
					for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
					{
						TempFinishPoints.push_back(*itss);
					}
				}
				m_pointDirection = !m_pointDirection;
			}

		}
	}

	return TempFinishPoints;
}
