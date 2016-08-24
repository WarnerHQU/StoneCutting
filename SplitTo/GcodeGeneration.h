#pragma once
#include"SplitToDoc.h"

//这个类用来对前置处理所产生的刀位点信息进行处理，即后置处理
//使其生成NC代码
//传统的要通过生成刀位文件再生成，及刀位点和刀轴矢量进行生成，而且刀位文件为APT语言，这里不想生成到位文件
//直接就生成NC代码，并且将NC代码进行保存输出并且显示在CSplitToView面板上的List控件中，使得其能够随着定时器进行实时的显示

/*思路就是对于各种加工类型进行判断（侧面/旋转面/圆弧/线条）进行判断，根据不同的加工类型进行相应的
  NC代码的生成，都是各个高度上面顺着底面的形状进行加工，进而生成相应的G代码
*/
typedef struct tagsort
{
	CPoint3D begin;
	CPoint3D end;
	int flag;//记录类型
	CPoint3D center = CPoint3D(0, 0, 0);
}sort;
typedef struct tagTyeSort
{
	CPoint3D position;
	int flag;//记录类型
	CPoint3D center; //存储圆心坐标
}TypeSort;
typedef struct tagWorkBlankRect
{
	CPoint3D LeftTop;
	CPoint3D LeftBottom;
	CPoint3D RightTop;
	CPoint3D RightBottom;
}WorkBlankRect;
class CGcodeGeneration
{
public:
	//CGcodeGeneration(CPtrArray& Array);
	CGcodeGeneration(CToolPath* tool);
	~CGcodeGeneration();
public:
	void Init();//初始化
	void OutPutNcCode();//输出并保存起来
	void ShowNcCode();//显示NC代码
	
	//不同模块NC的生成
	//旋转面加工NC生成
	void RotateNC(ofstream* of);//旋转加工粗加工NC代码生成
	CPtrArray* RotateCalculate(CPoint3D circlecenter,float circleradius,float workBlankLong,float workBlankWidth);//计算把圆分成几段圆弧与直线
	
	//侧面加工NC生成
	BOOL SideNCInit();//初始化相关计算
	BOOL JugeInSide(CPoint3D& pos, CPoint3D center, float workBlankLong, float workBlankWidth);
	CPtrArray* SideNCOffsetAlgorithm(CPtrArray& XFarray, int mark, double offset);//偏置算法
	void SideNC(ofstream* of);//侧面加工NC代码生成
	CPtrArray* SideCalculate(vector<TypeSort> pts);
	vector<vector<vector<TypeSort>>> m_SideRough;//存储侧面加工未裁剪离散后后的点的集合，带标记直线1圆弧2，圆带圆心符号
	vector<vector<CPtrArray*>> m_SideNCFinish;

	//圆弧截面加工NC生成
	BOOL ArcNCInit();//初始化
	void ArcNC(ofstream* of);//代码的输出
	vector<sort> ArcNCCalculate(CPtrArray& Array, float High);//计算圆弧与水平线的交点问题，有则两段圆弧一段线段，无责一段圆弧
	vector<vector<vector<sort>>> m_ArcRough;//存储双截面粗加工裁剪后的各段的标记，直线1圆弧2
	vector<vector<vector<sort>>> m_ArcFinish;//存储双截面精加工各段的标记，直线1圆弧2

	//线条加工NC生成
	BOOL LineNCInit();
	void LineNC(ofstream* of);
	vector<vector<sort>> LineRoughArc;//存储粗加工圆弧段
	vector<sort> LineFinishArc;//存储精加工圆弧段

	//正交加工NC生成
	BOOL OrthgonalNCInit();
	void OrthgonalNC(ofstream* of);


	//多边形加工NC生成
	BOOL PolygonNCInit();
	void PolygonNC(ofstream* of);

	//台面板加工
	void TableBoardNC(ofstream* of);
	void TableBoardOutNC(vector<vector<vector<elements>>> data, ofstream* of);
	void TableBoardInSawNC(vector<vector<vector<vector<elements>>>> data, ofstream* of);
	void TableBoardInMillNC(vector<vector<vector<vector<elements>>>> data, ofstream* of);

public:
	void Input_G00(float posx, float posy, float posz, ofstream* of);
	void Input_G01(float posx, float posy, float posz, ofstream* of);
	void Input_G02(float posx, float posy, float posz, float posi, float posj, ofstream* of);
	void Input_G03(float posx, float posy, float posz, float posi, float posj, ofstream* of);

private:
	//CPtrArray m_CodeArray;//记录原始图元信息
	CToolPath* pTool;
	int m_Number;//记录层数

	//记录外侧面粗加工每层的形状顺序信息,由外向内
	vector<vector<CPtrArray*>> m_EverySideRoughPile;

	//标记
	CString FLAG;
};

