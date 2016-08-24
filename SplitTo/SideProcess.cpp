// SideProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "SplitTo.h"
#include "SideProcess.h"
#include "afxdialogex.h"
#include "Graphic.h"

// CSideProcess 对话框

IMPLEMENT_DYNAMIC(CSideProcess, CDialogEx)

CSideProcess::CSideProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSideProcess::IDD, pParent)
	, m_PieceLong(0)
	, m_PieceWidth(0)
	, m_PieceHeigth(0)
	, m_PieceExtend(0)
	, m_Feed(0)
	, m_Time(0)
	, m_LongSet(0)
	, m_WidthSet(0)
	, m_HeigthSet(0)
	, m_Load(0)
	, m_Feed_VX(0)
	, m_Time_VX(0)
	, m_Load_VX(0)
	, m_DeapthOfCut(0)
	, m_RoughLeft(0)
	, m_Speed_Finish(0)
	, m_Time_Finish(0)
	, m_Speed_Finish_VX(0)
	, m_Time_Finish_VX(0)
	, m_Load_Finish(0)
	, m_Load_Finish_VX(0)
	, m_DepthOfCut_Finish(0)
	, m_Rough_Plies(0)
{
	m_BoundingBox_2D.left = 0;
	m_BoundingBox_2D.right = 0;
	m_BoundingBox_2D.top = 0;
	m_BoundingBox_2D.bottom = 0;
	ratio = 20.0;

	PieceSize.left = 0;
	PieceSize.right = 0;
	PieceSize.top = 0;
	PieceSize.bottom = 0;

	PieceSizeXY.left = 0;
	PieceSizeXY.right = 0;
	PieceSizeXY.top = 0;
	PieceSizeXY.bottom = 0;

	PieceSizeXZ.left = 0;
	PieceSizeXZ.right = 0;
	PieceSizeXZ.top = 0;
	PieceSizeXZ.bottom = 0;

	m_IsShow = FALSE;
}

CSideProcess::~CSideProcess()
{
	
}

void CSideProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LONG, m_PieceLong);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_PieceWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_PieceHeigth);
	DDX_Text(pDX, IDC_EDIT_EXTEND, m_PieceExtend);
	DDX_Text(pDX, IDC_EDIT1, m_Feed);
	DDX_Text(pDX, IDC_EDIT3, m_Time);
	DDX_Text(pDX, IDC_EDIT5, m_LongSet);
	DDX_Text(pDX, IDC_EDIT6, m_WidthSet);
	DDX_Text(pDX, IDC_EDIT7, m_HeigthSet);
	DDX_Text(pDX, IDC_EDIT4, m_Load);
	DDX_Text(pDX, IDC_EDIT2, m_Feed_VX);
	DDX_Text(pDX, IDC_EDIT8, m_Time_VX);
	DDX_Text(pDX, IDC_EDIT9, m_Load_VX);
	DDX_Text(pDX, IDC_EDIT10, m_DeapthOfCut);
	DDX_Text(pDX, IDC_EDIT11, m_RoughLeft);
	DDX_Text(pDX, IDC_EDIT13, m_Speed_Finish);
	DDX_Text(pDX, IDC_EDIT14, m_Time_Finish);
	DDX_Text(pDX, IDC_EDIT16, m_Speed_Finish_VX);
	DDX_Text(pDX, IDC_EDIT17, m_Time_Finish_VX);
	DDX_Text(pDX, IDC_EDIT15, m_Load_Finish);
	DDX_Text(pDX, IDC_EDIT18, m_Load_Finish_VX);
	DDX_Text(pDX, IDC_EDIT19, m_DepthOfCut_Finish);
	DDX_Text(pDX, IDC_EDIT12, m_Rough_Plies);
}


BEGIN_MESSAGE_MAP(CSideProcess, CDialogEx)
	ON_BN_CLICKED(IDC_SIDE_OPEN_FILE, &CSideProcess::OnBnClickedSideOpenFile)
	ON_WM_PAINT()
