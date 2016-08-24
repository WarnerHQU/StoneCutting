// ArcSurface.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "ArcSurface.h"
#include "afxdialogex.h"


// CArcSurface 对话框

IMPLEMENT_DYNAMIC(CArcSurface, CDialogEx)

CArcSurface::CArcSurface(CWnd* pParent /*=NULL*/)
	: CDialogEx(CArcSurface::IDD, pParent)
	, m_ArcPieceLong(0)
	, m_ArcPieceWidth(0)
	, m_ArcPieceHigth(0)
	, m_ArcYSideRadius(0)
	, m_ArcXSideRadius(0)
	, m_WorkBlankLong(0)
	, m_WorkBlankWidth(0)
	, m_WorkBlankHeigth(0)
	, m_Rough_YArcFeedSpeed(0)
	, m_Rough_YArcFeedTime(0)
	, m_Rough_YArcFeedLoad(0)
	, m_Rough_XArcFeedSpeed(0)
	, m_Rough_XArcFeedTime(0)
	, m_Rough_XArcFeedLoad(0)
	, m_Rough_VerticalDown(0)
	, m_Rough_NumberOfPiles(0)
	, m_Rough_Left(0)
	, m_Finish_YArcFeedSpeed(0)
	, m_Finish_YArcFeedTime(0)
	, m_Finish_YArcFeedLoad(0)
	, m_Finish_XArcFeedSpeed(0)
	, m_Finish_XArcFeedTime(0)
	, m_Finish_XArcFeedLoad(0)
	, m_Finish_VerticalDistance(0)
	, m_Finish_NumberOfPile(0)
{
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	ratio = 0.0;
	m_ShowTips = FALSE;
}

CArcSurface::~CArcSurface()
{
}

void CArcSurface::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ArcPieceLong);
	DDX_Text(pDX, IDC_EDIT2, m_ArcPieceWidth);
	DDX_Text(pDX, IDC_EDIT3, m_ArcPieceHigth);
	DDX_Text(pDX, IDC_EDIT10, m_ArcYSideRadius);
	DDX_Text(pDX, IDC_EDIT11, m_ArcXSideRadius);
	DDX_Text(pDX, IDC_EDIT4, m_WorkBlankLong);
	DDX_Text(pDX, IDC_EDIT5, m_WorkBlankWidth);
	DDX_Text(pDX, IDC_EDIT6, m_WorkBlankHeigth);
	DDX_Text(pDX, IDC_EDIT7, m_Rough_YArcFeedSpeed);
	DDX_Text(pDX, IDC_EDIT8, m_Rough_YArcFeedTime);
	DDX_Text(pDX, IDC_EDIT9, m_Rough_YArcFeedLoad);
	DDX_Text(pDX, IDC_EDIT12, m_Rough_XArcFeedSpeed);
	DDX_Text(pDX, IDC_EDIT13, m_Rough_XArcFeedTime);
	DDX_Text(pDX, IDC_EDIT14, m_Rough_XArcFeedLoad);
	DDX_Text(pDX, IDC_EDIT15, m_Rough_VerticalDown);
	DDX_Text(pDX, IDC_EDIT16, m_Rough_NumberOfPiles);
	DDX_Text(pDX, IDC_EDIT17, m_Rough_Left);
	DDX_Text(pDX, IDC_EDIT18, m_Finish_YArcFeedSpeed);
	DDX_Text(pDX, IDC_EDIT19, m_Finish_YArcFeedTime);
	DDX_Text(pDX, IDC_EDIT20, m_Finish_YArcFeedLoad);
	DDX_Text(pDX, IDC_EDIT21, m_Finish_XArcFeedSpeed);
	DDX_Text(pDX, IDC_EDIT22, m_Finish_XArcFeedTime);
	DDX_Text(pDX, IDC_EDIT23, m_Finish_XArcFeedLoad);
	DDX_Text(pDX, IDC_EDIT24, m_Finish_VerticalDistance);
	DDX_Text(pDX, IDC_EDIT25, m_Finish_NumberOfPile);
}


BEGIN_MESSAGE_MAP(CArcSurface, CDialogEx)
	ON_BN_CLICKED(IDC_ARC_OPENFILE, &CArcSurface::OnBnClickedArcOpenfile)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_EXTEND_BTN, &CArcSurface::OnBnClickedExtendBtn)
	ON_BN_CLICKED(IDC_SHRINK_BTN, &CArcSurface::OnBnClickedShrinkBtn)
	ON_BN_CLICKED(IDC_TIPS_BTN, &CArcSurface::OnBnClickedTipsBtn)
