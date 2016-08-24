#pragma once
#include "ToolPath.h"



//对话框提取的数据到该类中进行保存、计算

typedef struct tagelements
{
	CPoint3D begin;
	CPoint3D end;
	int flag=1;//记录类型

	CPoint3D center = CPoint3D(0, 0, 0);
	float radius = 0;
	float start_angle = 0.0;
	float end_angle = 0.0;
	float tudu ;//普通圆弧为0，多线段圆弧不为0
	BOOL arc_direction = false;//圆弧的方向，默认为false,顺时针，true为逆时针
}elements;

class CTableBoardProcess :
	public CToolPath
{

public:
	static CTableBoardProcess* GetTableProcessInstance();
private:
	static CTableBoardProcess* m_TableProcess;

	//私有内嵌类，它唯一的好处就是山吹CTableBoardDlg实例
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (CTableBoardProcess::m_TableProcess)
				delete CTableBoardProcess::m_TableProcess;
		}
	};
	//程序在结束的时候，系统会自动析构所有的全局变量。
	//事实上，系统也会析构所有的类的静态成员变量，就像这些静态成员也是全局变量一样。
	//定义一个静态变量，程序结束的时候自动调用其析构函数。
	//这样写比直接放在析构函数里面要好，不会出现错误。
	static CGarbo Garbo;


private:
	CTableBoardProcess();
	~CTableBoardProcess();

public:

	//初始化数据
	void InitData(CPtrArray* input);
	//将原先的数据内容清空
	void ClearData();

	void ClearBoardArray();

	//根据高度以及底面的数据进行创建模型
	vector<CPtrArray*>  InitTableModel(CPtrArray* data,float high,float depth);
	
	//计算原始数据
	vector<elements> CalculateData(CPtrArray* input);
	vector<elements> New_CalculateData(CPtrArray* input);
	//将输入的input里面的直线的数据根据切深，刀具半径，对直线进行打断处理，并将数据重新存储起来
	vector<elements> ChangeSegments(vector<elements> data);
	
	//针对直线将一条直线截断成三段，并保存
	vector<elements> SeperateToThreeParts(elements LineData,float Diamand,float CutDepth);
	//识别内外封闭轮廓，并将内外封闭轮廓数据进行输出
	vector<vector<elements>> RegonizeProfile(vector<elements> data);

	//将vector<elements>的元素转为CPtrArray方便数据的离散化以及显示
	CPtrArray* Change_To_Array(vector<elements> data);
	elements Copy_Elements(elements data);
	vector<CPtrArray*> Copy_CPtrArrays(vector<CPtrArray*> data);
private:	
	//存储原始的指针数据，所有的数据
	CPtrArray* m_ptBoardArray;//CPtrArray*->vector<elements>------->CPtrArray*方便画图与存储
	
	CPtrArray* m_innerArray;//内

	CPtrArray* m_outArray;//外

	CPtrArray* m_outOnlyLineArray;//只存储外轮廓的直线元素
	CPtrArray* m_inOnlyLineArray;//只存储内轮廓的直线元素

	vector<CPtrArray*> m_ArrayList;
	vector<CPtrArray*> m_InArrayList;
	vector<CPtrArray*> m_OutArrayList;

	
	
	//存储原始数据,只记录起始点，终止点，半径，圆心。
	vector<elements> m_SourceData;
	//存储裁剪后的数据，只记录起始点，终止点，半径，圆心。
	vector<elements> m_CutData;
	//存储内、外轮廓的数据，只记录起始点，终止点，半径，圆心。
	vector<vector<elements>> m_ProfileData;


