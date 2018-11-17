#include <fstream>
#include <iostream>
#include <complex>
#include <limits>
#include "brip_vil_float_ops.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_bounding_box.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_2x3.h>
#include <vnl/algo/vnl_fft_prime_factors.h>
#include <vnl/algo/vnl_svd.h>

#include <vil/vil_pixel_format.h>
#include <vil/vil_transpose.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_convolve_1d.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <brip/brip_roi.h>

// === Local utility functions ===

//: compute normalized cross correlation from the intensity moment sums.
static float cross_corr(double area, double si1, double si2,
                        double si1i1, double si2i2, double si1i2,
                        float intensity_thresh)
{
  if (!area)
    return 0.f;
  // the mean values
  double u1 = si1/area, u2 = si2/area;
  if (u1<intensity_thresh||u2<intensity_thresh)
    return -1.f;
  double neu = si1i2 - area*u1*u2;
  double sd1 = std::sqrt(std::fabs(si1i1-area*u1*u1)),
    sd2 = std::sqrt(std::fabs(si2i2-area*u2*u2));
  if (!neu)
    return 0.f;
  if (!sd1||!sd2) {
    if (neu>0)
      return 1.f;
    else
      return -1.f;
  }
  double den = sd1*sd2;
  return float(neu/den);
}

//------------------------------------------------------------
//:  Convolve with a kernel
//   It's assumed that the kernel is square with odd dimensions
vil_image_view<float>
brip_vil_float_ops::convolve(vil_image_view<float> const& input,
                             vbl_array_2d<float> const& kernel)
{
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  int kw = kernel.cols(); // kh = kernel.rows();
  // add a check for kernels that are not equal dimensions of odd size JLM
  int n = (kw-1)/2;
  vil_image_view<float> output;
  output.set_size(w,h);
  for (int y = n; y<(h-n); y++)
    for (int x = n; x<(w-n); x++)
    {
      float accum = 0;
      for (int j = -n; j<=n; j++)
        for (int i = -n; i<=n; i++)
        {
          float x1 = input(x+i,y+j);
          float x2 = kernel[i+n][j+n];
          accum += x1*x2;
        }
      output(x,y)=accum;
    }
  brip_vil_float_ops::fill_x_border(output, n, 0.0f);
  brip_vil_float_ops::fill_y_border(output, n, 0.0f);
  return output;
}

static void fill_1d_array(vil_image_view<float> const& input,
                          int y, float* output)
{
  unsigned w = input.ni();
  for (unsigned x = 0; x<w; x++)
    output[x] = input(x,y);
}

//: Downsamples the 1-d array by 2 using the Burt-Adelson reduction algorithm.
void brip_vil_float_ops::half_resolution_1d(const float* input, unsigned width,
                                            float k0, float k1, float k2,
                                            float* output)
{
  float w[5];
  int n = 0;
  for (; n<5; n++)
    w[n]=input[n];
  output[0]=k0*w[0]+ 2.0f*(k1*w[1] + k2*w[2]); // reflect at boundary
  for (unsigned x = 1; x<width; ++x)
  {
    output[x]=k0*w[2]+ k1*(w[1]+w[3]) + k2*(w[0]+w[4]);
    //shift the window, w, over by two pixels
    w[0] = w[2];       w[1] = w[3];     w[2] = w[4];
    //handle the boundary conditions
    if (x+2<width)
      w[3] = input[n++], w[4] = input[n++];
    else
      w[3] = w[1], w[4] = w[0];
  }
}

//: Downsamples the image by 2 using the Burt-Adelson reduction algorithm.
// Convolution with a 5-point kernel [(0.5-ka)/2, 0.25, ka, 0.25, (0.5-ka)/2]
// ka = 0.6  maximum decorrelation, wavelet for image compression.
// ka = 0.5  linear interpolation,
// ka = 0.4  Gaussian filter
// ka = 0.359375 min aliasing, wider than Gaussian
// The image sizes are related by: output_dimension = (input_dimension +1)/2.
vil_image_view<float>
brip_vil_float_ops::half_resolution(vil_image_view<float> const& input,
                                    float filter_coef)
{
  vul_timer t;
  float k0 = filter_coef, k1 = 0.25f*filter_coef, k2 = 0.5f*(0.5f-filter_coef);
  unsigned w = input.ni(), h = input.nj();
  int half_w =(w+1)/2, half_h = (h+1)/2;
  vil_image_view<float> output;
  output.set_size(half_w, half_h);
  // Generate input/output arrays
  int n = 0;
  auto* in0 = new float[w];  auto* in1 = new float[w];
  auto* in2 = new float[w];  auto* in3 = new float[w];
  auto* in4 = new float[w];

  auto* out0 = new float[half_w];  auto* out1 = new float[half_w];
  auto* out2 = new float[half_w];  auto* out3 = new float[half_w];
  auto* out4 = new float[half_w];
  // Initialize arrays
  fill_1d_array(input, n++, in0);   fill_1d_array(input, n++, in1);
  fill_1d_array(input, n++, in2);   fill_1d_array(input, n++, in3);
  fill_1d_array(input, n++, in4);

  // downsample initial arrays
  brip_vil_float_ops::half_resolution_1d(in0, half_w, k0, k1, k2, out0);
  brip_vil_float_ops::half_resolution_1d(in1, half_w, k0, k1, k2, out1);
  brip_vil_float_ops::half_resolution_1d(in2, half_w, k0, k1, k2, out2);
  brip_vil_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
  brip_vil_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
  int x=0, y;
  // do the first output line
  for (;x<half_w;x++)
    output(x,0)= k0*out0[x]+ 2.0f*(k1*out1[x]+k2*out2[x]);
  // normal lines
  for (y=1; y<half_h; y++)
  {
    for (x=0; x<half_w; x++)
      output(x,y) = k0*out2[x]+ k1*(out1[x]+out3[x]) + k2*(out0[x]+out4[x]);
    //shift the neighborhood down two lines
    float* temp0 = out0;
    float* temp1 = out1;
    out0 = out2;  out1 = out3;  out2 = out4;
    out3 = temp0; out4 = temp1;//reflect values
    //test border condition
    if (y<half_h-2)
    {
      //normal processing, so don't reflect
      fill_1d_array(input, n++, in3);
      fill_1d_array(input, n++, in4);
      brip_vil_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
      brip_vil_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
    }
  }
  delete [] in0;  delete [] in1; delete [] in2;
  delete [] in3;  delete [] in4;
  delete [] out0;  delete [] out1; delete [] out2;
  delete [] out3;  delete [] out4;
#ifdef DEBUG
  std::cout << "\nDownsample a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
#endif
  return output;
}

void brip_vil_float_ops::double_resolution_1d(const float* input, const unsigned n_input,
                                              const float k0, const float k1,
                                              const float k2, float* output)
{
  float w[3];
  unsigned i = 0;
  w[1]=input[i]; w[2]=input[i++];
  w[0]=w[2];
  for (unsigned c = 0; c<2*n_input; c+=2)
  {
    output[c] = k0*w[1] + k2*(w[0]+w[2]);
    output[c+1] = k1*(w[1]+w[2]);
    w[0]=w[1];
    w[1]=w[2];
    if (c<2*(n_input-2))
      w[2]=input[i++];
    else
      w[2]=w[0];
  }
}

//: interpolates the input using the Bert-Adelson algorithm
vil_image_view<float>
brip_vil_float_ops::double_resolution(vil_image_view<float> const& input,
                                      float filter_coef)
{
  unsigned ni_in = input.ni();
  unsigned nj_in = input.nj();
  unsigned ni_out = 2*ni_in;
  unsigned nj_out = 2*nj_in;
  vil_image_view<float> out(ni_out, nj_out);
  auto* input_1d = new float[ni_in];

  // An interpolation neighborhood of three lines
  auto* output0 = new float[ni_out];
  auto* output1 = new float[ni_out];
  auto* output2 = new float[ni_out];

  // The filter coefficients
  float k0 = filter_coef*2.0f;
  float k1 = 0.5f;
  float k2 = 0.5f-filter_coef;

  // initialize
  unsigned i = 0;
  fill_1d_array(input, i++, input_1d);
  brip_vil_float_ops::double_resolution_1d(input_1d, ni_in, k0, k1, k2, output1);
  fill_1d_array(input, i++, input_1d);
  brip_vil_float_ops::double_resolution_1d(input_1d, ni_in, k0, k1, k2, output2);
  for (unsigned k = 0; k<ni_out; ++k)
    output0[k]=output2[k];
  for (unsigned r = 0; r<nj_out; r+=2)
  {
    unsigned rp = r+1;
    for (unsigned c=0; c<ni_out; ++c)
    {
      out(c, r) = k0*output1[c] + k2*(output0[c]+output2[c]);
      out(c, rp) = k1*(output1[c]+output2[c]);
    }
    float* next = output0;
    output0 = output1;
    output1 = output2;
    output2 = next;
    if (r<nj_out-4)
    {
      fill_1d_array(input, i++, input_1d);
      brip_vil_float_ops::double_resolution_1d(input_1d, ni_in,
                                               k0, k1, k2, output2);
    }
    else
      for (unsigned k = 0; k<ni_out; ++k)
        output2[k]=output0[k];
  }
  delete [] input_1d;
  delete [] output0;
  delete [] output1;
  delete [] output2;
  return out;
}

static double brip_vil_gaussian(double x, double sigma)
{
  double x_on_sigma = x / sigma;
  return (double)std::exp(- x_on_sigma * x_on_sigma / 2);
}

unsigned brip_vil_float_ops::gaussian_radius(const double sigma,
                                             const double fuzz)
{
  unsigned radius = 0;
  while ((float)brip_vil_gaussian((double)radius, sigma) > fuzz) ++radius;
  return radius;
}

//: generate a 1-d Gaussian kernel  fuzz=0.02 is a good value
static void brip_1d_gaussian_kernel(double sigma, double fuzz,
                                    unsigned& radius, double*& kernel)
{
  radius = brip_vil_float_ops::gaussian_radius(sigma, fuzz);

  kernel = new double[2*radius + 1];
  if (!radius)
  {
    kernel[0]=1;
    return;
  }
  for (unsigned i=0; i<=radius; ++i)
    kernel[radius+i] = kernel[radius-i] = brip_vil_gaussian(double(i), sigma);
  double sum = 0;
  for (unsigned i= 0; i <= 2*radius; ++i)
    sum += kernel[i];                           // find integral of weights
  for (unsigned i= 0; i <= 2*radius; ++i)
    kernel[i] /= sum;                           // normalize by integral
}

vil_image_view<float>
brip_vil_float_ops::gaussian(vil_image_view<float> const& input, float sigma,
                             std::string const& boundary_condition,
                             float fill)
{
  vil_convolve_boundary_option option=vil_convolve_ignore_edge;
  if(boundary_condition == "zeros")
    option = vil_convolve_zero_extend;
  else if(boundary_condition == "const")
    option = vil_convolve_constant_extend;
  else if(boundary_condition == "periodic")
    option = vil_convolve_periodic_extend;
  else if(boundary_condition == "reflect")
    option = vil_convolve_reflect_extend;

  unsigned ni = input.ni(), nj = input.nj();
  unsigned np = input.nplanes();
  vil_image_view<float> dest(ni, nj, np);
  dest.fill(fill);
  unsigned r;
  double* ker;
  brip_1d_gaussian_kernel(sigma, 0.02, r, ker);
  for (unsigned p = 0; p<np; ++p) {
    vil_image_view<float> input_temp(ni, nj);
    vil_image_view<float> out_temp(ni, nj);
    vil_image_view<float> work(ni, nj);
    work.fill(fill);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        input_temp(i,j) = input(i,j,p);

    // filter horizontal
    int ksize = 2*r + 1 ;
    float accum=0.0f;
    vil_convolve_1d(input_temp, work, ker + ksize/2,
                    -ksize/2, r, accum,
                    option, option);
    // filter vertical
    vil_image_view<float> work_t = vil_transpose(work);
    vil_image_view<float> out_temp_t = vil_transpose(dest);
    vil_convolve_1d(work_t, out_temp_t, ker+ ksize/2,
                    -ksize/2, r, accum,
                    option, option);

    vil_image_view<float> plane = vil_transpose(out_temp_t);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        dest(i,j,p) = plane(i,j);
  }
  delete [] ker;
  return dest;
}

vil_image_view<float>
brip_vil_float_ops::absolute_value(vil_image_view<float> const& input)
{
  unsigned ni = input.ni(), nj = input.nj();
  unsigned np = input.nplanes();
  vil_image_view<float> dest(ni, nj, np);
  for (unsigned p = 0; p<np; ++p)
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        dest(i,j,p) = std::fabs(input(i,j,p));
  return dest;
}

#ifdef VIL_CONVOLVE_WITH_MASK_EXISTS // TODO
vil_image_view<float>
brip_vil_float_ops::gaussian(vil_image_view<float> const& input,
                             float sigma,
                             vil_image_view<float> const& mask)
{
  vil_image_view<float> dest(input.ni(), input.nj());

  int r;
  double* ker;
  brip_1d_gaussian_kernel(sigma, 0.02, r, ker);
  vil_image_view<float> work(input.ni(), input.nj());
  work.deep_copy(input);
  // filter horizontal
  int ksize = 2*r + 1 ;
  float accum=0.0f;
  vil_convolve_1d(input, work, mask, ker + ksize/2,
                  -ksize/2, r, accum,
                  vil_convolve_trim,
                  vil_convolve_trim);

  // filter vertical
  vil_image_view<float> work_t = vil_transpose(work);
  vil_image_view<float> dest_t = vil_transpose(dest);
  vil_convolve_1d(work_t, dest_t, vil_transpose(mask), ker+ ksize/2,
                  -ksize/2, r, accum,
                  vil_convolve_constant_extend,
                  vil_convolve_constant_extend);

  delete ker;
  return dest;
}

#endif // VIL_CONVOLVE_WITH_MASK_EXISTS

//-------------------------------------------------------------------
//: Determine if the center of a (2n+1)x(2n+1) neighborhood is a local maximum
//
bool brip_vil_float_ops::
local_maximum(vbl_array_2d<float> const& neighborhood, int n, float& value)
{
  bool local_max = true;
  value = 0;
  float center = neighborhood[n][n];
  for (int y = -n; y<=n; y++)
    for (int x = -n; x<=n; x++)
      local_max = local_max&&(neighborhood[y+n][x+n]<=center);
  if (!local_max)
    return false;
  value = center;
  return true;
}

//-------------------------------------------------------------------
// Interpolate the sub-pixel position of a neighborhood using a
// second order expansion on a 3x3 sub-neighborhood. Return the
// offset to the maximum, i.e. x=x0+dx, y = y0+dy. The design is
// similar to the droid counterpoint by fsm, which uses the Beaudet Hessian
//
void brip_vil_float_ops::
interpolate_center(vbl_array_2d<float>const& neighborhood, float& dx, float& dy)
{
  dx = 0; dy=0;
  // extract the neighborhood
  float n_m1_m1 = neighborhood[0][0];
  float n_m1_0 = neighborhood[0][1];
  float n_m1_1 = neighborhood[0][2];
  float n_0_m1 = neighborhood[1][0];
  float n_0_0 = neighborhood[1][1];
  float n_0_1 = neighborhood[1][2];
  float n_1_m1 = neighborhood[2][0];
  float n_1_0 = neighborhood[2][1];
  float n_1_1 = neighborhood[2][2];

  // Compute the 2nd order quadratic coefficients
  //      1/6 * [ -1  0 +1 ]
  // Ix =       [ -1  0 +1 ]
  //            [ -1  0 +1 ]
  float Ix =(-n_m1_m1+n_m1_1-n_0_m1+n_0_1-n_1_m1+n_1_1)/6.0f;
  //      1/6 * [ -1 -1 -1 ]
  // Iy =       [  0  0  0 ]
  //            [ +1 +1 +1 ]
  float Iy =(-n_m1_m1-n_m1_0-n_m1_1+n_1_m1+n_1_0+n_1_1)/6.0f;
  //      1/3 * [ +1 -2 +1 ]
  // Ixx =      [ +1 -2 +1 ]
  //            [ +1 -2 +1 ]
  float Ixx = ((n_m1_m1+n_0_m1+n_1_m1+n_m1_1+n_0_1+n_1_1)
               -2.0f*(n_m1_0+n_0_0+n_1_0))/3.0f;
  //      1/4 * [ +1  0 -1 ]
  // Ixy =      [  0  0  0 ]
  //            [ -1  0 +1 ]
  float Ixy = (n_m1_m1-n_m1_1+n_1_m1+n_1_1)/4.0f;
  //      1/3 * [ +1 +1 +1 ]
  // Iyy =      [ -2 -2 -2 ]
  //            [ +1 +1 +1 ]
  float Iyy = ((n_m1_m1+n_m1_0+n_m1_1+n_1_m1+n_1_0+n_1_1)
               -2.0f*(n_0_m1 + n_0_0 + n_1_0))/3.0f;
  //
  // The next bit is to find the extremum of the fitted surface by setting its
  // partial derivatives to zero. We need to solve the following linear system :
  // Given the fitted surface is
  // I(x,y) = Io + Ix x + Iy y + 1/2 Ixx x^2 + Ixy x y + 1/2 Iyy y^2
  // we solve for the maximum (x,y),
  //
  //  [ Ixx Ixy ] [ dx ] + [ Ix ] = [ 0 ]      (dI/dx = 0)
  //  [ Ixy Iyy ] [ dy ]   [ Iy ]   [ 0 ]      (dI/dy = 0)
  //
  float det = Ixx*Iyy - Ixy*Ixy;
  // det>0 corresponds to a true local extremum otherwise a saddle point
  if (det>0)
  {
    dx = (Iy*Ixy - Ix*Iyy) / det;
    dy = (Ix*Ixy - Iy*Ixx) / det;
    // more than one pixel away
    if (std::fabs(dx) > 1.0 || std::fabs(dy) > 1.0)
      dx = 0; dy = 0;
  }
}

//---------------------------------------------------------------
// Compute the local maxima of the input on a (2n+1)x(2n+2)
// neighborhood above the given threshold. At each local maximum,
// compute the sub-pixel location, (x_pos, y_pos).
void brip_vil_float_ops::
non_maximum_suppression(vil_image_view<float> const& input,
                        int n, float thresh,
                        std::vector<float>& x_pos,
                        std::vector<float>& y_pos,
                        std::vector<float>& value)
{
  vul_timer t;
  int N = 2*n+1;
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  x_pos.clear();  x_pos.clear();   value.clear();
  vbl_array_2d<float> neighborhood(N,N);
  for (int y =n; y<h-n; y++)
    for (int x = n; x<w-n; x++)
    {
      //If the center is not above threshold then there is
      //no hope
      if (input(x,y)<thresh)
        continue;
      //Fill the neighborhood
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
          neighborhood.put(j+n,i+n,input(x+i, y+j));
      //Check if the center is a local maximum
      float dx, dy, max_v;
      if (brip_vil_float_ops::local_maximum(neighborhood, n, max_v))
      {
        //if so sub-pixel interpolate (3x3) and output results
        brip_vil_float_ops::interpolate_center(neighborhood, dx, dy);
        x_pos.push_back((float)x+dx);
        y_pos.push_back((float)y+dy);
        value.push_back(max_v);
      }
    }
#ifdef DEBUG
  std::cout << "\nCompute non-maximum suppression on a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
#endif
}

// -----------------------------------------------------------------
//: Subtract image_1 from image_2.
// Will not operate unless the two input images are the same dimensions
//
vil_image_view<float>
brip_vil_float_ops::difference(vil_image_view<float> const& image_1,
                               vil_image_view<float> const& image_2)
{
  unsigned w1 = image_1.ni(), h1 = image_1.nj();
  unsigned w2 = image_2.ni(), h2 = image_2.nj();
  vil_image_view<float> temp(w1, h1);
  if (w1!=w2||h1!=h2)
  {
    std::cout << "In brip_vil_float_ops::difference(..) - images are not the same dimensions\n";
    return temp;
  }
  vil_image_view<float> out;
  out.set_size(w1, h1);
  for (unsigned y = 0; y<h1; y++)
    for (unsigned x = 0; x<w1; x++)
      out(x,y) = image_2(x,y)-image_1(x,y);
  return out;
}

//: negate an image returning the same pixel type (only greyscale)
vil_image_resource_sptr brip_vil_float_ops::negate(vil_image_resource_sptr const& imgr)
{
  vil_image_resource_sptr outr;
  if (!imgr)
    return outr;

  vil_pixel_format fmt = imgr->pixel_format();
  switch (fmt)
    {
#define NEGATE_CASE(FORMAT, T) \
   case FORMAT: { \
    vil_image_view<T> view = imgr->get_copy_view(); \
    T mxv = std::numeric_limits<T>::max(); \
    vil_math_scale_and_offset_values(view, -1.0, mxv); \
    outr = vil_new_image_resource_of_view(view);  \
    break; \
                }
      NEGATE_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
      NEGATE_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
      NEGATE_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
      NEGATE_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
      NEGATE_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
      NEGATE_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef NEGATE_CASE
    default:
      std::cout << "Unknown image format\n";
    }
  return outr;
}

