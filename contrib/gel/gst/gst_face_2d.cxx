/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
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
