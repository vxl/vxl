// CameraNode.cpp: implementation of the CameraNode class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CameraNode::CameraNode()
{
	_pCam = 0;
	_pCam = new Camera;
}

CameraNode::~CameraNode()
{
	if(_pCam)
		delete _pCam;
}

//DEL int CameraNode::addNeighbour(CameraNode *p)
//DEL {
//DEL 	_neighbours.push_back(p);
//DEL 	return 0;
//DEL }