vil_image_view<float>
brip_vil_float_ops::threshold(vil_image_view<float> const & image,
                              const float thresh, const float level)
{
  vil_image_view<float> out;
  unsigned w = image.ni(), h = image.nj();
  out.set_size(w, h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      if (image(x,y)>thresh)
        out(x,y) = level;
      else
        out(x,y) = 0;
    }
  return out;
}

vil_image_view<float>
brip_vil_float_ops::abs_clip_to_level(vil_image_view<float> const& image,
                                      float thresh, float level)
{
  vil_image_view<float> out;
  unsigned w = image.ni(), h = image.nj();
  out.set_size(w, h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      if (std::fabs(image(x,y))>thresh)
        out(x,y) = level;
      else
        out(x,y) = image(x,y);
    }
  return out;
}

vil_image_view<float> brip_vil_float_ops::average_NxN(vil_image_view<float> const & img, int N)
{
  vil_image_view<float> result;
  unsigned w = img.ni(), h = img.nj();
  result.set_size (w, h);

  vbl_array_2d <float> averaging_filt(N, N);
  averaging_filt.fill( float(1.00/double(N*N)) );
  result = brip_vil_float_ops::convolve(img, averaging_filt);
  return result;
}

//----------------------------------------------------------------
//: Compute the gradient of the input, use a 3x3 mask
// \verbatim
//         1  |-1  0  1|         1  |-1 -1 -1|
//   Ix = --- |-1  0  1|   Iy = --- | 0  0  0|
//         6  |-1  0  1|         6  | 1  1  1|
// \endverbatim
// Larger masks are computed by pre-convolving with a Gaussian
//
void brip_vil_float_ops::gradient_3x3(vil_image_view<float> const& input,
                                      vil_image_view<float>& grad_x,
                                      vil_image_view<float>& grad_y)
{
  vul_timer t;
  unsigned w = input.ni(), h = input.nj();
  float scale = 1.0f/6.0f;
  for (unsigned y = 1; y+1<h; ++y)
    for (unsigned x = 1; x+1<w; ++x)
    {
      float gx = input(x+1,y-1)+input(x+1,y)+input(x+1,y-1)
                -input(x-1,y-1)-input(x-1,y)-input(x-1,y-1);
      float gy = input(x+1,y+1)+input(x,y+1)+input(x-1,y+1)
                -input(x+1,y-1)-input(x,y-1)-input(x-1,y-1);
      grad_x(x,y) = scale*gx;
      grad_y(x,y) = scale*gy;
    }
  brip_vil_float_ops::fill_x_border(grad_x, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(grad_x, 1, 0.0f);
  brip_vil_float_ops::fill_x_border(grad_y, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(grad_y, 1, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute Gradient in " << t.real() << " msecs.\n";
#endif
}

void brip_vil_float_ops::gradient_mag_3x3(vil_image_view<float> const& input,
                                          vil_image_view<float>& mag)
{
  unsigned w = input.ni(), h = input.nj();
  float scale = 1.0f/6.0f;
  for (unsigned y = 1; y+1<h; ++y)
    for (unsigned x = 1; x+1<w; ++x)
    {
      float gx = input(x+1,y-1)+input(x+1,y)+input(x+1,y-1)
                -input(x-1,y-1)-input(x-1,y)-input(x-1,y-1);
      float gy = input(x+1,y+1)+input(x,y+1)+input(x-1,y+1)
                -input(x+1,y-1)-input(x,y-1)-input(x-1,y-1);
      mag(x,y) = scale*std::sqrt(gx*gx+gy*gy);
    }
  brip_vil_float_ops::fill_x_border(mag, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(mag, 1, 0.0f);
}

void brip_vil_float_ops::
gradient_mag_comp_3x3(vil_image_view<float> const& input,
                      vil_image_view<float>& mag,
                      vil_image_view<float>& gx,
                      vil_image_view<float>& gy)
{
  unsigned w = input.ni(), h = input.nj();
  float scale = 1.0f/6.0f;
  for (unsigned y = 1; y+1<h; ++y)
    for (unsigned x = 1; x+1<w; ++x)
    {
      float ggx = input(x+1,y-1)+input(x+1,y)+input(x+1,y-1)
                 -input(x-1,y-1)-input(x-1,y)-input(x-1,y-1);
      float ggy = input(x+1,y+1)+input(x,y+1)+input(x-1,y+1)
                 -input(x+1,y-1)-input(x,y-1)-input(x-1,y-1);
      mag(x,y) = scale*std::sqrt(ggx*ggx+ggy*ggy);
      gx(x,y) = ggx*scale;
      gy(x,y) = ggy*scale;
    }
  brip_vil_float_ops::fill_x_border(mag, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(mag, 1, 0.0f);
  brip_vil_float_ops::fill_x_border(gx, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(gy, 1, 0.0f);
}

//----------------------------------------------------------------
//: Compute the Hessian of the input, use a 3x3 mask
// \verbatim
//          1 | 1  -2  1|          1 |  1  1  1|         1  | 1  0 -1|
//   Ixx = ---| 1  -2  1|   Iyy = ---| -2 -2 -2|  Ixy = --- | 0  0  0|
//          3 | 1  -2  1|          3 |  1  1  1|         4  |-1  0  1|
// \endverbatim
// Larger masks are computed by pre-convolving with a Gaussian
//
void brip_vil_float_ops::hessian_3x3(vil_image_view<float> const& input,
                                     vil_image_view<float>& Ixx,
                                     vil_image_view<float>& Ixy,
                                     vil_image_view<float>& Iyy)
{
  vul_timer t;
  unsigned w = input.ni(), h = input.nj();
  for (unsigned y = 1; y+1<h; ++y)
    for (unsigned x = 1; x+1<w; ++x)
    {
      float xx = input(x-1,y-1)+input(x-1,y)+input(x+1,y+1)+
                 input(x+1,y-1)+input(x+1,y)+input(x+1,y+1)-
                 2.0f*(input(x,y-1)+input(x,y)+input(x,y+1));

      float xy = (input(x-1,y-1)+input(x+1,y+1))-
                 (input(x-1,y+1)+input(x+1,y-1));

      float yy = input(x-1,y-1)+input(x,y-1)+input(x+1,y-1)+
                 input(x-1,y+1)+input(x,y+1)+input(x+1,y+1)-
                 2.0f*(input(x-1,y)+input(x,y)+input(x+1,y));

      Ixx(x,y) = xx/3.0f;
      Ixy(x,y) = xy/4.0f;
      Iyy(x,y) = yy/3.0f;
    }
  brip_vil_float_ops::fill_x_border(Ixx, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(Ixx, 1, 0.0f);
  brip_vil_float_ops::fill_x_border(Ixy, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(Ixy, 1, 0.0f);
  brip_vil_float_ops::fill_x_border(Iyy, 1, 0.0f);
  brip_vil_float_ops::fill_y_border(Iyy, 1, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute a hessian matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
#endif
}

vil_image_view<float>
brip_vil_float_ops::beaudet(vil_image_view<float> const& Ixx,
                            vil_image_view<float> const& Ixy,
                            vil_image_view<float> const& Iyy,
                            bool determinant)
{
  unsigned w = Ixx.ni(), h = Ixx.nj();
  vil_image_view<float> output;
  output.set_size(w, h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      float xx = Ixx(x,y), xy = Ixy(x,y), yy = Iyy(x,y);

      //compute eigenvalues for experimentation
      float det = xx*yy-xy*xy;
      float tr = xx+yy;
      if (determinant)
        output(x,y) = det;
      else
        output(x,y) = tr;
    }
  return output;
}

//----------------------------------------------------------------
//: $Ix\cdot Ix^t$ gradient matrix elements
// That is,
// \verbatim
//                        _                           _
//                       | (dI/dx)^2    (dI/dx)(dI/dy) |
//                       |                             |
//  A = Sum(neighborhood)|                             |
//                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
//                       |_                           _|
// \endverbatim
// over a 2n+1 x 2n+1 neighborhood
//
void
brip_vil_float_ops::grad_matrix_NxN(vil_image_view<float> const& input,
                                    unsigned n,
                                    vil_image_view<float>& IxIx,
                                    vil_image_view<float>& IxIy,
                                    vil_image_view<float>& IyIy)
{
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  int N = (2*n+1)*(2*n+1);
  int ni = static_cast<int>(n);
  vil_image_view<float> grad_x, grad_y, output;
  grad_x.set_size(w,h);
  grad_y.set_size(w,h);
  output.set_size(w,h);
  brip_vil_float_ops::gradient_3x3(input, grad_x, grad_y);
  vul_timer t;
  for (int y = ni; y<h-ni;y++)
    for (int x = ni; x<w-ni;x++)
    {
      float xx=0, xy=0, yy=0;
      for (int i = -ni; i<=ni; i++)
        for (int j = -ni; j<=ni; j++)
        {
          float gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          xx += gx*gx;
          xy += gx*gy;
          yy += gy*gy;
        }
      IxIx(x,y) = xx/(float)N;
      IxIy(x,y) = xy/(float)N;
      IyIy(x,y) = yy/(float)N;
    }
  brip_vil_float_ops::fill_x_border(IxIx, ni, 0.0f);
  brip_vil_float_ops::fill_y_border(IxIx, ni, 0.0f);
  brip_vil_float_ops::fill_x_border(IxIy, ni, 0.0f);
  brip_vil_float_ops::fill_y_border(IxIy, ni, 0.0f);
  brip_vil_float_ops::fill_x_border(IyIy, ni, 0.0f);
  brip_vil_float_ops::fill_y_border(IyIy, ni, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute a gradient matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
#endif
}

vil_image_view<float> brip_vil_float_ops::
trace_grad_matrix_NxN(vil_image_view<float> const& input, unsigned n)
{
  unsigned ni = input.ni(), nj = input.nj();
  vil_image_view<float> IxIx;
  vil_image_view<float> IxIy;
  vil_image_view<float> IyIy;
  vil_image_view<float> tr;
  IxIx.set_size(ni, nj);   IxIy.set_size(ni, nj);   IyIy.set_size(ni, nj);
  tr.set_size(ni, nj);
  brip_vil_float_ops::grad_matrix_NxN(input, n, IxIx, IxIy, IyIy);
  vil_math_image_sum<float, float, float>(IxIx, IyIy, tr);
  return tr;
}

vil_image_view<float>
brip_vil_float_ops::harris(vil_image_view<float> const& IxIx,
                           vil_image_view<float> const& IxIy,
                           vil_image_view<float> const& IyIy,
                           double scale)
{
  unsigned w = IxIx.ni(), h = IxIx.nj();
  float norm = 1e-3f; // Scale the output to values in the 10->1000 range
  vil_image_view<float> output;
  output.set_size(w, h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      float xx = IxIx(x,y), xy = IxIy(x,y), yy = IyIy(x,y);
      float det = xx*yy-xy*xy, trace = xx+yy;
      output(x,y) = float(det - scale*trace*trace)*norm;
    }
  return output;
}

//----------------------------------------------------------------
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
//
vil_image_view<float>
brip_vil_float_ops::sqrt_grad_singular_values(vil_image_view<float>& input,
                                              int n)
{
  int N = (2*n+1)*(2*n+1);
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  vil_image_view<float> grad_x, grad_y, output;
  grad_x.set_size(w,h);
  grad_y.set_size(w,h);
  output.set_size(w,h);
  brip_vil_float_ops::gradient_3x3(input, grad_x, grad_y);
  vul_timer t;
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      float IxIx=0, IxIy=0, IyIy=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          float gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          IxIx += gx*gx;
          IxIy += gx*gy;
          IyIy += gy*gy;
        }
      float det = (IxIx*IyIy-IxIy*IxIy)/(float)N;
      output(x,y)=std::sqrt(std::fabs(det));
    }
  brip_vil_float_ops::fill_x_border(output, n, 0.0f);
  brip_vil_float_ops::fill_y_border(output, n, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute sqrt(sigma0*sigma1) in" << t.real() << " msecs.\n";
#endif
  return output;
}

vil_image_view<float> brip_vil_float_ops::
max_scale_trace(const vil_image_view<float>& input,
                float min_scale, float max_scale, float scale_inc)
{
  unsigned ni = input.ni(), nj = input.nj();
  vil_image_view<float> tr_max, sc;
  tr_max.set_size(ni, nj);
  tr_max.fill(0.0f);
  sc.set_size(ni, nj);
  sc.fill(min_scale);
  for (float s = min_scale; s<=max_scale; s+=scale_inc)
  {
    vil_image_view<float> smooth = brip_vil_float_ops::gaussian(input, s);
    auto N = static_cast<unsigned>(2.0f*s);
    vil_image_view<float> tr =
      brip_vil_float_ops::trace_grad_matrix_NxN(smooth, N);
    for (unsigned r = 0; r<nj; ++r)
      for (unsigned c = 0; c<ni; ++c)
      {
        float trv = s*s*tr(c,r);
        if (trv>tr_max(c,r))
        {
          tr_max(c,r) = trv;
          sc(c,r) = s;
        }
      }
  }
  return sc;
}

//: exactly same as max_scale_trace, only return the image with actual trace values at max scales instead of the image with max scale values
vil_image_view<float> brip_vil_float_ops::
max_scale_trace_value(const vil_image_view<float>& input,
                      float min_scale, float max_scale, float scale_inc)
{
  unsigned ni = input.ni(), nj = input.nj();
  vil_image_view<float> tr_max, sc;
  tr_max.set_size(ni, nj);
  tr_max.fill(0.0f);
  sc.set_size(ni, nj);
  sc.fill(min_scale);
  for (float s = min_scale; s<=max_scale; s+=scale_inc)
  {
    vil_image_view<float> smooth = brip_vil_float_ops::gaussian(input, s);
    auto N = static_cast<unsigned>(2.0f*s);
    vil_image_view<float> tr =
      brip_vil_float_ops::trace_grad_matrix_NxN(smooth, N);
    for (unsigned r = 0; r<nj; ++r)
      for (unsigned c = 0; c<ni; ++c)
      {
        float trv = s*s*tr(c,r);
        if (trv>tr_max(c,r))
        {
          tr_max(c,r) = trv;
          sc(c,r) = s;
        }
      }
  }
  // mask the region where integration region extends outside the image borders
  auto N = static_cast<unsigned>(5.0f*max_scale);
  unsigned njmax = nj-N;
  unsigned nimax = ni-N;
  for (unsigned r = 0; r<nj; ++r)
    for (unsigned c = 0; c<ni; ++c)
    {
      if (r <= N || r >= njmax || c <= N || c >= nimax)
        tr_max(c,r) = 0.0f;
    }

  // normalize the trace values
  float min_b,max_b;
  vil_math_value_range(tr_max,min_b,max_b);
  std::cout << "in trace max image min value: " << min_b << " max value: " << max_b << std::endl;

  vil_image_view<double> tr_normalized, tr_stretched;
  vil_convert_stretch_range(tr_max, tr_normalized, 0.0f, 1.0f);
  vil_convert_stretch_range(tr_max, tr_stretched, 0.0f, 256.0f);
  vil_image_view<float> tr_cast;
  vil_convert_cast(tr_stretched, tr_cast);
  vil_image_view<vxl_byte> tr_cast_byte;
  vil_convert_cast(tr_stretched, tr_cast_byte);
  vil_save_image_resource(vil_new_image_resource_of_view(tr_cast_byte), "D:\\projects\\vehicle_rec_on_models\\trace_image.png");

#if 0 // commented out
  // investigate illumination invariance
  vil_image_view<float> modified_input = input;
  vil_image_view<float> const_image(ni, nj);
  const_image.fill(3.0f);
  for (unsigned r = 0; r<nj; ++r)
    for (unsigned c = 0; c<ni; ++c)
    {
      if (modified_input(c,r) < 120 || modified_input(c,r) > 95) = modified_input(c,r)*const_image(c,r);
    }

  vil_image_view<vxl_byte> mod_cast;
  vil_convert_cast(modified_input, mod_cast);
  vil_save_image_resource(vil_new_image_resource_of_view(mod_cast), "D:\\projects\\vehicle_rec_on_models\\modified_image.png");

  vil_image_view<float> tr_max2, sc2;
  tr_max2.set_size(ni, nj);
  tr_max2.fill(0.0f);
  sc2.set_size(ni, nj);
  sc2.fill(min_scale);
  for (float s = min_scale; s<=max_scale; s+=scale_inc)
  {
    vil_image_view<float> smooth = brip_vil_float_ops::gaussian(modified_input, s);
    unsigned N = static_cast<unsigned>(2.0f*s);
    vil_image_view<float> tr =
      brip_vil_float_ops::trace_grad_matrix_NxN(smooth, N);
    for (unsigned r = 0; r<nj; ++r)
      for (unsigned c = 0; c<ni; ++c)
      {
        float trv = s*s*tr(c,r);
        if (trv>tr_max2(c,r))
        {
          tr_max2(c,r) = trv;
          sc2(c,r) = s;
        }
      }
  }
  // mask the region where integration region extends outside the image borders
  for (unsigned r = 0; r<nj; ++r)
    for (unsigned c = 0; c<ni; ++c)
    {
      if (r <= N || r >= njmax || c <= N || c >= nimax)
        tr_max2(c,r) = 0.0f;
    }

  vil_math_value_range(tr_max2,min_b,max_b);
  std::cout << "in trace max2 image min value: " << min_b << " max value: " << max_b << std::endl;

  // normalize
  vil_image_view<double> tr_normalized2;
  vil_convert_stretch_range(tr_max2, tr_normalized2, 0.0f, 1.0f);

  vil_image_view<double> difference_image = tr_normalized;
  for (unsigned r = 0; r<nj; ++r)
    for (unsigned c = 0; c<ni; ++c)
    {
      difference_image(c,r) = std::abs(difference_image(c,r) - tr_normalized2(c,r));
    }
  double min_bd, max_bd;
  vil_math_value_range(difference_image,min_bd,max_bd);
  std::cout << "in difference image of normalized trace images min value: " << min_bd << " max value: " << max_bd << std::endl;
  vil_image_view<vxl_byte> dif_cast;
  vil_convert_cast(difference_image, dif_cast);
  vil_save_image_resource(vil_new_image_resource_of_view(dif_cast), "D:\\projects\\vehicle_rec_on_models\\difference_image.png");

  vil_image_view<double> tr_stretched2;
  vil_convert_stretch_range(tr_max2, tr_stretched2, 0.0f, 256.0f);
  vil_image_view<float> tr_cast2;
  vil_convert_cast(tr_stretched2, tr_cast2);
  vil_image_view<vxl_byte> tr_cast2_byte;
  vil_convert_cast(tr_stretched2, tr_cast2_byte);
  vil_save_image_resource(vil_new_image_resource_of_view(tr_cast2_byte), "D:\\projects\\vehicle_rec_on_models\\trace_image2.png");

  return tr_cast2;
#else // 0
  return tr_cast;
#endif // 0
}


//---------------------------------------------------------------------
// Lucas-Kanade motion vectors:  Solve for the motion vectors over a
// (2n+1)x(2n+1) neighborhood. The time derivative of intensity is computed
// from the previous_frame. The threshold eliminates small values of
// the product of the time derivative and the motion matrix eigenvalues,
// i.e, |lambda_1*lambda_2*dI/dt|<thresh.  Thus motion is only reported when
// the solution is well-conditioned.
//
void
brip_vil_float_ops::Lucas_KanadeMotion(vil_image_view<float> & current_frame,
                                       vil_image_view<float> & previous_frame,
                                       int n, double thresh,
                                       vil_image_view<float>& vx,
                                       vil_image_view<float>& vy)
{
  int N = (2*n+1)*(2*n+1);
  int w = static_cast<int>(current_frame.ni()), h = static_cast<int>(current_frame.nj());
  vil_image_view<float> grad_x, grad_y, diff;
  grad_x.set_size(w,h);
  grad_y.set_size(w,h);
  // compute the gradient vector and the time derivative
  brip_vil_float_ops::gradient_3x3(current_frame, grad_x, grad_y);
  diff = brip_vil_float_ops::difference(previous_frame, current_frame);
  vul_timer t;
  // sum the motion terms over the (2n+1)x(2n+1) neighborhood.
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      float IxIx=0, IxIy=0, IyIy=0, IxIt=0, IyIt=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          float gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          float dt = diff(x+i, y+j);
          IxIx += gx*gx;
          IxIy += gx*gy;
          IyIy += gy*gy;
          IxIt += gx*dt;
          IyIt += gy*dt;
        }
      //Divide by the number of pixels in the neighborhood
      IxIx/=N;  IxIy/=N; IyIy/=N; IxIt/=N; IyIt/=N;
      auto det = float(IxIx*IyIy-IxIy*IxIy);
      //Eliminate small motion factors
      float dif = diff(x,y);
      float motion_factor = std::fabs(det*dif);
      if (motion_factor<thresh)
      {
        vx(x,y) = 0.0f;
        vy(x,y) = 0.0f;
        continue;
      }
      //solve for the motion vector
      vx(x,y) = (IyIy*IxIt-IxIy*IyIt)/det;
      vy(x,y) = (-IxIy*IxIt + IxIx*IyIt)/det;
    }
  brip_vil_float_ops::fill_x_border(vx, n, 0.0f);
  brip_vil_float_ops::fill_y_border(vx, n, 0.0f);
  brip_vil_float_ops::fill_x_border(vy, n, 0.0f);
  brip_vil_float_ops::fill_y_border(vy, n, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute Lucas-Kanade in " << t.real() << " msecs.\n";
#endif
}

//: computes Lucas-Kanade optical flow on the complete input views
void brip_vil_float_ops::
lucas_kanade_motion_on_view(vil_image_view<float> const& curr_frame,
                            vil_image_view<float> const& prev_frame,
                            const double thresh,
                            float& vx,
                            float& vy)
{
  unsigned w = curr_frame.ni(), h = curr_frame.nj();
  unsigned N  = w*h;
  vil_image_view<float> grad_x, grad_y;
  grad_x.set_size(w,h);
  grad_y.set_size(w,h);
  // compute the gradient vector and the time derivative
  brip_vil_float_ops::gradient_3x3(curr_frame, grad_x, grad_y);
  vil_image_view<float> diff =
    brip_vil_float_ops::difference(prev_frame, curr_frame);

  // sum the motion terms over the view
  float IxIx=0.0f, IxIy=0.0f, IyIy=0.0f, IxIt=0.0f, IyIt=0.0f, dsum=0.0f;
  for (unsigned j = 0; j<h; j++)
    for (unsigned i = 0; i<w; i++)
    {
      float gx = grad_x(i, j), gy = grad_y(i, j);
      float dt = diff(i, j);
      dsum += dt*dt;
      IxIx += gx*gx;
      IxIy += gx*gy;
      IyIy += gy*gy;
      IxIt += gx*dt;
      IyIt += gy*dt;
    }
  // Divide by the number of pixels in the neighborhood
  IxIx/=(float)N;  IxIy/=(float)N; IyIy/=(float)N; IxIt/=(float)N; IyIt/=(float)N; dsum/=(float)N;
  auto det = float(IxIx*IyIy-IxIy*IxIy);
  // Eliminate small motion factors
  float dif = std::sqrt(dsum);
  float motion_factor = std::fabs(det*dif);
  if (motion_factor<thresh)
  {
    vx = 0.0f;
    vy = 0.0f;
    return;
  }
  // solve for the motion vector
  vx = (IyIy*IxIt-IxIy*IyIt)/det;
  vy = (-IxIy*IxIt + IxIx*IyIt)/det;
}

//------------------------------------------------------------------------
// Assume that curr match region is larger than prev_region by the required
// search ranges. Step through the search and output the shift that
// maximizes the correlation. zero_i and zero_j indicate the curr_image
// pixel location corresponding to no velocity between frames
void brip_vil_float_ops::
velocity_by_correlation(vil_image_view<float> const& curr_image,
                        vil_image_view<float> const& prev_region,
                        const unsigned start_i, const unsigned end_i,
                        const unsigned start_j, const unsigned end_j,
                        const unsigned zero_i, const unsigned zero_j,
                        float& vx,
                        float& vy)
{
  unsigned ni = prev_region.ni(), nj = prev_region.nj();
  float corr_max = -10.0f;
  auto vx0 = static_cast<float>(zero_i);
  auto vy0 = static_cast<float>(zero_j);
  auto area = static_cast<float>(ni*nj);
  vx = 0; vy = 0;
  unsigned max_i = start_i, max_j = start_j;
  for (unsigned j = start_j; j<=end_j; ++j)
    for (unsigned i = start_i; i<=end_i; ++i)
    {
      float si1 = 0, si2 = 0, si1i1 = 0, si2i2 = 0, si1i2 = 0;
      //sum over the region
      for (unsigned r = 0; r<nj; ++r)
        for (unsigned c = 0; c<ni; ++c)
        {
          float I1 = prev_region(c, r);
          float I2 = curr_image(i+c, j+r);
          si1 += I1; si2 += I2;
          si1i1 += I1*I1;
          si2i2 += I2*I2;
          si1i2 += I1*I2;
        }
      float corr = cross_corr(area, si1, si2, si1i1, si2i2,si1i2, 1.0f);
      if (corr>corr_max)
      {
        corr_max = corr;
        max_i = i; max_j = j;
      }
#if 0
      float di = i-vx0, dj = j-vy0;
      std::cout <<  di << '\t' << dj << '\t' << corr << '\n';
#endif
    }
  // the velocity is given by the max indices relative to the zero location
  vx = static_cast<float>(max_i)- vx0;
  vy = static_cast<float>(max_j) - vy0;
  // std::cout << '(' << vx << ' ' << vy << "): " << corr_max << '\n';
}

//---------------------------------------------------------------------
// Horn-Schunk method for calc. motion vectors:  Solve for the motion vectors
// iteratively using a cost function with two terms (RHS of optical flow eqn
// and the magnitude of spatial derivatives of the velocity field
// (so that pixel-to-pixel variations are small). The second term is
// weighted by alpha_coef term. The iteration goes on until the error
// reaches below err_thresh
//  Error conditions:
//  -  -1  -  current_frame and previous_frame are equal
//  -  -2  -  at least one input frame or internal process image is all zeros
//  -   0  -  routine was successful

int brip_vil_float_ops::
Horn_SchunckMotion(vil_image_view<float> const& current_frame,
                   vil_image_view<float> const& previous_frame,
                   vil_image_view<float>& vx,
                   vil_image_view<float>& vy,
                   const float alpha_coef,
                   const int no_of_iterations)
{
  // Check for equal images
  if (vil_image_view_deep_equality (previous_frame, current_frame ) )
  {
    std::cout<<"Images are same";
    return -1;
  }

  // Declarations
  unsigned w = current_frame.ni(), h = current_frame.nj();

  vil_image_view<float> grad_x, grad_y, diff;

  vil_image_view<float> temp1;
  vil_image_view<float> temp2;

  vil_image_view<float> emptyimg;

  // Size Init

  grad_x.set_size(w,h);
  grad_y.set_size(w,h);
  diff.set_size(w,h);
  temp1.set_size(w,h);
  temp2.set_size(w,h);

  emptyimg.set_size(w,h);

  temp1.fill(0.0);
  temp2.fill(0.0);

  // Initialization
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      vx(x,y)=0.0f;
      vy(x,y)=0.0f;
      diff (x,y)=0.0f;
      grad_x (x, y)= 0.0f;
      grad_y (x, y)= 0.0f;

      emptyimg (x, y) = 0.0f;
    }

  // Check for empty images
  if ( (vil_image_view_deep_equality (emptyimg, current_frame )) || (vil_image_view_deep_equality(emptyimg, previous_frame)))
  {
    std::cout<<"Image is empty";
    return -2;
  }

  // compute the gradient vector for current and previous
  brip_vil_float_ops::gradient_3x3 (current_frame , grad_x , grad_y);
  brip_vil_float_ops::gradient_3x3 (previous_frame , temp1 , temp2);

  //  Grad = 0.5* Grad(current) + 0.5 * Grad(previous)
  vil_math_add_image_fraction(grad_x, 0.5, temp1, 0.5);
  vil_math_add_image_fraction(grad_y, 0.5, temp2, 0.5);
  if ( (vil_image_view_deep_equality(emptyimg, grad_x)) ||
       (vil_image_view_deep_equality(emptyimg, grad_y)) )
    {
      std::cout<<"Gradient Image is empty";
      return -2;
    }

  temp1.fill(0.0);
  temp2.fill(0.0);

  // Average the local intensities over 3x3 region
  temp1 = brip_vil_float_ops::average_NxN(previous_frame, 3);
  temp2 = brip_vil_float_ops::average_NxN(current_frame, 3);
  if (vil_image_view_deep_equality(emptyimg, temp1) ||
      vil_image_view_deep_equality(emptyimg, temp2))
    {
      std::cout<<"Averaged Image is empty";
      return -2;
    }

  // Compute the time derivative (difference of local average intensities)
  // diff = dI/dt
  diff = brip_vil_float_ops::difference(temp1 , temp2);
  if (vil_image_view_deep_equality(emptyimg, diff) )
  {
    std::cout<<"Difference Image is empty";
    return -2;
  }

  temp1.fill(0.0);
  temp2.fill(0.0);
  // Iterate
#ifdef DEBUG
  vul_timer t;
#endif
  for (int i=0;i<no_of_iterations;i++)
  {
    // Update vx and vy
    //Smoothed velocities on 3x3 region
    temp1 = brip_vil_float_ops::average_NxN (vx,  3);
    temp2 = brip_vil_float_ops::average_NxN (vy,  3);
    for (unsigned y = 1; y+1<h; ++y)
      for (unsigned x = 1; x+1<w; ++x)
      {
        float tempx = temp1(x,y);
        float tempy = temp2(x,y);

        float gx = grad_x(x, y), gy = grad_y(x, y);

        float dt = diff(x, y);
        //         _____
        // term = (v(x,y).Grad(x,y) + dI/dt(x,y))/(alpha + |Grad(x,y)|^2)
        // term is the brightness constraint normalized by gradient mag.
        //
        float term =
          ( (gx * tempx) + (gy * tempy) + dt )/ (alpha_coef + gx*gx + gy*gy);

        //         ______
        //v(x,y) = v(x,y) - Grad(x,y)* term
        vx(x,y) = tempx - (gx *  term);
        vy(x,y) = tempy - (gy *  term);
      }

#ifdef DEBUG
    std::cout << "Iteration No " << i << '\n';
#endif
    brip_vil_float_ops::fill_x_border(vx, 1, 0.0f);
    brip_vil_float_ops::fill_y_border(vx, 1, 0.0f);
    brip_vil_float_ops::fill_x_border(vy, 1, 0.0f);
    brip_vil_float_ops::fill_y_border(vy, 1, 0.0f);
  }
#ifdef DEBUG
  std::cout << "\nCompute Horn-Schunck iteration in " << t.real() << " msecs.\n";
#endif
  return 0;
}

void brip_vil_float_ops::fill_x_border(vil_image_view<float> & image,
                                       unsigned w, float value)
{
  unsigned width = image.ni(), height = image.nj();
  if (2*w>width)
  {
    std::cout << "In brip_vil_float_ops::fill_x_border(..) - 2xborder exceeds image width\n";
    return;
  }
  for (unsigned y = 0; y<height; y++)
    for (unsigned x = 0; x<w; x++)
      image(x, y) = value;

  for (unsigned y = 0; y<height; y++)
    for (unsigned x = width-w; x<width; x++)
      image(x, y) = value;
}

void brip_vil_float_ops::fill_y_border(vil_image_view<float> & image,
                                       unsigned h, float value)
{
  unsigned width = image.ni(), height = image.nj();
  if (2*h>height)
  {
    std::cout << "In brip_vil_float_ops::fill_y_border(..) - 2xborder exceeds image height\n";
    return;
  }
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<width; x++)
      image(x, y) = value;

  for (unsigned y = height-h; y<height; y++)
    for (unsigned x = 0; x<width; x++)
      image(x, y) = value;
}

vil_image_view<vxl_byte>
brip_vil_float_ops::convert_to_byte(vil_image_view<float> const& image)
{
  // determine the max min values
  float min_val = vnl_numeric_traits<float>::maxval;
  float max_val = -min_val;
  unsigned w = image.ni(), h = image.nj();
  vil_image_view<vxl_byte> output;
  output.set_size(w,h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      min_val = std::min(min_val, image(x,y));
      max_val = std::max(max_val, image(x,y));
    }
  float range = max_val-min_val;
  if (range == 0.f)
    range = 1.f;
  else
    range = 255.f/range;
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      float v = (image(x,y)-min_val)*range;
      if(v>255.0f) v = 255.0f;//could have small round off above 255
      output(x,y) = (unsigned char)v;
    }
  return output;
}

//------------------------------------------------------------
//: Convert the range between min_val and max_val to 255
vil_image_view<vxl_byte>
brip_vil_float_ops::convert_to_byte(vil_image_view<float> const& image,
                                    float min_val, float max_val)
{
  unsigned w = image.ni(), h = image.nj();
  vil_image_view<vxl_byte> output;
  output.set_size(w,h);
  float range = max_val-min_val;
  if (range == 0.f)
    range = 1.f;
  else
    range = 256.f/range;
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      float v = (image(x,y)-min_val)*range;
      if (v>=256)
        v=255;
      else if (v<0)
        v=0;
      output(x,y) = (unsigned char)v;
    }

  return output;
}

vil_image_view<vxl_byte> brip_vil_float_ops::
convert_to_byte(vil_image_view<vxl_uint_16> const& image,
                vxl_uint_16 min_val, vxl_uint_16 max_val)
{
  unsigned ni = image.ni(), nj = image.nj();
  vil_image_view<vxl_byte> output;
  output.set_size(ni, nj);
  auto range = static_cast<float>(max_val-min_val);
  if (!range)
    range = 1.f;
  else
    range = 256.f/range;
  for (unsigned r = 0; r<nj; r++)
    for (unsigned c = 0; c<ni; c++)
    {
      float v = float(image(c, r)-min_val)*range;
      if (v>=256)
        v=255;
      else if (v<0)
        v=0;
      output(c, r) = static_cast<unsigned char>(v);
    }
  return output;
}

// Note this is a more standard interface than convert_to_grey
vil_image_view<vxl_byte>
brip_vil_float_ops::convert_to_byte(vil_image_resource_sptr const& image)
{
  return brip_vil_float_ops::convert_to_grey(*image);
}

vil_image_view<vxl_uint_16>
brip_vil_float_ops::convert_to_short(vil_image_view<float> const& image,
                                     float min_val, float max_val)
{
  unsigned w = image.ni(), h = image.nj();
  float max_short = 65355.f;
  vil_image_view<vxl_uint_16> output;
  output.set_size(w,h);
  float range = max_val-min_val;
  if (!range)
    range = 1;
  else
    range = max_short/range;
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      float v = (image(x,y)-min_val)*range;
      if (v>max_short)
        v=max_short;
      if (v<0)
        v=0;
      output(x,y) = (unsigned short)v;
    }
  return output;
}

//: converts a float image to an unsigned short (16-bit) image.
// range determined automatically
vil_image_view<vxl_uint_16>
brip_vil_float_ops::convert_to_short(vil_image_view<float> const& image)
{
  float minv = vnl_numeric_traits<float>::maxval;
  float maxv = -minv;
  unsigned ni = image.ni(), nj = image.nj();
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float v = image(i,j);
      if (v<minv)
        minv = v;
      if (v>maxv)
        maxv = v;
    }
  return brip_vil_float_ops::convert_to_short(image, minv, maxv);
}

vil_image_view<vxl_uint_16>
brip_vil_float_ops::convert_to_short(vil_image_resource_sptr const& image)
{
  // Check if the image is a float
  if (image->nplanes()==1 &&image->pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float> temp = image->get_view();
    float vmin=0, vmax= 65355;
    vil_math_value_range<float>(temp, vmin, vmax);
    return brip_vil_float_ops::convert_to_short(temp, vmin, vmax);
  }

  // Here we assume that the image is an unsigned char
  if (image->nplanes()==1&&image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char > temp = image->get_view();
    vil_image_view<unsigned short> short_image;
    unsigned width = temp.ni(), height = temp.nj();
    short_image.set_size(width, height);
    for (unsigned y = 0; y<height; y++)
      for (unsigned x = 0; x<width; x++)
        short_image(x,y) = static_cast<unsigned short>(temp(x,y));
    return temp;
  }

  // Here the image is an unsigned short (16-bit) image so just return it
  if (image->nplanes()==1&&image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short > temp = image->get_view();
    return temp;
  }

  // the image is color so we should convert it to greyscale
  // Here we assume the color elements are unsigned char.
  if (image->nplanes()==3&&image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte> color_image = image->get_view();
    unsigned width = color_image.ni(), height = color_image.nj();
    // the output image
    vil_image_view<unsigned short> short_image;
    short_image.set_size(width, height);
    for (unsigned y = 0; y<height; y++)
      for (unsigned x = 0; x<width; x++)
      {
        double v = color_image(x,y,0)+color_image(x,y,1)+color_image(x,y,2);
        v/=3.0;
        short_image(x,y) = static_cast<unsigned short>(v);
      }
    return short_image;
  }

  // the image is multispectral so we should convert it to greyscale
  // Here we assume the color elements are unsigned short (16-bit).
  if (image->nplanes()==4&&image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short > mband_image = image->get_view();
    unsigned width = mband_image.ni(), height = mband_image.nj();
    // the output image
    vil_image_view<unsigned short> short_image;
    short_image.set_size(width, height);
    for (unsigned y = 0; y<height; y++)
      for (unsigned x = 0; x<width; x++)
      {
        unsigned short v = 0;
        for (unsigned p = 0; p<4; ++p)
          v += mband_image(x, y, p);
        v/=4;
        short_image(x,y) = v;
      }
    return short_image;
  }

  // If we get here then the input is not a type we handle so return a null view
  return vil_image_view<vxl_uint_16>();
}

vil_image_view<float>
brip_vil_float_ops::convert_to_float(vil_image_view<vxl_byte> const& image)
{
  vil_image_view<float> output;
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  output.set_size(ni,nj, np);
  for (unsigned j = 0; j<nj; j++)
    for (unsigned i = 0; i<ni; i++)
      for (unsigned p = 0; p<np; ++p)
        output(i,j,p) = (float)image(i,j,p);
  return output;
}

vil_image_view<float>
brip_vil_float_ops::convert_to_float(vil_image_view<vxl_uint_16> const& image)
{
  vil_image_view<float> output;
  unsigned w = image.ni(), h = image.nj();
  output.set_size(w,h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
      output(x,y) = (float)image(x,y);
  return output;
}

vil_image_view<bool>
brip_vil_float_ops::convert_to_bool(vil_image_view<vxl_byte> const& image)
{
  vil_image_view<bool> output;
  unsigned w = image.ni(), h = image.nj();
  output.set_size(w,h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
      if (image(x,y) > 128)
        output(x,y)=true;
      else
        output(x,y)=false;
  return output;
}

vil_image_view<float>
brip_vil_float_ops::convert_to_float(vil_image_view<vil_rgb<vxl_byte> > const& image)
{
  vil_image_view<float> output;
  unsigned w = image.ni(), h = image.nj();
  output.set_size(w,h);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
    {
      vil_rgb<vxl_byte> rgb = image(x,y);
      output(x,y) = (float)rgb.grey();
    }
  return output;
}

void brip_vil_float_ops::rgb_to_ihs(vil_rgb<vxl_byte> const& rgb,
                                    float& i, float& h, float& s)
{
  float r=rgb.R();
  float g=rgb.G();
  float b=rgb.B();

  float maxval = std::max(r,std::max(g,b));
  float minval = std::min(r,std::min(g,b));

  float delta = maxval - minval;
  i = maxval;
  if (maxval == 0)
    s = 0;
  else
    s = delta / maxval;

  if (s==0)
    h = 0;                   //!< (Hue is undefined)

  if (r== maxval)
    h = (g - b) / delta ;    //!< (between yellow and magenta)
  if (g == maxval)
    h = 2 + (b - r)/delta ;  //!< (between cyan and yellow)
  if (b == maxval)
    h = 4 + (r - g) / delta; //!< (between magenta and cyan)
  h *= 60.f;                 //!< (convert Hue to degrees)
  if (h < 0.f)
    h += 360.f;              //!< (Hue must be positive)
  else if (h >= 360.f)
    h -= 360.f;              //!< (Hue must be less than 360)

  h *= 256.0f / 360.0f; // range 0 .. 255
  s *= 256.0f;          // range 0 .. 255
}

void brip_vil_float_ops::rgb_to_ihs_tsai(vil_rgb<vxl_byte> const& rgb, float& i, float& h, float& s)
{
  float r=rgb.R();
  float g=rgb.G();
  float b=rgb.B();
  float sq6 = std::sqrt(6.0f);
  float V1, V2;
  i = (r+g+b)/3.0f;
  V1 = (2.0f*b-r-g)/sq6;
  V2 = (r-2.0f*g)/sq6;
  s = std::sqrt(V1*V1 + V2*V2);
  auto two_pi = static_cast<float>(vnl_math::twopi);
  float a = std::atan2(V2, V1)/two_pi;
  if (a<0.0f) a += 1.0f; // range [0..1)
  h = a*256.0f; // range [0..256)
}

void brip_vil_float_ops::ihs_to_rgb(vil_rgb<vxl_byte> & rgb,
                                    const float i, const float h, const float s)
{
  // Reference: page 593 of Foley & van Dam
  float R = 0.0f;
  float G = 0.0f;
  float B = 0.0f;

  if (s == 0) {
    R=i;
    G=i;
    B=i;
  }
  else if (s > 0.0)
  {
    float ss = s, hh = h;
    ss *= 1.f / 256.f;
    hh *= 6.f / 256.f;

    float J = std::floor(hh);
    float F = hh - J;
    float P =( i * (1 - ss));
    float Q = (i * (1 - (ss * F)));
    float T = (i * (1 - (ss * (1 - F))));

    if (J == 0) { R=i; G=T; B=P; }
    if (J == 1) { R=Q; G=i; B=P; }
    if (J == 2) { R=P; G=i; B=T; }
    if (J == 3) { R=P; G=Q; B=i; }
    if (J == 4) { R=T; G=P; B=i; }
    if (J == 5) { R=i; G=P; B=Q; }
  }
  rgb.r = (vxl_byte)R;
  rgb.g = (vxl_byte)G;
  rgb.b = (vxl_byte)B;
}

void brip_vil_float_ops::
convert_to_IHS(vil_image_view<vil_rgb<vxl_byte> > const& image,
               vil_image_view<float>& I,
               vil_image_view<float>& H,
               vil_image_view<float>& S)
{
  unsigned w = image.ni(), h = image.nj();
  I.set_size(w,h);
  H.set_size(w,h);
  S.set_size(w,h);
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      float in, hue, sat;
      rgb_to_ihs(image(c,r), in, hue, sat);
      I(c,r) = in;
      H(c,r) = hue;
      S(c,r) = sat;
    }
}

void brip_vil_float_ops::
convert_to_IHS(vil_image_view<vxl_byte> const& image,
               vil_image_view<float>& I,
               vil_image_view<float>& H,
               vil_image_view<float>& S)
{
  unsigned w = image.ni(), h = image.nj();
  I.set_size(w,h);
  H.set_size(w,h);
  S.set_size(w,h);
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      float in, hue, sat;
      vil_rgb<vxl_byte> imint(image(c,r,0),image(c,r,1),image(c,r,2));
      rgb_to_ihs(imint, in, hue, sat);
      I(c,r) = in;
      H(c,r) = hue;
      S(c,r) = sat;
    }
}

void brip_vil_float_ops::
convert_to_IHS_tsai(vil_image_view<vxl_byte> const& image,
                    vil_image_view<float>& I,
                    vil_image_view<float>& H,
                    vil_image_view<float>& S,
                    vil_image_view<float>& ratio)
{
  unsigned w = image.ni(), h = image.nj();
  I.set_size(w,h);
  H.set_size(w,h);
  S.set_size(w,h);
  ratio.set_size(w,h);
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      float in, hue, sat;
      vil_rgb<vxl_byte> imint(image(c,r,0),image(c,r,1),image(c,r,2));
      rgb_to_ihs_tsai(imint, in, hue, sat);
      I(c,r) = in;
      H(c,r) = hue;
      S(c,r) = sat;
      ratio(c,r) = (hue+1.0f)/(in+1.0f);
    }
}

#if 0 // commented out
void brip_vil_float_ops::
display_IHS_as_RGB(vil_image_view<float> const& I,
                   vil_image_view<float> const& H,
                   vil_image_view<float> const& S,
                   vil_image_view<vil_rgb<vxl_byte> >& image)
{
  unsigned w = I.ni(), h = I.nj();
  image.set_size(w,h);
  float s = 256.0f/360.0f;
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      float in = I(c,r);
      float hue = s * H(c,r);
      float sat = S(c,r);
      if (in<0) in = 0;
      if (sat<0) sat = 0;
      if (hue<0) hue = 0;
      if (in>=256) in = 255;
      if (hue>=256) hue = 255;
      if (sat>=256) sat = 255;
      image(c,r).r = (vxl_byte)in;
      image(c,r).g = (vxl_byte)hue;
      image(c,r).b = (vxl_byte)sat;
    }
}
#endif // 0

