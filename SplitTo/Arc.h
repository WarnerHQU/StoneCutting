#pragma once
#include<vector>
#include"OPENGL\CadBase.h"
#include"OPENGL\OpenGLDC.h"
#include<fstream>
using namespace std;

class CArc 
{
public:
	CArc(void);
	~CArc(void);
	CPoint3D m_begin;
	CPoint3D m_end;
	CPoint3D m_center;
	float m_radio;
	float  m_begin_angle;
	float  m_end_angle;
	UINT flag;
	float m_Tudu;
	BOOL m_Direction;
public:
	vector<CPoint> Pos;

public:
    //XOY平面计算
	//画图形不是根据插补算法，而是根据角度
     void PolyCalculate(CPoint3D begin,CPoint3D end,float TuduAngle);
	void PolyDrawSArc(CPoint3D& center,float start_angle,float end_angle,float delta_angle,float radius);
	void PolyDrawNArc(CPoint3D& center,float start_angle,float end_angle,float delta_angle,float radius);
	static BOOL Calculate(CPoint3D& center,float radio,float beginAngle,float endAngle,CPtrArray& XFarray,int index);//修改后
    void GLDrawArc(CPoint3D& center,float start_angle,float end_angle,float delta_angle,float radius,CPtrArray& XFarray,int index);//修改后
	
	//XOZ平面计算
public:
	void PolyCalculateXZ(CPoint3D begin, CPoint3D end, float TuduAngle);
	void PolyDrawSArcXZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius);
	void PolyDrawNArcXZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius);
	static BOOL CalculateXZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index);
	void GLDrawArcXZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius, CPtrArray& XFarray, int index);


	//YOZ平面计算
public:
	void PolyCalculateYZ(CPoint3D begin, CPoint3D end, float TuduAngle);
	void PolyDrawSArcYZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius);
	void PolyDrawNArcYZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius);
	static BOOL CalculateYZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index);
	void GLDrawArcYZ(CPoint3D& center, float start_angle, float end_angle, float delta_angle, float radius, CPtrArray& XFarray, int index);

public://时间分割法圆弧插补
	static vector<CPoint3D> ArcTDMInterpolation(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray,
		                              int index, float Inter, float F);//XOY平面Inter插补周期ms以及F进给速度mm/min

	static vector<CPoint3D> ArcTDMInterpolation(CPoint3D& center, float radio, BOOL Direction, CPoint3D ptBegin, CPoint3D ptEnd, float Inter, float F);

	static vector<CPoint3D> ArcTDMInterpolationXZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray,
		int index, float Inter, float F);

	static vector<CPoint3D> ArcTDMInterpolationYZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray,
		int index, float Inter, float F);

	static vector<CPoint3D> ArcTDMInterpolation(CPoint3D& center, float radio, CPoint3D ptBegin,CPoint3D ptEnd,
		float Inter, float F,BOOL Direction);//偏移时候可以求得起点与起点

public://计算圆弧与圆弧,圆弧与直线的交点，起始元素是圆弧
	static CPoint3D IntersectOfArc(CPoint3D& BeginCenter, float BeginRadio,
		                       CPoint3D& EndCenter, float EndRadio, int mark,CPtrArray& XFarray, int index);//圆弧与圆弧的交点
	static CPoint3D IntersectOfLine(CPoint3D& center, float radio,CPoint3D& LineBegin, CPoint3D& LineEnd, int mark, CPtrArray& XFarray, int index);//圆弧与直线的交点

};

