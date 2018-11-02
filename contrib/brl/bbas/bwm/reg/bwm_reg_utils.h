#ifndef bwm_reg_utils_h_
#define bwm_reg_utils_h_

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_3d_sptr.h>
class bwm_reg_utils {
 public:
  //Back project a set of 2d digital curves onto a 3-d plane
static bool
  back_project_edges(std::vector<vsol_digital_curve_2d_sptr> const& edges_2d,
                     vpgl_rational_camera<double> const& rcam,
                     vgl_plane_3d<double> const& pl,
                     vgl_point_3d<double> const& initial_guess,
                     std::vector<vsol_digital_curve_3d_sptr>& edges_3d
                     );

//Forward project a set of 3d digital curves onto an image
static
void project_edges(std::vector<vsol_digital_curve_3d_sptr> const& edges_3d,
                   vpgl_rational_camera<double> const& rcam,
                   std::vector<vsol_digital_curve_2d_sptr>& edges_2d
                   );

private:
bwm_reg_utils();
};

#endif
