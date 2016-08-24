// RotateProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "RotateProcess.h"
#include "afxdialogex.h"


// CRotateProcess 对话框

IMPLEMENT_DYNAMIC(CRotateProcess, CDialogEx)

CRotateProcess::CRotateProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRotateProcess::IDD, pParent)
	, m_underside_Long(0)
	, m_underside_Width(0)
	, m_Highth(0)
	, m_raised(0)
	, m_workBlank_Long(0)
	, m_workBlank_Width(0)
	, m_workBlank_Heighth(0)
	, m_FeedSpeed(0)
	, m_FeedTime(0)
	, m_FeedLoad(0)
	, m_FeedDistance(0)
	, m_RoughPlies(0)
	, m_RoughLeft(0)
	, m_FeedSpeed_VX(0)
	, m_FeedTime_VX(0)
	, m_FeedLoad_VX(0)
	, m_FeedSpeed_Finish(0)
	, m_FeedTime_Finish(0)
	, m_FeedLoad_Finish(0)
	, m_FeedDistance_Finish(0)
	, m_FeedSpeed_VX_Finish(0)
	, m_FeedTime_VX_Finish(0)
	, m_FeedLoad_VX_Finish(0)
{
	ratio = 0.0;
	m_ShowTips = FALSE;
}

CRotateProcess::~CRotateProcess()
{
}

void CRotateProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_underside_Long);
	DDV_MinMaxFloat(pDX, m_underside_Long, 0, 5000);
	DDX_Text(pDX, IDC_EDIT2, m_underside_Width);
	DDV_MinMaxFloat(pDX, m_underside_Width, 0, 5000);
	DDX_Text(pDX, IDC_EDIT3, m_Highth);
	DDV_MinMaxFloat(pDX, m_Highth, 0, 10000);
	DDX_Text(pDX, IDC_EDIT4, m_raised);
	DDV_MinMaxFloat(pDX, m_raised, 0, 5000);
	DDX_Text(pDX, IDC_EDIT5, m_workBlank_Long);
	DDV_MinMaxFloat(pDX, m_workBlank_Long, 0, 10000);
	DDX_Text(pDX, IDC_EDIT6, m_workBlank_Width);
	DDV_MinMaxFloat(pDX, m_workBlank_Width, 0, 10000);
	DDX_Text(pDX, IDC_EDIT7, m_workBlank_Heighth);
	DDV_MinMaxFloat(pDX, m_workBlank_Heighth, 0, 10000);
	DDX_Text(pDX, IDC_EDIT8, m_FeedSpeed);
	DDX_Text(pDX, IDC_EDIT9, m_FeedTime);
	DDX_Text(pDX, IDC_EDIT10, m_FeedLoad);
	DDX_Text(pDX, IDC_EDIT11, m_FeedDistance);
	DDX_Text(pDX, IDC_EDIT12, m_RoughPlies);
	DDX_Text(pDX, IDC_EDIT13, m_RoughLeft);
	DDX_Text(pDX, IDC_EDIT14, m_FeedSpeed_VX);
	DDX_Text(pDX, IDC_EDIT15, m_FeedTime_VX);
	DDX_Text(pDX, IDC_EDIT16, m_FeedLoad_VX);
	DDX_Text(pDX, IDC_EDIT17, m_FeedSpeed_Finish);
	DDX_Text(pDX, IDC_EDIT18, m_FeedTime_Finish);
	DDX_Text(pDX, IDC_EDIT19, m_FeedLoad_Finish);
	DDX_Text(pDX, IDC_EDIT20, m_FeedDistance_Finish);
	DDX_Text(pDX, IDC_EDIT21, m_FeedSpeed_VX_Finish);
	DDX_Text(pDX, IDC_EDIT22, m_FeedTime_VX_Finish);
	DDX_Text(pDX, IDC_EDIT23, m_FeedLoad_VX_Finish);
}


