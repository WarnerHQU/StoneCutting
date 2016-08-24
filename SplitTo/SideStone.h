
#pragma once
#include"ToolPath.h"
/*继承自CToolPath会出现基类未定义的问题！*/
class CSideStone : public  CToolPath
{
public:
	CSideStone();
	~CSideStone();
public:
	int mark;//用于存储封闭图形有几个元素
	CPoint3D XMax;//用于存储X坐标方向上值最大的点的坐标，一般为起点
	void ChangeSideProcess();
	vector<CPoint3D> OffsetAlgorithm(CPtrArray& XFarray, int mark, double offset);
	void RenderSide(COpenGLDC* pDC);
	CPoint3D GetCenterPoint();
	void SetArray(CPtrArray& Array)
	{
		this->m_ptArray.Copy(Array);
	}
	CPtrArray& GetArray()
	{
		return this->m_ptArray;
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

private:
	CPtrArray m_ptArray;
	float m_FeedSpeed;
	float m_FeedTime;
	float m_FeedSpeed_VX;
	float m_FeedTime_VX;
	float m_Piece_Long;
	float m_Piece_Width;
	float m_Piece_Heigth;

};

