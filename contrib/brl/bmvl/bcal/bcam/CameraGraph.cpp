// CameraGraph.cpp: implementation of the CameraGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraGraph.h"
#include <vcl_cassert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CameraGraph::CameraGraph()
{
	pCamG_ = new vcsl_graph;
	source_ = 0;
}

CameraGraph::~CameraGraph()
{
}

int CameraGraph::addSource(vcsl_spatial_sptr &v)
{
	// if already have one.
	if(source_){
		return 1;
	}
	else{
		v->set_graph(pCamG_);
		source_ = v;
		return 0;
	}
}

int CameraGraph::addVertex(vcsl_spatial_sptr &v, vcsl_spatial_transformation_sptr &e)
{
	if(!source_) // if no source
		return 1;

	if(pCamG_->has(v))
		return 2;

	v->set_graph(pCamG_);	
	v->set_unique(source_, e);
	return 0;
}

/*
int CameraGraph::addEdge(vcsl_spatial_sptr &v1, vcsl_spatial_sptr &v2, vcsl_spatial_transformation_sptr &e12)
{
}
*/