public:
	//显示工件正常尺寸
	void RenderTableBoard(COpenGLDC* pDC);
	//根据CPtrArray*来画,里面还需要计算，比较慢
	void DrawDIY(CPtrArray* array, COpenGLDC* pDC);
	//
	void DrawDIYFromPt(vector<vector<CPoint3D>> data, COpenGLDC* pDC);
	void DrawDIYFromPt(vector<vector<vector<vector<CPoint3D>>>> data,COpenGLDC* pDC);
	void DrawDIYFromOffset(vector<OffsetPoint> data, COpenGLDC* pDC);
	void DrawOutRetractLine(vector<vector<vector<CPoint3D>>> data, COpenGLDC* pDC);/*绘制外部轮廓的进退刀线*/
	void DrawInSawRetractLine(vector<vector<vector<vector<CPoint3D>>>> data, COpenGLDC* pDC);/*绘制内部锯片轮廓的进退刀线*/
	void DrawInMillRetractLine(vector<vector<vector<vector<CPoint3D>>>> data, COpenGLDC* pDC);/*绘制内部铣刀轮廓的进退刀线*/

	//显示毛坯
	void InitTableBlank();
	vector<OffsetPoint> OffsetShapeAlgorithm(CPtrArray& XFarray, int mark, double offset);//获取偏移后轮廓起始点信息
	void RenderTableBlank(COpenGLDC* pDC);
	//显示内轮廓轨迹
	void InitTableInPath();
	void InitInArray();
	void InitTableSimulationPoints(vector<vector<vector<CPoint3D>>> m_outPts, 
		                          vector<vector<vector<vector<CPoint3D>>>> m_inSaw, vector<vector<vector<vector<CPoint3D>>>> m_inMill);
	//返回插入后的新的elements集合
	vector<elements> NewInsertElement(vector<elements> data);
	CPtrArray* CopyData(CPtrArray* data,vector<int> number);//复制数据,根据位置,number里面存储的是下标的位置
	vector<vector<vector<vector<CPoint3D>>>> ChangesToMillPoints(vector<vector<vector<vector<CPoint3D>>>> data,float m_MillDepth);//对离散后的铣刀的点进行处理，使其点能够按照铣刀的加工方向分布
	vector<vector<vector<CPtrArray*>>> ChangesToMillProfiles(vector<vector<vector<CPtrArray*>>> data, float high, float depth);//转成符合铣刀加工的偏置轨迹，从原先与直线相连的状态下。
	int pos[4];//记录位置信息
	void RenderTableMillPath(COpenGLDC* pDC);//显示铣刀轨迹
	void RenderTableSawPath(COpenGLDC* pDC);//显示锯片轨迹
	//显示外轮廓轨迹
	void InitTableOutPath();
	vector<elements> OffsetOutPathAlgorithm(CPtrArray& XFarray, int mark, double offset);//获取外部轮廓偏置后的详细信息 
	vector<elements> InsertExtendLineElements(vector<elements> data,float extendDis);//在首尾处插入直线，方便锯片进刀。
	void RenderTableOutPath(COpenGLDC* pDC);

public:
	//转为点
	vector<CPoint3D> ConverToPoints(CPtrArray* data, float F, float T);
	vector<vector<CPoint3D>> GetPoints(vector<CPtrArray*> data,float F,float T);
	vector<CPoint3D> GetPoints(CPtrArray* data, float F, float T);

	//将外侧面向外偏移offset距离
	vector<vector<CPoint3D>> CoverOutsideProfToPoints(vector<CPtrArray*> data,float offset,float F, float T);
	//不根据偏置求取点，根据存储的CPtrArray*
	vector<vector<CPoint3D>> CoverOutsideProfToPoints(vector<CPtrArray*> data, float F, float T);
	//将内侧面向内偏移offset距离
	vector<vector<CPoint3D>> CoverInsideProfToPoints(vector<CPtrArray*> data, float offset, float F, float T);

	//内置仿真点
	vector<CPoint3D> m_BoardRoughSimulation;
	vector<CPoint3D> m_BoardFinishSimulation;

private:
	vector<vector<CPoint3D>> m_ModelPoints;//纯模型的离散点
	vector<vector<CPoint3D>> m_InnerModelPoints;
	vector<vector<CPoint3D>> m_OuterModelPoints;

	vector<vector<CPoint3D>> m_OuterBlankPoints;

	//底面和顶面的偏置轮廓
	vector<OffsetPoint> m_OuterBlankOffset;
	vector<OffsetPoint> m_InBlankOffset;
	vector<OffsetPoint> m_OuterBlankTopOffset;
	vector<OffsetPoint> m_InBlankTopOffset;

	//外侧偏置轨迹用点
	vector<vector<CPtrArray*>> m_OutOffsetProfile;
	vector<vector<vector<CPoint3D>>> m_OutOffsetPoints;
	//外侧裁剪轨迹用点

	//内侧锯片偏置轨迹
	vector<vector<CPtrArray*>> m_inOffsetProfile;
	vector<vector<vector<CPoint3D>>> m_inOffsetPoints;

	vector<vector<vector<CPtrArray*>>> m_InOffsetSawProfile;
	vector<vector<vector<vector<CPoint3D>>>> m_InOffsetSawPoints;
	//内侧铣刀偏置轨迹
	vector<vector<vector<CPtrArray*>>> m_InOffsetMillProfile;
	vector<vector<vector<vector<CPoint3D>>>> m_InOffsetMillPoints;

	//初始化生成NC代码所需要的数据，需要插入相应的点
	//用函数calculate(CPtrArray* )计算可得vector<elements>
	public:
		vector<vector<vector<elements>>> InitTableOutNC(vector<vector<CPtrArray*>> data);
		vector<vector<vector<vector<elements>>>> InitTableInSawNC(vector<vector<vector<CPtrArray*>>> data);
		vector<vector<vector<vector<elements>>>> InitTableInMillNC(vector<vector<vector<CPtrArray*>>> data);
