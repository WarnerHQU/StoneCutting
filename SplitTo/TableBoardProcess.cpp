#include "stdafx.h"
#include "TableBoardProcess.h"
//用于台面板加工的C++类
#include"Utils.h"

CTableBoardProcess* CTableBoardProcess::m_TableProcess = NULL;

CTableBoardProcess::CTableBoardProcess()
{
	//试验git
	m_ptBoardArray = NULL;
	m_innerArray = NULL;
	m_outArray = NULL;
	m_ExtendLength = 200;
	m_TotalSize = 0;
	m_PerDownDis = 0.0;

}


CTableBoardProcess::~CTableBoardProcess()
{
	if (m_ptBoardArray != NULL)
		delete m_ptBoardArray;
	if (m_innerArray != NULL)
		delete m_innerArray;
	if (m_outArray != NULL)
		delete m_outArray;
}


void CTableBoardProcess::RenderTableBoard(COpenGLDC* pDC)
{
	/*
	pDC->SetMaterialColor(RGB(255, 0, 0));
	DrawDIY(m_innerArray, pDC);
	pDC->SetMaterialColor(RGB(100, 100, 100));
	DrawDIY(m_outArray, pDC);
	pDC->SetMaterialColor(RGB(200,100,150));
	DrawDIY(Change_To_Array(m_CutData), pDC);
	*/

	/*
	for (elements it : m_CutData)
	{
		//对直线进行操作
		if (it.flag == 1)
		{
			glPointSize(5);
			pDC->DrawPoint(it.begin);
			pDC->DrawPoint(it.end);
		}
	}*/

	DrawDIYFromPt(m_InnerModelPoints, pDC);
	DrawDIYFromPt(m_OuterModelPoints, pDC);
	//RenderTableBlank(pDC);
	
}


void CTableBoardProcess::DrawDIY(CPtrArray* array, COpenGLDC* pDC)
{
	if (array != NULL)
	{
		for (int i = 0; i != array->GetSize(); i++)
		{
			if (((CLine*)array->GetAt(i))->flag == 1)
			{
				pDC->DrawLine(((CLine*)array->GetAt(i))->m_begin, ((CLine*)array->GetAt(i))->m_end);
			}
			else if (((CArc*)array->GetAt(i))->flag == 2)
			{
				if (((CArc*)array->GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					((CArc*)array->GetAt(i))->GLDrawArc(((CArc*)array->GetAt(i))->m_center, ((CArc*)array->GetAt(i))->m_begin_angle,
						((CArc*)array->GetAt(i))->m_end_angle, 0.0001, ((CArc*)array->GetAt(i))->m_radio, *array, i);
				}
				else//多线段圆弧
				{
					((CArc*)array->GetAt(i))->PolyCalculate(((CArc*)array->GetAt(i))->m_begin, ((CArc*)array->GetAt(i))->m_end,
						((CArc*)array->GetAt(i))->m_Tudu);
					if (((CArc*)array->GetAt(i))->m_Tudu > 0)//逆时针
					{
						((CArc*)array->GetAt(i))->PolyDrawNArc(((CArc*)array->GetAt(i))->m_center, ((CArc*)array->GetAt(i))->m_begin_angle,
							((CArc*)array->GetAt(i))->m_end_angle, 0.0001, ((CArc*)array->GetAt(i))->m_radio);
					}
					else//顺时针
					{
						((CArc*)array->GetAt(i))->PolyDrawSArc(((CArc*)array->GetAt(i))->m_center, ((CArc*)array->GetAt(i))->m_begin_angle,
							((CArc*)array->GetAt(i))->m_end_angle, 0.0001, ((CArc*)array->GetAt(i))->m_radio);
					}

				}
			}
			else if (((CCircle*)array->GetAt(i))->flag == 3)
			{
				((CCircle*)array->GetAt(i))->GLDrawCircle(((CCircle*)array->GetAt(i))->m_center, ((CCircle*)array->GetAt(i))->m_radio);
			}

		}
	}
}

void CTableBoardProcess::DrawDIYFromPt(vector<vector<CPoint3D>> data, COpenGLDC* pDC)
{
	for (vector<vector<CPoint3D>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		for (vector<CPoint3D>::iterator its = it->begin(); its != it->end() - 1; ++its)
		{
			pDC->DrawLine(*its, *(its + 1));
		}
	}
}

void CTableBoardProcess::DrawDIYFromPt(vector<vector<vector<vector<CPoint3D>>>> data, COpenGLDC* pDC)
{
	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end() - 1; ++itsss)
				{
					pDC->DrawLine(*itsss, *(itsss + 1));
				}
			}
		}
	}
}
void CTableBoardProcess::DrawDIYFromOffset(vector<OffsetPoint> data, COpenGLDC* pDC)
{
	for (OffsetPoint it : data)
	{
		pDC->DrawLine(it.begin, it.end);
	}
}

