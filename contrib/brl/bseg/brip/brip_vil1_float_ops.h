//--*-c++-*--
// This is brl/bseg/brip/brip_vil1_float_ops.h
#ifndef brip_vil1_float_ops_h_
#define brip_vil1_float_ops_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief operations on memory_image_of<float> operands
//
// These methods are similar to the VanDuc gevd_float_ops methods. However,
// they use vil1_image_of<float> buffers rather than the old bufferxy
// structure. The purpose is to provide efficient foundational
// segmentation routines. They are not meant to be generic.
//
// \verbatim
//  Modifications
//   Initial version February 15, 2003
//   Renamed to brip_vil1_float_ops January 24, 2004, In order to support
//   moving from vil1 to vil.
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_complex.h>
#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

class brip_vil1_float_ops
{
 public:
  ~brip_vil1_float_ops() {}

  //: convolves with the specified kernel
  static vil1_memory_image_of<float>
    convolve(vil1_memory_image_of<float> const & input,
             vbl_array_2d<float> const & kernel);

  //: convolves with a Gaussian kernel
  static vil1_memory_image_of<float>
    gaussian(vil1_memory_image_of<float> const & input, float sigma);

  //: non-maximum suppression on a NxN neighborhood, with sub-pixel location
  static void non_maximum_suppression(vil1_memory_image_of<float> const & input,
                                      const int n,
                                      const float thresh,
                                      vcl_vector<float>& x_pos,
                                      vcl_vector<float>& y_pos,
                                      vcl_vector<float>& value);

  //:downsamples the input using the Bert-Adelson algorithm
  static vil1_memory_image_of<float>
  half_resolution(vil1_memory_image_of<float> const & input,
                  float filter_coef=0.359375);

#if 0
  //: interpolates the input using the Bert-Adelson algorithm
  static vil1_memory_image_of<float>
    double_resolution(vil1_memory_image_of<float> const & input,
                      float filter_coef=0.359375);
#endif

  //: subtracts image_1 from image_2
  static vil1_memory_image_of<float>
    difference(vil1_memory_image_of<float> const & image_1,
               vil1_memory_image_of<float> const & image_2);

  //: sets absolute values greater than thresh to specified level
  static vil1_memory_image_of<float>
    abs_clip_to_level(vil1_memory_image_of<float> const & image,
                      const float thresh, const float level = 0.0);

  //: The gradient using a 3x3 kernel
  static void gradient_3x3(vil1_memory_image_of<float> const & input,
                           vil1_memory_image_of<float>& grad_x,
                           vil1_memory_image_of<float>& grad_y);

  static void hessian_3x3(vil1_memory_image_of<float> const & input,
                          vil1_memory_image_of<float>& Ixx,
                          vil1_memory_image_of<float>& Ixy,
                          vil1_memory_image_of<float>& Iyy);

  static vil1_memory_image_of<float>
  beaudet(vil1_memory_image_of<float> const & Ixx,
          vil1_memory_image_of<float> const & Ixy,
          vil1_memory_image_of<float> const & Iyy);


  //: IxIx.transpose gradient matrix elements (N = 2n+1)
  static void grad_matrix_NxN(vil1_memory_image_of<float> const & input,
                              const int n,
                              vil1_memory_image_of<float>& IxIx,
                              vil1_memory_image_of<float>& IxIy,
                              vil1_memory_image_of<float>& IyIy);

  //: Computes the Harris corner measure
  static vil1_memory_image_of<float>
  harris(vil1_memory_image_of<float> const & IxIx,
         vil1_memory_image_of<float> const & IxIy,
         vil1_memory_image_of<float> const & IyIy,
         double scale=0.04);


  //: computes the conditioning of the 2n+1 x 2n+1 gradient neigborhood
  static vil1_memory_image_of<float>
    sqrt_grad_singular_values(vil1_memory_image_of<float> & input, int n);

  //: computes Lucas-Kanade optical flow on a 2n+1 neighborhood
  static void Lucas_KanadeMotion(vil1_memory_image_of<float> & current_frame,
                                 vil1_memory_image_of<float> & previous_frame,
                                 int n, double thresh,
                                 vil1_memory_image_of<float>& vx,
                                 vil1_memory_image_of<float>& vy);

  //: fills a border of width w on left and right of image with value
  static void fill_x_border(vil1_memory_image_of<float> & image,
                            int w, float value);

  //: fills a border of width h on top and bottom of image with value
  static void fill_y_border(vil1_memory_image_of<float> & image,
                            int h, float value);

  //: converts a float image to a byte value range
  static vil1_memory_image_of<unsigned char>
    convert_to_byte(vil1_memory_image_of<float> const & image);