// map so that intensity is proportional to saturation and hue is color
void brip_vil_float_ops::
display_IHS_as_RGB(vil_image_view<float> const& I,
                   vil_image_view<float> const& H,
                   vil_image_view<float> const& S,
                   vil_image_view<vil_rgb<vxl_byte> >& image)
{
  unsigned w = I.ni(), h = I.nj();
  image.set_size(w,h);

  const auto deg_to_rad = float(vnl_math::pi_over_180);
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      float hue = H(c,r);
      float sat = 2.f*S(c,r);
      if (sat<0)
        sat = 0.f;
      else if (sat>=256)
        sat = 255.999f;
      float ang = deg_to_rad*hue;
      float cs = std::cos(ang), si = std::fabs(std::sin(ang));
      float red=0.0f, blue=0.0f;
      float green = si*sat;
      if (cs>=0)
        red = cs*sat;
      else
        blue = sat*(-cs);
      image(c,r).r = (vxl_byte)red;
      image(c,r).g = (vxl_byte)green;
      image(c,r).b = (vxl_byte)blue;
    }
}

vil_image_view<vil_rgb<vxl_byte> > brip_vil_float_ops::
combine_color_planes(vil_image_view<vxl_byte> const& R,
                     vil_image_view<vxl_byte> const& G,
                     vil_image_view<vxl_byte> const& B)
{
  unsigned w = R.ni(), h = R.nj();
  vil_image_view<vil_rgb<vxl_byte> > image(w,h);
  for (unsigned r = 0; r < h; r++)
    for (unsigned c = 0; c < w; c++)
    {
      image(c,r).r = R(c,r);
      image(c,r).g = G(c,r);
      image(c,r).b = B(c,r);
    }
  return image;
}

