// CameraNode.h: interface for the CameraNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_)
#define AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_list.h>
#include <vcsl/vcsl_spatial.h>

#include "Camera.h"
class CameraNode : public vcsl_spatial 
{
protected:
	Camera* _pCam;
public:
	CameraNode();
	virtual ~CameraNode();
};

#endif // !defined(AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_)
