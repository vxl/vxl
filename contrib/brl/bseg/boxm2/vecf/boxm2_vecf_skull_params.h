#ifndef boxm2_vecf_skull_params_h_
#define boxm2_vecf_skull_params_h_
//:
// \file
// \brief  Parameters for the skull model
//
// \author J.L. Mundy
// \date   06 September 2015
//
#include "boxm2_vecf_articulated_params.h"
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
class boxm2_vecf_skull_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_skull_params():  left_lateral_canthus_(vgl_point_3d<double>(59.1,23.52,52.78)),
    right_lateral_canthus_(vgl_point_3d<double>(-59.1,23.52,52.78)), mid_upper_jaw_(vgl_point_3d<double>(0.0, -59.26, 78.9)),
    mid_forehead_(vgl_point_3d<double>(0.33, 70.68,81.95)), mid_forehead_normal_(vgl_vector_3d<double>(0.078,0.133, 0.988)),
    trans_(vgl_h_matrix_3d<double>()){
    compute_auxillary_pts();
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
  //: transform from source to target
  vgl_h_matrix_3d<double> trans_;
};
vcl_ostream&  operator << (vcl_ostream& s, boxm2_vecf_skull_params const& pr);
vcl_istream&  operator >> (vcl_istream& s, boxm2_vecf_skull_params& pr);

#endif// boxm2_vecf_skull_params
