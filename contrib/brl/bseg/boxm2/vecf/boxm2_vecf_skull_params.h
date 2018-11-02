#ifndef boxm2_vecf_skull_params_h_
#define boxm2_vecf_skull_params_h_
//:
// \file
// \brief  Parameters for the skull model
//
// \author J.L. Mundy
// \date   06 September 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_cranium_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
class boxm2_vecf_skull_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_skull_params():left_lateral_canthus_(vgl_point_3d<double>(59.1,23.52,52.78)),
    right_lateral_canthus_(vgl_point_3d<double>(-59.1,23.52,52.78)), mid_upper_jaw_(vgl_point_3d<double>(0.0, -59.26, 78.9)),
    mid_forehead_(vgl_point_3d<double>(0.33, 70.68,81.95)), mid_forehead_normal_(vgl_vector_3d<double>(0.078,0.133, 0.988)),
    trans_(vgl_h_matrix_3d<double>())
   {
  }
  void compute_auxillary_pts();
  // the points and vectors below are defined for the source skull geometry
  // these points define the affine transformation of the skull to the target head
  vgl_point_3d<double> left_lateral_canthus_;
  vgl_point_3d<double> right_lateral_canthus_;
  vgl_point_3d<double> mid_upper_jaw_;
  vgl_point_3d<double> mid_forehead_;
  vgl_vector_3d<double> mid_forehead_normal_;
  vgl_vector_3d<double> canth_mid_vect_;
  vgl_point_3d<double> forehead_intersection_;
  //: transform skull from source to target
  vgl_h_matrix_3d<double> trans_;

  //: component parameters
  boxm2_vecf_mandible_params mandible_params_;
  boxm2_vecf_cranium_params cranium_params_;
};
// TO DO implement these
std::ostream&  operator << (std::ostream& s, boxm2_vecf_skull_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_skull_params& pr);

#endif// boxm2_vecf_skull_params
