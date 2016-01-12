#ifndef boxm2_vecf_labeled_point_h_
#define boxm2_vecf_labeled_point_h_
//:
// \file
// \brief  A labeled 3-d point to represent anatomical landmarks
//
// \author J.L. Mundy
// \date   6 Sept 2015
//
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_map.h>
//: a structure for holding a 3-d point with associated label
struct boxm2_vecf_labeled_point{
  boxm2_vecf_labeled_point(){}
boxm2_vecf_labeled_point(vgl_point_3d<double> const& p, vcl_string const& label):
  p3d_(p), label_(label){}
  boxm2_vecf_labeled_point(double x, double y, double z, vcl_string const& label):
  p3d_(vgl_point_3d<double>(x,y,z)), label_(label){}
  static bool read_points(vcl_string const& path, vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > >& point_map);
  //members
  vgl_point_3d<double> p3d_;
  vcl_string label_;
};
#endif// boxm2_vecf_labeled_point
