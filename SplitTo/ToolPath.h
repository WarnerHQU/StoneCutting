#pragma once
/*
把每个加工类型的路径生成都写在该toolpath类中
*/
/*
应该把相应加工的路径生成写在相应的类中，在ctoolpath类中实例化相应的对象？？
*/
#include"OPENGL\CadBase.h"
#include"OPENGL\OpenGLDC.h"
#include<vector>
#include"Arc.h"
#include"Line.h"
#include"Circle.h"

using namespace std;
typedef struct tagOffsetPoint
{
	CPoint3D begin;
	CPoint3D end;
}OffsetPoint;

class CToolPath
{
public:
	CToolPath();
	~CToolPath();

	//清除这六种加工模式的初始数据
	void DeleteAllKindData();
	
	//画的函数也应该放在这里面151207
	// 多边形偏置操作

public:
	CVector3D slope(CPoint3D PtBegin, CPoint3D PtEnd);
	vector<CPoint3D> LineIntersection;//存储偏移后交点的坐标
	vector<OffsetPoint> LineOffsetPoint;//存储偏移后直线两点的坐标
	vector<OffsetPoint> ExtendLine;//存储第一层偏置伸长后的点位置
	vector<vector<OffsetPoint>> Line;//存储各层上面点的数据
	void Polygeon(CPtrArray& Array, float distance, int number, float depth);
	CPoint3D TwoLineNot(OffsetPoint One, OffsetPoint Another);

	//多边形切割加工****20160607*****
	void RenderPolygeonInit();//显示多边形加工形状示意图的初始化设置
	void RenderPolygeon(COpenGLDC* pDC);//显示多边形加工形状示意图

	vector<OffsetPoint> m_PolygeonLines;//存储各边的直线
	vector<vector<OffsetPoint>> m_OffsetPolygeonLines;//存储偏置后的直线的位置，这里不考虑锯片的厚度问题，厚度问题在延伸的时候需要考虑

	void RenderPolygeonCuttingPathInit();//多边形加工轨迹初始化
	void RenderPolygeonCuttingPath(COpenGLDC* pDC);//显示加工轨迹
	vector<vector<OffsetPoint>> m_PolygeonCuttingLines;//存储加了延长线以及锯片宽度后的锯片的中点的轨迹线
	vector<vector<OffsetPoint>> m_PolygeonSequentialLines;//存储轨迹线按照边存储的轨迹线

	vector<vector<vector<CPoint3D>>> m_tempPolygeonSimulationPts;//存储按层离散后的仿真点
    public://一些参数
		float m_EveryAngelOfSide;
		float m_PolygeonPiles;
		
   public:
	   void SetPolygeonNumber(int NUMBER)
	   {
		   this->m_NumberOfPolygeon = NUMBER;
	   }
	   int GetPolygeonNumber()
	   {
		   return this->m_NumberOfPolygeon;
	   }
	   void SetPolygeonCenter(float CenterX, float CenterY, float CenterZ)
	   {
		   this->m_PolygeonCenter.x = CenterX;
		   this->m_PolygeonCenter.y = CenterY;
		   this->m_PolygeonCenter.z = CenterZ;
	   }
	   CPoint3D GetPolygeonCenter()
	   {
		   return this->m_PolygeonCenter;
	   }
	   void SetRadiusOfPolygeon(float Radius)
	   {
		   this->m_RadiusOfPolygeon = Radius;
	   }
	   float GetRadiusOfPolygeon()
	   {
		   return this->m_RadiusOfPolygeon;
	   }
	   void SetPolygeonBottomHigh(float BottomHigh)
	   {
		   this->m_PolygeonBottomHigh = BottomHigh;
	   }
	   float GetPolygeonBottomHigh()
	   {
		   return this->m_PolygeonBottomHigh;
	   }
	   void SetPolygeonBottomAngel(float angel)
	   {
		   this->m_PolygeonBottomAngel = angel;
	   }
	   float GetPolygeonBottomAngel()
	   {
		   return this->m_PolygeonBottomAngel;
	   }


	   void SetPolygeonFeedLoad(float PolygeonFeedLoad)
	   {
		   this->m_PolygeonFeedLoad = PolygeonFeedLoad;
	   }
	   float GetPolygeonFeedLoad()
	   {
		   return this->m_PolygeonFeedLoad;
	   }
	   void SetPolygeonFeedSpeed(float PolygeonFeedSpeed)
	   {
		   this->m_PolygeonFeedSpeed = PolygeonFeedSpeed;
	   }
	   float GetPolygeonFeedSpeed()
	   {
		   return this->m_PolygeonFeedSpeed;
	   }
	   void SetPolygeonFeedTime(float PolygeonFeedTime)
	   {
		   this->m_PolygeonFeedTime = PolygeonFeedTime;
	   }
	   float GetPolygeonFeedTime()
	   {
		   return this->m_PolygeonFeedTime;
	   }

	   void SetPolygeonCutDirection(int CutDirection)
	   {
		   this->m_PolygeonCuttingDirection = CutDirection;
	   }
	   int GetPolygeonCutDirection()
	   {
		   return this->m_PolygeonCuttingDirection;
	   }
	   void SetPolygeonExtention(float Extention)
	   {
		   this->m_PolygeonExtendDistance = Extention;
	   }
	   float GetPolygeonExtention()
	   {
		   return this->m_PolygeonExtendDistance;
	   }

	   void SetPolygeonSawWidth(float SawWidth)
	   {
		   this->m_PolygeonSawWidth = SawWidth;
	   }
	   float GetPolygeonSawWidth()
	   {
		   return this->m_PolygeonSawWidth;
	   }

