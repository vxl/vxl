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
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_complex.h>
#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <brip/brip_roi_sptr.h>

class brip_vil_float_ops
{
 public:
  ~brip_vil_float_ops() {}

  //: convolves with the specified kernel
  static vil_image_view<float>
    convolve(vil_image_view<float> const& input,
             vbl_array_2d<float> const& kernel);

  //: helper to determine processing border required by Gaussian smoothing
  static unsigned gaussian_radius(const double sigma, const double fuzz=0.02);

  //: convolves with a Gaussian kernel
  static vil_image_view<float> gaussian(vil_image_view<float> const& input,
                                        float sigma);

#if 1 // #ifdef VIL_CONVOLVE_WITH_MASK_EXISTS // TODO
  //: convolves with a Gaussian kernel and for a given mask
  static vil_image_view<float> gaussian(vil_image_view<float> const& input,
                                        float sigma,
                                        vil_image_view<float> mask);
#endif

  //: non-maximum suppression on a NxN neighborhood, with sub-pixel location
  static void non_maximum_suppression(vil_image_view<float> const& input,
                                      int n, float thresh,
                                      vcl_vector<float>& x_pos,
                                      vcl_vector<float>& y_pos,
                                      vcl_vector<float>& value);

  //: downsamples the input using the Bert-Adelson algorithm
  static vil_image_view<float>
    half_resolution(vil_image_view<float> const& input,
                    float filter_coef=0.359375f);


  //: interpolates the input using the Bert-Adelson algorithm
  static vil_image_view<float>
    double_resolution(vil_image_view<float> const& input,
                      float filter_coef=0.6f);


  //: subtracts image_1 from image_2
  static vil_image_view<float>
    difference(vil_image_view<float> const& image_1,
               vil_image_view<float> const& image_2);

  //: sets values greater than thresh to specified level and the rest to zero
  static vil_image_view<float>
    threshold(vil_image_view<float> const & image,
              const float thresh, const float level = 255.0);

  //: sets absolute values greater than thresh to specified level
  static vil_image_view<float>
    abs_clip_to_level(vil_image_view<float> const & image,
                      const float thresh, const float level = 0.0);

  static vil_image_view<float>
    average_NxN(vil_image_view<float> const & img, int N);

  //: The gradient using a 3x3 kernel
  static void gradient_3x3(vil_image_view<float> const& input,
                           vil_image_view<float>& grad_x,
                           vil_image_view<float>& grad_y);

  static void hessian_3x3(vil_image_view<float> const& input,
                          vil_image_view<float>& Ixx,
                          vil_image_view<float>& Ixy,
                          vil_image_view<float>& Iyy);

  static vil_image_view<float>
    beaudet(vil_image_view<float> const& Ixx,
            vil_image_view<float> const& Ixy,
            vil_image_view<float> const& Iyy);

  //: IxIx.transpose gradient matrix elements for a NxN region(N = 2n+1)
  static void grad_matrix_NxN(vil_image_view<float> const& input, unsigned n,
                              vil_image_view<float>& IxIx,
                              vil_image_view<float>& IxIy,
                              vil_image_view<float>& IyIy);

  //: Tr(IxIx.transpose) for a NxN region, N = 2n+1)
  static  vil_image_view<float>
    trace_grad_matrix_NxN(vil_image_view<float> const& input, unsigned n);

  //: Computes the Harris corner measure
  static vil_image_view<float> harris(vil_image_view<float> const& IxIx,
                                      vil_image_view<float> const& IxIy,
                                      vil_image_view<float> const& IyIy,
                                      double scale=0.04);

  //: computes the conditioning of the 2n+1 x 2n+1 gradient neigborhood
  static vil_image_view<float>
    sqrt_grad_singular_values(vil_image_view<float>& input, int n);

  static vil_image_view<float> max_scale_trace(vil_image_view<float> input,
                                               float min_scale,
                                               float max_scale,
                                               float scale_inc);

  //: computes Lucas-Kanade optical flow on a 2n+1 neighborhood
  static void Lucas_KanadeMotion(vil_image_view<float>& current_frame,
                                 vil_image_view<float>& previous_frame,
                                 int n, double thresh,
                                 vil_image_view<float>& vx,
                                 vil_image_view<float>& vy);

  //: computes optical flow using Horn & Schunck's method
  static int Horn_SchunckMotion(vil_image_view<float> const & current_frame,
                                vil_image_view<float> const& previous_frame,
                                vil_image_view<float>& vx,
                                vil_image_view<float>& vy,
                                const float alpha_coef=10000.0f,
                                const int no_of_iterations=5);

  //: fills a border of width w on left and right of image with value
  static void fill_x_border(vil_image_view<float>& image, unsigned w, float value);

  //: fills a border of width h on top and bottom of image with value
  static void fill_y_border(vil_image_view<float>& image, unsigned h, float value);

