//------------------------------------------------------------------------------
// FileName    : bcal\bcam\CameraGraph.h
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

#include <vcsl/vcsl_graph.h>
#include <vcsl/vcsl_spatial.h>
#include <vcsl/vcsl_spatial_sptr.h>
#include <vcsl/vcsl_spatial_transformation_sptr.h>

class CameraGraph
{
  vcsl_graph_sptr pCamG_;
  vcsl_spatial_sptr source_;
 public:
  int addSource(vcsl_spatial_sptr &v);
  int addVertex(vcsl_spatial_sptr &v, vcsl_spatial_transformation_sptr &e);
#if 0 // not implement yet.
  int addEdge(vcsl_spatial_sptr &v1, vcsl_spatial_sptr &v2, vcsl_spatial_transformation_sptr &e12);
#endif
  CameraGraph();
  virtual ~CameraGraph();
};

#endif // AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_
