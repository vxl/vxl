#include "boxm2_vecf_ocl_landmark_warp.h"
#include <boxm2/vecf/boxm2_vecf_landmark_warp.h>

boxm2_vecf_ocl_landmark_warp::
    boxm2_vecf_ocl_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                                 vcl_vector<vgl_point_3d<double> > const& control_pts_target)
: boxm2_vecf_ocl_vector_field_adaptor( 
      boxm2_vecf_vector_field_sptr(new boxm2_vecf_landmark_warp(control_pts_source,
                                                                control_pts_target))
      )
{
}
