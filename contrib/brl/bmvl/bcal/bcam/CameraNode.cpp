// CameraNode.cpp: implementation of the CameraNode class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CameraNode::CameraNode()
{
  pCam_ = 0;
  pCam_ = new Camera;
}

CameraNode::~CameraNode()
{
  if (pCam_)
    delete pCam_;
}

#if 0 // removed
int CameraNode::addNeighbour(CameraNode *p)
{
  neighbours_.push_back(p);
  return 0;
}
#endif // 0

