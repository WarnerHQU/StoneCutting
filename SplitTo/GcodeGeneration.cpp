#include "stdafx.h"
#include "GcodeGeneration.h"


CGcodeGeneration::CGcodeGeneration(CToolPath* tool)
{
	pTool=tool;
	FLAG = "";
}


CGcodeGeneration::~CGcodeGeneration()
{
}

void CGcodeGeneration::Init()
{
	if (pTool->GetRotateArray().IsEmpty()==0)//旋转加工
	{
		FLAG = _T("Rotate");
	}
	if (pTool->GetArray().IsEmpty() == 0)//侧面加工
	{
		FLAG = _T("Side");
	}
	if (pTool->GetArcArray().IsEmpty()== 0)//双截面加工
	{
		FLAG = _T("ARC");
	}
	if (pTool->GetLineArray().IsEmpty() == 0)//线条加工
	{
		FLAG = _T("LineProcess");
	}
	if (pTool->RenderOrthgonalInit())
	{
		FLAG = _T("Orthgonal");
	}
	if (pTool->GetPolygeonPolyLine().size()!=0)
	{
		FLAG = _T("Polygon");
	}
	if (CTableBoardProcess::GetTableProcessInstance()->GetBoardArray() != NULL)
	{
		FLAG = _T("TableBoard");
	}
}