vil_image_view<vil_rgb<vxl_byte> >
brip_vil_float_ops::combine_color_planes(vil_image_resource_sptr const& R,
                                         vil_image_resource_sptr const& G,
                                         vil_image_resource_sptr const& B)
{
  vil_image_view<vil_rgb<vxl_byte> > view(0,0);
  if (!R||!G||!B)
    return view; // return an empty view
  // determine the union of all the resources
  vbl_bounding_box<unsigned int, 2> b;
  unsigned int r_ni = R->ni(), r_nj = R->nj();
  unsigned int g_ni = G->ni(), g_nj = G->nj();
  unsigned int b_ni = B->ni(), b_nj = B->nj();
  b.update(r_ni, r_nj);
  b.update(g_ni, g_nj);
  b.update(b_ni, b_nj);
  unsigned int n_i = b.xmax(), n_j = b.ymax();
  view.set_size(n_i, n_j);
  vil_rgb<vxl_byte> zero(0, 0, 0);
  vil_image_view<float>    fR = brip_vil_float_ops::convert_to_float(R);
  vil_image_view<vxl_byte> cR = brip_vil_float_ops::convert_to_byte(fR);
  vil_image_view<float>    fG = brip_vil_float_ops::convert_to_float(G);
  vil_image_view<vxl_byte> cG = brip_vil_float_ops::convert_to_byte(fG);
  vil_image_view<float>    fB = brip_vil_float_ops::convert_to_float(B);
  vil_image_view<vxl_byte> cB = brip_vil_float_ops::convert_to_byte(fB);
  for (vxl_uint_16 j = 0; j<n_j; ++j)
    for (vxl_uint_16 i = 0; i<n_i; ++i)
    {
      vil_rgb<vxl_byte> v = zero;
      if (i<r_ni&&j<r_nj)
        v.r = cR(i,j);
      if (i<g_ni&&j<g_nj)
        v.g= cG(i,j);
      if (i<b_ni&&j<b_nj)
        v.b= cB(i,j);
      view(i,j)=v;
    }
  return view;
}

vil_image_view<float>
brip_vil_float_ops::convert_to_float(vil_image_resource const& image)
{
  vil_image_view<float> fimg;
  vil_pixel_format fmt = image.pixel_format();
  if (vil_pixel_format_num_components(image.pixel_format())==1)
  {
    if (fmt==VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<unsigned short> temp=image.get_view();
      fimg = brip_vil_float_ops::convert_to_float(temp);
    }
    else if (fmt==VIL_PIXEL_FORMAT_BYTE)
    {
      vil_image_view<unsigned char> temp=image.get_view();
      fimg = brip_vil_float_ops::convert_to_float(temp);
    }
    else if (fmt==VIL_PIXEL_FORMAT_FLOAT)
      return image.get_view();
  }
  else if (vil_pixel_format_num_components(fmt)==3)
  {
    vil_image_view<vil_rgb<vxl_byte> > temp= image.get_view();
    fimg = brip_vil_float_ops::convert_to_float(temp);
  }
  else
  {
    std::cout << "In brip_vil_float_ops::convert_to_float - input not color or grey\n";
    return vil_image_view<float>();
  }
  return fimg;
}

//-----------------------------------------------------------------
//: Convert any image to an unsigned_char image
vil_image_view<vxl_byte>
brip_vil_float_ops::convert_to_grey(vil_image_resource const& image)
{
  // Check if the image is a float
  if (image.nplanes()==1 &&image.pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float> temp = image.get_view();
    float vmin=0.f, vmax=256.f;
    vil_math_value_range<float>(temp, vmin, vmax);
    return brip_vil_float_ops::convert_to_byte(temp, vmin, vmax);
  }
  if (image.nplanes()==1 &&image.pixel_format()==VIL_PIXEL_FORMAT_BOOL)
  {
    vil_image_view<bool> temp = image.get_view();
    unsigned nj = temp.nj(), ni = temp.ni();
    vil_image_view<unsigned char> out(ni, nj);
    out.fill(0);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        if (temp(i,j)) out(i,j) = 255;
    return out;
  }

  // Here we assume that the image is an unsigned char
  // In this case we should just return it.
  if (image.nplanes()==1&&image.pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char > temp = image.get_view();
    return temp;
  }

  if (image.nplanes()==1&&image.pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short > temp = image.get_view();
    unsigned short vmin=0, vmax=255;
    vil_math_value_range<unsigned short>(temp, vmin, vmax);
    return brip_vil_float_ops::convert_to_byte(temp, vmin, vmax);
  }
  // the image is color so we should convert it to greyscale
  // Here we assume the color elements are unsigned char.
  if (image.nplanes()==3&&image.pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vil_rgb<vxl_byte> > color_image = image.get_view();
    unsigned width = color_image.ni(), height = color_image.nj();
    // the output image
    vil_image_view<unsigned char> grey_image;
    grey_image.set_size(width, height);
    for (unsigned y = 0; y<height; y++)
      for (unsigned x = 0; x<width; x++)
        grey_image(x,y) = color_image(x,y).grey();
    return grey_image;
  }
  if (image.nplanes()==3&&image.pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float> color_image = image.get_view();
    unsigned width = color_image.ni(), height = color_image.nj();
    // the output image
    vil_image_view<float> grey_image_f;
    grey_image_f.set_size(width, height);
    for (unsigned y = 0; y<height; y++)
      for (unsigned x = 0; x<width; x++) {
        float v = 0;
        for (unsigned p = 0; p<3; ++p)
          v += color_image(x,y,p);
        grey_image_f(x,y) = v/3.0f;
      }
    float vmin=0.f, vmax=256.f;
    vil_math_value_range<float>(grey_image_f, vmin, vmax);
    return brip_vil_float_ops::convert_to_byte(grey_image_f, vmin, vmax);
  }
  // If we get here then the input is not a type we handle so return a null view
  return vil_image_view<vxl_byte>();
}

//--------------------------------------------------------------
//: Read a convolution kernel from file
// Assumes a square kernel with odd dimensions, i.e., w,h = 2n+1
// format:
// \verbatim
//     n
//     scale
//     k00  k01  ... k02n
//           ...
//     k2n0 k2n1 ... k2n2n
// \endverbatim
//
vbl_array_2d<float> brip_vil_float_ops::load_kernel(std::string const& file)
{
  std::ifstream instr(file.c_str(), std::ios::in);
  if (!instr)
  {
    std::cout << "In brip_vil_float_ops::load_kernel - failed to load kernel\n";
    return vbl_array_2d<float>(0,0);
  }
  unsigned n;
  float scale;
  float v =0.0f;
  instr >> n;
  instr >> scale;
  unsigned N = 2*n+1;
  vbl_array_2d<float> output(N, N);
  for (unsigned y = 0; y<N; y++)
    for (unsigned x = 0; x<N; x++)
    {
      instr >> v;
      output.put(x, y, v/scale);
    }
#ifdef DEBUG
  std::cout << "The Kernel\n";
  for (unsigned y = 0; y<N; y++)
  {
    for (unsigned x = 0; x<N; x++)
      std::cout << ' ' <<  output[x][y];
    std::cout << '\n';
  }
#endif
  return output;
}

static void insert_image(vil_image_view<float> const& image, int col,
                         vnl_matrix<float> & I)
{
  unsigned width = image.ni(), height = image.nj(), row=0;
  for (unsigned y =0; y<height; y++)
    for (unsigned x = 0; x<width; x++, row++)
      I.put(row, col, image(x,y));
}

void brip_vil_float_ops::
basis_images(std::vector<vil_image_view<float> > const& input_images,
             std::vector<vil_image_view<float> >& basis)
{
  basis.clear();
  unsigned n_images = input_images.size();
  if (!n_images)
  {
    std::cout << "In brip_vil_float_ops::basis_images(.) - no input images\n";
    return;
  }
  unsigned width = input_images[0].ni(), height = input_images[0].nj();
  unsigned npix = width*height;

  // Insert the images into matrix I
  vnl_matrix<float> I(npix, n_images, 0.f);
  for (unsigned i = 0; i<n_images; i++)
    insert_image(input_images[i], i, I);

  // Compute the SVD of matrix I
#ifdef DEBUG
  std::cout << "Computing Singular values of a " <<  npix << " by "
           << n_images << " matrix\n";
  vul_timer t;
#endif
  vnl_svd<float> svd(I);
#ifdef DEBUG
  std::cout << "SVD Took " << t.real() << " msecs\n"
           << "Eigenvalues:\n";
  for (unsigned i = 0; i<n_images; i++)
    std::cout << svd.W(i) << '\n';
#endif
  // Extract the Basis images
  unsigned rank = svd.rank();
  if (!rank)
  {
    std::cout << "In brip_vil_float_ops::basis_images(.) - I has zero rank\n";
    return;
  }
  vnl_matrix<float> U = svd.U();
  // Output the basis images
  unsigned rows = U.rows();
  for (unsigned k = 0; k<rank; k++)
  {
    vil_image_view<float> out(width, height);
    unsigned x =0, y = 0;
    for (unsigned r = 0; r<rows; r++)
    {
      out(x, y) = U(r,k);
      x++;
      if (x>=width)
      {
        y++;
        x=0;
      }
      if (y>=width)
      {
        std::cout<<"In brip_vil_float_ops::basis_images(.) - shouldn't happen\n";
        return;
      }
    }
    basis.push_back(out);
  }
}

//: 1d fourier transform
//-------------------------------------------------------------------------
// This computes an in-place complex-to-complex FFT
// x and y are the real and imaginary arrays of 2^m points.
// dir =  1 gives forward transform
// dir = -1 gives reverse transform
//
//   Formula: forward
// \verbatim
//                N-1
//                ---
//            1   \          - j k 2 pi n / N
//    X(n) = ---   >   x(k) e                    = forward transform
//            N   /                                n=0..N-1
//                ---
//                k=0
// \endverbatim
//
//    Formula: reverse
// \verbatim
//                N-1
//                ---
//                \          j k 2 pi n / N
//    X(n) =       >   x(k) e                    = forward transform
//                /                                n=0..N-1
//                ---
//                k=0
// \endverbatim
//
bool brip_vil_float_ops::fft_1d(int dir, int m, double* x, double* y)
{
  long nn,i,i1,j,k,i2,l,l1,l2;
  double c1,c2,tx,ty,t1,t2,u1,u2,z;

  // Calculate the number of points
  nn = 1;
  for (i=0;i<m;i++)
    nn *= 2;

  // Do the bit reversal
  i2 = nn >> 1;
  j = 0;
  for (i=0;i<nn-1;i++) {
    if (i < j) {
      tx = x[i];
      ty = y[i];
      x[i] = x[j];
      y[i] = y[j];
      x[j] = tx;
      y[j] = ty;
    }
    k = i2;
    while (k <= j) {
      j -= k;
      k >>= 1;
    }
    j += k;
  }

  // Compute the FFT
  c1 = -1.0;
  c2 = 0.0;
  l2 = 1;
  for (l=0;l<m;l++) {
    l1 = l2;
    l2 <<= 1;
    u1 = 1.0;
    u2 = 0.0;
    for (j=0;j<l1;j++) {
      for (i=j;i<nn;i+=l2) {
        i1 = i + l1;
        t1 = u1 * x[i1] - u2 * y[i1];
        t2 = u1 * y[i1] + u2 * x[i1];
        x[i1] = x[i] - t1;
        y[i1] = y[i] - t2;
        x[i] += t1;
        y[i] += t2;
      }
      z =  u1 * c1 - u2 * c2;
      u2 = u1 * c2 + u2 * c1;
      u1 = z;
    }
    c2 = std::sqrt((1.0 - c1) / 2.0);
    if (dir == 1)
      c2 = -c2;
    c1 = std::sqrt((1.0 + c1) / 2.0);
  }

  // Scaling for forward transform
  if (dir == 1) {
    for (i=0;i<nn;i++) {
      x[i] /= (double)nn;
      y[i] /= (double)nn;
    }
  }

  return true;
}

//-------------------------------------------------------------------------
//: Perform a 2D FFT inplace given a complex 2D array
//  The direction dir, 1 for forward, -1 for reverse
//  The size of the array (nx,ny)
//  Return false if there are memory problems or
//  the dimensions are not powers of 2
//
bool brip_vil_float_ops::fft_2d(vnl_matrix<std::complex<double> >& c,int nx,int ny,int dir)
{
  int i,j;
  int mx, my;
  double *real,*imag;
  vnl_fft_prime_factors<double> pfx (nx);
  vnl_fft_prime_factors<double> pfy (ny);
  mx = (int)pfx.pqr()[0];
  my = (int)pfy.pqr()[0];
  // Transform the rows
  real = new double[nx];
  imag = new double[nx];
  if (real == nullptr || imag == nullptr)
    return false;
  for (j=0;j<ny;j++) {
    for (i=0;i<nx;i++) {
      real[i] = c[j][i].real();
      imag[i] = c[j][i].imag();
    }
    brip_vil_float_ops::fft_1d(dir,mx,real,imag);
    for (i=0;i<nx;i++) {
      std::complex<double> v(real[i], imag[i]);
      c[j][i] = v;
    }
  }
  delete [] real;
  delete [] imag;
  // Transform the columns
  real = new double[ny];
  imag = new double[ny];
  if (real == nullptr || imag == nullptr)
    return false;
  for (i=0;i<nx;i++) {
    for (j=0;j<ny;j++) {
      real[j] = c[j][i].real();
      imag[j] = c[j][i].imag();
    }
    fft_1d(dir,my,real,imag);
    for (j=0;j<ny;j++) {
      std::complex<double> v(real[j], imag[j]);
      c[j][i] =  v;
    }
  }
  delete [] real;
  delete [] imag;
  return true;
}

//: reorder the transform values to sequential frequencies as in conventional Fourier transforms.
//  The transformation is its self-inverse.
void brip_vil_float_ops::
ftt_fourier_2d_reorder(vnl_matrix<std::complex<double> > const& F1,
                       vnl_matrix<std::complex<double> > & F2)
{
  int rows = static_cast<int>(F1.rows()), cols = static_cast<int>(F1.cols());
  int half_rows = rows/2, half_cols = cols/2;
  int ri, ci;
  for (int r = 0; r<rows; r++)
  {
    if (r<half_rows)
      ri = half_rows+r;
    else
      ri = r-half_rows;
    for (int c = 0; c<cols; c++)
    {
      if (c<half_cols)
        ci = half_cols+c;
      else
        ci = c-half_cols;
      F2[ri][ci]=F1[r][c];
    }
  }
}

//: Compute the fourier transform.
//  If the image dimensions are not a power of 2 then the operation fails.
bool brip_vil_float_ops::
fourier_transform(vil_image_view<float> const& input,
                  vil_image_view<float>& mag,
                  vil_image_view<float>& phase)
{
  unsigned w = input.ni(), h = input.nj();

  vnl_fft_prime_factors<float> pfx (w);
  vnl_fft_prime_factors<float> pfy (h);
  if (!pfx.pqr()[0]||!pfy.pqr()[0])
    return false;
  // fill the fft matrix
  vnl_matrix<std::complex<double> > fft_matrix(h, w), fourier_matrix(h,w);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x =0; x<w; x++)
    {
      std::complex<double> cv(input(x,y), 0.0);
      fft_matrix.put(y, x, cv);
    }