  //: converts a float image to a byte value range within a specified range
  static vil1_memory_image_of<unsigned char>
    convert_to_byte(vil1_memory_image_of<float> const & image,
                    const float min_val, const float max_val);

  //: converts an float image to an unsigned short image within a range
  static vil1_memory_image_of<unsigned short>
    convert_to_short(vil1_memory_image_of<float> const & image,
                     const float min_val, const float max_val);


  //: converts a vil1_image to a float image
  static vil1_memory_image_of<float>
    convert_to_float(vil1_image const & image);

  //: converts a byte image to a float image
  static vil1_memory_image_of<float>
    convert_to_float(vil1_memory_image_of<unsigned char> const & image);

  //: converts an RGB image to a float image
  static vil1_memory_image_of<float>
    convert_to_float(vil1_memory_image_of<vil1_rgb<unsigned char> > const& image);
  //: convert a color image to float IHS images
  static void
    convert_to_IHS(vil1_memory_image_of<vil1_rgb<unsigned char> >const& image, 
                   vil1_memory_image_of<float>& I,
                   vil1_memory_image_of<float>& H,
                   vil1_memory_image_of<float>& S);

  //: display IHS images as RGB (not conversion from IHS to RGB)
  static void
    display_IHS_as_RGB(vil1_memory_image_of<float> const& I,
                       vil1_memory_image_of<float> const& H,
                       vil1_memory_image_of<float> const& S,
                       vil1_memory_image_of<vil1_rgb<unsigned char> >& image);

  //: converts a generic image to greyscale (RGB<unsigned char>)
  static vil1_memory_image_of<unsigned char>
    convert_to_grey(vil1_image const& img);

  //: loads a 2n+1 x 2n+1 convolution kernel (see .cxx for file format)
  static vbl_array_2d<float> load_kernel(vcl_string const & file);

  //:compute basis images for a set of input images
  static 
    void basis_images(vcl_vector<vil1_memory_image_of<float> > const & input_images,
                      vcl_vector<vil1_memory_image_of<float> > & basis);

  //:compute the Fourier transform using the vnl FFT algorithm
  static bool fourier_transform(vil1_memory_image_of<float> const & input, 
                                vil1_memory_image_of<float>& mag,
                                vil1_memory_image_of<float>& phase);

  //:compute the inverse Fourier transform using the vnl FFT algorithm
  static 
    bool inverse_fourier_transform(vil1_memory_image_of<float> const& mag,
                                   vil1_memory_image_of<float> const& phase,
                                   vil1_memory_image_of<float>& output);

  //:resize to specified dimensions, fill with zeros if output is larger
  static 
    void resize(vil1_memory_image_of<float> const & input, 
                const int width, const int height,
                vil1_memory_image_of<float>& output);


  //:resize to closest power of two larger dimensions than the input 
  static 
    bool resize_to_power_of_two(vil1_memory_image_of<float> const & input, 
                                vil1_memory_image_of<float>& output);

  //:filter the input image with a Gaussian blocking filter
  static bool 
    spatial_frequency_filter(vil1_memory_image_of<float> const & input,
                             const float dir_fx, const float dir_fy, 
                             const float f0, const float radius,
                             const bool output_fourier_mag,
                             vil1_memory_image_of<float> & output);
  //: 2x2 bilinear interpolation of image at specified location
  static float
    bilinear_interpolation(vil1_memory_image_of<float> const & input,
                            const double x, const double y);
  //:map the input to the output by a homography.
  // \note if the output size is fixed then only the corresponding 
  // input image space is transformed.
  static bool homography(vil1_memory_image_of<float> const & input,
                         vgl_h_matrix_2d<double>const& H,
                         vil1_memory_image_of<float>& output,
                         bool output_size_fixed = false,
                         float output_fill_value = 0.0);

  //:rotate the input image counter-clockwise about the image origin        
  static 
  vil1_memory_image_of<float> rotate(vil1_memory_image_of<float> const & input,
                                     const double theta_deg);

  bool chip(vil1_memory_image_of<float> const & input,
                               vsol_box_2d_sptr const& roi,
                               vil1_memory_image_of<float> chip);
  
  
  //:cross-correlate two images at a given sub-pixel location
  static float
  cross_correlate(vil1_memory_image_of<float> const & image1,
                  vil1_memory_image_of<float> const & image2,
                  const float x, const float y,
                  const int radius = 5, 
                  const float intensity_thresh=25.0);

  //:Cross-correlate two images using faster running sums
  static vil1_memory_image_of<float> 
  cross_correlate(vil1_memory_image_of<float> const & image1,
                  vil1_memory_image_of<float> const & image2,
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
  brip_vil1_float_ops() {}
};

#endif // brip_vil1_float_ops_h_
