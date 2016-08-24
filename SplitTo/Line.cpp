#include "StdAfx.h"
#include "Line.h"


CLine::CLine(void)
{
	m_begin.x=0;
	m_begin.y=0;
	m_begin.z=0;
	m_end.x=0;
	m_end.y=0;
	m_end.z=0;
	flag=1;

	sum=0.0;

}


CLine::~CLine(void)
{
}



	
double CLine::Length(CPoint3D m_begin, CPoint3D m_end)
{
	double length;
	length = sqrt(pow(m_end.x - m_begin.x, 2) + pow(m_end.y - m_begin.y, 2) + pow(m_end.z - m_begin.z, 2));


	return length;
}

CPoint3D CLine::IntersectOfLine(const CPoint3D& pt0, const CVector3D& vec0, const CPoint3D& pt1, const CVector3D& vec1)
{

	
	//如果两条直线平行，求交失败，直接返回FALSE
	if (_IsParallel(vec0, vec1))
		return FALSE;

	//为两条直线配置方程系数
	double a[2], b[2], c[2];
	a[0] = CMatrix2D::GetValue(pt0.y, 1, (pt0 + vec0).y, 1);
	b[0] = -1 * CMatrix2D::GetValue(pt0.x, 1, (pt0 + vec0).x, 1);
	c[0] = CMatrix2D::GetValue(pt0.x, pt0.y, (pt0 + vec0).x, (pt0 + vec0).y);

	a[1] = CMatrix2D::GetValue(pt1.y, 1, (pt1 + vec1).y, 1);
	b[1] = -1 * CMatrix2D::GetValue(pt1.x, 1, (pt1 + vec1).x, 1);
	c[1] = CMatrix2D::GetValue(pt1.x, pt1.y, (pt1 + vec1).x, (pt1+ vec1).y);


	double m = CMatrix2D::GetValue(a[0], b[0], a[1], b[1]);
	if (IS_ZERO(m))
		return FALSE;

	//计算交点
	CPoint3D point;
	point.x = -1 * CMatrix2D::GetValue(c[0], b[0], c[1], b[1])/m;
	point.y = -1 * CMatrix2D::GetValue(a[0], c[0], a[1], c[1]) / m;
	point.z = 0;

	return point;
}

CPoint3D CLine::IntersectOfArc(CPoint3D& LineBegin, CPoint3D& LineEnd, CPoint3D& center, float radio, int mark, CPtrArray& XFarray, int index)
{
	CPoint3D pt1;
	double a, b, c,m;//圆弧求交ax2+bx+c=0
	double k, t;//直线y=kx+t
	//BOOL Direction=((CArc*)XFarray.GetAt(index))->Calculate(center, radio, beginAngle, endAngle, XFarray, index);
	int flag = 0;;
	if (LineBegin.y == LineEnd.y)
	{
		flag = 3;
		pt1.y = LineBegin.y;

	}
	else if (LineBegin.x == LineEnd.x)
	{
		flag = 2;
		pt1.x = LineBegin.x;
	}
	else
	{
		flag = 1;
		k = (LineBegin.y - LineEnd.y) / (LineBegin.x - LineEnd.x);
		t = LineBegin.y - k*LineBegin.x;
	}

	if (flag==1)
	{

		a = pow(k, 2)+1;
		b = -2 * center.x + 2 * k*t - 2 * k*center.y;
		c = pow(center.x, 2) + pow(t, 2) - 2 * t*center.y + pow(center.y, 2) - pow(radio, 2);

		m = pow(b, 2) - 4 * a*c;

		if (m == 0)
		{
			pt1.x = -1 * b / (2 * a);
			pt1.y = k*pt1.x + t;
		}
		else if (m > 0)
		{
			CPoint3D s1, s2;//存储求解的点
			double dis1, dis2;
			s1.x = (-1 * b + sqrt(m)) / (2 * a);
			s1.y = k*s1.x + t;
			s2.x = (-1 * b - sqrt(m)) / (2 * a);
			s2.y = k*s2.x + t;

			if (index != mark)//如果圆弧不是最后一个元素
			{

				dis1 = Length(s1, LineEnd);
				dis2 = Length(s2, LineEnd);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}
			else if (index == mark)
			{
				dis1 = Length(s1, LineBegin);
				dis2 = Length(s2, LineBegin);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}

		}
		else
		{
			return FALSE;
		}
	}
	else if(flag==2)//直线与X轴垂直
	{
		CPoint3D s1, s2;//存储求解的点
		double dis1, dis2;
		s1.x = s2.x = LineBegin.x;

		if (sqrt(pow(radio, 2) - pow(pt1.x - center.x, 2)) >= 0)
		{

			s1.y = center.y + sqrt(pow(radio, 2) - pow(pt1.x - center.x, 2));
			s2.y = center.y - sqrt(pow(radio, 2) - pow(pt1.x - center.x, 2));

			if (index != mark)//如果圆弧不是最后一个元素
			{

				dis1 = Length(s1, LineEnd);
				dis2 = Length(s2, LineEnd);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}
			else if (index == mark)
			{
				dis1 = Length(s1, LineBegin);
				dis2 = Length(s2, LineBegin);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}

		}
		else
		{
			return FALSE;
		}
		
	}
	else//与Y轴垂直
	{

		CPoint3D s1, s2;//存储求解的点
		double dis1, dis2;
		s1.y = s2.y = LineBegin.y;

		if (sqrt(pow(radio, 2) - pow(pt1.y - center.y, 2)) >= 0)
		{

			s1.x = center.x + sqrt(pow(radio, 2) - pow(pt1.y - center.y, 2));
			s2.x = center.x - sqrt(pow(radio, 2) - pow(pt1.y - center.y, 2));

			if (index != mark)//如果圆弧不是最后一个元素
			{

				dis1 = Length(s1, LineEnd);
				dis2 = Length(s2, LineEnd);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}
			else if (index == mark)
			{
				dis1 = Length(s1, LineBegin);
				dis2 = Length(s2, LineBegin);
				if (dis1 < dis2)
					pt1 = s1;
				else
					pt1 = s2;
			}

		}
		else
		{
			return FALSE;
		}
	}

	return pt1;
}