#ifdef DEBUG
  for (unsigned r = 0; r<h; r++)
    for (unsigned c =0; c<w; c++)
    {
      std::complex<double> res = fft_matrix[r][c];
      std::cout << res << '\n';
    }
#endif

  brip_vil_float_ops::fft_2d(fft_matrix, w, h, 1);
  brip_vil_float_ops::ftt_fourier_2d_reorder(fft_matrix, fourier_matrix);
  mag.set_size(w,h);
  phase.set_size(w,h);

  // extract magnitude and phase
  for (unsigned r = 0; r<h; r++)
    for (unsigned c = 0; c<w; c++)
    {
      auto re = (float)fourier_matrix[r][c].real(),
        im = (float)fourier_matrix[r][c].imag();
      mag(c,r) = std::sqrt(re*re + im*im);
      phase(c,r) = std::atan2(im, re);
    }
  return true;
}

bool brip_vil_float_ops::
inverse_fourier_transform(vil_image_view<float> const& mag,
                          vil_image_view<float> const& phase,
                          vil_image_view<float>& output)
{
  unsigned w = mag.ni(), h = mag.nj();
  vnl_matrix<std::complex<double> > fft_matrix(h, w), fourier_matrix(h, w);
  for (unsigned y = 0; y<h; y++)
    for (unsigned x =0; x<w; x++)
    {
      float m = mag(x,y);
      float p = phase(x,y);
      std::complex<double> cv(m*std::cos(p), m*std::sin(p));
      fourier_matrix.put(y, x, cv);
    }

  brip_vil_float_ops::ftt_fourier_2d_reorder(fourier_matrix, fft_matrix);
  brip_vil_float_ops::fft_2d(fft_matrix, w, h, -1);

  output.set_size(w,h);

  for (unsigned y = 0; y<h; y++)
    for (unsigned x = 0; x<w; x++)
      output(x,y) = (float)fft_matrix[y][x].real();
  return true;
}

void brip_vil_float_ops::resize(vil_image_view<float> const& input,
                                unsigned width, unsigned height,
                                vil_image_view<float>& output)
{
  unsigned w = input.ni(), h = input.nj();
  output.set_size(width, height);
  for (unsigned y = 0; y<height; y++)
    for (unsigned x = 0; x<width; x++)
      if (x<w && y<h)
        output(x,y) = input(x,y);
      else
        output(x,y) = 0; // pad with zeroes
}

//: resize the input to the closest power of two image dimensions
bool brip_vil_float_ops::
resize_to_power_of_two(vil_image_view<float> const& input,
                       vil_image_view<float>& output)
{
  unsigned max_exp = 13; // we wouldn't want to have such large images in memory
  unsigned w = input.ni(), h = input.nj();
  unsigned prodw = 1, prodh = 1;
  // Find power of two width
  unsigned nw, nh;
  for (nw = 1; nw<=max_exp; nw++)
    if (prodw>w)
      break;
    else
      prodw *= 2;
  if (nw==max_exp)
    return false;
  // Find power of two height
  for (nh = 1; nh<=max_exp; nh++)
    if (prodh>h)
      break;
    else
      prodh *= 2;
  if (nh==max_exp)
    return false;
  brip_vil_float_ops::resize(input, prodw, prodh, output);

  return true;
}

//
//: block a periodic signal by suppressing two Gaussian lobes in the frequency domain.
//  The lobes are on the line defined by dir_fx and dir_fy through the
//  dc origin, assumed (0, 0).  The center frequency, f0, is the distance along
//  the line to the center of each blocking lobe (+- f0). radius is the
//  standard deviation of each lobe. Later we can define a "filter" class.
//
float brip_vil_float_ops::gaussian_blocking_filter(float dir_fx,
                                                   float dir_fy,
                                                   float f0,
                                                   float radius,
                                                   float fx,
                                                   float fy)
{
  // normalize dir_fx and dir_fy
  float mag = std::sqrt(dir_fx*dir_fx + dir_fy*dir_fy);
  if (!mag)
    return 0.f;
  float r2 = 2.f*radius*radius;
  float dx = dir_fx/mag, dy = dir_fy/mag;
  // compute the centers of each lobe
  float fx0p = dx*f0, fy0p = dy*f0;
  float fx0m = -dx*f0, fy0m = -dy*f0;
  // the squared distance of fx, fy from each center
  float d2p = (fx-fx0p)*(fx-fx0p) + (fy-fy0p)*(fy-fy0p);
  float d2m = (fx-fx0m)*(fx-fx0m) + (fy-fy0m)*(fy-fy0m);
  // use the closest lobe
  float d = d2p;
  if (d2m<d2p)
    d = d2m;
  // the gaussian blocking function
  float gb = 1.f-(float)std::exp(-d/r2);
  return gb;
}

bool brip_vil_float_ops::
spatial_frequency_filter(vil_image_view<float> const& input,
                         float dir_fx, float dir_fy,
                         float f0, float radius,
                         bool output_fourier_mag,
                         vil_image_view<float>& output)
{
  // Compute the fourier transform of the image.
  vil_image_view<float> pow_two, mag, bmag, phase, pow_two_filt;
  brip_vil_float_ops::resize_to_power_of_two(input, pow_two);
  unsigned Nfx = pow_two.ni(), Nfy = pow_two.nj();

  if (!brip_vil_float_ops::fourier_transform(pow_two, mag, phase))
    return false;
  bmag.set_size(Nfx, Nfy);

  // filter the magnitude function
  float Ofx = Nfx*0.5f, Ofy = Nfy*0.5f;
  for (unsigned fy =0; fy<Nfy; fy++)
    for (unsigned fx =0; fx<Nfx; fx++)
    {
      float gb = gaussian_blocking_filter(dir_fx, dir_fy, f0,
                                          radius,
                                          (float)fx-Ofx, (float)fy-Ofy);
      bmag(fx,fy) = mag(fx,fy)*gb;
    }
  if (output_fourier_mag)
  {
    output = bmag;
    return true;
  }
  // Transform back
  pow_two_filt.set_size(Nfx, Nfy);
  brip_vil_float_ops::inverse_fourier_transform(bmag, phase, pow_two_filt);

  // Resize to original input size
  brip_vil_float_ops::resize(pow_two_filt, input.ni(), input.nj(), output);
  return true;
}

//----------------------------------------------------------------------
//: Bi-linear interpolation on the neighborhood below.
// \verbatim
//      xr
//   yr 0  x
//      x  x
// \endverbatim
//
double brip_vil_float_ops::
bilinear_interpolation(vil_image_view<float> const& input, double x, double y)
{
  // check bounds
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  // the pixel containing the interpolated point
  int xr = (int)x, yr = (int)y;
  double fx = x-xr, fy = y-yr;
  if (xr<0||xr>w-2)
    return 0.0;
  if (yr<0||yr>h-2)
    return 0.0;
  double int00 = input(xr, yr), int10 = input(xr+1,yr);
  double int01 = input(xr, yr+1), int11 = input(xr+1,yr+1);
  double int0 = int00 + fy * (int01 - int00);
  double int1 = int10 + fy * (int11 - int10);
  auto val = (float) (int0 + fx * (int1 - int0));
  return val;
}

//: Transform the input to the output by a homography.
//  If the output size is fixed then only the corresponding
//  region of input image space is transformed.
bool brip_vil_float_ops::homography(vil_image_view<float> const& input,
                                    vgl_h_matrix_2d<double> const& H,
                                    vil_image_view<float>& output,
                                    bool output_size_fixed,
                                    float output_fill_value)
{
  if (!input)
    return false;
  // smooth the input to condition interpolation
  vil_image_view<float> gimage =
    brip_vil_float_ops::gaussian(input, 0.5f);

  // First, there is some rather complex bookeeping to insure that
  // the input and output image rois are consistent with the homography.

  // the bounding boxes corresponding to input and output rois
  // We also construct polygons since homographies turn boxes into arbitrary
  // quadrilaterals.
  vsol_box_2d_sptr input_roi, output_roi;
  vsol_polygon_2d_sptr input_poly, output_poly;
  vgl_h_matrix_2d<double> Hinv;
  // set up the roi and poly for the input image
  unsigned win = gimage.ni(), hin = gimage.nj();
  input_roi = new vsol_box_2d();
  input_roi->add_point(0, 0);
  input_roi->add_point(win, hin);
  input_poly = bsol_algs::poly_from_box(input_roi);
  // Case I
  //  the output image size and input transform can be adjusted
  //  to map the transformed image onto the full range
  if (!output_size_fixed)
  {
    if (!bsol_algs::homography(input_poly, H, output_poly))
      return false;
    vsol_box_2d_sptr temp = output_poly->get_bounding_box();
    output.set_size((int)temp->width(), (int)temp->height());
    output.fill(output_fill_value);
    //offset the transform so the origin is (0,0)
    output_roi = new vsol_box_2d();
    output_roi->add_point(0, 0);
    output_roi->add_point(temp->width(), temp->height());
    vgl_h_matrix_2d<double> t; t.set_identity().set_translation(-temp->get_min_x(),-temp->get_min_y());
    Hinv = (t*H).get_inverse();
  }
  else // Case II, the output image size is fixed so we have to find the
  {    // inverse mapping of the output roi and intersect with the input roi
    // to determine the domain of the mapping
    if (!output)
      return false;
    //The output roi and poly
    unsigned wout = output.ni(), hout = output.nj();
    output.fill(output_fill_value);
    output_roi = new vsol_box_2d();
    output_roi->add_point(0, 0);
    output_roi->add_point(wout, hout);
    output_poly = bsol_algs::poly_from_box(output_roi);

    //Construct the reverse mapping of the output bounds
    vsol_polygon_2d_sptr tpoly;
    Hinv = H.get_inverse();
    if (!bsol_algs::homography(output_poly, Hinv, tpoly))
      return false;

    //form the roi corresponding to the inverse mapped output bounds
    vsol_box_2d_sptr tbox = tpoly->get_bounding_box();

    //intersect with the input image bounds to get the input roi
    vsol_box_2d_sptr temp;
    if (!bsol_algs::intersection(tbox, input_roi, temp))
      return false;
    input_roi = temp;
  }
  // At this point we have the correct bounds for the input and
  // the output image

  // Iterate over the output image space and map the location of each
  // pixel into the input image space. Then carry out interpolation to
  // get the value of each output pixel

  // Dimensions of the input image
  int ailow  = int(input_roi->get_min_x()+0.9999f); // round up to nearest int
  int aihigh = int(input_roi->get_max_x());      // round down to nearest int
  int ajlow  = int(input_roi->get_min_y()+0.9999f);
  int ajhigh = int(input_roi->get_max_y());

  // Dimensions of the output image
  int bilow  = int(output_roi->get_min_x()+0.9999f);
  int bihigh = int(output_roi->get_max_x());
  int bjlow  = int(output_roi->get_min_y()+0.9999f);
  int bjhigh = int(output_roi->get_max_y());

  // The inverse transform is used to map backwards from the output
  const vnl_matrix_fixed<double,3,3>& Minv = Hinv.get_matrix();

  // Now use Hinv to transform the image
  for (int i = bilow; i<bihigh; i++)
    for (int j = bjlow; j<bjhigh; j++)
    {
      // Transform the pixel
      float val;
      double u = Minv[0][0] * i + Minv[0][1] * j + Minv[0][2];
      double v = Minv[1][0] * i + Minv[1][1] * j + Minv[1][2];
      double w = Minv[2][0] * i + Minv[2][1] * j + Minv[2][2];
      u /= w;
      v /= w;

      // Now do linear interpolation
      {
        int iu = (int) u;
        int iv = (int) v;

        if (iu >= ailow && iu < aihigh-1 &&
            iv >= ajlow && iv < ajhigh-1)
        {
          // Get the neighbouring pixels
          //      (u  v)    (u+1  v)
          //      (u v+1)   (u+1 v+1)
          //
          double v00 = gimage(iu, iv);
          double v01 = gimage(iu, iv+1);
          double v10 = gimage(iu+1,iv);
          double v11 = gimage(iu+1, iv+1);

          double fu = u - iu;
          double fv = v - iv;
          double v0 = v00 + fv * (v01 - v00);
          double v1 = v10 + fv * (v11 - v10);
          val = (float) (v0 + fu * (v1 - v0));
          // Set the value
          output(i,j) = val;
        }
      }
    }
  return true;
}

//: rotate the input image counter-clockwise about the image origin.
// Demonstrates the use of image homography
vil_image_view<float>
brip_vil_float_ops::rotate(vil_image_view<float> const& input,
                           double theta_deg)
{
  vil_image_view<float> out;
  if (!input)
    return out;
  double ang = theta_deg;
  // map theta_deg to [0 360]
  while (ang>360)
    ang-=360;
  while (ang<0)
    ang+=360;
  // convert to radians
  double deg_to_rad = vnl_math::pi_over_180;
  double rang = deg_to_rad*ang;
  vgl_h_matrix_2d<double> H;
  H.set_identity().set_rotation(rang);
  vil_image_view<float> temp;
  // The transform is adjusted to map the full input domain onto
  // the output image.
  if (!brip_vil_float_ops::homography(input, H, temp))
    return out;
  return temp;
}

bool brip_vil_float_ops::chip(vil_image_view<float> const& input,
                              vsol_box_2d_sptr const& roi,
                              vil_image_view<float>& chip)
{
  if (!input||!roi)
    return false;
  int w = static_cast<int>(input.ni()), h = static_cast<int>(input.nj());
  int x_min = (int)roi->get_min_x(), y_min = (int)roi->get_min_y();
  int x_max = (int)roi->get_max_x(), y_max = (int)roi->get_max_y();
  if (x_min<0)
    x_min = 0;
  if (y_min<0)
    y_min = 0;
  if (x_max>w-1)
    x_max=w-1;
  if (y_max>h-1)
    y_max=h-1;
  int rw = x_max-x_min, rh = y_max-y_min;
  if (rw<=0||rh<=0)
    return false;
  vil_image_view<float> temp(rw+1, rh+1, 1);
  for (int y = y_min; y<=y_max; y++)  //!< changed < to <= to include the boundary points too
    for (int x =x_min; x<=x_max; x++)
      temp(x-x_min, y-y_min) = input(x, y);
  chip = temp;
  return true;
}

//: convert image resource to cropped view according to a roi.
bool brip_vil_float_ops::chip(vil_image_resource_sptr const& image,
                              brip_roi_sptr const& roi,
                              vil_image_resource_sptr & chip)
{
  // image bounds
  unsigned ni = image->ni(), nj = image->nj();

  // image bounds for the chip
  unsigned cm = roi->cmin(0), rm = roi->rmin(0);
  unsigned niv = roi->csize(0), njv = roi->rsize(0);
  std::cout << "org(" << cm << ' ' << rm << "):size(" << niv
           << ' ' << njv << ")\n" << std::flush;
  // check bounds
  if (cm>ni-1||rm>nj-1||cm+niv>ni||rm+njv>nj)
    return false;
  vil_pixel_format pix_format = image->pixel_format();
  // get an appropriate image view for scalar images we care about
  if (image->nplanes()==1)
  {
    if (pix_format==VIL_PIXEL_FORMAT_BYTE)
    {
      vil_image_view<unsigned char> temp = image->get_view(cm, niv, rm, njv);
      if (!temp) return false;
      chip = vil_new_image_resource_of_view(temp);
      return true;
    }
    else if (pix_format==VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<unsigned short> temp = image->get_view(cm, niv, rm, njv);
      if (!temp) return false;
      chip = vil_new_image_resource_of_view(temp);
      std::cout << "resc(" << chip->ni() << ' ' << chip->nj()<< ")\n"
               << std::flush;
      return true;
    }
    else if (pix_format==VIL_PIXEL_FORMAT_FLOAT)
    {
      vil_image_view<float> temp = image->get_view(cm, niv, rm, njv);
      if (!temp) return false;
      chip = vil_new_image_resource_of_view(temp);
      return true;
    }
  }

  // color data
  if (image->nplanes()==3)
  {
    if (pix_format==VIL_PIXEL_FORMAT_BYTE) //the only way now
    {
      //extract view corresponding to region of interest
      vil_image_view<vil_rgb<vxl_byte> > temp = image->get_view(cm, niv, rm, njv);
      if (!temp) return false;
      chip = vil_new_image_resource_of_view(temp);
      return true;
    }
  }
  return false;
}

bool brip_vil_float_ops::
chip(std::vector<vil_image_resource_sptr> const& images,
     brip_roi_sptr const& roi,
     std::vector<vil_image_resource_sptr>& chips)
{
  for (const auto & image : images) {
    vil_image_resource_sptr chip;
    if (!brip_vil_float_ops::chip(image, roi, chip))
      return false;
    chips.push_back(chip);
  }
  return true;
}

//: perform normalized cross-correlation at a sub-pixel location.
// Thus all the pixel values are interpolated.
float brip_vil_float_ops::
cross_correlate(vil_image_view<float> const& image1,
                vil_image_view<float> const& image2,
                float x, float y, int radius, float intensity_thresh)
{
  unsigned w1 = image1.ni(), h1 = image1.nj();
  unsigned w2 = image1.ni(), h2 = image1.nj();
  // bounds checks
  if (w1!=w2||h1!=h2)
    return -1;
  if (x<radius||x>w1-radius-1||y<radius||y>h1-radius-1)
    return -1;

  // accumulate correlation sums,
  // bi-linear interpolate the values
  int s = 2*radius+1;
  double area = s*s;
  double sI1=0, sI2=0, sI1I1=0, sI2I2=0, sI1I2=0;
  for (int y0 = -10*radius; y0<=10*radius; ++y0)
    for (int x0 = -10*radius; x0<=10*radius; ++x0)
    {
      float xp = x+0.1f*(float)x0, yp = y+0.1f*(float)y0;
      double v1 = brip_vil_float_ops::bilinear_interpolation(image1, xp, yp);
      double v2 = brip_vil_float_ops::bilinear_interpolation(image2, xp, yp);
      sI1 += v1;
      sI2 += v2;
      sI1I1 += v1*v1;
      sI2I2 += v2*v2;
      sI1I2 += v1*v2;
    }
  // compute correlation.
  float cc = cross_corr(area, sI1, sI2, sI1I1, sI2I2, sI1I2, intensity_thresh);
  return cc;
}

//: r0 is the image from which to read the new intensity values.
//  \param r is the summing array row in which the values are to be accumulated
static bool update_row(vil_image_view<float> const& image1,
                       vil_image_view<float> const& image2,
                       int r0, int r,
                       vbl_array_2d<double>& SI1,
                       vbl_array_2d<double>& SI2,
                       vbl_array_2d<double>& SI1I1,
                       vbl_array_2d<double>& SI2I2,
                       vbl_array_2d<double>& SI1I2)
{
  unsigned w1 = image1.ni();
  unsigned w2 = image2.ni();
  int h1 = image1.nj();
  int h2 = image2.nj();
  if (w1!=w2||h1!=h2||r<0||r>=h1)
    return false;
  double i10 = image1(0,r0), i20 = image2(0,r0);
  SI1[r][0] = i10; SI2[r][0] = i20; SI1I1[r][0]=i10*i10;
  SI2I2[r][0]=i20*i20; SI1I2[r][0]=i10*i20;
  for (unsigned c = 1; c<w1; c++)
  {
    double i1c = image1(c,r0);
    double i2c = image2(c,r0);
    SI1[r][c]    = SI1[r][c-1]+i1c;
    SI2[r][c]    = SI2[r][c-1]+i2c;
    SI1I1[r][c]  = SI1I1[r][c-1]+ i1c*i1c;
    SI2I2[r][c]  = SI2I2[r][c-1]+ i2c*i2c;
    SI1I2[r][c]  = SI1I2[r][c-1]+ i1c*i2c;
  }
  return true;
}

static bool initialize_slice(vil_image_view<float> const& image1,
                             vil_image_view<float> const& image2,
                             unsigned radius,
                             vbl_array_2d<double>& SI1,
                             vbl_array_2d<double>& SI2,
                             vbl_array_2d<double>& SI1I1,
                             vbl_array_2d<double>& SI2I2,
                             vbl_array_2d<double>& SI1I2)
{
  for (unsigned r = 0; r<=2*radius; r++)
    if (!update_row(image1, image2, r, r, SI1, SI2, SI1I1, SI2I2, SI1I2))
      return false;
  return true;
}

