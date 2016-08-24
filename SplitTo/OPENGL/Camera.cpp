#include "StdAfx.h"
#include "Camera.h"
#include"gl\gl.h"
#include "gl\GLU.h"

GCamera::GCamera(void)
{
}


GCamera::~GCamera(void)
{
}
void GCamera::projection() 
{
	//switch to projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glRenderMode(GL_RENDER);

	//apply projective matrix
	double left		=  - m_width/2.0;
	double right	=  m_width/2.0;
	double bottom	=  - m_height/2.0;
	double top		=  m_height/2.0;

	glOrtho(left,right,bottom,top,m_near,m_far);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	gluLookAt(m_eye.x,m_eye.y,m_eye.z,m_ref.x,m_ref.y,m_ref.z,m_vecUp.dx, m_vecUp.dy, m_vecUp.dz);	
}


void GCamera::selection(int xPos,int yPos)
{
	GLint	vp[4];
	glGetIntegerv(GL_VIEWPORT,vp);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glRenderMode(GL_SELECT);
	gluPickMatrix(xPos,vp[3]-yPos, 1, 1, vp );

	//apply projective matrix
	double left		=  - m_width/2.0;
	double right	=  m_width/2.0;
	double bottom	=  - m_height/2.0;
	double top		=  m_height/2.0;

	glOrtho(left,right,bottom,top,m_near,m_far);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	gluLookAt(m_eye.x,m_eye.y,m_eye.z,m_ref.x,m_ref.y,m_ref.z,
		m_vecUp.dx, m_vecUp.dy, m_vecUp.dz);
}


void GCamera::init()
{
	m_eye = CPoint3D(0,0,500);
	m_ref = CPoint3D(0,0,0);
	m_far = 15000;
	m_near= -15000;

	m_width = 2400.0;
	m_height = 2400.0;

	m_vecUp = CVector3D(0,1,0);

	m_screen[0] = 400;
	m_screen[1] = 400;
}

void GCamera::set_screen( int x, int y) 
{ 
	glViewport(0,0,x,y);
	if(y==0) y=1;
	double ratio = (double)x/(double)y;
	m_width *= (double)x/m_screen[0];
	m_height *= (double)y/m_screen[1];
	m_width =  m_height*ratio;
	m_screen[0] = x;
	m_screen[1] = y; 
}

void GCamera::set_eye(double eye_x,double eye_y,double eye_z)
{
	m_eye.x = eye_x;
	m_eye.y = eye_y;
	m_eye.z = eye_z;
}

void GCamera::set_ref(double ref_x,double ref_y,double ref_z)
{
	m_ref.x = ref_x;
	m_ref.y = ref_y;
	m_ref.z = ref_z;
}

void GCamera::set_vecUp(double up_dx,double up_dy,double up_dz)
{
	m_vecUp.dx = up_dx;
	m_vecUp.dy = up_dy;
	m_vecUp.dz = up_dz;
}

void GCamera::set_view_rect(double width,double height)
{
	m_width = width;
	m_height = height;
	double aspect = m_screen[0]/m_screen[1];
	m_width =  m_height*aspect;
}

void GCamera::get_view_rect(double& width,double& height)
{
	width = m_width;
	height = m_height;
}
void GCamera::zoom(double scale)
{
	ASSERT(scale > 0.0);
	m_width *= scale;
	m_height *= scale;
}

void GCamera::zoom_all(double x0,double y0,double z0,double x1,double y1,double z1)
{
	double width,height;
	double	xl, yl, zl;
	xl = x1-x0;
	yl = y1-y0;
	zl = z1-z0;

	width = max(max(xl,yl),zl);
	height= max(max(xl,yl),zl);

	set_view_rect(width,height);

	CVector3D vec = m_eye - m_ref;
	m_ref.x = (x0+x1)/2;
	m_ref.y = (y0+y1)/2;
	m_ref.z = (z0+z1)/2;
	m_eye = m_ref + vec;
}

void GCamera::set_view_type( int type )
{
	double r;
	CVector3D vec;

	vec = m_ref - m_eye;
	r = vec.GetLength();

	if(IS_ZERO(r)) r = 50.0;
	if( r > 10000)  r = 10000;

	switch(type){
	case VIEW_FRONT:
		m_eye = m_ref + CVector3D(0,-r,0);
		m_vecUp = CVector3D(0,0,1);
		break;
	case VIEW_BACK:
		m_eye = m_ref + CVector3D(0,r,0);
		m_vecUp = CVector3D(0,0,1);
		break;
	case VIEW_TOP:
		m_eye = m_ref + CVector3D(0,0,r);
		m_vecUp = CVector3D(0,1,0);
		break;
	case VIEW_BOTTOM:
		m_eye = m_ref + CVector3D(0,0,-r);
		m_vecUp = CVector3D(0,1,0);
		break;
	case VIEW_RIGHT:
		m_eye = m_ref + CVector3D(r,0,0);
		m_vecUp = CVector3D(0,0,1);
		break;
	case VIEW_LEFT:
		m_eye = m_ref + CVector3D(-r,0,0);
		m_vecUp = CVector3D(0,0,1);
		break;
	case VIEW_SW_ISOMETRIC:
		m_eye = m_ref + CVector3D(-1,-1,1).GetNormal()*r;
		update_upVec();		
		break;
	case VIEW_SE_ISOMETRIC:
		m_eye = m_ref + CVector3D(1,-1,1).GetNormal()*r;
		update_upVec();		
		break;
	case VIEW_NE_ISOMETRIC:
		m_eye = m_ref + CVector3D(1,1,1).GetNormal()*r;
		update_upVec();
		break;
	case VIEW_NW_ISOMETRIC:
		m_eye = m_ref + CVector3D(-1,1,1).GetNormal()*r;
		update_upVec();
		break;
	}
}

void GCamera::move_view(double dpx, double dpy)
{
	CVector3D vec;
	CVector3D xUp, yUp;

	vec = m_ref - m_eye;
	vec.Normalize();
	xUp = vec*m_vecUp;
	yUp = xUp*vec;

	m_eye -= xUp*m_width*dpx + yUp*m_height*dpy;
	m_ref -= xUp*m_width*dpx + yUp*m_height*dpy;
}

void GCamera::update_upVec()
{
	CVector3D vec = m_ref - m_eye;
	CVector3D zVec(0,0,1);
	CVector3D vec0;

	vec.Normalize();
	vec0 = vec*zVec; 
	m_vecUp = vec0*vec;
}