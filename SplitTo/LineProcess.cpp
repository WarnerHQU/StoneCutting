// LineProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "LineProcess.h"
#include "afxdialogex.h"


// CLineProcess 对话框

IMPLEMENT_DYNAMIC(CLineProcess, CDialogEx)

CLineProcess::CLineProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineProcess::IDD, pParent)
	, m_PierceLong(0)
	, m_PierceWidth(0)
	, m_PierceTopHigh(0)
	, m_PierceBottomHigh(0)
	, m_WorkBlankLength(0)
	, m_WorkBlankWidth(0)
	, m_WorkBlankHigh(0)
	, m_Rough_H_FeedSpeed(0)
	, m_Rough_H_FeedTime(0)
	, m_Rough_H_FeedLoad(0)
	, m_Rough_V_FeedSpeed(0)
	, m_Rough_V_FeedTime(0)
	, m_Rough_V_FeedLoad(0)
	/*, m_Rough_VerticalDistance(0)
	, m_Rough_Piles(0)*/
	, m_Rough_Left(0)
	, m_Finish_H_FeedSpeed(0)
	, m_Finish_H_FeedTime(0)
	, m_Finish_H_FeedLoad(0)
	, m_Finish_V_FeedSpeed(0)
	, m_Finish_V_FeedTime(0)
	, m_Finish_V_FeedLoad(0)
	/*, m_Finish_VerticalDistance(0)
	, m_Finish_Piles(0)*/
	, m_Line_Length(0)
	, m_Arc_CenterX(0)
	, m_Arc_CenterY(0)
	, m_Arc_CenterZ(0)
	, m_Arc_Radius(0)
	, m_Arc_BeginAngle(0)
	, m_Arc_EndAngle(0)
	, m_LineRadio(0)
	/*, m_LineDirection(0)*/
{
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	ratio = 0.0;
	m_ShowTips = FALSE;
	m_AddDistance = 0;
	m_LineRadio = 1;
}

CLineProcess::~CLineProcess()
{
}

void CLineProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT10, m_PierceLong);
	DDX_Text(pDX, IDC_EDIT27, m_PierceWidth);
	DDX_Text(pDX, IDC_EDIT28, m_PierceTopHigh);
	DDX_Text(pDX, IDC_EDIT32, m_PierceBottomHigh);
	DDX_Text(pDX, IDC_EDIT29, m_WorkBlankLength);
	DDX_Text(pDX, IDC_EDIT30, m_WorkBlankWidth);
	DDX_Text(pDX, IDC_EDIT31, m_WorkBlankHigh);
	DDX_Text(pDX, IDC_EDIT2, m_Rough_H_FeedSpeed);
	DDX_Text(pDX, IDC_EDIT11, m_Rough_H_FeedTime);
	DDX_Text(pDX, IDC_EDIT12, m_Rough_H_FeedLoad);
	DDX_Text(pDX, IDC_EDIT13, m_Rough_V_FeedSpeed);
	DDX_Text(pDX, IDC_EDIT14, m_Rough_V_FeedTime);
	DDX_Text(pDX, IDC_EDIT15, m_Rough_V_FeedLoad);
	//  DDX_Text(pDX, IDC_EDIT16, m_Rough_VerticalDistance);
	//  DDX_Text(pDX, IDC_EDIT17, m_Rough_Piles);
	DDX_Text(pDX, IDC_EDIT18, m_Rough_Left);
	DDX_Text(pDX, IDC_EDIT19, m_Finish_H_FeedSpeed);
	DDX_Text(pDX, IDC_EDIT20, m_Finish_H_FeedTime);
	DDX_Text(pDX, IDC_EDIT21, m_Finish_H_FeedLoad);
	DDX_Text(pDX, IDC_EDIT22, m_Finish_V_FeedSpeed);
	DDX_Text(pDX, IDC_EDIT23, m_Finish_V_FeedTime);
	DDX_Text(pDX, IDC_EDIT24, m_Finish_V_FeedLoad);
	//  DDX_Text(pDX, IDC_EDIT25, m_Finish_VerticalDistance);
	//  DDX_Text(pDX, IDC_EDIT26, m_Finish_Piles);
	DDX_Text(pDX, IDC_EDIT9, m_Line_Length);
	DDX_Text(pDX, IDC_EDIT1, m_Arc_CenterX);
	DDX_Text(pDX, IDC_EDIT4, m_Arc_CenterY);
	DDX_Text(pDX, IDC_EDIT5, m_Arc_CenterZ);
	DDX_Text(pDX, IDC_EDIT6, m_Arc_Radius);
	DDX_Text(pDX, IDC_EDIT7, m_Arc_BeginAngle);
	DDX_Text(pDX, IDC_EDIT8, m_Arc_EndAngle);
	//  DDX_Control(pDX, IDC_RADIO1, m_LineRadio);
	DDX_Radio(pDX, IDC_RADIO1, m_LineRadio);
	//  DDX_Radio(pDX, IDC_RADIO6, m_LineDirection);
}