static bool collapse_slice(vbl_array_2d<double> const& SI1,
                           vbl_array_2d<double> const& SI2,
                           vbl_array_2d<double> const& SI1I1,
                           vbl_array_2d<double> const& SI2I2,
                           vbl_array_2d<double> const& SI1I2,
                           vbl_array_1d<double>& dSI1,
                           vbl_array_1d<double>& dSI2,
                           vbl_array_1d<double>& dSI1I1,
                           vbl_array_1d<double>& dSI2I2,
                           vbl_array_1d<double>& dSI1I2)
{
  // sanity check
  unsigned w = SI1.cols(), h = SI1.rows();
  unsigned dw = dSI1.size();
  if (dw!=w)
    return false;

  for (unsigned c = 0; c<w; c++)
  {
    dSI1[c]=0; dSI2[c]=0; dSI1I1[c]=0;
    dSI2I2[c]=0; dSI1I2[c]=0;
    for (unsigned r = 0; r<h; r++)
    {
      dSI1[c] += SI1[r][c];
      dSI2[c] += SI2[r][c];
      dSI1I1[c] += SI1I1[r][c];
      dSI2I2[c] += SI2I2[r][c];
      dSI1I2[c] += SI1I2[r][c];
    }
  }
  return true;
}

static bool cross_correlate_row(int radius,
                                vbl_array_1d<double>& dSI1,
                                vbl_array_1d<double>& dSI2,
                                vbl_array_1d<double>& dSI1I1,
                                vbl_array_1d<double>& dSI2I2,
                                vbl_array_1d<double>& dSI1I2,
                                float intensity_thresh,
                                vbl_array_1d<float>& cc)
{
  // sanity check
  unsigned w = dSI1.size(), wc = cc.size();
  if (!w||!wc||w!=wc)
    return false;
  int s = 2*radius+1;
  double area = s*s;
  // the general case
  double si1=dSI1[s-1], si2=dSI2[s-1], si1i1=dSI1I1[s-1],
    si2i2=dSI2I2[s-1], si1i2=dSI1I2[s-1];
  cc[radius]= cross_corr(area, si1, si2, si1i1, si2i2, si1i2, intensity_thresh);
  // the remaining columns
  for (unsigned c = radius+1; c+radius<w; c++)
  {
    si1=dSI1[c+radius]-dSI1[c-radius-1];
    si2=dSI2[c+radius]-dSI2[c-radius-1];
    si1i1=dSI1I1[c+radius]-dSI1I1[c-radius-1];
    si2i2=dSI2I2[c+radius]-dSI2I2[c-radius-1];
    si1i2=dSI1I2[c+radius]-dSI1I2[c-radius-1];
    cc[c] = cross_corr(area, si1, si2, si1i1, si2i2, si1i2, intensity_thresh);
  }
  return true;
}

static void advance_rows(vbl_array_2d<double>& S)
{
  unsigned nr = S.rows(), nc = S.cols();
  for (unsigned r = 0; r<nr-1; r++)
    for (unsigned c =0; c<nc; c++)
      S[r][c]=S[r+1][c];
}

static bool output_cc_row(int r0, vbl_array_1d<float> const& cc,
                          vil_image_view<float>& out)
{
  unsigned n = cc.size(), w = out.ni();
  if (n!=w)
    return false;
  for (unsigned c = 0; c<w; c++)
    out(c, r0) = cc[c];
  return true;
}

bool brip_vil_float_ops::
cross_correlate(vil_image_view<float> const& image1,
                vil_image_view<float> const& image2,
                vil_image_view<float>& out,
                int radius, float intensity_thresh)
{
  vul_timer t;
  unsigned w = image1.ni(), h = image1.nj();
  unsigned w2 = image2.ni(), h2 = image2.nj();
  // sizes must match
  if (w!=w2||h!=h2)
  {
    std::cout << "In brip_vil_float_ops::cross_correlate(..) -"
             << " image sizes don't match\n";
    return out;
  }
  out.set_size(w, h);
  out.fill(0.f);
  int s = 2*radius+1;
  // Create the running sum slices
  vbl_array_2d<double> SI1(s,w), SI2(s,w),
    SI1I1(s,w), SI2I2(s,w), SI1I2(s,w);
  vbl_array_1d<float> cc(w, 0.f);
  vbl_array_1d<double> dSI1(w, 0.0), dSI2(w, 0.0),
    dSI1I1(w, 0.0), dSI2I2(w, 0.0), dSI1I2(w, 0.0);
  initialize_slice(image1, image2, radius, SI1, SI2, SI1I1, SI2I2, SI1I2);
  if (!collapse_slice(SI1,  SI2,  SI1I1,  SI2I2,  SI1I2,
                      dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2))
    return false;
  unsigned r0 = radius;
  for (; r0+radius+1<h; r0++)
  {
    if (r0==5)
      r0=5;
#ifdef DEBUG
    std::cout << "r0 " << r0 << '\n';
#endif
    if (!cross_correlate_row(radius, dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2,
                             intensity_thresh, cc))
      return false;
#ifdef DEBUG
    std::cout << '\n';
#endif
    advance_rows(SI1); advance_rows(SI2);  advance_rows(SI1I1);
    advance_rows(SI2I2); advance_rows(SI1I2);
    if (!update_row(image1, image2, r0+radius+1, 2*radius,
                    SI1, SI2, SI1I1, SI2I2, SI1I2))
      return false;
    if (!collapse_slice(SI1,  SI2,  SI1I1,  SI2I2,  SI1I2,
                        dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2))
      return false;
    if (!output_cc_row(r0, cc, out))
      return out;
  }
  // handle the last row
#ifdef DEBUG
  std::cout << "r0 " << r0 << '\n';
#endif
  if (!cross_correlate_row(radius, dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2,
                           intensity_thresh, cc))
    return false;
#ifdef DEBUG
  std::cout << '\n';
#endif
  if (!output_cc_row(r0, cc, out))
    return false;
#ifdef DEBUG
  std::cout << "RunningSumCrossCorrelation for " << w*h/1000.0f << " k pixels in "
           << t.real() << " msecs\n"<< std::flush;
#endif
  return true;
}

vil_image_resource_sptr
brip_vil_float_ops::sum(vil_image_resource_sptr const& img0,
                        vil_image_resource_sptr const& img1)
{
  vil_image_view<float> op0 = brip_vil_float_ops::convert_to_float(img0);
  vil_image_view<float> op1 = brip_vil_float_ops::convert_to_float(img1);
  vil_image_view<float> sum;
  vil_math_image_sum(op0, op1, sum);

  // find out the types of the input images for now, only do greyscale operands
  vil_pixel_format pix_format0 = img0->pixel_format();
  vil_pixel_format pix_format1 = img1->pixel_format();
  if (pix_format0 == VIL_PIXEL_FORMAT_FLOAT ||
      pix_format1 == VIL_PIXEL_FORMAT_FLOAT)
    {
      return vil_new_image_resource_of_view(sum);
    }

  if (pix_format0 == VIL_PIXEL_FORMAT_UINT_16 ||
      pix_format1 == VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<vxl_uint_16> res =
        brip_vil_float_ops::convert_to_short(vil_new_image_resource_of_view(sum));
      return vil_new_image_resource_of_view(res);
    }

  if (pix_format0 == VIL_PIXEL_FORMAT_BYTE ||
      pix_format1 == VIL_PIXEL_FORMAT_BYTE)
    {
      vil_image_view<vxl_byte> res =
        brip_vil_float_ops::convert_to_byte(sum);
      return vil_new_image_resource_of_view(res);
    }
  // for color return the float image
  return vil_new_image_resource_of_view(sum);
}

// Compute img0 - img1
vil_image_resource_sptr
brip_vil_float_ops::difference(vil_image_resource_sptr const& img0,
                               vil_image_resource_sptr const& img1)
{
  vil_image_view<float> op0 = brip_vil_float_ops::convert_to_float(img0);
  vil_image_view<float> op1 = brip_vil_float_ops::convert_to_float(img1);
  vil_image_view<float> diff;
  vil_math_image_difference(op0, op1, diff);

  // find out the types of the input images for now, only do greyscale operands
  vil_pixel_format pix_format0 = img0->pixel_format();
  vil_pixel_format pix_format1 = img1->pixel_format();
  if (pix_format0 == VIL_PIXEL_FORMAT_FLOAT ||
      pix_format1 == VIL_PIXEL_FORMAT_FLOAT)
    {
      return vil_new_image_resource_of_view(diff);
    }

  if (pix_format0 == VIL_PIXEL_FORMAT_UINT_16 ||
      pix_format1 == VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<vxl_uint_16> res =
        brip_vil_float_ops::convert_to_short(vil_new_image_resource_of_view(diff));
      return vil_new_image_resource_of_view(res);
    }

  if (pix_format0 == VIL_PIXEL_FORMAT_BYTE ||
      pix_format1 == VIL_PIXEL_FORMAT_BYTE)
    {
      vil_image_view<vxl_byte> res = brip_vil_float_ops::convert_to_byte(diff);
      return vil_new_image_resource_of_view(res);
    }
  // for color return the float image
  return vil_new_image_resource_of_view(diff);
}

//: Compute the entropy of the intensity of a region
//  Note no bounds checking!
float brip_vil_float_ops::entropy_i(const unsigned i, const unsigned j,
                                    const unsigned i_radius,
                                    const unsigned j_radius,
                                    vil_image_view<float> const& intensity,
                                    const float range, const unsigned bins)
{
  bsta_histogram<float> hi(range, bins);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float inten = intensity(i+di, j+dj);
      hi.upcount(inten, 1.0f);
    }
  return hi.entropy();
}

//: Compute the entropy of the gradient direction of a region
//  Note no bounds checking!
float brip_vil_float_ops::entropy_g(const unsigned i, const unsigned j,
                                    const unsigned i_radius,
                                    const unsigned j_radius,
                                    vil_image_view<float> const& gradx,
                                    vil_image_view<float> const& grady,
                                    const float range, const unsigned bins)
{
  bsta_histogram<float> hg(range, bins);
  static const auto deg_rad = (float)(vnl_math::deg_per_rad);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float Ix = gradx(i+di, j+dj), Iy = grady(i+di, j+dj);
      float ang = deg_rad*std::atan2(Iy, Ix) + 180.0f;
      float mag = std::abs(Ix)+std::abs(Iy);
      hg.upcount(ang, mag);
    }
  return hg.entropy();
}

//: Compute the hue and saturation entropy of a region about the specified pixel
//  Note no bounds checking!
float brip_vil_float_ops::entropy_hs(const unsigned i, const unsigned j,
                                     const unsigned i_radius,
                                     const unsigned j_radius,
                                     vil_image_view<float> const& hue,
                                     vil_image_view<float> const& sat,
                                     const float range, const unsigned bins)
{
  bsta_histogram<float> hg(range, bins);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float h = hue(i+di, j+dj), s = sat(i+di, j+dj);
      hg.upcount(h, s);
    }
  return hg.entropy();
}

vil_image_view<float>
brip_vil_float_ops::entropy(const unsigned i_radius,
                            const unsigned j_radius,
                            const unsigned step,
                            vil_image_resource_sptr const& img,
                            const float sigma,
                            const unsigned bins,
                            const bool intensity,
                            const bool gradient,
                            const bool ihs)
{
  vil_image_view<float> ent;
  if (!intensity&&!gradient&&!ihs)
  {
    std::cout << "In brip_vil_float_ops::entropy(.) - No computation to do\n";
    return ent;
  }

  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> gimage =
    brip_vil_float_ops::gaussian(fimage, sigma);

  unsigned ni = img->ni(), nj = img->nj();
  ent.set_size(ni/step+1, nj/step+1);
  ent.fill(0.0f);
  if (intensity)
    for (unsigned j = j_radius; j<(nj-j_radius); j+=step)
      for (unsigned i = i_radius; i<(ni-i_radius); i+=step)
        ent(i/step,j/step) =
          brip_vil_float_ops::entropy_i(i, j, i_radius, j_radius, gimage, 255.0f, bins);

  if (gradient)
  {
    vil_image_view<float> grad_x, grad_y;
    grad_x.set_size(ni, nj);
    grad_y.set_size(ni, nj);
    brip_vil_float_ops::gradient_3x3 (gimage , grad_x , grad_y);
    for (unsigned j = j_radius; j<(nj-j_radius); j+=step)
      for (unsigned i = i_radius; i<(ni-i_radius); i+=step)
        ent(i/step,j/step) +=
          brip_vil_float_ops::entropy_g(i, j, i_radius, j_radius,
                                        grad_x, grad_y);
  }
  if (ihs&&img->nplanes()==3)
  {
    vil_image_view<float> inten, hue, sat;
    vil_image_view<vil_rgb<vxl_byte> > cimage = img->get_view();
    brip_vil_float_ops::convert_to_IHS(cimage, inten, hue, sat);
    for (unsigned j = j_radius; j<(nj-j_radius); j+=step)
      for (unsigned i = i_radius; i<(ni-i_radius); i+=step)
        ent(i/step,j/step) +=
          brip_vil_float_ops::entropy_hs(i, j, i_radius, j_radius,
                                         hue, sat);
  }
  return ent;
}

float brip_vil_float_ops::minfo_i(const unsigned i0, const unsigned j0,
                                  const unsigned i1, const unsigned j1,
                                  const unsigned i_radius,
                                  const unsigned j_radius,
                                  vil_image_view<float> const& int0,
                                  vil_image_view<float> const& int1,
                                  const float range, const unsigned bins)
{
  bsta_histogram<float> hi0(range, bins);
  bsta_histogram<float> hi1(range, bins);
  bsta_joint_histogram<float> hji(range, bins);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float inten0 = int0(i0+di, j0+dj);
      float inten1 = int1(i1+di, j1+dj);
      hi0.upcount(inten0, 1.0f);
      hi1.upcount(inten1, 1.0f);
      hji.upcount(inten0, 1.0f, inten1, 1.0f);
    }
  float H0 = hi0.entropy();
  float H1 = hi1.entropy();
  float HJ = hji.entropy();
  float minfo_i = H0 + H1 - HJ;
#ifdef DEBUG
  if (minfo<0)
    std::cout << "intensity MI LT 0 " << minfo << std::endl;
#endif
  return minfo_i;
}

float brip_vil_float_ops::minfo_g(const unsigned i0, const unsigned j0,
                                  const unsigned i1, const unsigned j1,
                                  const unsigned i_radius,
                                  const unsigned j_radius,
                                  vil_image_view<float> const& gradx0,
                                  vil_image_view<float> const& grady0,
                                  vil_image_view<float> const& gradx1,
                                  vil_image_view<float> const& grady1,
                                  const float range, const unsigned bins)
{
  bsta_histogram<float> hg0(range, bins);
  bsta_histogram<float> hg1(range, bins);
  bsta_joint_histogram<float> hjg(range, bins);
  static const auto deg_rad = (float)(vnl_math::deg_per_rad);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float Ix0 = gradx0(i0+di, j0+dj), Iy0 = grady0(i0+di, j0+dj);
      float Ix1 = gradx1(i1+di, j1+dj), Iy1 = grady1(i1+di, j1+dj);
      float ang0 = deg_rad*std::atan2(Iy0, Ix0) + 180.0f;
      float ang1 = deg_rad*std::atan2(Iy1, Ix1) + 180.0f;
      float mag0 = std::abs(Ix0)+std::abs(Iy0);
      float mag1 = std::abs(Ix1)+std::abs(Iy1);
      hg0.upcount(ang0, mag0);
      hg1.upcount(ang1, mag1);
      hjg.upcount(ang0, mag0, ang1, mag1);
    }
  float H0 = hg0.entropy();
  float H1 = hg1.entropy();
  float HJ = hjg.entropy();
  float minfo_g = H0 + H1 - HJ;
#ifdef DEBUG
  if (minfo<0)
    std::cout << "gradient MI LT 0 " << minfo << std::endl;
#endif
  return minfo_g;
}

float brip_vil_float_ops::minfo_hs(const unsigned i0, const unsigned j0,
                                   const unsigned i1, const unsigned j1,
                                   const unsigned i_radius,
                                   const unsigned j_radius,
                                   vil_image_view<float> const& hue0,
                                   vil_image_view<float> const& sat0,
                                   vil_image_view<float> const& hue1,
                                   vil_image_view<float> const& sat1,
                                   const float range, const unsigned bins)
{
  bsta_histogram<float> hh0(range, bins);
  bsta_histogram<float> hh1(range, bins);
  bsta_joint_histogram<float> hjh(range, bins);
  int ir = static_cast<int>(i_radius), jr = static_cast<int>(j_radius);
  for (int dj = -jr; dj<=jr; ++dj)
    for (int di = -ir; di<=ir; ++di)
    {
      float h0 = hue0(i0+di, j0+dj), s0 = sat0(i0+di, j0+dj);
      float h1 = hue1(i1+di, j1+dj), s1 = sat1(i1+di, j1+dj);
      hh0.upcount(h0, s0);
      hh1.upcount(h1, s1);
      hjh.upcount(h0, s0, h1, s1);
    }
  float H0 = hh0.entropy();
  float H1 = hh1.entropy();
  float HJ = hjh.entropy();
  float minfo_h = H0 + H1 - HJ;
#ifdef DEBUG
  if (minfo<0)
    std::cout << "color MI LT 0 " << minfo << std::endl;
#endif
  return minfo_h;
}

bool brip_vil_float_ops::minfo(const unsigned i_radius,
                               const unsigned j_radius,
                               const unsigned step,
                               vil_image_resource_sptr const& img0,
                               vil_image_resource_sptr const& img1,
                               vil_image_view<float>& MI0,
                               vil_image_view<float>& MI1,
                               const float sigma,
                               const bool intensity,
                               const bool gradient,
                               const bool ihs)
{
  if (!intensity&&!gradient&&!ihs)
  {
    std::cout << "In brip_vil_float_ops::minforopy(.) - No computation to do\n";
    return false;
  }

  vil_image_view<float> fimage0 = brip_vil_float_ops::convert_to_float(img0);
  vil_image_view<float> gimage0 =
    brip_vil_float_ops::gaussian(fimage0, sigma);

  vil_image_view<float> fimage1 = brip_vil_float_ops::convert_to_float(img1);
  vil_image_view<float> gimage1 =
    brip_vil_float_ops::gaussian(fimage1, sigma);

  unsigned ni0 = img0->ni(), nj0 = img0->nj();
  unsigned ni1 = img1->ni(), nj1 = img1->nj();
  unsigned ilimit = 2*i_radius +1, jlimit = 2*j_radius+1;
  if (ni0<ilimit||nj0<jlimit||ni1<ilimit||nj1<jlimit)
  {
    std::cout << "In brip_vil_float_ops::minfo(...) - image too small\n";
    return false;
  }
  MI0.set_size(ni0/step+1, nj0/step+1); MI0.fill(0.0f);
  MI1.set_size(ni1/step+1, nj1/step+1); MI1.fill(0.0f);
  if (intensity)
    for (unsigned j0 = j_radius; j0<(nj0-j_radius); j0+=step)
      for (unsigned i0 = i_radius; i0<(ni0-i_radius); i0+=step)
        for (unsigned j1 = j_radius; j1<(nj1-j_radius); j1+=step)
          for (unsigned i1 = i_radius; i1<(ni1-i_radius); i1+=step)
          {
            float minfo = brip_vil_float_ops::minfo_i(i0, j0,i1, j1,
                                                      i_radius, j_radius,
                                                      gimage0, gimage1);
            MI0(i0/step,j0/step) = minfo;
            MI1(i1/step,j1/step) = minfo;
          }
  if (gradient)
  {
    vil_image_view<float> grad_x0, grad_y0, grad_x1, grad_y1;
    grad_x0.set_size(ni0, nj0);
    grad_y0.set_size(ni0, nj0);
    grad_x1.set_size(ni1, nj1);
    grad_y1.set_size(ni1, nj1);
    brip_vil_float_ops::gradient_3x3 (gimage0 , grad_x0 , grad_y0);
    brip_vil_float_ops::gradient_3x3 (gimage1 , grad_x1 , grad_y1);
    for (unsigned j0 = j_radius; j0<(nj0-j_radius); j0+=step)
      for (unsigned i0 = i_radius; i0<(ni0-i_radius); i0+=step)
        for (unsigned j1 = j_radius; j1<(nj1-j_radius); j1+=step)
          for (unsigned i1 = i_radius; i1<(ni1-i_radius); i1+=step)
          {
            float minfo = brip_vil_float_ops::minfo_g(i0, j0,i1, j1,
                                                      i_radius, j_radius,
                                                      grad_x0, grad_y0,
                                                      grad_x1, grad_y1);
            MI0(i0/step,j0/step) += minfo;
            MI1(i1/step,j1/step) += minfo;
          }
  }
  if (ihs&&img0->nplanes()==3&&img1->nplanes()==3)
  {
    vil_image_view<float> inten0, hue0, sat0;
    vil_image_view<float> inten1, hue1, sat1;
    vil_image_view<vil_rgb<vxl_byte> > cimage0 = img0->get_view();
    vil_image_view<vil_rgb<vxl_byte> > cimage1 = img1->get_view();
    brip_vil_float_ops::convert_to_IHS(cimage0, inten0, hue0, sat0);
    brip_vil_float_ops::convert_to_IHS(cimage1, inten1, hue1, sat1);

    for (unsigned j0 = j_radius; j0<(nj0-j_radius); j0+=step)
      for (unsigned i0 = i_radius; i0<(ni0-i_radius); i0+=step)
        for (unsigned j1 = j_radius; j1<(nj1-j_radius); j1+=step)
          for (unsigned i1 = i_radius; i1<(ni1-i_radius); i1+=step)
          {
            float minfo = brip_vil_float_ops::minfo_hs(i0, j0,i1, j1,
                                                       i_radius, j_radius,
                                                       hue0, sat0,
                                                       hue1, sat1);
            MI0(i0/step,j0/step) += minfo;
            MI1(i1/step,j1/step) += minfo;
          }
  }
  return true;
}