	   void SetPolygeonWorkBlankLength(float PolygeonWorkLong)
	   {
		   this->m_PolygeonWorkBlankLength = PolygeonWorkLong;
	   }
	   float GetPolygeonWorkBlankLength()
	   {
		   return this->m_PolygeonWorkBlankLength;
	   }
	   void SetPolygeonWorkBlankWidth(float PolygeonWorkWidth)
	   {
		   this->m_PolygeonWorkBlankWidth = PolygeonWorkWidth;
	   }
	   float GetPolygeonWorkBlankWidth()
	   {
		   return this->m_PolygeonWorkBlankWidth;
	   }
	   void SetPolygeonWorkBlankHigh(float PolygeonWorkHigh)
	   {
		   this->m_PolygeonWorkBlankHigh = PolygeonWorkHigh;
	   }
	   float GetPolygeonWorkBlankHigh()
	   {
		   return this->m_PolygeonWorkBlankHigh;
	   }

	   void SetPolygeonPolyLine(vector<OffsetPoint> PolyLine)
	   {
		   vector<OffsetPoint>().swap(m_PolygeonLines);
		   for (vector<OffsetPoint>::iterator it = PolyLine.begin(); it != PolyLine.end(); ++it)
		   {
			   OffsetPoint m_tempPolygeonLines;
			   m_tempPolygeonLines.begin = it->begin;
			   m_tempPolygeonLines.end = it->end;
			   m_PolygeonLines.push_back(m_tempPolygeonLines);
		   }
	   }
	   vector<OffsetPoint> GetPolygeonPolyLine()
	   {
		   return this->m_PolygeonLines;
	   }

	private:
		int m_NumberOfPolygeon;//记录多边形的边数
		CPoint3D m_PolygeonCenter;//记录多边形中心点的坐标
		float m_RadiusOfPolygeon;//记录多边形的外接圆半径
		float m_PolygeonBottomHigh;//记录底面的高度
		float m_PolygeonBottomAngel;//记录侧边与底面的夹角

		float m_PolygeonFeedLoad;
		float m_PolygeonFeedSpeed;
		float m_PolygeonFeedTime;

		int m_PolygeonCuttingDirection;//记录刀补的方向
		float m_PolygeonExtendDistance;//记录延伸距离

		float m_PolygeonSawWidth;//记录锯片的宽度

		float m_PolygeonWorkBlankLength;//记录多边形加工毛坯的长度
		float m_PolygeonWorkBlankWidth;//记录多边形加工毛坯的宽度
		float m_PolygeonWorkBlankHigh;//记录多边形加工的高度


	//正交切割orthgonal
public:
	int Calculate(float Long, float Gap, float PartLength);//通过输入的参数判断生成多少多少个的零件
	/*vector<vector<OffsetPoint>> HorizonPos;
	vector<vector<OffsetPoint>> VerticalPos;
	vector<vector<OffsetPoint>> HorizonPosRT;
	vector<vector<OffsetPoint>> VerticalPosRT;*/
	//梯形
	vector<vector<OffsetPoint>> Horizon(float ToolWidth, float Angle, float Depth, float Height, float Width
		, float Long, float Gap, float PartLength);//锯片沿着X轴正方向运动，在YOZ平面内计算相关坐标
	vector<vector<OffsetPoint>> Vertical(float ToolWidth, float Angle, float Depth, float Height, float Width
		, float Long, float Gap, float PartLength);//锯片沿着Y轴正方向运动，在XOZ平面内计算相关坐标
	//倒梯形
	vector<vector<OffsetPoint>> HorizonRT(float ToolWidth, float Angle, float Depth, float Height, float Width
		, float Long, float Gap, float PartLength);
	vector<vector<OffsetPoint>> VerticalRT(float ToolWidth, float Angle, float Depth, float Height, float Width
		, float Long, float Gap, float PartLength);
public:
	BOOL RenderOrthgonalInit();
	void RenderOrthgonal(COpenGLDC* pDC);//显示正交加工区域示意图
	CPoint3D GetOrthgonalWorkBlankCenter(float Length, float Width, float High);

	void RenderOrthgonalCuttingPathInit();
	void RenderOrthgonalCuttingPath(COpenGLDC* pDC);//显示加工轨迹示意图

	vector<vector<OffsetPoint>> NormalTrapezoid;//存储正梯形的所有直线
	vector<vector<OffsetPoint>> ReverseTrapezoid;//存储倒梯形的所有直线


	vector<vector<OffsetPoint>> NormalTrapezoidCut;//存储正梯形加工所有直线(顺序转换之后)
	vector<vector<OffsetPoint>> ReverseTrapezoidCut;//存储倒梯形加工所有直线

	//存储部分的精加工离散点
	vector<vector<vector<CPoint3D>>> m_TempNormalFinish;
	vector<vector<vector<CPoint3D>>> m_TempReverseFinish;

	//毛坯尺寸的设置和获取
	void SetOrthgonalWorkBlankLength(float Length)
	{
		this->m_OrthgonalWorkBlankLength = Length;
	}
	float GetOrthgonalWorkBlankLength()
	{
		return this->m_OrthgonalWorkBlankLength;
	}
	void SetOrthgonalWorkBlankWidth(float Width)
	{
		this->m_OrthgonalWorkBlankWidth = Width;
	}
	float GetOrthgonalWorkBlankWidth()
	{
		return this->m_OrthgonalWorkBlankWidth;
	}
	void SetOrthgonalWorkBlankHigh(float high)
	{
		this->m_OrthgonalWorkBlankHigh = high;
	}
	float GetOrthgonalWorkBlankHigh()
	{
		return this->m_OrthgonalWorkBlankHigh;
	}
	void SetOrthgonalTrapezoidAngle(float TrapezoidAngle)
	{
		this->m_TrapezoidAngle = TrapezoidAngle;
	}
	float GetOrthgonalTrapezoidAngle()
	{
		return this->m_TrapezoidAngle;
	}
	void SetOrthgonalSawWidth(float SawWidth)
	{
		this->m_OrthgonalSawWidth = SawWidth;
	}
	float GetOrthgonalSawWidth()
	{
		return this->m_OrthgonalSawWidth;
	}
	//梯形的形状的获取
	void SetOrthgonalTrapezoidType(int Type)
	{
		this->m_TrapezoidType = Type;
	}
	int GetOrthgonalTrapezoidType()
	{
		return this->m_TrapezoidType;
	}
	void SetOrthgonalProcessStyle(int style)
	{
		this->m_OrthgonalProcessStyle = style;
	}
	int GetOrthgonalProcessStyle()
	{
		return this->m_OrthgonalProcessStyle;
	}