vector<CPoint3D> CLine::LineTDMInterpolation(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F)
{
	//float feed = F*Inter / 60;//单位是微米级的
	float feed = 0.001*F*Inter / 60;//单位是毫米级的

	CPoint3D temp;
	vector<CPoint3D> LineTDMPos;
	LineTDMPos.push_back(ptBegin);
	
	double m_OffsetX = ptEnd.x - ptBegin.x;
	double m_OffsetY = ptEnd.y - ptBegin.y;

	double TanA, CosA;

	if (fabs(m_OffsetX) > fabs(m_OffsetY))
	{
		TanA = m_OffsetY / m_OffsetX;
		CosA = m_OffsetX / sqrt(pow(m_OffsetX, 2) + pow(m_OffsetY, 2));

	}
	else
	{
		TanA = m_OffsetX / m_OffsetY;
		CosA = m_OffsetY / sqrt(pow(m_OffsetX, 2) + pow(m_OffsetY, 2));
	}

	int m_Juge=0 ;/*20160323*/

	if (m_OffsetX >= 0 && m_OffsetY >= 0)
		m_Juge = 1;
	else if (m_OffsetX <= 0 && m_OffsetY >= 0)
		m_Juge = 2;
	else if (m_OffsetX <= 0 && m_OffsetY <= 0)
		m_Juge = 3;
    else if (m_OffsetX >= 0 && m_OffsetY <= 0)
		m_Juge = 4;

	if (m_Juge == 1)//终点相对于起点在第一象限
	{
		if (fabs(m_OffsetY) >=fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;


			} while (sqrt(pow(temp.x-ptEnd.x,2)+pow(temp.y-ptEnd.y,2))>feed*CosA);
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1,yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>feed*CosA);
		}
	}
	else if (m_Juge == 2)//终点相对于起点在第2象限
	{
		if (fabs(m_OffsetY) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 3)//终点相对于起点在第3象限
	{
		if (fabs(m_OffsetY) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 4)//终点相对于起点在第4象限
	{
		if (fabs(m_OffsetY) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.y = ptBegin.y + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>fabs(feed*CosA));
		}
	}

	LineTDMPos.push_back(ptEnd);

	for (vector<CPoint3D>::iterator it = LineTDMPos.begin(); it != LineTDMPos.end(); ++it)
	{
		it->z = ptBegin.z;
	}

	return LineTDMPos;
}