// compute the average of the image intensity within the specified region
float brip_vil_float_ops::
average_in_box(vil_image_view<float> const& v, vgl_box_2d<double> const&  box)
{
  vgl_point_2d<double> p0 = box.min_point();
  auto i0 = static_cast<unsigned>(p0.x()), j0 = static_cast<unsigned>(p0.y());
  vgl_point_2d<double> p1 = box.max_point();
  auto i1 = static_cast<unsigned>(p1.x()), j1 = static_cast<unsigned>(p1.y());
  float n = 0.0f;
  float sum = 0.0f;
  for (unsigned i = i0; i<=i1; ++i)
    for (unsigned j = j0; j<=j1; ++j, ++n)
      sum += v(i,j);
  if (n>0)
    sum /= n;
  return sum;
}

#if 0 // For now remove dependency on vimt. Save for illustration
bool brip_vil_float_ops::vimt_homography(vil_image_view<float> const& curr_view,
                                         vgl_h_matrix_2d<double>const& H,
                                         vil_image_view<float>& output)
{
  int bimg_ni;
  int bimg_nj;

  int offset_i;
  int offset_j;

  vbl_bounding_box<double,2> box;

  unsigned ni =  curr_view.ni(), nj =  curr_view.nj();
  vimt_transform_2d p;
  vnl_double_3x3 Mh = H.get_matrix();
  vnl_double_2x3 M;
  for (unsigned c = 0; c<3; ++c)
    for (unsigned r = 0; r<2; ++r)
      M[r][c] = Mh[r][c]/Mh[2][2];
  p.set_affine(M);
  box.update(p(0,0).x(),p(0,0).y());
  box.update(p(0,nj).x(),p(0,nj).y());
  box.update(p(ni,0).x(),p(ni,0).y());
  box.update(p(ni,nj).x(),p(ni,nj).y());

  bimg_ni=(int)std::ceil(box.max()[0]-box.min()[0]);
  bimg_nj=(int)std::ceil(box.max()[1]-box.min()[1]);

  offset_i=(int)std::ceil(0-box.min()[0]);
  offset_j=(int)std::ceil(0-box.min()[1]);

  vimt_image_2d_of<float> sample_im;
  vgl_point_2d<double> q(-offset_i,-offset_j);
  vgl_vector_2d<double> u(1,0);
  vgl_vector_2d<double> v(0,1);

  vimt_image_2d_of<float> curr_img(curr_view,p);
  vimt_resample_bilin(curr_img,sample_im,q,u,v,bimg_ni,bimg_nj);
  output = sample_im.image();
  return true;
}

#endif // 0

std::vector<float> brip_vil_float_ops::scan_region(const vil_image_resource_sptr& img,
                                                  const vgl_polygon<double>& poly,
                                                  float& min,
                                                  float& max)
{
  std::vector<float> pixels;
  min = vnl_numeric_traits<float>::maxval;
  max = 0;
  unsigned np = img->nplanes();
  vgl_polygon_scan_iterator<double> si(poly, false);
  if (img->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    if (np==1) // single plane
    {
      for (si.reset(); si.next();)
      {
        auto j = static_cast<unsigned>(si.scany());
        for (int x = si.startx(); x<=si.endx(); ++x)
        {
          auto i = static_cast<unsigned>(x);
          vil_image_view<unsigned char> v = img->get_view(i, 1,j, 1);
          auto fv = static_cast<float>(v(0,0));
          if (fv<min) min = fv;
          if (fv>max) max = fv;
          pixels.push_back(fv);
        }
      }
      return pixels;
    }
    else
    {
      for (si.reset(); si.next();)
      {
        auto j = static_cast<unsigned>(si.scany());
        for (int x = si.startx(); x<=si.endx(); ++x)
        {
          auto i = static_cast<unsigned>(x);
          vil_image_view<unsigned char> v = img->get_view(i, 1,j, 1);
          float fv = 0;
          for (unsigned p = 0; p<np; ++p)
            fv += v(0,0,p);
          fv/=3;
          if (fv<min) min = fv;
          if (fv>max) max = fv;
          pixels.push_back(fv);
        }
      }
    }
    return pixels;
  }
  else if (img->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    if (np) // single plane
    {
      for (si.reset(); si.next();)
      {
        auto j = static_cast<unsigned>(si.scany());
        for (int x = si.startx(); x<=si.endx(); ++x)
        {
          auto i = static_cast<unsigned>(x);
          vil_image_view<unsigned short> v = img->get_view(i, 1,j, 1);
          auto fv = static_cast<float>(v(0,0));
          if (fv<min) min = fv;
          if (fv>max) max = fv;
          pixels.push_back(fv);
        }
      }
      return pixels;
    }
    else
    {
      for (si.reset(); si.next();)
      {
        auto j = static_cast<unsigned>(si.scany());
        for (int x = si.startx(); x<=si.endx(); ++x)
        {
          auto i = static_cast<unsigned>(x);
          vil_image_view<unsigned short> v = img->get_view(i, 1,j, 1);
          float fv = 0;
          for (unsigned p = 0; p<np; ++p)
            fv += v(0,0,p);
          fv/=3;
          if (fv<min) min = fv;
          if (fv>max) max = fv;
          pixels.push_back(fv);
        }
      }
      return pixels;
    }
  }
  std::cerr << "In brip_vil_float_ops::scan_region() - unknown format\n";
  return pixels;
}

// It has been observed that color order is somewhat invariant to illumination
// Compute an index based on color order. The tolerance determines if two
// color bands are too close to determine order, i.e. they should be considered
// equal instead
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
//
vil_image_view<vxl_byte> brip_vil_float_ops::
color_order(vil_image_view<float> const& color_image, float eq_tol)
{
  unsigned ni = color_image.ni(), nj = color_image.nj();
  unsigned np = color_image.nplanes();
  vil_image_view<vxl_byte> temp;
  if (np!=3)
    return temp; // improper call
  temp.set_size(ni, nj);
  vxl_byte rg_eq=192, rg_lt=128, rg_gt=64;
  vxl_byte rb_eq=48, rb_lt=32, rb_gt=16;
  vxl_byte gb_eq=12, gb_lt=8, gb_gt=4;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      float r=color_image(i,j,0), g=color_image(i,j,1), b=color_image(i,j,2);
      vxl_byte rg, rb, gb;
      // rg code
      if (std::fabs(r-g)<eq_tol)
        rg = rg_eq;
      else if (r<g)
        rg = rg_lt;
      else rg = rg_gt;
      // rb code
      if (std::fabs(r-b)<eq_tol)
        rb = rb_eq;
      else if (r<b)
        rb = rb_lt;
      else rb = rb_gt;
      // gb code
      if (std::fabs(g-b)<eq_tol)
        gb = gb_eq;
      else if (g<b)
        gb = gb_lt;
      else gb = gb_gt;
      auto v = static_cast<vxl_byte>(rg|rb|gb); // bitwise or
      temp(i,j) = v;
    }
  return temp;
}

#if 0 // only used in commented-out part of brip_vil_float_ops::extrema()
static double zs(double x)
{ if (x < 0.0001 && x > -0.0001) return 0; else return x; }
#endif // 0

static double brip_vil_rot_gauss(double x, double y,
                                 double sigma_x, double sigma_y, double theta)
{
  double theta_rad = theta*vnl_math::pi_over_180;
  double s = std::sin(theta_rad), c = std::cos(theta_rad);
  double sxr = 1.0/sigma_x, syr = 1.0/sigma_y;
  double ax = (c*x + s*y)*sxr, ay = (-s*x + c*y)*syr;
  double ret = std::exp(-0.5*(ax*ax + ay*ay));
  return ret*sxr*syr/vnl_math::twopi;
}

// revert angle to the range [-90, 90]
float brip_vil_float_ops::extrema_revert_angle(float angle)
{
  if (angle>=0.0f) {
    if (angle>90.0f&&angle<=270.0f)
      angle -= 180.0f;
    else if (angle>270.0f&&angle<=360.0f)
      angle -= 360.0f;
  }
  if (angle<0.0f) {
    if (angle<-90.0f&&angle>=-270.0f)
      angle += 180.0f;
    else if (angle<-270.0f&&angle>=-360.0f)
      angle += 360.0f;
  }
  return angle;
}

// if scale_invariant = true then the response mag is independent of lambda1
void brip_vil_float_ops::extrema_kernel_mask(float lambda0, float lambda1,
                                             float theta,
                                             vbl_array_2d<float>& kernel,
                                             vbl_array_2d<bool>& mask, float cutoff_percentage, bool scale_invariant)
{
  theta = extrema_revert_angle(theta); // in range [-90, 90]
  // convert theta to radians
  double theta_rad = theta*vnl_math::pi_over_180;
  double s = std::sin(theta_rad), c = std::cos(theta_rad);
  double s0 = lambda0, s1 = lambda1;
  double s1sq = 1.0/(s1*s1);
  // determine the size of the array
  double max_v = brip_vil_rot_gauss(0, 0, s0, s1, 0);
  double cutoff = max_v*cutoff_percentage; // if 0.01 then 1% tails removed
  unsigned ru = 0;
  while (brip_vil_rot_gauss(ru, 0, s0, s1, 0) >= cutoff)
    ++ru;
  unsigned rv = 0;
  while (brip_vil_rot_gauss(0, rv, s0, s1, 0) >= cutoff)
    ++rv;

  // rotate to get bounds
  int ri = static_cast<int>(std::fabs(ru*c+rv*s) +0.5);
  int rj = static_cast<int>(std::fabs(ru*s+rv*c) +0.5);
  if (s<0) {
    ri = static_cast<int>(std::fabs(ru*c-rv*s) +0.5);
    rj = static_cast<int>(std::fabs(ru*s-rv*c) +0.5);
  }

  unsigned ncols = 2*ri +1, nrows = 2*rj+1;
  vbl_array_2d<double> coef(nrows, ncols);
  mask.resize(nrows,ncols);
  double residual = 0.0, total  = 0.0;
  for (int ry = -rj; ry<=rj; ++ry)
    for (int rx = -ri; rx<=ri; ++rx)
    {
      double g = brip_vil_rot_gauss(rx, ry, s0, s1, theta);
      mask[ry+rj][rx+ri] = g>=cutoff;
      double temp = (-s*rx + c*ry);
      temp = temp*temp*s1sq;
      double v = (temp -1)*g*(scale_invariant?1.0:s1sq);
      if (g<cutoff) v = 0.0;
      coef[ry+rj][rx+ri] = v;
      residual+=v;
      total += std::fabs(v);
    }
  double cor = 0.0;
  if (total != 0)
    cor = -residual/total;
  kernel.resize(nrows, ncols);
  // correct any residual offset in coefficients
  // distribute proportionally to coefficient magnitude
  for (unsigned j = 0; j<nrows; ++j)
    for (unsigned i = 0; i<ncols; ++i)
    {
      double v = std::fabs(coef[j][i]);
      coef[j][i]+=v*cor;
      kernel[j][i]=static_cast<float>(coef[j][i]);
    }
}

//:
//  \p theta must be given in degrees.
//  Scale invariant means that the response is independent of the
//  \a sigma_y of the unrotated operator, i.e. the direction of the derivative
void brip_vil_float_ops::gaussian_kernel_mask(float lambda, vbl_array_2d<float>& kernel, vbl_array_2d<bool>& mask, float cutoff_percentage)
{
  double s1 = lambda;
  double s1sq = 1.0/(s1*s1);
  // determine the size of the array
  double max_v = brip_vil_rot_gauss(0, 0, s1, s1, 0);
  double cutoff = max_v*cutoff_percentage; // if 0.01 then 1% tails removed
  unsigned r = 0;
  while (brip_vil_rot_gauss(r, 0, s1, s1, 0) >= cutoff)
    ++r;

  int ri = static_cast<int>(std::fabs((float)r) +0.5);

  unsigned n = 2*ri +1;
  mask.resize(n,n);
  kernel.resize(n, n);
  for (int ry = -ri; ry<=ri; ++ry)
    for (int rx = -ri; rx<=ri; ++rx)
    {
      double g = brip_vil_rot_gauss(rx, ry, s1, s1, 0);
      mask[ry+ri][rx+ri] = g>=cutoff;
      double temp = ry*s1sq;
      double v = (temp -1)*g;
      if (g<cutoff) v = 0.0;
      kernel[ry+ri][rx+ri] = static_cast<float>(std::fabs(v));
    }
}

//: return a square mask and the coefficient matrix [kernel] for a symmetric gaussian distribution
void brip_vil_float_ops::gaussian_kernel_square_mask(float lambda, vbl_array_2d<float>& kernel, vbl_array_2d<bool>& mask, float cutoff_percentage)
{
  double s1 = lambda;
  double s1sq = 1.0/(s1*s1);
  // determine the size of the array
  double max_v = brip_vil_rot_gauss(0, 0, s1, s1, 0);
  double cutoff = max_v*cutoff_percentage; // if 0.01 then 1% tails removed
  unsigned r = 0;
  while (brip_vil_rot_gauss(r, 0, s1, s1, 0) >= cutoff)
    ++r;

  int ri = static_cast<int>(std::fabs((float)r) +0.5);

  unsigned n = 2*ri +1;
  vbl_array_2d<double> coef(n, n);
  mask.resize(n,n);
  kernel.resize(n, n);
  for (int ry = -ri; ry<=ri; ++ry)
    for (int rx = -ri; rx<=ri; ++rx)
    {
      double g = brip_vil_rot_gauss(rx, ry, s1, s1, 0);
      mask[ry+ri][rx+ri] = true;
      double temp = ry*s1sq;
      kernel[ry+ri][rx+ri] = static_cast<float>(std::fabs((temp -1)*g));
    }
}


// Compute the standard deviation of an operator response
// given the image intensity standard deviation at each pixel
vil_image_view<float> brip_vil_float_ops::
std_dev_operator(vil_image_view<float> const& sd_image,
                 vbl_array_2d<float> const& kernel)
{
  unsigned ni = sd_image.ni(), nj = sd_image.nj();
  unsigned nrows = kernel.rows(), ncols = kernel.cols();
  int rrad = (nrows-1)/2, crad = (ncols-1)/2;
  float sd_min, sd_max;
  vil_math_value_range(sd_image, sd_min, sd_max);

  vil_image_view<float> res(ni, nj);
  res.fill(sd_max);

  vbl_array_2d<float>ksq(nrows, ncols);
  for (unsigned r = 0; r<nrows; ++r)
    for (unsigned c = 0; c<ncols; ++c)
      ksq[r][c] = kernel[r][c]*kernel[r][c];

  for (int j = rrad; j<static_cast<int>(nj-rrad); ++j)
    for (int i = crad; i<static_cast<int>(ni-crad); ++i)
    {
      float sum = 0;
      for (int jj = -rrad; jj<=rrad; ++jj)
        for (int ii = -crad; ii<=crad; ++ii) {
          float sd_sq = sd_image(i+ii, j+jj);
          sd_sq *= sd_sq;
          sum += sd_sq*ksq[jj+rrad][ii+crad];
        }
      res(i,j) = std::sqrt(sum);
    }
  return res;
}

// Compute the standard deviation of an operator response
// given the image intensity standard deviation at each pixel
// uses a modified formula to compute std_dev
vil_image_view<float> brip_vil_float_ops::
std_dev_operator_method2(vil_image_view<float> const& sd_image,
                         vbl_array_2d<float> const& kernel)
{
  unsigned ni = sd_image.ni(), nj = sd_image.nj();
  unsigned nrows = kernel.rows(), ncols = kernel.cols();
  int rrad = (nrows-1)/2, crad = (ncols-1)/2;
  float sd_min, sd_max;
  vil_math_value_range(sd_image, sd_min, sd_max);

  vil_image_view<float> res(ni, nj);
  res.fill(sd_max);

  for (int j = rrad; j<static_cast<int>(nj-rrad); ++j)
    for (int i = crad; i<static_cast<int>(ni-crad); ++i)
    {
      float sum = 0;
      for (int jj = -rrad; jj<=rrad; ++jj)
        for (int ii = -crad; ii<=crad; ++ii) {
          float sd = sd_image(i+ii, j+jj);
          sum += (float)(sd*std::abs(kernel[jj+rrad][ii+crad]));
        }
      res(i,j) = sum;
    }
  return res;
}

vil_image_view<float>
brip_vil_float_ops::extrema(vil_image_view<float> const& input,
                            float lambda0, float lambda1, float theta,
                            bool bright, bool mag_only,
                            bool output_response_mask,
                            bool signed_response, bool scale_invariant,
                            bool non_max_suppress, float cutoff_per)
{
  assert(!(mag_only&&signed_response));
  vbl_array_2d<float> fa;
  vbl_array_2d<bool> mask;
  brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, theta,
                                          fa, mask, cutoff_per,
                                          scale_invariant);
  unsigned nrows = fa.rows(), ncols = fa.cols();
  int rj = (nrows-1)/2, ri = (ncols-1)/2;
  vbl_array_2d<double> coef(nrows,ncols);
  for (unsigned r = 0; r<nrows; ++r)
    for (unsigned c = 0; c<ncols; ++c)
      coef[r][c]=fa[r][c];

#if 0
  // (unused) double max_v = brip_vil_rot_gauss(0, 0, lambda0, lambda1, 0);
  // (unused) double cutoff=static_cast<float>(max_v*0.01); // 1% tails removed
  std::cout << "\ngauss ={";
  for (unsigned j = 0; j<nrows; ++j) {
    std::cout << '{';
    for (unsigned i = 0; i<ncols-1; ++i)
      std::cout << zs(coef[j][i]) << ',';
    if (j != nrows-1)
      std::cout << zs(coef[j][ncols-1]) << "},";
    else
      std::cout << zs(coef[j][ncols-1]) << '}';
  }

  std::cout << "};\n\nmask ={";
  for (unsigned j = 0; j<nrows; ++j) {
    std::cout << '{';
    for (unsigned i = 0; i<ncols-1; ++i)
      std::cout << mask[j][i] << ',';
    if (j != nrows-1)
      std::cout << mask[j][ncols-1] << "},";
    else
      std::cout << mask[j][ncols-1] << '}';
  }
  std::cout << "};" << std::flush;
#endif
  // compute response image
  unsigned ni = input.ni(), nj = input.nj();
  vil_image_view<float> temp(ni, nj);
  vil_image_view<float> temp2(ni, nj);
  temp.fill(0.0f); temp2.fill(0.0f);
  for (unsigned j = rj; j<(nj-rj); j++)
    for (unsigned i = ri; i<(ni-ri); i++) {
      double sum = 0;
      for (int jj=-rj; jj<=rj; ++jj)
        for (int ii=-ri; ii<=ri; ++ii)
          if (mask[jj+rj][ii+ri])
            sum += coef[jj+rj][ii+ri]*input(i+ii, j+jj);
      temp2(i,j) = static_cast<float>(sum);
      if (mag_only) {
        temp(i,j) = static_cast<float>(std::fabs(sum));
      }
      else if (bright) { // coefficients are negative at center
        if (sum<0) temp(i,j) = static_cast<float>(-sum);
      }
      else {
        if (sum>0) temp(i,j) = static_cast<float>(sum);
      }
    }
  // non-max suppression
  vil_image_view<float> res(temp);
  if (non_max_suppress)
    for (unsigned j = rj; j<(nj-rj); j++)
      for (unsigned i = ri; i<(ni-ri); i++)
      {
        float cv = temp(i,j);
        for (int jj=-rj; jj<=rj; ++jj)
          for (int ii=-ri; ii<=ri; ++ii)
            if ((ii!=0||jj!=0) && mask[jj+rj][ii+ri] && temp(i+ii, j+jj)>cv)
              res(i,j)=0.0f;
      }
  if (!output_response_mask&&!signed_response)
    return res;
  unsigned np = 2;
  if (output_response_mask&&signed_response)
    np = 3;
  //response plane and mask plane and/or signed response
  vil_image_view<float> res_mask(ni, nj, np);
  res_mask.fill(0.0f);
  // always copy response to plane 0
  for (unsigned j = rj; j<(nj-rj); j++)
    for (unsigned i = ri; i<(ni-ri); i++)
    {
      float rv = res(i,j);
      res_mask(i,j,0)=rv;
    }
  // copy mask plane to plane 1 (or not)
  if (output_response_mask) {
    for (unsigned j = rj; j<(nj-rj); j++)
      for (unsigned i = ri; i<(ni-ri); i++)
      {
        float rv = res(i,j);
        if (!rv)
          continue;
        for (int jj=-rj; jj<=rj; ++jj)
          for (int ii=-ri; ii<=ri; ++ii)
            if (mask[jj+rj][ii+ri])
              if (rv>res_mask(i+ii,j+jj,1))
                res_mask(i+ii,j+jj,1) = rv;
      }
  }
  // copy signed response to plane 1 if no mask plane, otherwise plane 2
  if (signed_response) {
    unsigned p = np-1;
    for (unsigned j = rj; j<(nj-rj); j++)
      for (unsigned i = ri; i<(ni-ri); i++)
        res_mask(i,j,p) = temp2(i,j);
  }
  return res_mask;
}