void CTableBoardProcess::DrawOutRetractLine(vector<vector<vector<CPoint3D>>> data, COpenGLDC* pDC)
{
	pDC->SetMaterialColor(RGB(72, 61,139));//深板岩蓝
	/*将集合放到一个集合里方便绘制*/
	vector<vector<vector<elements>>> m_SumRetractLines;

	vector<vector<elements>> m_RetractBLines;//起始点出退刀线集合
	vector<vector<elements>> m_RetractELines;//终止点处退到线集合

	for (vector<vector<vector<CPoint3D>>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		vector<elements> mm_RetractBLines;
		vector<elements> mm_RetractELines;
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			elements e1, e2;

			CVector3D m_LineDirection =*(its->begin()+1)-*(its->begin());
			CPoint3D m_RetractBegin = *(its->begin()) - m_LineDirection.GetNormal() * m_ExtendLength;//假设延伸200mm
			e1.flag = 1;
			e1.begin = m_RetractBegin;
			e1.end = *(its->begin());
			mm_RetractBLines.push_back(e1);
			pDC->DrawLine(m_RetractBegin, *(its->begin()));

			m_LineDirection = *(its->end()-1) - *(its->end()-2);
			CPoint3D m_RetractEnd = *(its->end() - 1) + m_LineDirection.GetNormal() * m_ExtendLength;
			e2.flag = 2;
			e2.begin = *(its->end() - 1);
			e2.end = m_RetractEnd;
			mm_RetractELines.push_back(e2);

			pDC->DrawLine(m_RetractEnd, *(its->end()-1));
		}
		m_RetractBLines.push_back(mm_RetractBLines);
		m_RetractELines.push_back(mm_RetractELines);
	}

	m_SumRetractLines.push_back(m_RetractBLines);
	m_SumRetractLines.push_back(m_RetractELines);

	/*
	for (vector<vector<elements>>::iterator it = m_RetractBLines.begin(); it != m_RetractBLines.end(); ++it)
	{
		for (int i = 0; i != it->size(); ++i)
		{
			if (i% 2 == 1)//奇数层，起点连接起点
			{
				if (i != it->size() - 1)
				{
					pDC->DrawLine(((*it)[i]).begin, ((*it)[i + 1]).begin);
				}
			}
		}
	}
	for (vector<vector<elements>>::iterator it = m_RetractELines.begin(); it != m_RetractELines.end(); ++it)
	{
		for (int i = 0; i != it->size(); ++i)
		{
			if (i % 2 == 0)//偶数层，终点连接终点
			{
				if (i != it->size() - 1)
				{
					pDC->DrawLine(((*it)[i]).end, ((*it)[i + 1]).end);
				}
			}
		}
	}*/
	for (vector<vector<vector<elements>>>::iterator it = m_SumRetractLines.begin(); it != m_SumRetractLines.end(); ++it)
	{
		if (it == m_SumRetractLines.begin())//起点的延伸线部分
		{
			for (vector<vector<elements>>::iterator its = it->begin(); its != it->end(); ++its)
			{
				for (int i = 0; i != its->size(); ++i)
				{
					if (i % 2 == 1)//奇数层，起点连接起点
					{
						if (i != its->size() - 1)
						{
							pDC->SetMaterialColor(RGB(72, 61, 139));//深板岩蓝
							pDC->DrawLine(((*its)[i]).begin, ((*its)[i + 1]).begin);
						}
						else//最后一个是奇数，连接最开始的起点
						{
							//绘制虚线
							//非最后一层，则连接下一层起点
							pDC->SetMaterialColor(RGB(255, 255, 255));
							if (its != it->end()-1)
							{
								pDC->DrawStippleLine(((*its)[i]).begin, ((*(its+1))[0]).begin);
							}
							else
							{
								//如果没有下一层
								pDC->DrawStippleLine(((*its)[i]).begin, CPoint3D(((*its)[i]).begin.x, 
									                 ((*its)[i]).begin.y, m_Board_Heigh*2));
							}
						}
					}
					
				}
			}
		}
		else if (it == m_SumRetractLines.end() - 1)//终点的延伸线部分
		{
			//记录层数
			int m_ZCount = 0;
			for (vector<vector<elements>>::iterator its = it->begin(); its != it->end(); ++its)
			{	
				for (int i = 0; i != its->size(); ++i)
				{
					if (i % 2 == 0)//偶数层，终点连接终点
					{
						if (i != its->size() - 1)
						{
							pDC->SetMaterialColor(RGB(72, 61, 139));//深板岩蓝
							pDC->DrawLine(((*its)[i]).end, ((*its)[i + 1]).end);
						}
						else//最后一个是偶数层，连接下一起点延伸层的起点
						{
							pDC->SetMaterialColor(RGB(255, 255, 255));
							if (its != it->end() - 1)
							{
								//pDC->DrawStippleLine(((*its)[i]).end, ((*(its + 1))[0]).end);
								pDC->DrawStippleLine(((*its)[i]).end, (*(it-1))[m_ZCount+1].begin()->begin);
							}
							else
							{

								pDC->DrawStippleLine(((*its)[i]).end, CPoint3D(((*its)[i]).end.x,
									((*its)[i]).end.y, m_Board_Heigh * 2));
							}
						}
					}
				}
				m_ZCount += 1;
			}

		}
		
	}

}
void CTableBoardProcess::DrawInSawRetractLine(vector<vector<vector<vector<CPoint3D>>>> data, COpenGLDC* pDC)
{
	vector<vector<vector<vector<elements>>>> m_InSawRetractElements;

	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = data.begin(); it != data.end(); ++it)//哪一块的锯片轨迹
	{
		vector<vector<vector<elements>>> m_SawElements;

		vector<vector<elements>> mm_SawCutElements;//进
		vector<vector<elements>> mm_SawLeaveElements;//退

		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)//哪一块的哪一层的锯片轨迹
		{
			

			//vector<vector<elements>> mm_SawCutElements;//进
			//vector<vector<elements>> mm_SawLeaveElements;//退
			CVector3D m_Linedirection = *(its->begin()->end() - 1) - *(its->begin()->begin());//直线的方向
			CVector3D m_NRotateLine = CVector3D(m_Linedirection.dy, -m_Linedirection.dx, 0).GetNormal();//逆时针旋转后的单位向量
			
			vector<elements> mmm_SawCutElements;
			vector<elements> mmm_SawLeaveElements;
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				elements e1,e2;
				e1.flag = 1;
				e1.begin = *(itss->begin()) + m_NRotateLine*m_ExtendLength;//依然200mm
				e1.end = *(itss->begin());
				e2.flag = 1;
				e2.begin = *(itss->end()-1);
				e2.end = *(itss->end() - 1) + m_NRotateLine * m_ExtendLength;
				mmm_SawCutElements.push_back(e1);
				mmm_SawLeaveElements.push_back(e2);
			}

			mm_SawCutElements.push_back(mmm_SawCutElements);
			mm_SawLeaveElements.push_back(mmm_SawLeaveElements);

			//m_SawElements.push_back(mm_SawCutElements);
			//m_SawElements.push_back(mm_SawLeaveElements);

			
		}

		m_SawElements.push_back(mm_SawCutElements);
		m_SawElements.push_back(mm_SawLeaveElements);

		m_InSawRetractElements.push_back(m_SawElements);
	}
	int j = 0;
	for (vector<vector<vector<vector<elements>>>>::iterator it = m_InSawRetractElements.begin(); it != m_InSawRetractElements.end(); ++it)//哪一个部分的延伸线
	{
		for (vector<vector<vector<elements>>>::iterator its = it->begin(); its != it->end(); ++its)//进刀或退刀
		{
			int i = 0;
			for (vector<vector<elements>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪一层
			{
				BOOL m_LineDirection = TRUE;
				
				for (vector<elements>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//哪一高度
				{
					pDC->SetMaterialColor(RGB(72, 61, 139));//深板岩蓝
					pDC->DrawLine(itsss->begin, itsss->end);
					if (its == it->begin())
					{
						if (!m_LineDirection)//进刀部分
						{
							if (itsss != itss->end() - 1)//非最后一条线段
							{
								pDC->DrawLine(itsss->begin, (itsss + 1)->begin);
							}
							else//最后一条线段，需要连接到下一部分的进刀部分的第一条开始点
							{
								pDC->SetMaterialColor(RGB(255, 255, 255));
								if (itss == its->end() - 1)//最后一层的进刀层
								{
									if (it != m_InSawRetractElements.end() - 1)//非最后一部分的进刀层
									{
										pDC->DrawStippleLine(itsss->begin, (it + 1)->begin()->begin()->begin()->begin);
									}
									else//最后一部分的最后一层
									{
										pDC->DrawStippleLine(itsss->begin, CPoint3D(itsss->begin.x,itsss->begin.y,m_Board_Heigh*2));
									}
								}
								else
								{
									pDC->DrawStippleLine(itsss->begin, (itss + 1)->begin()->begin);
								}
								
							}
						}
					}
					else
					{
						if (m_LineDirection)//退刀部分
						{
							if (itsss != itss->end() - 1)
							{
								pDC->DrawLine(itsss->end, (itsss + 1)->end);
							}
							else
							{
								pDC->SetMaterialColor(RGB(255, 255, 255));
								if (itss == its->end() - 1)
								{		
									if (it != m_InSawRetractElements.end() - 1)
									{
										pDC->DrawStippleLine(itsss->end, (it + 1)->begin()->begin()->begin()->begin);
									}
									else//最后一部分的最后一层
									{
										pDC->DrawStippleLine(itsss->end, CPoint3D(itsss->end.x, itsss->end.y, m_Board_Heigh * 2));
									}
								}
								else
								{
									pDC->DrawStippleLine(itsss->end, ((m_InSawRetractElements.begin()+j)->begin()->begin()+i+1)->begin()->begin);
								}
								
							}
						}
					}
					m_LineDirection = !m_LineDirection;
				}
				i = i + 1;
			}
			
		}
		j += 1;
	}

}
void CTableBoardProcess::DrawInMillRetractLine(vector<vector<vector<vector<CPoint3D>>>> data, COpenGLDC* pDC)
{
	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = data.begin(); it != data.end(); ++it)//哪一部分
	{
		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)//哪一层
		{
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪一高度
			{
				pDC->SetMaterialColor(RGB(255, 255, 255));
				if (its == it->begin())//每一层第一个高度
				{
					pDC->DrawStippleLine(CPoint3D(itss->begin()->x, itss->begin()->y, m_Board_Heigh * 2), 
						CPoint3D(itss->begin()->x, itss->begin()->y, m_Board_Heigh));

				}
				if (itss == its->end() - 1)//每一层的最后一个高度,连接下一层的第一个高度
				{
					if (its !=it->end() - 1)
					{
						pDC->DrawStippleLine(*(itss->begin()),*((its+1)->begin()->begin()));
					}
					else
					{
						if (it != data.end() - 1)//非最后一层
						{
							pDC->DrawStippleLine(*(itss->begin()),
								CPoint3D((it + 1)->begin()->begin()->begin()->x, (it + 1)->begin()->begin()->begin()->y, m_Board_Heigh * 2));
						}
						else//最后一层
						{
							pDC->DrawStippleLine(*(itss->begin()),
								CPoint3D((itss->begin())->x, (itss->begin())->y, m_Board_Heigh * 2));
						}
					}
					
				}
			}
		}
	}
}
CTableBoardProcess* CTableBoardProcess::GetTableProcessInstance()
{
	if (m_TableProcess == NULL)//判断第一次调用
	{
		m_TableProcess = new CTableBoardProcess();
	}
	return m_TableProcess;
}


