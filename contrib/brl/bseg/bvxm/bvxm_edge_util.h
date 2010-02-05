#ifndef bvxm_edge_util_h_
#define bvxm_edge_util_h_
//:
// \file
// \brief Various utility methods and classes related to edge probability updates
// \author Ibrahim Eden (ieden@lems.brown.edu)
// \date Jul 14,  2009
//
// \verbatim
//  Modifications
//   December 15, 2009 - Ibrahim Eden - removed convert_image_types
// \endverbatim

#include "bvxm_world_params.h"

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

class bvxm_edge_util
{
 public:
  //returns a binary map of edges with pixel resolution
  static vil_image_view<vxl_byte> detect_edges(vil_image_view<vxl_byte> img, double noise_multiplier, double smooth, bool automatic_threshold, bool junctionp, bool aggressive_junction_closure);

  static void edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt);

  static vil_image_view<float> multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma);
  static void estimate_edge_prob_image(const vil_image_view<vxl_byte>& img_edge, vil_image_view<float>& img_edgeness, const int mask_size, const float mask_sigma);
  static vbl_array_2d<float> get_spherical_gaussian_kernel(const int size, const float sigma);

  static float convert_edge_statistics_to_probability(float edge_statistic, float n_normal, int dof);
};

#endif // bvxm_edge_util_h_