//	ON_STN_CLICKED(IDC_PICTURE, &CSideProcess::OnStnClickedPicture)
ON_BN_CLICKED(IDC_AMPHSIZE, &CSideProcess::OnBnClickedAmphsize)
ON_BN_CLICKED(IDC_SHRINK, &CSideProcess::OnBnClickedShrink)
ON_WM_ERASEBKGND()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_SHOW_TIP, &CSideProcess::OnBnClickedShowTip)
END_MESSAGE_MAP()


// CSideProcess 消息处理程序


void CSideProcess::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CSideProcess::OnOK()//如何设置参数为零时候的提示！！
{
		//CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
		CSplitToDoc* pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

		pDoc->tool.SetArray(m_TempArray);//将数据进行复制
		pDoc->tool.SetFeedspeed(m_Feed);
		pDoc->tool.SetTime(m_Time);
		pDoc->tool.SetSpeedVx(m_Feed_VX);
		pDoc->tool.SetTimeVx(m_Time_VX);

		pDoc->tool.SetSpeedFinish(m_Speed_Finish);
		pDoc->tool.SetTimeFinish(m_Time_Finish);
		pDoc->tool.SetSpeedFinishVX(m_Speed_Finish_VX);
		pDoc->tool.SetTimeFinishVX(m_Time_Finish_VX);

		pDoc->tool.SetLongth(m_LongSet);
		pDoc->tool.SetWidth(m_WidthSet);
		pDoc->tool.SetHeigth(m_HeigthSet);
		pDoc->tool.SetDepthCut(m_DeapthOfCut);
		pDoc->tool.SetDepthCutFinish(m_DepthOfCut_Finish);
		pDoc->tool.SetRoughLeft(m_RoughLeft);

		pDoc->tool.SetRoughPlies(m_Rough_Plies);//设置粗加工层数

		pDoc->tool.ChangeSideProcess();//将数据进行计算，不可以放到view类中进行计算，它会一直计算！！
		pDoc->tool.RenderSideInit();
		pDoc->UpdateAllViews(NULL);
		DestroyWindow();
	
	//CDialogEx::OnOK();
}


void CSideProcess::OnCancel()
{
	DestroyWindow();

	//CDialogEx::OnCancel();
}


void CSideProcess::OnBnClickedSideOpenFile()//通过读取DXF的信息，进行二维图形显示，并刷新Doc中的数据，当点击确定按钮的时候就在view中进行显示
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
		GetDlgItem(IDC_FILEPATH)->SetWindowTextW(FilePath);
		
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

	PieceSize =m_BoundingBox_2D = BoundingBox(m_TempArray);//整体的零件的边界

	m_CorCenter.x = PieceSize.left + (PieceSize.right - PieceSize.left) / 2;
	m_CorCenter.y = PieceSize.bottom + (PieceSize.top - PieceSize.bottom) / 2;
	if (m_CorCenter.x < m_CorCenter.y)
		ratio = m_CorCenter.x / 65;
	else
		ratio = m_CorCenter.y / 65;
	}

	Invalidate();//需要刷新,程序进入对话框先执行OnPaint函数，再执行打开操作，要想打开操作之后进行图形的显示，要对画图区域进行重绘
}