BEGIN_MESSAGE_MAP(CLineProcess, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CLineProcess::OnBnClickedLineProcessOpenFile)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON2, &CLineProcess::OnBnClickedShowBigger)
	ON_BN_CLICKED(IDC_BUTTON3, &CLineProcess::OnBnClickedShowSmaller)
	ON_BN_CLICKED(IDC_BUTTON4, &CLineProcess::OnBnClickedShowTips)
	ON_BN_CLICKED(IDC_RADIO2, &CLineProcess::OnBnClickedArcRadio)
	ON_BN_CLICKED(IDC_RADIO1, &CLineProcess::OnBnClickedLineRadio)
	ON_EN_CHANGE(IDC_EDIT17, &CLineProcess::OnEnChangeEdit17)
END_MESSAGE_MAP()


// CLineProcess 消息处理程序


void CLineProcess::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CLineProcess::OnOK()
{
	UpdateData(TRUE);//获取数据值
	//设置基本数据
	pDoc->tool.SetLineArray(m_TempArray);
	//设置毛坯尺寸
	pDoc->tool.SetLineWorkBlankLength(m_WorkBlankLength);
	pDoc->tool.SetLineWorkBlankWidth(m_WorkBlankWidth);
	pDoc->tool.SetLineWorkBlankHigh(m_WorkBlankHigh);
	//设置粗加工参数
	pDoc->tool.SetLineRoughHFeedSpeed(m_Rough_H_FeedSpeed);
	pDoc->tool.SetLineRoughHFeedTime(m_Rough_H_FeedTime);
	pDoc->tool.SetLineRoughVFeedSpeed(m_Rough_V_FeedSpeed);
	pDoc->tool.SetLineRoughVFeedTime(m_Rough_V_FeedTime);
	/*pDoc->tool.SetLineRoughVerticalDistance(m_Rough_VerticalDistance);
	pDoc->tool.SetLineRoughPiles(m_Rough_Piles);*/
	pDoc->tool.SetLineRoughLeft(m_Rough_Left);
	//设置精加工参数
	pDoc->tool.SetLineFinishHFeedSpeed(m_Finish_H_FeedSpeed);
	pDoc->tool.SetLineFinishHFeedTime(m_Finish_H_FeedTime);
	pDoc->tool.SetLineFinishVFeedSpeed(m_Finish_V_FeedSpeed);
	pDoc->tool.SetLineFinishVFeedTime(m_Finish_V_FeedTime);
	/*pDoc->tool.SetLineFinishVerticalDistance(m_Finish_VerticalDistance);
	pDoc->tool.SetLineFinishPiles(m_Finish_Piles);*/
	//设置加工类型
	pDoc->tool.SetLineStyle(m_LineRadio);//0为直线，1为圆弧
	//设置线条弧形数据,是圆弧是才设置
	if (m_LineRadio == 1)
	{
		CPtrArray tempArray;
		CArc* pArc = new CArc;
		tempArray.Add(pArc);
		pArc->m_center.x = m_Arc_CenterX;
		pArc->m_center.y = m_Arc_CenterY;
		pArc->m_center.z = m_Arc_CenterZ;
		pArc->m_begin_angle = m_Arc_BeginAngle;
		pArc->m_end_angle = m_Arc_EndAngle;
		pArc->m_radio = m_Arc_Radius;

		pDoc->tool.SetLineArcShapeArray(tempArray);
	}
	//直线的时候手动设置直线的长度，并填写工件尺寸长度和毛坯长度，三者是一样的

	pDoc->tool.LineRenerInit();
	DestroyWindow();
	//CDialogEx::OnOK();
}