	//梯形参数的设置,Y侧平面
	void SetOrthgonalYShort(float YShort)
	{
		this->m_OrthgonalYShort = YShort;
	}
	float GetOrthgonalYShort()
	{
		return this->m_OrthgonalYShort;
	}
	void SetOrthgonalYLong(float YLong)
	{
		this->m_OrthgonalYLong = YLong;
	}
	float GetOrthgonalYLong()
	{
		return this->m_OrthgonalYLong;
	}
	void SetOrthgonalYGap(float Gap)
	{
		this->m_OrthgonalYGap = Gap;
	}
	float GetOrthgonalYGap()
	{
		return this->m_OrthgonalYGap;
	}
	void SetOrthgonalYFeedLoad(float Load)
	{
		this->m_OrthgonalYFeedLoad = Load;
	}
	float GetOrthgonalYFeedLoad()
	{
		return this->m_OrthgonalYFeedLoad;
	}

	//梯形参数的设置,X侧平面
	void SetOrthgonalXShort(float XShort)
	{
		this->m_OrthgonalXShort = XShort;
	}
	float GetOrthgonalXShort()
	{
		return this->m_OrthgonalXShort;
	}
	void SetOrthgonalXLong(float XLong)
	{
		this->m_OrthgonalXLong = XLong;
	}
	float GetOrthgonalXLong()
	{
		return this->m_OrthgonalXLong;
	}
	void SetOrthgonalXGap(float Gap)
	{
		this->m_OrthgonalXGap = Gap;
	}
	float GetOrthgonalXGap()
	{
		return this->m_OrthgonalXGap;
	}
	void SetOrthgonalXFeedLoad(float Load)
	{
		this->m_OrthgonalXFeedLoad = Load;
	}
	float GetOrthgonalXFeedLoad()
	{
		return this->m_OrthgonalXFeedLoad;
	}

private:
	
private:
	float m_OrthgonalWorkBlankLength;
	float m_OrthgonalWorkBlankWidth;
	float m_OrthgonalWorkBlankHigh;
	float  m_TrapezoidAngle;
	float m_OrthgonalSawWidth;

	int m_TrapezoidType;//梯形类型
	int m_OrthgonalProcessStyle;//加工模式

	float m_OrthgonalYShort;
	float m_OrthgonalYLong;
	float m_OrthgonalYGap;
	float m_OrthgonalYFeedLoad;

	float m_OrthgonalXShort;
	float m_OrthgonalXLong;
	float m_OrthgonalXGap;
	float m_OrthgonalXFeedLoad;

	//线条加工 LineProcess
	//插补算法实现离散化，精度还不是很够151207
public:
	vector<CPoint3D> LineDotPos;//存储直线的离散点
	vector<CPoint3D> ArcDotPos;//存储圆弧的离散点
	vector<CPoint3D> CircleDotPos;
	vector<LinePos> Opposite;//存储对应的两点

	vector<CPoint3D> LineDiscrete(CPoint3D m_Begin, CPoint3D m_End, float Inter, float F);//直线插补离散，调用CLine中的函数
	vector<CPoint3D> ArcDiscrete(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F);//普通圆弧插补离散，调用CArc里面的函数
	vector<CPoint3D> PolyArcDiscrete(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F);//多线段圆弧插补离散,不好调用故直接写个函数，截取CArcl里的部分代码
	vector<CPoint3D> CircleDiscrete(CPoint3D center, float radius, CPoint3D EnterPoint, float Inter, float F);
	vector<LinePos> Reverse(vector<CPoint3D> start, double width);//求出加了板厚之后点的位置
private:
	CPoint3D m_center;
	float m_radio;
	float m_begin_angle;
	float m_end_angle;
public:
	void ChangeToXOZ(CPtrArray& Array);//转换到XOZ平面
	void ChangeToYOZ(CPtrArray& Array);//转换到YOZ平面
	void LineRenerInit();
	void LineRender(COpenGLDC* pDC);

	//直线线条粗加工显示及初始化
	void LineRoughRenderInit();
	void LineRoughRender(COpenGLDC* pDC);

	//直线线条粗加工余量显示
	void LineRoughLeftRenderInit();
	void LineRoughLeftRender(COpenGLDC* pDC);

	//毛坯线框中心点
	CPoint3D GetLineProcessWorkBlankCenter();
	BOOL BeInXZLineWorkBlank(CPoint3D point,  CPoint3D center,float Width, float High);

	//直线线条精加工轨迹的生成及初始化，利用设定的初加工余量，与上面粗加工的生成是一样的，
	//只不过这时候侧面和垂直面插补的参数要用精加工设定的值
	void LineFinishRenderInit();
	void LineFinishRender(COpenGLDC* pDC);

public://存放点用来显示
	vector<LinePos> m_LineProcess;//存放直线线条的首尾点的集合***L
	vector<vector<CPoint3D>> m_LineArcProcessPts;//存储圆弧形线条的所有点的位置
	vector<CPtrArray*> m_LineArrayArcList;//记录圆弧线条XoZ平面离散后每个点所对应的圆弧集合

	CPtrArray m_LineRoughLeft;//存储直线线条加工粗加工余量的截面线信息
	vector<CPoint3D> m_LineRoughLeftPts;//存储直线线条粗加工余量离散后的点集合
	vector<CPtrArray*> m_LineRoughLeftArc;//存储粗加工余量离散点为最右点的圆弧的集合
	vector<vector<CPoint3D>> m_LineRoughLeftArcDisPts;//粗加工余量圆弧离散点
	vector<LinePos> m_LineRoughLeftLines;//存储直线线条粗加工余量的线的集合