private:
	vector<vector<vector<elements>>> m_TableOutNCElements;
	vector<vector<vector<vector<elements>>>> m_TableInSawNCElements;
	vector<vector<vector<vector<elements>>>> m_TableInMillNCElements;
	public:
		vector<vector<vector<elements>>> GetTableOutNCElements()
		{
			return this->m_TableOutNCElements;
		}
		vector<vector<vector<vector<elements>>>> GetTableInSawNCElements()
		{
			return this->m_TableInSawNCElements;
		}
		vector<vector<vector<vector<elements>>>> GetTableInMillNCElements()
		{
			return this->m_TableInMillNCElements;
		}
	//对话框数据存储
private:
	int m_TotalSize;//存储铣刀每一小段的点总数
	float m_PerDownDis;//存储每一小段下降的高度


	float m_ExtendLength;
	float m_Board_Heigh;
	float m_Board_Length;
	float m_Board_Width;

	float m_OutLeft;
	float m_InLeft;

	float m_B_FeedSpeed_Horizon;
	float m_B_FeedTime_Horizon;
	float m_B_FeedLoad_Horizon;
	//水平每刀切的深度
	float m_B_FeedDis_Horizon;

	float m_B_FeedSpeed_Vertical;
	float m_B_FeedTime_Vertical;
	//垂直下刀每刀的深度
	float m_B_FeedLoad_Vertical;


	//铣刀设置
	float m_MillToolDiamand;
	float m_MillCutDepth_Vertical;
	float m_MillCutDepth_Horizon;
	//参数的设置与获取
public:
	void SetBoardToolDiamnd(float data)
	{
		this->m_MillToolDiamand = data;
	}
	float GetBoardToolDiamand()
	{
		return this->m_MillToolDiamand;
	}
	void SetBoardMillCutDepthVertical(float data)
	{
		this->m_MillCutDepth_Vertical = data;
	}
	float GetBoardMillCutDepthVertical()
	{
		return this->m_MillCutDepth_Vertical;
	}
	void SetBoardMillCutDepthHorizon(float data)
	{
		this->m_MillCutDepth_Horizon = data;
	}
	float GetBoardMillCutDepthHorizon()
	{
		return this->m_MillCutDepth_Horizon;
	}

	CPtrArray* GetBoardArray()
	{
		//保持数据的封装性，对于m_ptBoardArray只能访问，不能更改其指向地址，但是可以通过指针更改指针所指向的值。
		return this->m_ptBoardArray;
	}

	void SetBoardHeigh(float data)
	{
		this->m_Board_Heigh = data;
	}
	float GetBoardHeigh()
	{
		return  this->m_Board_Heigh;
	}

	void SetBoardWidth(float data)
	{
		this->m_Board_Width = data;
	}
	float GetBoardWidth()
	{
		return this->m_Board_Width;
	}

	void SetBoardLength(float data)
	{
		this->m_Board_Length = data;
	}
	float GetBoardLength()
	{
		return this->m_Board_Length;
	}

	void SetBoardInLeft(float data)
	{
		this->m_InLeft = data;
	}
	float GetBoardInLeft()
	{
		return this->m_InLeft;
	}
	void SetBoardOutLeft(float data)
	{
		this->m_OutLeft = data;
	}
	float GetBoardOutLeft()
	{
		return this->m_OutLeft;
	}

	void SetBoardFeedSpeedHorizon(float data)
	{
		this->m_B_FeedSpeed_Horizon = data;
	}
	float GetBoardFeedSpeedHorizon()
	{
		return this->m_B_FeedSpeed_Horizon;
	}

	void SetBoardFeedTimeHorizon(float data)
	{
		this->m_B_FeedTime_Horizon = data;
	}
	float GetBoardFeedTimeHorizon()
	{
		return this->m_B_FeedTime_Horizon;
	}

	void SetBoardFeedLoadHorizon(float data)
	{
		this->m_B_FeedLoad_Horizon = data;
	}
	float GetBoardFeedLoadHorizon()
	{
		return this->m_B_FeedLoad_Horizon;
	}

	void SetBoardFeedDisHorizon(float data)
	{
		this->m_B_FeedDis_Horizon = data;
	}
	float GetBoardFeedDisHorizon()
	{
		return this->m_B_FeedDis_Horizon;
	}

	void SetBoardFeedSpeedVertical(float data)
	{
		this->m_B_FeedSpeed_Vertical = data;
	}
	float GetBoardFeedSpeedVertical()
	{
		return this->m_B_FeedSpeed_Vertical;

	}
	void SetBoardFeedTimeVertical(float data)
	{
		this->m_B_FeedTime_Vertical = data;
	}
	float GetBoardFeedTimeVertical()
	{
		return this->m_B_FeedTime_Vertical;
	}
	void SetBoardFeedLoadVertical(float data)
	{
		this->m_B_FeedLoad_Vertical = data;
	}
	float GetBoardFeedLoadVertical()
	{
		return this->m_B_FeedLoad_Vertical;
	}
};

