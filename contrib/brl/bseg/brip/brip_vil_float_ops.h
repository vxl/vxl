//--*-c++-*--
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
//   Initial version February 15, 2003
//   Renamed to brip_vil_float_ops January 24, 2004, In order to support
//   moving from vil1 to vil.
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
class brip_vil_float_ops
{
 public:
  ~brip_vil_float_ops() {}

  //: convolves with the specified kernel
  static vil_image_view<float>
    convolve(vil_image_view<float> const & input,
             vbl_array_2d<float> const & kernel);

  //: convolves with a Gaussian kernel
  static vil_image_view<float>
    gaussian(vil_image_view<float> const & input, float sigma);

  //: non-maximum suppression on a NxN neighborhood, with sub-pixel location
  static void non_maximum_suppression(vil_image_view<float> const & input,
                                      const int n,
                                      const float thresh,
                                      vcl_vector<float>& x_pos,
                                      vcl_vector<float>& y_pos,
                                      vcl_vector<float>& value);

  //:downsamples the input using the Bert-Adelson algorithm
  static vil_image_view<float>
  half_resolution(vil_image_view<float> const & input,
                  float filter_coef=0.359375);

#if 0
  //: interpolates the input using the Bert-Adelson algorithm
  static vil_image_view<float>
    double_resolution(vil_image_view<float> const & input,
                      float filter_coef=0.359375);
#endif

  //: subtracts image_1 from image_2
  static vil_image_view<float>
    difference(vil_image_view<float> const & image_1,
               vil_image_view<float> const & image_2);

  //: sets absolute values greater than thresh to specified level
  static vil_image_view<float>
    abs_clip_to_level(vil_image_view<float> const & image,
                      const float thresh, const float level = 0.0);

  //: The gradient using a 3x3 kernel
  static void gradient_3x3(vil_image_view<float> const & input,
                           vil_image_view<float>& grad_x,
                           vil_image_view<float>& grad_y);

  static void hessian_3x3(vil_image_view<float> const & input,
                          vil_image_view<float>& Ixx,
                          vil_image_view<float>& Ixy,
                          vil_image_view<float>& Iyy);

  static vil_image_view<float>
  beaudet(vil_image_view<float> const & Ixx,
          vil_image_view<float> const & Ixy,
          vil_image_view<float> const & Iyy);


  //: IxIx.transpose gradient matrix elements (N = 2n+1)
  static void grad_matrix_NxN(vil_image_view<float> const & input,
                              const int n,
                              vil_image_view<float>& IxIx,
                              vil_image_view<float>& IxIy,
                              vil_image_view<float>& IyIy);

  //: Computes the Harris corner measure
  static vil_image_view<float>
  harris(vil_image_view<float> const & IxIx,
         vil_image_view<float> const & IxIy,
         vil_image_view<float> const & IyIy,
         double scale=0.04);


  //: computes the conditioning of the 2n+1 x 2n+1 gradient neigborhood
  static vil_image_view<float>
    sqrt_grad_singular_values(vil_image_view<float> & input, int n);

  //: computes Lucas-Kanade optical flow on a 2n+1 neighborhood
  static void Lucas_KanadeMotion(vil_image_view<float> & current_frame,
                                 vil_image_view<float> & previous_frame,
                                 int n, double thresh,
                                 vil_image_view<float>& vx,
                                 vil_image_view<float>& vy);

  //: fills a border of width w on left and right of image with value
  static void fill_x_border(vil_image_view<float> & image,
                            int w, float value);

  //: fills a border of width h on top and bottom of image with value
  static void fill_y_border(vil_image_view<float> & image,
                            int h, float value);