	vector<CPoint3D> m_LineRoughPts;
	vector<LinePos> m_LineRoughVSegment;//存储毛坯到粗加工余量的垂直直线的首尾点，从上往下
	vector<vector<CPoint3D>> m_LineRoughVSegmentDisPts;//存储线段离散后的点
	vector<vector<LinePos>> m_LineRoughVSegmentDisLine;//将离散后的点进行Y向延伸并连接成直线****L
	vector<vector<CPtrArray*>> m_LineRoughVSegmentDisArc;//将离散后的点按照设置的值生成圆弧并保存***A
	vector<vector<vector<CPoint3D>>> m_LineRoughVSegmentDisArcDisPts;//圆弧离散后生成的点

	vector<LinePos> m_LineFinishVSegment;//存储粗加工余量到加工零件表面的垂直线段的首尾点，从上往下
	vector<vector<CPoint3D>> m_LineFinishVSegmentDisPts;//存储垂直线段离散后的点
	vector<vector<LinePos>> m_LineFinishVSegmentDisLine;//存储离散后的点，并将该点与Y方向延伸后的点进行连接成直线***L
	vector<CPtrArray*> m_LineFinishVSegmentDisArc;//存储按照离散点的位置生成的圆弧并保存****A
	vector<vector<CPoint3D>> m_LineFinishVSegmentDisArcDisPts;//存储圆弧离散后的点

	vector<CPoint3D> m_LineArcSurfacePts;//存储圆弧形线条零件表面按照精加工参数离散后的点

public:
	//基本数据的获取和设置
	void SetLineArray(CPtrArray& Array)
	{
		this->m_LineArray.Copy(Array);
	}
	CPtrArray& GetLineArray()
	{
		return this->m_LineArray;
	}
	//粗加工数据的获取和设置
	void SetLineRoughHFeedSpeed(float speed)
	{
		this->m_LineRough_H_FeedSpeed = speed;
	}
	float GetLineRoughHFeedSpeed()
	{
		return this->m_LineRough_H_FeedSpeed;
	}
	void SetLineRoughHFeedTime(float time)
	{
		this->m_LineRough_H_FeedTime = time;
	}
	float GetLineRoughHFeedTime()
	{
		return this->m_LineRough_H_FeedTime;
	}
	void SetLineRoughVFeedSpeed(float speed)
	{
		this->m_LineRough_V_FeedSpeed = speed;
	}
	float GetLineRoughVFeedSpeed()
	{
		return this->m_LineRough_V_FeedSpeed;
	}
	void SetLineRoughVFeedTime(float time)
	{
		this->m_LineRough_V_FeedTime = time;
	}
	float GetLineRoughVFeedTime()
	{
		return this->m_LineRough_V_FeedTime;
	}
	void SetLineRoughVerticalDistance(float distance)
	{
		this->m_LineRough_VerticalDistance = distance;
	}
	float GetLineRoughVerticalDistance()
	{
		return this->m_LineRough_VerticalDistance;
	}
	void SetLineRoughPiles(float piles)
	{
		this->m_LineRough_Piles = piles;
	}
	float GetLineRoughPiles()
	{
		return this->m_LineRough_Piles;
	}
	void SetLineRoughLeft(float left)
	{
		this->m_LineRough_Left = left;
	}
	float GetLineRoughLeft()
	{
		return this->m_LineRough_Left;
	}

	//精加工数据的获取及设置
	void SetLineFinishHFeedSpeed(float speed)
	{
		this->m_LineFinish_H_FeedSpeed = speed;
	}
	float GetLineFinishHFeedSpeed()
	{
		return this->m_LineFinish_H_FeedSpeed;
	}
	void SetLineFinishHFeedTime(float time)
	{
		this->m_LineFinish_H_FeedTime = time;
	}
	float GetLineFinishHFeedTime()
	{
		return this->m_LineFinish_H_FeedTime;
	}
	void SetLineFinishVFeedSpeed(float speed)
	{
		this->m_LineFinish_V_FeedSpeed = speed;
	}
	float GetLineFinishVFeedSpeed()
	{
		return this->m_LineFinish_V_FeedSpeed;
	}
	void SetLineFinishVFeedTime(float time)
	{
		this->m_LineFinish_V_FeedTime = time;
	}
	float GetLineFinishVFeedTime()
	{
		return this->m_LineFinish_V_FeedTime;
	}
	void SetLineFinishVerticalDistance(float distance)
	{
		this->m_LineFinish_VerticalDistance = distance;
	}
	float GetLineFinishVerticalDistance()
	{
		return this->m_LineFinish_VerticalDistance;
	}
	void SetLineFinishPiles(float piles)
	{
		this->m_LineFinish_Piles = piles;
	}
	float GetLineFinishPiles()
	{
		return this->m_LineFinish_Piles;
	}

	//石材线条类型
	void SetLineStyle(BOOL style)
	{
		this->m_LineStyle = style;
	}
	BOOL GetLineStyle()
	{
		return this->m_LineStyle;
	}
	//弧形线条形状
	void SetLineArcShapeArray(CPtrArray& Array)
	{
		this->m_LineOfArc.Copy(Array);
	}
	CPtrArray& GetLineArcShapeArray()
	{
		return this->m_LineOfArc;
	}

	//毛坯尺寸获取与设置
	void SetLineWorkBlankLength(float length)
	{
		this->m_LineWorkBlank_Length = length;
	}
	float GetLineWorkBlankLength()
	{
		return this->m_LineWorkBlank_Length;
	}
	void SetLineWorkBlankWidth(float width)
	{
		this->m_LineWorkBlank_Width = width;
	}
	float GetLineWorkBlankWidth()
	{
		return this->m_LineWorkBlank_Width;
	}
	void SetLineWorkBlankHigh(float high)
	{
		this->m_LineWorkBlank_High = high;
	}
	float GetLineWorkBlankHigh()
	{
		return this->m_LineWorkBlank_High;
	}
private:
	//基本数据
	CPtrArray m_LineArray;
	//粗加工数据提取
	float m_LineRough_H_FeedSpeed;
	float m_LineRough_H_FeedTime;
	float m_LineRough_V_FeedSpeed;
	float m_LineRough_V_FeedTime;
	float m_LineRough_VerticalDistance;
	float m_LineRough_Piles;
	float m_LineRough_Left;