void  CSideProcess::SortDraw(CDC* dc,CPtrArray& m_Array)
{
	CPtrArray m_Sort1;
	CPtrArray m_Sort2;

	CVector3D IsSealing;//判断是否闭合
	 
	int mark=0;//用于存储封闭图形有几个元素

	for (int i = 0; i != m_Array.GetSize(); ++i)
	{
		if (((CLine*)m_Array.GetAt(i))->flag == 1)
		{
			CVector3D temp;
			temp.dx = ((CLine*)m_Array.GetAt(i))->m_end.x - ((CLine*)m_Array.GetAt(i))->m_begin.x;
			temp.dy = ((CLine*)m_Array.GetAt(i))->m_end.y - ((CLine*)m_Array.GetAt(i))->m_begin.y;
			temp.dz = ((CLine*)m_Array.GetAt(i))->m_end.z - ((CLine*)m_Array.GetAt(i))->m_begin.z;

			IsSealing += temp;


		}
		else if (((CArc*)m_Array.GetAt(i))->flag == 2)
		{
			CVector3D temp;
			if (((CArc*)m_Array.GetAt(i))->m_Tudu == 0)
			{

				((CArc*)m_Array.GetAt(i))->Calculate(((CArc*)m_Array.GetAt(i))->m_center, ((CArc*)m_Array.GetAt(i))->m_radio, ((CArc*)m_Array.GetAt(i))->m_begin_angle,
					((CArc*)m_Array.GetAt(i))->m_end_angle, m_Array, i);

				temp.dx = ((CArc*)m_Array.GetAt(i))->m_end.x - ((CArc*)m_Array.GetAt(i))->m_begin.x;
				temp.dy = ((CArc*)m_Array.GetAt(i))->m_end.y - ((CArc*)m_Array.GetAt(i))->m_begin.y;
				temp.dz = ((CArc*)m_Array.GetAt(i))->m_end.z - ((CArc*)m_Array.GetAt(i))->m_begin.z;
			}
			else
			{
				temp.dx = ((CArc*)m_Array.GetAt(i))->m_end.x - ((CArc*)m_Array.GetAt(i))->m_begin.x;
				temp.dy = ((CArc*)m_Array.GetAt(i))->m_end.y - ((CArc*)m_Array.GetAt(i))->m_begin.y;
				temp.dz = ((CArc*)m_Array.GetAt(i))->m_end.z - ((CArc*)m_Array.GetAt(i))->m_begin.z;
			}

			IsSealing += temp;

		}
		else if (((CCircle*)m_Array.GetAt(i))->flag == 3)//圆的话则无须考虑，这里的侧面加工就是底面不是圆形的时候的加工
		{

		}

		if (IsSealing.GetLength() < 0.1)//如果该向量的长度等于0则表示其是闭合的
		{
			mark = i;
			for (int j = 0; j <= mark; ++j)//将m_Array的前面mark+1个元素复制到m_Sort1中
			{
				m_Sort1.Add(m_Array.GetAt(j));
			}
			for (int j = mark+1 ; j != m_Array.GetSize(); ++j)//将m_Array的mark+1个元素后面的元素复制到m_Sort2中
			{
				m_Sort2.Add(m_Array.GetAt(j));
			}

			break;//等于零的时候则跳出循环
		}
		
	}
       
	if (m_Sort1.IsEmpty() == 0 && m_Sort2.IsEmpty() == 0)
	{
		CRect rec1 = BoundingBox(m_Sort1);
		CRect rec2 = BoundingBox(m_Sort2);

		m_PieceLong = rec1.right - rec1.left;
		m_PieceWidth = rec1.top - rec1.bottom;
		m_PieceHeigth = rec2.top-rec2.bottom;
		m_PieceExtend = rec2.right - rec2.left;
		UpdateData(FALSE);

		rec1.left = rec1.left / ratio;
		rec1.right = rec1.right / ratio;
		rec1.top = rec.Height()- rec1.top/ratio;
		rec1.bottom = rec.Height() - rec1.bottom /ratio;

		rec2.left = rec2.left / ratio;
		rec2.right = rec2.right / ratio;
		rec2.top = rec.Height() - rec2.top/ratio;
		rec2.bottom = rec.Height() - rec2.bottom/ratio;

		//绘制标注线&&提示
		DrawaLabel(dc, rec1);
		DrawaLabelHeigth(dc, rec2);

		CPen GreenPen(PS_SOLID,1, RGB(0, 255, 0));

		CPen* pBeforeRed = dc->SelectObject(&GreenPen);
		dc->Rectangle(rec1);
		dc->Rectangle(rec2);
		dc->SelectObject(pBeforeRed);
	}
	
}

