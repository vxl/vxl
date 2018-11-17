// This is brl/bseg/sdet/sdet_img_edge.h
#ifndef sdet_img_edge_h_
#define sdet_img_edge_h_
//:
// \file
// \brief Various utility methods and classes related to edge probability updates
// \author Ibrahim Eden (ieden@lems.brown.edu)
// \date Jul 14,  2009
//
// \verbatim
//  Modifications
//   December 15, 2009 - Ibrahim Eden - removed convert_image_types
//   January 13, 2010 - Gamze Tunali - moved from bvxm/bvxm_edge_util.h to sdet_img_edge.h
//   July 07, 2010 - Ozge C. Ozcanli - added detect_edge_tangent_interpolated method
// \endverbatim

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class sdet_img_edge
{
 public:
  //returns a binary map of edges with pixel resolution
  static vil_image_view<vxl_byte> detect_edges(vil_image_view<vxl_byte> img, double noise_multiplier, double smooth, bool automatic_threshold, bool junctionp, bool aggressive_junction_closure);

  // return image has three planes:
  // plane 0 - sub-pixel column position of the edge.
  //           Negative value indicates no edge is present
  // plane 1 - sub-pixel row position of the edge.
  //           Negative value indicates no edge is present
  // plane 2 - Orientation of local edge tangent direction in radians
  // range is [0, 2pi).
  static vil_image_view<float> detect_edge_tangent(const vil_image_view<vxl_byte>& img,
                                                   double noise_multiplier,
                                                   double smooth,
                                                   bool automatic_threshold,
                                                   bool junctionp,
                                                   bool aggressive_junction_closure);

  // return image has three planes as in detect_edge_tangent
  // Canny edge detector returns edgel chains with a linear interpolator by default, replace this interpolator with a cubic one and read the edge tangents from this interpolator
  static vil_image_view<float> detect_edge_tangent_interpolated(const vil_image_view<vxl_byte>& img,
                                                                double noise_multiplier,
                                                                double smooth,
                                                                bool automatic_threshold,
                                                                bool junctionp,
                                                                bool aggressive_junction_closure);
  // return image has three planes as in detect_edge_tangent
  static vil_image_view<float> detect_edge_line_fitted(const vil_image_view<vxl_byte>& img,
                                                   double noise_multiplier,
                                                   double smooth,
                                                   bool automatic_threshold,
                                                   bool junctionp,
                                                   bool aggressive_junction_closure,
                                                   int min_fit_length, double rms_distance);

  // input is an edge image as the output of detect_edge_tangent(); output is line image:
  // plane 0 - line coefficient a --
  //                                |-- components of line normal vector
  // plane 1 - line coefficient b --
  //
  // plane 2 - line coefficient c
  static void convert_edge_image_to_line_image(vil_image_view<float>& edge_img, vil_image_view<float>& output_line_img);


  static void edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt);

  static vil_image_view<float> multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma);
  static void estimate_edge_prob_image(const vil_image_view<vxl_byte>& img_edge, vil_image_view<float>& img_edgeness, const int mask_size, const float mask_sigma);
  static vbl_array_2d<float> get_spherical_gaussian_kernel(const int size, const float sigma);

  static float convert_edge_statistics_to_probability(float edge_statistic, float n_normal, int dof);

  static void convert_true_edge_prob_to_edge_statistics(const vil_image_view<float>& img_tep,vil_image_view<float>& img_es);
};

#endif // sdet_img_edge_h_
