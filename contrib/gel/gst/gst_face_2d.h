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

#include <vcl/vcl_vector.h>
#include <gst/gst_edge_2d_ref.h>

class gst_face_2d
{
public:



protected:

  // ordered list of OUTSIDE edges
  vcl_vector<gst_edge_2d_ref> edges_;

  // list of holes (only one depth of hole is allowed
};


#endif
