// This is brl/bseg/brip/brip_vil_float_ops.h
#ifndef brip_vil_float_ops_h_
#define brip_vil_float_ops_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief operations on image_view<float> operands
//
// These methods are similar to the VanDuc gevd_float_ops methods. However,
// they use vil_image_view<float> buffers rather than the old bufferxy
// structure. The purpose is to provide efficient foundational
// segmentation routines. They are not meant to be generic.
//
// \verbatim
//  Modifications
//   Feb 15 2003 Initial version
//   Jan 24 2004 Renamed to brip_vil_float_ops, to support moving from vil1 to vil
//   Dec 11 2011 - Peter Vanroose - replaced all unsigned char by vxl_byte
//                                  (before there was a mix of the two)
//                                  and all unsigned short by vxl_uint_16
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include <complex>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_rgb.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <brip/brip_roi_sptr.h>
#include <vxl_config.h> // for vxl_byte & vxl_uint_16

class brip_vil_float_ops
{
 public:
  ~brip_vil_float_ops() = default;

  //: convolves with the specified kernel
  static vil_image_view<float>
    convolve(vil_image_view<float> const& input,
             vbl_array_2d<float> const& kernel);

  //: helper to determine processing border required by Gaussian smoothing
  static unsigned gaussian_radius(const double sigma, const double fuzz=0.02);

  //: convolves with a Gaussian kernel
  // boundary_condition = {"none", "zeros", "const", "periodic", "reflect}
  // "none"
  // K                       ----*-------
  // in   ... ---------------------------
  // out  ... --------------------0000000
  // "zeros"
  // K                              ----*--------
  // in   ... ---------------------------000000000000...
  // out  ... ---------------------------
  // "const"
  // K                              ----*--------
  // in   ... --------------------------aaaaaaaaaaaaa...
  // out  ... ---------------------------
  // "periodic"
  // K                              ----*--------
  // in   abc...-------------------------abc...------..
  // out  ... ---------------------------
  // "reflect"
  // K                              ----*--------
  // in   ... -------------------...edcbabcde...
  // out  ... ---------------------------
  static vil_image_view<float> gaussian(vil_image_view<float> const& input,
                                        float sigma,
                                        std::string const& boundary_condition = "none",
                                        float fill = 0.0f);
  //: computes absolute value
  static vil_image_view<float>
    absolute_value(vil_image_view<float> const& input);

#ifdef VIL_CONVOLVE_WITH_MASK_EXISTS // TODO
  //: convolves with a Gaussian kernel and for a given mask
  static vil_image_view<float> gaussian(vil_image_view<float> const& input,
                                        float sigma,
                                        vil_image_view<float> mask);
#endif

  //: non-maximum suppression on a NxN neighborhood, with sub-pixel location
  static void non_maximum_suppression(vil_image_view<float> const& input,
                                      int n, float thresh,
                                      std::vector<float>& x_pos,
                                      std::vector<float>& y_pos,
                                      std::vector<float>& value);

  //: downsamples the input using the Bert-Adelson algorithm
  static vil_image_view<float>
    half_resolution(vil_image_view<float> const& input,
                    float filter_coef=0.359375f);

  //: interpolates the input using the Bert-Adelson algorithm
  static vil_image_view<float>
    double_resolution(vil_image_view<float> const& input,
                      float filter_coef=0.6f);

  //: sets values greater than \p thresh to specified \p level and the rest to zero
  static vil_image_view<float>
    threshold(vil_image_view<float> const & image,
              const float thresh, const float level = 255.0f);

  template <class T_from,class T_to>
  static void normalize_to_interval(const vil_image_view<T_from>& img_inp,
                                    vil_image_view<T_to>& img_out,
                                    float min,
                                    float max);

  //: sets absolute values greater than \p thresh to specified \p level
  static vil_image_view<float>
    abs_clip_to_level(vil_image_view<float> const & image,
                      const float thresh, const float level = 0.0f);