//在对话框中已经判断input的内容是否为空，故这里无须再判断
void CTableBoardProcess::InitData(CPtrArray* input)
{
	m_ptBoardArray = input;

	ClearData();

	m_ArrayList = InitTableModel(m_ptBoardArray, m_Board_Heigh,2);

	m_ModelPoints = GetPoints(m_ArrayList, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);

	if (m_SourceData.size())
		m_SourceData.clear();
	m_SourceData = CalculateData(input);
	if (m_SourceData.size())
	{
		m_ProfileData = RegonizeProfile(m_SourceData);

		//需要由vector<elements>转向CPtrArray*的函数
		m_InArrayList = InitTableModel(Change_To_Array(m_ProfileData[1]), m_Board_Heigh,2);
		m_OutArrayList = InitTableModel(Change_To_Array(m_ProfileData[0]), m_Board_Heigh,2);

		m_InnerModelPoints = GetPoints(m_InArrayList, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		m_OuterModelPoints = GetPoints(m_OutArrayList, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		//获取偏置后离散各点
		m_OuterBlankPoints = CoverOutsideProfToPoints(m_OutArrayList, 20, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);

		//只需要对内轮廓进行处理
		m_CutData = ChangeSegments(m_ProfileData[1]);
	}

	InitTableBlank();
	InitTableOutPath();
	InitTableInPath();
	InitTableSimulationPoints(m_OutOffsetPoints, m_InOffsetSawPoints, m_InOffsetMillPoints);
	//初始化NC数据
	m_TableOutNCElements=InitTableOutNC(m_OutOffsetProfile);
	m_TableInSawNCElements=InitTableInSawNC(m_InOffsetSawProfile);
	m_TableInMillNCElements=InitTableInMillNC(m_InOffsetMillProfile);
}

void CTableBoardProcess::ClearData()
{
	vector<CPtrArray*>().swap(m_ArrayList);
	vector<CPtrArray*>().swap(m_InArrayList);
	vector<CPtrArray*>().swap(m_OutArrayList);

	vector<vector<CPoint3D>>().swap(m_ModelPoints);
	vector<vector<CPoint3D>>().swap(m_InnerModelPoints);
	vector<vector<CPoint3D>>().swap(m_OuterModelPoints);

	vector<vector<elements>>().swap(m_ProfileData);

	vector<elements>().swap(m_CutData);

	vector<vector<CPoint3D>>().swap(m_OuterBlankPoints);

	//清空内外轮廓毛坯数据
	vector<OffsetPoint>().swap(m_OuterBlankOffset);
	vector<OffsetPoint>().swap(m_InBlankOffset);
	vector<OffsetPoint>().swap(m_OuterBlankTopOffset);
	vector<OffsetPoint>().swap(m_InBlankTopOffset);

	//清空外轮廓离散点
	vector<vector<vector<CPoint3D>>>().swap(m_OutOffsetPoints);
	vector<vector<CPtrArray*>>().swap(m_OutOffsetProfile);

	//清空内轮廓离散点
	vector<vector<CPtrArray*>>().swap(m_inOffsetProfile);
	vector<vector<vector<CPoint3D>>>().swap(m_inOffsetPoints);

	vector<vector<vector<CPtrArray*>>>().swap(m_InOffsetSawProfile);
	vector<vector<vector<vector<CPoint3D>>>>().swap(m_InOffsetSawPoints);

	vector<vector<vector<CPtrArray*>>>().swap(m_InOffsetMillProfile);
	vector<vector<vector<vector<CPoint3D>>>>().swap(m_InOffsetMillPoints);

	//清空NC储存元素
	vector<vector<vector<elements>>>().swap(m_TableOutNCElements);
	vector<vector<vector<vector<elements>>>>().swap(m_TableInSawNCElements);
	vector<vector<vector<vector<elements>>>>().swap(m_TableInMillNCElements);
}

void CTableBoardProcess::ClearBoardArray()
{
	//设为空
	m_ptBoardArray = NULL;
}

vector<CPtrArray*> CTableBoardProcess::InitTableModel(CPtrArray* data,float high,float depth)
{
	vector<CPtrArray*> s;

	//每层隔2mm
	//int fild =2;

	vector<float> PilesHeigh;
	while (high>0)
	{
		PilesHeigh.push_back(high);
		if (high <= depth)
		{
			PilesHeigh.push_back(0.0);
		}
		high -= depth;
	}
	
	/*
	for (float it : PilesHeigh)
	{
		//指针内容复制？
		CPtrArray& datas=*data;

		for (int i = 0; i < datas.GetSize(); ++i)
		{
			if (((CLine*)datas.GetAt(i))->flag == 1)
			{
				((CLine*)datas.GetAt(i))->m_begin.z = it;
				((CLine*)datas.GetAt(i))->m_end.z = it;
			}
			else if (((CArc*)datas.GetAt(i))->flag == 2)
			{
				((CArc*)datas.GetAt(i))->m_center.z = it;
				((CArc*)datas.GetAt(i))->m_begin.z = it;
				((CArc*)datas.GetAt(i))->m_end.z = it;
				
			}
		}
		s.push_back(&datas);
	}*/

	for (float it : PilesHeigh)
	{
		CPtrArray* newArray = new CPtrArray();
		for (int i = 0; i < data->GetSize(); ++i)
		{
			if (((CLine*)data->GetAt(i))->flag == 1)
			{
				CLine* pLine = new CLine();
				pLine->flag = ((CLine*)data->GetAt(i))->flag;
				pLine->m_begin = ((CLine*)data->GetAt(i))->m_begin;
				pLine->m_end = ((CLine*)data->GetAt(i))->m_end;
				pLine->m_end.z = it;
				pLine->m_begin.z = it;
				newArray->Add(pLine);
			}
			else if (((CArc*)data->GetAt(i))->flag == 2)
			{
				CArc* pArc = new CArc();
				pArc->m_center = ((CArc*)data->GetAt(i))->m_center;
				pArc->m_begin = ((CArc*)data->GetAt(i))->m_begin;
				pArc->m_end = ((CArc*)data->GetAt(i))->m_end;
				pArc->m_radio = ((CArc*)data->GetAt(i))->m_radio;
				pArc->m_begin_angle = ((CArc*)data->GetAt(i))->m_begin_angle;
				pArc->m_end_angle = ((CArc*)data->GetAt(i))->m_end_angle;

				pArc->m_Tudu = ((CArc*)data->GetAt(i))->m_Tudu;
				pArc->flag = ((CArc*)data->GetAt(i))->flag;
				pArc->m_Direction = ((CArc*)data->GetAt(i))->m_Direction;

				pArc->m_begin.z = it;
				pArc->m_center.z = it;
				pArc->m_end.z = it;

				newArray->Add(pArc);
			}
		}
		s.push_back(newArray);
	}
	return s;
}


vector<elements> CTableBoardProcess::CalculateData(CPtrArray* input)
{
	vector<elements> m_TempElements;

	if (input != NULL)
	{
		for (int i = 0; i != input->GetSize(); i++)
		{
			if (((CLine*)input->GetAt(i))->flag == 1)
			{
				elements e;
				e.begin = ((CLine*)input->GetAt(i))->m_begin;
				e.end = ((CLine*)input->GetAt(i))->m_end;
				m_TempElements.push_back(e);
			}
			else if (((CArc*)input->GetAt(i))->flag == 2)
			{
				elements e;
				if (((CArc*)input->GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					((CArc*)input->GetAt(i))->Calculate(((CArc*)input->GetAt(i))->m_center, ((CArc*)input->GetAt(i))->m_radio,
						((CArc*)input->GetAt(i))->m_begin_angle, ((CArc*)input->GetAt(i))->m_end_angle, *input, i);
					e.tudu = 0;
				}
				else//多线段圆弧
				{
					((CArc*)input->GetAt(i))->PolyCalculate(((CArc*)input->GetAt(i))->m_begin, ((CArc*)input->GetAt(i))->m_end,
						((CArc*)input->GetAt(i))->m_Tudu);
					e.tudu = ((CArc*)input->GetAt(i))->m_Tudu;
				}

				e.flag = 2;
				e.begin = ((CArc*)input->GetAt(i))->m_begin;
				e.end = ((CArc*)input->GetAt(i))->m_end;
				e.center = ((CArc*)input->GetAt(i))->m_center;
				e.radius = ((CArc*)input->GetAt(i))->m_radio;
				e.start_angle = ((CArc*)input->GetAt(i))->m_begin_angle;
				e.end_angle = ((CArc*)input->GetAt(i))->m_end_angle;
				m_TempElements.push_back(e);
			}
		

		}
	}

	return m_TempElements;
	
}

//新的不经过计算即可转换的转换器
vector<elements> CTableBoardProcess::New_CalculateData(CPtrArray* input)
{
	vector<elements> m_TempElements;

	if (input != NULL)
	{
		for (int i = 0; i != input->GetSize(); i++)
		{
			if (((CLine*)input->GetAt(i))->flag == 1)
			{
				elements e;
				e.begin = ((CLine*)input->GetAt(i))->m_begin;
				e.end = ((CLine*)input->GetAt(i))->m_end;
				m_TempElements.push_back(e);
			}
			else if (((CArc*)input->GetAt(i))->flag == 2)
			{
				elements e;
			
				e.tudu = ((CArc*)input->GetAt(i))->m_Tudu;

				e.flag = 2;
				e.begin = ((CArc*)input->GetAt(i))->m_begin;
				e.end = ((CArc*)input->GetAt(i))->m_end;
				e.center = ((CArc*)input->GetAt(i))->m_center;
				e.radius = ((CArc*)input->GetAt(i))->m_radio;
				e.start_angle = ((CArc*)input->GetAt(i))->m_begin_angle;
				e.end_angle = ((CArc*)input->GetAt(i))->m_end_angle;
				m_TempElements.push_back(e);
			}


		}
	}

	return m_TempElements;

}


//改变线段的段数，只改变直线
vector<elements> CTableBoardProcess::ChangeSegments(vector<elements> data)
{
	vector<elements> s;
	float d = 200;//由于先前所画图略小，故这边刀具的直径先设置小点
	float ap = 10;

	for (elements it : data)
	{
		if (it.flag == 1)
		{
			//it.begin.x += 1200;
			//it.end.x += 1200;

			//vector<elements> temps = SeperateToThreeParts(it, d / 2, ap);
			//用铣刀的半径表示锯片的半径，乘以10.
			vector<elements> temps = SeperateToThreeParts(it,10*m_MillToolDiamand/2, m_MillCutDepth_Vertical);
			for (elements its : temps)
			{
				s.push_back(its);
			}
		}
		else
		{
			//it.begin.x += 1200;
			//it.end.x += 1200;
			//it.center.x += 1200;
			s.push_back(it);
		}
	}

	return s;
}

//将直线根据锯片半径及切削深度将直线分成三段
vector<elements> CTableBoardProcess::SeperateToThreeParts(elements LineData , float Radius, float CutDepth)
{
	//只要直线的两段留有半径大小的余量即可？
	vector<elements> s;

	//R-sqrt(R*R-(R-cut)*(R-cut))
	float LeftDis = Radius - sqrtf(pow(Radius, 2) - pow(Radius - CutDepth, 2));

	//确定直线的方向
	CVector3D dir = CVector3D(LineData.end.x - LineData.begin.x, LineData.end.y - LineData.begin.y, 0);
	//单位向量
	CVector3D normal = dir.GetNormal();

	elements e1, e2, e3;
	e1.begin = LineData.begin;
	//e1.end = LineData.begin + normal*LeftDis;
	e1.end = LineData.begin + normal*Radius;
	s.push_back(e1);

	//e2.begin = LineData.begin + normal*LeftDis;
	//e2.end = LineData.end - normal*LeftDis;
	e2.begin = LineData.begin + normal*Radius;
	e2.end = LineData.end - normal*Radius;
	s.push_back(e2);

	//e3.begin = LineData.end - normal*LeftDis;
	e3.begin = LineData.end - normal*Radius;
	e3.end = LineData.end;
	s.push_back(e3);

	return s; 
}

//识别内外轮廓
vector<vector<elements>> CTableBoardProcess::RegonizeProfile(vector<elements> data)
{
	vector<vector<elements>> s;
	//这边只假设只有一个外轮廓，一个内轮廓,如果是多个的话，可以通过更改数组的个数进行相应的调整.
	//默认是先画外轮廓，再画内轮廓，如果想要任意画，这边需要加上判断尺寸，尺寸小的在内，尺寸大的在外。
	//此处未加。
	vector<elements> tempS[2];
	int i = 0;

	if (data.size())
	{
		CVector3D dir = CVector3D(0, 0, 0);
	
		for (elements it : data)
		{
			dir += it.end - it.begin;
			if (dir.GetLength() <0.001)
			{
				tempS[i].push_back(it);

				//添加到集合中
				s.push_back(tempS[i]);

				if (i<1)
					++i;
				//向量归零
				dir = CVector3D(0, 0, 0);

			}
			else
			{
				tempS[i].push_back(it);
			}

		}
	}

	//因为尽量要求是要先加工内轮廓再加工外轮廓，外轮廓存储在0位置，内轮廓存储在1位置。
	for (int i = 0; i < s.size(); ++i)
	{
		if (i == 1)
		{
			m_innerArray = Change_To_Array(s[i]);

		}
		if (i == 0)
		{
			m_outArray = Change_To_Array(s[i]);
		}
	}


	return s;
}


CPtrArray* CTableBoardProcess::Change_To_Array(vector<elements> data)
{
	CPtrArray* tempArray = new CPtrArray();
	for (elements it : data)
	{
		if (it.flag == 1)
		{
			CLine* pLine = new CLine();
			pLine->flag = 1;
			pLine->m_begin = it.begin;
			pLine->m_end = it.end;
			tempArray->Add(pLine);

		}
		else if (it.flag == 2)
		{
			CArc* pArc = new CArc();

			it.flag = 2;
			pArc->m_center = it.center;
			pArc->m_radio = it.radius;
			pArc->m_begin_angle = it.start_angle;
			pArc->m_end_angle = it.end_angle;
			pArc->m_begin = it.begin;
			pArc->m_end = it.end;
			pArc->m_Tudu = it.tudu;//0可以判断是普通圆弧还是多线段圆弧，多线段圆弧的正负可以用来判断圆弧的顺逆。
			pArc->m_Direction = it.arc_direction;

			tempArray->Add(pArc);

		}
	}

	return tempArray;
}

elements CTableBoardProcess::Copy_Elements(elements data)
{
	elements tempElements;

	tempElements.begin = data.begin;
	tempElements.center = data.center;
	tempElements.end = data.end;
	tempElements.end_angle = data.end_angle;
	tempElements.flag = data.flag;
	tempElements.radius = data.radius;
	tempElements.start_angle = data.start_angle;
	tempElements.tudu = data.tudu;


	return tempElements;
}

vector<CPtrArray*> CTableBoardProcess::Copy_CPtrArrays(vector<CPtrArray*> data)
{
	vector<CPtrArray*> m_tempNewArrays;
	for (CPtrArray* it : data)
	{
		CPtrArray* m_tempNewArray = new CPtrArray();
		for (int i = 0; i != it->GetSize(); ++i)
		{
			if (((CLine*)it->GetAt(i))->flag == 1)
			{
				CLine* pLine = new CLine();
				pLine->flag = 1;
				pLine->m_begin = ((CLine*)it->GetAt(i))->m_begin;
				pLine->m_end = ((CLine*)it->GetAt(i))->m_end;

				m_tempNewArray->Add(pLine);

			}
			else if (((CArc*)it->GetAt(i))->flag == 2)
			{
				CArc* pArc = new CArc();

				pArc->flag = 2;
				pArc->m_center = ((CArc*)it->GetAt(i))->m_center;
				pArc->m_radio= ((CArc*)it->GetAt(i))->m_radio;
				pArc->m_begin_angle = ((CArc*)it->GetAt(i))->m_begin_angle;
				pArc->m_end_angle = ((CArc*)it->GetAt(i))->m_end_angle;
				pArc->m_begin = ((CArc*)it->GetAt(i))->m_begin;
				pArc->m_end = ((CArc*)it->GetAt(i))->m_end;
				pArc->m_Tudu = ((CArc*)it->GetAt(i))->m_Tudu;
				pArc->m_Direction = ((CArc*)it->GetAt(i))->m_Direction;

				m_tempNewArray->Add(pArc);

			}
		}
		m_tempNewArrays.push_back(m_tempNewArray);
	}
	return m_tempNewArrays;
}

//离散点
vector<CPoint3D> CTableBoardProcess::ConverToPoints(CPtrArray* data, float F, float T)
{
	vector<CPoint3D> s;

	for (int i = 0; i != data->GetSize(); ++i)
	{
		if (((CLine*)data->GetAt(i))->flag == 1)
		{
			vector<CPoint3D> temps = this->LineDiscrete(((CLine*)data->GetAt(i))->m_begin,
												((CLine*)data->GetAt(i))->m_end,T, F);
			for (CPoint3D it : temps)
			{
				s.push_back(it);
			}
		}
		else if (((CArc*)data->GetAt(i))->flag == 2)
		{
			vector<CPoint3D> temps;

			if (((CArc*)data->GetAt(i))->m_Tudu == 0)
			{
				temps = this->ArcDiscrete(((CArc*)data->GetAt(i))->m_center, ((CArc*)data->GetAt(i))->m_radio,
					((CArc*)data->GetAt(i))->m_begin_angle, ((CArc*)data->GetAt(i))->m_end_angle, *data,i, T, F);
			}
			else
			{
				temps = this->PolyArcDiscrete(((CArc*)data->GetAt(i))->m_begin, ((CArc*)data->GetAt(i))->m_end, 
					                             ((CArc*)data->GetAt(i))->m_Tudu, T, F);
			}
			for (CPoint3D it : temps)
			{
				s.push_back(it);
			}
		}
	}



	return s;
}

vector<vector<CPoint3D>> CTableBoardProcess::GetPoints(vector<CPtrArray*> data, float F, float T)
{
	vector<vector<CPoint3D>> s;
	if (data.size())
	{
		for (CPtrArray* it : data)
		{
			vector<CPoint3D> temp = ConverToPoints(it,F,T);
			s.push_back(temp);
		}
	}
	return s;
}

vector<CPoint3D> CTableBoardProcess::GetPoints(CPtrArray* data, float F, float T)
{
	return ConverToPoints(data, F, T);
}

vector<vector<CPoint3D>> CTableBoardProcess::CoverOutsideProfToPoints(vector<CPtrArray*> data,float offset, float F, float T)
{
	vector<vector<CPoint3D>> s;
	this->SetFeedspeed(F);
	this->SetTime(T);
	for (CPtrArray* it : data)
	{
		vector<CPoint3D> ss;
		ss = OffsetAlgorithm(*it, it->GetSize() - 1, offset);
		for (CPoint3D& its : ss)
		{
			if (((CLine*)it->GetAt(0))->flag == 1)
			{
				its.z = ((CLine*)it->GetAt(0))->m_begin.z;
			}
			else if (((CArc*)it->GetAt(0))->flag ==2)
			{
				its.z = ((CArc*)it->GetAt(0))->m_center.z;
			}
		}
		s.push_back(ss);
	}
	return s;
}

vector<vector<CPoint3D>> CTableBoardProcess::CoverOutsideProfToPoints(vector<CPtrArray*> data, float F, float T)
{
	vector<vector<CPoint3D>> s;
	for (CPtrArray* it : data)
	{
		vector<CPoint3D> ss;
		//ss = OffsetAlgorithm(*it, it->GetSize() - 1, offset);
		for (int i = 0; i!=it->GetSize(); ++i)
		{
			vector<CPoint3D> sss;
			if (((CLine*)it->GetAt(i))->flag == 1)
			{
				sss = LineDiscrete(((CLine*)it->GetAt(i))->m_begin, ((CLine*)it->GetAt(i))->m_end, T, F);
			}
			else if (((CArc*)it->GetAt(i))->flag == 2)
			{
				sss = ((CArc*)it->GetAt(i))->ArcTDMInterpolation(((CArc*)it->GetAt(i))->m_center, ((CArc*)it->GetAt(i))->m_radio,
					((CArc*)it->GetAt(i))->m_Direction, ((CArc*)it->GetAt(i))->m_begin, ((CArc*)it->GetAt(i))->m_end, T, F);
			}
			for (CPoint3D its : sss)
			{
				ss.push_back(its);
			}
		}

		for (CPoint3D& its : ss)
		{
			if (((CLine*)it->GetAt(0))->flag == 1)
			{
				its.z = ((CLine*)it->GetAt(0))->m_begin.z;
			}
			else if (((CArc*)it->GetAt(0))->flag == 2)
			{
				its.z = ((CArc*)it->GetAt(0))->m_center.z;
			}
		}
		s.push_back(ss);
	}
	return s;
}

//毛坯显示
void CTableBoardProcess::InitTableBlank()
{
	if (m_ProfileData.size())
	{
		vector<elements> m_tempElements;//获取外轮廓的元素信息
		for (elements it : m_ProfileData[0])
		{
			if (it.flag == 1)
			{
				m_tempElements.push_back(it);
			}
		}

		vector<elements> m_tempElementss;//获取内轮廓的元素信息
		for (elements it : m_ProfileData[1])
		{
			if (it.flag == 1)
			{
				m_tempElementss.push_back(it);
			}
		}


		m_outOnlyLineArray = Change_To_Array(m_tempElements);
		m_inOnlyLineArray = Change_To_Array(m_tempElementss);

		/*
		vector<vector<CPoint3D>>().swap(m_OuterBlankPoints);
		m_OuterBlankPoints.push_back(OffsetAlgorithm(*m_outOnlyLineArray, m_outOnlyLineArray->GetSize() - 1, 30));*/
		m_OuterBlankOffset = OffsetShapeAlgorithm(*m_outOnlyLineArray, m_outOnlyLineArray->GetSize() - 1, m_OutLeft);
		m_InBlankOffset = OffsetShapeAlgorithm(*m_inOnlyLineArray, m_inOnlyLineArray->GetSize() - 1, -m_InLeft);
		for (OffsetPoint it : m_OuterBlankOffset)
		{
			OffsetPoint its = it;
			its.begin.z = m_Board_Heigh;
			its.end.z = m_Board_Heigh;
			m_OuterBlankTopOffset.push_back(its);
		}
		for (OffsetPoint it : m_InBlankOffset)
		{
			OffsetPoint its = it;
			its.begin.z = m_Board_Heigh;
			its.end.z = m_Board_Heigh;
			m_InBlankTopOffset.push_back(its);
		}

	}
}

vector<OffsetPoint> CTableBoardProcess::OffsetShapeAlgorithm(CPtrArray& XFarray, int mark, double offset)
{
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

	return points;
}

void CTableBoardProcess::RenderTableBlank(COpenGLDC* pDC)
{
	//DrawDIYFromPt(m_OuterBlankPoints, pDC);
	//判断里面有内容才绘制
	if (m_OuterBlankOffset.size() && m_InBlankOffset.size())
	{
		//pDC->SetMaterialColor(RGB(0,255,0));
		DrawDIYFromOffset(m_OuterBlankOffset, pDC);
		DrawDIYFromOffset(m_InBlankOffset, pDC);
		for (OffsetPoint it : m_OuterBlankOffset)
		{
			pDC->DrawLine(it.begin, CPoint3D(it.begin.x, it.begin.y, m_Board_Heigh));

		}
		for (OffsetPoint it : m_InBlankOffset)
		{
			pDC->DrawLine(it.begin, CPoint3D(it.begin.x, it.begin.y, m_Board_Heigh));

		}
		DrawDIYFromOffset(m_OuterBlankTopOffset, pDC);
		DrawDIYFromOffset(m_InBlankTopOffset, pDC);
	}
}

//外侧刀路轨迹
void CTableBoardProcess::InitTableOutPath()
{
	/*
	通过设置外围的锯片每层进刀量和毛坯的厚度，确定需要加工的层数。
	不用先前的外侧面加工的，沿着毛坯走一圈的方式。
	通过OffsetShapeAlgorithm将CptrArray*偏置转成成vector<elements>的数据类型，
	再将vector<elements>中的元素通过chang_To_Array转换成CPtrArray*。
	通过获取偏置的vector<vector<CPtrArray*>>，再用方法CoverOutsideProfToPoints将vector<CPtrArray*>转为点，
	存储到vector<vector<vector<CPoint3D>>>中去，最后将点进行输出显示即可。
	*/
	int n =m_OutLeft/m_B_FeedDis_Horizon ;

	for (int i = n; i !=-1; i--)
	{
		vector<elements> m_tempElements = OffsetOutPathAlgorithm(*m_outArray, m_outArray->GetSize() - 1,m_B_FeedDis_Horizon*i);
		
		//m_tempElements = InsertExtendLineElements(m_tempElements, 400);


		CPtrArray* m_tempOut = Change_To_Array(m_tempElements);

		vector<CPtrArray*> m_tempOutArrays = this->InitTableModel(m_tempOut, m_Board_Heigh, m_B_FeedLoad_Vertical/1000);

		//做个替代，因为是指针，其所对应的内容随着计算改变而改变
		vector<CPtrArray*> mm_tempOutArrays = Copy_CPtrArrays(m_tempOutArrays);
		//将外轮廓的偏置进行存储
		m_OutOffsetProfile.push_back(m_tempOutArrays);

		vector<vector<CPoint3D>> m_tempPTS;
		if (i == 0)
		{
			m_tempPTS = GetPoints(InitTableModel(Change_To_Array(m_ProfileData[0]), m_Board_Heigh, m_B_FeedLoad_Vertical/1000),
				m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		}
		else
		{
		
		//此处CoverOutsideProfToPoints里的offset参数要设置为0，要不然会重复计算偏置
		/* m_tempPTS = CoverOutsideProfToPoints(mm_tempOutArrays, 0, 
			                          m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);*///******0819*****
			m_tempPTS = CoverOutsideProfToPoints(mm_tempOutArrays,
				m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		}
		m_OutOffsetPoints.push_back(m_tempPTS);
	}
}

vector<elements> CTableBoardProcess::InsertExtendLineElements(vector<elements> data, float extendDis)
{
	//判断首尾,并插入直线
	if (data[0].flag == 1)
	{
		elements ee;
		ee.flag = 1;
		//获取第一条直线的方向向量
		CVector3D bd = data[0].end - data[0].begin;

		ee.end = data[0].begin;
		ee.begin = ee.end - bd.GetNormal()*extendDis;

		data.insert(data.begin(), ee);

	}
	else if (data[0].flag == 2)
	{

	}

	int m_DataSize = data.size();

	if (data[m_DataSize - 1].flag == 1)
	{
		elements ee;
		ee.flag = 1;

		//获取倒数第一条数据的方向向量
		CVector3D bd = data[m_DataSize - 1].end - data[m_DataSize - 1].begin;

		ee.begin = data[m_DataSize - 1].end;

		ee.end = ee.begin + bd.GetNormal()*extendDis;

		data.insert(data.end(), ee);

	}
	else if (data[m_DataSize - 1].flag == 2)
	{

	}


	return data;
}

vector<elements> CTableBoardProcess::OffsetOutPathAlgorithm(CPtrArray& XFarray, int mark, double offset)
{
	vector<elements> points;//存储每个图素偏移后的起始点和终点
	for (int i = 0; i != mark + 1; ++i)
	{
		if (((CLine*)XFarray.GetAt(i))->flag == 1)
		{
			elements temp;
			temp.flag = 1;

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
						//************************0810**************************顺时针
						if (!((CArc*)XFarray.GetAt(i + 1))->Calculate(((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio,
							((CArc*)XFarray.GetAt(i + 1))->m_begin_angle, ((CArc*)XFarray.GetAt(i + 1))->m_end_angle, XFarray, i + 1))
						{
							Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
								((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio + offset, mark, XFarray, i);

						}
						else//*********0810************逆时针
						{
							Not = CLine::IntersectOfArc(((CLine*)XFarray.GetAt(i))->m_begin + Roat1.GetNormal()*offset,
								((CLine*)XFarray.GetAt(i))->m_end + Roat1.GetNormal()*offset, ((CArc*)XFarray.GetAt(i + 1))->m_center, ((CArc*)XFarray.GetAt(i + 1))->m_radio - offset, mark, XFarray, i);
						}
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
			elements temp;
			temp.flag = 2;

			CPoint3D Not;

			if (((CArc*)XFarray.GetAt(i))->m_Tudu == 0)
			{
				BOOL dddd=((CArc*)XFarray.GetAt(i))->Calculate(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio,
					((CArc*)XFarray.GetAt(i))->m_begin_angle, ((CArc*)XFarray.GetAt(i))->m_end_angle, XFarray, i);//计算起始点及终止点

				temp.center = ((CArc*)XFarray.GetAt(i))->m_center;
				temp.start_angle = ((CArc*)XFarray.GetAt(i))->m_begin_angle;
				temp.end_angle = ((CArc*)XFarray.GetAt(i))->m_end_angle;
				//temp.radius = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
				temp.tudu = 0;
				temp.arc_direction = ((CArc*)XFarray.GetAt(i))->m_Direction;

				//************************0810**************************
				if (!dddd)
				{
					temp.radius = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
				}
				else
				{
					temp.radius = ((CArc*)XFarray.GetAt(i))->m_radio - offset;
				}
				/*
				if (dddd)
				{
					offset = -offset;//*********0818************
				}	
				temp.radius = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
				*/
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

						if (dddd)//逆时针*****0818*******
						{//逆时针*****0818*******
							Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio - offset,
								((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
						}//逆时针*****0818*******
						else//*****0818*******
						{//*****0818*******
							Not = CArc::IntersectOfLine(((CArc*)XFarray.GetAt(i))->m_center, ((CArc*)XFarray.GetAt(i))->m_radio + offset,
								((CLine*)XFarray.GetAt(i + 1))->m_begin + Roat1.GetNormal()*offset, ((CLine*)XFarray.GetAt(i + 1))->m_end + Roat1.GetNormal()*offset, mark, XFarray, i);
						}//*****0818*******
						
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

				temp.center = ((CArc*)XFarray.GetAt(i))->m_center;
				temp.radius = ((CArc*)XFarray.GetAt(i))->m_radio + offset;
				temp.tudu = ((CArc*)XFarray.GetAt(i))->m_Tudu;
				temp.start_angle = ((CArc*)XFarray.GetAt(i))->m_begin_angle;
				temp.end_angle = ((CArc*)XFarray.GetAt(i))->m_end_angle;
			

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

	return points;
}


void CTableBoardProcess::RenderTableOutPath(COpenGLDC* pDC)
{
	

	for (vector<vector<vector<CPoint3D>>>::iterator it = m_OutOffsetPoints.begin(); it != m_OutOffsetPoints.end(); ++it)
	{
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end()-1; ++itss)
			{
				pDC->DrawLine(*itss, *(itss + 1));
			}
		}
	}
	
	
	DrawOutRetractLine(m_OutOffsetPoints, pDC);

}

//内侧刀路轨迹
void CTableBoardProcess::InitTableInPath()
{
	int n = m_InLeft / m_B_FeedDis_Horizon;

	for (int i = n; i != -1; i--)
	{
		//获取插入元素后的数据信息
		vector<elements> m_tempElements = NewInsertElement(OffsetOutPathAlgorithm(*m_innerArray, m_innerArray->GetSize() - 1, -1 * m_B_FeedDis_Horizon*i));
		
		CPtrArray* m_tempIn = Change_To_Array(m_tempElements);

		vector<CPtrArray*> m_tempInArrays = this->InitTableModel(m_tempIn, m_Board_Heigh, m_B_FeedLoad_Vertical / 1000);
		//得到内轮廓偏置数据
		m_inOffsetProfile.push_back(m_tempInArrays);

		/*
		vector<vector<CPoint3D>> m_tempPTS;
		if (i == 0)
		{
			m_tempPTS = GetPoints(InitTableModel(Change_To_Array(m_ProfileData[1]), m_Board_Heigh, m_B_FeedLoad_Vertical / 1000),
				m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		}
		else
		{

			//此处CoverOutsideProfToPoints里的offset参数要设置为0，要不然会重复计算偏置
			m_tempPTS = CoverOutsideProfToPoints(m_tempInArrays, 0,
				m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
		}
		m_inOffsetPoints.push_back(m_tempPTS);*/
	}

	InitInArray();

	//锯片点的离散化,点指的是刀尖点
	for (vector<vector<vector<CPtrArray*>>>::iterator it = m_InOffsetSawProfile.begin(); it != m_InOffsetSawProfile.end(); ++it)
	{
		vector<vector<vector<CPoint3D>>> mmm_tempBoardPts;
		for (vector<vector<CPtrArray*>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<vector<CPoint3D>> mm_tempBoardPts;
			for (vector<CPtrArray*>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				vector<CPoint3D> m_tempBoardPts = this->GetPoints(*itss, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
				mm_tempBoardPts.push_back(m_tempBoardPts);
			}
			mmm_tempBoardPts.push_back(mm_tempBoardPts);
		}
		m_InOffsetSawPoints.push_back(mmm_tempBoardPts);

	}
	/*需要根据铣刀设置的每刀切削深度进行分层，也就是要根据设置的值，将m_InOffsetMillProfile中的Z的坐标进行重新的分配。
	因为铣刀加工需要对切削深度进行线性下降加工，返程的时候直接用原先的轨迹进行加工即可*/
	
	vector<vector<vector<vector<CPoint3D>>>> m_tempInOffsetMillPoints;
	//铣刀点的离散化
	for (vector<vector<vector<CPtrArray*>>>::iterator it = m_InOffsetMillProfile.begin(); it != m_InOffsetMillProfile.end(); ++it)
	{
		vector<vector<vector<CPoint3D>>> mmm_tempBoardPts;
		for (vector<vector<CPtrArray*>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<vector<CPoint3D>> mm_tempBoardPts;
			for (vector<CPtrArray*>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				vector<CPoint3D> m_tempBoardPts = this->GetPoints(*itss, m_B_FeedSpeed_Horizon, m_B_FeedTime_Horizon);
				mm_tempBoardPts.push_back(m_tempBoardPts);
			}
			mmm_tempBoardPts.push_back(mm_tempBoardPts);
		}

		m_tempInOffsetMillPoints.push_back(mmm_tempBoardPts);
	}

	//假设铣刀每刀切10mm,对临时存储点进行修改
	m_InOffsetMillPoints = ChangesToMillPoints(m_tempInOffsetMillPoints, m_MillCutDepth_Vertical);

}

vector<vector<vector<CPtrArray*>>> CTableBoardProcess::ChangesToMillProfiles(vector<vector<vector<CPtrArray*>>> data, float high, float depth)
{
	vector<vector<vector<CPtrArray*>>> datas;

	for (vector<vector<vector<CPtrArray*>>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		vector < vector<CPtrArray*>> m_tempDATA;
		for (vector<vector<CPtrArray*>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<CPtrArray*> mm_tempDATA = InitTableModel(*(its->begin()), high, depth);
			m_tempDATA.push_back(mm_tempDATA);
		}
		datas.push_back(m_tempDATA);
	}

	return datas;
}
vector<vector<vector<vector<CPoint3D>>>> CTableBoardProcess::ChangesToMillPoints(vector<vector<vector<vector<CPoint3D>>>> data, float m_MillDepth)
{
	vector<vector<vector<vector<CPoint3D>>>> s;
	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = data.begin(); it != data.end(); ++it)//哪一部分
	{

		vector<vector<vector<CPoint3D>>> ss;
		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)//哪一层
		{
			vector<vector<CPoint3D>> sss;
			BOOL DIRECTION = TRUE;//True表示需要进行转换
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪一个高度
			{
				/*需要进行修改，11个点有10段，需要的数据有二十段*/
				/*
				vector<CPoint3D> ssss;
				if (!DIRECTION)//不做转换
				{
					for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//哪一条直线
					{
						ssss.push_back(*itsss);//添加点即可
					}
				}
				else//做转换
				{
					//获取点的总数
					int m_TotalSize=itss->size();
					float m_PerDownDis = m_MillDepth / m_TotalSize;
					int m_COUNT = 0;
					if (itss==its->begin())//第一层
					{
						for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)
						{
							itsss->z -= m_COUNT*m_PerDownDis;
							ssss.push_back(*itsss);
							m_COUNT += 1;
						}
					}
					else//中间及最后一层
					{
						for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)
						{
							itsss->z = itsss->z-m_COUNT*m_PerDownDis+m_MillDepth;
							ssss.push_back(*itsss);
							m_COUNT+=1;
						}
					}
				}
				sss.push_back(ssss);
				DIRECTION = !DIRECTION;*/
				vector<CPoint3D> ssss;
				vector<CPoint3D> ssss_s;
				m_TotalSize = itss->size();
				m_PerDownDis = m_MillDepth / m_TotalSize;
				int m_COUNT = 1;
				if (itss != its->begin())//非最高的那一层
				{
					for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)
					{
						CPoint3D m_tempPTTS = *itsss;
						m_tempPTTS.z= m_tempPTTS.z-m_COUNT*m_PerDownDis + m_MillDepth;
						ssss.push_back(m_tempPTTS);//添加点即可
						m_COUNT += 1;
					}
					for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//添加直线点即可
					{
						ssss_s.push_back(*itsss);//添加点即可
					}
				sss.push_back(ssss);
				sss.push_back(ssss_s);
				}
				
			}
			ss.push_back(sss);
		}
		s.push_back(ss);
	}


	return s;
}

void CTableBoardProcess::InitInArray()
{
	//获取内轮廓偏置后的CPtrArray*集合
	//锯片加工从上往下，从内往外
	//锯片
	vector<vector<CPtrArray*>> m_S1;
	vector<vector<CPtrArray*>> m_S5;
	vector<vector<CPtrArray*>> m_S9;
	vector<vector<CPtrArray*>> m_S13;

	//铣刀
	vector<vector<CPtrArray*>> m_S2_4;
	vector<vector<CPtrArray*>> m_S6_8;
	vector<vector<CPtrArray*>> m_S10_12;
	vector<vector<CPtrArray*>> m_S14_0;


	//暂存数据
	vector<vector<vector<CPtrArray*>>> m_tempInOffsetMillProfile;

	//从内往外
	//每一层轮廓
	for (vector<vector<CPtrArray*>>::iterator it = m_inOffsetProfile.begin(); it != m_inOffsetProfile.end(); ++it)
	{
		//从上往下
		vector<CPtrArray*> mm_S1;
		vector<CPtrArray*> mm_S5;
		vector<CPtrArray*> mm_S9;
		vector<CPtrArray*> mm_S13;

		vector<CPtrArray*> mm_S2_4;
		vector<CPtrArray*> mm_S6_8;
		vector<CPtrArray*> mm_S10_12;
		vector<CPtrArray*> mm_S14_0;

		//vector<vector<vector<CPtrArray*>>> m_tempInOffsetSawProfile;
		//vector<vector<vector<CPtrArray*>>> m_tempInOffsetMillProfile;

		//每一个高度上的轮廓
		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
		{

			//每一个轮廓
			for (int i = 0; i != (*its)->GetSize(); ++i)
			{
				CPtrArray* m_testArray = new CPtrArray();
				vector<int> m_testCount;

				if (i == pos[0])
				{
					m_testCount.push_back(i);
					m_testArray = CopyData(*its, m_testCount);
					mm_S1.push_back(m_testArray);

				}
				else if (i == pos[1])
				{
					m_testCount.push_back(i);
					m_testArray = CopyData(*its, m_testCount);
					mm_S5.push_back(m_testArray);
				}
				else if (i == pos[2])
				{
					m_testCount.push_back(i);
					m_testArray = CopyData(*its, m_testCount);
					mm_S9.push_back(m_testArray);
				}
				else if (i == pos[3])
				{
					m_testCount.push_back(i);
					m_testArray = CopyData(*its, m_testCount);
					mm_S13.push_back(m_testArray);
				}
				else if (i > pos[0] && i < pos[1])
				{
					if (i == pos[0] + 1)
					{
						for (int j = pos[0] + 1; j != pos[1]; ++j)
						{
							m_testCount.push_back(j);
						}
						m_testArray = CopyData(*its, m_testCount);
						mm_S2_4.push_back(m_testArray);

					}
					else
						continue;
				}
				else if (i>pos[1] && i < pos[2])
				{
					if (i == pos[1] + 1)
					{
						for (int j = pos[1] + 1; j != pos[2]; ++j)
						{
							m_testCount.push_back(j);
						}
						m_testArray = CopyData(*its, m_testCount);
						mm_S6_8.push_back(m_testArray);
					}
					else
						continue;
				}
				else if (i>pos[2] && i < pos[3])
				{
					if (i == pos[2] + 1)
					{
						for (int j = pos[2] + 1; j != pos[3]; ++j)
						{
							m_testCount.push_back(j);
						}
						m_testArray = CopyData(*its, m_testCount);
						mm_S10_12.push_back(m_testArray);
					}
					else
						continue;
				}
				else if (i>pos[3])//最后一组要加上第一个为0的数据
				{
					if (i == pos[3] + 1)
					{
						for (int j = pos[3] + 1; j != (*its)->GetSize(); ++j)
						{
							m_testCount.push_back(j);
						}
						//加上最后一个
						m_testCount.push_back(0);
						m_testArray = CopyData(*its, m_testCount);
						mm_S14_0.push_back(m_testArray);
					}
					else
						continue;
				}
			}

		}

		m_S1.push_back(mm_S1);
		m_S5.push_back(mm_S5);
		m_S9.push_back(mm_S9);
		m_S13.push_back(mm_S13);


		m_S2_4.push_back(mm_S2_4);
		m_S6_8.push_back(mm_S6_8);
		m_S10_12.push_back(mm_S10_12);
		m_S14_0.push_back(mm_S14_0);


	}
	m_InOffsetSawProfile.push_back(m_S1);
	m_InOffsetSawProfile.push_back(m_S5);
	m_InOffsetSawProfile.push_back(m_S9);
	m_InOffsetSawProfile.push_back(m_S13);


	m_tempInOffsetMillProfile.push_back(m_S2_4);
	m_tempInOffsetMillProfile.push_back(m_S6_8);
	m_tempInOffsetMillProfile.push_back(m_S10_12);
	m_tempInOffsetMillProfile.push_back(m_S14_0);
	//暂且假设铣刀切削每刀下刀为10mm
	m_InOffsetMillProfile = ChangesToMillProfiles(m_tempInOffsetMillProfile,m_Board_Heigh,m_MillCutDepth_Vertical);
	/*
	m_InOffsetMillProfile.push_back(m_S2_4);
	m_InOffsetMillProfile.push_back(m_S6_8);
	m_InOffsetMillProfile.push_back(m_S10_12);
	m_InOffsetMillProfile.push_back(m_S14_0);*/
}

CPtrArray* CTableBoardProcess::CopyData(CPtrArray* data, vector<int> number)
{
	CPtrArray* s=new CPtrArray();
	for (int i : number)
	{	
		if (((CLine*)data->GetAt(i))->flag == 1)
		{
			CLine* pLine = new CLine();
			s->Add(pLine);
			pLine->flag = ((CLine*)data->GetAt(i))->flag;
			pLine->m_begin = ((CLine*)data->GetAt(i))->m_begin;
			pLine->m_end = ((CLine*)data->GetAt(i))->m_end;
		}
		else if (((CArc*)data->GetAt(i))->flag == 2)
		{
			CArc* pArc = new CArc();
			s->Add(pArc);

			pArc->flag = ((CArc*)data->GetAt(i))->flag;
			pArc->m_center = ((CArc*)data->GetAt(i))->m_center;
			pArc->m_radio = ((CArc*)data->GetAt(i))->m_radio;
			pArc->m_begin_angle = ((CArc*)data->GetAt(i))->m_begin_angle;
			pArc->m_end_angle = ((CArc*)data->GetAt(i))->m_end_angle;

			pArc->m_begin = ((CArc*)data->GetAt(i))->m_begin;
			pArc->m_end = ((CArc*)data->GetAt(i))->m_end;
			pArc->m_Tudu = ((CArc*)data->GetAt(i))->m_Tudu;


		}
	}
	return s;
}


vector<elements> CTableBoardProcess::NewInsertElement(vector<elements> data)
{
	vector<elements> s;

	for (vector<elements>::iterator it =data.begin(); it != data.end(); ++it)
	{

		if (it->flag == 1)
		{
			vector<elements> m_cutElements =SeperateToThreeParts(*it, 10*m_MillToolDiamand/2, m_MillCutDepth_Horizon);
			for (elements ss : m_cutElements)
			{
				s.push_back(ss);
			}
		}
		else
		{
			s.push_back(*it);
		}

	}
	int p = 0;
	for (int i = 0; i != s.size(); ++i)
	{
		
		if (i >= 1 && i <= s.size() -2)
		{
			if (s[i].flag == 1 && s[i + 1].flag == 1 && s[i - 1].flag == 1)
			{
				pos[p] = i;
				p++;
			}
		}
	}

	return s;
}
void CTableBoardProcess::RenderTableSawPath(COpenGLDC* pDC)
{
	DrawDIYFromPt(m_InOffsetSawPoints,pDC);
	DrawInSawRetractLine(m_InOffsetSawPoints, pDC);
}
void CTableBoardProcess::RenderTableMillPath(COpenGLDC* pDC)
{
	DrawDIYFromPt(m_InOffsetMillPoints, pDC);
	DrawInMillRetractLine(m_InOffsetMillPoints, pDC);
}

//初始化仿真点
void CTableBoardProcess::InitTableSimulationPoints(vector<vector<vector<CPoint3D>>> m_outPts,
	vector<vector<vector<vector<CPoint3D>>>> m_inSaw, vector<vector<vector<vector<CPoint3D>>>> m_inMill)
{
	/*
	按照先内后外的加工顺序进行加工工件
	先进行锯片加工后进行铣加工
	*/
	//清空以往的仿真数据点
	vector<CPoint3D>().swap(m_BoardRoughSimulation);
	vector<CPoint3D>().swap(m_BoardFinishSimulation);
	
	//对内侧锯片
	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = m_inSaw.begin(); it != m_inSaw.end(); ++it)//哪块
	{
		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)//哪层
		{
			BOOL m_Direction = TRUE;//true为默认顺时针方向，false为逆时针方向
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪高度
			{
				//需在起点添加一个点，终点添加一个点
				CVector3D m_Linedirection = *(its->begin()->end() - 1) - *(its->begin()->begin());//直线的方向
				CVector3D m_NRotateLine = CVector3D(m_Linedirection.dy, -m_Linedirection.dx, 0).GetNormal();

				if (m_Direction)
				{
					m_BoardRoughSimulation.push_back(*(itss->begin()) + m_NRotateLine*m_ExtendLength);
					m_BoardFinishSimulation.push_back(*(itss->begin()) + m_NRotateLine*m_ExtendLength);
					for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//哪点
					{
						m_BoardRoughSimulation.push_back(*itsss);
						m_BoardFinishSimulation.push_back(*itsss);
					}
					m_BoardRoughSimulation.push_back(*(itss->end() - 1) + m_NRotateLine * m_ExtendLength);
					m_BoardFinishSimulation.push_back(*(itss->end() - 1) + m_NRotateLine * m_ExtendLength);
				}
				else
				{
					m_BoardRoughSimulation.push_back(*(itss->end() - 1) + m_NRotateLine * m_ExtendLength);
					m_BoardFinishSimulation.push_back(*(itss->end() - 1) + m_NRotateLine * m_ExtendLength);
					for (vector<CPoint3D>::reverse_iterator itsss = itss->rbegin(); itsss != itss->rend(); ++itsss)//哪点
					{
						m_BoardRoughSimulation.push_back(*itsss);
						m_BoardFinishSimulation.push_back(*itsss);
					}
					m_BoardRoughSimulation.push_back(*(itss->begin()) + m_NRotateLine*m_ExtendLength);
					m_BoardFinishSimulation.push_back(*(itss->begin()) + m_NRotateLine*m_ExtendLength);
				}
				//最后一个部分最后一层的最低高度处
				if (it == m_inSaw.end() - 1 && its == it->end() - 1 && itss == its->end() - 1)
				{
					CPoint3D tempPTS = *(m_BoardRoughSimulation.end() - 1);
					m_BoardRoughSimulation.push_back(CPoint3D(tempPTS.x, tempPTS.y, 2 * m_Board_Heigh));
					m_BoardFinishSimulation.push_back(CPoint3D(tempPTS.x, tempPTS.y, 2 * m_Board_Heigh));
				}
				m_Direction = !m_Direction;
			}
		}
	}
	
	//对内侧铣刀
	
	for (vector<vector<vector<vector<CPoint3D>>>>::iterator it = m_inMill.begin(); it != m_inMill.end(); ++it)//哪块
	{
		for (vector<vector<vector<CPoint3D>>>::iterator its = it->begin(); its != it->end(); ++its)//哪层
		{
			BOOL m_Direction = TRUE;
			if (its == it->begin())
			{
				m_BoardRoughSimulation.push_back(CPoint3D(its->begin()->begin()->x, its->begin()->begin() ->y,m_Board_Heigh*2));
				m_BoardFinishSimulation.push_back(CPoint3D(its->begin()->begin()->x, its->begin()->begin()->y, m_Board_Heigh * 2));
			}
			for (vector<vector<CPoint3D>>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪高度
			{
				if (m_Direction)
				{
					for (vector<CPoint3D>::iterator itsss = itss->begin(); itsss != itss->end(); ++itsss)//哪点
					{
						m_BoardRoughSimulation.push_back(*itsss);
						m_BoardFinishSimulation.push_back(*itsss);
						//最后一层的最低一个高度的最后一个点,最后一个点抬高到2*m_board_high高度
						if (its == it->end() - 1 && itss == its->end() - 1&&itsss==itss->end()-1)
						{
							m_BoardRoughSimulation.push_back(CPoint3D(itsss->x,itsss->y,2*m_Board_Heigh));
							m_BoardFinishSimulation.push_back( CPoint3D(itsss->x, itsss->y, 2 * m_Board_Heigh));
						}
					}
				}
				else
				{
					for (vector<CPoint3D>::reverse_iterator itsss = itss->rbegin(); itsss != itss->rend(); ++itsss)
					{
						m_BoardRoughSimulation.push_back(*itsss);
						m_BoardFinishSimulation.push_back(*itsss);
						//最后一层的最低一个高度的最后一个点
						if (its == it->end() - 1 && itss == its->end() - 1 && itsss==itss->rend() - 1)
						{
							m_BoardRoughSimulation.push_back(CPoint3D(itsss->x, itsss->y, 2 * m_Board_Heigh));
							m_BoardFinishSimulation.push_back(CPoint3D(itsss->x, itsss->y, 2 * m_Board_Heigh));
						}
					}
				}
				
				m_Direction = !m_Direction;
			}
			
		}
	}
	
	//对外侧锯片
	for (vector<vector<vector<CPoint3D>>>::iterator it = m_outPts.begin(); it != m_outPts.end(); ++it)//哪层
	{
		BOOL m_Direction = TRUE;
		for (vector<vector<CPoint3D>>::iterator its = it->begin(); its != it->end(); ++its)//哪高度
		{
			if (m_Direction)
			{
				CVector3D m_LineDirection = *(its->begin() + 1) - *(its->begin());
				CPoint3D m_RetractBegin = *(its->begin()) - m_LineDirection.GetNormal() * m_ExtendLength;
				m_BoardRoughSimulation.push_back(m_RetractBegin);
				m_BoardFinishSimulation.push_back(m_RetractBegin);

				for (vector<CPoint3D>::iterator itss = its->begin(); itss != its->end(); ++itss)//哪点
				{
					m_BoardRoughSimulation.push_back(*itss);
					m_BoardFinishSimulation.push_back(*itss);
				}
				m_LineDirection = *(its->end() - 1) - *(its->end() - 2);
				CPoint3D m_RetractEnd = *(its->end() - 1) + m_LineDirection.GetNormal() * m_ExtendLength;
				m_BoardRoughSimulation.push_back(m_RetractEnd);
				m_BoardFinishSimulation.push_back(m_RetractEnd);
			}
			else
			{
				CVector3D m_LineDirection = *(its->end() - 1) - *(its->end() - 2);
				CPoint3D m_RetractBegin = *(its->end() - 1)+m_LineDirection.GetNormal() * m_ExtendLength;
				m_BoardRoughSimulation.push_back(m_RetractBegin);
				m_BoardFinishSimulation.push_back(m_RetractBegin);
				for (vector<CPoint3D>::reverse_iterator itss = its->rbegin(); itss != its->rend(); ++itss)//哪点
				{
					m_BoardRoughSimulation.push_back(*itss);
					m_BoardFinishSimulation.push_back(*itss);
				}
				m_LineDirection = *(its->begin() + 1) - *(its->begin());
				CPoint3D m_RetractEnd = *(its->begin())-m_LineDirection.GetNormal() * m_ExtendLength;
				m_BoardRoughSimulation.push_back(m_RetractEnd);
				m_BoardFinishSimulation.push_back(m_RetractEnd);
			}
			m_Direction = !m_Direction;
			
		}
	}
}


//NC代码数据的初始化
//内部锯片
vector<vector<vector<vector<elements>>>> CTableBoardProcess::InitTableInSawNC(vector<vector<vector<CPtrArray*>>> data)
{
	vector<vector<vector<vector<elements>>>> s;

	for (vector<vector<vector<CPtrArray*>>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		vector<vector<vector<elements>>> ss;
		for (vector<vector<CPtrArray*>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<vector<elements>> sss;
			for (vector<CPtrArray*>::iterator itss = its->begin(); itss != its->end(); ++itss)
			{
				vector<elements> ssss = CalculateData(*itss);
				//需要在首尾插入进退刀线
				CVector3D m_DirectionLine = ssss.begin()->end-ssss.begin()->begin;
				CVector3D m_NRotateLine = CVector3D(m_DirectionLine.dy, -m_DirectionLine.dx, 0).GetNormal();

				elements e1, e2;
				e1.begin = ssss.begin()->begin + m_NRotateLine*m_ExtendLength;
				e1.end = ssss.begin()->begin;
				e2.begin = (ssss.end() - 1)->end;
				e2.end = (ssss.end() - 1)->end + m_NRotateLine*m_ExtendLength;

				//首尾插入点
				ssss.insert(ssss.begin(), e1);
				ssss.insert(ssss.end(), e2);

				sss.push_back(ssss);
			}
			ss.push_back(sss);
		}
		s.push_back(ss);
	}

	return s;
}
//内部铣刀
vector<vector<vector<vector<elements>>>> CTableBoardProcess::InitTableInMillNC(vector<vector<vector<CPtrArray*>>> data)
{
	vector<vector<vector<vector<elements>>>> s;

	for (vector<vector<vector<CPtrArray*>>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		vector<vector<vector<elements>>> ss;
		for (vector<vector<CPtrArray*>>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<vector<elements>> sss;
			//除去第一层进行操作
			for (vector<CPtrArray*>::iterator itss = its->begin()+1; itss != its->end(); ++itss)
			{
				vector<elements> ssss = CalculateData(*itss);
				vector<elements> m_ssss;
				//对Z坐标进行改变,并存储数据
				for (vector<elements>::iterator itsss = ssss.begin(); itsss!= ssss.end(); ++itsss)
				{
					elements e;
					e = *itsss;
					if (itsss->flag == 1)//直线
					{
						vector<CPoint3D> m_PointSum = LineDiscrete(itsss->begin, itsss->end, m_B_FeedTime_Horizon, m_B_FeedSpeed_Horizon);

						if (itsss != ssss.end()-1)//非最后一个改变的是终点
						{	
							e.begin.z = e.begin.z + m_MillCutDepth_Vertical;
							e.end.z = e.end.z - m_PerDownDis*m_PointSum.size() + m_MillCutDepth_Vertical;
						}
						else//最后一个改变的是起点
						{
							e.begin.z = e.begin.z + m_PerDownDis*m_PointSum.size();
						}
					}
					else if (itsss->flag == 2)//圆弧
					{
						if (itsss != ssss.end() - 1)//非倒数第一个
						{
							vector<CPoint3D> m_PointSum = LineDiscrete((itsss-1)->begin, (itsss-1)->end, m_B_FeedTime_Horizon, m_B_FeedSpeed_Horizon);
							e.begin = (itsss - 1)->end;//圆弧的起点改为上一个元素的终点
							e.begin.z = (itsss - 1)->end.z - m_PerDownDis*m_PointSum.size() + m_MillCutDepth_Vertical;
							m_PointSum = LineDiscrete((itsss + 1)->begin, (itsss +1)->end, m_B_FeedTime_Horizon, m_B_FeedSpeed_Horizon);

							e.end = (itsss + 1)->begin;//圆弧的终点改为下一个元素的起点
							e.end.z = (itsss+1)->begin.z+m_PerDownDis*m_PointSum.size();
						}
					}

					m_ssss.push_back(e);
				}

				sss.push_back(m_ssss);
				sss.push_back(ssss);

				
			}
			ss.push_back(sss);
		}
		s.push_back(ss);
	}

	return s;
}
//外部锯片
vector<vector<vector<elements>>> CTableBoardProcess::InitTableOutNC(vector<vector<CPtrArray*>> data)
{
	vector<vector<vector<elements>>> s;

	for (vector<vector<CPtrArray*>>::iterator it = data.begin(); it != data.end(); ++it)
	{
		vector<vector<elements>> ss;
		for (vector<CPtrArray*>::iterator its = it->begin(); its != it->end(); ++its)
		{
			vector<elements> sss = New_CalculateData(*its);
			//对sss中的逆时针圆弧进行标记
			for (int i = 0; i != (*its)->GetSize();++i)
			{
				//圆弧
				if (((CArc*)(*its)->GetAt(i))->flag == 2)
				{
					//BOOL temp = CArc::Calculate(((CArc*)(*its)->GetAt(i))->m_center, ((CArc*)(*its)->GetAt(i))->m_radio,
						//((CArc*)(*its)->GetAt(i))->m_begin_angle, ((CArc*)(*its)->GetAt(i))->m_end_angle, **its, i);
					//if (i==5)//固定5的位置为弯角
					if(((CArc*)(*its)->GetAt(i)) ->m_Direction)
					{
						sss[i].arc_direction = TRUE;//逆时针为true
					}
				}
				
			}

			//在每层的首尾插入延伸点，延长m_ExtendLength的长度
			sss = InsertExtendLineElements(sss, m_ExtendLength);

			ss.push_back(sss);
		}
		s.push_back(ss);
	}
	return s;
}