BEGIN_MESSAGE_MAP(CRotateProcess, CDialogEx)
	ON_BN_CLICKED(IDC_ROTATE_OPEN_FILE, &CRotateProcess::OnBnClickedRotateOpenFile)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SHOW_BIGEER, &CRotateProcess::OnBnClickedShowBigeer)
	ON_BN_CLICKED(IDC_SHOW_SMALLER, &CRotateProcess::OnBnClickedShowSmaller)
	ON_BN_CLICKED(IDC_TIPS, &CRotateProcess::OnBnClickedTips)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CRotateProcess 消息处理程序


void CRotateProcess::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CRotateProcess::OnOK()
{
	UpdateData(TRUE);
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	CSplitToDoc* pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	pDoc->tool.SetRotateArray(m_TempArray);
	//获取毛坯的尺寸
	pDoc->tool.SetRotateBlankHeighth(m_workBlank_Heighth);
	pDoc->tool.SetRotateBlankLongth(m_workBlank_Long);
	pDoc->tool.SetRotateBlankWidth(m_workBlank_Width);

	//获取粗精加工的速度等信息
	//获取粗加工水平及垂直面的信息
	pDoc->tool.SetRotateFeedSpeed(m_FeedSpeed);
	pDoc->tool.SetRotateFeedTime(m_FeedTime);
	pDoc->tool.SetRotateFeedSpeedVx(m_FeedSpeed_VX);
	pDoc->tool.SetRotateFeedTimeVx(m_FeedTime_VX);
	//获取精加工水平及垂直面的信息
	pDoc->tool.SetRotateFeedSpeedFinish(m_FeedSpeed_Finish);
	pDoc->tool.SetRotateFeedTimeFinish(m_FeedTime_Finish);
	pDoc->tool.SetRotateFeedSpeedFinishVX(m_FeedSpeed_VX_Finish);
	pDoc->tool.SetRotateFeedTimeFinishVX(m_FeedTime_VX_Finish);
	//获取粗加工切深及粗加工层数
	pDoc->tool.SetRotateRoughCutOfDepth(m_FeedDistance);
	pDoc->tool.SetRotateRoughPlies(m_RoughPlies);
	//获取粗加工余量及精加工切深
	pDoc->tool.SetRotateRoughLeft(m_RoughLeft);
	pDoc->tool.SetRotateFinishCutOfDepth(m_FeedDistance_Finish);

	//初始化数据以用于视图中显示
	pDoc->tool.InitRotateRender();

	DestroyWindow();
	//CDialogEx::OnOK();
}


void CRotateProcess::OnCancel()
{
	
	DestroyWindow();
	//CDialogEx::OnCancel();
}


void CRotateProcess::OnBnClickedRotateOpenFile()
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
		GetDlgItem(IDC_ROTATE_SHOW_FILEPATH)->SetWindowTextW(FilePath);

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
			ratio = m_CorCenter.x /65;
		else
			ratio = m_CorCenter.y /65;
	
	}

	//刷新
	Invalidate();

}