void CSideProcess::DrawaLabel(CDC* m_pDC, CRect Rect)
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

	CPen Pen(PS_SOLID, 1, RGB(0,255,0));
	CPen* pOldPen=m_pDC->SelectObject(&Pen);

	CPoint p1, p2,p3,p4;
	p1.x = Rect.left-5;
	p1.y = Rect.top;

	p2.x = Rect.left-5;
	p2.y = Rect.bottom;

	p3.x = Rect.left;
	p3.y = Rect.top -5;

	p4.x = Rect.right;
	p4.y = Rect.top -5;

	//画标注箭头线
	DrawArrow(m_pDC, p1, p2, 45, 5);
	DrawArrow(m_pDC, p3, p4, 45, 5);


	//画延伸线
	m_pDC->MoveTo(Rect.left, Rect.bottom);
	m_pDC->LineTo(Rect.left - 6, Rect.bottom);

	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left - 6 , Rect.top);

	//标注线旁写上“W”
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));
	m_pDC->TextOutW(Rect.left - 15, Rect.top + Rect.Height()*0.3,_T("W"));
	

	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left, Rect.top-6);

	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right, Rect.top-6);

	//标注线上写上“L”
	m_pDC->TextOutW(Rect.left+Rect.Width()*0.5, Rect.top-20, _T("L"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}

void CSideProcess::DrawaLabelHeigth(CDC* m_pDC, CRect Rect)
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

	CPoint  p1, p2, p3, p4;
	p3.x = Rect.right+5;
	p3.y = Rect.top;

	p4.x = Rect.right+5;
	p4.y = Rect.bottom;


	p1.x = Rect.left;
	p1.y = Rect.top - 5;

	p2.x = Rect.right;
	p2.y = Rect.top - 5;

	//画标注箭头线
	DrawArrow(m_pDC, p3, p4, 45, 5);
	DrawArrow(m_pDC, p1, p2, 45, 5);

	//画延伸线

	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right+6, Rect.top);

	m_pDC->MoveTo(Rect.right, Rect.bottom);
	m_pDC->LineTo(Rect.right+6, Rect.bottom);

	m_pDC->MoveTo(Rect.left, Rect.top);
	m_pDC->LineTo(Rect.left, Rect.top - 6);

	m_pDC->MoveTo(Rect.right, Rect.top);
	m_pDC->LineTo(Rect.right, Rect.top - 6);

	//标注线上写上H
	CFont* pOldFont = m_pDC->SelectObject(&font);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SetTextColor(RGB(50, 150, 200));

	m_pDC->TextOutW(Rect.right + 15, Rect.top + Rect.Height()*0.3, _T("H"));
	m_pDC->TextOutW(Rect.left + Rect.Width()*0.5, Rect.top - 20, _T("E"));

	m_pDC->SelectObject(pOldFont);
	m_pDC->SelectObject(pOldPen);
}

void CSideProcess::DrawArrow(CDC* m_pDC, CPoint p1, CPoint p2, double theta, int length)
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

void CSideProcess::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// 不为绘图消息调用 CDialogEx::OnPaint()
	
	CDC* pDC;
	pDC = GetDlgItem(IDC_PICTURE)->GetDC();

	GetDlgItem(IDC_PICTURE)->Invalidate();//刷新窗口才会显示图形
	GetDlgItem(IDC_PICTURE)->UpdateWindow();

	    
		GetDlgItem(IDC_PICTURE)->GetClientRect(rec);

		

		//float m_ratio =15.0;
		CPen RedPen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* OldPen = pDC->SelectObject(&RedPen);


		for (int i = 0; i != m_TempArray.GetSize(); ++i)
		{
			if (((CLine*)m_TempArray.GetAt(i))->flag == 1)//直线
			{
				pDC->MoveTo(((CLine*)m_TempArray.GetAt(i))->m_begin.x/ratio, rec.Height()-((CLine*)m_TempArray.GetAt(i))->m_begin.y/ratio);
				pDC->LineTo(((CLine*)m_TempArray.GetAt(i))->m_end.x / ratio, rec.Height()-((CLine*)m_TempArray.GetAt(i))->m_end.y / ratio);

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
					for (vector<CPoint3D>::iterator it = pt.begin(); it != pt.end()-1; ++it)
					{
						pDC->MoveTo(it->x/ratio, rec.Height()-it->y/ratio);
						pDC->LineTo((it + 1)->x/ratio, rec.Height()-(it + 1)->y/ratio);
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

		}

		//画出绿框包容盒

		CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0));
		pDC->SelectObject(GreenPen);
		m_BoundingBox_2D.left = PieceSize.left / ratio;
		m_BoundingBox_2D.right = PieceSize.right / ratio;
		m_BoundingBox_2D.top = rec.Height() - PieceSize.top / ratio;
		m_BoundingBox_2D.bottom = rec.Height() - PieceSize.bottom / ratio;

		CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));//空画刷
		CBrush* pOldBrush = pDC->SelectObject(pBrush);
		//pDC->Rectangle(m_BoundingBox_2D);
		
		if (m_TempArray.IsEmpty() ==0&&m_IsShow)//设置一个TRUEor FALSE的变量即可以调节可看的见框及不可见框
		{
			SortDraw(pDC, m_TempArray);

		}

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(OldPen);
		
	  

}


