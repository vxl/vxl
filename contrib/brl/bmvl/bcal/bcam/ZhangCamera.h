// ZhangCamera.h: interface for the ZhangCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_)
#define AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CameraNode.h"

class ZhangCameraNode : public CameraNode  
{
private:
	vcl_list<HomePoint2D> *_pImageLists; 
public:
	ZhangCameraNode(int nViews);
	virtual ~ZhangCameraNode();

};

#endif // !defined(AFX_ZHANGCAMERA_H__EB787129_58FA_4195_A386_71D7CC0C9546__INCLUDED_)
