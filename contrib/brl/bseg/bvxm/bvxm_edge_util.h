#ifndef bvxm_edge_util_h_
#define bvxm_edge_util_h_
//:
// \file
// \brief Various utility methods and classes related to edge probability updates
// \author Ibrahim Eden (ieden@lems.brown.edu)
// \date Jul 14,  2009
//
// \verbatim
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

  static vil_image_view<float> multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma);

  //returns a binary map of edges with pixel resolution
  static vil_image_view<vxl_byte> detect_edges(vil_image_view<vxl_byte> img, double noise_multiplier, double smooth, bool automatic_threshold, bool junctionp, bool aggressive_junction_closure);

  // return image has three planes:
  // plane 0 - sub-pixel column position of the edge.
  //           Negative value indicates no edge is present
  // plane 1 - sub-pixel row position of the edge.
  //           Negative value indicates no edge is present
  // plane 2 - Orientation of local edge tangent direction in radians
  // range is [0, 2pi).
  static vil_image_view<float> detect_edge_tangent(vil_image_view<vxl_byte> img,
                                                   double noise_multiplier,
                                                   double smooth,
                                                   bool automatic_threshold,
                                                   bool junctionp,
                                                   bool aggressive_junction_closure);

  static void edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt);

  static float convert_edge_statistics_to_probability(float edge_statistic, float n_normal, int dof);

  static void estimate_edge_prob_image(const vil_image_view<vxl_byte>& img_edge, vil_image_view<float>& img_edgeness, const int mask_size, const float mask_sigma);

  static vbl_array_2d<float> get_spherical_gaussian_kernel(const int size, const float sigma);

  template <class T_from,class T_to>
  static void convert_image_types(const vil_image_view<T_from>& inp_image, vil_image_view<T_to>& out_image, float scale, float offset);
};

template <class T_from,class T_to>
void bvxm_edge_util::convert_image_types(const vil_image_view<T_from>& inp_image, vil_image_view<T_to>& out_image, float scale, float offset)
{
  out_image.set_size(inp_image.ni(),inp_image.nj(),inp_image.nplanes());

  for (unsigned i=0; i<inp_image.ni(); i++) {
    for (unsigned j=0; j<inp_image.nj(); j++) {
      for (unsigned k=0; k<inp_image.nplanes(); k++) {
        float curr_pixel = (float)inp_image(i,j,k);
        out_image(i,j,k) = (T_to)((curr_pixel*scale) + offset);
      }
    }
  }
}

#endif // bvxm_edge_util_h_