void CRotateProcess::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	CDC* pDC;
	pDC = GetDlgItem(IDC_RAOTATE_PICTURE)->GetDC();

	GetDlgItem(IDC_RAOTATE_PICTURE)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_RAOTATE_PICTURE)->UpdateWindow();

	GetDlgItem(IDC_RAOTATE_PICTURE)->GetClientRect(rec);

	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&redpen);
	CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
	CBrush* pOldBrush = pDC->SelectObject(pBrush);

	if (m_TempArray.GetSize() != 0)
	{
		for (int i = 0; i != m_TempArray.GetSize(); ++i)
		{
			if (((CLine*)m_TempArray.GetAt(i))->flag == 1)//直线
			{
				pDC->MoveTo(((CLine*)m_TempArray.GetAt(i))->m_begin.x / ratio, rec.Height() - ((CLine*)m_TempArray.GetAt(i))->m_begin.y / ratio);
				pDC->LineTo(((CLine*)m_TempArray.GetAt(i))->m_end.x / ratio, rec.Height() - ((CLine*)m_TempArray.GetAt(i))->m_end.y / ratio);

				CPoint3D pt;
				pt = ((CLine*)m_TempArray.GetAt(i))->m_end;

			}
			else if (((CArc*)m_TempArray.GetAt(i))->flag == 2)//圆弧
			{
				if (((CArc*)m_TempArray.GetAt(i))->m_Tudu == 0)//普通圆弧
				{
					CToolPath t;
					vector<CPoint3D> pt = t.ArcDiscrete(((CArc*)m_TempArray.GetAt(i))->m_center, ((CArc*)m_TempArray.GetAt(i))->m_radio, ((CArc*)m_TempArray.GetAt(i))->m_begin_angle,
						((CArc*)m_TempArray.GetAt(i))->m_end_angle, m_TempArray, i, 80, 6000);
					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
					{
						pDC->MoveTo(it->x / ratio, rec.Height() - it->y / ratio);
						pDC->LineTo((it + 1)->x / ratio, rec.Height() - (it + 1)->y / ratio);
					}
				}
				else//多义线段圆弧
				{
					CToolPath t;
					vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)m_TempArray.GetAt(i))->m_begin, ((CArc*)m_TempArray.GetAt(i))->m_end,
						((CArc*)m_TempArray.GetAt(i))->m_Tudu, 80, 6000);

					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
					{
						pDC->MoveTo(it->x / ratio, rec.Height() - it->y / ratio);
						pDC->LineTo((it + 1)->x / ratio, rec.Height() - (it + 1)->y / ratio);
					}

				}
			}
			else if (((CCircle*)m_TempArray.GetAt(i))->flag == 3)
			{
				CRect CircleRect;
				CircleRect.left = (((CCircle*)m_TempArray.GetAt(i))->m_center.x - ((CCircle*)m_TempArray.GetAt(i))->m_radio)/ratio;
				CircleRect.top = rec.Height() - (((CCircle*)m_TempArray.GetAt(i))->m_center.y + ((CCircle*)m_TempArray.GetAt(i))->m_radio) / ratio;
				CircleRect.right = (((CCircle*)m_TempArray.GetAt(i))->m_center.x + ((CCircle*)m_TempArray.GetAt(i))->m_radio) / ratio;
				CircleRect.bottom = rec.Height() - (((CCircle*)m_TempArray.GetAt(i))->m_center.y - ((CCircle*)m_TempArray.GetAt(i))->m_radio) / ratio;
				pDC->Ellipse(CircleRect);
			}
		}
		//显示提示
		if (m_ShowTips)
		{
			SortDraw(pDC, m_TempArray);
		}
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}
CRect CRotateProcess::BoundingBox(CPtrArray& Array)
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

void CRotateProcess::OnBnClickedShowBigeer()
{
	if (ratio != 0)
	{
		ratio *= 0.9;
		
		InvalidateRect(rec);
	}
}


void CRotateProcess::OnBnClickedShowSmaller()
{
	if (ratio != 0)
	{
		ratio *= 1.1;
		
		InvalidateRect(rec);
	}
}


void CRotateProcess::OnBnClickedTips()
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


BOOL CRotateProcess::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CDialogEx::OnEraseBkgnd(pDC);
}

void CRotateProcess::SortDraw(CDC* dc, CPtrArray& m_Array)
{
	CPtrArray CircleSort, OtherSort;
	CircleSort.Add(m_Array.GetAt(0));

	for (int i = 1; i != m_Array.GetSize(); ++i)
	{
		OtherSort.Add(m_Array.GetAt(i));
	}
	CRect CircleRect, OtherRect;
	CircleRect = BoundingBox(CircleSort);
	OtherRect = BoundingBox(OtherSort);

	m_underside_Long = m_underside_Width = CircleRect.right - CircleRect.left;
	m_Highth = OtherRect.top - OtherRect.bottom;
	m_raised = OtherRect.right-OtherRect.left;
	UpdateData(FALSE);

	CircleRect.left = CircleRect.left / ratio;
	CircleRect.right = CircleRect.right / ratio;
	CircleRect.top = rec.Height() - CircleRect.top / ratio;
	CircleRect.bottom = rec.Height() - CircleRect.bottom / ratio;

	OtherRect.left = OtherRect.left / ratio;
	OtherRect.right = OtherRect.right / ratio;
	OtherRect.top = rec.Height() - OtherRect.top / ratio;
	OtherRect.bottom = rec.Height() - OtherRect.bottom / ratio;


	DrawaLabel(dc, OtherRect);
	DrawCircleLabel(dc, CircleRect);

	CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pPen = dc->SelectObject(&GreenPen);
	dc->Rectangle(OtherRect);
	dc->Rectangle(CircleRect);
	dc->SelectObject(pPen);
}