END_MESSAGE_MAP()


// CArcSurface 消息处理程序


void CArcSurface::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CArcSurface::OnOK()
{
	UpdateData(TRUE);

	pDoc->tool.SetArcArray(m_TempArray);
	pDoc->tool.SetArcWorkBlank(m_WorkBlankLong, m_WorkBlankWidth, m_WorkBlankHeigth);//毛坯设置
	//粗加工参数获取
	pDoc->tool.SetYArcRoughFeedSpeed(m_Rough_YArcFeedSpeed);
	pDoc->tool.SetYArcRoughFeedTime(m_Rough_YArcFeedTime);
	pDoc->tool.SetXArcRoughFeedSpeed(m_Rough_XArcFeedSpeed);
	pDoc->tool.SetXArcRoughFeedTime(m_Rough_XArcFeedTime);
	pDoc->tool.SetArcRoughVerticalDistance(m_Rough_VerticalDown);
	pDoc->tool.SetArcRoughPiles(m_Rough_NumberOfPiles);
	pDoc->tool.SetArcRoughLeft(m_Rough_Left);

	//精加工参数获取
	pDoc->tool.SetYArcFinishFeedSpeed(m_Finish_YArcFeedSpeed);
	pDoc->tool.SetYArcFinishFeedTime(m_Finish_YArcFeedTime);
	pDoc->tool.SetXArcFinishFeedSpeed(m_Finish_XArcFeedSpeed);
	pDoc->tool.SetXArcFinishFeedTime(m_Finish_XArcFeedTime);
	pDoc->tool.SetArcFinishVerticalDistance(m_Finish_VerticalDistance);
	pDoc->tool.SetArcFinishPiles(m_Finish_NumberOfPile);

	pDoc->tool.RenderArcInit();

	DestroyWindow();

	//CDialogEx::OnOK();
}


void CArcSurface::OnCancel()
{
	DestroyWindow();

	//CDialogEx::OnCancel();
}