  //: Returns the gradient using a 3x3 kernel
  // \verbatim
  //         1  |-1  0  1|         1  |-1 -1 -1|
  //   Ix = --- |-1  0  1|   Iy = --- | 0  0  0|
  //         6  |-1  0  1|         6  | 1  1  1|
  // \endverbatim
  // Larger masks are computed by pre-convolving with a Gaussian
  static void gradient_3x3(vil_image_view<float> const& input,
                           vil_image_view<float>& grad_x,
                           vil_image_view<float>& grad_y);

  //: Returns the gradient magnitude using a 3x3 kernel
  static void gradient_mag_3x3(vil_image_view<float> const& input,
                               vil_image_view<float>& mag);

  //: Returns the gradient and its magnitude, using a 3x3 kernel
  static void gradient_mag_comp_3x3(vil_image_view<float> const& input,
                                    vil_image_view<float>& mag,
                                    vil_image_view<float>& grad_x,
                                    vil_image_view<float>& grad_y);

  //: Returns the Hessian using a 3x3 kernel
  // \verbatim
  //          1 | 1  -2  1|          1 |  1  1  1|         1  | 1  0 -1|
  //   Ixx = ---| 1  -2  1|   Iyy = ---| -2 -2 -2|  Ixy = --- | 0  0  0|
  //          3 | 1  -2  1|          3 |  1  1  1|         4  |-1  0  1|
  // \endverbatim
  // Larger masks are computed by pre-convolving with a Gaussian
  static void hessian_3x3(vil_image_view<float> const& input,
                          vil_image_view<float>& Ixx,
                          vil_image_view<float>& Ixy,
                          vil_image_view<float>& Iyy);

  static vil_image_view<float>
    beaudet(vil_image_view<float> const& Ixx,
            vil_image_view<float> const& Ixy,
            vil_image_view<float> const& Iyy,
            bool determinant = true);

  //:
  //  \p theta must be given in degrees.
  //  Scale invariant means that the response is independent of the
  //  \a sigma_y of the unrotated derivative operator, i.e. the direction
  //  of the derivative
  static void extrema_kernel_mask(float lambda0, float lambda1, float theta,
                                  vbl_array_2d<float>& kernel,
                                  vbl_array_2d<bool>& mask,
                                  float cutoff_percentage = 0.01f,
                                  bool scale_invariant = false);

  //: return a spherical mask and the coefficient matrix [kernel] for a symmetric gaussian distribution
  static void gaussian_kernel_mask(float lambda, vbl_array_2d<float>& kernel, vbl_array_2d<bool>& mask, float cutoff_percentage = 0.01f);

  //: return a square mask and the coefficient matrix [kernel] for a symmetric gaussian distribution
  static void gaussian_kernel_square_mask(float lambda, vbl_array_2d<float>& kernel, vbl_array_2d<bool>& mask, float cutoff_percentage = 0.01f);

  //: Compute the standard deviation of an operator response, given the image intensity standard deviation at each pixel
  static  vil_image_view<float>
    std_dev_operator(vil_image_view<float> const& sd_image,
                     vbl_array_2d<float> const& kernel);

  //: Compute the standard deviation of an operator response, given the image intensity standard deviation at each pixel
  //  Uses a modified formula to compute std_dev
  static vil_image_view<float>
    std_dev_operator_method2(vil_image_view<float> const& sd_image,
                             vbl_array_2d<float> const& kernel);

  //: a helper function for the extrema method: reverts angle to the range [-90, 90]
  static float extrema_revert_angle(float angle);


  //: Find anisotropic intensity extrema (Gaussian 2nd derivative). Theta is in degrees
  //  The effect of bright, mag_only and signed response are as follows:
  //  bright  mag_only signed_response        result
  // ------------------------------------------------
  //  false    false      false          dark extrema response
  //  false    false      true           signed output(full range)
  //  false    true       false          absolute mag output
  //  false    true       true           invalid
  //  true     false      false          bright extrema output
  //  true     false      true           signed output(full range)
  //  true     true       false          absolute mag output
  //  true     true       true           invalid
  //

