/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gst_face_2d"
#endif
#include "gst_face_2d.h"


gst_face_2d::gst_face_2d( gst_polygon_2d_ref polygon)
  : outside_( polygon)
{
}

gst_face_2d::gst_face_2d( gst_polygon_2d_ref polygon, const vcl_vector<gst_polygon_2d_ref> &holes)
  : outside_( polygon),
    holes_( holes)
{
}

gst_face_2d::~gst_face_2d()
{
}
