// This is gel/gst/gst_face_2d.h
#ifndef gst_face_2d_h_
#define gst_face_2d_h_
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "gst_polygon_2d_sptr.h"

//: This class defines a simplified topology structure in 2D.
// It is light weight, and all interfacing should be done through the
// face class

class gst_face_2d
{
 public:

  gst_face_2d( const gst_polygon_2d_sptr& polygon);
  gst_face_2d( const gst_polygon_2d_sptr& polygon, std::vector<gst_polygon_2d_sptr> holes);
  ~gst_face_2d();

 protected:

  // outside edge list
  gst_polygon_2d_sptr outside_;

  // list of holes
  std::vector<gst_polygon_2d_sptr> holes_;
};

#endif // gst_face_2d_h_