  static vil_image_view<float> extrema(vil_image_view<float> const& input,
                                       float lambda0, float lambda1,
                                       float theta, bool bright = true,
                                       bool mag_only = false,
                                       bool output_response_mask = true,
                                       bool signed_response = false,
                                       bool scale_invariant = false,
                                       bool non_max_suppress = true,
                                       float cutoff_per = 0.01f);

  //: Find anisotropic intensity extrema at a range of orientations and return the maximal response at the best orientation.
  // \p theta_interval is in degrees
  //  If \p lambda0 == \p lambda1 then reduces to the normal extrema operator
  static vil_image_view<float> extrema_rotational(vil_image_view<float> const& input,
                                                  float lambda0, float lambda1,
                                                  float theta_interval,
                                                  bool bright = true,
                                                  bool mag_only = false,
                                                  bool signed_response = false,
                                                  bool scale_invariant = false,
                                                  bool non_max_suppress = true,
                                                  float cutoff_per = 0.01f);

  //: Compute the inscribed rectangle in an ellipse with largest $(1+h)(1+w)$.
  //  Needed for fast non-maximal suppression.
  //  \p theta is in degrees.
  static void max_inscribed_rect(float lambda0, float lambda1, float theta,
                                 float& u_rect, float& v_rect);

  //: Find intensity extrema using kernel decomposition.
  //  \p theta is in degrees.
  //  Image rotation is applied then separated u, v kernels produce the response.
  static vil_image_view<float> fast_extrema(vil_image_view<float> const& input,
                                            float lambda0, float lambda1,
                                            float theta, bool bright = true,
                                            bool mag_only = false,
                                            bool output_response_mask = true,
                                            bool signed_response = false,
                                            bool scale_invariant = false,
                                            bool non_max_suppress = true,
                                            float cutoff= 0.01f);


  // by default rotates between theta_init = 0 to theta_end = 180 for full rotation invariance
  // set theta_init and theta_end accordingly to detect rotational extrema response in a given angular range
  static vil_image_view<float>& resp,
    fast_extrema_rotational(vil_image_view<float> const& input,
                            float lambda0, float lambda1,
                            float theta_interval,
                            bool bright =false,
                            bool mag_only = false,
                            bool signed_response =true,
                            bool scale_invariant = true,
                            bool non_max_suppress = false,
                            float cutoff=0.01f,
                            float theta_init = 0.0f,
                            float theta_end = 180.0f);

  //: Ix.Ix-transpose gradient matrix elements for an NxN region ($N = 2n+1$)
  // That is,
  // \verbatim
  //                        _                           _
  //                       | (dI/dx)^2    (dI/dx)(dI/dy) |
  //                       |                             |
  //  A = Sum(neighborhood)|                             |
  //                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
  //                       |_                           _|
  // \endverbatim
  // over a $2n+1 ~\times~ 2n+1$ neighborhood.

  static void grad_matrix_NxN(vil_image_view<float> const& input, unsigned n,
                              vil_image_view<float>& IxIx,
                              vil_image_view<float>& IxIy,
                              vil_image_view<float>& IyIy);

  //: Tr(IxIx.transpose) for a NxN region ($N = 2n+1$)
  static  vil_image_view<float>
    trace_grad_matrix_NxN(vil_image_view<float> const& input, unsigned n);

  //: Computes the Harris corner measure
  static vil_image_view<float> harris(vil_image_view<float> const& IxIx,
                                      vil_image_view<float> const& IxIy,
                                      vil_image_view<float> const& IyIy,
                                      double scale=0.04);

  //: Computes the conditioning of the $2n+1 ~\times~ 2n+1$ gradient neighborhood
  // Compute the sqrt of the product of the eigenvalues of the
  // gradient matrix over a 2n+1 x 2n+1 neighborhood
  // That is,
  // \verbatim
  //                        _                           _
  //                       | (dI/dx)^2    (dI/dx)(dI/dy) |
  //                       |                             |
  //  A = Sum(neighborhood)|                             |
  //                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
  //                       |_                           _|
  // \endverbatim
  // The output image is sqrt(lamba_1*lambda_2) where lambda_i are the eigenvalues
  static vil_image_view<float>
    sqrt_grad_singular_values(vil_image_view<float>& input, int n);