	//精加工数据提取
	float m_LineFinish_H_FeedSpeed;
	float m_LineFinish_H_FeedTime;
	float m_LineFinish_V_FeedSpeed;
	float m_LineFinish_V_FeedTime;
	float m_LineFinish_VerticalDistance;
	float m_LineFinish_Piles;

	//线条类型
	BOOL m_LineStyle;//FALSE表示直线，TRUE表示圆弧

	//弧形线条的数据
	CPtrArray m_LineOfArc;

	//毛坯尺寸获取
	float m_LineWorkBlank_Length;
	float m_LineWorkBlank_Width;
	float m_LineWorkBlank_High;


	//旋转面加工Rotate Process.*******这边指定的下刀点位X方向上最大的点位切入点2016.1.5
public:
	vector<CPoint3D> LineDiscreteXZ(CPoint3D m_Begin, CPoint3D m_End, float Inter, float F);
	vector<CPoint3D> ArcDiscreteXZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F);
	vector<CPoint3D> PolyArcDiscreteXZ(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F);

	BOOL InitRotateRender();
	void RenderRotate(COpenGLDC* pDC);//绘图：工件示意图
	BOOL InitRotateRough();
	void RenderRoughRotate(COpenGLDC* pDC);//绘制粗加工路径
	BOOL InitRotateWorkBlank();
	void RenderRotateWorkBlank(COpenGLDC* pDC);//绘制毛坯示意图
	string BeInRotateWorkBlank(CPoint3D pt);
	BOOL InitRotateRoughCut();
	void RenderRoughCutRotate(COpenGLDC* pDC);//绘制裁剪完的粗加工路径
	BOOL InitRotateFinish();
	void RenderFinishRotate(COpenGLDC* pDC);//绘制精加工路径
public:
	void SetRotateArray(CPtrArray& Array)
	{
		this->m_Rotate_Array.Copy(Array);
	}
	CPtrArray& GetRotateArray()
	{
		return this->m_Rotate_Array;
	}
	//粗加工水平
	void SetRotateFeedSpeed(float speed)
	{
		this->m_Rotate_FeedSpeed = speed;
	}
	float GetRotateFeedSpeed()
	{
		return this->m_Rotate_FeedSpeed;
	}
	void SetRotateFeedTime(float Time)
	{
		this->m_Rotate_FeedTime = Time;
	}
	float GetRotateFeedTime()
	{
		return this->m_Rotate_FeedTime;
	}
	//粗加工垂直
	void SetRotateFeedSpeedVx(float speed)
	{
		this->m_Rotate_FeedSpeed_VX = speed;
	}
	float GetRotateFeedSpeedVx()
	{
		return this->m_Rotate_FeedSpeed_VX;
	}
	void SetRotateFeedTimeVx(float Time)
	{
		this->m_Rotate_FeedTime_VX = Time;
	}
	float GetRotateFeedTimeVx()
	{
		return this->m_Rotate_FeedTime_VX;
	}
	//精加工水平
	void SetRotateFeedSpeedFinish(float speed)
	{
		this->m_Rotate_FeedSpeed_Finish = speed;
	}
	float GetRotateFeedSpeedFinsh()
	{
		return this->m_Rotate_FeedSpeed_Finish;
	}
	void SetRotateFeedTimeFinish(float Time)
	{
		this->m_Rotate_FeedTime_Finish = Time;
	}
	float GetRotateFeedTimeFinish()
	{
		return this->m_Rotate_FeedTime_Finish;
	}
	
	//精加工垂直
	void SetRotateFeedSpeedFinishVX(float speed)
	{
		this->m_Rotate_FeedSpeed_VX_Finish = speed;
	}
	float GetRotateFeedSpeedFinshVX()
	{
		return this->m_Rotate_FeedSpeed_VX_Finish;
	}
	void SetRotateFeedTimeFinishVX(float Time)
	{
		this->m_Rotate_FeedTime_VX_Finish = Time;
	}
	float GetRotateFeedTimeFinishVX()
	{
		return this->m_Rotate_FeedTime_VX_Finish;
	}
	//毛坯尺寸的获取与设置
	void SetRotateBlankLongth(float longth)
	{
		this->m_Rotate_WorkBlank_Longth = longth;
	}
	float GetRotateBlankLongth()
	{
		return this->m_Rotate_WorkBlank_Longth;
	}
	void SetRotateBlankWidth(float width)
	{
		this->m_Rotate_WorkBlank_Width = width;
	}
	float GetRotateBlankWidth()
	{
		return this->m_Rotate_WorkBlank_Width;
	}
	void SetRotateBlankHeighth(float higth)
	{
		this->m_Rotate_WorkBlank_Heighth = higth;
	}
	float GetRotateBlankHeighth()
	{
		return this->m_Rotate_WorkBlank_Heighth;
	}
	//获取粗加工切深及粗加工层数
	void SetRotateRoughPlies(float plies)
	{
		this->m_Rotate_RoughPlies = plies;
	}
	float GetRotateRoughPlies()
	{
		return this->m_Rotate_RoughPlies;
	}
	void SetRotateRoughCutOfDepth(float depth)
	{
		this->m_Rotate_RoughCutOfDepth = depth;
	}
	float GetRotateRoughCutOfDepth()
	{
		return this->m_Rotate_RoughCutOfDepth;
	}
	//获取粗加工余量及精加工切深
	void SetRotateRoughLeft(float left)
	{
		this->m_Rotate_RoughLeft = left;
	}
	float GetRotateRoughLeft()
	{
		return this->m_Rotate_RoughLeft;
	}
	void SetRotateFinishCutOfDepth(float depth)
	{
		this->m_Rotate_FinishCutOfDepth = depth;
	}
	float GetRotateFinishCutOfDepth()
	{
		return this->m_Rotate_FinishCutOfDepth;
	}
	//获取粗加工的所有点
	vector<vector<vector<CPoint3D>>> GetRotateRoughCutPoints()
	{
		return this->m_RotateRoughCutPoints;
	}
	vector<vector<vector<CPoint3D>>> GetRotateRoughPoints()
	{
		return this->m_RotateRoughPoints;
	}
	//获取精加工的所有点
	vector<vector<vector<CPoint3D>>> GetRotateFinishPoints()
	{
		return this->m_RotateFinishPoints;
	}

	CPoint3D m_RotateRoughWorkBlankCenter;//存储旋转加工毛坯的中心点

