//------------------------------------------------------------------------------
// FileName    : bcal\bcam\camera_graph.h
// Author      : Kongbin Kang (kk@lems.brown.edu)
// Company     : Brown University
// Purpose     : a wapper class for provide some functionals
//               which are not supported by vcsl_graph.
// Date Of Creation: 3/25/2003
// Modification History :
// Date             Modifications
//------------------------------------------------------------------------------

#ifndef AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_
#define AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcsl/vcsl_graph.h>
#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_spatial_sptr.h>
#include <vcsl/vcsl_spatial_transformation_sptr.h>

template<class ObjectNode, class CameraNode, class Trans>
class camera_graph
{
 private:
  vcsl_graph_sptr pCamG_;
  vcl_vector<Trans*> edges_;
  vcl_vector<int> vertice_; // store camera IDs
  ObjectNode* source_;
  int nCams_; // number of cameras

 public: // constructor and de-constructor

  camera_graph()
  {
    pCamG_ = new vcsl_graph;
    source_ = new ObjectNode;
    source_->set_graph(pCamG_);
    nCams_ = 0;
  }

  virtual ~camera_graph()
  {
    if (source_)
      delete source_;

    source_ = 0;

    vertice_.clear();
  }

 public: // operations
  ObjectNode* getSource() {  return source_;}

  // add a new camera and edge into the graph
  // it return a camera ID.
  int addVertex()
  {
    CameraNode *newCamera = new CameraNode(nCams_++);
    vcsl_spatial_sptr v = newCamera;
    if (!source_){ // if no source
      vcl_cerr<<"no source in the camera graph\n";
      vcl_exit(1);
    }

    Trans* e = new Trans;
    v->set_graph(pCamG_);
    v->set_unique(source_, e);
    edges_.push_back(e);

    int id = newCamera->get_id();
    vertice_.push_back(id);
    return id;
  }

  // return a camera 0-based
  CameraNode* get_vertex(int i)
  {
    // the first one are source
     vcsl_spatial_sptr v = pCamG_->item(i+1);
     CameraNode* pV = (CameraNode*)v.ptr();
     //assert(pV->get_id()==i);
     return pV;
  }

  vcl_vector<int> get_vertex_ids()
  {
    return vertice_;
  }

  // get edge from v1 to v2
  Trans* getEdge(int v1, int v2)
  {
    assert(v1 == 0 && v2 <= nCams_); // only from souce to camera is avaible
    return edges[v2-1];
  }

  int num_vertice() { return vertice_.size();}
};

#endif // AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_