vector<CPoint3D> CLine::LineTDMInterpolationXZ(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F)
{
	//float feed = F*Inter / 60;//单位是微米级的
	float feed = 0.001*F*Inter / 60;//单位是米级的

	CPoint3D temp;
	vector<CPoint3D> LineTDMPos;
	LineTDMPos.push_back(ptBegin);

	double m_OffsetX = ptEnd.x - ptBegin.x;
	double m_OffsetZ = ptEnd.z - ptBegin.z;

	double TanA, CosA;

	if (fabs(m_OffsetX) > fabs(m_OffsetZ))
	{
		TanA = m_OffsetZ / m_OffsetX;
		CosA = m_OffsetX / sqrt(pow(m_OffsetX, 2) + pow(m_OffsetZ, 2));

	}
	else
	{
		TanA = m_OffsetX / m_OffsetZ;
		CosA = m_OffsetZ / sqrt(pow(m_OffsetX, 2) + pow(m_OffsetZ, 2));
	}

	int m_Juge;

	if (m_OffsetX >= 0 && m_OffsetZ >= 0)
		m_Juge = 1;
	else if (m_OffsetX <= 0 && m_OffsetZ >= 0)
		m_Juge = 2;
	else if (m_OffsetX <= 0 && m_OffsetZ <= 0)
		m_Juge = 3;
	else if (m_OffsetX >= 0 && m_OffsetZ <= 0)
		m_Juge = 4;

	if (m_Juge == 1)//终点相对于起点在第一象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;


			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>feed*CosA);
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>feed*CosA);
		}
	}
	else if (m_Juge == 2)//终点相对于起点在第2象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 3)//终点相对于起点在第3象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 4)//终点相对于起点在第4象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetX))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.x = ptBegin.x + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}

	LineTDMPos.push_back(ptEnd);

	for (vector<CPoint3D>::iterator it = LineTDMPos.begin(); it != LineTDMPos.end(); ++it)
	{
		it->y = ptBegin.y;
	}

	return LineTDMPos;
}

vector<CPoint3D> CLine::LineTDMInterpolationYZ(CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F)
{

	//float feed = F*Inter / 60;//单位是微米级的
	float feed = 0.001*F*Inter / 60;//单位是米级的

	CPoint3D temp;
	vector<CPoint3D> LineTDMPos;
	LineTDMPos.push_back(ptBegin);

	double m_OffsetY = ptEnd.y - ptBegin.y;
	double m_OffsetZ = ptEnd.z - ptBegin.z;

	double TanA, CosA;

	if (fabs(m_OffsetY) > fabs(m_OffsetZ))
	{
		TanA = m_OffsetZ / m_OffsetY;
		CosA = m_OffsetY / sqrt(pow(m_OffsetY, 2) + pow(m_OffsetZ, 2));

	}
	else
	{
		TanA = m_OffsetY / m_OffsetZ;
		CosA = m_OffsetZ / sqrt(pow(m_OffsetY, 2) + pow(m_OffsetZ, 2));
	}

	int m_Juge;

	if (m_OffsetY >= 0 && m_OffsetZ >= 0)
		m_Juge = 1;
	else if (m_OffsetY <= 0 && m_OffsetZ >= 0)
		m_Juge = 2;
	else if (m_OffsetY <= 0 && m_OffsetZ <= 0)
		m_Juge = 3;
	else if (m_OffsetY >= 0 && m_OffsetZ <= 0)
		m_Juge = 4;

	if (m_Juge == 1)//终点相对于起点在第一象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetY))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;


			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>feed*CosA);
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>feed*CosA);
		}
	}
	else if (m_Juge == 2)//终点相对于起点在第2象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetY))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 3)//终点相对于起点在第3象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetY))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}
	else if (m_Juge == 4)//终点相对于起点在第4象限
	{
		if (fabs(m_OffsetZ) >= fabs(m_OffsetY))//第二种算法
		{
			float yz = feed*CosA;
			float  y1, x1, xz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				y1 = y0 + yz;//象限中对应的点的Y坐标
				x1 = y1*TanA;//象限中对应的点的X坐标
				xz = x1 - x0;//X轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
		else//第一种算法
		{
			float xz = feed*CosA;
			float  y1, x1, yz;
			float x0 = 0.0;
			float y0 = 0.0;

			do
			{
				x1 = x0 + xz;//象限中对应的点的X坐标
				y1 = x1*TanA;//象限中对应的点的Y坐标
				yz = y1 - y0;//Y轴增量

				temp.y = ptBegin.y + x1;
				temp.z = ptBegin.z + y1;

				LineTDMPos.push_back(temp);

				x0 = x1;
				y0 = y1;

			} while (sqrt(pow(temp.y - ptEnd.y, 2) + pow(temp.z - ptEnd.z, 2))>fabs(feed*CosA));
		}
	}

	LineTDMPos.push_back(ptEnd);

	for (vector<CPoint3D>::iterator it = LineTDMPos.begin(); it != LineTDMPos.end(); ++it)
	{
		it->x = ptBegin.x;
	}

	return LineTDMPos;
}