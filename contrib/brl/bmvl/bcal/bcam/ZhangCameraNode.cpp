// ZhangCamera.cpp: implementation of the ZhangCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "ZhangCameraNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZhangCameraNode::ZhangCameraNode(int nViews)
{
	// build lens distortion model
	vcl_vector<bool> flags(7, false);
	flags[0] = true;
	flags[1] = true;
	_pCam -> setLensModel(flags);

	// allocate space to store features.
	_pImageLists = new vcl_list< vgl_point_2d<double> > [nViews];
}

ZhangCameraNode::~ZhangCameraNode()
{
	if(_pImageLists)
		delete  [] _pImageLists;
}