  //: converts a float image to a byte value range
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_view<float> const& image);

  //: converts a float image to a byte value range within a specified range
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_view<float> const& image,
                    float min_val, float max_val);

  //: converts an unsigned short image to a byte value range within a specified range
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_view<unsigned short> const& image,
                    unsigned short min_val, unsigned short max_val);

  //: converts a generic image to a byte image. Use this instead of convert_to_grey
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_resource_sptr const& image);

  //: converts a float image to an unsigned short image within a range
  static vil_image_view<unsigned short>
    convert_to_short(vil_image_view<float> const& image,
                     float min_val, float max_val);
  //: converts a float image to an unsigned short image.
  // range determined automatically
  static vil_image_view<unsigned short>
    convert_to_short(vil_image_view<float> const& image);

  //: converts a generic image to an unsigned short image
  static vil_image_view<unsigned short>
    convert_to_short(vil_image_resource_sptr const& image);

  //: converts a vil_image_resource to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_resource const& image);

  //: converts a vil_image_resource to a float image (preferred interface)
  static vil_image_view<float>
    convert_to_float(vil_image_resource_sptr const& image)
    {return brip_vil_float_ops::convert_to_float(*image);}

  static vil_image_view<float>
    convert_to_float(vil_image_view<unsigned char> const& image);

  static vil_image_view<float>
    convert_to_float(vil_image_view<unsigned short> const& image);

  //: converts a byte image to a bool image
  static vil_image_view<bool>
    convert_to_bool(vil_image_view<unsigned char> const& image);

  //: converts an RGB image to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_view<vil_rgb<vxl_byte> > const& image);

  //: convert a color image to float IHS images
  static void
    convert_to_IHS(vil_image_view<vil_rgb<vxl_byte> > const& image,
                   vil_image_view<float>& I,
                   vil_image_view<float>& H,
                   vil_image_view<float>& S);
  static void
    convert_to_IHS(vil_image_view<unsigned char> const& image,
                   vil_image_view<float>& I,
                   vil_image_view<float>& H,
                   vil_image_view<float>& S);

  //: display IHS images as RGB (not conversion from IHS to RGB)
  static void
    display_IHS_as_RGB(vil_image_view<float> const& I,
                       vil_image_view<float> const& H,
                       vil_image_view<float> const& S,
                       vil_image_view<vil_rgb<vxl_byte> >& image);

  //: Create a color image from multiple view channels
  // all views have to have the same array dimensions
  static vil_image_view<vil_rgb<vxl_byte> >
    combine_color_planes(vil_image_view<unsigned char> const& R,
                         vil_image_view<unsigned char> const& G,
                         vil_image_view<unsigned char> const& B);

  //: Create a unsigned char color image from multiple resource channels
  // images do not have to be the same size arraysp
  static vil_image_view<vil_rgb<vxl_byte> >
   combine_color_planes(vil_image_resource_sptr const& R,
                        vil_image_resource_sptr const& G,
                        vil_image_resource_sptr const& B);


  //: converts a generic image to greyscale (RGB<unsigned char>)
  static vil_image_view<unsigned char>
    convert_to_grey(vil_image_resource const& img);

  //: loads a 2n+1 x 2n+1 convolution kernel (see .cxx for file format)
  static vbl_array_2d<float> load_kernel(vcl_string const& file);

  //: compute basis images for a set of input images
  static void basis_images(vcl_vector<vil_image_view<float> > const& input_images,
                           vcl_vector<vil_image_view<float> >& basis);

  //: compute the Fourier transform using the vnl FFT algorithm
  static bool fourier_transform(vil_image_view<float> const& input,
                                vil_image_view<float>& mag,
                                vil_image_view<float>& phase);

  //: compute the inverse Fourier transform using the vnl FFT algorithm
  static bool inverse_fourier_transform(vil_image_view<float> const& mag,
                                        vil_image_view<float> const& phase,
                                        vil_image_view<float>& output);

  //: resize to specified dimensions, fill with zeros if output is larger
  static void resize(vil_image_view<float> const& input,
                     unsigned width, unsigned height,
                     vil_image_view<float>& output);

  //: resize to closest power of two larger dimensions than the input
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
  static double
    bilinear_interpolation(vil_image_view<float> const& input,
                           double x, double y);

  //: map the input to the output by a homography.
  // \note if the output size is fixed then only the corresponding
  // input image space is transformed.
  static bool homography(vil_image_view<float> const& input,
                         vgl_h_matrix_2d<double>const& H,
                         vil_image_view<float>& output,
                         bool output_size_fixed = false,
                         float output_fill_value = 0.0f);

  //: rotate the input image counter-clockwise about the image origin
  static vil_image_view<float> rotate(vil_image_view<float> const& input,
                                      double theta_deg);

  //: extract a region of interest. If roi does not overlap input, return false
  static bool chip(vil_image_view<float> const& input,
                   vsol_box_2d_sptr const& roi, vil_image_view<float>& chip);

  //: convert image resource to a chip of equivalent pixel type
  static bool chip(vil_image_resource_sptr const& image,
                   brip_roi_sptr const& roi,
                   vil_image_resource_sptr & chip);

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

  //: convert a single i,h,s  pixel to rgb
  static void ihs_to_rgb(vil_rgb<vxl_byte>& rgb,
                         const float i, const float h, const float s);

  //: convert a single rgb pixel to ihs
  static void rgb_to_ihs(vil_rgb<vxl_byte> const& rgb,
                         float& i, float& h, float& s);

  //: Compute the intensity entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_i(const unsigned i, const unsigned j,
                         const unsigned i_radius,
                         const unsigned j_radius,
                         vil_image_view<float> const& intensity,
                         const float range = 255.0, const unsigned bins = 16);

  //: Compute the gradient entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_g(const unsigned i, const unsigned j,
                         const unsigned i_radius,
                         const unsigned j_radius,
                         vil_image_view<float> const& gradx,
                         vil_image_view<float> const& grady,
                         const float range = 360.0, const unsigned bins = 8);

  //: Compute the hue and saturation entropy of a region about the specified pixel
  //  No bounds check
  static float entropy_hs(const unsigned i, const unsigned j,
                          const unsigned i_radius,
                          const unsigned j_radius,
                          vil_image_view<float> const& hue,
                          vil_image_view<float> const& sat,
                          const float range = 360.0, const unsigned bins = 8);

  //: Compute the entropy of the specified region about each pixel
  static vil_image_view<float> entropy(const unsigned i_radius,
                                       const unsigned j_radius,
                                       const unsigned step,
                                       vil_image_resource_sptr const& img,
                                       const float sigma = 1.0f,
                                       const bool intensity = true,
                                       const bool gradient = true,
                                       const bool ihs = false);

  //no bounds check
  static float minfo_i(const unsigned i0, const unsigned j0,
                       const unsigned i1, const unsigned j1,
                       const unsigned i_radius,
                       const unsigned j_radius,
                       vil_image_view<float> const& intensity0,
                       vil_image_view<float> const& intensity1,
                       const float range = 255.0, const unsigned bins = 16);

  //:Compute the gradient minfo of a region about the specified pixel
  // No bounds check
  static float minfo_g(const unsigned i0, const unsigned j0,
                       const unsigned i1, const unsigned j1,
                       const unsigned i_radius,
                       const unsigned j_radius,
                       vil_image_view<float> const& gradx0,
                       vil_image_view<float> const& grady0,
                       vil_image_view<float> const& gradx1,
                       vil_image_view<float> const& grady1,
                       const float range = 360.0, const unsigned bins = 8);

  //:Compute the hue and saturation minfo of a region about the specified pixel
  // No bounds check
  static float minfo_hs(const unsigned i0, const unsigned j0,
                        const unsigned i1, const unsigned j1,
                        const unsigned i_radius,
                        const unsigned j_radius,
                        vil_image_view<float> const& hue0,
                        vil_image_view<float> const& sat0,
                        vil_image_view<float> const& hue1,
                        vil_image_view<float> const& sat1,
                        const float range = 360.0, const unsigned bins = 8);

  //:compute the minfo of the specified region about each pixel
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

  // Arithmetic operations

  //: Add two images from a general resource (forces types to be the same)
  static vil_image_resource_sptr sum(vil_image_resource_sptr const& img0,
                                     vil_image_resource_sptr const& img1);

  //: subtract two generic images, return img0-img1 (forces types to the same)
  static vil_image_resource_sptr difference(vil_image_resource_sptr const& img0,
                                            vil_image_resource_sptr const& img1);

  //: negate an image returning the same pixel type (only greyscale)
  static vil_image_resource_sptr negate(vil_image_resource_sptr const& imgr);
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

  //: One dimensional fft
  static bool fft_1d(int dir, int m, double* x, double* y);

  //: Two dimensonal fft
  static bool fft_2d(vnl_matrix<vcl_complex<double> >& c, int nx,int ny,int dir);

  //: Transform the fft coefficients from/to fft/frequency order(self inverse).
  static void ftt_fourier_2d_reorder(vnl_matrix<vcl_complex<double> > const& F1,
                                     vnl_matrix<vcl_complex<double> >& F2);

  //: Blocking filter function
  static float gaussian_blocking_filter(float dir_fx, float dir_fy,
                                        float f0, float radius,
                                        float fx, float fy);
#if 0 // TODO ?
  //: converting IHS to RGB
  static void
    convert_IHS_as_RGB(vil_image_view<float> const& I,
                       vil_image_view<float> const& H,
                       vil_image_view<float> const& S,
                       vil_image_view<vil_rgb<vxl_byte> >& image);
#endif // 0

  //: Default constructor is private
  brip_vil_float_ops() {}
};

#endif // brip_vil_float_ops_h_