void CArcSurface::OnBnClickedArcOpenfile()
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
		GetDlgItem(IDC_ARC_FILEPATH)->SetWindowTextW(FilePath);

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

		for (int i = 0; i != m_TempArray.GetSize(); ++i)//值求取最后最后一个圆弧的半径值，对于半径不相等的情况还要进行值得区分？？
		{
			if (i == m_TempArray.GetSize() - 1)//最后一个元素
			{
				if (((CArc*)m_TempArray.GetAt(i))->flag == 2)
				{
					if (((CArc*)m_TempArray.GetAt(i))->m_Tudu == 0)
					{
						YRadius = ((CArc*)m_TempArray.GetAt(i))->m_radio;
					}
					else
					{
						((CArc*)m_TempArray.GetAt(i))->PolyCalculate(((CArc*)m_TempArray.GetAt(i))->m_begin, 
							((CArc*)m_TempArray.GetAt(i))->m_end, ((CArc*)m_TempArray.GetAt(i))->m_Tudu);
						YRadius = ((CArc*)m_TempArray.GetAt(i))->m_radio;

					}
				}
			}
			else//非最后一个元素
			{
				if (((CArc*)m_TempArray.GetAt(i))->flag == 2)
				{
					if (((CArc*)m_TempArray.GetAt(i))->m_Tudu == 0)
					{
						XRadius = ((CArc*)m_TempArray.GetAt(i))->m_radio;
					}
					else
					{
						((CArc*)m_TempArray.GetAt(i))->PolyCalculate(((CArc*)m_TempArray.GetAt(i))->m_begin,
							((CArc*)m_TempArray.GetAt(i))->m_end, ((CArc*)m_TempArray.GetAt(i))->m_Tudu);
						XRadius = ((CArc*)m_TempArray.GetAt(i))->m_radio;

					}
				}
			}
		}

		PieceSize = m_BoundingBox_2D = BoundingBox(m_TempArray);//整体的零件的边界

		m_CorCenter.x = PieceSize.left + (PieceSize.right - PieceSize.left) / 2;
		m_CorCenter.y = PieceSize.bottom + (PieceSize.top - PieceSize.bottom) / 2;
		if (m_CorCenter.x < m_CorCenter.y)
			ratio = m_CorCenter.x / 50;
		else
			ratio = m_CorCenter.y / 50;
	}

	Invalidate();
}
CRect CArcSurface::BoundingBox(CPtrArray& Array)
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
void CArcSurface::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC* pDC;
	pDC = GetDlgItem(IDC_ARC_PICTURE)->GetDC();
	GetDlgItem(IDC_ARC_PICTURE)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_ARC_PICTURE)->UpdateWindow();
	GetDlgItem(IDC_ARC_PICTURE)->GetClientRect(rec);

	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&redpen);
	CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
	CBrush* pOldBrush = pDC->SelectObject(pBrush);
	int m_AddDistance = 50;
	if (m_TempArray.GetSize() != 0)
	{
		for (int i = 0; i != m_TempArray.GetSize(); ++i)
		{
			if (((CLine*)m_TempArray.GetAt(i))->flag == 1)//直线
			{
				pDC->MoveTo(((CLine*)m_TempArray.GetAt(i))->m_begin.x / ratio, rec.Height() - ((CLine*)m_TempArray.GetAt(i))->m_begin.y / ratio);
				pDC->LineTo(((CLine*)m_TempArray.GetAt(i))->m_end.x / ratio, rec.Height() - ((CLine*)m_TempArray.GetAt(i))->m_end.y / ratio);
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
				else//多义线段圆弧
				{
					CToolPath t;
					vector<CPoint3D> pt = t.PolyArcDiscrete(((CArc*)m_TempArray.GetAt(i))->m_begin, ((CArc*)m_TempArray.GetAt(i))->m_end,
						((CArc*)m_TempArray.GetAt(i))->m_Tudu, 60, 3000);

					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end() - 1; ++it)
					{
						pDC->MoveTo(it->x / ratio, rec.Height() - m_AddDistance - it->y / ratio);
						pDC->LineTo((it + 1)->x / ratio, rec.Height() - m_AddDistance- (it + 1)->y / ratio);
					}

				}
			}
			else if (((CCircle*)m_TempArray.GetAt(i))->flag == 3)
			{
				CRect CircleRect;
				CircleRect.left = (((CCircle*)m_TempArray.GetAt(i))->m_center.x - ((CCircle*)m_TempArray.GetAt(i))->m_radio) / ratio;
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
void CArcSurface::OnBnClickedExtendBtn()
{
	if (ratio != 0)
	{
		ratio *= 0.9;

		InvalidateRect(rec);
	}
}
void CArcSurface::OnBnClickedShrinkBtn()
{
	if (ratio != 0)
	{
		ratio *= 1.1;

		InvalidateRect(rec);
	}
}
void CArcSurface::OnBnClickedTipsBtn()
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
void CArcSurface::SortDraw(CDC* dc, CPtrArray& m_Array)
{
	CPtrArray XSort, YSort;//记录X侧面和Y侧面
	int m_AddDistance = 50;

	for (int i = 0; i != m_Array.GetSize(); ++i)
	{
		if (i == m_Array.GetSize() - 1)//最后一个元素存储为Y侧面
		{
			YSort.Add(m_Array.GetAt(i));

		}
		else//其余元素存储为X侧面
		{
			XSort.Add(m_Array.GetAt(i));
		}
	}
	CRect XRect, YRect;
	XRect = BoundingBox(XSort);
	YRect = BoundingBox(YSort);

	m_ArcPieceLong = XRect.right - XRect.left;
	m_ArcPieceWidth = YRect.top - YRect.bottom;
	m_ArcPieceHigth = XRect.top;
	m_ArcXSideRadius = XRadius;
	m_ArcYSideRadius = YRadius;
	UpdateData(FALSE);

	XRect.left = XRect.left / ratio;
	XRect.right = XRect.right / ratio;
	XRect.top = rec.Height() - m_AddDistance-XRect.top / ratio;
	XRect.bottom = rec.Height() - m_AddDistance - XRect.bottom / ratio;

	YRect.left = YRect.left / ratio;
	YRect.right = YRect.right / ratio+1;//适当调整
	YRect.top = rec.Height() - m_AddDistance - YRect.top / ratio;
	YRect.bottom = rec.Height() - m_AddDistance - YRect.bottom / ratio;

	//标上标号
	DrawaLabel(dc, XRect);
	DrawYLabel(dc, YRect);

	CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* pPen = dc->SelectObject(&GreenPen);
	dc->Rectangle(XRect);
	dc->Rectangle(YRect);
	dc->SelectObject(pPen);
}
void CArcSurface::DrawaLabel(CDC* m_pDC, CRect Rect)
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
	p2.y = Rect.bottom+60;

	p3.x = Rect.left;
	p3.y = Rect.bottom + 5;

	p4.x = Rect.right;
	p4.y = Rect.bottom + 5;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);
	DrawArrow(m_pDC, p3, p4, 45, 5);

	//标注线旁写上“L”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	
	m_pDC->MoveTo(Rect.left, Rect.bottom);
	m_pDC->LineTo(Rect.left, Rect.bottom+ 5);

	m_pDC->MoveTo(Rect.right - 1, Rect.bottom);
	m_pDC->LineTo(Rect.right - 1, Rect.bottom+5);
	m_pDC->TextOutW(Rect.left + Rect.Width()*0.5, Rect.bottom + 10, _T("L"));


	//标注线上写“H”
	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left - 6, Rect.top);

	m_pDC->MoveTo(Rect.left, Rect.bottom+60);
	m_pDC->LineTo(Rect.left - 6, Rect.bottom+60);
	m_pDC->TextOutW(Rect.left-14, Rect.top + 25, _T("H"));


	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}
