// zhang_linear_calibrate.h: interface for the zhang_linear_calibrate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_
#define AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcsl/vcsl_graph_sptr.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include "camera_graph.h"
#include "calibrate_plane.h"
#include "zhang_camera_node.h"
#include "euclidean_transformation.h"

class zhang_linear_calibrate
{
 public:
  int calibrate();
  void setCameraGraph(camera_graph<calibrate_plane, zhang_camera_node, euclidean_transformation>* pG);
  zhang_linear_calibrate();
  virtual ~zhang_linear_calibrate();

 private:
  camera_graph<calibrate_plane, zhang_camera_node, euclidean_transformation>* camGraph_;
  vcl_vector<vgl_h_matrix_2d<double> *> h_matrice_;
  vcl_vector<int> num_views_;

 protected:
  int compute_homography();
  vnl_vector_fixed<double, 6> homg_constrain(vgl_h_matrix_2d<double> const& hm, int i, int j);
  // homographies list hm_list[0...n-1]
  vnl_double_3x3 compute_intrinsic(vgl_h_matrix_2d<double> *hm_list, int n);
  // allocate memory to store homography according to camera graph
  int initialize();
  // clear allocated memory.
  int clear();
};

#endif // AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_
