#pragma once
#include"OPENGL\CadBase.h"
#include"OPENGL\OpenGLDC.h"
#include<vector>
#include"Arc.h"
#include"Line.h"
using namespace std;

typedef struct tagOffsetPoint
{
	CPoint3D begin;
	CPoint3D end;
}OffsetPoint;

//typedef struct tagLinePos
//{
//	CPoint3D begin;
//	CPoint3D end;
//}LinePos;


class CRough
{
public:
	CRough();
	~CRough();
};