void CArcSurface::DrawYLabel(CDC* m_pDC, CRect Rect)
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
	p1.x = Rect.right+5;
	p1.y = Rect.top;

	p2.x = Rect.right+5;
	p2.y = Rect.bottom-1;


	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);


	//画延伸线
	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right+ 6, Rect.top);

	m_pDC->MoveTo(Rect.right, Rect.bottom-1);
	m_pDC->LineTo(Rect.right+6, Rect.bottom-1);

	//标注线旁写上“W”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.right+10, Rect.top + Rect.Height()*0.3, _T("W"));


	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}
void CArcSurface::DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length)
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
BOOL CArcSurface::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/*if (pDoc->tool.GetArcArray().GetSize() != 0)
	{
		pDoc->tool.GetArcArray().Copy(m_TempArray);
	}*/
	m_WorkBlankLong = pDoc->tool.GetArcWorkBlankLongth();
	m_WorkBlankWidth = pDoc->tool.GetArcWorkBlankWidth();
	m_WorkBlankHeigth = pDoc->tool.GetArcWorkBlankHeigth();

	m_Rough_YArcFeedSpeed = pDoc->tool.GetYArcRoughFeedSpeed();
	m_Rough_YArcFeedTime = pDoc->tool.GetYArcRoughFeedTime();
	m_Rough_XArcFeedSpeed = pDoc->tool.GetXArcRoughFeedSpeed();
	m_Rough_XArcFeedTime = pDoc->tool.GetXArcRoughFeedTime();
	m_Rough_VerticalDown = pDoc->tool.GetArcRoughVerticalDistance();
	m_Rough_NumberOfPiles = pDoc->tool.GetArcRoughPiles();
	m_Rough_Left = pDoc->tool.GetArcRoughLeft();

	m_Finish_YArcFeedSpeed = pDoc->tool.GetYArcFinishFeedSpeed();
	m_Finish_YArcFeedTime = pDoc->tool.GetYArcFinishFeedTime();
	m_Finish_XArcFeedSpeed = pDoc->tool.GetXArcFinishFeedSpeed();
	m_Finish_XArcFeedTime = pDoc->tool.GetXArcFinishFeedTime();
	m_Finish_VerticalDistance = pDoc->tool.GetArcFinishVerticalDistance();
	m_Finish_NumberOfPile = pDoc->tool.GetArcFinishPiles();


	//设置初始值有助调试
	m_Rough_YArcFeedSpeed = 10000;
	m_Rough_YArcFeedTime =60;
	m_Rough_XArcFeedSpeed =10000;
	m_Rough_XArcFeedTime =60;
	m_Rough_VerticalDown = 100;
	m_Rough_NumberOfPiles = 2;
	m_Rough_Left = 20;

	m_Finish_YArcFeedSpeed = 3000;
	m_Finish_YArcFeedTime = 60;
	m_Finish_XArcFeedSpeed = 3000;
	m_Finish_XArcFeedTime = 60;
	m_Finish_VerticalDistance =10;
	m_Finish_NumberOfPile =2;

	//三段圆弧
	m_WorkBlankLong = 1462;
	m_WorkBlankWidth = 380;
	m_WorkBlankHeigth =1000;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL CArcSurface::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);
			m_Rough_YArcFeedLoad = m_Rough_YArcFeedSpeed*m_Rough_YArcFeedTime / 60;
			m_Rough_XArcFeedLoad = m_Rough_XArcFeedSpeed*m_Rough_XArcFeedTime / 60;

			m_Finish_YArcFeedLoad = m_Finish_YArcFeedSpeed*m_Finish_YArcFeedTime / 60;
			m_Finish_XArcFeedLoad = m_Finish_XArcFeedSpeed*m_Finish_XArcFeedTime / 60;

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
