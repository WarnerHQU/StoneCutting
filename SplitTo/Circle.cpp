#include "StdAfx.h"
#include "Circle.h"
#include<math.h>
#define PI 3.1415926
CCircle::CCircle(void)
{
	m_center.x=0;
	m_center.y=0;
	m_center.z=0;
	m_radio=0.0;
	flag=3;
}


CCircle::~CCircle(void)
{
}
void CCircle::GLDrawCircle(CPoint3D center,float radius)
{

	glBegin(GL_LINE_STRIP);//OpenGL要求：指定顶点的命令必须包含在glBegin函数之后，  
	//glEnd函数之前（否则指定的顶点将被忽略）。并由glBegin来指明如何使用这些点  
	//GL_POLYGON表示画多边形（由点连接成多边形）  
	glColor3f(0.0f,0.0f,1.0f);
	for(float a=0; a<2*PI; a+=0.01)  
		glVertex3f(center.x+radius*cos(a), center.y+radius*sin(a),center.z);  
	glEnd();
}

void CCircle::GLDrawCircleXZ(CPoint3D center, float radius)
{

	glBegin(GL_LINE_STRIP);//OpenGL要求：指定顶点的命令必须包含在glBegin函数之后，  
	//glEnd函数之前（否则指定的顶点将被忽略）。并由glBegin来指明如何使用这些点  
	//GL_POLYGON表示画多边形（由点连接成多边形）  
	glColor3f(0.0f, 0.0f, 1.0f);
	for (float a = 0; a<2 * PI; a += 0.01)
		glVertex3f(center.x + radius*cos(a),center.y,center.z + radius*sin(a));
	glEnd();
}
void CCircle::GLDrawCircleYZ(CPoint3D center, float radius)
{

	glBegin(GL_LINE_STRIP);//OpenGL要求：指定顶点的命令必须包含在glBegin函数之后，  
	//glEnd函数之前（否则指定的顶点将被忽略）。并由glBegin来指明如何使用这些点  
	//GL_POLYGON表示画多边形（由点连接成多边形）  
	glColor3f(0.0f, 0.0f, 1.0f);
	for (float a = 0; a<2 * PI; a += 0.01)
		glVertex3f(center.x,center.y + radius*cos(a),  center.z + radius*sin(a));
	glEnd();
}