private:
	CPtrArray m_Rotate_Array;//用于存储对话框中提取的数据
	//粗加工时候的参数设置
	float m_Rotate_FeedSpeed;
	float m_Rotate_FeedTime;
	float m_Rotate_FeedSpeed_VX;
	float m_Rotate_FeedTime_VX;
	//精加工时候的参数设置
	float m_Rotate_FeedSpeed_Finish;
	float m_Rotate_FeedTime_Finish;
	float m_Rotate_FeedSpeed_VX_Finish;
	float m_Rotate_FeedTime_VX_Finish;
	//毛坯尺寸的设置
	float m_Rotate_WorkBlank_Longth;
	float m_Rotate_WorkBlank_Width;
	float m_Rotate_WorkBlank_Heighth;
	//设置粗加工加工层数及粗加工余量
	float m_Rotate_RoughLeft;
	float m_Rotate_RoughPlies;
	float m_Rotate_RoughCutOfDepth;//粗加工切深
	float m_Rotate_FinishCutOfDepth;//精加工切深
	//存放每个高度圆的圆心坐标
	CPoint3D m_RotateCenter;

	//工件示意图的所有点
	vector<vector<CPoint3D>> RotateWorkPiecePoints;

	//粗加工轨迹分层的存储及粗加工和粗加工裁剪点的存储
	vector<CPtrArray*> m_RotateRoughArrayList;
	vector<vector<vector<CPoint3D>>> m_RotateRoughPoints;
	vector<vector<vector<CPoint3D>>> m_RotateRoughCutPoints;

	//存储旋转加工精加工所有点
	vector<CPtrArray*> m_RotateFinsihArrayList;
	vector<vector<vector<CPoint3D>>> m_RotateFinishPoints;

	
	
	//外侧面加工 Side Process
public:	
	int mark;//用于存储封闭图形有几个元素
	CPoint3D XMax;//用于存储X坐标方向上值最大的点的坐标，一般为起点
	void ChangeSideProcess();
	vector<CPoint3D> OffsetAlgorithm(CPtrArray& XFarray, int mark, double offset);
	BOOL RenderSideInit();//将计算放在里面进行
	void RenderSide(COpenGLDC* pDC);//绘制加工形状
	CPoint3D GetCenterPoint();
	string BeInBox(CPoint3D pt);//用来判断是否在所设定的毛坯的尺寸范围内
	
	void RoughInit();
	void RenderSideRough(COpenGLDC* pDC);//绘制粗加工的刀尖轨迹
	void DeletePointOutOfBox();//***通过对矩形区域进行分区，使得在其外面的部分按照沿着矩形边界走直线的思路进行，不妥的是对于左上、右上、左下、右下这几个区域他们走的是共同的点就是尖点****
	void RenderSideRoughCut(COpenGLDC* pDC);//绘制经过选择在毛坯之内的粗加工路径
	
	void InitSideRoughLeft();//初始化数据
	void RenderSideRoughLeft(COpenGLDC* pDC);//绘制粗加工余量的示意图

	vector<CPoint3D> FinishOffsetAlgorithm(CPtrArray& XFarray, int mark, double offset);
	void InitSideFinish();//初始化数据
	void RenderSideFinish(COpenGLDC* pDC);//绘制精加工路径

public:
	void SetArray(CPtrArray& Array)
	{
		this->m_ptArray.Copy(Array);	
	}
	CPtrArray& GetArray( )
	{
		return this->m_ptArray;
	}
	//返回外侧面加工粗加工的轮廓集合
	vector<CPtrArray*> GetArrayRoughList()
	{
		return this->NewArrayList;
	}
	vector<CPtrArray*> GetArrayFinishList()
	{
		return this->FinishArrayList;
	}
	void SetTime(float time)
	{
		this->m_FeedTime = time;
	}
	float GetTime()
	{
		return this->m_FeedTime;
	}
	void SetFeedspeed(float speed)
	{
		this->m_FeedSpeed = speed;
	}
	float GetFeedspeed()
	{
		return this->m_FeedSpeed;
	}
	void SetTimeVx(float time)
	{
		this->m_FeedTime_VX = time;
	}
	float GetTimeVx()
	{
		return this->m_FeedTime_VX;
	}
	void SetSpeedVx(float speed)
	{
		this->m_FeedSpeed_VX = speed;
	}
	float GetSpeedVx()
	{
		return this->m_FeedSpeed_VX;
	}
	void SetSpeedFinish(float speed)
	{
		this->m_FeedSpeed_Finish = speed;
	}
	float GetSpeedFinish()
	{
		return m_FeedSpeed_Finish;
	}
	void SetTimeFinish(float time)
	{
		this->m_FeedTime_Finish=time;
	}
	float GetTimeFinish()
	{
		return this->m_FeedTime_Finish;
	}
	void SetSpeedFinishVX(float speed)
	{
		this->m_FeedSpeed_VX_Finish = speed;
	}
	float GetSpeedFinishVX()
	{
		return this->m_FeedSpeed_VX_Finish;
	}
	void SetTimeFinishVX(float time)
	{
		this->m_FeedTime_VX_Finish = time;
	}
	float GetTimeFinishVX()
	{
		return this->m_FeedTime_VX_Finish;
	}
	
	void SetLongth(float Long)
	{
		this->m_Piece_Long = Long;
	}
	float GetLongth()
	{
		return this->m_Piece_Long;
	}
	void SetWidth(float width)
	{
		this->m_Piece_Width = width;
	}
	float GetWidth()
	{
		return this->m_Piece_Width;
	}
	void SetHeigth(float heigth)
	{
		this->m_Piece_Heigth = heigth;
	}
	float GetHeigth()
	{
		return this->m_Piece_Heigth;
	}
	
	void SetDepthCut(float depth)
	{
		this->m_DepthOfCut = depth;
	}
	float GetDepthCut()
	{
		return this->m_DepthOfCut;
	}
	void SetDepthCutFinish(float depth)
	{
		this->m_DepthOfCut_Finish=depth;
	}
	float GetDepthCutFinish()
	{
		return this->m_DepthOfCut_Finish;
	}
	void SetRoughLeft(float rough)
	{
		this->m_RoughLeft = rough;
	}
	float GetRoughLeft()
	{
		return this->m_RoughLeft;
	}
	void SetRoughPlies(int plies)
	{
		this->m_Rough_Plies = plies;
	}
	int GetRoughPlies()
	{
		return this->m_Rough_Plies;
	}

	CPoint3D GetCenter()
	{
		return m_RectCenter;
	}
	vector<vector<vector<CPoint3D>>> GetRoughPoints()
	{
		return this->ArrayPoint;
	}
	vector<vector<vector<CPoint3D>>> GetRoughCutPoints()
	{
		return this->ADArrayPoint;
	}
	vector<vector<vector<CPoint3D>>> GetFinishPoints()
	{
		return this->FinishPoints;
	}
