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
