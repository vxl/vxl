#ifndef boxm2_vecf_middle_fat_pocket_h_
#define boxm2_vecf_middle_fat_pocket_h_
//:
// \file
// \brief  The middle_fat_pocket component of the head
//
// \author J.L. Mundy
// \date   10 Jan 2016
//
#include <iostream>
#include <string>
#include <vector>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <bvgl/bvgl_spline_region_3d.h>
#include <bvgl/bvgl_scaled_shape_3d.h>
#include "boxm2_vecf_middle_fat_pocket_params.h"
#include "boxm2_vecf_geometry_base.h"
class boxm2_vecf_middle_fat_pocket : public boxm2_vecf_geometry_base{
 public:
  boxm2_vecf_middle_fat_pocket()= default;
  boxm2_vecf_middle_fat_pocket(std::string const& geometry_file);
  boxm2_vecf_middle_fat_pocket(bvgl_scaled_shape_3d<double> const& ss3d, boxm2_vecf_middle_fat_pocket_params const& params);

  void set_params(boxm2_vecf_middle_fat_pocket_params const& params){ params_ = params;
    if(params_.fit_to_subject_)
      this->apply_scale_params();
    else
      this->apply_deformation_params();
  }

  boxm2_vecf_middle_fat_pocket_params params() const {return params_;}

  vgl_box_3d<double> bounding_box() const{
    return pocket_.bounding_box();}

  bool in(vgl_point_3d<double> const& pt) const{
    return pocket_.in(pt);}

  bool has_appearance() const {return false;}
  double distance(vgl_point_3d<double> const& p) const {return pocket_.distance(p);}

  //: the functor operator for surface distance.
  double operator() (vgl_point_3d<double> const& p) const override{ return pocket_.distance(p);}


  //: returns a deformed fat pocket
  boxm2_vecf_middle_fat_pocket deform() const;

  vgl_pointset_3d<double> random_pointset(unsigned n_pts) const{
    return pocket_.random_pointset(n_pts);
  }
  //: apply deformation parameters to the scaled shape. reduces overhead in computing
  // the inverse vector field since the same parameters are used for each call to
  // ::inverse_vector_field
  void apply_deformation_params();

  //: for an inverse vector field with anisotropic scaling (see previous function comment)
  void apply_scale_params();

  bool inverse_vector_field(vgl_point_3d<double> const& p, vgl_vector_3d<double>& inv_v) const override;

  //: find the forward vector field for the closest point on *this
  bool closest_inverse_vector_field(vgl_point_3d<double> const& p, vgl_vector_3d<double>& vf) const;

  //: accessors
  const bvgl_scaled_shape_3d<double>& pocket_shape() const{return pocket_;}

    //: for debug purposes
  void print_vf_centroid_scan(double off_coef) const;
 private:
  void read_middle_fat_pocket(std::istream& istr);
  bvgl_spline_region_3d<double> base_;
  boxm2_vecf_middle_fat_pocket_params params_;
  bvgl_scaled_shape_3d<double> pocket_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_middle_fat_pocket const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_middle_fat_pocket& pr);

#endif// boxm2_vecf_middle_fat_pocket
