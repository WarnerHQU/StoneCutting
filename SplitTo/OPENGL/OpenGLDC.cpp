// OpenGLDC.cpp : 实现文件
//

#include "stdafx.h"
#include "OpenGLDC.h"


#define GL_PI 3.1415f
// COpenGLDC

COpenGLDC::COpenGLDC(HWND hWnd):m_hWnd(hWnd)
{
	m_bSelectionMode = FALSE;
}

COpenGLDC::~COpenGLDC()
{
}


// COpenGLDC 成员函数
BOOL COpenGLDC::InitDC()
{
	if (m_hWnd == NULL) return FALSE;

	m_Camera.init();

	m_hDC = ::GetDC(m_hWnd);			// Get the Device context

	PIXELFORMATDESCRIPTOR pfdWnd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Structure size.
		1,                             // Structure version number.
		PFD_DRAW_TO_WINDOW |           // Property flags.
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,                            // 24-bit color.
		0, 0, 0, 0, 0, 0,              // Not concerned with these.
		0, 0, 0, 0, 0, 0, 0,           // No alpha or accum buffer.
		32,                            // 32-bit depth buffer.
		0, 0,                          // No stencil or aux buffer.
		PFD_MAIN_PLANE,                // Main layer type.
		0,                             // Reserved.
		0, 0, 0                        // Unsupported.
	};

	int pixelformat;
	if ( (pixelformat = ChoosePixelFormat(m_hDC, &pfdWnd)) == 0 )
	{
		AfxMessageBox(_T("ChoosePixelFormat to wnd failed"));
		return FALSE;
	}

	if (SetPixelFormat(m_hDC, pixelformat, &pfdWnd) == FALSE)
		AfxMessageBox(_T("SetPixelFormat failed"));

	m_hRC=wglCreateContext(m_hDC);

	VERIFY(wglMakeCurrent(m_hDC,m_hRC));
	GLSetupRC();
	wglMakeCurrent(NULL,NULL);
	return m_hRC!=0;
}


void COpenGLDC::GLResize(int w,int h)
{
	wglMakeCurrent(m_hDC,m_hRC);

	// Prevent a divide by zero
	if(h == 0) h = 1;
	if(w == 0) w = 1;
	m_Camera.set_screen(w,h);

	//wglMakeCurrent(NULL,NULL);
}

void COpenGLDC::GLSetupRC()
{
	//initialize color and rendering
	m_bShading = TRUE;

	//bright white light - full intensity RGB values
	GLfloat lightAmbient[] = {0.75f,0.75f,0.75f,1.0f};
	GLfloat lightDiffuse[] = {1.0f,1.0f,1.0f,1.0f};

	glEnable(GL_DEPTH_TEST);		//Hidden surface removal
	glEnable(GL_CULL_FACE);			//Do not calculate inside of object
	glFrontFace(GL_CCW);			//counter clock-wise polygons face out
	

	glEnable(GL_LIGHTING);			//enable lighting

	//双面光照（需要显示背面)
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuse);
	SetLightDirection(1,1,1);
	glEnable(GL_LIGHT0);

	//Initialize Material Color to Gray
	SetBkColor(RGB(0,0,0));				//white background
	SetMaterialColor(RGB(0,0,255));	//golden material color
	SetColor(RGB(255,255,255));			//white frame color
	SetHighlightColor(RGB(255,0,0));	//red highlight color	

	//Point Size
	glPointSize(3.0);
}


void COpenGLDC::Ready()
{
	wglMakeCurrent(m_hDC,m_hRC);
	ClearBkground();
	OnShading();
	m_Camera.projection();
}

void COpenGLDC::Finish()
{
	glFlush();
	SwapBuffers(m_hDC);
	wglMakeCurrent(m_hDC,NULL);
}