CRect CSideProcess::BoundingBox(CPtrArray& Array)//比较m_TempArray里面的数据得出X\Y的最大最小值,这里要用引用否则会编译错误
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




void CSideProcess::OnBnClickedAmphsize()
{
	
	ratio*=0.9;
	Invalidate(FALSE);
}


void CSideProcess::OnBnClickedShrink()
{
	
	ratio*=1.1;
	Invalidate();
	
}


BOOL CSideProcess::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//消除闪屏

	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CSideProcess::OnDestroy()
{
	CDialogEx::OnDestroy();

}


void CSideProcess::OnBnClickedShowTip()
{
	if (m_IsShow)
		m_IsShow = FALSE;
	else
		m_IsShow = TRUE;
	InvalidateRect(rec);
}


BOOL CSideProcess::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	//屏蔽掉Enter键和Esc按钮
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_RETURN:
			UpdateData(TRUE);
			m_Load = m_Feed*m_Time/60;
			m_Load_VX = m_Feed_VX*m_Time_VX/60;

			m_Load_Finish = m_Speed_Finish*m_Time_Finish/60;
			m_Load_Finish_VX = m_Speed_Finish_VX*m_Time_Finish_VX/60;

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


BOOL CSideProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMainFrame* pAppFrame = (CMainFrame*)AfxGetMainWnd();
	CSplitToDoc* pDoc = (CSplitToDoc*)pAppFrame->GetActiveDocument();

	
	m_Feed=pDoc->tool.GetFeedspeed();
	m_Time=pDoc->tool.GetTime();
	m_Feed_VX=pDoc->tool.GetSpeedVx();
	m_Time_VX=pDoc->tool.GetTimeVx();


	m_Speed_Finish=pDoc->tool.GetSpeedFinish();
	m_Time_Finish=pDoc->tool.GetTimeFinish();
	m_Speed_Finish_VX=pDoc->tool.GetSpeedFinishVX();
	m_Time_Finish_VX=pDoc->tool.GetTimeFinishVX();

	m_LongSet=pDoc->tool.GetLongth();
	m_WidthSet=pDoc->tool.GetWidth();
	m_HeigthSet=pDoc->tool.GetHeigth();
	m_DeapthOfCut=pDoc->tool.GetDepthCut();
	m_DepthOfCut_Finish=pDoc->tool.GetDepthCutFinish();
	m_RoughLeft=pDoc->tool.GetRoughLeft();

	m_Rough_Plies=pDoc->tool.GetRoughPlies();

	//设置初值有便调试
	m_Feed = 10000;
	m_Time = 60;
	m_Feed_VX = 10000;
	m_Time_VX = 60;
	m_Speed_Finish = 5000;
	m_Time_Finish = 60;
	m_Speed_Finish_VX = 5000;
	m_Time_Finish_VX = 60;
	m_LongSet = 1100;//4200;
	m_WidthSet = 700;// 3200;
	m_HeigthSet = 368;// 1200;
	m_DeapthOfCut = 100;// 500;
	m_DepthOfCut_Finish = 25;// 50;
	m_RoughLeft = 50;// 100;
	m_Rough_Plies =1;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