  //:
  // Returns the image with max scale values
  static vil_image_view<float> max_scale_trace(const vil_image_view<float>& input,
                                               float min_scale,
                                               float max_scale,
                                               float scale_inc);

  //:
  // Exactly same as max_scale_trace,
  // only return the image with actual trace values at max scales instead of the image with max scale values
  static vil_image_view<float> max_scale_trace_value(const vil_image_view<float>& input,
                                                     float min_scale,
                                                     float max_scale,
                                                     float scale_inc);

  //: computes Lucas-Kanade optical flow on a $2n+1$ neighborhood
  static void Lucas_KanadeMotion(vil_image_view<float>& current_frame,
                                 vil_image_view<float>& previous_frame,
                                 int n, double thresh,
                                 vil_image_view<float>& vx,
                                 vil_image_view<float>& vy);

  //: computes velocity of a region(view) using Lucas Kanade
  static void
    lucas_kanade_motion_on_view(vil_image_view<float> const& curr_frame,
                                vil_image_view<float> const& prev_frame,
                                const double thresh,
                                float& vx,
                                float& vy);

  //: computes velocity of a region(view) using correlation
  static void
    velocity_by_correlation(vil_image_view<float> const& curr_image,
                            vil_image_view<float> const& prev_region,
                            const unsigned start_i, const unsigned end_i,
                            const unsigned start_j, const unsigned end_j,
                            const unsigned zero_i, const unsigned zero_j,
                            float& vx,
                            float& vy);

  //: computes optical flow using Horn and Schunck's method
  static int Horn_SchunckMotion(vil_image_view<float> const & current_frame,
                                vil_image_view<float> const& previous_frame,
                                vil_image_view<float>& vx,
                                vil_image_view<float>& vy,
                                const float alpha_coef=10000.0f,
                                const int no_of_iterations=5);

  //: fills a border of width \p w on left and right of image with value
  static void fill_x_border(vil_image_view<float>& image, unsigned w, float value);

  //: fills a border of height \p h on top and bottom of image with value
  static void fill_y_border(vil_image_view<float>& image, unsigned h, float value);

  //: converts a float image to a byte value range
  static vil_image_view<vxl_byte>
    convert_to_byte(vil_image_view<float> const& image);

  //: converts a float image to a byte value range within a specified range
  static vil_image_view<vxl_byte>
    convert_to_byte(vil_image_view<float> const& image,
                    float min_val, float max_val);

  //: converts an unsigned short (16-bit) image to a byte value range within a specified range
  static vil_image_view<vxl_byte>
    convert_to_byte(vil_image_view<vxl_uint_16> const& image,
                    vxl_uint_16 min_val, vxl_uint_16 max_val);

  //: converts a generic image to a byte image.
  // Use this instead of convert_to_grey
  static vil_image_view<vxl_byte>
    convert_to_byte(vil_image_resource_sptr const& image);

  //: converts a float image to an unsigned short (16-bit) image within a range
  // Use this instead of convert_to_grey
  static vil_image_view<vxl_uint_16>
    convert_to_short(vil_image_view<float> const& image,
                     float min_val, float max_val);

  //: converts a float image to an unsigned short (16-bit) image.
  // Range is determined automatically
  static vil_image_view<vxl_uint_16>
    convert_to_short(vil_image_view<float> const& image);

  //: converts a generic image to an unsigned short (16-bit) image
  // Use this instead of convert_to_grey
  static vil_image_view<vxl_uint_16>
    convert_to_short(vil_image_resource_sptr const& image);

  //: converts a vil_image_resource to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_resource const& image);

