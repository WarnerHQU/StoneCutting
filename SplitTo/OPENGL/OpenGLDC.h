#pragma once
#include"gl\GL.h"
#include"gl\GLU.h"
#include"gl\GLAUX.H"
#include"Camera.h"
#include"CadBase.h"
#include"../Arc.h"

// COpenGLDC 命令目标
#define BUFFER_LENGTH 64

typedef struct tagLinePos
{
	CPoint3D begin;
	CPoint3D end;
}LinePos;

class COpenGLDC : public CObject
{
public:
	COpenGLDC(HWND hWnd);
	virtual ~COpenGLDC();
private:
	HWND	m_hWnd;
	HGLRC	m_hRC;
	HDC		m_hDC;

	COLORREF	m_clrBk;		//Background Color
	COLORREF	m_clr;			//Polygon Color for unshading
	COLORREF	m_clrHighlight; //for highlight using
	COLORREF	m_clrMaterial;  //for normal rendering
	BOOL		m_bShading;		//use material property
	GLfloat		m_vecLight[3];	//lighting direction

	//selection
	BOOL	m_bSelectionMode;
	GLuint	m_selectBuff[BUFFER_LENGTH];

protected:
	void ClearBkground();
	void OnShading();

public:
	GCamera	m_Camera;

public:
	//initialize
	BOOL InitDC();
	void GLResize(int cx,int cy);
	void GLSetupRC();

	void Ready();
	void Finish();

	/////////////////////// light & color //////////////////////
	void Shading(BOOL bShading);
	BOOL IsShading();

	void Lighting(BOOL bLighting);
	BOOL IsLighting();

	//Light direction
	void SetLightDirection(float dx,float dy,float dz);
	void GetLightDirection(float& dx,float& dy,float& dz);

	//material
	void SetMaterialColor(COLORREF clr);
	void GetMaterialColor(COLORREF& clr);

	//back ground
	void SetBkColor(COLORREF rgb);
	void GetBkColor(COLORREF& rgb);

	//frame material
	void SetColor(COLORREF rgb);
	void GetColor(COLORREF& rgb);

	//high light setting
	void SetHighlightColor(COLORREF clr);
	void GetHighlightColor(COLORREF& clr);
	void Highlight(BOOL bLight = TRUE);


	/////////////////// Drawing ////////////////////
	void DrawPoint(const CPoint3D&);

	//drawing curves
	void DrawCoord();

	void DrawLine(const CPoint3D& sp,const CPoint3D& ep);
	void DrawStippleLine(const CPoint3D& sp, const CPoint3D& ep);//画虚线
	void DrawArc(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius);
	void DrawPolyline(const CPoint3D* pt,int size);
	void DrawRectangleFrame(CPoint3D pt, float Longth, float Width, float Heigth);
	void DrawArcFrame(CPtrArray& ArcArray, CPoint3D Center,float Width,float Height);//绘制弧形毛坯

	//drawing surface
	void DrawTriChip(double n0,double n1,double n2,double v00,double v01,double v02,
		double v10,double v11,double v12,double v20,double v21,double v22);

	//drawing solid entities
	void DrawSphere(const CPoint3D& cen,double r,const CVector3D& vec);
	void DrawCylinder(const CPoint3D& cen,double r,const CVector3D& h);
	void DrawCone(const CPoint3D& cen,double r,const CVector3D& h);
	void DrawTorus(const CPoint3D& cen,const CVector3D& ax,double r_in,double r_out);

	//selection Mode
	void	BeginSelection(int xPos,int yPos);
	int		EndSelection(UINT* items);
	BOOL	IsSelectionMode();

	void	InitNames();
	void	LoadName(UINT name);
	void	PushName(UINT name);
	void    PopName();

};


