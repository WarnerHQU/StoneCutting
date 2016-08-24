#pragma once

#include"SplitToDoc.h"
#include"MainFrm.h"

//工具类，提供一些工具方法，可以设定成是静态的方法，故可以不用实例化对象


class CUtils
{
public:
	CUtils();
	~CUtils();
private:
	//CSplitToDoc* pMyDoc = NULL;
	
public:
	//指定具体的对话框的句柄，设置打开文件对话框的标题，以及显示打开路径的文本空间的ID
	static CPtrArray* OpenFile(HWND hwnd,CString DialogName,int nShowText);

public:
	//清除数据
	static void ClearAllData();
	

	
};

