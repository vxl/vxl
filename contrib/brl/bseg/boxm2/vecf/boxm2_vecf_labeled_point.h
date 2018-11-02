#ifndef boxm2_vecf_labeled_point_h_
#define boxm2_vecf_labeled_point_h_
//:
// \file
// \brief  A labeled 3-d point to represent anatomical landmarks
//
// \author J.L. Mundy
// \date   6 Sept 2015
//
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//: a structure for holding a 3-d point with associated label
struct boxm2_vecf_labeled_point{
  boxm2_vecf_labeled_point()= default;
boxm2_vecf_labeled_point(vgl_point_3d<double> const& p, std::string  label):
  p3d_(p), label_(std::move(label)){}
  boxm2_vecf_labeled_point(double x, double y, double z, std::string  label):
  p3d_(vgl_point_3d<double>(x,y,z)), label_(std::move(label)){}
  static bool read_points(std::string const& path, std::map<std::string, std::vector<vgl_point_3d<double> > >& point_map);
  //members
  vgl_point_3d<double> p3d_;
  std::string label_;
};
#endif// boxm2_vecf_labeled_point
