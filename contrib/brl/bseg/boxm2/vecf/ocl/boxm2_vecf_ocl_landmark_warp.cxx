#include "boxm2_vecf_ocl_landmark_warp.h"
#include <boxm2/vecf/boxm2_vecf_vector_field.h>
#include <boxm2/vecf/boxm2_vecf_inverse_square_weighting_function.h>
#include <boxm2/vecf/boxm2_vecf_landmark_warp.h>

boxm2_vecf_ocl_landmark_warp::
    boxm2_vecf_ocl_landmark_warp(std::vector<vgl_point_3d<double> > const& control_pts_source,
                                 std::vector<vgl_point_3d<double> > const& control_pts_target,
                                 double weight_fun_max, double weight_fun_x_at_10_percent)
: boxm2_vecf_ocl_vector_field_adaptor( boxm2_vecf_vector_field_base_sptr(
     new boxm2_vecf_landmark_warp<boxm2_vecf_inverse_square_weighting_function>(
          control_pts_source,
          control_pts_target,
          boxm2_vecf_inverse_square_weighting_function(weight_fun_max, weight_fun_x_at_10_percent)
          )
      )
  )
{
}