private:
	CPtrArray m_ptArray;
	CVector3D m_Translation;//用于存储XY平面上X最大值点与XZ最低值点的相对位置
	float m_FeedSpeed;
	float m_FeedTime;
	float m_FeedSpeed_VX;
	float m_FeedTime_VX;
	float m_FeedSpeed_Finish;//精加工水平面进给速度
	float m_FeedTime_Finish;//精加工水平面插补周期
	float m_FeedSpeed_VX_Finish;//精加工垂直面进给速度
	float m_FeedTime_VX_Finish;//精加工垂直面插补周期
	float m_Piece_Long;
	float m_Piece_Width;
	float m_Piece_Heigth;
	float m_DepthOfCut;//存储粗加工每次的水平进给量
	float m_DepthOfCut_Finish;//存储精加工每次的水平进给量
	float m_RoughLeft;//存储粗加工余量
	CPoint3D m_RectCenter;
	int m_Rough_Plies;//存放粗加工层数

	//对于CPtrArray这种存放指针的容器，用复制对其进行复制，可以更改指针所指向对象的值
	
	vector<vector<CPoint3D>> SidePoint;//存储读取的准确信息的点
	
	vector<CPtrArray*> NewArrayList;//用于存储粗加工侧面轨迹每层的轮廓点位的指针

	vector<vector<vector<CPoint3D>>> ArrayPoint;//用来存储加工轨迹每层的点位vector<CPoint3D>存储每一个高度上的离散点，
	                                            //vector<vector<CPoint3D>>存储的是一个偏移的侧面轮廓上面的所有点
	                                            //vector<vector<vector<CPoint3D>>>存储的是所有偏移距离的侧面轮廓上的所有点
	vector<vector<vector<CPoint3D>>> ADArrayPoint;//存储ArrayPoint进行判断后的所有点

	vector<vector<CPoint3D>> RoughLeftPoints;//存储粗加工余量上的点

	vector<CPtrArray*> FinishArrayList;//存储精加工侧面每层的轮廓点位的指针
	vector<vector<vector<CPoint3D>>> FinishPoints;//存储精加工的点

	public://公用的仿真点
	vector<CPoint3D> RoughSimulationPoints;//存储粗加工的仿真点再粗加工初始化函数中进行处理，并加上了抬刀点
	vector<CPoint3D> FinishSimulationPoints;//存储精加工的仿真点，加上抬刀点
	


//曲面加工Surface Process
public:
	vector<CPoint3D> ArcDiscreteYZ(CPoint3D& center, float radio, float beginAngle, float endAngle, CPtrArray& XFarray, int index, float Inter, float F);
	vector<CPoint3D> PolyArcDiscreteYZ(CPoint3D begin, CPoint3D end, float TuduAngle, float Inter, float F);
