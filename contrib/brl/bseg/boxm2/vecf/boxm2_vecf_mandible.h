#ifndef boxm2_vecf_mandible_h_
#define boxm2_vecf_mandible_h_
//:
// \file
// \brief  The mandible represented as a generalized cylinder
//
// \author J.L. Mundy
// \date   28 Aug 2015
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <vcl_compiler.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include "boxm2_vecf_spline_field.h"
#include <bvgl/bvgl_cross_section.h>
#include <bvgl/bvgl_gen_cylinder.h>
#include "boxm2_vecf_mandible_params.h"
class boxm2_vecf_mandible : public bvgl_gen_cylinder{
 public:

 boxm2_vecf_mandible(){
   fill_boundary_map();
  }
 boxm2_vecf_mandible(std::string const& geometry_file);

 boxm2_vecf_mandible(std::map<std::string, unsigned> const& boundary_knots, vgl_cubic_spline_3d<double> const& axis,
                     std::vector<bvgl_cross_section> const& cross_sects, double cross_section_interval=0.5):
 bvgl_gen_cylinder(axis, cross_sects, cross_section_interval), boundary_knots_(boundary_knots){
   fill_boundary_map();
 }

 boxm2_vecf_spline_field translate(vgl_vector_3d<double> const& tr);

 //: offset axis curve in the curve normal direction
 boxm2_vecf_spline_field offset_axis(double offset);

 //: adjust slope of ramus relative to nominal
 boxm2_vecf_spline_field tilt_ramus(double delta_z_at_condyle);

 //: adjust slope of body relative to nominal
 boxm2_vecf_spline_field  tilt_body(double delta_y_at_chin);

 //: create a new mandible by applying a vector field
 boxm2_vecf_mandible apply_vector_field(boxm2_vecf_spline_field const& field) const;

 //: the functor operator for surface distance. dist_thresh is the distance a closest point on the normal plane
 // can be away from the closest point in the cross-section pointset.
 double operator() (vgl_point_3d<double> p) const{ return bvgl_gen_cylinder::distance(p, params_.planar_surface_dist_thresh_);}

 //:for debug purposes
 virtual void display_axis_spline(std::ofstream& ostr) const;
 private:
 boxm2_vecf_mandible_params params_;
 void fill_boundary_map();
 std::map<std::string, unsigned> boundary_knots_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_mandible const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_mandible& pr);

#endif// boxm2_vecf_mandible