  //: converts a vil_image_resource to a float image (preferred interface)
  static vil_image_view<float>
    convert_to_float(vil_image_resource_sptr const& image)
  { return brip_vil_float_ops::convert_to_float(*image); }

  static vil_image_view<float>
    convert_to_float(vil_image_view<vxl_byte> const& image);

  static vil_image_view<float>
    convert_to_float(vil_image_view<vxl_uint_16> const& image);

  //: converts a byte image to a bool image
  static vil_image_view<bool>
    convert_to_bool(vil_image_view<vxl_byte> const& image);

  //: converts an RGB image to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_view<vil_rgb<vxl_byte> > const& image);

  //: convert a single RGB pixel to (I,H,S)
  //  Foley and Van Damm
  static void rgb_to_ihs(vil_rgb<vxl_byte> const& rgb,
                         float& i, float& h, float& s);

  //: convert a single RGB pixel to (I,H,S) with the following map
  // V. Tsai
  // IEEE Transactions on Geoscience and Remote Sensing,
  // VOL. 44, NO. 6, JUNE 2006
  //
  //  I = (R+G+B)/3
  //  V1 = -R/sqrt(6) - G/sqrt(6) + B*sqrt(2/3)
  //  V2 = R/sqrt(6) - 2*G/sqrt(6)
  //  S = sqrt(V1^2 + V2^2)
  //  H = atan2(V2/V1)
  //
  static void rgb_to_ihs_tsai(vil_rgb<vxl_byte> const& rgb,
                              float& i, float& h, float& s);


  //: convert a single (I,H,S) pixel to RGB
  static void ihs_to_rgb(vil_rgb<vxl_byte>& rgb,
                         const float i, const float h, const float s);

  //: converts a byte-pixel color image to float (I,H,S) image triple
  static void
    convert_to_IHS(vil_image_view<vil_rgb<vxl_byte> > const& image,
                   vil_image_view<float>& I,
                   vil_image_view<float>& H,
                   vil_image_view<float>& S);



  //: converts a byte-pixel image to float (I,H,S) image triple
  static void
    convert_to_IHS(vil_image_view<vxl_byte> const& image,
                   vil_image_view<float>& I,
                   vil_image_view<float>& H,
                   vil_image_view<float>& S);

  //: somewhat different map to IHS, ratio = (H+1)/(I+1) indicating shadows
  static void
    convert_to_IHS_tsai(vil_image_view<vxl_byte > const& image,
                        vil_image_view<float>& I,
                        vil_image_view<float>& H,
                        vil_image_view<float>& S,
                        vil_image_view<float>& ratio);

//: display (I,H,S) image triple as RGB (no conversion from IHS to RGB!)
  static void
    display_IHS_as_RGB(vil_image_view<float> const& I,
                       vil_image_view<float> const& H,
                       vil_image_view<float> const& S,
                       vil_image_view<vil_rgb<vxl_byte> >& image);

#if 0 // TODO ?
  //: converting IHS to RGB
  static void
    convert_IHS_as_RGB(vil_image_view<float> const& I,
                       vil_image_view<float> const& H,
                       vil_image_view<float> const& S,
                       vil_image_view<vil_rgb<vxl_byte> >& image);
#endif // 0

  //: Create a byte-pixel color image from multiple view channels (R,G,B)
  // All views have to have the same array dimensions
  static vil_image_view<vil_rgb<vxl_byte> >
    combine_color_planes(vil_image_view<vxl_byte> const& R,
                         vil_image_view<vxl_byte> const& G,
                         vil_image_view<vxl_byte> const& B);

  //: Create a byte-pixel color image from multiple resource channels (R,G,B)
  // Images do not have to be the same size arrays
  static vil_image_view<vil_rgb<vxl_byte> >
    combine_color_planes(vil_image_resource_sptr const& R,
                         vil_image_resource_sptr const& G,
                         vil_image_resource_sptr const& B);

  //: converts a generic (byte-pixel RGB) image to greyscale
  static vil_image_view<vxl_byte>
    convert_to_grey(vil_image_resource const& img);

