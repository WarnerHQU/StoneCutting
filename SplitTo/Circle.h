#pragma once
#include"OPENGL\CadBase.h"
#include"OPENGL\OpenGLDC.h"
#include<vector>
using namespace std;

class CCircle
{
public:
	CCircle(void);
	~CCircle(void);
	CPoint3D m_center;
	float  m_radio;

	UINT flag;

	static void GLDrawCircle(CPoint3D center,float radius);
	static void GLDrawCircleXZ(CPoint3D center, float radius);
	static void GLDrawCircleYZ(CPoint3D center, float radius);

public://圆上点的离散化
	static vector<CPoint3D> CircleTDMInterpolation(CPoint3D center, float radius, CPoint3D EnterPoint, float Inter, float F);

};