//////////////////LIGHT && MATERIALS SETTING//////////
void COpenGLDC::ClearBkground()
{
	GLclampf	r,g,b;
	r = (GLclampf)GetRValue(m_clrBk)/255.0;
	g = (GLclampf)GetGValue(m_clrBk)/255.0;
	b = (GLclampf)GetBValue(m_clrBk)/255.0;

	glClearColor(r,g,b,0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//setting model
void COpenGLDC::OnShading()
{
	if(m_bShading){
		glEnable( GL_LIGHTING );
		glEnable(GL_LIGHT0);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
	else{
		glDisable( GL_LIGHTING );
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
}

void COpenGLDC::Shading(BOOL bShading)
{
	m_bShading = bShading;
}

BOOL COpenGLDC::IsShading()
{
	return m_bShading;
}

void COpenGLDC::Lighting(BOOL bLighting)
{
	if(bLighting)
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );
}

BOOL COpenGLDC::IsLighting()
{
	GLboolean bLighting;
	glGetBooleanv(GL_LIGHTING,&bLighting);   
	return bLighting;
}

void COpenGLDC::SetLightDirection(float dx,float dy,float dz)
{
	m_vecLight[0] = dx;
	m_vecLight[1] = dy;
	m_vecLight[2] = dz;
	GLfloat lightPos[] = { dx, dy, dz, 0.0f };
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
}

void COpenGLDC::GetLightDirection(float& dx,float& dy,float& dz)
{
	dx = m_vecLight[0];
	dy = m_vecLight[1];
	dz = m_vecLight[2];
}

//rendering color
void COpenGLDC::SetMaterialColor(COLORREF clr)
{
	m_clrMaterial = clr;
	BYTE r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	GLfloat mat_amb_diff[] = {(GLfloat)r/255,(GLfloat)g/255,(GLfloat)b/255,1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
}

void COpenGLDC::GetMaterialColor(COLORREF& clr)
{
	clr = m_clrMaterial;
}

void COpenGLDC::SetBkColor(COLORREF clr)
{
	m_clrBk = clr;
}

void COpenGLDC::GetBkColor(COLORREF& clr)
{
	clr = m_clrBk;
}

void COpenGLDC::SetColor(COLORREF clr)
{
	m_clr = clr;
	BYTE r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	glColor3ub(r,g,b);
}

void COpenGLDC::GetColor(COLORREF& clr)
{
	clr =  m_clr;
}

void COpenGLDC::SetHighlightColor(COLORREF clr)
{
	m_clrHighlight = clr;
}

void COpenGLDC::GetHighlightColor(COLORREF& clr)
{
	clr = m_clrHighlight;
}

void COpenGLDC::Highlight(BOOL bHighlight)
{
	BYTE r,g,b;
	if(bHighlight){
		r = GetRValue(m_clrHighlight);
		g = GetGValue(m_clrHighlight);
		b = GetBValue(m_clrHighlight);
	}
	else{
		r = GetRValue(m_clrMaterial);
		g = GetGValue(m_clrMaterial);
		b = GetBValue(m_clrMaterial);
	}
	GLfloat mat_amb_diff[] = {(GLfloat)r/255,(GLfloat)g/255,(GLfloat)b/255,1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat_amb_diff);
}


//////////////////////// drawing functions ////////////////////////
//draw point
void COpenGLDC::DrawPoint(const CPoint3D& pt)
{
	glBegin(GL_POINTS);
	glVertex3f(pt.x,pt.y,pt.z);
	glEnd();
}

void COpenGLDC::DrawLine(const CPoint3D& sp,const CPoint3D& ep)
{
	glBegin(GL_LINES);
	glVertex3f(sp.x,sp.y,sp.z);
	glVertex3f(ep.x,ep.y,ep.z);
	glEnd();
}
void COpenGLDC::DrawStippleLine(const CPoint3D& sp, const CPoint3D& ep)
{
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);	
	glVertex3f(sp.x,sp.y,sp.z);
	glVertex3f(ep.x,ep.y,ep.z);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glFlush();
}

void COpenGLDC::DrawArc(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius)
{
	glBegin(GL_LINE_STRIP);
	glColor3f(0.5f, 0.5f, 0.5f);
	float vx, vy;

	//默认顺时针画
	start_angle = 2 * PI*start_angle / 360;
	end_angle = 2 * PI*end_angle / 360;
	if (end_angle<start_angle)
	{
		for (float a = end_angle; a >= start_angle - 2 * PI; a -= delta_angle)
		{
			vx = center.x + radius*cos(a);

			vy = center.y + radius*sin(a);
			glVertex3f(vx, vy, center.z);
		}
	}
	else
	{
		for (float a = start_angle; a <= end_angle; a += delta_angle)
		{
			vx = center.x + radius*cos(a);

			vy = center.y + radius*sin(a);
			glVertex3f(vx, vy, center.z);
		}
	}

	glEnd();
}

void COpenGLDC::DrawPolyline(const CPoint3D* pt,int size)
{
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<size;i++)
		glVertex3f(pt[i].x,pt[i].y,pt[i].z);
	glEnd();
}

void COpenGLDC::DrawRectangleFrame(CPoint3D pt, float Longth, float Width, float Heigth)
{
	//画的方框其都是底面为零，高度为设置的高度
	LinePos line[12];
	//0-3是z=0平面上，4-7是高度平面上的，8-11是连接各个角点的直线
	line[0].begin.x = pt.x - Longth / 2;
	line[0].begin.y = pt.y + Width / 2;
	line[0].begin.z = 0;
	line[0].end.x = pt.x + Longth / 2;
	line[0].end.y = pt.y + Width / 2;
	line[0].end.z = 0;


	line[1].begin.x = pt.x + Longth / 2;
	line[1].begin.y = pt.y + Width / 2;
	line[1].begin.z = 0;
	line[1].end.x = pt.x + Longth / 2;
	line[1].end.y = pt.y - Width / 2;
	line[1].end.z = 0;

	line[2].begin.x = pt.x + Longth / 2;
	line[2].begin.y = pt.y - Width / 2;
	line[2].begin.z = 0;
	line[2].end.x = pt.x- Longth / 2;
	line[2].end.y = pt.y - Width / 2;
	line[2].end.z = 0;

	line[3].begin.x = pt.x - Longth / 2;
	line[3].begin.y = pt.y - Width / 2;
	line[3].begin.z =0;
	line[3].end.x = pt.x - Longth / 2;
	line[3].end.y = pt.y + Width / 2;
	line[3].end.z = 0;

	line[4].begin.x = pt.x - Longth / 2;
	line[4].begin.y = pt.y + Width / 2;
	line[4].begin.z = Heigth;
	line[4].end.x = pt.x + Longth / 2;
	line[4].end.y = pt.y + Width / 2;
	line[4].end.z = Heigth;


	line[5].begin.x = pt.x + Longth / 2;
	line[5].begin.y = pt.y + Width / 2;
	line[5].begin.z = Heigth;
	line[5].end.x = pt.x + Longth / 2;
	line[5].end.y = pt.y - Width / 2;
	line[5].end.z = Heigth;

	line[6].begin.x = pt.x + Longth / 2;
	line[6].begin.y = pt.y - Width / 2;
	line[6].begin.z = Heigth;
	line[6].end.x = pt.x - Longth / 2;
	line[6].end.y = pt.y - Width / 2;
	line[6].end.z = Heigth;

	line[7].begin.x = pt.x - Longth / 2;
	line[7].begin.y = pt.y - Width / 2;
	line[7].begin.z = Heigth;
	line[7].end.x = pt.x - Longth / 2;
	line[7].end.y = pt.y + Width / 2;
	line[7].end.z = Heigth;


	line[8].begin.x = pt.x - Longth / 2;
	line[8].begin.y = pt.y + Width / 2;
	line[8].begin.z = 0;
	line[8].end.x = pt.x - Longth / 2;
	line[8].end.y = pt.y + Width / 2;
	line[8].end.z = Heigth;


	line[9].begin.x = pt.x + Longth / 2;
	line[9].begin.y = pt.y + Width / 2;
	line[9].begin.z = 0;
	line[9].end.x = pt.x + Longth / 2;
	line[9].end.y = pt.y + Width / 2;
	line[9].end.z = Heigth;

	line[10].begin.x = pt.x + Longth / 2;
	line[10].begin.y = pt.y - Width / 2;
	line[10].begin.z = 0;
	line[10].end.x = pt.x + Longth / 2;
	line[10].end.y = pt.y - Width / 2;
	line[10].end.z = Heigth;

	line[11].begin.x = pt.x - Longth / 2;
	line[11].begin.y = pt.y - Width / 2;
	line[11].begin.z = 0;
	line[11].end.x = pt.x - Longth / 2;
	line[11].end.y = pt.y - Width / 2;
	line[11].end.z =Heigth;

	for (int i = 0; i <= 11; ++i)
	{
		DrawLine(line[i].begin, line[i].end);
	}
}
void COpenGLDC::DrawArcFrame(CPtrArray& ArcArray,CPoint3D Center, float Width, float Height)
{
	vector<CArc> Arcvector;
	CPoint3D tempPts[8];
	CArc tempArc;
	tempArc.m_center = ((CArc*)ArcArray.GetAt(0))->m_center;
	tempArc.m_radio = ((CArc*)ArcArray.GetAt(0))->m_radio;
	tempArc.m_begin_angle = ((CArc*)ArcArray.GetAt(0))->m_begin_angle;
	tempArc.m_end_angle = ((CArc*)ArcArray.GetAt(0))->m_end_angle;
	tempArc.m_center.z = Height/*Center.z + Height / 2*/;
	Arcvector.push_back(tempArc);
	tempArc.m_center.z = 0;
	Arcvector.push_back(tempArc);

	tempArc.m_radio = ((CArc*)ArcArray.GetAt(0))->m_radio+Width;
	tempArc.m_center.z = Height/* Center.z + Height / 2*/;
	Arcvector.push_back(tempArc);
	tempArc.m_center.z = 0;
	Arcvector.push_back(tempArc);

	
	float start_angle, end_angle;

	start_angle = 2 * PI*((CArc*)ArcArray.GetAt(0))->m_begin_angle/ 360;
	end_angle = 2 * PI*((CArc*)ArcArray.GetAt(0))->m_end_angle / 360;

	Arcvector[0].m_begin.x = Arcvector[0].m_center.x + Arcvector[0].m_radio*cos(start_angle);
	Arcvector[0].m_begin.y = Arcvector[0].m_center.y + Arcvector[0].m_radio*sin(start_angle);
	Arcvector[0].m_begin.z = Arcvector[0].m_center.z;
	Arcvector[0].m_end.x = Arcvector[0].m_center.x + Arcvector[0].m_radio*cos(end_angle);
	Arcvector[0].m_end.y = Arcvector[0].m_center.y + Arcvector[0].m_radio*sin(end_angle);
	Arcvector[0].m_end.z = Arcvector[0].m_center.z;
	tempPts[0] = Arcvector[0].m_begin;
	tempPts[1] = Arcvector[0].m_end;

	Arcvector[1].m_begin.x = Arcvector[1].m_center.x + Arcvector[1].m_radio*cos(start_angle);
	Arcvector[1].m_begin.y = Arcvector[1].m_center.y + Arcvector[1].m_radio*sin(start_angle);
	Arcvector[1].m_begin.z = Arcvector[1].m_center.z;
	Arcvector[1].m_end.x = Arcvector[1].m_center.x + Arcvector[1].m_radio*cos(end_angle);
	Arcvector[1].m_end.y = Arcvector[1].m_center.y + Arcvector[1].m_radio*sin(end_angle);
	Arcvector[1].m_end.z = Arcvector[1].m_center.z;
	tempPts[2] = Arcvector[1].m_begin;
	tempPts[3] = Arcvector[1].m_end;

	Arcvector[2].m_begin.x = Arcvector[2].m_center.x + Arcvector[2].m_radio*cos(start_angle);
	Arcvector[2].m_begin.y = Arcvector[2].m_center.y + Arcvector[2].m_radio*sin(start_angle);
	Arcvector[2].m_begin.z = Arcvector[2].m_center.z;
	Arcvector[2].m_end.x = Arcvector[2].m_center.x + Arcvector[2].m_radio*cos(end_angle);
	Arcvector[2].m_end.y = Arcvector[2].m_center.y + Arcvector[2].m_radio*sin(end_angle);
	Arcvector[2].m_end.z = Arcvector[2].m_center.z;
	tempPts[4] = Arcvector[2].m_begin;
	tempPts[5] = Arcvector[2].m_end;

	Arcvector[3].m_begin.x = Arcvector[3].m_center.x + Arcvector[3].m_radio*cos(start_angle);
	Arcvector[3].m_begin.y = Arcvector[3].m_center.y + Arcvector[3].m_radio*sin(start_angle);
	Arcvector[3].m_begin.z = Arcvector[3].m_center.z;
	Arcvector[3].m_end.x = Arcvector[3].m_center.x + Arcvector[3].m_radio*cos(end_angle);
	Arcvector[3].m_end.y = Arcvector[3].m_center.y + Arcvector[3].m_radio*sin(end_angle);
	Arcvector[3].m_end.z = Arcvector[3].m_center.z;
	tempPts[6] = Arcvector[3].m_begin;
	tempPts[7] = Arcvector[3].m_end;

	DrawLine(tempPts[0], tempPts[2]);
	DrawLine(tempPts[1], tempPts[3]);
	DrawLine(tempPts[4], tempPts[6]);
	DrawLine(tempPts[5], tempPts[7]);

	DrawLine(tempPts[0], tempPts[4]);
	DrawLine(tempPts[1], tempPts[5]);
	DrawLine(tempPts[2], tempPts[6]);
	DrawLine(tempPts[3], tempPts[7]);

	
	float vx, vy;
	for (vector<CArc>::iterator it = Arcvector.begin(); it != Arcvector.end(); ++it)
	{
		glBegin(GL_LINE_STRIP);
		if (start_angle<end_angle)
		{
			for (float a = start_angle; a <= end_angle; a += 0.001)
			{
				vx = it->m_center.x + it->m_radio*cos(a);
				vy = it->m_center.y + it->m_radio*sin(a);
				glVertex3f(vx, vy, it->m_center.z);
			}
		}
		else
		{
			for (float a = start_angle - 2 * PI; a <= end_angle; a += 0.001)
			{
				vx = it->m_center.x + it->m_radio*cos(a);
				vy = it->m_center.y + it->m_radio*sin(a);
				glVertex3f(vx, vy, it->m_center.z);
			}
		}
		glEnd();
	}
	
	

}

void COpenGLDC::DrawTriChip(double n0,double n1,double n2,double v00,double v01,double v02,
	double v10,double v11,double v12,double v20,double v21,double v22)
{
	glBegin(GL_TRIANGLES);
	glNormal3d(n0,n1,n2);
	glVertex3d(v00,v01,v02);
	glVertex3d(v10,v11,v12);
	glVertex3d(v20,v21,v22);
	glEnd();
}

//Draw 3D Solid
void COpenGLDC::DrawSphere(const CPoint3D& cen,double r,const CVector3D& vec)
{
	glPushMatrix();
	glTranslatef(cen.x,cen.y,cen.z);

	CVector3D vecNY(0,-1,0);
	CVector3D axis = vecNY*vec;
	double ang = _AngleBetween(vecNY,vec);
	ang = ang*180/GL_PI;
	glRotatef(ang,axis.dx,axis.dy,axis.dz);

	auxSolidSphere(r);
	glPopMatrix();
}

void COpenGLDC::DrawCylinder(const CPoint3D& cen,double r,const CVector3D& h)
{
	glPushMatrix();

	glTranslatef(cen.x,cen.y,cen.z);

	CVector3D vecNY(0,-1,0);
	CVector3D axis = vecNY*h;
	double ang = _AngleBetween(vecNY,h);
	ang = ang*180/GL_PI;
	glRotatef(ang,axis.dx,axis.dy,axis.dz);


	auxSolidCylinder(r,h.GetLength());
	glPopMatrix();
}

void COpenGLDC::DrawTorus(const CPoint3D& cen,const CVector3D& ax,double r_in,double r_out)
{
	glPushMatrix();

	glTranslatef(cen.x,cen.y,cen.z);

	CVector3D vecNY(0,-1,0);
	CVector3D axis = vecNY*ax;
	double ang = _AngleBetween(vecNY,ax);
	ang = ang*180/GL_PI;
	glRotatef(ang,axis.dx,axis.dy,axis.dz);


	auxSolidTorus(r_in,r_out);
	glPopMatrix();
}

void COpenGLDC::DrawCone(const CPoint3D& cen,double r,const CVector3D& h)
{
	glPushMatrix();

	glTranslatef(cen.x,cen.y,cen.z);

	CVector3D vecNY(0,-1,0);
	CVector3D axis = vecNY*h;
	double ang = _AngleBetween(vecNY,h);
	ang = ang*180/GL_PI;

	glRotatef(ang,axis.dx,axis.dy,axis.dz);

	GLfloat angle,x,y;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0,0,h.GetLength());
	for(angle =0.0f;angle<(2.0f*GL_PI);angle += (GL_PI/8.0f))
	{
		x = r*sin(angle);
		y = r*cos(angle);
		glVertex2f(x,y);
	}
	glEnd();

	// Begin a new triangle fan to cover the bottom
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f,0.0f);
	for(angle =0.0f;angle<(2.0f*GL_PI);angle += (GL_PI/8.0f))
	{
		x = r*sin(angle);
		y = r*cos(angle);
		glVertex2f(x,y);
	}
	glEnd();
	glPopMatrix();
}

