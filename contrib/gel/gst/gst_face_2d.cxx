// This is gel/gst/gst_face_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_face_2d.h"

gst_face_2d::gst_face_2d( gst_polygon_2d_sptr polygon)
  : outside_( polygon)
{
}

gst_face_2d::gst_face_2d( gst_polygon_2d_sptr polygon, const vcl_vector<gst_polygon_2d_sptr> &holes)
  : outside_( polygon),
    holes_( holes)
{
}

gst_face_2d::~gst_face_2d()
{
}
