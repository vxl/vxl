#ifndef bvgl_gen_cylinder_h_
#define bvgl_gen_cylinder_h_
//:
// \file
// \brief  A simple generalized cylinder geometry
//
// \author J.L. Mundy
// \date   3 November 2015
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_box_3d.h>
#include "bvgl_cross_section.h"
class bvgl_gen_cylinder{
 public:
 bvgl_gen_cylinder(): cross_section_interval_(0.5){}
  bvgl_gen_cylinder(vgl_cubic_spline_3d<double>  axis, std::vector<bvgl_cross_section> const& cross_sects, double cross_section_interval=0.5);

  //:read the generalized cylinder axis
 void read_axis_spline(std::ifstream& istr){
   istr >> axis_;
 }
 //: construct cross sections (planes are defined at each knot perpendicular to the axis curve)
 // input is a pointset for the entire generalized cylinder. max_distance is how far a point
 // can be from the axis and be included in cross section pointset.
 void load_cross_section_pointsets(std::ifstream& istr);

 //: accessors
 const vgl_cubic_spline_3d<double>& axis() const {return axis_;}
 unsigned n_cross_sections() const{ return static_cast<unsigned>(cross_sections_.size());}

 bvgl_cross_section& cs(unsigned i){
   assert(i<static_cast<unsigned>(cross_sections_.size()));
   return cross_sections_[i];
 }
 vgl_box_3d<double> bounding_box() const{
   return bbox_;
 }
 //: geometric queries
 //: which cross_sections contain the point (can be more than one)
 std::vector<unsigned> cross_section_contains(vgl_point_3d<double> const& p) const;

 //: the point contained in a cross_section closest to p
 bool closest_point(vgl_point_3d<double> const& p, vgl_point_3d<double>& pc, double dist_thresh) const;

 //: the distance from p to a plane defined by the closest point and its normal
 double distance(vgl_point_3d<double> const& p, double dist_thresh) const;

 //: the total pointset contained in all the cross sections (may duplicate points)
 vgl_pointset_3d<double> aggregate_pointset() const;

 //: displays for debugging purposes
 // may be specialized to segment the cylinder into regions
 virtual void display_axis_spline(std::ofstream& ostr) const;

 void display_cross_section_planes(std::ofstream& ostr) const;
 void display_cross_section_pointsets(std::ofstream& ostr) const;
 void display_surface_disks(std::ofstream& ostr) const;

 protected:
 double cross_section_interval_;
  vgl_cubic_spline_3d<double> axis_;
  std::vector<bvgl_cross_section> cross_sections_;
  vgl_box_3d<double> bbox_;
};
#endif// bvgl_gen_cylinder
