// This is brl/bbas/bvgl/bvgl_cross_section.h
#ifndef bvgl_cross_section_h_
#define bvgl_cross_section_h_
//:
// \file
// \brief A planar cross section as part of a generalized cylinder representation
// \author J. L. Mundy
// \date November 2, 2015

#include <utility>
#include <vector>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_bounding_box.h>

class bvgl_cross_section
{
public:
 bvgl_cross_section():t_(0.0){}
 bvgl_cross_section(double t, vgl_point_3d<double> p, vgl_plane_3d<double> plane, vgl_pointset_3d<double> ptset):
  t_(t),p_(p), plane_(plane), ptset_(std::move(ptset)){bbox_ = vgl_bounding_box(ptset_);}
  //: accessors
  double t() const {return t_;}
  vgl_point_3d<double> p() {return p_;}
  vgl_plane_3d<double> pl() const {return plane_;}
  vgl_pointset_3d<double> pts() const {return ptset_;}
  vgl_box_3d<double> bounding_box() const {return bbox_;}
  //: utility functions
  bool contains(vgl_point_3d<double> const&p) const;
  vgl_point_3d<double> closest_point(vgl_point_3d<double> const&p, double dist_thresh) const;

  //: for display purposes
  bvgl_cross_section apply_vector(vgl_vector_3d<double> const& v) const;
  // the swept cross section of the generalized cylinder
  void display_cross_section_plane(std::ofstream& ostr) const;
  // the point set associated with a cross section, i.e. points closest to the plane
  void display_cross_section_pts(std::ofstream& ostr) const;
  // each point with normal is represented by a disk, where the disk plane normal is the point normal
  void display_cross_section_normal_disks(std::ofstream& ostr) const;
 private:
  double t_;
  vgl_point_3d<double> p_;
  vgl_plane_3d<double> plane_;
  vgl_pointset_3d<double> ptset_;
  vgl_box_3d<double> bbox_;
};

#endif // bvgl_cross_section_h_