//: Find anisotropic intensity extrema at a range of orientations and return the maximal response at the best orientation. Theta interval is in degrees
//  if lambda0 == lambda1 then reduces to the normal extrema operator
vil_image_view<float> brip_vil_float_ops::
extrema_rotational(vil_image_view<float> const& input, float lambda0,
                   float lambda1, float theta_interval, bool bright,
                   bool mag_only, bool signed_response,
                   bool scale_invariant, bool non_max_suppress,
                   float cutoff_per)
{
  //invalid response
  assert(!(mag_only&&signed_response));

  unsigned ni = input.ni();
  unsigned nj = input.nj();

  vil_image_view<float> output(ni, nj, 3); // the first plane is the response strength, the second is the best angle and the third is the mask for that angle
  output.fill(0.0f);

  if (lambda0 == lambda1) {  // if isotropic reduces to normal extrema calculation (perfect orientation symmetry)
    vil_image_view<float> res_mask_current =
      extrema(input, lambda0, lambda1, 0.0f, bright, mag_only,
              true, signed_response, scale_invariant,
              non_max_suppress, cutoff_per);
    unsigned np = res_mask_current.nplanes();
    for (unsigned j = 0; j<nj; j++)
      for (unsigned i = 0; i<ni; i++) {
        if (!signed_response)
          output(i,j,0) = res_mask_current(i,j,0);  // return the non-max suppressed for angle 0
        else
          output(i,j,0) = res_mask_current(i,j,np-1);
        output(i,j,1) = 0.0f;
        output(i,j,2) = res_mask_current(i,j,1);
      }
    return output;
  }

  // The kernel generator does not treat the x and y axis symmetrically, the method works correctly only when lambda0 > lambda1
  // Theoretically one can always call this method by switching the lambdas but the caller of the method should make this switch if needed hence the assertion
  if (lambda0 < lambda1) {
    std::cout << "In brip_vil_float_ops::extrema_rotational() - ERROR! rotational extrema operator requires lambda0 to be larger than lambda1! switch the lambdas and use the output angle map accordingly!\n";
    throw 0;
  }
  if (theta_interval < std::numeric_limits<float>::epsilon()) {
    std::cout << "In brip_vil_float_ops::extrema_rotational() - ERROR! theta_interval needs to be larger than 0!\n";
    throw 0;
  }

  vil_image_view<float> res_img(ni, nj);
  vil_image_view<int> res_angle(ni, nj);
  res_img.fill(0.0f); res_angle.fill(0);

  std::vector<float> angles;
  angles.push_back(-1.0f);
  for (float theta = 0.0f; theta < 180.0f; theta += theta_interval) { angles.push_back(theta); }

  std::vector<vbl_array_2d<bool> > mask_vect(angles.size(), vbl_array_2d<bool>());
  int max_rji = 0;
  // elliptical operator has 180 degree rotational symmetry, so only the angles in the range [0,180] matter
  float theta = 0.0f; unsigned theta_i = 1;
  for ( ; theta < 180; theta += theta_interval, theta_i++)
  {
    // compute the response
    vbl_array_2d<float> fa; vbl_array_2d<bool> mask;
    brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, theta,
                                            fa, mask, cutoff_per, scale_invariant);
    mask_vect[theta_i] = mask;
    unsigned nrows = fa.rows(), ncols = fa.cols();
    int rj = (nrows-1)/2, ri = (ncols-1)/2;
    if (rj > max_rji) max_rji = rj;
    if (ri > max_rji) max_rji = ri;
    for (unsigned j = rj; j<(nj-rj); j++) {
      for (unsigned i = ri; i<(ni-ri); i++) {
        float res = 0.0f;
        double sum = 0;
        for (int jj=-rj; jj<=rj; ++jj)
          for (int ii=-ri; ii<=ri; ++ii)
            if (mask[jj+rj][ii+ri]) {
              sum += double(fa[jj+rj][ii+ri])*input(i+ii, j+jj);
            }
        if (mag_only) {
          res = static_cast<float>(std::fabs(sum));
        }
        else if (signed_response) {
          res = static_cast<float>(sum);
        }
        else if (bright) { // coefficients are negative at center
          if (sum<0) res = static_cast<float>(-sum);
        }
        else {
          if (sum>0) res = static_cast<float>(sum);
        }
        // pick largest angle response magnitude
        if (std::fabs(res_img(i,j)) < std::fabs(res)) {
          res_img(i,j) = res;
          res_angle(i,j) = theta_i;
          output(i,j,0) = res;
          output(i,j,1) = theta_i;
        }
      }
    }
    std::cout << '.';
  }
  std::cout << '\n';
  //if (!non_max_suppress) return res_img;
  if (!non_max_suppress) return output;
  // now we have pixel-wise best angle, run the non-max suppression around each non-zero pixel using the angles mask
  vil_image_view<float> res(res_img);

  for (unsigned j = max_rji; j<(nj-max_rji); j++)
    for (unsigned i = max_rji; i<(ni-max_rji); i++)
    {
      float cv = res_img(i,j);
      if (!cv)
        continue;
      int theta_i = res_angle(i,j);
      vbl_array_2d<bool> mask = mask_vect[theta_i];
      unsigned nrows = mask.rows(), ncols = mask.cols();
      int rj = (nrows-1)/2, ri = (ncols-1)/2;

      bool max = true;
      for (int jj=-rj; jj<=rj; ++jj) {
        for (int ii=-ri; ii<=ri; ++ii) {
          if ((ii!=0||jj!=0) && mask[jj+rj][ii+ri] && res_img(i+ii, j+jj)>cv) {
            max = false;
            break;
          }
        }
        if (!max)
          break;
      }
      if (!max) {
        res(i,j) = 0.0f;
        res_angle(i, j) = 0; // the zeroth angle is -1.0f, so invalid
      }
    }

  vil_image_view<float> res_mask(ni, nj);
  res_mask.fill(0.0f);
  // now all the non-zero elements in res are our responses, create the mask image using the angle info
  for (unsigned j = max_rji; j<(nj-max_rji); j++)
    for (unsigned i = max_rji; i<(ni-max_rji); i++)
    {
      float rv = res(i,j);
      if (!rv)
        continue;
      int theta_i = res_angle(i,j);
      // get the mask for this angle
      vbl_array_2d<bool> mask = mask_vect[theta_i];
      unsigned nrows = mask.rows(), ncols = mask.cols();
      int rj = (nrows-1)/2, ri = (ncols-1)/2;

      for (int jj=-rj; jj<=rj; ++jj)
        for (int ii=-ri; ii<=ri; ++ii)
          if (mask[jj+rj][ii+ri])
            if (rv>res_mask(i+ii,j+jj))
              res_mask(i+ii,j+jj) = rv;
    }

  // now prepare the output accordingly
  for (unsigned j = max_rji; j<(nj-max_rji); j++)
    for (unsigned i = max_rji; i<(ni-max_rji); i++)
    {
      output(i,j,0) = res(i,j);
      output(i,j,1) = angles[res_angle(i,j)];
      output(i,j,2) = res_mask(i,j);
    }
  return output;
}


//: theta and phi are in radians
float brip_vil_float_ops::elu(float phi, float lamda0,
                              float lambda1, float theta)
{
  double sp = std::sin(phi), cp = std::cos(phi);
  double st = std::sin(theta), ct = std::cos(theta);
  double temp = 3.0*lamda0*cp*ct-3.0*lambda1*sp*st;
  return static_cast<float>(temp);
}

//: theta and phi are in radians
float brip_vil_float_ops::elv(float phi, float lamda0,
                              float lambda1, float theta)
{
  double sp = std::sin(phi), cp = std::cos(phi);
  double st = std::sin(theta), ct = std::cos(theta);
  double temp = 3.0*lamda0*st*cp + 3.0*lambda1*sp*ct;
  return static_cast<float>(temp);
}

//: theta and phi are in radians
void brip_vil_float_ops::
max_inscribed_rect(float lambda0, float lambda1, float theta,
                   float& u_rect, float& v_rect)
{
  float sign = -1.0f;
  if (theta<0) sign = 1.0f;
  auto th_rad = static_cast<float>(theta*vnl_math::pi_over_180);
  float maxa = 0.0f, max_phi = 0.0f;
  for (float phi = -float(vnl_math::pi); phi<=float(vnl_math::pi); phi+=0.01f)
  {
    float a = (1.0f+brip_vil_float_ops::elu(phi, lambda0, lambda1, th_rad));
    a *= (1.0f+ sign*brip_vil_float_ops::elv(phi, lambda0, lambda1, th_rad));
    if (a>maxa)
    {
      maxa = a;
      max_phi = phi;
    }
  }
  u_rect = brip_vil_float_ops::elu(max_phi, lambda0, lambda1, th_rad);
  v_rect = brip_vil_float_ops::elv(max_phi, lambda0, lambda1, th_rad);
  v_rect = std::fabs(v_rect);
}

static void eu(float lambda0, float cutoff_per, std::vector<float>& kernel)
{
  kernel.clear();
  double l_sqi = 1.0/(lambda0*lambda0);
  double norm = vnl_math::one_over_sqrt2pi;
  norm /= lambda0;
  int r = static_cast<int>(std::sqrt((-2.0*std::log(cutoff_per))/l_sqi)+0.5);
  for (int i = -r; i<=r; ++i)
  {
    double k = std::exp(-0.5*i*i*l_sqi);
    k*=norm;
    kernel.push_back(static_cast<float>(k));
  }
}

static void ev(float lambda1, float cutoff_per, bool scale_invariant,
               std::vector<float>& kernel)
{
  kernel.clear();
  double l1_sqi = 1/(lambda1*lambda1);
  int r = static_cast<int>(std::sqrt((-2.0*std::log(cutoff_per))/l1_sqi)+0.5);
  double norm = scale_invariant ? 1/lambda1 : l1_sqi/lambda1;
  norm *= vnl_math::one_over_sqrt2pi;
  for (int i = -r; i<=r; ++i)
  {
    double s = i*i*l1_sqi;
    double k = std::exp(-0.5*s);
    k *= norm*(s-1.0);
    kernel.push_back(static_cast<float>(k));
  }
}

static vil_image_view<float> convolve_u(vil_image_view<float> const& image,
                                        std::vector<float> const& kernel,
                                        bool initialize = true)
{
  int nk = kernel.size();
  if (nk<3)
    return image;
  int ni = image.ni(), nj = image.nj();
  int ru = (nk-1)/2;
  vil_image_view<float> Iu(ni, nj);
  if (initialize) Iu.fill(0.0f);
  for (int j = 0; j<nj; ++j)
    for (int i = ru; i<ni-ru; ++i) {
      float sum = 0.0f;
      for (int k = -ru; k<=ru; ++k)
      {
        float v = image(i+k, j);
        sum += kernel[k+ru]*v;
      }
      Iu(i,j) = sum;
    }
  return Iu;
}

static vil_image_view<float> convolve_v(vil_image_view<float> const& image,
                                        std::vector<float> const& kernel,
                                        bool initialize = true)
{
  int nk = kernel.size();
  if (nk<3)
    return image;
  int ni = image.ni(), nj = image.nj();
  int rv = (nk-1)/2;
  vil_image_view<float> Iv(ni, nj);
  if (initialize) Iv.fill(0.0f);
  for (int j = rv; j<nj-rv; ++j)
    for (int i = 0; i<ni; ++i) {
      float sum = 0.0f;
      for (int k = -rv; k<=rv; ++k)
      {
        float v = image(i, j+k);
        sum += kernel[k+rv]*v;
      }
      Iv(i,j) = sum;
    }
  return Iv;
}

// this function tracks the image origin during rotation
// the calculations are the same as used in the homography method.
static void rotation_offset(int ni, int nj, float theta_deg,
                            int i, int j, int& ti, int& tj)
{
  ti= 0; tj = 0;
  double deg_to_rad = vnl_math::pi_over_180;
  double rang = deg_to_rad*theta_deg;
  vgl_h_matrix_2d<double> H;
  H.set_identity().set_rotation(rang);
  vsol_box_2d_sptr input_roi;
  vsol_polygon_2d_sptr input_poly, output_poly;
  input_roi = new vsol_box_2d();
  input_roi->add_point(0, 0);
  input_roi->add_point(ni, nj);
  input_poly = bsol_algs::poly_from_box(input_roi);
  if (!bsol_algs::homography(input_poly, H, output_poly))
    return;
  vsol_box_2d_sptr temp = output_poly->get_bounding_box();
  vgl_h_matrix_2d<double> t; t.set_identity().set_translation(-temp->get_min_x(),-temp->get_min_y());
  vgl_homg_point_2d<double> torg = (t*H)*vgl_homg_point_2d<double>(double(i),double(j));
  ti = static_cast<int>(torg.x());
  tj = static_cast<int>(torg.y());
}

vil_image_view<float> brip_vil_float_ops::
fast_extrema(vil_image_view<float> const& input, float lambda0, float lambda1,
             float theta, bool bright, bool mag_only,bool output_response_mask,
             bool signed_response, bool scale_invariant, bool non_max_suppress,
             float cutoff)
{
  // invalid choice
  assert(!(mag_only&&signed_response));
  // the kernels for u and v
  std::vector<float> euk, evk;
  eu(lambda0, cutoff, euk);
  ev(lambda1, cutoff, scale_invariant, evk);
  vil_image_view<float> pre_res;
  int tuf=0, tvf=0, tui=0, tvi=0;
  vil_image_view<float> rot = input;
  bool ang90 = theta==90.0f||theta == 270.0f||
    theta ==-90.0f||theta == -270.0f;
  // rotate the image to the specified angle (minus due to flipped v axis)
  if (theta!=0&&!ang90)
    rot = brip_vil_float_ops::rotate(input, -theta);
  if (!ang90) {
    // convolve kernel across rows
    vil_image_view<float> rotu = convolve_u(rot, euk);
    // then convolve along columns
    vil_image_view<float> rotuv = convolve_v(rotu, evk);
    // rotate back
    if (theta!=0)
    {
      pre_res = brip_vil_float_ops::rotate(rotuv, +theta);
      // there is a border around the image to contain the rotated version
      // which should be removed.
      rotation_offset(input.ni(), input.nj(), -theta, 0, 0, tuf, tvf);
      rotation_offset(rotuv.ni(), rotuv.nj(), +theta, tuf, tvf, tui, tvi);
    }
    else pre_res = rotuv;
  }
  else {
    // convolve along columns
    vil_image_view<float> rotv = convolve_v(rot, euk);
    // convolve across rows
    vil_image_view<float> rotvu = convolve_u(rotv, evk);
    pre_res = rotvu;
  }
  int ni = input.ni(), nj = input.nj();
  vil_image_view<float> res(ni, nj);
  res.fill(0.0f);
  vil_image_view<float> temp2(ni, nj);
  temp2.fill(0.0f);
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i)
    {
      float v = pre_res(i + tui, j+ tvi);
      if (mag_only) {
        res(i,j) = std::fabs(v);
      }
      else if (bright) { // coefficients are negative at center
        if (v<0) res(i,j) = -v;
      }
      else {
        if (v>0) res(i,j) = v;
      }
      temp2(i,j) = v;// signed output
    }
  if (!non_max_suppress && !output_response_mask) {
    return signed_response ? temp2 : res;
  }
  // TO DO! handle non_max_suppress==false but response_mask==true
  // December 12, 2011 --- JLM
  //
  // the second derivative convolution is complete
  // non-maximal suppression
  // determine the inscribed rect in the response mask with max area
  float u_rect, v_rect;
  brip_vil_float_ops::max_inscribed_rect(lambda0,lambda1,theta,u_rect,v_rect);
  int ni_rect = static_cast<int>(u_rect);
  int nj_rect = static_cast<int>(v_rect);
  vil_image_view<float> res_sup(ni, nj);
  res_sup.fill(0.0f);
  for ( int i= ni_rect; i< ni-ni_rect; i+= ni_rect+1 )
    for ( int j = nj_rect; j < nj-nj_rect; j += nj_rect+1 ) {
      int ci= i, cj = j;
      bool find_response = false;
      for ( int di= 0; di<= ni_rect; di++ )
        for ( int dj = 0; dj <= nj_rect; dj++ )
        {
          float v = res(ci,cj);
          float dv = res(i+di,j+dj);
          if (v==0.0f&&dv==0.0f) continue;
          find_response = true;
          if ( v < dv )
          {
            ci= i+di;
            cj = j+dj;
          }
        }
      if (!find_response)
        continue;
      res_sup(ci,cj) = res(ci,cj);
    }

  // determine the outside bounding box
  vbl_array_2d<float> kern;
  vbl_array_2d<bool> mask;
  brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, theta, kern, mask);
  int ri = (kern.cols()-1)/2, rj = (kern.rows()-1)/2;
  // go through the array again and search about each retained
  // local maximum using the outside bounding box
  // there will be at most one non-zero response to test in the inscribed rect
  for (int j = rj; j<(nj-rj); j++)
    for (int i = ri; i<(ni-ri); i++)
    {
      float v = res_sup(i,j);
      if (v==0.0f) continue;
      for (int jj=-rj; jj<=rj; ++jj)
        for (int ii=-ri; ii<=ri; ++ii)
          if (mask[rj+jj][ri+ii] && res_sup(i+ii, j+jj)<v)
            res_sup(i+ii,j+jj)=0.0f;
    }

  if (!output_response_mask&&!signed_response)
    return res_sup;
  unsigned np = 2;
  if (output_response_mask&&signed_response)
    np = 3;
  //response plane and mask plane and/or signed response

  vil_image_view<float> res_mask(ni, nj, np);
  res_mask.fill(0.0f);
  // always copy response to plane 0
  for (int j = rj; j<(nj-rj); j++)
    for (int i = ri; i<(ni-ri); i++)
    {
      float rv = res_sup(i,j);
      res_mask(i,j,0)=rv;
    }
  // copy mask plane to plane 1 (or not)
  if (output_response_mask) {
    for (int j = rj; j<(nj-rj); j++)
      for (int i = ri; i<(ni-ri); i++)
      {
        float rv = res_sup(i,j);
        if (!rv)
          continue;
        for (int jj=-rj; jj<=rj; ++jj)
          for (int ii=-ri; ii<=ri; ++ii)
            if (mask[jj+rj][ii+ri])
              if (rv>res_mask(i+ii,j+jj,1))
                res_mask(i+ii,j+jj,1) = rv;
      }
  }
  // copy signed response to plane 1 if no mask plane, otherwise plane 2
  if (signed_response) {
    unsigned p = np-1;
    for (int j = rj; j<(nj-rj); j++)
      for (int i = ri; i<(ni-ri); i++)
        res_mask(i,j,p) = temp2(i,j);
  }
  return res_mask;
}

vil_image_view<float> brip_vil_float_ops::
fast_extrema_rotational(vil_image_view<float> const& input,
                        float lambda0, float lambda1,
                        float theta_interval,
                        bool bright, bool mag_only,
                        bool signed_response, bool scale_invariant,
                        bool non_max_suppress,
                        float cutoff, float theta_init, float theta_end) {
  unsigned ni = input.ni(), nj = input.nj();
  vil_image_view<float> resp(ni, nj, 3);
  resp.fill(0.0f);

    // elliptical operator has 180 degree rotational symmetry, so only the angles in the range [0,180] matter
  float theta = theta_init; unsigned theta_i = 1;
  for (; theta < theta_end; theta += theta_interval, theta_i++)
  {
   vil_image_view<float> temp =
     fast_extrema(input, lambda0, lambda1, theta, bright,
                  mag_only, false, signed_response,
       scale_invariant, non_max_suppress, cutoff);//scale_invariant, false, cutoff);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i) {
        float v = temp(i,j);
        if (std::fabs(v)>std::fabs(resp(i,j,0)))
          resp(i,j,0) = v;
      }
  }
  return resp;
}