public:
	void RenderArcInit();
	void RenderArc(COpenGLDC* pDC);//绘制双截面零件示意图
	
	vector<vector<CPoint3D>> ArcDiscrete(CPtrArray& m_Array);
	vector<vector<CPoint3D>> ArcFinishDiscrete(CPtrArray& m_Array);
	void RenderArcSection(CPtrArray& m_Array, COpenGLDC* pDC);
	void RenderRoughArcInit();
	void RenderRoughArc(COpenGLDC* pDC);//绘制双截面圆弧粗加工偏置轨迹

	CPoint3D GetArcWorkBlankCenter(float higth);//获取毛坯矩形的中心点

	//粗加工轨迹截断,设置加工层数的时候尽量只让超出一层，减少加工时间
	void RenderArcRoughCutInit();//将Y侧面轮廓线沿着X侧面轮廓线提取出来存储到vector<CArc*>中，然后提取出来跟直线比较有交点，求取交点，圆弧变为两个小圆弧和一段线段
	void RenderArcRoughCut(COpenGLDC* pDC);

	//粗加工余量
	void RenderArcRoughLeftInit();
	void RenderArcRoughLeft(COpenGLDC* pDC);
	//精加工
	void RenderArcFinishInit();
	void RenderArcFinish(COpenGLDC* pDC);
	
	//设置仿真点
	vector<CPoint3D> SetArcRoughSimulationPoints(vector<vector<vector<CPoint3D>>> ArcRoughPoints);
	vector<CPoint3D> SetArcFinishSimulationPoints(vector<vector<vector<CPoint3D>>> ArcFinishPoints);

	public://存储点的数据
		vector<vector<CPoint3D>> m_ArcPiecePoints;//存储双截面圆弧示意图点
		vector<CPtrArray*> m_ArcRoughArrayList;
		
		vector<vector<vector<CPoint3D>>> m_ArcRoughPoints;//存储粗加工离散点

		vector<vector<CPtrArray*>> m_ArcsYSide;//存储Y侧圆弧的集合
		vector<vector<vector<CPoint3D>>> m_ArcRoughCutPoints;//存储粗加工裁剪后的点

		CPtrArray m_ArcRoughLeftArray;
		vector<vector<CPoint3D>> m_ArcRoughLeftPoints;//存储粗加工余量离散点

		vector<CPtrArray*> m_ArcFinishArrayList;
		vector<vector<CPtrArray*>> m_ArcFinishYSide;//存储精加工Y侧圆弧的集合
		vector<vector<vector<CPoint3D>>> m_ArcFinishPoints;//存储精精加工离散点

	public:
		void SetArcArray(CPtrArray& Array)
		{
			this->m_ArcArray.Copy(Array);
		}
		CPtrArray& GetArcArray()
		{
			return this->m_ArcArray;
		}
		void SetArcWorkBlank(float length, float width, float heigth)
		{
			this->m_Arc_WorkBlankLong = length;
			this->m_Arc_WorkBlankWidth = width;
			this->m_Arc_WorkBlankHeigth = heigth;
		}
		float GetArcWorkBlankLongth()
		{
			return this->m_Arc_WorkBlankLong;
		}
		float GetArcWorkBlankWidth()
		{
			return this->m_Arc_WorkBlankWidth;
		}
		float GetArcWorkBlankHeigth()
		{
			return this->m_Arc_WorkBlankHeigth;
		}
		//粗加工参数
		void SetYArcRoughFeedSpeed(float speed)
		{
			this->m_Rough_YArcFeedSpeed = speed;
		}
		float GetYArcRoughFeedSpeed()
		{
			return this->m_Rough_YArcFeedSpeed;
		}
		void SetYArcRoughFeedTime(float time)
		{
			this->m_Rough_YArcFeedTime = time;
		}
		float GetYArcRoughFeedTime()
		{
			return this->m_Rough_YArcFeedTime;
		}
		void SetXArcRoughFeedSpeed(float speed)
		{
			this->m_Rough_XArcFeedSpeed = speed;
		}
		float GetXArcRoughFeedSpeed()
		{
			return this->m_Rough_XArcFeedSpeed;
		}
		void SetXArcRoughFeedTime(float time)
		{
			this->m_Rough_XArcFeedTime = time;
		}
		float GetXArcRoughFeedTime()
		{
			return this->m_Rough_XArcFeedTime;
		}
		void SetArcRoughVerticalDistance(float distance)
		{
			this->m_Rough_VerticalDistance = distance;
		}
		float GetArcRoughVerticalDistance()
		{
			return this->m_Rough_VerticalDistance;
		}
		void SetArcRoughPiles(float piles)
		{
			this->m_ArcRough_NumberOfPiles = piles;
		}
		float GetArcRoughPiles()
		{
			return m_ArcRough_NumberOfPiles;
		}
		void SetArcRoughLeft(float left)
		{
			this->m_ArcRough_Left = left;
		}
		float GetArcRoughLeft()
		{
			return this->m_ArcRough_Left;
		}

		//精加工参数
		void SetYArcFinishFeedSpeed(float speed)
		{
			this->m_Finish_YArcFeedSpeed = speed;
		}
		float GetYArcFinishFeedSpeed()
		{
			return this->m_Finish_YArcFeedSpeed;
		}
		void SetYArcFinishFeedTime(float time)
		{
			this->m_Finish_YArcFeedTime = time;
		}
		float GetYArcFinishFeedTime()
		{
			return this->m_Finish_YArcFeedTime;
		}

		void SetXArcFinishFeedSpeed(float speed)
		{
			this->m_Finish_XArcFeedSpeed=speed;
		}
		float GetXArcFinishFeedSpeed()
		{
			return m_Finish_XArcFeedSpeed;
		}
		void SetXArcFinishFeedTime(float time)
		{
			this->m_Finish_XArcFeedTime = time;
		}
		float GetXArcFinishFeedTime()
		{
			return this->m_Finish_XArcFeedTime;
		}
		void SetArcFinishVerticalDistance(float distance)
		{
			this->m_Finish_VerticalDistance = distance;
		}
		float GetArcFinishVerticalDistance()
		{
			return this->m_Finish_VerticalDistance;
		}
		void SetArcFinishPiles(float piles)
		{
			this->m_ArcFinish_NumberOfPiles = piles;
		}
		float GetArcFinishPiles()
		{
			return this->m_ArcFinish_NumberOfPiles;
		}
	private:
		//基本数据的获取
		CPtrArray m_ArcArray;
		//毛坯设置参数
		float m_Arc_WorkBlankLong;
		float m_Arc_WorkBlankWidth;
		float m_Arc_WorkBlankHeigth;
		//粗加工参数
		float m_Rough_YArcFeedSpeed;//Y侧圆弧
		float m_Rough_YArcFeedTime;
		float m_Rough_XArcFeedSpeed;//X侧圆弧
		float m_Rough_XArcFeedTime;
		float m_Rough_VerticalDistance;//垂直下刀距离
		float m_ArcRough_NumberOfPiles;//粗加工层数
		float m_ArcRough_Left;//粗加工余量

		//精加工参数
		float m_Finish_YArcFeedSpeed;//Y侧圆弧
		float m_Finish_YArcFeedTime;
		float m_Finish_XArcFeedSpeed;//X侧圆弧
		float m_Finish_XArcFeedTime;
		float m_Finish_VerticalDistance;//垂直下刀距离
		float m_ArcFinish_NumberOfPiles;//精加工层数

};