////////////////////////////////////////

void COpenGLDC::DrawCoord()
{

	BOOL bLighting = IsLighting();
	Lighting(FALSE);

	double width,height;
	m_Camera.get_view_rect(width,height);
	double len = min(width,height);
	len *= 0.05;

	CPoint3D cPt,xPt,yPt,zPt;
	xPt.x = yPt.y = zPt.z = len;

	COLORREF old_clr;
	GetColor(old_clr);

	//axis-x: red
	SetColor(RGB(255,0,0));
	DrawLine(cPt,xPt);

	//axis-y: green
	SetColor(RGB(0,255,0));
	DrawLine(cPt,yPt);

	//axis-z: blue
	SetColor(RGB(0,0,255));
	DrawLine(cPt,zPt);

	Lighting(bLighting);
	SetColor(old_clr);

	

}

//////////////////////////////////////////////////
void COpenGLDC::BeginSelection(int xPos,int yPos)
{
	m_bSelectionMode = TRUE;
	wglMakeCurrent(m_hDC,m_hRC);

	GLint	viewport[4];

	//set up selection buffer	
	glSelectBuffer(BUFFER_LENGTH,m_selectBuff);

	//switch to projection and save the matrix
	m_Camera.selection(xPos,yPos);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	InitNames();
}

int	COpenGLDC::EndSelection(UINT* items)
{
	m_bSelectionMode = FALSE;
	int hits = glRenderMode(GL_RENDER);
	for(int i=0;i<hits;i++){
		items[i] = m_selectBuff[i*4+3];
	}
	wglMakeCurrent(m_hDC,NULL);
	return hits;
}

BOOL COpenGLDC::IsSelectionMode()
{
	return m_bSelectionMode;
}

void COpenGLDC::InitNames()
{
	glInitNames();
	glPushName(0);
}

void COpenGLDC::LoadName(UINT name)
{
	glLoadName(name);
}

void COpenGLDC::PushName(UINT name)
{
	glPushName(name);
}

void COpenGLDC::PopName()
{
	glPopName();
}

