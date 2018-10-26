#ifndef boxm2_vecf_mouth_h_
#define boxm2_vecf_mouth_h_
//:
// \file
// \brief  The mouth component of the head
//
// \author J.L. Mundy
// \date   20 Dec 2015
//
#include <iostream>
#include <string>
#include <vector>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bvgl/bvgl_spline_region_3d.h>
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_mouth_params.h"
class boxm2_vecf_mouth{
 public:
  boxm2_vecf_mouth()= default;
  boxm2_vecf_mouth(std::string const& geometry_file){};

  boxm2_vecf_mouth(std::vector<vgl_point_3d<double> >  const& knots);
  boxm2_vecf_mouth(vgl_pointset_3d<double>  const& ptset);

  void read_mouth(std::istream& istr);

  void set_params(boxm2_vecf_mouth_params const& params){ params_ = params;}

  void set_mandible_params(boxm2_vecf_mandible_params const& mand_params);

  vgl_box_3d<double> bounding_box() const;

  bool valid_t(double t, double tolerance = 0.1) const{
    return (t>=(params_.t_min_-tolerance) && t<=(params_.t_max_+tolerance));
  }

  bool in_oris(vgl_point_3d<double> const& pt) const;
  bool in(vgl_point_3d<double> const& pt) const;

  //:for debug purposes
  void display_vrml(std::ofstream& ostr) const;
  void rotate_inf();
  vgl_pointset_3d<double> random_pointset(unsigned n_pts) const;
 private:

  bvgl_spline_region_3d<double> sup_;
  bvgl_spline_region_3d<double> inf_;
  boxm2_vecf_mandible_params mand_params_;
  boxm2_vecf_mouth_params params_;
  vgl_rotation_3d<double> rot_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_mouth const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_mouth& pr);

#endif// boxm2_vecf_mouth