  //: loads a $2n+1 ~\times~ 2n+1$ convolution kernel
  // Assumes a square kernel with odd dimensions, i.e., $w,h = 2n+1$
  // format:
  // \verbatim
  //     n
  //     scale
  //     k00  k01  ... k02n
  //           ...
  //     k2n0 k2n1 ... k2n2n
  // \endverbatim
  static vbl_array_2d<float> load_kernel(std::string const& file);

  //: compute basis images for a set of input images
  static void basis_images(std::vector<vil_image_view<float> > const& input_images,
                           std::vector<vil_image_view<float> >& basis);

  //: compute the Fourier transform using the vnl FFT algorithm
  static bool fourier_transform(vil_image_view<float> const& input,
                                vil_image_view<float>& mag,
                                vil_image_view<float>& phase);

  //: compute the inverse Fourier transform using the vnl FFT algorithm
  static bool inverse_fourier_transform(vil_image_view<float> const& mag,
                                        vil_image_view<float> const& phase,
                                        vil_image_view<float>& output);

  //: resize to specified dimensions.
  //  Fill with zeros if output is larger
  static void resize(vil_image_view<float> const& input,
                     unsigned width, unsigned height,
                     vil_image_view<float>& output);

  //: resize to closest power of two larger dimensions than the input.
  //  Fill with zeros if output is larger
  static bool
    resize_to_power_of_two(vil_image_view<float> const& input,
                           vil_image_view<float>& output);

  //: filter the input image with a Gaussian blocking filter
  static bool
    spatial_frequency_filter(vil_image_view<float> const& input,
                             float dir_fx, float dir_fy,
                             float f0, float radius,
                             bool output_fourier_mag,
                             vil_image_view<float>& output);

  //: 2x2 bilinear interpolation of image at specified location
  // Neighborhood:
  // \verbatim
  //      xr
  //   yr 0  x
  //      x  x
  // \endverbatim
  static double
    bilinear_interpolation(vil_image_view<float> const& input,
                           double x, double y);

  //: map the input to the output by a homography.
  // \note if the output size is fixed then only the corresponding
  // input image space is transformed.
  static bool homography(vil_image_view<float> const& input,
                         vgl_h_matrix_2d<double> const& H,
                         vil_image_view<float>& output,
                         bool output_size_fixed = false,
                         float output_fill_value = 0.0f);

  //: rotate the input image counter-clockwise about the image origin
  static vil_image_view<float> rotate(vil_image_view<float> const& input,
                                      double theta_deg);

  //: extract a region of interest.
  // If \p roi does not overlap input, return false
  static bool chip(vil_image_view<float> const& input,
                   vsol_box_2d_sptr const& roi, vil_image_view<float>& chip);

  //: convert image resource to a chip of equivalent pixel type
  // If \p roi does not overlap input, return false
  static bool chip(vil_image_resource_sptr const& image,
                   brip_roi_sptr const& roi,
                   vil_image_resource_sptr & chip);

  //: chip multiple images.
  // Must be all the same dimensions
  // If \p roi does not overlap input, return false
  static bool chip(std::vector<vil_image_resource_sptr> const& images,
                   brip_roi_sptr const& roi,
                   std::vector<vil_image_resource_sptr>& chips);

  //: compute the average of the image intensity within the specified region
  static float average_in_box(vil_image_view<float> const& v,
                              vgl_box_2d<double> const& box);

  //: scan a polygon and return the pixel values as well as max min
  static std::vector<float> scan_region(const vil_image_resource_sptr& img,
                                       const vgl_polygon<double>& poly,
                                       float& min,
                                       float& max);
  //: cross-correlate two images at a given sub-pixel location
  static float cross_correlate(vil_image_view<float> const& image1,
                               vil_image_view<float> const& image2,
                               float x, float y, int radius = 5,
                               float intensity_thresh=25.0f);

  //: cross_correlate two images using running sums
  static bool cross_correlate(vil_image_view<float> const& image1,
                              vil_image_view<float> const& image2,
                              vil_image_view<float>& out,
                              int radius = 5, float intensity_thresh=25.0f);

