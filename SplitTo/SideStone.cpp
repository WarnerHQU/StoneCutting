
#include "stdafx.h"
#include"SideStone.h"

CSideStone::CSideStone()
{
	mark = 0; 
	m_FeedSpeed = 0.0;	
	m_FeedTime = 0.0;
}


CSideStone::~CSideStone()
{

}
//侧面加工
vector<CPoint3D> CSideStone::OffsetAlgorithm(CPtrArray& XFarray, int mark, double offset/*必须是点到直线的距离*/)//侧边只能是垂直于X轴的直线
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
					points[i].begin, points[i].end, m_FeedTime, m_FeedSpeed);

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
void CSideStone::ChangeSideProcess()
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
void CSideStone::RenderSide(COpenGLDC* pDC)
{
	for (int i = 0; i != m_ptArray.GetSize(); ++i)
	{
		if (((CLine*)m_ptArray.GetAt(i))->flag == 1)
		{
			//pDC->DrawLine(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end);

			if (((CLine*)m_ptArray.GetAt(i))->m_begin.z == 0.0 && ((CLine*)m_ptArray.GetAt(i))->m_end.z == 0.0)
			{
				vector<CPoint3D> point = LineDiscrete(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end, m_FeedTime, m_FeedSpeed);

				glBegin(GL_POINTS);
				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
				{
					glVertex3f(it->x, it->y, it->z);
				}
				glEnd();

			}
			else
			{
				vector<CPoint3D> point = LineDiscreteXZ(((CLine*)m_ptArray.GetAt(i))->m_begin, ((CLine*)m_ptArray.GetAt(i))->m_end, m_FeedTime_VX, m_FeedSpeed_VX);
				for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
				{
					double offset;
					offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
					vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);

					for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end() - 1; ++its)
					{
						its->z = it->z;
						(its + 1)->z = it->z;
						pDC->DrawLine(*its, *(its + 1));
					}

				}



			}

		}
		else if (((CArc*)m_ptArray.GetAt(i))->flag == 2)
		{
			if (((CArc*)m_ptArray.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				if (((CArc*)m_ptArray.GetAt(i))->m_center.z == 0.0)//判断是否是在XOY平面上还是不是在XOY平面上
				{
					((CArc*)m_ptArray.GetAt(i))->GLDrawArc(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio, m_ptArray, i);
					vector<CPoint3D> point = ArcDiscrete(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle, m_ptArray, i, m_FeedTime, m_FeedSpeed);

					glBegin(GL_POINTS);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						glVertex3f(it->x, it->y, it->z);
					}
					glEnd();


				}
				else
				{
					((CArc*)m_ptArray.GetAt(i))->GLDrawArcXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio, m_ptArray, i);

					vector<CPoint3D> point = ArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_radio, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
						((CArc*)m_ptArray.GetAt(i))->m_end_angle, m_ptArray, i, m_FeedTime_VX, m_FeedSpeed_VX);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						double offset;
						offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
						vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
						for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end() - 1; ++its)
						{
							its->z = it->z;
							(its + 1)->z = it->z;
							pDC->DrawLine(*its, *(its + 1));
						}
					}
				}
			}
			else//多线段圆弧
			{
				if (((CArc*)m_ptArray.GetAt(i))->m_begin.z == 0.0 && ((CArc*)m_ptArray.GetAt(i))->m_end.z == 0.0)
				{
					((CArc*)m_ptArray.GetAt(i))->PolyCalculate(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
						((CArc*)m_ptArray.GetAt(i))->m_Tudu);

					if (((CArc*)m_ptArray.GetAt(i))->m_Tudu>0)//逆时针
					{
						((CArc*)m_ptArray.GetAt(i))->PolyDrawNArc(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio);
					}
					else//顺时针
					{
						((CArc*)m_ptArray.GetAt(i))->PolyDrawSArc(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio);
					}

					vector<CPoint3D> point = PolyArcDiscrete(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
						((CArc*)m_ptArray.GetAt(i))->m_Tudu, m_FeedTime, m_FeedSpeed);

					glBegin(GL_POINTS);
					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						glVertex3f(it->x, it->y, it->z);
					}
					glEnd();


				}
				else
				{
					((CArc*)m_ptArray.GetAt(i))->PolyCalculateXZ(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
						((CArc*)m_ptArray.GetAt(i))->m_Tudu);

					if (((CArc*)m_ptArray.GetAt(i))->m_Tudu > 0)//逆时针
					{
						((CArc*)m_ptArray.GetAt(i))->PolyDrawNArcXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio);
					}
					else
					{
						((CArc*)m_ptArray.GetAt(i))->PolyDrawSArcXZ(((CArc*)m_ptArray.GetAt(i))->m_center, ((CArc*)m_ptArray.GetAt(i))->m_begin_angle,
							((CArc*)m_ptArray.GetAt(i))->m_end_angle, 0.0001, ((CArc*)m_ptArray.GetAt(i))->m_radio);
					}
					vector<CPoint3D> point = PolyArcDiscreteXZ(((CArc*)m_ptArray.GetAt(i))->m_begin, ((CArc*)m_ptArray.GetAt(i))->m_end,
						((CArc*)m_ptArray.GetAt(i))->m_Tudu, m_FeedTime_VX, m_FeedSpeed_VX);

					for (vector<CPoint3D>::iterator it = point.begin(); it != point.end(); ++it)
					{
						double offset;
						offset = sqrt(pow(it->x - XMax.x, 2) + pow(it->y - XMax.y, 2));
						vector<CPoint3D> OffsetPoint = OffsetAlgorithm(m_ptArray, mark, offset);
						for (vector<CPoint3D>::iterator its = OffsetPoint.begin(); its != OffsetPoint.end() - 1; ++its)
						{
							its->z = it->z;
							(its + 1)->z = it->z;
							pDC->DrawLine(*its, *(its + 1));
						}
					}
				}
			}
		}
		else if (((CCircle*)m_ptArray.GetAt(i))->flag == 3)
		{

		}
	}
}
CPoint3D CSideStone::GetCenterPoint()
{
	CPoint3D center;//存储中心点
	if (this->m_ptArray.IsEmpty() == 0)
	{
		double Minx, Miny, Minz, Maxx, Maxy, Maxz, Mine, Maxe;//用来存放XYZ三个方向的最大最小值
		Minx = ((CLine*)this->m_ptArray.GetAt(0))->m_begin.x;
		Miny = ((CLine*)this->m_ptArray.GetAt(0))->m_begin.y;
		Maxx = ((CLine*)this->m_ptArray.GetAt(0))->m_end.x;
		Maxy = ((CLine*)this->m_ptArray.GetAt(0))->m_end.y;


		Minz = Maxz = 0.0;
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
	return center;
}
