#ifndef gst_face_2d_h_
#define gst_face_2d_h_
#ifdef __GNUC__
#pragma interface "gst_face_2d"
#endif
/*
  crossge@crd.ge.com
*/

/* 
This class defines a simplified topology structure in 2D.  It
is light weight, and all interfacing should be done through the
face class
*/

#include <vcl_vector.h>
#include <gst/gst_polygon_2d_ref.h>

class gst_face_2d
{
public:

  gst_face_2d( gst_polygon_2d_ref polygon);
  gst_face_2d( gst_polygon_2d_ref polygon, const vcl_vector<gst_polygon_2d_ref> &holes);
  ~gst_face_2d();

protected:

  // outside edge list
  gst_polygon_2d_ref outside_;

  // list of holes
  vcl_vector<gst_polygon_2d_ref> holes_;
};


#endif