void CRotateProcess::DrawaLabel(CDC* m_pDC, CRect Rect)
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
	p1.x = Rect.left - 5;
	p1.y = Rect.top;

	p2.x = Rect.left - 5;
	p2.y = Rect.bottom;

	p3.x = Rect.left;
	p3.y = Rect.top - 5;

	p4.x = Rect.right;
	p4.y = Rect.top - 5;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);
	DrawArrow(m_pDC, p3, p4, 45, 5);


	//画延伸线
	m_pDC->MoveTo(Rect.left, Rect.bottom-1);
	m_pDC->LineTo(Rect.left - 6, Rect.bottom-1);

	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left - 6, Rect.top);

	//标注线旁写上“H”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.left - 15, Rect.top + Rect.Height()*0.3, _T("H"));


	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left, Rect.top - 6);

	m_pDC->MoveTo(Rect.right-1, Rect.top);
	m_pDC->LineTo(Rect.right-1, Rect.top - 6);

	//标注线上写上“E”
	m_pDC->TextOutW(Rect.left + Rect.Width()*0.5, Rect.top - 20, _T("E"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}

void CRotateProcess::DrawCircleLabel(CDC* m_pDC, CRect Rect)
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
	p1.x = Rect.left - 5;
	p1.y = Rect.top;

	p2.x = Rect.left - 5;
	p2.y = Rect.bottom;


	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);


	//画延伸线
	m_pDC->MoveTo(Rect.left, Rect.bottom-1);
	m_pDC->LineTo(Rect.left - 6, Rect.bottom-1);

	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left - 6, Rect.top);

	//标注线旁写上“D”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.left - 15, Rect.top + Rect.Height()*0.3, _T("D"));


	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}

void CRotateProcess::DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length)
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

BOOL CRotateProcess::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);

			m_FeedLoad = m_FeedSpeed*m_FeedTime/60;
			m_FeedLoad_VX = m_FeedSpeed_VX*m_FeedTime_VX/60;

			m_FeedLoad_Finish = m_FeedSpeed_Finish*m_FeedTime_Finish/60;
			m_FeedLoad_VX_Finish = m_FeedSpeed_VX_Finish*m_FeedTime_VX_Finish/60;

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


BOOL CRotateProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	CSplitToDoc* pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	/*m_FeedSpeed = 150;
	m_FeedTime = 8;
	m_FeedSpeed_VX = 150;
	m_FeedTime_VX = 8;

	m_FeedSpeed_Finish = 100;
	m_FeedTime_Finish = 8;
	m_FeedSpeed_VX_Finish = 100;
	m_FeedTime_VX_Finish = 8;*/

	m_FeedSpeed = pDoc->tool.GetRotateFeedSpeed();
	m_FeedTime = pDoc->tool.GetRotateFeedTime();
	m_FeedSpeed_VX = pDoc->tool.GetRotateFeedSpeedVx();
	m_FeedTime_VX = pDoc->tool.GetRotateFeedTimeVx();
	m_FeedDistance = pDoc->tool.GetRotateRoughCutOfDepth();
	m_RoughPlies = pDoc->tool.GetRotateRoughPlies();
	m_RoughLeft = pDoc->tool.GetRotateRoughLeft();


	m_FeedSpeed_Finish = pDoc->tool.GetRotateFeedSpeedFinsh();
	m_FeedTime_Finish = pDoc->tool.GetRotateFeedTimeFinish();
	m_FeedSpeed_VX_Finish = pDoc->tool.GetRotateFeedSpeedFinshVX();
	m_FeedTime_VX_Finish = pDoc->tool.GetRotateFeedTimeFinishVX();
	m_FeedDistance_Finish = pDoc->tool.GetRotateFinishCutOfDepth();

	m_workBlank_Long = pDoc->tool.GetRotateBlankLongth();
	m_workBlank_Width = pDoc->tool.GetRotateBlankWidth();
	m_workBlank_Heighth = pDoc->tool.GetRotateBlankHeighth();


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