void CGcodeGeneration::OutPutNcCode()//把G代码存起来
{
	CFileDialog openfile(FALSE, NULL, _T("*.nc"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL, NULL);
	openfile.m_ofn.lpstrTitle = _T("谢峰NC代码保存窗口");

	openfile.m_ofn.lpstrFilter = _T("文本文件(.txt)\0*.txt\0dxf文件(.dxf)\0*.dxf\0NC文件(.nc)\0*.nc\0所有文件(*.*)\0*.*\0\0");

	if (IDOK == openfile.DoModal())
	{
		ofstream outfile;//数据写入
		outfile.open(openfile.GetPathName()); //myfile.bat是存放数据的文件名  
		if (outfile.is_open())
		{
			
			if (FLAG == "Rotate")
			{
				//outfile << "Hello!" << endl;    //message是程序中处理的数据 
				RotateNC(&outfile);
			}
			else if (FLAG == "Side")
			{
				SideNC(&outfile);
			}
			else if (FLAG == "ARC")
			{
				ArcNC(&outfile);
			}
			else if (FLAG == "LineProcess")
			{
				LineNC(&outfile);
			}
			else if (FLAG == "Orthgonal")
			{
				OrthgonalNC(&outfile);
			}
			else if (FLAG == "Polygon")
			{
				PolygonNC(&outfile);
			}
			else if (FLAG == "TableBoard")
			{
				TableBoardNC(&outfile);
			}
			outfile.close();
			
		}
		else
		{
			AfxMessageBox(_T("不能保存文件"));
		}
	}

}
//旋转加工NC代码的生成
void CGcodeGeneration::RotateNC(ofstream* of)
{
	//(*of) << "Hello" << endl;
	(*of) << "旋转加工粗加工NC代码：" << endl;
	vector<vector<vector<CPoint3D>>> temp = pTool->GetRotateRoughCutPoints();//获取粗加工裁剪后的点
	vector<vector<vector<CPoint3D>>> tempRoughPoints = pTool->GetRotateRoughPoints();//获取粗加工未经裁剪后的点
	vector<vector<vector<CPoint3D>>> tempFinishPoints = pTool->GetRotateFinishPoints();//获取精加工点


	CPoint3D CircleCenter = ((CCircle*)pTool->GetRotateArray().GetAt(0))->m_center;//获得圆心坐标

	vector<vector<vector<CPoint3D>>>::iterator itRough = tempRoughPoints.begin();
	for (vector<vector<vector<CPoint3D>>>::iterator it = temp.begin(); it != temp.end(); ++it)
	{
		
		vector<vector<CPoint3D>>::reverse_iterator itsRough = itRough->rbegin();
		BOOL Direction = TRUE;
		for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
		{
			vector<CPoint3D>::iterator itssRough = itsRough->begin();//粗加工未经裁剪的初始点

			if (Direction)//假设默认逆时针
			{
				if (its == it->rbegin())//每一层的第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						CircleCenter.z = itss->z;
						//距离不对
						//double tempRadius = CLine::Length(*itss, CircleCenter);
						double tempRadius = CLine::Length(*itssRough, CircleCenter);
						CString str;
						//点定位到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						//点定位刀圆插补的起点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss ->x, itss->y, itss->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						CPtrArray* Array=new CPtrArray;
						for (int j = 0; j != RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetSize(); ++j)
						{
							if (((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 1)
							{
								CLine* pLine = new CLine;
								Array->Add(pLine);
								pLine->m_begin = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pLine->m_end = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
							}
							else if (((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 2)
							{
								CArc* pArc = new CArc;
								Array->Add(pArc);
								pArc->m_begin = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pArc->m_end = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
								pArc->m_center = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_center;
								pArc->m_radio = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_radio;
							}
						}
						if (Array->IsEmpty() == 0)//非空
						{
							for (int i = 0; i != Array->GetSize(); ++i)
							{
								if (((CLine*)Array->GetAt(i))->flag==1)//直线
								{
									str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x, ((CLine*)Array->GetAt(i)) ->m_end.y,
										                         ((CLine*)Array->GetAt(i))->m_end.z);
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else if (((CArc*)Array->GetAt(i))->flag == 2)//圆弧
								{
									str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_end.y,
										((CArc*)Array->GetAt(i))->m_end.z, ((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_begin.x,
										((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_begin.y);
									//可以实现输出
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
							}
						}
						else//空了就进行整圆插补
						{
							//开整圆插补(需要分辨是直线还是圆弧然后进行直线插补或者是圆弧插补)
							str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
								CircleCenter.x - itss->x, CircleCenter.y - itss->y);
							//可以实现输出
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}

						//回到抬刀点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;

					}
				}
				else//非每一层的第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						CircleCenter.z = itss->z;
						//double tempRadius = CLine::Length(*itss, CircleCenter);
						double tempRadius = CLine::Length(*itssRough, CircleCenter);
						CString str;
						//点定位到抬刀处
						str.Format(_T("G00 Z%0.3f"), (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//点定位刀圆插补的起点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//CPtrArray* Array=RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth());
						CPtrArray* Array = new CPtrArray;
						//将CPtrArray里的值弄出来
						for (int j = 0; j != RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetSize(); ++j)
						{
							if (((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 1)
							{
								CLine* pLine = new CLine;
								Array->Add(pLine);
								pLine->m_begin = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pLine->m_end = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
							}
							else if (((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 2)
							{
								CArc* pArc = new CArc;
								Array->Add(pArc);
								pArc->m_begin = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pArc->m_end = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
								pArc->m_center = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_center;
								pArc->m_radio = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_radio;
							}
						}

						if (Array->IsEmpty() == 0)//非空
						{
							for (int i = 0; i != Array->GetSize(); ++i)
							{
								if (((CLine*)Array->GetAt(i))->flag == 1)//直线
								{
									str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x, ((CLine*)Array->GetAt(i))->m_end.y,
										((CLine*)Array->GetAt(i))->m_end.z);
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else if (((CArc*)Array->GetAt(i))->flag == 2)//圆弧
								{
									str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_end.y,
										((CArc*)Array->GetAt(i))->m_end.z, ((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_begin.x,
										((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_begin.y);
									//可以实现输出
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
							}
						}
						else
						{
							//开始整圆插补
							str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
								CircleCenter.x - itss->x, CircleCenter.y - itss->y);
							//可以实现输出
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}

						//回到抬刀点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
			}
			else//反向
			{
				if (its == it->rbegin())//每一层的第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						CircleCenter.z = itss->z;
						//double tempRadius = CLine::Length(*itss, CircleCenter);
						double tempRadius = CLine::Length(*itssRough, CircleCenter);
						CString str;
						//点定位到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//点定位刀圆插补的起点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;


						CPtrArray* Array = new CPtrArray;
						for (int j = 0; j != RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetSize(); ++j)
						{
							if (((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 1)
							{
								CLine* pLine = new CLine;
								Array->Add(pLine);
								pLine->m_begin = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pLine->m_end = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
							}
							else if (((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 2)
							{
								CArc* pArc = new CArc;
								Array->Add(pArc);
								pArc->m_begin = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pArc->m_end = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
								pArc->m_center = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_center;
								pArc->m_radio = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_radio;
							}
						}
						if (Array->IsEmpty() == 0)//非空
						{
							for (int i = Array->GetSize()-1; i != -1; --i)
							{
								if (((CLine*)Array->GetAt(i))->flag == 1)//直线
								{
									str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x, ((CLine*)Array->GetAt(i))->m_begin.y,
										((CLine*)Array->GetAt(i))->m_begin.z);
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else if (((CArc*)Array->GetAt(i))->flag == 2)//圆弧
								{
									str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_begin.y,
										((CArc*)Array->GetAt(i))->m_begin.z, ((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_end.x,
										((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_end.y);
									//可以实现输出
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
							}
						}
				        else
				       {
					       //开始整圆插补
					       str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
						    CircleCenter.x - itss->x, CircleCenter.y - itss->y);
					        //可以实现输出
					        STR = CStringA(str);
					         (*of) << STR.c_str() << endl;
				        }

						//回到抬刀点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
				else//非每一层的第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						CircleCenter.z = itss->z;
						//double tempRadius = CLine::Length(*itss, CircleCenter);
						double tempRadius = CLine::Length(*itssRough, CircleCenter);

						CString str;
						//点定位到抬刀处
						str.Format(_T("G00 Z%0.3f"), (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//点定位刀圆插补的起点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;


						CPtrArray* Array = new CPtrArray;
						for (int j = 0; j != RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetSize(); ++j)
						{
							if (((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 1)
							{
								CLine* pLine = new CLine;
								Array->Add(pLine);
								pLine->m_begin = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pLine->m_end = ((CLine*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
							}
							else if (((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->flag == 2)
							{
								CArc* pArc = new CArc;
								Array->Add(pArc);
								pArc->m_begin = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_begin;
								pArc->m_end = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_end;
								pArc->m_center = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_center;
								pArc->m_radio = ((CArc*)RotateCalculate(CircleCenter, tempRadius, pTool->GetRotateBlankLongth(), pTool->GetRotateBlankWidth())->GetAt(j))->m_radio;
							}
						}
						if (Array->IsEmpty() == 0)//非空
						{
							for (int i = Array->GetSize() - 1; i != -1; --i)
							{
								if (((CLine*)Array->GetAt(i))->flag == 1)//直线
								{
									str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x, ((CLine*)Array->GetAt(i))->m_begin.y,
										((CLine*)Array->GetAt(i))->m_begin.z);
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else if (((CArc*)Array->GetAt(i))->flag == 2)//圆弧
								{
									str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_begin.y,
										((CArc*)Array->GetAt(i))->m_begin.z, ((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_end.x,
										((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_end.y);
									//可以实现输出
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
							}
						}
						else
						{
							//开始整圆插补
							str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
								CircleCenter.x - itss->x, CircleCenter.y - itss->y);
							//可以实现输出
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}

						//回到抬刀点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
			}
			Direction = !Direction;

			++itsRough;//粗加工每层从上往下移动
		}

		++itRough;//粗加工层数向内移动
	}

	(*of) << "旋转加工精加工NC代码：" << endl;
	for (vector<vector<vector<CPoint3D>>>::iterator it = tempFinishPoints.begin(); it != tempFinishPoints.end(); ++it)
	{
		BOOL Direction = TRUE;
		for (vector<vector<CPoint3D>>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
		{
			if (Direction)//按照原先的方向顺序
			{
				if (its == it->rbegin())//第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						//点定位到抬刀点
						CString str;
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						//点定位到切入点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//开始整圆插补
						str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
							CircleCenter.x - itss->x, CircleCenter.y - itss->y);
						//可以实现输出
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//回到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
				else//不是第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						//点定位到抬刀点
						CString str;
						str.Format(_T("G00 Z%0.3f"),(itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						//点定位到切入点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						 STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//开始整圆插补
						str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
							CircleCenter.x - itss->x, CircleCenter.y - itss->y);
						//可以实现输出
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//回到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
			}
			else//反向
			{
				if (its == it->rbegin())//方向上如果在第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						//点定位到抬刀点
						CString str;
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						//点定位到切入点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//开始整圆插补
						str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
							CircleCenter.x - itss->x, CircleCenter.y - itss->y);
						//可以实现输出
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//回到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}

				}
				else//不在第一圈
				{
					for (vector<CPoint3D>::iterator itss = its->begin() + 1; itss != its->begin() + 2; ++itss)
					{
						//点定位到抬刀点
						CString str;
						str.Format(_T("G00 Z%0.3f"), (itss - 1)->z);
						std::string STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						//点定位到切入点
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), itss->x, itss->y, itss->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//开始整圆插补
						str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), itss->x, itss->y, itss->z,
							CircleCenter.x - itss->x, CircleCenter.y - itss->y);
						//可以实现输出
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						//回到抬刀处
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), (itss - 1)->x, (itss - 1)->y, (itss - 1)->z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
			}
			Direction = !Direction;
		}
	}
}

CPtrArray* CGcodeGeneration::RotateCalculate(CPoint3D circlecenter, float circleradius, float workBlankLong, float workBlankWidth)
{
	CPtrArray tempArray;
	CPoint3D EnterPoint;

	//求取各个顶点的坐标值
	CPoint3D CornerPointRT;//右上角的顶点
	CornerPointRT.x = circlecenter.x + workBlankLong / 2;
	CornerPointRT.y = circlecenter.y + workBlankWidth / 2;
	CornerPointRT.z = circlecenter.z;

	CPoint3D CornerPointRB;//右下角的顶点
	CornerPointRB.x = circlecenter.x + workBlankLong / 2;
	CornerPointRB.y = circlecenter.y - workBlankWidth / 2;
	CornerPointRB.z = circlecenter.z;

	CPoint3D CornerPointLT;//左上角的顶点
	CornerPointLT.x = circlecenter.x - workBlankLong / 2;
	CornerPointLT.y = circlecenter.y + workBlankWidth / 2;
	CornerPointLT.z = circlecenter.z;

	CPoint3D CornerPointLB;//左下角的顶点
	CornerPointLB.x = circlecenter.x - workBlankLong / 2;
	CornerPointLB.y = circlecenter.y - workBlankWidth / 2;
	CornerPointLB.z = circlecenter.z;

	float Distance = CLine::Length(circlecenter, CornerPointRT);//获取圆心到右上角的顶点的距离

	float min;//比较获取较小值并存储起来
	float max;
	if (workBlankLong > workBlankWidth)
	{
		min = workBlankWidth/2;
		max = workBlankLong/2;
	}
	else
	{
		min = workBlankLong/2;
		max = workBlankWidth/2;
	}

	if (circleradius > min&&circleradius<max)//说明圆与矩形是有交点的,但是只有长或者宽与圆有交点
	{
		if (min == workBlankLong/2)
		{
			float Juge1 = pow(circleradius, 2) - pow(CornerPointRT.x - circlecenter.x, 2);
			if (Juge1 >= 0)
			{
				vector<sort> pos;
				sort pts;
				//直线
				pts.begin.x = CornerPointRB.x;
				pts.begin.y = circlecenter.y;
				pts.begin.z = CornerPointRB.z;
				pts.end.x = CornerPointRB.x;
				pts.end.y = circlecenter.y + sqrt(Juge1);
				pts.end.z = CornerPointRT.z;
				pts.flag = 1;
				pos.push_back(pts);

				//圆弧
				pts.begin.x = CornerPointRB.x;
				pts.begin.y = circlecenter.y + sqrt(Juge1);
				pts.begin.z = CornerPointRT.z;
				pts.end.x = CornerPointLT.x;
				pts.end.y = circlecenter.y + sqrt(Juge1);
				pts.end.z = CornerPointLT.z;
				pts.flag = 2;
				pos.push_back(pts);
				//直线
				pts.begin.x = CornerPointLT.x;
				pts.begin.y = circlecenter.y + sqrt(Juge1);
				pts.begin.z = CornerPointLT.z;
				pts.end.x = CornerPointLB.x;
				pts.end.y = circlecenter.y - sqrt(Juge1);
				pts.end.z = CornerPointLB.z;
				pts.flag = 1;
				pos.push_back(pts);
				//圆弧
				pts.begin.x = CornerPointLB.x;
				pts.begin.y = circlecenter.y - sqrt(Juge1);
				pts.begin.z = CornerPointLB.z;
				pts.end.x = CornerPointRB.x;
				pts.end.y = circlecenter.y - sqrt(Juge1);
				pts.end.z = CornerPointRB.z;
				pts.flag = 2;
				pos.push_back(pts);
				//直线
				pts.begin.x = CornerPointRB.x;
				pts.begin.y = circlecenter.y - sqrt(Juge1);
				pts.begin.z = CornerPointRB.z;
				pts.end.x = CornerPointRB.x;
				pts.end.y = circlecenter.y;
				pts.end.z = CornerPointRB.z;
				pts.flag = 1;
				pos.push_back(pts);

				for (vector<sort>::iterator it = pos.begin(); it != pos.end(); ++it)
				{
					if (it->flag == 1)
					{
						CLine* pLine = new CLine;
						tempArray.Add(pLine);
						pLine->m_begin = it->begin;
						pLine->m_end = it->end;

					}
					else
					{
						CArc* pArc = new CArc;
						tempArray.Add(pArc);
						pArc->m_begin = it->begin;
						pArc->m_end = it->end;
						pArc->m_center = circlecenter;
						pArc->m_radio = circleradius;
					}
				}
			}

		}
		else if (min==workBlankWidth/2)
		{
			float Juge2 = pow(circleradius, 2) - pow(CornerPointRT.y - circlecenter.y, 2);
			if (Juge2 >= 0)
			{
				vector<sort> pos;
				sort pts;
				//圆弧
				pts.begin.x = circlecenter.x + circleradius;
				pts.begin.y = circlecenter.y;
				pts.begin.z = circlecenter.z;
				pts.end.x = circlecenter.x + sqrt(Juge2);
				pts.end.y = CornerPointRT.y;
				pts.end.z = CornerPointRT.z;
				pts.flag = 2;
				pos.push_back(pts);
				//直线
				pts.begin.x = circlecenter.x + sqrt(Juge2);
				pts.begin.y = CornerPointRT.y;
				pts.begin.z = CornerPointRT.z;
				pts.end.x = circlecenter.x - sqrt(Juge2);
				pts.end.y = CornerPointLT.y;
				pts.end.z = CornerPointLT.z;
				pts.flag = 1;
				pos.push_back(pts);
				//圆弧
				pts.begin.x = circlecenter.x - sqrt(Juge2);
				pts.begin.y = CornerPointLT.y;
				pts.begin.z = CornerPointLT.z;
				pts.end.x = circlecenter.x - sqrt(Juge2);
				pts.end.y = CornerPointLB.y;
				pts.end.z = CornerPointLB.z;
				pts.flag = 2;
				pos.push_back(pts);
				//直线
				pts.begin.x = circlecenter.x - sqrt(Juge2);
				pts.begin.y = CornerPointLB.y;
				pts.begin.z = CornerPointLB.z;
				pts.end.x = circlecenter.x + sqrt(Juge2);
				pts.end.y = CornerPointRB.y;
				pts.end.z = CornerPointRB.z;
				pts.flag = 1;
				pos.push_back(pts);
				//圆弧
				pts.begin.x = circlecenter.x + sqrt(Juge2);
				pts.begin.y = CornerPointRB.y;
				pts.begin.z = CornerPointRB.z;
				pts.end.x = circlecenter.x + circleradius;
				pts.end.y = circlecenter.y;
				pts.end.z = circlecenter.z;
				pts.flag = 2;
				pos.push_back(pts);

				for (vector<sort>::iterator it = pos.begin(); it != pos.end(); ++it)
				{
					if (it->flag == 1)
					{
						CLine* pLine = new CLine;
						tempArray.Add(pLine);
						pLine->m_begin = it->begin;
						pLine->m_end = it->end;

					}
					else
					{
						CArc* pArc = new CArc;
						tempArray.Add(pArc);
						pArc->m_begin = it->begin;
						pArc->m_end = it->end;
						pArc->m_center = circlecenter;
						pArc->m_radio = circleradius;
					}
				}

			}
		}
	}
	else if (circleradius >= max&&circleradius<Distance)//半径的长度还未到达覆盖顶点的长度，与两边全部都有交点
	{
		vector<sort> pos;
		float Juge1 = pow(circleradius, 2) - pow(CornerPointRT.x - circlecenter.x, 2);//为右侧切入点为起点计算，圆与直线求交点，这个是与垂直线,可根据对称求取两外两条线与圆交点
		float Juge2 = pow(circleradius, 2) - pow(CornerPointRT.y - circlecenter.y, 2);//圆与直线求交点，这个是与水平线
		if (Juge1 >= 0 && Juge2 >= 0)//有解的情况下求取
		{
			sort pts;
			//直线侧面
			pts.begin.x = CornerPointRT.x;
			pts.begin.y = circlecenter.y;
			pts.begin.z = CornerPointRT.z;
			pts.end.x = CornerPointRT.x;
			pts.end.y = circlecenter.y + sqrt(Juge1);
			pts.end.z = CornerPointRT.z;
			pts.flag = 1;//表示直线
			pos.push_back(pts);
			//圆弧段
			pts.begin.x = CornerPointRT.x;
			pts.begin.y = circlecenter.y + sqrt(Juge1);
			pts.begin.z = CornerPointRT.z;
			pts.end.x = circlecenter.x + sqrt(Juge2);
			pts.end.y = CornerPointRT.y;
			pts.end.z = CornerPointRT.z;
			pts.flag = 2;//表示圆弧
			pos.push_back(pts);

			//直线段顶面
			pts.begin.x = circlecenter.x + sqrt(Juge2);
			pts.begin.y = CornerPointRT.y;
			pts.begin.z = CornerPointRT.z;
			pts.end.x = circlecenter.x -sqrt(Juge2);
			pts.end.y = CornerPointLT.y;
			pts.end.z = CornerPointLT.z;
			pts.flag = 1;//表示直线
			pos.push_back(pts);

			//圆弧段
			pts.begin.x = circlecenter.x - sqrt(Juge2);
			pts.begin.y = CornerPointLT.y;
			pts.begin.z = CornerPointLT.z;
			pts.end.x = CornerPointLT.x;
			pts.end.y = circlecenter.y + sqrt(Juge1);
			pts.end.z = CornerPointLT.z;
			pts.flag = 2;//表示圆弧
			pos.push_back(pts);

			//直线段
			pts.begin.x = CornerPointLT.x;
			pts.begin.y = circlecenter.y + sqrt(Juge1);
			pts.begin.z = CornerPointLT.z;
			pts.end.x = CornerPointLT.x;
			pts.end.y = circlecenter.y - sqrt(Juge1);
			pts.end.z = CornerPointLT.z;
			pts.flag = 1;//表示直线
			pos.push_back(pts);

			//圆弧
			pts.begin.x = CornerPointLT.x;
			pts.begin.y = circlecenter.y - sqrt(Juge1);
			pts.begin.z = CornerPointLT.z;
			pts.end.x = circlecenter.x - sqrt(Juge2);
			pts.end.y = CornerPointLB.y;
			pts.end.z = CornerPointLB.z;
			pts.flag = 2;//表示圆弧
			pos.push_back(pts);

			//直线
			pts.begin.x = circlecenter.x - sqrt(Juge2);
			pts.begin.y = CornerPointLB.y;
			pts.begin.z = CornerPointLB.z;
			pts.end.x = circlecenter.x+ sqrt(Juge2);
			pts.end.y = CornerPointLB.y;
			pts.end.z = CornerPointLB.z;
			pts.flag = 1;//表示直线
			pos.push_back(pts);

			//圆弧
			pts.begin.x = circlecenter.x + sqrt(Juge2);
			pts.begin.y = CornerPointRB.y;
			pts.begin.z = CornerPointRB.z;
			pts.end.x = CornerPointRB.x;
			pts.end.y = circlecenter.y - sqrt(Juge1);
			pts.end.z = CornerPointRB.z;
			pts.flag = 2;//表示圆弧
			pos.push_back(pts);

			//直线
			pts.begin.x = CornerPointRB.x;
			pts.begin.y = circlecenter.y - sqrt(Juge1);
			pts.begin.z = CornerPointRB.z;
			pts.end.x = CornerPointRB.x;
			pts.end.y = circlecenter.y;
			pts.end.z = CornerPointRB.z;
			pts.flag = 1;//表示直线
			pos.push_back(pts);

			for (vector<sort>::iterator it = pos.begin(); it != pos.end(); ++it)
			{
				if (it->flag == 1)//直线
				{
					CLine* pLine = new CLine;
					tempArray.Add(pLine);
					pLine->m_begin = it->begin;
					pLine->m_end = it->end;
				}
				else if (it->flag==2)//圆弧
				{
					CArc* pArc = new CArc;
					tempArray.Add(pArc);
					pArc->m_begin = it->begin;
					pArc->m_end = it->end;
					pArc->m_center = circlecenter;
					pArc->m_radio = circleradius;
				}
			}

		}
	}
	else if (circleradius >= Distance)//圆半径大于等于中心点到顶点的距离，此时沿着矩形外轮廓走,假设逆时针
	{
		CLine* Line1 = new CLine;
		tempArray.Add(Line1);
		Line1->m_begin.x = CornerPointRT.x;
		Line1->m_begin.y = circlecenter.y;
		Line1->m_begin.z = CornerPointRT.z;
		Line1->m_end = CornerPointRT;

		CLine* Line2 = new CLine;
		tempArray.Add(Line2);
		Line2->m_begin = CornerPointRT;
		Line2->m_end = CornerPointLT;

		CLine* Line3 = new CLine;
		tempArray.Add(Line3);
		Line3->m_begin = CornerPointLT;
		Line3->m_end = CornerPointLB;

		CLine* Line4 = new CLine;
		tempArray.Add(Line4);
		Line4->m_begin = CornerPointLB;
		Line4->m_end = CornerPointRB;

		CLine* Line5 = new CLine;
		tempArray.Add(Line5);
		Line5->m_begin = CornerPointRB;
		Line5->m_end.x = CornerPointRB.x;
		Line5->m_end.y = circlecenter.y;
		Line5->m_end.z = CornerPointRB.z;

		
	}
	else//半径小于最小值说明在矩形内无需进行区分，直接进行圆插补即可
	{

	}

	return &tempArray;
}

//侧面加工NC代码的生成

BOOL CGcodeGeneration::SideNCInit()
{
		vector<vector<CPtrArray*>>().swap(m_EverySideRoughPile);
		vector<CPtrArray*> SideRoughArrayList = pTool->GetArrayRoughList();//获取粗加工侧面偏置轮廓

	    for (vector<CPtrArray*>::iterator it = SideRoughArrayList.begin(); it != SideRoughArrayList.end(); ++it)
		{
			vector<CPtrArray*> temp;

			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (i > pTool->mark)//元素个数大于底面的个数的时候才开始算
				{

					if (((CLine*)(*it)->GetAt(i))->flag == 1)//直线
					{
						//离散
						vector<CPoint3D> offsetPoints = pTool->LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end, 
							                                                              pTool->GetTime(),pTool->GetFeedspeed() );
						//求取每个高度上的元素及其顺序信息并保存
						for (vector<CPoint3D>::iterator its = offsetPoints.begin(); its != offsetPoints.end(); ++its)
						{
							CPoint3D m_XFPoint = *its;
							m_XFPoint.z = 0.0;
							double distance = CLine::Length(pTool->XMax, m_XFPoint);
							CPtrArray* Array =SideNCOffsetAlgorithm((**it),pTool->mark,distance);
							
							//对Z高度值进行赋值
							for (int j = 0; j != Array->GetSize(); ++j)
							{
								if (((CLine*)Array->GetAt(j))->flag == 1)
								{
									((CLine*)Array->GetAt(j))->m_begin.z = its->z;
									((CLine*)Array->GetAt(j))->m_end.z = its->z;
									CPoint3D pt1 = ((CLine*)Array->GetAt(j))->m_begin;
									CPoint3D pt2 = ((CLine*)Array->GetAt(j))->m_end;
								}
								else if (((CArc*)Array->GetAt(j))->flag == 2)
								{
									if (((CArc*)Array->GetAt(j))->m_Tudu == 0)
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;
										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
									else
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
								}

							}

							temp.push_back(Array);
							//SideRoughArrayList.push_back(Array);

						}
					}
					else if (((CArc*)(*it)->GetAt(i))->flag == 2)//圆弧
					{
						vector<CPoint3D> offsetPoints;
						if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
						{
							offsetPoints = pTool->ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
								((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, pTool->GetTime(), pTool->GetFeedspeed());
						}
						else//多线段圆弧
						{
							offsetPoints = pTool->PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, pTool->GetTime(), pTool->GetFeedspeed());
						}

						for (vector<CPoint3D>::iterator its = offsetPoints.begin(); its != offsetPoints.end(); ++its)
						{
							CPoint3D m_XFPoint = *its;
							m_XFPoint.z = 0.0;
							double distance = CLine::Length(pTool->XMax, m_XFPoint);
							CPtrArray* Array = SideNCOffsetAlgorithm(**it, pTool->mark, distance);

							for (int j = 0; j != Array->GetSize(); ++j)
							{
								if (((CLine*)Array->GetAt(j))->flag == 1)
								{
									((CLine*)Array->GetAt(j))->m_begin.z = its->z;
									((CLine*)Array->GetAt(j))->m_end.z = its->z;
									CPoint3D pt1 = ((CLine*)Array->GetAt(j))->m_begin;
									CPoint3D pt2 = ((CLine*)Array->GetAt(j))->m_end;
								}
								else if (((CArc*)Array->GetAt(j))->flag == 2)
								{
									if (((CArc*)Array->GetAt(j))->m_Tudu == 0)
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;

									}
									else
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
								}

							}
							temp.push_back(Array);
							//SideRoughArrayList.push_back(Array);
						}

					}
				}

			}

			m_EverySideRoughPile.push_back(temp);
			vector<CPtrArray*>().swap(temp);
		}

		//粗加工分层，由外向内，由上向下
		//根据每层与矩形的交点重新建立每层的点的指针集合
		vector<vector<vector<TypeSort>>>().swap(m_SideRough);//清空 存储处理后的所有点
		vector<vector<vector<TypeSort>>> m_TempSideRough;//存储离散后的所有点
		vector<vector<vector<CPoint3D>>> m_SideRoughCutPoints = pTool->GetRoughCutPoints();//获取裁剪后粗加工的点
		for (vector<vector<CPtrArray*>>::iterator it = m_EverySideRoughPile.begin(); it != m_EverySideRoughPile.end(); ++it)
		{
			vector<vector<TypeSort>> TempOnePile;
			for (vector<CPtrArray*>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)
			{
				//CPtrArray* Array = SideCalculate(**its, pTool->GetCenter(), pTool->GetLongth(), pTool->GetWidth());
				vector<TypeSort> Temp;
				for (int i = 0; i != (*its)->GetSize(); ++i)
				{
					if (((CLine*)(*its)->GetAt(i))->flag == 1)//对直线
					{
						vector<CPoint3D> points = pTool->LineDiscrete(((CLine*)(*its)->GetAt(i))->m_begin, ((CLine*)(*its)->GetAt(i))->m_end,
							30, 1000);
						for (vector<CPoint3D>::iterator itss = points.begin(); itss != points.end(); ++itss)
						{
							TypeSort pt;
							itss->z = ((CLine*)(*its)->GetAt(i))->m_begin.z;
							pt.position = *itss;
							pt.flag = 1;
							pt.center = CPoint3D(0, 0, 0);
							Temp.push_back(pt);
						}

					}
					else if (((CArc*)(*its)->GetAt(i))->flag == 2)
					{//0510
						//假设圆弧顺时针
						vector<CPoint3D> points = CArc::ArcTDMInterpolation(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_radio,
							0,((CArc*)(*its)->GetAt(i))->m_begin, ((CArc*)(*its)->GetAt(i))->m_end, 30, 1000);
						for (vector<CPoint3D>::iterator itss = points.begin(); itss != points.end(); ++itss)
						{
							TypeSort pt;
							itss->z = ((CArc*)(*its)->GetAt(i)) ->m_center.z;
							pt.position = *itss;
							pt.flag = 2;//表示圆弧
							pt.center = ((CArc*)(*its)->GetAt(i))->m_center;
							Temp.push_back(pt);
						}
					}

				}
				TempOnePile.push_back(Temp);
				vector<TypeSort>().swap(Temp);
			}
			m_TempSideRough.push_back(TempOnePile);
			vector<vector<TypeSort>>().swap(TempOnePile);
		}
		//2016.5.9遍历m_TempSideRough里面的点需要分的密点精度才会更高，判断在矩形内的点就保留，矩形外的点就不要了，之后再连接截断的点，连接可得到相应截断后的图形。
		for (vector<vector<vector<TypeSort>>>::iterator it = m_TempSideRough.begin(); it != m_TempSideRough.end(); ++it)
		{
			vector<vector<TypeSort>> TempOnePile;
			for (vector<vector<TypeSort>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				vector<TypeSort> Temp;
				for (vector<TypeSort>::iterator itss = its->begin(); itss != its->end(); ++itss)
				{
					BOOL m_JUGE = JugeInSide(itss->position, pTool->GetCenter(), pTool->GetLongth(), pTool->GetWidth());
					//if (m_JUGE)//在矩形内则保存起来
						
						if (!m_JUGE)//将在矩形外的点设为直线
						{
							itss->flag = 1;
							itss->center = CPoint3D(0, 0, 0);
						}
						Temp.push_back(*itss);
				}
				TempOnePile.push_back(Temp);
				vector<TypeSort>().swap(Temp);
			}
			m_SideRough.push_back(TempOnePile);
			vector<vector<TypeSort>>().swap(TempOnePile);
		}

		//精加工数据提取2016.5.12
		vector<vector<CPtrArray*>>().swap(m_SideNCFinish);
		vector<CPtrArray*> SideFinishArrayList = pTool->GetArrayFinishList();//获取精加工侧面偏置轮廓
		for (vector<CPtrArray*>::iterator it = SideFinishArrayList.begin(); it != SideFinishArrayList.end(); ++it)
		{
			vector<CPtrArray*> temp;
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (i > pTool->mark)//元素个数大于底面的个数的时候才开始算
				{
					if (((CLine*)(*it)->GetAt(i))->flag == 1)//直线
					{
						//离散
						vector<CPoint3D> offsetPoints = pTool->LineDiscreteXZ(((CLine*)(*it)->GetAt(i))->m_begin, ((CLine*)(*it)->GetAt(i))->m_end,
							pTool->GetTime(), pTool->GetFeedspeed());
						//求取每个高度上的元素及其顺序信息并保存
						for (vector<CPoint3D>::iterator its = offsetPoints.begin(); its != offsetPoints.end(); ++its)
						{
							CPoint3D m_XFPoint = *its;
							m_XFPoint.z = 0.0;
							double distance = CLine::Length(pTool->XMax, m_XFPoint);
							CPtrArray* Array = SideNCOffsetAlgorithm((**it), pTool->mark, distance);

							//对Z高度值进行赋值
							for (int j = 0; j != Array->GetSize(); ++j)
							{
								if (((CLine*)Array->GetAt(j))->flag == 1)
								{
									((CLine*)Array->GetAt(j))->m_begin.z = its->z;
									((CLine*)Array->GetAt(j))->m_end.z = its->z;
									CPoint3D pt1 = ((CLine*)Array->GetAt(j))->m_begin;
									CPoint3D pt2 = ((CLine*)Array->GetAt(j))->m_end;
								}
								else if (((CArc*)Array->GetAt(j))->flag == 2)
								{
									if (((CArc*)Array->GetAt(j))->m_Tudu == 0)
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;
										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
									else
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
								}

							}

							temp.push_back(Array);

						}
					}
					else if (((CArc*)(*it)->GetAt(i))->flag == 2)//圆弧
					{
						vector<CPoint3D> offsetPoints;
						if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)//普通圆弧
						{
							offsetPoints = pTool->ArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio, ((CArc*)(*it)->GetAt(i))->m_begin_angle,
								((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i, pTool->GetTime(), pTool->GetFeedspeed());
						}
						else//多线段圆弧
						{
							offsetPoints = pTool->PolyArcDiscreteXZ(((CArc*)(*it)->GetAt(i))->m_begin, ((CArc*)(*it)->GetAt(i))->m_end,
								((CArc*)(*it)->GetAt(i))->m_Tudu, pTool->GetTime(), pTool->GetFeedspeed());
						}

						for (vector<CPoint3D>::iterator its = offsetPoints.begin(); its != offsetPoints.end(); ++its)
						{
							CPoint3D m_XFPoint = *its;
							m_XFPoint.z = 0.0;
							double distance = CLine::Length(pTool->XMax, m_XFPoint);
							CPtrArray* Array = SideNCOffsetAlgorithm(**it, pTool->mark, distance);

							for (int j = 0; j != Array->GetSize(); ++j)
							{
								if (((CLine*)Array->GetAt(j))->flag == 1)
								{
									((CLine*)Array->GetAt(j))->m_begin.z = its->z;
									((CLine*)Array->GetAt(j))->m_end.z = its->z;
									CPoint3D pt1 = ((CLine*)Array->GetAt(j))->m_begin;
									CPoint3D pt2 = ((CLine*)Array->GetAt(j))->m_end;
								}
								else if (((CArc*)Array->GetAt(j))->flag == 2)
								{
									if (((CArc*)Array->GetAt(j))->m_Tudu == 0)
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;

									}
									else
									{
										((CArc*)Array->GetAt(j))->m_center.z = its->z;
										((CArc*)Array->GetAt(j))->m_begin.z = its->z;
										((CArc*)Array->GetAt(j))->m_end.z = its->z;

										CPoint3D pt1 = ((CArc*)Array->GetAt(j))->m_begin;
										CPoint3D pt2 = ((CArc*)Array->GetAt(j))->m_end;
										CPoint3D pt3 = ((CArc*)Array->GetAt(j))->m_center;
										float RADIUS = ((CArc*)Array->GetAt(j))->m_radio;
									}
								}

							}
							temp.push_back(Array);
						}

					}
				}

			}

			m_SideNCFinish.push_back(temp);
			vector<CPtrArray*>().swap(temp);
		}

	if (m_SideRough.size() != 0&&m_SideNCFinish.size()!=0)//2016.5.12
		return TRUE;
	else
		return FALSE;
}
BOOL CGcodeGeneration::JugeInSide(CPoint3D& pos, CPoint3D center, float workBlankLong, float workBlankWidth)
{
		WorkBlankRect rec;
		//矩形左上角点
		rec.LeftTop.x = center.x - workBlankLong*0.5;
		rec.LeftTop.y = center.y + workBlankWidth*0.5;
		rec.LeftTop.z = center.z;
		
		//矩形左下角点
		rec.LeftBottom.x = center.x - workBlankLong*0.5;
		rec.LeftBottom.y = center.y - workBlankWidth*0.5;
		rec.LeftBottom.z = center.z;
	
		//矩形右上角点
		rec.RightTop.x = center.x + workBlankLong*0.5;
		rec.RightTop.y = center.y + workBlankWidth*0.5;
		rec.RightTop.z = center.z;
	
		//矩形右下角点
		rec.RightBottom.x = center.x + workBlankLong*0.5;
		rec.RightBottom.y = center.y - workBlankWidth*0.5;
		rec.RightBottom.z = center.z;

			if (pos.x <= rec.RightTop.x&&pos.x >= rec.LeftTop.x)//介于X方向左右之间
			{
				if (pos.y <= rec.RightTop.y&&pos.y >= rec.LeftBottom.y)//介于Y方向最大最小值之间
				{
					return TRUE;
				}
				else if (pos.y >=rec.RightTop.y)//在Y最大值上方
				{
					pos.y = rec.RightTop.y;
					return FALSE;
				}
				else if (pos.y <= rec.LeftBottom.y)//在Y最小值下方
				{
					pos.y = rec.LeftBottom.y;
					return FALSE;
				}
			}
			else
			{
				if (pos.x >= rec.RightTop.x)//在右边
				{
					if (pos.y <= rec.RightTop.y&&pos.y >= rec.LeftBottom.y)//介于Y方向最大最小值之间
					{
						pos.x = rec.RightTop.x;
						return FALSE;
					}
					else if (pos.y >= rec.RightTop.y)//在Y最大值上方
					{
						pos.x = rec.RightTop.x;
						pos.y = rec.RightTop.y;
						return FALSE;
					}
					else if (pos.y <= rec.LeftBottom.y)//在Y最小值下方
					{
						pos.x = rec.RightBottom.x;
						pos.y = rec.RightBottom.y;
						return FALSE;
					}
				}
				else if (pos.x <= rec.LeftTop.x)//在左边
				{
					if (pos.y <= rec.RightTop.y&&pos.y >= rec.LeftBottom.y)//介于Y方向最大最小值之间
					{
						pos.x = rec.LeftTop.x;
						return FALSE;
					}
					else if (pos.y >= rec.RightTop.y)//在Y最大值上方
					{
						pos.x = rec.LeftTop.x;
						pos.y = rec.LeftTop.y;
						return FALSE;
					}
					else if (pos.y <= rec.LeftBottom.y)//在Y最小值下方
					{
						pos.x = rec.LeftBottom.x;
						pos.y = rec.LeftBottom.y;
						return FALSE;
					}
				}
			}
}
CPtrArray* CGcodeGeneration::SideNCOffsetAlgorithm(CPtrArray& XFarray, int mark, double offset)
{
	CPtrArray* Array = new CPtrArray;
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

				//1.将新的线条进行存储
				CLine* pLine = new CLine;
				Array->Add(pLine);
				pLine->m_begin = temp.begin;
				pLine->m_end = temp.end;

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

				//2.将新的线条进行存储
				CLine* pLine = new CLine;
				Array->Add(pLine);
				pLine->m_begin = temp.begin;
				pLine->m_end = temp.end;

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

					//3.将新的线条进行存储
					CLine* pLine = new CLine;
					Array->Add(pLine);
					pLine->m_begin = temp.begin;
					pLine->m_end = temp.end;

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

					//4.将新的线条进行存储
					CLine* pLine = new CLine;
					Array->Add(pLine);
					pLine->m_begin = temp.begin;
					pLine->m_end = temp.end;

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

					//1.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin_angle = ((CArc*)XFarray.GetAt(i))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)XFarray.GetAt(i))->m_end_angle;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					//2.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin_angle = ((CArc*)XFarray.GetAt(i))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)XFarray.GetAt(i))->m_end_angle;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;

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

					//3.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin_angle = ((CArc*)XFarray.GetAt(i))->m_begin_angle;
					pArc->m_end_angle = ((CArc*)XFarray.GetAt(i))->m_end_angle;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;


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

					//1.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;
					pArc->m_Tudu = ((CArc*)XFarray.GetAt(i))->m_Tudu;

					points.push_back(temp);

				}
				else if (i == mark)
				{
					temp.begin = points[mark - 1].end;
					temp.end = points[0].begin;

					//2.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;
					pArc->m_Tudu = ((CArc*)XFarray.GetAt(i))->m_Tudu;

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

					//3.
					CArc* pArc = new CArc;
					Array->Add(pArc);
					pArc->m_center = ((CArc*)XFarray.GetAt(i))->m_center;
					pArc->m_radio = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
					pArc->m_begin = temp.begin;
					pArc->m_end = temp.end;
					pArc->m_Tudu = ((CArc*)XFarray.GetAt(i))->m_Tudu;


					points.push_back(temp);
				}
			}
		}
	}
	return Array;
}
void CGcodeGeneration::SideNC(ofstream* of)
{
	//将m_EverySideRoughPile进行计算，算出其每个元素是否被矩形毛坯所截取，并且将其交点算出，并添加被截取后的元素于m_EverySideRoughPile 中或别处
	//对于精加工则不需要进行判断截取，直接进行输出即可
	
	if (SideNCInit()==TRUE)
	{

		CString str;
		str = _T("侧面加工粗加工NC代码：");
		std::string STR = CStringA(str);
		(*of) << STR.c_str() << endl;
		CPoint3D CENTER = pTool->GetCenter();
		CPoint3D m_CutLiftPt = CPoint3D(0, 0, 0);//设置抬刀点
		

		for (vector<vector<vector<TypeSort>>>::iterator it = m_SideRough.begin(); it != m_SideRough.end(); ++it)
		{
			int count = 0;
			BOOL FeedDirection = FALSE;//默认顺时针		
			for (vector<vector<TypeSort>>::iterator its = it->begin(); its != it->end(); ++its)//从上向底面
			{
				CPtrArray* Array = new CPtrArray;
				int number = SideCalculate(*its)->GetSize();
				for (int j = 0; j != number; ++j)
				{
					if (((CLine*)SideCalculate(*its)->GetAt(j))->flag == 1)
					{
						CLine* pLine = new CLine;
						Array->Add(pLine);
						pLine->m_begin = ((CLine*)SideCalculate(*its)->GetAt(j))->m_begin;
						pLine->m_end = ((CLine*)SideCalculate(*its)->GetAt(j))->m_end;
						pLine->flag = 1;
					}
					else if (((CArc*)SideCalculate(*its)->GetAt(j))->flag == 2)
					{
						CArc* pArc = new CArc;
						Array->Add(pArc);
						pArc->m_begin = ((CArc*)SideCalculate(*its)->GetAt(j))->m_begin;
						pArc->m_end = ((CArc*)SideCalculate(*its)->GetAt(j))->m_end;
						pArc->m_center = ((CArc*)SideCalculate(*its)->GetAt(j))->m_center;
						pArc->flag = 2;
					}
				}
				//计算层数
				str.Format(_T("第 %d 层: "), count);
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;

				if (!FeedDirection)//顺时针的时候，从记录的第一个元素开始算起
				{
					for (int i = 0; i != Array->GetSize(); ++i)
					{
						if (((CLine*)Array->GetAt(i))->flag == 1)//直线
						{
							if (i == 0)//直线元素为第一个元素
							{
								if (its == it->begin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处，Z轴方向下降
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x,
									((CLine*)Array->GetAt(i))->m_begin.y, ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到加工起始点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x,
									((CLine*)Array->GetAt(i))->m_end.y, ((CLine*)Array->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

							}
							else if (i == Array->GetSize() - 1)//直线元素为最后一个元素的时候
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x,
									((CLine*)Array->GetAt(i))->m_end.y, ((CLine*)Array->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

							}
							else//直线元素为中间元素
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x,
									((CLine*)Array->GetAt(i))->m_end.y, ((CLine*)Array->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						else if (((CArc*)Array->GetAt(i))->flag == 2)
						{
							if (i == 0)//圆弧元素为第一个开头
							{
								if (its == it->begin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else//非第一层
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x,
									((CArc*)Array->GetAt(i))->m_begin.y, ((CArc*)Array->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_end.y, ((CArc*)Array->GetAt(i))->m_end.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (i == Array->GetSize() - 1)//圆弧元素以最后一个收尾
							{
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_end.y, ((CArc*)Array->GetAt(i))->m_end.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else//圆弧元素在中间
							{
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_end.y, ((CArc*)Array->GetAt(i))->m_end.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
					}
				}
				else//逆时针的时候，从记录的最后一个元素开始算起
				{
					for (int i = Array->GetSize() - 1; i != -1; --i)
					{
						if (((CLine*)Array->GetAt(i))->flag == 1)//直线
						{
							if (i == Array->GetSize() - 1)//第一个元素是直线
							{
								if (its == it->begin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处，Z轴方向下降
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_end.x,
									((CLine*)Array->GetAt(i))->m_end.y, ((CLine*)Array->GetAt(i))->m_end.z);//点定位到加工起始点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x,
									((CLine*)Array->GetAt(i))->m_begin.y, ((CLine*)Array->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

							}
							else if (i ==0)//最后一个元素是直线
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x,
									((CLine*)Array->GetAt(i))->m_begin.y, ((CLine*)Array->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)Array->GetAt(i))->m_begin.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else//中间元素是直线段
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)Array->GetAt(i))->m_begin.x,
									((CLine*)Array->GetAt(i))->m_begin.y, ((CLine*)Array->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						else if (((CArc*)Array->GetAt(i))->flag == 2)//圆弧
						{
							if (i == Array->GetSize() - 1)//逆时针的第一个元素是圆弧
							{
								if (its == it->begin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else//非第一层
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_end.x,
									((CArc*)Array->GetAt(i))->m_end.y, ((CArc*)Array->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_begin.y, ((CArc*)Array->GetAt(i))->m_begin.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

							}
							else if (i == 0)//逆时针的最后一个元素是圆弧
							{
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_begin.y, ((CArc*)Array->GetAt(i))->m_begin.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)Array->GetAt(i))->m_center.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else//中间的元素是圆弧段
							{
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)Array->GetAt(i))->m_begin.x, ((CArc*)Array->GetAt(i))->m_begin.y, ((CArc*)Array->GetAt(i))->m_begin.z,
									((CArc*)Array->GetAt(i))->m_center.x - ((CArc*)Array->GetAt(i))->m_end.x, ((CArc*)Array->GetAt(i))->m_center.y - ((CArc*)Array->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
					}

				}
				//记号
				++count;
				FeedDirection=!FeedDirection;
			}
		}

		str = _T("侧面加工精加工NC代码：");
		STR = CStringA(str);
		(*of) << STR.c_str() << endl;
		for (vector<vector<CPtrArray*>>::iterator it = m_SideNCFinish.begin(); it != m_SideNCFinish.end(); ++it)
		{
			BOOL FeedDirection = FALSE;//默认顺时针
			int count = 0;
			for (vector<CPtrArray*>::reverse_iterator its = it->rbegin(); its != it->rend(); ++its)//反过来，从上往下
			{
				str.Format(_T("第 %d 层: "), count);
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;

				if (!FeedDirection)//顺时针
				{
					for (int i = 0; i != (*its)->GetSize(); ++i)
					{
						if (((CLine*)(*its)->GetAt(i))->flag == 1)
						{
							if (i == 0)
							{
								if (its == it->rbegin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处，Z轴方向下降
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.x,
									((CLine*)(*its)->GetAt(i))->m_begin.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到加工起始点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_end.x,
									((CLine*)(*its)->GetAt(i))->m_end.y, ((CLine*)(*its)->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (i == (*its)->GetSize()-1)
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_end.x,
									((CLine*)(*its)->GetAt(i))->m_end.y, ((CLine*)(*its)->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

							}
							else
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_end.x,
									((CLine*)(*its)->GetAt(i))->m_end.y, ((CLine*)(*its)->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						else if (((CArc*)(*its)->GetAt(i))->flag == 2)
						{
							if (i == 0)
							{
								if (its == it->rbegin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else//非第一层
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_begin.x,
									((CArc*)(*its)->GetAt(i))->m_begin.y, ((CArc*)(*its)->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_end.y, ((CArc*)(*its)->GetAt(i))->m_end.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (i == (*its)->GetSize() - 1)
							{
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_end.y, ((CArc*)(*its)->GetAt(i))->m_end.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else
							{
								str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_end.y, ((CArc*)(*its)->GetAt(i))->m_end.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_begin.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
					}
				}
				else//逆时针生成？？
				{
					for (int i = (*its)->GetSize() - 1; i != -1; --i)
					{
						if (((CLine*)(*its)->GetAt(i))->flag == 1)
						{
							if (i == (*its)->GetSize() - 1)
							{
								if (its == it->rbegin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处，Z轴方向下降
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_end.x,
									((CLine*)(*its)->GetAt(i))->m_end.y, ((CLine*)(*its)->GetAt(i))->m_end.z);//点定位到加工起始点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.x,
									((CLine*)(*its)->GetAt(i))->m_begin.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (i == 0)
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.x,
									((CLine*)(*its)->GetAt(i))->m_begin.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else
							{
								str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), ((CLine*)(*its)->GetAt(i))->m_begin.x,
									((CLine*)(*its)->GetAt(i))->m_begin.y, ((CLine*)(*its)->GetAt(i))->m_begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						else if (((CArc*)(*its)->GetAt(i))->flag == 2)
						{
							if (i == (*its)->GetSize() - 1)
							{
								if (its == it->rbegin())//每片的第一层
								{
									str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}
								else//非第一层
								{
									str.Format(_T("G00 Z%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
									STR = CStringA(str);
									(*of) << STR.c_str() << endl;
								}

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_end.x,
									((CArc*)(*its)->GetAt(i))->m_end.y, ((CArc*)(*its)->GetAt(i))->m_end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_begin.y, ((CArc*)(*its)->GetAt(i))->m_begin.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (i == 0)
							{
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_begin.y, ((CArc*)(*its)->GetAt(i))->m_begin.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;

								str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), m_CutLiftPt.x, m_CutLiftPt.y, ((CArc*)(*its)->GetAt(i))->m_center.z);//点定位到抬刀点处
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else
							{
								str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), ((CArc*)(*its)->GetAt(i))->m_begin.x, ((CArc*)(*its)->GetAt(i))->m_begin.y, ((CArc*)(*its)->GetAt(i))->m_begin.z,
									((CArc*)(*its)->GetAt(i))->m_center.x - ((CArc*)(*its)->GetAt(i))->m_end.x, ((CArc*)(*its)->GetAt(i))->m_center.y - ((CArc*)(*its)->GetAt(i))->m_end.y);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
					}
				}

				FeedDirection = !FeedDirection;
				++count;
			}
		}
	}	
}
CPtrArray* CGcodeGeneration::SideCalculate(vector<TypeSort> pts)
{
	/*该段中有问题，要将pts中相同的点去掉*/
	CPtrArray* temp=new CPtrArray;
	vector<sort> TempSort;
	vector<sort> DeleteSort;//存储用于判断的sort，判断后存入TempSort
	vector<TypeSort> TempPts;
	//删除重复的点移到别的容器中
	for (vector<TypeSort>::iterator it = pts.begin()+1; it != pts.end(); ++it)
	{
		TypeSort m_TempSort;
		while (it->position.x == (it -1)->position.x&&it->position.y == (it -1)->position.y)
		{
			if (it != pts.end() - 1)
				++it;
			else
				break;
		}
		if (it == pts.begin() + 1)
		{
			m_TempSort.position = (it-1)->position;
			m_TempSort.center = (it-1)->center;
			m_TempSort.flag = (it-1)->flag;
			TempPts.push_back(m_TempSort);
		}
		m_TempSort.position =it->position;
		m_TempSort.center = it->center;
		m_TempSort.flag = it->flag;
		TempPts.push_back(m_TempSort);
		
	}
	//一小段小段线段
	for (vector<TypeSort>::iterator it = TempPts.begin(); it != TempPts.end() - 1; ++it)
	{
		sort xf;
		if (it->flag == 1&&(it+1)->flag==1)//前后两个都是直线的元素
		{
			xf.flag = 1;
			xf.begin = it->position;
			xf.end = (it + 1)->position;
		}
		else if (it->flag == 2&&(it+1)->flag==2)//前后两个都是圆弧的元素
		{
			xf.flag = 2;
			//0510
			xf.begin = it->position;
			xf.center = it->center;
			xf.end = (it + 1)->position;
		}
		else if (it->flag == 1 && (it + 1)->flag == 2)//前直线的元素后市圆弧的元素
		{
			xf.flag = 2;
			xf.begin = it->position;
			xf.center = (it + 1)->center;
			xf.end = (it + 1)->position;
		}
		else if (it->flag == 2 && (it + 1)->flag == 1)//前圆弧的元素后是直线的元素
		{
			xf.flag = 2;
			xf.begin = it->position;
			xf.center = it->center;
			xf.end = (it + 1)->position;
		}
		TempSort.push_back(xf);
		//DeleteSort.push_back(xf);
	}
	
	int i = 0;
	//将离散点分类出直线，利用斜率
	for (vector<sort>::iterator it = TempSort.begin(); it!=TempSort.end();++it)
	{	//0510
		if (it->flag == 1)
		{
			if ((float)it->begin.x == (float)it->end.x)//说明在垂直X轴的直线上，斜率无穷大
			{
				CLine* pLine = new CLine;
				//temp.Add(pLine);
				temp->Add(pLine);
				pLine->flag = 1;
				pLine->m_begin = it->begin;
				BOOL JUGE = FALSE;
				do
				{
					if (it == TempSort.end() - 1)
					{
						JUGE = FALSE;
					}
					/*else if ( fabs(it->end.x-(it + 1)->end.x)<0.5 &&fabs(it->end.x-(it + 1)->begin.x)<0.5)*/
					else if (it->end.x == (it + 1)->end.x && it->end.x == (it + 1)->begin.x)
					{
						JUGE = TRUE;
						++it;
						++i;
					}
					else
						JUGE = FALSE;
				} while (JUGE);
				pLine->m_end = it->end;
			}
			else
			{
				CLine* pLine = new CLine;
				//temp.Add(pLine);
				temp->Add(pLine);
				pLine->flag = 1;
				pLine->m_begin = it->begin;
				BOOL JUGE = FALSE;
				do
				{
					if (it == TempSort.end() - 1)
						JUGE = FALSE;
					else if ((it + 1)->end.x != (it + 1)->begin.x)
					{
						double ratio1 = (it->end.y - it->begin.y) / (it->end.x - it->begin.x);
						double ratio2 = ((it + 1)->end.y - (it + 1)->begin.y) / ((it + 1)->end.x - (it + 1)->begin.x);

						if (fabs((it->end.y - it->begin.y) / (it->end.x - it->begin.x) -
							((it + 1)->end.y - (it + 1)->begin.y) / ((it + 1)->end.x - (it + 1)->begin.x)) < 0.1)//原0.05
						{
							JUGE = TRUE;
							++it;
							++i;
						}
						else
							JUGE = FALSE;
					}
					else
						JUGE = FALSE;
				} while (JUGE);
				pLine->m_end = it->end;
			}
			++i;
		}
		else if (it->flag == 2)//0510
		{
			CArc* pNewArc = new CArc;
			temp->Add(pNewArc);
			pNewArc->flag = 2;
			pNewArc->m_begin = it->begin;
			pNewArc->m_center = it->center;
			BOOL JUGE = FALSE;
			do
			{
				if (it == TempSort.end() - 1)
					JUGE = FALSE;
				else if (it->center == (it + 1)->center)//圆心相等表示同一个圆弧
				{
					JUGE =TRUE;
					++it;
				}
				else
					JUGE = FALSE;
			} while (JUGE);
			
			pNewArc->m_end = it->end;
			
		}
	}
	CPtrArray* NewTemp = new CPtrArray;//存储经过调整后的元素
	vector<TypeSort> NewTypeSort;//提取各个顶点的信息
	for (int i = 0; i != temp->GetSize(); ++i)
	{
		if (((CLine*)temp->GetAt(i))->flag == 1)
		{
			TypeSort m_LocalTypeSort;
			if (i == 0)
			{
				m_LocalTypeSort.center = CPoint3D(0, 0, 0);
				m_LocalTypeSort.flag = 1;
				m_LocalTypeSort.position = ((CLine*)temp->GetAt(i))->m_begin;
				NewTypeSort.push_back(m_LocalTypeSort);
				m_LocalTypeSort.position = ((CLine*)temp->GetAt(i))->m_end;
				NewTypeSort.push_back(m_LocalTypeSort);
			}
			else
			{
				m_LocalTypeSort.center = CPoint3D(0, 0, 0);
				m_LocalTypeSort.flag = 1;
				m_LocalTypeSort.position = ((CLine*)temp->GetAt(i))->m_end;
				NewTypeSort.push_back(m_LocalTypeSort);
			}
		}
		else if (((CArc*)temp->GetAt(i))->flag == 2)
		{
			TypeSort m_LocalTypeSort;
			if (i == 0)
			{
				m_LocalTypeSort.center = ((CArc*)temp->GetAt(i)) ->m_center;
				m_LocalTypeSort.flag = 2;
				m_LocalTypeSort.position = ((CArc*)temp->GetAt(i))->m_begin;
				NewTypeSort.push_back(m_LocalTypeSort);
				m_LocalTypeSort.position = ((CArc*)temp->GetAt(i))->m_end;
				NewTypeSort.push_back(m_LocalTypeSort);
			}
			else
			{
				m_LocalTypeSort.center = ((CArc*)temp->GetAt(i))->m_center;
				m_LocalTypeSort.flag = 2;
				m_LocalTypeSort.position = ((CArc*)temp->GetAt(i))->m_end;
				NewTypeSort.push_back(m_LocalTypeSort);
			}
		}
	}

	//将最后一个点进行处理，如果它起点和终点不相等则保留，如果相等，使其不显示出来
	if (((CLine*)temp->GetAt(temp->GetSize() - 1))->flag == 1)
	{
		if (((CLine*)temp->GetAt(temp->GetSize() - 1))->m_begin == ((CLine*)temp->GetAt(temp->GetSize() - 1))->m_end)
		{
			((CLine*)temp->GetAt(temp->GetSize() - 1))->flag = 4;
		}
	}
	else if (((CArc*)temp->GetAt(temp->GetSize() - 1))->flag == 2)
	{
		if (((CArc*)temp->GetAt(temp->GetSize() - 1))->m_begin == ((CArc*)temp->GetAt(temp->GetSize() - 1))->m_end)
			((CArc*)temp->GetAt(temp->GetSize() - 1))->flag = 4;
	}
	//记录直线段长度小于一定距离的集合
	vector<int> countNumber;
	for (int i = 0; i != temp->GetSize(); ++i)
	{
		if (((CLine*)temp->GetAt(i))->flag == 1)
		{
			float JugeLength = CLine::Length(((CLine*)temp->GetAt(i))->m_begin, ((CLine*)temp->GetAt(i))->m_end);
			if (JugeLength < 1&&JugeLength!=0)
				countNumber.push_back(i);
		}
	}

	//将距离超过一定距离的直线段进行隐藏，并将其终点值赋给下一个元素
	for (vector<int>::iterator it = countNumber.begin(); it != countNumber.end();++it)
	{
		int total = temp->GetSize() - 1;
		if (*it == total)//如果为最后一个元素
		{//第一个元素与倒数第二个元素为直线，则将倒数第二个元素的终点赋值给起点的起始点，将最后一个元素隐藏
			if (((CLine*)temp->GetAt(0))->flag == 1 && ((CLine*)temp->GetAt(*it-1))->flag == 1)
			{
				((CLine*)temp->GetAt(0))->m_begin = ((CLine*)temp->GetAt(*it - 1))->m_end;
				((CLine*)temp->GetAt(*it))->flag = 4;
			}
		}
		else//不是最后一个元素(判断有三段的时候需要迭代计算？？，这里顶多只能计算两段)
		{
			if (((CLine*)temp->GetAt(*it))->flag == 1 && ((CLine*)temp->GetAt(*it + 1))->flag == 1)
			{
				((CLine*)temp->GetAt(*it))->m_end = ((CLine*)temp->GetAt(*it + 1))->m_end;
				
				if (CLine::Length(((CLine*)temp->GetAt(*it))->m_begin, ((CLine*)temp->GetAt(*it))->m_end) < 1)
				{
					if (((CLine*)temp->GetAt(*it-1))->flag == 1)
					{
						((CLine*)temp->GetAt(*it - 1))->m_end = ((CLine*)temp->GetAt(*it))->m_end;
						((CLine*)temp->GetAt(*it))->flag = 4;
					}
				}
				((CLine*)temp->GetAt(*it + 1))->flag = 4;
			}
		}
	}

	return temp;
}

//圆弧双截面加工NC代码的生成
BOOL CGcodeGeneration::ArcNCInit()
{
	vector<vector<vector<sort>>>().swap(m_ArcRough);//清零
	vector<vector<vector<sort>>>().swap(m_ArcFinish);
	vector<vector<CPtrArray*>> m_ArcNCRoughList=pTool->m_ArcsYSide;
	vector<vector<CPtrArray*>> m_ArcNCFinishList = pTool->m_ArcFinishYSide;
	for (vector<vector<CPtrArray*>>::iterator it = m_ArcNCRoughList.begin(); it != m_ArcNCRoughList.end(); ++it)
	{
		vector<vector<sort>> m_tempArcRough;
		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end()/* - 1*/; ++its)
		{
			vector<sort> m_tempTempArcRough = ArcNCCalculate(**its, pTool->GetArcWorkBlankHeigth());
			m_tempArcRough.push_back(m_tempTempArcRough);
			vector<sort>().swap(m_tempTempArcRough);
		}

		//插入抬刀点和进刀点
		vector<sort> HeadSort;
		vector<sort> EndSort;
		sort InsertSort;
		InsertSort.flag = 1;
		InsertSort.begin = m_tempArcRough.begin()->begin()->begin;
		InsertSort.begin.z = 2500;
		InsertSort.end = m_tempArcRough.begin()->begin()->begin;
		HeadSort.push_back(InsertSort);
		m_tempArcRough.insert(m_tempArcRough.begin(), HeadSort);

		if (m_tempArcRough.size() % 2 == 0)//偶数
		{
			InsertSort.begin = ((m_tempArcRough.end() - 1)->end() - 1)->end;
			InsertSort.end = ((m_tempArcRough.end() - 1)->end() - 1)->end;
			InsertSort.end.z = 2500;
			EndSort.push_back(InsertSort);
		}
		else//奇数
		{
			InsertSort.begin = ((m_tempArcRough.end() - 1)->begin())->begin;
			InsertSort.end = ((m_tempArcRough.end() - 1)->begin())->begin;
			InsertSort.end.z = 2500;
			EndSort.push_back(InsertSort);
		}
		m_tempArcRough.push_back(EndSort);

		m_ArcRough.push_back(m_tempArcRough);
		vector<vector<sort>>().swap(m_tempArcRough);
	}
	//精加工点处理
	for (vector<vector<CPtrArray*>>::iterator it = m_ArcNCFinishList.begin(); it!= m_ArcNCFinishList.end(); ++it)
	{
		vector<vector<sort>> m_tempArcFinish;
		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<sort> m_tempTempArcFinish = ArcNCCalculate(**its, pTool->GetArcWorkBlankHeigth());
			m_tempArcFinish.push_back(m_tempTempArcFinish);
			vector<sort>().swap(m_tempTempArcFinish);
		}

		//插入抬刀点和进刀点
		vector<sort> HeadSort;
		vector<sort> EndSort;
		sort InsertSort;
		InsertSort.flag = 1;
		InsertSort.begin = m_tempArcFinish.begin()->begin()->begin;
		InsertSort.begin.z = 2500;
		InsertSort.end = m_tempArcFinish.begin()->begin()->begin;
		HeadSort.push_back(InsertSort);
		m_tempArcFinish.insert(m_tempArcFinish.begin(), HeadSort);

		if (m_tempArcFinish.size() % 2 == 0)//偶数
		{
			InsertSort.begin = ((m_tempArcFinish.end() - 1)->end() - 1)->end;
			InsertSort.end = ((m_tempArcFinish.end() - 1)->end() - 1)->end;
			InsertSort.end.z = 2500;
			EndSort.push_back(InsertSort);
		}
		else//奇数
		{
			InsertSort.begin = ((m_tempArcFinish.end() - 1)->begin())->begin;
			InsertSort.end = ((m_tempArcFinish.end() - 1)->begin())->begin;
			InsertSort.end.z = 2500;
			EndSort.push_back(InsertSort);
		}
		m_tempArcFinish.push_back(EndSort);

		m_ArcFinish.push_back(m_tempArcFinish);
		vector<vector<sort>>().swap(m_tempArcFinish);
	}

	if (m_ArcRough.size() != 0 && m_ArcFinish.size() != 0)
		return TRUE;
	else
		return FALSE;
}

vector<sort> CGcodeGeneration::ArcNCCalculate(CPtrArray& Array, float High)
{
	vector<sort> m_tempSort;
	for (int i = 0; i != Array.GetSize(); ++i)
	{
		if (((CArc*)Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)Array.GetAt(i))->m_Tudu == 0)
			{
				BOOL direction = CArc::CalculateYZ(((CArc*)Array.GetAt(i))->m_center, ((CArc*)Array.GetAt(i))->m_radio, ((CArc*)Array.GetAt(i))->m_begin_angle - 270,
					((CArc*)Array.GetAt(i))->m_end_angle + 90, Array, i);
			}
			else
			{
				((CArc*)Array.GetAt(i))->PolyCalculateYZ(((CArc*)Array.GetAt(i))->m_begin, ((CArc*)Array.GetAt(i))->m_end, ((CArc*)Array.GetAt(i))->m_Tudu);
			}

			if (((CArc*)Array.GetAt(i))->m_center.z >= High)//圆心在线之上的直接变直线
			{
				sort m_LineSort;
				m_LineSort.flag = 1;
				m_LineSort.begin = ((CArc*)Array.GetAt(i))->m_begin;
				m_LineSort.end = ((CArc*)Array.GetAt(i))->m_end;
				m_LineSort.begin.z =m_LineSort.end.z= High;
				m_tempSort.push_back(m_LineSort);
			}
			else//圆心在线段的下面
			{
				float distance = High - ((CArc*)Array.GetAt(i))->m_center.z;//求取圆心与线段的垂直距离跟半径做比较
				if (distance >= ((CArc*)Array.GetAt(i))->m_radio)//说明圆弧与直线没有交点
				{
					sort m_ArcSort;
					m_ArcSort.flag = 2;
					m_ArcSort.begin = ((CArc*)Array.GetAt(i))->m_begin;
					m_ArcSort.end = ((CArc*)Array.GetAt(i))->m_end;
					m_ArcSort.center = ((CArc*)Array.GetAt(i))->m_center;
					m_tempSort.push_back(m_ArcSort);
				}
				else//圆弧与直线有交点
				{
					float y1, y2;
					y1 = ((CArc*)Array.GetAt(i))->m_center.y + sqrt(pow(((CArc*)Array.GetAt(i))->m_radio, 2) - pow(High - ((CArc*)Array.GetAt(i))->m_center.z, 2));
					y2 = ((CArc*)Array.GetAt(i))->m_center.y - sqrt(pow(((CArc*)Array.GetAt(i))->m_radio, 2) - pow(High - ((CArc*)Array.GetAt(i))->m_center.z, 2));
					if (y1 - y2 >= fabs(((CArc*)Array.GetAt(i))->m_end.y - ((CArc*)Array.GetAt(i))->m_begin.y))//两解之间的长度大于起始点终止点之间的长度，为一直线
					{
						sort m_LineSort;
						m_LineSort.flag = 1;
						m_LineSort.begin = ((CArc*)Array.GetAt(i))->m_begin;
						m_LineSort.end = ((CArc*)Array.GetAt(i))->m_end;
						m_LineSort.begin.z = m_LineSort.end.z = High;
						m_tempSort.push_back(m_LineSort);
					}
					else
					{
						sort m_ArcSort;
						m_ArcSort.flag = 2;
						m_ArcSort.begin = ((CArc*)Array.GetAt(i))->m_begin;
						m_ArcSort.end.x = ((CArc*)Array.GetAt(i))->m_end.x;
						/*m_ArcSort.end.y = y1;*/
						m_ArcSort.end.y = y2;
						m_ArcSort.end.z = High;
						m_ArcSort.center = ((CArc*)Array.GetAt(i))->m_center;
						m_tempSort.push_back(m_ArcSort);

						sort m_LineSort;
						m_LineSort.flag = 1;
						m_LineSort.begin.x = ((CArc*)Array.GetAt(i))->m_begin.x;
						/*m_LineSort.begin.y = y1;*/
						m_LineSort.begin.y = y2;
						m_LineSort.end.x = ((CArc*)Array.GetAt(i))->m_end.x;
						/*m_LineSort.end.y = y2;*/
						m_LineSort.end.y = y1;
						m_LineSort.begin.z = m_LineSort.end.z = High;
						m_tempSort.push_back(m_LineSort);

						m_ArcSort.begin.x = ((CArc*)Array.GetAt(i))->m_begin.x;
						/*m_ArcSort.begin.y = y2;*/
						m_ArcSort.begin.y = y1;
						m_ArcSort.begin.z = High;
						m_ArcSort.end = ((CArc*)Array.GetAt(i))->m_end;
						m_ArcSort.center = ((CArc*)Array.GetAt(i))->m_center;
						m_tempSort.push_back(m_ArcSort);


					}
				}

			}
		}
	}

	return m_tempSort;
}

void CGcodeGeneration::ArcNC(ofstream* of)
{
	if (ArcNCInit())
	{
		CString str;
		str = _T("双截面圆弧粗加工NC代码：");
		std::string STR = CStringA(str);
		(*of) << STR.c_str() << endl;
		str = _T("G90 G19 M03");//绝对值方式，YZ平面，主轴正转
		STR = CStringA(str);
		(*of) << STR.c_str() << endl;
		int number = 0;
	
		//粗加工NC代码生成,默认单圆的方向为逆时针
		for (vector<vector<vector<sort>>>::iterator it = m_ArcRough.begin(); it != m_ArcRough.end(); ++it)
		{
			BOOL direction = TRUE;
			str.Format(_T("第%d层："), number);
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			for (vector<vector<sort>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				if (its==it->begin())//第一个部分只有一条直线
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin()->begin.x, its->begin()->begin.y, its->begin()->begin.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin()->end.x, its->begin()->end.y, its->begin()->end.z);
					STR = CStringA(str);
				    (*of) << STR.c_str() << endl;
					
				}
				else if (its==it->end()-1)//最后一个1条直线
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin()->end.x, its->begin()->end.y, its->begin()->end.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
				else
				{
					if (direction)//原先方向
					{
						for (vector<sort>::iterator itss = its->begin(); itss != its->end(); ++itss)
						{
							if (itss->flag == 1)//直线
							{
								str.Format(_T("G01 Y%0.3f Z%0.3f F1500"),itss->end.y, itss->end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (itss->flag == 2)
							{
								str.Format(_T("G02 Y%0.3f Z%0.3f J%0.3f K%0.3f F1500"),  itss->end.y, itss->end.z,
									itss->center.y - itss->begin.y, itss->center.z - itss->begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}

						}
						if (its != it->end() - 2)//倒数第二个元素不用进行各段的连接
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->rbegin()->end.x, (its + 1)->rbegin()->end.y, (its + 1)->rbegin()->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					else//反方向
					{
						for (vector<sort>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						{
							if (itss->flag == 1)
							{
								str.Format(_T("G01 Y%0.3f Z%0.3f F1500"), itss->begin.y, itss->begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (itss->flag == 2)
							{
								str.Format(_T("G03 Y%0.3f Z%0.3f J%0.3f K%0.3f F1500"),  itss->begin.y, itss->begin.z,
									itss->center.y - itss->end.y, itss->center.z - itss->end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						//各段的连接
						if (its != it->end() - 2)//倒数第二个元素不用进行各段的连接
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->begin()->begin.x, (its + 1)->begin()->begin.y, (its + 1)->begin()->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}

					direction = !direction;
				}

			}

			++number;
		}

		str = _T("双截面圆弧精加工NC代码：");
		STR = CStringA(str);
		(*of) << STR.c_str() << endl;

		for (vector<vector<vector<sort>>>::iterator it = m_ArcFinish.begin(); it != m_ArcFinish.end(); ++it)
		{
			BOOL direction = TRUE;
			str.Format(_T("第%d层："), number);
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			for (vector<vector<sort>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				if (its == it->begin())//第一个部分只有一条直线
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin()->begin.x, its->begin()->begin.y, its->begin()->begin.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin()->end.x, its->begin()->end.y, its->begin()->end.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
				else if (its == it->end() - 1)//最后一个1条直线
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z% 0.3f F1500"), its->begin()->end.x, its->begin()->end.y, its->begin()->end.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
				else
				{
					if (direction)//原先方向
					{
						for (vector<sort>::iterator itss = its->begin(); itss != its->end(); ++itss)
						{
							if (itss->flag == 1)//直线
							{
								str.Format(_T("G01 Y%0.3f Z%0.3f F1500"), itss->end.y, itss->end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (itss->flag == 2)
							{
								str.Format(_T("G02 Y%0.3f Z%0.3f J%0.3f K%0.3f F1600"), itss->end.y, itss->end.z,
									itss->center.y - itss->begin.y, itss->center.z - itss->begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}

						}
						if (its != it->end() - 2)//倒数第二个元素不用进行各段的连接
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z% 0.3f F1500"), (its + 1)->rbegin()->end.x, (its + 1)->rbegin()->end.y, (its + 1)->rbegin()->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					else//反方向
					{
						for (vector<sort>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)
						{
							if (itss->flag == 1)
							{
								str.Format(_T("G01 Y%0.3f Z% 0.3f F1500"), itss->begin.y, itss->begin.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
							else if (itss->flag == 2)
							{
								str.Format(_T("G03 Y%0.3f Z%0.3f J%0.3f K%0.3f F1700"), itss->begin.y, itss->begin.z,
									itss->center.y - itss->end.y, itss->center.z - itss->end.z);
								STR = CStringA(str);
								(*of) << STR.c_str() << endl;
							}
						}
						//各段的连接
						if (its != it->end() - 2)//倒数第二个元素不用进行各段的连接
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z% 0.3f F1500"), (its + 1)->begin()->begin.x, (its + 1)->begin()->begin.y, (its + 1)->begin()->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}

					direction = !direction;
				}
			}
			++number;
		}

	}
}


//线条加工NC代码的生成
BOOL CGcodeGeneration::LineNCInit()//将直线或者圆弧段按照顺序排好
{
	if (!pTool->GetLineStyle())//直线
	{
		return TRUE;
	}
	else//圆弧
	{
		vector<vector<sort>>().swap(LineRoughArc);
		vector<sort>().swap(LineFinishArc);
		//粗加工数据提取
		for (vector<vector<CPtrArray*>>::iterator it = pTool->m_LineRoughVSegmentDisArc.begin();it != pTool->m_LineRoughVSegmentDisArc.end(); ++it)
		{
			vector<sort> m_tempLineRoughArc;
			for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
			{
				sort temp;
				for (int i = 0; i != (*its)->GetSize(); ++i)
				{
					if (((CArc*)(*its)->GetAt(i))->flag == 2)
					{
						if (((CArc*)(*its)->GetAt(i))->m_Tudu == 0)
						{
							((CArc*)(*its)->GetAt(i))->Calculate(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_radio,
								((CArc*)(*its)->GetAt(i))->m_begin_angle, ((CArc*)(*its)->GetAt(i))->m_end_angle, **its, i);
							temp.begin = ((CArc*)(*its)->GetAt(i))->m_begin;
							temp.end = ((CArc*)(*its)->GetAt(i))->m_end;
							temp.center = ((CArc*)(*its)->GetAt(i))->m_center;
							temp.flag = 2;
						}
					}
				}
				m_tempLineRoughArc.push_back(temp);
			}
			LineRoughArc.push_back(m_tempLineRoughArc);
			vector<sort>().swap(m_tempLineRoughArc);
		}
		//精加工数据提取
		for (vector<CPtrArray*>::iterator it = pTool->m_LineFinishVSegmentDisArc.begin(); it != pTool->m_LineFinishVSegmentDisArc.end(); ++it)
		{
			sort temp;
			for (int i = 0; i != (*it)->GetSize(); ++i)
			{
				if (((CArc*)(*it)->GetAt(i))->flag == 2)
				{
					if (((CArc*)(*it)->GetAt(i))->m_Tudu == 0)
					{
						((CArc*)(*it)->GetAt(i))->Calculate(((CArc*)(*it)->GetAt(i))->m_center, ((CArc*)(*it)->GetAt(i))->m_radio,
							((CArc*)(*it)->GetAt(i))->m_begin_angle, ((CArc*)(*it)->GetAt(i))->m_end_angle, **it, i);
						temp.begin = ((CArc*)(*it)->GetAt(i))->m_begin;
						temp.end = ((CArc*)(*it)->GetAt(i))->m_end;
						temp.center = ((CArc*)(*it)->GetAt(i))->m_center;
						temp.flag = 2;
					}
				}
			}
			LineFinishArc.push_back(temp);
		}

		if (LineRoughArc.size() != 0 && LineFinishArc.size() != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}


}
void CGcodeGeneration::LineNC(ofstream* of)
{
	if (LineNCInit())
	{
		CString str;
		str = _T("线条加工加工NC代码：");
		std::string STR = CStringA(str);
		(*of) << STR.c_str() << endl;
		if (!pTool->GetLineStyle())//直线
		{
			str = _T("直线型线条粗加工：");
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), 0, 0, pTool->GetLineWorkBlankHigh());//点定位到某高度
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			for (vector<vector<LinePos>>::iterator it = pTool->m_LineRoughVSegmentDisLine.begin();
				it != pTool->m_LineRoughVSegmentDisLine.end(); ++it)
			{
				BOOL Direction = TRUE;
				for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if ( its == it->begin())//第一个，点位
					{
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);//点定位到抬刀点处
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}

					if (its == it->end() - 1)//最后一个直线，抬到结束点的毛坯高度
					{
						if (Direction)//正向，往Y正方向
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, pTool->GetLineWorkBlankHigh());
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, pTool->GetLineWorkBlankHigh());
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					else
					{
						if (Direction)//正向，往Y正方向
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its+1)->end.x, (its+1)->end.y, (its+1)->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else//逆向，往Y负方向
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its+1)->begin.x, (its+1)->begin.y, (its+1)->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}

					}

					Direction = !Direction;

				}
			}

			str = _T("直线型线条精加工：");
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;
			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"),pTool->m_LineRoughVSegmentDisLine.begin()->begin()->begin.x,
				    pTool->m_LineRoughVSegmentDisLine.begin()->begin()->begin.y, pTool->GetLineWorkBlankHigh());//点定位到原先粗加工起点
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), pTool->m_LineFinishVSegmentDisLine.begin()->begin()->begin.x,
				pTool->m_LineFinishVSegmentDisLine.begin()->begin()->begin.y, pTool->m_LineFinishVSegmentDisLine.begin()->begin()->begin.z);//点定位到精加工起点
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			for (vector<vector<LinePos>>::iterator it = pTool->m_LineFinishVSegmentDisLine.begin();
				it != pTool->m_LineFinishVSegmentDisLine.begin()+1; ++it)
			{
				BOOL Direction = TRUE;
				for (vector<LinePos>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if (its == it->end()-1)//最后一个一条直线与m_LineProcess的第一条重叠
					{
						if (Direction)
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (pTool->m_LineProcess.begin()+1)->end.x, 
								            (pTool->m_LineProcess.begin()+1)->end.y,( pTool->m_LineProcess.begin()+1)->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							Direction = FALSE;
							for (vector<LinePos>::iterator itss = pTool->m_LineProcess.begin() + 1;
								itss != pTool->m_LineProcess.end(); ++itss)
							{
								if (itss == pTool->m_LineProcess.end() - 1)
								{
									if (Direction)
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, itss->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, pTool->GetLineWorkBlankHigh());
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
									else
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, itss->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, pTool->GetLineWorkBlankHigh());
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
								}
								else
								{
									if (Direction)
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, itss->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (itss + 1)->end.x, (itss + 1)->end.y, (itss + 1)->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
									else
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, itss->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (itss + 1)->begin.x, (itss + 1)->begin.y, (itss + 1)->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
								}

								Direction = !Direction;
							}

						}
						else
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (pTool->m_LineProcess.begin()+1)->begin.x,
								(pTool->m_LineProcess.begin()+1)->begin.y, (pTool->m_LineProcess.begin()+1)->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							Direction = TRUE;
							for (vector<LinePos>::iterator itss = pTool->m_LineProcess.begin() + 1;
								itss != pTool->m_LineProcess.end(); ++itss)
							{
								if (itss == pTool->m_LineProcess.end() - 1)
								{
									if (Direction)
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, itss->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, pTool->GetLineWorkBlankHigh());
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
									else
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, itss->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, pTool->GetLineWorkBlankHigh());
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
								}
								else
								{
									if (Direction)
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->end.x, itss->end.y, itss->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (itss + 1)->end.x, (itss + 1)->end.y, (itss + 1)->end.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
									else
									{
										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), itss->begin.x, itss->begin.y, itss->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;

										str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (itss + 1)->begin.x, (itss + 1)->begin.y, (itss + 1)->begin.z);
										STR = CStringA(str);
										(*of) << STR.c_str() << endl;
									}
								}

								Direction = !Direction;
							}

						}
						

					}
					else
					{
						if (Direction)
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->end.x, (its + 1)->end.y, (its + 1)->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->begin.x, (its + 1)->begin.y, (its + 1)->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}

					Direction = !Direction;
				}
			}




		}
		else//圆弧
		{
			str = _T("圆弧型线条粗加工：");
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), 0, 0, pTool->GetLineWorkBlankHigh());//点定位到某高度
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			for (vector<vector<sort>>::iterator it = LineRoughArc.begin(); it != LineRoughArc.end(); ++it)
			{
				BOOL Direction=TRUE;
				for (vector<sort>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if (its == it->begin())//第一个
					{
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);//点定位到起始点
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					if (its == it->end() - 1)//最后一个
					{
						if (Direction)//逆时针
						{
							str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), its->end.x, its->end.y, its->end.z,
								its->center.x - its->begin.x, its->center.y - its->begin.y);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, pTool->GetLineWorkBlankHigh());//点定位到结束点的毛坯高度
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else
						{
							str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z,
								its->center.x - its->end.x, its->center.y - its->end.y);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, pTool->GetLineWorkBlankHigh());//点定位到结束点的毛坯高度
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

						}
					}
					else//中间
					{
						if (Direction)//逆时针
						{
							str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), its->end.x, its->end.y, its->end.z,
								its->center.x - its->begin.x, its->center.y - its->begin.y);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its+1)->end.x, (its+1)->end.y, (its+1)->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else//顺时针
						{
							str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z,
								its->center.x - its->end.x, its->center.y - its->end.y);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;

							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->begin.x, (its + 1)->begin.y, (its + 1)->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					Direction = !Direction;
				}
			}

			str = _T("圆弧型线条精加工：");
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"),LineRoughArc.begin()->begin()->begin.x,LineRoughArc.begin()->begin()->begin.y, 
				                                            LineRoughArc.begin()->begin()->begin.z);//点定位到粗加工起始位置
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), LineFinishArc.begin()->begin.x,LineFinishArc.begin()->begin.y,
				                                             LineFinishArc.begin()->begin.z);//点定位到精加工起始位置
			STR = CStringA(str);
			(*of) << STR.c_str() << endl;

			BOOL Direction = TRUE;
			for (vector<sort>::iterator it = LineFinishArc.begin(); it != LineFinishArc.end(); ++it)
			{
				if (it == LineFinishArc.begin())
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin.x, it->begin.y, it->begin.z);//点定位到起始点
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
				if (it == LineFinishArc.end() - 1)
				{
					if (Direction)//逆时针
					{
						str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), it->end.x, it->end.y, it->end.z,
							it->center.x - it->begin.x, it->center.y - it->begin.y);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), it->end.x, it->end.y, pTool->GetLineWorkBlankHigh());//点定位到终点的毛坯高度
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					else
					{
						str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), it->begin.x, it->begin.y, it->begin.z,
							it->center.x - it->end.x, it->center.y - it->end.y);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin.x, it->begin.y, pTool->GetLineWorkBlankHigh());//点定位到终点的毛坯高度
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
				else
				{
					if (Direction)//逆时针
					{
						str.Format(_T("G03 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), it->end.x, it->end.y, it->end.z,
							it->center.x - it->begin.x, it->center.y - it->begin.y);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (it + 1)->end.x, (it + 1)->end.y, (it + 1)->end.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					else
					{
						str.Format(_T("G02 X%0.3f Y%0.3f Z%0.3f I%0.3f J%0.3f F1500"), it->begin.x, it->begin.y, it->begin.z,
							it->center.x - it->end.x, it->center.y - it->end.y);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;

						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (it + 1)->begin.x, (it + 1)->begin.y, (it + 1)->begin.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
				Direction = !Direction;
			}


		}
	}
}

//正交加工NC代码的生成
BOOL CGcodeGeneration::OrthgonalNCInit()
{
	if (pTool->GetOrthgonalTrapezoidType() == 0)//正梯形
	{
		if (pTool->NormalTrapezoidCut.size() != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (pTool->GetOrthgonalTrapezoidType() == 1)//倒梯形
	{
		if (pTool->ReverseTrapezoidCut.size() != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}
void CGcodeGeneration::OrthgonalNC(ofstream* of)
{
	if (OrthgonalNCInit())
	{
		CString str;
		str = _T("正交加工加工NC代码：");
		std::string STR = CStringA(str);
		(*of) << STR.c_str() << endl;

		if (pTool->GetOrthgonalTrapezoidType() == 0)//正梯形
		{
			if (pTool->GetOrthgonalProcessStyle() == 0)//横切
			{
				str = _T("正梯形横切加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}
			else if (pTool->GetOrthgonalProcessStyle() == 1)//纵切
			{
				str = _T("正梯形竖切加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}
			else if (pTool->GetOrthgonalProcessStyle() == 2)//正交切
			{
				str = _T("正梯形正交加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}

			for (vector<vector<OffsetPoint>>::iterator it = pTool->NormalTrapezoidCut.begin(); it != pTool->NormalTrapezoidCut.end(); ++it)
			{
				for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if (its == it->begin())
					{
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), its->begin.x, its->begin.y, its->begin.z+300);//点定位到某高度
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);//点定位到起始点
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;

					if (its == it->end() - 1)
					{
						if (it->size() % 2 == 0)//偶数个，结束点对第一条线起点
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z+300);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else//奇数个，结束点对第一条线终点
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z+300);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					else
					{
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its+1)->begin.x, (its+1)->begin.y, (its+1)->begin.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}

				}
			}

		}
		else if (pTool->GetOrthgonalTrapezoidType() == 1)//倒梯形
		{
			if (pTool->GetOrthgonalProcessStyle() == 0)//横切
			{
				str = _T("倒梯形横切加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}
			else if (pTool->GetOrthgonalProcessStyle() == 1)//纵切
			{
				str = _T("倒梯形竖切加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}
			else if (pTool->GetOrthgonalProcessStyle() == 2)//正交切
			{
				str = _T("倒梯形正交加工加工NC代码：");
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;
			}

			for (vector<vector<OffsetPoint>>::iterator it = pTool->ReverseTrapezoidCut.begin(); it != pTool->ReverseTrapezoidCut.end(); ++it)
			{
				for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
				{
					if (its == it->begin())
					{
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), its->begin.x, its->begin.y, its->begin.z + 300);//点定位到某高度
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);//点定位到起始点
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;

					if (its == it->end() - 1)
					{
						if (it->size() % 2 == 0)//偶数个，结束点对第一条线起点
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
						else//奇数个，结束点对第一条线终点
						{
							str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
							str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300);
							STR = CStringA(str);
							(*of) << STR.c_str() << endl;
						}
					}
					else
					{
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->begin.x, (its + 1)->begin.y, (its + 1)->begin.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}

				}
			}
		}
	}
}


//多边形加工NC输出及初始化
BOOL CGcodeGeneration::PolygonNCInit()
{
	return TRUE;
}
void CGcodeGeneration::PolygonNC(ofstream* of)
{
	if (PolygonNCInit())
	{
		CString str;
		str.Format(_T("%d 边形正交加工NC代码："), pTool->GetPolygeonNumber());
		std::string STR = CStringA(str);
		(*of) << STR.c_str() << endl;

		for (vector<vector<OffsetPoint>>::iterator it = pTool->m_PolygeonSequentialLines.begin(); it != pTool->m_PolygeonSequentialLines.end(); ++it)
		{
			for (vector<OffsetPoint>::iterator its = it->begin(); its != it->end(); ++its)
			{
				if (its == it->begin())
				{
					str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), its->begin.x, its->begin.y, its->begin.z + 300);//点定位到某高度
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->begin.x, its->begin.y, its->begin.z);//点定位到起始点
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
				str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), its->end.x, its->end.y, its->end.z);
				STR = CStringA(str);
				(*of) << STR.c_str() << endl;

				if (its == it->end() - 1)
				{
					if (it->size() % 2 == 0)//偶数个，结束点对第一条线起点
					{
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->begin.x, it->begin()->begin.y, it->begin()->begin.z + 300);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
					else//奇数个，结束点对第一条线终点
					{
						str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
						str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"), it->begin()->end.x, it->begin()->end.y, it->begin()->end.z + 300);
						STR = CStringA(str);
						(*of) << STR.c_str() << endl;
					}
				}
				else
				{
					str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), (its + 1)->begin.x, (its + 1)->begin.y, (its + 1)->begin.z);
					STR = CStringA(str);
					(*of) << STR.c_str() << endl;
				}
			}
		}
	}
}
//NC代码的显示
void CGcodeGeneration::ShowNcCode()//把G代码显示在List控件中
{

}

//台面板加工
void CGcodeGeneration::TableBoardNC(ofstream* of)
{
	TableBoardInSawNC(CTableBoardProcess::GetTableProcessInstance()->GetTableInSawNCElements(), of);
	TableBoardInMillNC(CTableBoardProcess::GetTableProcessInstance()->GetTableInMillNCElements(), of);
	TableBoardOutNC(CTableBoardProcess::GetTableProcessInstance()->GetTableOutNCElements(), of);
}


void CGcodeGeneration::TableBoardInSawNC(vector<vector<vector<vector<elements>>>> data, ofstream* of)
{
	CString str = _T("台面板加工内侧锯片加工NC代码：");
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;

	str = _T("G90 G17 M03");//绝对值方式，XY平面，主轴正转
	STR = CStringA(str);
	(*of) << STR.c_str() << endl;

	//初始点移动到2倍工件高的地方
	Input_G00(0, 0, 2*CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(),of);

	for (vector<vector<vector<vector<elements>>>>::iterator it = data.begin(); it != data.end(); ++it)//哪一部分
	{
		for (vector<vector<vector<elements>>>::iterator its = it->begin(); its != it->end(); ++its)//哪层
		{
			BOOL m_Direction = TRUE;//TRUE则顺序输出，否则，则逆序输出

			for (vector<vector<elements>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪高度
			{
				if (m_Direction)//正向
				{
					for (vector<elements>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)
					{
						if (itsss->flag == 1)
						{
							//每一个高度
							if (itsss == itss->begin())
							{
								//定位到起点
								Input_G00(itsss->begin.x, itsss->begin.y, itsss->begin.z, of);
								Input_G01(itsss->end.x, itsss->end.y, itsss->end.z, of);
							}
							else
							{
								Input_G01(itsss->end.x, itsss->end.y, itsss->end.z, of);
							}
							//最后一部分的最后一层的最低高度的最后一点
							if (it == data.end() - 1 && its == it->end() - 1 && itss == its->end() - 1 && itsss == itss->end() - 1)
							{
								Input_G00(itsss->end.x, itsss->end.y, 
									            2 * CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
							}
						}
					}
				}
				else//逆向
				{
					for (vector<elements>::reverse_iterator itsss = itss->rbegin(); itsss != itss->rend(); ++itsss)
					{
						if (itsss->flag == 1)
						{
							if (itsss == itss->rbegin())
							{
								Input_G00(itsss->end.x, itsss->end.y, itsss->end.z, of);
								Input_G01(itsss->begin.x, itsss->begin.y, itsss->begin.z, of);
							}
							else
							{
								Input_G01(itsss->begin.x, itsss->begin.y, itsss->begin.z, of);
							}
							//最后一部分的最后一层的最低高度的最后一点
							if (it == data.end() - 1 && its == it->end() - 1 && itss == its->end() - 1 && itsss == itss->rend() - 1)
							{
								Input_G00(itsss->begin.x, itsss->begin.y,
									2 * CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
							}
						}
					}
				}
				
				m_Direction = !m_Direction;
			}
		}
	}

}
void CGcodeGeneration::TableBoardInMillNC(vector<vector<vector<vector<elements>>>> data, ofstream* of)
{
	CString str = _T("G28 Z0 T01 M06");//换T01铣刀
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;

	for (vector<vector<vector<vector<elements>>>>::iterator it = data.begin(); it != data.end(); ++it)//哪块
	{
		for (vector<vector<vector<elements>>>::iterator its = it->begin(); its != it->end(); ++its)//哪层
		{
			BOOL m_ArcDirection = TRUE;

			for (vector<vector<elements>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪高度
			{
				if (m_ArcDirection)//正向,只有第一个，没有最后一个
				{
					for (vector<elements>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//哪段
					{
						if (itsss->flag == 1)//直线段
						{
							if (its==it->begin()&&itss == its->begin() && itsss == itss->begin())//每块第一层的最高位置的第一条线段
							{
								Input_G00(itsss->begin.x, itsss->begin.y,
									2 * CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
								Input_G00(itsss->begin.x, itsss->begin.y, itsss->begin.z, of);
							}
							else if (itss == its->begin()&&itsss==itss->begin())//每层的最高位置的第一条线段
							{
								Input_G00(itsss->begin.x, itsss->begin.y, itsss->begin.z,of);
							}
							Input_G01(itsss->end.x, itsss->end.y, itsss->end.z, of);
						
						}
						else if (itsss->flag == 2)//圆弧段
						{
							//这边z为0的情况怎样？
							Input_G02(itsss->end.x, itsss->end.y, itsss->end.z,
								itsss->center.x - itsss->begin.x, itsss->center.y - itsss->begin.y, of);
						}
					}
				}
				else//逆向，只有最后一个没有第一个
				{
					for (vector<elements>::reverse_iterator itsss = itss->rbegin(); itsss != itss->rend(); ++itsss)//哪段
					{
						if (itsss->flag == 1)
						{
							Input_G01(itsss->begin.x, itsss->begin.y, itsss->begin.z, of);

							//最后一部分的最后一层最低位置的最后一个元素
							if (it == data.end() - 1 && its == it->end() - 1 && itss == its->end() - 1 && itsss == itss->rend() - 1)
							{
								Input_G00(itsss->begin.x, itsss->begin.y,
									2 * CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
							}
						}
						else if (itsss->flag == 2)
						{
							Input_G03(itsss->begin.x, itsss->begin.y, itsss->begin.z,
								itsss->center.x - itsss->end.x, itsss->center.y - itsss->end.y, of);
						}
					}
				}
				m_ArcDirection = !m_ArcDirection;
			}
		}
	}


}
void CGcodeGeneration::TableBoardOutNC(vector<vector<vector<elements>>> data, ofstream* of)
{
	CString str = _T("G28 Z0 T02 M06");//换T02锯片
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;
	for (vector<vector<vector<elements>>>::iterator it = data.begin(); it != data.end(); ++it)//哪一层
	{
		BOOL m_Direction = TRUE;
		for (vector<vector<elements>>::iterator its = it->begin(); its != it->end(); ++its)//哪高度
		{
			if (m_Direction)//正向顺时针
			{
				for (vector<elements>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪线段
				{
					if (itss->flag == 1)
					{
						if (itss == its->begin())//每段高度最高的第一段元素
						{
							//定位到起点
							Input_G00(itss->begin.x, itss->begin.y, itss->begin.z, of);
						}

						Input_G01(itss->end.x, itss->end.y, itss->end.z, of);

					}
					else if (itss->flag==2)
					{
						if (itss == its->begin())//每段高度最高的第一段元素
						{
							//定位到起点
							Input_G00(itss->begin.x, itss->begin.y, itss->begin.z, of);
						}
						if (itss->arc_direction == 0)//顺时针时针
						{
							Input_G02(itss->end.x, itss->end.y, itss->end.z,
								itss->center.x - itss->begin.x, itss->center.y - itss->begin.y, of);
						}
						else if (itss->arc_direction==1)//逆时针
						{
							Input_G03(itss->end.x,itss->end.y,itss->end.z,
								itss->center.x - itss->begin.x, itss->center.y - itss->begin.y, of);
						}
						
					}

					//最后一层的最低高度最后一段元素
					if (it == data.end() - 1 && its == it->end() - 1 && itss==its->end() - 1)
					{
						Input_G00(itss->end.x, itss->end.y, 2*CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
					}

				}
			}
			else//逆向逆时针
			{
				for (vector<elements>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)//哪线段
				{
					if (itss == its->rbegin())//每段高度最高的第一段元素
					{
						Input_G00(itss->end.x, itss->end.y, itss->end.z, of);
					}

					if (itss->flag == 1)
					{		
						Input_G01(itss->begin.x, itss->begin.y, itss->begin.z, of);
					}
					else  if (itss->flag == 2)
					{
						if (itss->arc_direction == 1)//原本就是逆时针，反过来走应该是顺时针
						{
							Input_G02(itss->begin.x, itss->begin.y, itss->begin.z,
								itss->center.x - itss->end.x, itss->center.y - itss->end.y, of);
						}
						else if (itss->arc_direction == 0)//原本是顺时针的，反过来就是逆时针
						{
							Input_G03(itss->begin.x, itss->begin.y, itss->begin.z,
								itss->center.x - itss->end.x, itss->center.y - itss->end.y, of);
						}
						
					}

					//最后一层的最低高度最后一段元素
					if (it == data.end() - 1 && its == it->end() - 1 && itss == its->rend() - 1)
					{
						Input_G00(itss->begin.x, itss->begin.y,
							       2 * CTableBoardProcess::GetTableProcessInstance()->GetBoardHeigh(), of);
					}
				}
			}
			

			m_Direction = !m_Direction;
		}
	}
}

//输入
void CGcodeGeneration::Input_G00(float posx, float posy, float posz,ofstream* of)
{
	CString str;
	str.Format(_T("G00 X%0.3f Y%0.3f Z%0.3f"),posx,posy,posz);
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;
}
void CGcodeGeneration::Input_G01(float posx, float posy, float posz, ofstream* of)
{
	CString str;
	str.Format(_T("G01 X%0.3f Y%0.3f Z%0.3f F1500"), posx, posy, posz);
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;
}
void CGcodeGeneration::Input_G02(float posx, float posy, float posz,float posi,float posj, ofstream* of)
{
	CString str;
	str.Format(_T("G02 X%0.3f Y%0.3f I%0.3f J%0.3f Z%0.3f F1500"), posx, posy, posi, posj, posz);
	
	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;
}
void CGcodeGeneration::Input_G03(float posx, float posy, float posz, float posi, float posj, ofstream* of)
{
	CString str;
	
	str.Format(_T("G03 X%0.3f Y%0.3f I%0.3f J%0.3f Z%0.3f F1500"), posx, posy, posi, posj, posz);


	std::string STR = CStringA(str);
	(*of) << STR.c_str() << endl;
}