  //: Compute the intensity entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_i(const unsigned i, const unsigned j,
                         const unsigned i_radius,
                         const unsigned j_radius,
                         vil_image_view<float> const& intensity,
                         const float range = 255.0f, const unsigned bins = 16);

  //: Compute the gradient entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_g(const unsigned i, const unsigned j,
                         const unsigned i_radius,
                         const unsigned j_radius,
                         vil_image_view<float> const& gradx,
                         vil_image_view<float> const& grady,
                         const float range = 360.0f, const unsigned bins = 8);

  //: Compute the hue and saturation entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_hs(const unsigned i, const unsigned j,
                          const unsigned i_radius,
                          const unsigned j_radius,
                          vil_image_view<float> const& hue,
                          vil_image_view<float> const& sat,
                          const float range = 360.0f, const unsigned bins = 8);

  //: Compute the entropy of the specified region about each pixel
  static vil_image_view<float> entropy(const unsigned i_radius,
                                       const unsigned j_radius,
                                       const unsigned step,
                                       vil_image_resource_sptr const& img,
                                       const float sigma = 1.0f,
                                       const unsigned bins = 16,
                                       const bool intensity = true,
                                       const bool gradient = true,
                                       const bool ihs = false);

  //: Compute the intensity minfo of a region about the specified pixel
  //  No bounds check
  static float minfo_i(const unsigned i0, const unsigned j0,
                       const unsigned i1, const unsigned j1,
                       const unsigned i_radius,
                       const unsigned j_radius,
                       vil_image_view<float> const& intensity0,
                       vil_image_view<float> const& intensity1,
                       const float range = 255.0f, const unsigned bins = 16);

  //: Compute the gradient minfo of a region about the specified pixel
  //  No bounds check
  static float minfo_g(const unsigned i0, const unsigned j0,
                       const unsigned i1, const unsigned j1,
                       const unsigned i_radius,
                       const unsigned j_radius,
                       vil_image_view<float> const& gradx0,
                       vil_image_view<float> const& grady0,
                       vil_image_view<float> const& gradx1,
                       vil_image_view<float> const& grady1,
                       const float range = 360.0f, const unsigned bins = 8);

  //: Compute the hue and saturation minfo of a region about the specified pixel
  //  No bounds check
  static float minfo_hs(const unsigned i0, const unsigned j0,
                        const unsigned i1, const unsigned j1,
                        const unsigned i_radius,
                        const unsigned j_radius,
                        vil_image_view<float> const& hue0,
                        vil_image_view<float> const& sat0,
                        vil_image_view<float> const& hue1,
                        vil_image_view<float> const& sat1,
                        const float range = 360.0f, const unsigned bins = 8);

  //: Compute the minfo of the specified region about each pixel
  static bool minfo(const unsigned i_radius,
                    const unsigned j_radius,
                    const unsigned step,
                    vil_image_resource_sptr const& img0,
                    vil_image_resource_sptr const& img1,
                    vil_image_view<float>& MI0,
                    vil_image_view<float>& MI1,
                    const float sigma = 1.0f,
                    const bool intensity = true,
                    const bool gradient = true,
                    const bool ihs = false);

  //  ===  Arithmetic operations  ===

  //: Blur the image with an NxN averaging filter
  static vil_image_view<float> average_NxN(vil_image_view<float> const& img, int N);

  //: Add two images from a general resource (forces types to be the same)
  static vil_image_resource_sptr sum(vil_image_resource_sptr const& img0,
                                     vil_image_resource_sptr const& img1);

  //: subtracts \p image_1 from \p image_2
  static vil_image_view<float> difference(vil_image_view<float> const& image_1,
                                          vil_image_view<float> const& image_2);

  //: subtract two generic images, return img0-img1 (forces types to the same)
  static vil_image_resource_sptr difference(vil_image_resource_sptr const& img0,
                                            vil_image_resource_sptr const& img1);

  //: negate an image returning the same pixel type (only greyscale)
  static vil_image_resource_sptr negate(vil_image_resource_sptr const& imgr);

  //: Color order operator, output an index based on RGB intensity order
  // It has been observed that color order is somewhat invariant to illumination
  // The tolerance determines if two color bands are too close to determine order,
  // i.e. they should be considered equal instead
  // the two relations being considered  are <, > and =, so the relationship
  // graph looks like:
  // \verbatim
  //         G
  //       /   \.
  //   > < =   > < =
  //    /         \.
  //  R  - > < = -  B
  // \endverbatim
  // Thus, there are three graph edges with each of three possible labels or
  // 9 possible order codes. An easy coding scheme is to use the top 6 bits of
  // the byte output pixel. The relationship is encoded as states of bit pairs
  // \verbatim
  // Color relations  R*G  R*B  G*B    * indicates > < = (1,2,3)
  // Bit indices      7,6  5,4  3,2
  // \endverbatim
  static vil_image_view<vxl_byte>
    color_order(vil_image_view<float> const& color_image, float eq_tol);

  //  ===  Internals  ===

 private:

  //: find if the center pixel of a neighborhood is the maximum value
  static bool local_maximum(vbl_array_2d<float> const& nighborhood,
                            int n, float& value);

  //: find the sub-pixel offset to the maximum using a 3x3 quad interpolation
  static void interpolate_center(vbl_array_2d<float> const& neighborhood,
                                 float& dx, float& dy);

  //: sub-sample a 1-d array using the Bert-Adelson algorithm
  static void half_resolution_1d(const float* input, unsigned n,
                                 float k0, float k1, float k2, float* output);

  //: interpolate a 1-d array using the Bert-Adelson algorithm
  static void double_resolution_1d(const float* input, const unsigned n_input,
                                   const float k0, const float k1, const float k2,
                                   float* output);

  //: One-dimensional fft
  static bool fft_1d(int dir, int m, double* x, double* y);

  //: Two-dimensional fft
  static bool fft_2d(vnl_matrix<std::complex<double> >& c, int nx,int ny,int dir);

  //: Transform the fft coefficients from/to fft/frequency order(self inverse).
  static void ftt_fourier_2d_reorder(vnl_matrix<std::complex<double> > const& F1,
                                     vnl_matrix<std::complex<double> >& F2);

  //: Blocking filter function
  static float gaussian_blocking_filter(float dir_fx, float dir_fy,
                                        float f0, float radius,
                                        float fx, float fy);

  //: u-coordinate of an ellipse defined by lambda0, lambda1 and theta, vs. phi
  static float elu(float phi, float lamda0, float lambda1, float theta);
  //: v-coordinate of an ellipse defined by lambda0, lambda1 and theta, vs. phi
  static float elv(float phi, float lamda0, float lambda1, float theta);

  //: Default constructor is private
  brip_vil_float_ops() = default;
};

template <class T_inp,class T_out>
void brip_vil_float_ops::normalize_to_interval(const vil_image_view<T_inp>& img_inp,
                                               vil_image_view<T_out>& img_out,
                                               float min,
                                               float max)
{
  assert(min<max);
  T_inp min_inp;
  T_inp max_inp;
  vil_math_value_range<T_inp>(img_inp,min_inp,max_inp);

  if (min_inp >= max_inp) {
    img_out.fill(T_out(0));
    return;
  }

  float min_inp_f = (float)min_inp;
  float max_inp_f = (float)max_inp;
  float scale = (max-min)/(max_inp_f-min_inp_f);

  img_out.set_size(img_inp.ni(),img_inp.nj(),1);
  for (unsigned i=0; i<img_out.ni(); i++) {
    for (unsigned j=0; j<img_out.nj(); j++) {
      float inp_val = (float)img_inp(i,j);
      float out_val = (inp_val-min_inp_f)*scale;
      img_out(i,j) = T_out(out_val);
    }
  }
}

#endif // brip_vil_float_ops_h_
