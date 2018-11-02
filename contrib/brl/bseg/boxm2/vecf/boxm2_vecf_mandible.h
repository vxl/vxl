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
#include <utility>
#include <vector>
#include <string>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include "boxm2_vecf_spline_field.h"
#include <bvgl/bvgl_cross_section.h>
#include <bvgl/bvgl_gen_cylinder.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_geometry_base.h"
class boxm2_vecf_mandible : public bvgl_gen_cylinder, public boxm2_vecf_geometry_base{
 public:

 boxm2_vecf_mandible(){
   fill_boundary_map(); set_inv_rot();
  }
 boxm2_vecf_mandible(std::string const& geometry_file);

 boxm2_vecf_mandible(std::map<std::string, unsigned>  boundary_knots, vgl_cubic_spline_3d<double> const& axis,
                     std::vector<bvgl_cross_section> const& cross_sects, double cross_section_interval=0.5):
 bvgl_gen_cylinder(axis, cross_sects, cross_section_interval), boundary_knots_(std::move(boundary_knots)){
   fill_boundary_map(); set_inv_rot();
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
 double operator() (vgl_point_3d<double> const& p) const override{ return bvgl_gen_cylinder::distance(p, params_.planar_surface_dist_thresh_);}

 //: compute the inverse vector field for a target point
 bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;
 //: accessors
 void set_params(boxm2_vecf_mandible_params const& params){params_ = params; this->set_inv_rot();}

 //: set inv_rot_ from params_;
 void set_inv_rot();
 const vgl_rotation_3d<double>& inv_rot() const{return inv_rot_;}

 //:for debug purposes
 void display_axis_spline(std::ofstream& ostr) const override;
 private:
 vgl_rotation_3d<double> inv_rot_;
 boxm2_vecf_mandible_params params_;
 void fill_boundary_map();
 std::map<std::string, unsigned> boundary_knots_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_mandible const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_mandible& pr);

#endif// boxm2_vecf_mandible
