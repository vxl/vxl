/******************************************************************************************
FileName		: E:\...\bcal\bcam\CameraNode.h
Author			: your name here
Purpose		    : This is a wrap class for vcsl_graph which provide some functions not supported by vcl_graph yet
Date Of Creation: 3/25/2003
Modification History :
Date             Modifications
******************************************************************************************/

// CameraNode.h: interface for the CameraNode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_
#define AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcsl/vcsl_spatial.h>
#include "camera.h"

class CameraNode : public vcsl_spatial
{
 protected:
  Camera* pCam_;
 public:
  CameraNode();
  virtual ~CameraNode();
};

#endif // AFX_CAMERANODE_H__72E24F49_51C3_4792_A5E8_A670182B472F__INCLUDED_