void CLineProcess::OnCancel()
{
	
	DestroyWindow();
	//CDialogEx::OnCancel();
}


void CLineProcess::OnBnClickedLineProcessOpenFile()
{
	CFileDialog openfile(TRUE, NULL, _T("*.dxf"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL, NULL);
	openfile.m_ofn.lpstrTitle = _T("谢峰DXF打开窗口");

	openfile.m_ofn.lpstrFilter = _T("文本文件(.txt)\0*.txt\0dxf文件(.dxf)\0*.dxf\0NC文件(.nc)\0所有文件(*.*)\0*.*\0\0");


	if (IDOK == openfile.DoModal())//按下确定键按钮的时候可以对数据进行存储，但是要将类型设置为public
	{
		vector<string>().swap(m_TempData);
		m_TempArray.RemoveAll();

		CString FilePath;
		FilePath = openfile.GetPathName();
		GetDlgItem(IDC_LINEPROCESS_FILEPATH)->SetWindowTextW(FilePath);

		ifstream ifs(FilePath);
		if (ifs.is_open())
		{
			string s;
			if (!ifs)exit(EXIT_FAILURE);
			while (getline(ifs, s))
			{
				cout << s << endl;
				m_TempData.push_back(s);
			}
			ifs.close();
			ifs.clear();

		}
		for (vector<string>::iterator it = m_TempData.begin(); it != m_TempData.end(); ++it)
		{
			static int i = 0;
			if ((*it) == "LINE")
			{
				CLine* pline = new CLine;
				m_TempArray.Add(pline);
				for (it; *(it + 1) != "LINE"; ++it)
				{
					if (*it == " 10")
						pline->m_begin.x = (atof)((it + 1)->c_str());
					if (*it == " 20")
						pline->m_begin.y = (atof)((it + 1)->c_str());
					if (*it == " 30")
						pline->m_begin.z = (atof)((it + 1)->c_str());
					if (*it == " 11")
						pline->m_end.x = (atof)((it + 1)->c_str());
					if (*it == " 21")
						pline->m_end.y = (atof)((it + 1)->c_str());
					if (*it == " 31")
					{
						pline->m_end.z = (atof)((it + 1)->c_str());
						break;
					}

				}

				++i;
			}
			else if ((*it) == "ARC")
			{
				CArc* parc = new CArc;
				m_TempArray.Add(parc);
				for (it; *(it + 1) != "LINE"; ++it)
				{
					if ((*it) == " 10")
						parc->m_center.x = (atof)((it + 1)->c_str());
					else if ((*it) == " 20")
						parc->m_center.y = (atof)((it + 1)->c_str());
					else if ((*it) == " 30")
						parc->m_center.z = (atof)((it + 1)->c_str());
					else if ((*it) == " 40")
						parc->m_radio = (float)(atof)((it + 1)->c_str());
					else if ((*it) == " 50")
						parc->m_begin_angle = (float)(atof)((it + 1)->c_str());
					else if ((*it) == " 51")
					{
						parc->m_end_angle = (float)(atof)((it + 1)->c_str());
						break;
					}
				}
				++i;
			}
			else if ((*it) == "CIRCLE")
			{
				CCircle* pcircle = new CCircle;
				m_TempArray.Add(pcircle);
				for (it; *(it + 1) != "LINE"; ++it)
				{
					if ((*it) == " 10")
						pcircle->m_center.x = (atof)((it + 1)->c_str());
					else if ((*it) == " 20")
						pcircle->m_center.y = (atof)((it + 1)->c_str());
					else if ((*it) == " 30")
						pcircle->m_center.z = (atof)((it + 1)->c_str());
					else if ((*it) == " 40")
					{
						pcircle->m_radio = (float)(atof)((it + 1)->c_str());
						break;
					}

				}
				++i;
			}
			else if ((*it) == "LWPOLYLINE")//凸度为0是直线，凸度为负值为圆弧顺时针，凸度为正值为圆弧逆时针
				//凸度绝对值大于1表示的是大于180度的角，绝对值小于1表示的是小于180度的角
			{
				do
				{

					if ((*it) == " 10" && (*(it + 2)) == " 20" && (*(it + 4)) != "  0" && (*(it + 4)) != " 42")//读取直线
					{
						CLine* pline = new CLine;
						m_TempArray.Add(pline);
						pline->m_begin.x = (atof)((it + 1)->c_str());
						pline->m_begin.y = (atof)((it + 3)->c_str());
						pline->m_begin.z = 0.0;//二维polyline
						pline->m_end.x = (atof)((it + 5)->c_str());
						pline->m_end.y = (atof)((it + 7)->c_str());
						pline->m_end.z = 0.0;//二维polyline
						++i;
						it = it + 3;
					}
					else if ((*it) == " 10" && (*(it + 2)) == " 20" && (*(it + 4)) == " 42" && (*(it + 6) != "  0"))//读取圆弧
					{
						CArc* parc = new CArc;
						m_TempArray.Add(parc);
						parc->m_begin.x = (atof)((it + 1)->c_str());
						parc->m_begin.y = (atof)((it + 3)->c_str());
						parc->m_begin.z = 0.0;
						parc->m_Tudu = (atof)((it + 5)->c_str());
						parc->m_end.x = (atof)((it + 7)->c_str());
						parc->m_end.y = (atof)((it + 9)->c_str());
						parc->m_end.z = 0.0;
						++i;
						it = it + 5;
					}

					++it;
				} while ((*it) != "ENDSEC");
				++i;
			}

		}

		PieceSize = m_BoundingBox_2D = BoundingBox(m_TempArray);//整体的零件的边界

		m_CorCenter.x = PieceSize.left + (PieceSize.right - PieceSize.left) / 2;
		m_CorCenter.y = PieceSize.bottom + (PieceSize.top - PieceSize.bottom) / 2;
		if (m_CorCenter.x < m_CorCenter.y)
			ratio = m_CorCenter.x / 20;
		else
			ratio = m_CorCenter.y / 20;
	}

	Invalidate();
}

CRect CLineProcess::BoundingBox(CPtrArray& Array)
{
	CRect Zone;
	double Xmin, Ymin, Xmax, Ymax;//将起始元素的首位点设置为最大最小点
	if (((CLine*)Array.GetAt(0))->flag == 1)
	{
		Xmin = ((CLine*)Array.GetAt(0))->m_begin.x;
		Xmax = ((CLine*)Array.GetAt(0))->m_end.x;
		Ymin = ((CLine*)Array.GetAt(0))->m_begin.y;
		Ymax = ((CLine*)Array.GetAt(0))->m_end.y;
	}
	else if (((CArc*)Array.GetAt(0))->flag == 2)
	{
		if (((CArc*)Array.GetAt(0))->m_Tudu == 0)
		{
			CArc::Calculate(((CArc*)Array.GetAt(0))->m_center, ((CArc*)Array.GetAt(0))->m_radio, ((CArc*)Array.GetAt(0))->m_begin_angle,
				((CArc*)Array.GetAt(0))->m_end_angle, Array, 0);
			Xmin = ((CArc*)Array.GetAt(0))->m_begin.x;
			Xmax = ((CArc*)Array.GetAt(0))->m_end.x;
			Ymin = ((CArc*)Array.GetAt(0))->m_begin.y;
			Ymax = ((CArc*)Array.GetAt(0))->m_end.y;
		}
		else
		{
			Xmin = ((CArc*)Array.GetAt(0))->m_begin.x;
			Xmax = ((CArc*)Array.GetAt(0))->m_end.x;
			Ymin = ((CArc*)Array.GetAt(0))->m_begin.y;
			Ymax = ((CArc*)Array.GetAt(0))->m_end.y;
		}
	}
	else if (((CCircle*)Array.GetAt(0))->flag == 3)
	{
		Xmin = ((CCircle*)Array.GetAt(0))->m_center.x - ((CCircle*)Array.GetAt(0))->m_radio;
		Xmax = ((CCircle*)Array.GetAt(0))->m_center.x + ((CCircle*)Array.GetAt(0))->m_radio;
		Ymin = ((CCircle*)Array.GetAt(0))->m_center.y - ((CCircle*)Array.GetAt(0))->m_radio;
		Ymax = ((CCircle*)Array.GetAt(0))->m_center.y + ((CCircle*)Array.GetAt(0))->m_radio;
	}


	/*搜索Array中的元素比较X\Y最大最小值*/
	for (int i = 0; i != Array.GetSize(); ++i)
	{

		if (((CLine*)Array.GetAt(i))->flag == 1)
		{
			if (((CLine*)Array.GetAt(i))->m_begin.x < Xmin)
				Xmin = ((CLine*)Array.GetAt(i))->m_begin.x;
			else if (((CLine*)Array.GetAt(i))->m_begin.x>Xmax)
				Xmax = ((CLine*)Array.GetAt(i))->m_begin.x;

			if (((CLine*)Array.GetAt(i))->m_end.x < Xmin)
				Xmin = ((CLine*)Array.GetAt(i))->m_end.x;
			else if (((CLine*)Array.GetAt(i))->m_end.x>Xmax)
				Xmax = ((CLine*)Array.GetAt(i))->m_end.x;

			if (((CLine*)Array.GetAt(i))->m_begin.y < Ymin)
				Ymin = ((CLine*)Array.GetAt(i))->m_begin.y;
			else if (((CLine*)Array.GetAt(i))->m_begin.y>Ymax)
				Ymax = ((CLine*)Array.GetAt(i))->m_begin.y;



			if (((CLine*)Array.GetAt(i))->m_end.y < Ymin)
				Ymin = ((CLine*)Array.GetAt(i))->m_end.y;
			else if (((CLine*)Array.GetAt(i))->m_end.y>Ymax)
				Ymax = ((CLine*)Array.GetAt(i))->m_end.y;

		}
		else if (((CArc*)Array.GetAt(i))->flag == 2)
		{
			if (((CArc*)Array.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				CToolPath t;
				vector<CPoint3D> pt = t.ArcDiscrete(((CArc*)Array.GetAt(i))->m_center, ((CArc*)Array.GetAt(i))->m_radio, ((CArc*)Array.GetAt(i))->m_begin_angle,
					((CArc*)Array.GetAt(i))->m_end_angle, Array, i, 80, 6000);
				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end(); ++it)
				{
					if (it->x > Xmax)
						Xmax = it->x;
					if (it->x < Xmin)
						Xmin = it->x;
					if (it->y>Ymax)
						Ymax = it->y;
					if (it->y < Ymin)
						Ymin = it->y;
				}

			}
			else//多义线段圆弧
			{
				CToolPath t;
				vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)Array.GetAt(i))->m_begin, ((CArc*)Array.GetAt(i))->m_end,
					((CArc*)Array.GetAt(i))->m_Tudu, 80, 6000);

				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end(); ++it)
				{
					if (it->x > Xmax)
						Xmax = it->x;
					if (it->x < Xmin)
						Xmin = it->x;
					if (it->y>Ymax)
						Ymax = it->y;
					if (it->y < Ymin)
						Ymin = it->y;
				}
			}
		}
	}
	Zone.left = Xmin;
	Zone.top = Ymax;
	Zone.right = Xmax;
	Zone.bottom = Ymin;
	return Zone;
}
void CLineProcess::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	 
	CDC* pDC;
	pDC = GetDlgItem(IDC_LINEPROCESS_GRAPHIC)->GetDC();
	GetDlgItem(IDC_LINEPROCESS_GRAPHIC)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_LINEPROCESS_GRAPHIC)->UpdateWindow();
	GetDlgItem(IDC_LINEPROCESS_GRAPHIC)->GetClientRect(rec);

	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&redpen);
	CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
	CBrush* pOldBrush = pDC->SelectObject(pBrush);
    m_AddDistance = 50;

	for (int i = 0; i != m_TempArray.GetSize(); ++i)
	{
		if (((CLine*)m_TempArray.GetAt(i))->flag == 1)//直线
		{
			pDC->MoveTo(((CLine*)m_TempArray.GetAt(i))->m_begin.x / ratio, rec.Height() - m_AddDistance-((CLine*)m_TempArray.GetAt(i))->m_begin.y / ratio);
			pDC->LineTo(((CLine*)m_TempArray.GetAt(i))->m_end.x / ratio, rec.Height() - m_AddDistance - ((CLine*)m_TempArray.GetAt(i))->m_end.y / ratio);
		}
		else if (((CArc*)m_TempArray.GetAt(i))->flag == 2)//圆弧
		{
			if (((CArc*)m_TempArray.GetAt(i))->m_Tudu == 0)//普通圆弧
			{
				CToolPath t;
				vector<CPoint3D> pt = t.ArcDiscrete(((CArc*)m_TempArray.GetAt(i))->m_center, ((CArc*)m_TempArray.GetAt(i))->m_radio, ((CArc*)m_TempArray.GetAt(i))->m_begin_angle,
					((CArc*)m_TempArray.GetAt(i))->m_end_angle, m_TempArray, i, 60, 3000);
				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
				{
					pDC->MoveTo(it->x / ratio, rec.Height() - m_AddDistance - it->y / ratio);
					pDC->LineTo((it + 1)->x / ratio, rec.Height() - m_AddDistance - (it + 1)->y / ratio);
				}
			}
			else//多义线段
			{
				CToolPath t;
				vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)m_TempArray.GetAt(i))->m_begin, ((CArc*)m_TempArray.GetAt(i))->m_end,
					((CArc*)m_TempArray.GetAt(i))->m_Tudu, 60, 3000);

				for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
				{
					pDC->MoveTo(it->x / ratio, rec.Height() - m_AddDistance - it->y / ratio);
					pDC->LineTo((it + 1)->x / ratio, rec.Height() - m_AddDistance - (it + 1)->y / ratio);
				}
			}
		}
	}

	//画提示线
	if (m_ShowTips)
	{
		SortDraw(pDC, m_TempArray);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CLineProcess::OnBnClickedShowBigger()
{
	if (ratio != 0)
	{
		ratio *= 0.9;

		InvalidateRect(rec);
	}
}
void CLineProcess::OnBnClickedShowSmaller()
{
	if (ratio != 0)
	{
		ratio *= 1.1;

		InvalidateRect(rec);
	}
}

void CLineProcess::OnBnClickedShowTips()
{
	if (m_ShowTips)
	{
		m_ShowTips = FALSE;
		InvalidateRect(rec);
	}
	else
	{
		m_ShowTips = TRUE;
		InvalidateRect(rec);
	}
}
//画提示线
void CLineProcess::SortDraw(CDC* dc, CPtrArray& m_Array)
{
	CRect m_RectLineProcess;
	m_RectLineProcess = BoundingBox(m_Array);

	m_PierceWidth = m_RectLineProcess.right - m_RectLineProcess.left;
	m_PierceTopHigh = m_RectLineProcess.top - m_RectLineProcess.bottom;
	m_PierceBottomHigh = m_RectLineProcess.bottom;
	UpdateData(FALSE);

	m_RectLineProcess.left = m_RectLineProcess.left / ratio;
	m_RectLineProcess.right = m_RectLineProcess.right / ratio;
	m_RectLineProcess.top = rec.Height() - m_AddDistance - m_RectLineProcess.top / ratio;
	m_RectLineProcess.bottom = rec.Height() - m_AddDistance - m_RectLineProcess.bottom / ratio;
	//画标线
	DrawaLabel(dc,m_RectLineProcess);
	DrawH2Label(dc, m_RectLineProcess);

	CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pPen = dc->SelectObject(&GreenPen);
	dc->Rectangle(m_RectLineProcess);
	dc->SelectObject(pPen);
}

void CLineProcess::DrawaLabel(CDC* m_pDC, CRect Rect)
{
	CFont font;
	font.CreateFont(15,                                          //   nHeight     
		0,                                                   //   nWidth     
		0,                                                   //   nEscapement   
		0,                                                   //   nOrientation     
		FW_NORMAL,                                   //   nWeight     
		FALSE,                                           //   bItalic     
		FALSE,                                           //   bUnderline     
		0,                                                   //   cStrikeOut     
		ANSI_CHARSET,                             //   nCharSet     
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision     
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision     
		DEFAULT_QUALITY,                       //   nQuality     
		DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily       
		_T("宋体"));

	CPen Pen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pOldPen = m_pDC->SelectObject(&Pen);

	CPoint p1, p2, p3, p4;
	p1.x = Rect.right + 5;
	p1.y = Rect.top;

	p2.x = Rect.right + 5;
	p2.y = Rect.bottom;

	p3.x = Rect.left;
	p3.y = Rect.top - 5;

	p4.x = Rect.right;
	p4.y = Rect.top - 5;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);
	DrawArrow(m_pDC, p3, p4, 45, 5);

	//画延伸线
	m_pDC->MoveTo(Rect.right, Rect.bottom - 1);
	m_pDC->LineTo(Rect.right + 6, Rect.bottom - 1);

	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right + 6, Rect.top);


	//标注线旁写上“H1”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.right + 15, Rect.top + Rect.Height()*0.3, _T("H1"));


	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left, Rect.top - 6);

	m_pDC->MoveTo(Rect.right - 1, Rect.top);
	m_pDC->LineTo(Rect.right - 1, Rect.top - 6);

	//标注线上写上“W”
	m_pDC->TextOutW(Rect.left + Rect.Width()*0.5, Rect.top - 20, _T("W"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);


}
void CLineProcess::DrawH2Label(CDC* m_pDC, CRect Rect)
{
	CFont font;
	font.CreateFont(15,                                          //   nHeight     
		0,                                                   //   nWidth     
		0,                                                   //   nEscapement   
		0,                                                   //   nOrientation     
		FW_NORMAL,                                   //   nWeight     
		FALSE,                                           //   bItalic     
		FALSE,                                           //   bUnderline     
		0,                                                   //   cStrikeOut     
		ANSI_CHARSET,                             //   nCharSet     
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision     
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision     
		DEFAULT_QUALITY,                       //   nQuality     
		DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily       
		_T("宋体"));

	CPen Pen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pOldPen = m_pDC->SelectObject(&Pen);

	CPoint p1, p2;
	p1.x = Rect.right +5;
	p1.y = Rect.bottom;

	p2.x = Rect.right + 5;
	p2.y = Rect.bottom + 30;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);

	//标注线上写“H2”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));

	m_pDC->TextOutW(Rect.right + 14, Rect.bottom + 10, _T("H2"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}

void CLineProcess::DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length)
{
	theta = 3.1415926*theta / 180;//转换为弧度
	double Px, Py, P1x, P1y, P2x, P2y;
	//以P2为原点得到向量P2P1（P）
	Px = p1.x - p2.x;
	Py = p1.y - p2.y;
	//向量P旋转theta角得到向量P1
	P1x = Px*cos(theta) - Py*sin(theta);
	P1y = Px*sin(theta) + Py*cos(theta);
	//向量P旋转-theta角得到向量P2
	P2x = Px*cos(-theta) - Py*sin(-theta);
	P2y = Px*sin(-theta) + Py*cos(-theta);
	//伸缩向量至制定长度
	double x1, x2;
	x1 = sqrt(P1x*P1x + P1y*P1y);
	P1x = P1x*length / x1;
	P1y = P1y*length / x1;
	x2 = sqrt(P2x*P2x + P2y*P2y);
	P2x = P2x*length / x2;
	P2y = P2y*length / x2;
	//平移变量到直线的末端
	P1x = P1x + p2.x;
	P1y = P1y + p2.y;
	P2x = P2x + p2.x;
	P2y = P2y + p2.y;

	CPen pen, pen1, *oldpen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pen1.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	oldpen = m_pDC->SelectObject(&pen);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(p2.x, p2.y);
	m_pDC->SelectObject(&pen1);
	m_pDC->MoveTo(p2.x, p2.y);
	m_pDC->LineTo(P1x, P1y);
	m_pDC->MoveTo(p2.x, p2.y);
	m_pDC->LineTo(P2x, P2y);
	m_pDC->SelectObject(oldpen);


	//以P1为原点得到向量P1P2（P）
	Px = p2.x - p1.x;
	Py = p2.y - p1.y;
	//向量P旋转theta角得到向量P1
	P1x = Px*cos(theta) - Py*sin(theta);
	P1y = Px*sin(theta) + Py*cos(theta);
	//向量P旋转-theta角得到向量P2
	P2x = Px*cos(-theta) - Py*sin(-theta);
	P2y = Px*sin(-theta) + Py*cos(-theta);
	//伸缩向量至制定长度
	x1 = sqrt(P1x*P1x + P1y*P1y);
	P1x = P1x*length / x1;
	P1y = P1y*length / x1;
	x2 = sqrt(P2x*P2x + P2y*P2y);
	P2x = P2x*length / x2;
	P2y = P2y*length / x2;
	//平移变量到直线的首端
	P1x = P1x + p1.x;
	P1y = P1y + p1.y;
	P2x = P2x + p1.x;
	P2y = P2y + p1.y;


	oldpen = m_pDC->SelectObject(&pen1);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(P1x, P1y);
	m_pDC->MoveTo(p1.x, p1.y);
	m_pDC->LineTo(P2x, P2y);
	m_pDC->SelectObject(oldpen);

}
//初始化对话框
BOOL CLineProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_LineRadio == 0)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
	}
	else if (m_LineRadio == 1)
	{
		GetDlgItem(IDC_EDIT9)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT10)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT29)->EnableWindow(FALSE);
		/*GetDlgItem(IDC_RADIO6)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO7)->EnableWindow(FALSE);*/
	}
	
	m_WorkBlankLength=m_PierceLong=m_Line_Length = 200;

	m_Rough_H_FeedSpeed = 10000;
	m_Rough_H_FeedTime = 60;
	m_Rough_V_FeedSpeed = 10000;
	m_Rough_V_FeedTime = 60;
	/*m_Rough_VerticalDistance = 20;
	m_Rough_Piles = 3;*/
	m_Rough_Left = 30;

	m_Finish_H_FeedSpeed = 5000;
	m_Finish_H_FeedTime = 60;
	m_Finish_V_FeedSpeed = 5000;
	m_Finish_V_FeedTime = 60;
	/*m_Finish_Piles = 2;
	m_Finish_VerticalDistance = 5;*/

	m_Arc_CenterX = 500;
	m_Arc_CenterY = 500;
	m_Arc_CenterZ = 0;
	m_Arc_BeginAngle = -30;
	m_Arc_EndAngle = 30;
	m_Arc_Radius = 500;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CLineProcess::OnBnClickedArcRadio()
{
	UpdateData(TRUE);
	m_LineRadio =1;
	UpdateData(FALSE);
	
	GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT9)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT10)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT29)->EnableWindow(FALSE);
	/*GetDlgItem(IDC_RADIO6)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO7)->EnableWindow(FALSE);*/
	
}


void CLineProcess::OnBnClickedLineRadio()
{
	UpdateData(TRUE);
	m_LineRadio = 0;
	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT10)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT29)->EnableWindow(TRUE);
	/*GetDlgItem(IDC_RADIO6)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO7)->EnableWindow(TRUE);*/

	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
	
}


BOOL CLineProcess::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);
			m_Rough_H_FeedLoad = m_Rough_H_FeedSpeed*m_Rough_H_FeedTime / 60;
			m_Rough_V_FeedLoad = m_Rough_V_FeedSpeed*m_Rough_V_FeedTime / 60;

			m_Finish_H_FeedLoad = m_Finish_H_FeedSpeed*m_Finish_H_FeedTime / 60;
			m_Finish_V_FeedLoad = m_Finish_V_FeedSpeed*m_Finish_V_FeedTime / 60;

			UpdateData(FALSE);
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CLineProcess::OnEnChangeEdit17()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