vector<CPoint3D> CCircle::CircleTDMInterpolation(CPoint3D center, float radius, CPoint3D EnterPoint, float Inter, float F)
{
	
	CPoint3D temp;
	vector<CPoint3D> ArcTDMPos;
	CPoint3D ptBegin;
	CPoint3D ptEnd;

	float m_ptOffsetX;
	float m_ptOffsetY;

	float m_Juge;

	//float feed = F*Inter / 60;//单位是微米级的
	float feed =0.001*F*Inter / 60;//单位是毫米级的
	float data;

	ptBegin = EnterPoint;
	ptEnd = EnterPoint;

	m_ptOffsetX = ptBegin.x - center.x;
	m_ptOffsetY = ptBegin.y - center.y;


	BOOL Direction = TRUE;//用于判断顺时针还是逆时针，这里强制设定为逆时针

	ArcTDMPos.push_back(ptBegin);
	
	
	if (!Direction)//Direcrtion为False时候是顺时针，这里是求顺时针的插补Go2
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetY;//插补起点Y坐标
		float zx = feed*y0 / radius;//起始的增量
		float zy = feed*x0 / radius;//起始的增量

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

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 + zx;
					y1 = sqrt(fabs(radius*radius - x1*x1));
					zy = y0 - y1;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 - zy;
					x1 = sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zy = -0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					if (radius*radius > y1*y1)
						x1 = -sqrt(radius*radius - y1*y1);
					else
						x1 = sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 + zx;
					y1 = sqrt(radius*radius - x1*x1);
					//zy = y0 - y1;
					zy = y1 - y0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 + zx;
					y1 = -sqrt(radius*radius - x1*x1);
					zy = y0 - y1;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 - zy;
					x1 = -sqrt(radius*radius - y1*y1);
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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


					zy = -0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					//判断下以免出现无穷小的情况
					if (radius*radius > y1*y1)
						x1 = sqrt(radius*radius - y1*y1);
					else
						x1 = -sqrt(fabs(radius*radius - y1*y1));
					//x1 = sqrt(fabs(radio*radio - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 + zx;
					y1 = -sqrt(radius*radius - x1*x1);
					zy = y1 - y0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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
			data /= 2;//原本的间距太小了容易跳出循环故边一半试试
		} while (sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2))>=data);
	}

	else//逆时针的插补G03
	{
		float x0 = m_ptOffsetX;//插补起点X坐标
		float y0 = m_ptOffsetY;//插补起点Y坐标
		float zx = feed*y0 / radius;//起始的增量
		float zy = feed*x0 / radius;//起始的增量

		float distance;

		do{
			if (m_ptOffsetX >= 0 && m_ptOffsetY >= 0)
				m_Juge = 1;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY >= 0)
				m_Juge = 2;
			else if (m_ptOffsetX <= 0 && m_ptOffsetY <= 0)
				m_Juge = 3;
			else if (m_ptOffsetX >= 0 && m_ptOffsetY <= 0)
				m_Juge = 4;



			if (m_Juge == 1)//第一象限内******2016.1.1改的第一象限的A用B的方法，B用A的方法******
			{

				if (fabs(m_ptOffsetX) <= fabs(m_ptOffsetY))//用第2种方法B
				{
					float x1;
					float y1;

					/*zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					if (radius*radius >y1*y1)
						x1 = sqrt(radius*radius - y1*y1);
					else
						x1 = -sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;*/

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 - zx;
					if (radius*radius >x1*x1)
						y1 = sqrt(fabs(radius*radius - x1*x1));
					else
						y1 = -sqrt(fabs(radius*radius - x1*x1));
					
					zy = y1 - y0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
					ArcTDMPos.push_back(temp);

					
					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;


				}
				else//用第1种方法A
				{
					float x1;
					float y1;

					/*zx = 0.5*feed*(y0-zy) / radius;
					x1 = x0 - zx;
					y1 = sqrt(fabs(radius*radius - x1*x1));
					zy = y1 - y0;*/

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					x1 = sqrt(radius*radius - y1*y1);
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第1种方法A
				{
					float x1;
					float y1;

					zx = -0.5*feed*(y0 - zy) / radius;
					x1 = x0 + zx;
					y1 = sqrt(fabs(radius*radius - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;
				}
				else//用第2种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					x1 = -sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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
				if (fabs(m_ptOffsetX) < fabs(m_ptOffsetY))//用第2种方法B
				{
					float x1;
					float y1;

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					if (radius*radius >y1*y1)
						x1 = -sqrt(fabs(radius*radius - y1*y1));
					else
						x1 = sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
					ArcTDMPos.push_back(temp);

					//插补点做变换
					y0 = y1;
					x0 = x1;

					//判断象限点进行变化
					m_ptOffsetX = x0;
					m_ptOffsetY = y0;

				}
				else//用第1种方法A
				{
					float x1;
					float y1;

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(radius*radius - x1*x1));
					zy = y1 - y0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zx = 0.5*feed*(y0 - zy) / radius;
					x1 = x0 - zx;
					y1 = -sqrt(fabs(radius*radius - x1*x1));
					zy = y1 - y0;


					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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

					zy = 0.5*feed*(x0 + zx) / radius;
					y1 = y0 + zy;
					x1 = sqrt(fabs(radius*radius - y1*y1));
					zx = x1 - x0;

					temp.x = x1 + center.x;
					temp.y = y1 + center.y;
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
			data /= 2;//原本的间距太小了容易跳出循环故边一半试试

			distance = sqrt(pow(temp.x - ptEnd.x, 2) + pow(temp.y - ptEnd.y, 2));
		} while (distance>=data);


	}




	ArcTDMPos.push_back(ptEnd);


	for (vector<CPoint3D>::iterator it = ArcTDMPos.begin(); it != ArcTDMPos.end(); ++it)
	{
		it->z = center.z;
	}


	return ArcTDMPos;
}