  //: converts a float image to a byte value range
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_view<float> const & image);

  //: converts a float image to a byte value range within a specified range
  static vil_image_view<unsigned char>
    convert_to_byte(vil_image_view<float> const & image,
                    const float min_val, const float max_val);

  //: converts an float image to an unsigned short image within a range
  static vil_image_view<unsigned short>
    convert_to_short(vil_image_view<float> const & image,
                     const float min_val, const float max_val);


  //: converts a vil_image_resource to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_resource const & image);

  //: converts a byte image to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_view<unsigned char> const & image);

  //: converts an RGB image to a float image
  static vil_image_view<float>
    convert_to_float(vil_image_view<vil_rgb<vxl_byte> > const& image);
  //: convert a color image to float IHS images
  static void
    convert_to_IHS(vil_image_view<vil_rgb<vxl_byte> >const& image, 
                   vil_image_view<float>& I,
                   vil_image_view<float>& H,
                   vil_image_view<float>& S);

  //: display IHS images as RGB (not conversion from IHS to RGB)
  static void
    display_IHS_as_RGB(vil_image_view<float> const& I,
                       vil_image_view<float> const& H,
                       vil_image_view<float> const& S,
                       vil_image_view<vil_rgb<vxl_byte> >& image);

  //: converts a generic image to greyscale (RGB<unsigned char>)
  static vil_image_view<unsigned char>
    convert_to_grey(vil_image_resource const& img);

  //: loads a 2n+1 x 2n+1 convolution kernel (see .cxx for file format)
  static vbl_array_2d<float> load_kernel(vcl_string const & file);

  //:compute basis images for a set of input images
  static 
    void basis_images(vcl_vector<vil_image_view<float> > const & input_images,
                      vcl_vector<vil_image_view<float> > & basis);

  //:compute the Fourier transform using the vnl FFT algorithm
  static bool fourier_transform(vil_image_view<float> const & input, 
                                vil_image_view<float>& mag,
                                vil_image_view<float>& phase);

  //:compute the inverse Fourier transform using the vnl FFT algorithm
  static 
    bool inverse_fourier_transform(vil_image_view<float> const& mag,
                                   vil_image_view<float> const& phase,
                                   vil_image_view<float>& output);

  //:resize to specified dimensions, fill with zeros if output is larger
  static 
    void resize(vil_image_view<float> const & input, 
                const int width, const int height,
                vil_image_view<float>& output);


  //:resize to closest power of two larger dimensions than the input 
  static 
    bool resize_to_power_of_two(vil_image_view<float> const & input, 
                                vil_image_view<float>& output);

  //:filter the input image with a Gaussian blocking filter
  static bool 
    spatial_frequency_filter(vil_image_view<float> const & input,
                             const float dir_fx, const float dir_fy, 
                             const float f0, const float radius,
                             const bool output_fourier_mag,
                             vil_image_view<float> & output);
  //: 2x2 bilinear interpolation of image at specified location
  static double 
    bilinear_interpolation(vil_image_view<float> const & input,
                            const double x, const double y);
  //:map the input to the output by a homography.
  // \note if the output size is fixed then only the corresponding 
  // input image space is transformed.
  static bool homography(vil_image_view<float> const & input,
                         vgl_h_matrix_2d<double>const& H,
                         vil_image_view<float>& output,
                         bool output_size_fixed = false,
                         float output_fill_value = 0.0);

  //:rotate the input image counter-clockwise about the image origin        
  static 
  vil_image_view<float> rotate(vil_image_view<float> const & input,
                                     const double theta_deg);

  //:extract a region of interest. If roi does not overlap input, return false
static bool chip(vil_image_view<float> const & input,
                 vsol_box_2d_sptr const& roi, vil_image_view<float> chip);

  //:cross-correlate two images at a given sub-pixel location
  static float
  cross_correlate(vil_image_view<float> const & image1,
                  vil_image_view<float> const & image2,
                  const float x, const float y,
                  const int radius = 5, 
                  const float intensity_thresh=25.0);

  //:cross_correlate two images using running sums
  static bool
  cross_correlate(vil_image_view<float> const & image1,
                  vil_image_view<float> const & image2,
				  vil_image_view<float>& out, 
                  const int radius = 5, 
                  const float intensity_thresh=25.0);

 private:

  //: find if the center pixel of a neighborhood is the maximum value
  static bool local_maximum(vbl_array_2d<float> const & nighborhood,
                            int n, float& value);

  //: find the sub-pixel offset to the maximum using a 3x3 quad interpolation
  static void interpolate_center(vbl_array_2d<float> const & neighborhood,
                                 float& dx, float& dy);

  //: sub-sample a 1-d array using the Bert-Adelson algorithm
  static void half_resolution_1d(const float* input, int n,
                                 const float k0, const float k1,
                                 const float k2, float* output);

  //:One dimensional fft 
  static bool fft_1d(int dir, int m, double* x, double* y);

  //:Two dimensonal fft
  static bool fft_2d(vnl_matrix<vcl_complex<double> >& c, int nx,int ny,int dir);
  //: Transform the fft coefficients from/to fft/frequency order(self inverse).
  static 
    void ftt_fourier_2d_reorder(vnl_matrix<vcl_complex<double> > const& F1,
                                vnl_matrix<vcl_complex<double> > & F2);
  //: Blocking filter function
  static float gaussian_blocking_filter(const float dir_fx,
                                        const float dir_fy, 
                                        const float f0, const float radius,
                                        const float fx, const float fy);
  //: Default constructor is private
  brip_vil_float_ops() {}
};

#endif // brip_vil_float_ops_h_
