#include <fstream>
#include <iostream>
#include <complex>
#include "brip_vil1_float_ops.h"
//:
// \file

#include <vul/vul_timer.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vcl_compiler.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/algo/vnl_fft_prime_factors.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_inverse.h>
#include <vil1/vil1_smooth.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>
#include <brip/brip_roi.h>

//------------------------------------------------------------
//:  Convolve with a kernel
//   It's assumed that the kernel is square with odd dimensions
vil1_memory_image_of<float>
brip_vil1_float_ops::convolve(vil1_memory_image_of<float> const & input,
                              vbl_array_2d<float> const & kernel)
{
  const int w = input.width(), h = input.height();
  const int kw = kernel.cols(); // kh = kernel.rows();
  // add a check for kernels that are not equal dimensions of odd size JLM
  int n = (kw-1)/2;
  vil1_memory_image_of<float> output;
  output.resize(w,h);
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
  brip_vil1_float_ops::fill_x_border(output, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(output, n, 0.0f);
  return output;
}

static void fill_1d_array(vil1_memory_image_of<float> const & input,
                          const int y, float* output)
{
  const int w = input.width();
  for (int x = 0; x<w; x++)
    output[x] = input(x,y);
}

//: Downsamples the 1-d array by 2 using the Burt-Adelson reduction algorithm.
void brip_vil1_float_ops::half_resolution_1d(const float* input, int width,
                                             const float k0, const float k1,
                                             const float k2, float* output)
{
  float w[5];
  int n = 0;
  for (; n<5; n++)
    w[n]=input[n];
  output[0]=k0*w[0]+ 2.0f*(k1*w[1] + k2*w[2]);//reflect at boundary
  for (int x = 1; x<width; x++)
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
vil1_memory_image_of<float>
brip_vil1_float_ops::half_resolution(vil1_memory_image_of<float> const & input,
                                     float filter_coef)
{
  vul_timer t;
  float k0 = filter_coef, k1 = 0.25f*filter_coef, k2 = 0.5f*(0.5f-filter_coef);
  const int w = input.width(), h = input.height();
  int half_w =(w+1)/2, half_h = (h+1)/2;
  vil1_memory_image_of<float> output;
  output.resize(half_w, half_h);
  //Generate input/output arrays
  int n = 0;
  float* in0 = new float[w];  float* in1 = new float[w];
  float* in2 = new float[w];  float* in3 = new float[w];
  float* in4 = new float[w];

  float* out0 = new float[half_w];  float* out1 = new float[half_w];
  float* out2 = new float[half_w];  float* out3 = new float[half_w];
  float* out4 = new float[half_w];
  //Initialize arrays
  fill_1d_array(input, n++, in0);   fill_1d_array(input, n++, in1);
  fill_1d_array(input, n++, in2);   fill_1d_array(input, n++, in3);
  fill_1d_array(input, n++, in4);

  //downsample initial arrays
  brip_vil1_float_ops::half_resolution_1d(in0, half_w, k0, k1, k2, out0);
  brip_vil1_float_ops::half_resolution_1d(in1, half_w, k0, k1, k2, out1);
  brip_vil1_float_ops::half_resolution_1d(in2, half_w, k0, k1, k2, out2);
  brip_vil1_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
  brip_vil1_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
  int x=0, y;
  //do the first output line
  for (;x<half_w;x++)
    output(x,0)= k0*out0[x]+ 2.0f*(k1*out1[x]+k2*out2[x]);
  //normal lines
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
      brip_vil1_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
      brip_vil1_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
    }
  }
  delete [] in0;  delete [] in1; delete [] in2;
  delete [] in3;  delete [] in4;
  delete [] out0;  delete [] out1; delete [] out2;
  delete [] out3;  delete [] out4;
  std::cout << "\nDownsample a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
  return output;
}

vil1_memory_image_of<vil1_rgb<unsigned char> > brip_vil1_float_ops::
half_resolution(vil1_memory_image_of<vil1_rgb<unsigned char> > const & input,
                float filter_coef)
{
  int w = input.width(), h = input.height();
  //make the three color planes
  vil1_memory_image_of<float> red(w,h), grn(w,h), blu(w,h);
  for (int row = 0; row<h; row++)
    for (int col = 0; col<w; col++)
    {
      vil1_rgb<unsigned char> v = input(col,row);
      red(col,row) = v.r;
      grn(col,row) = v.g;
      blu(col,row) = v.b;
    }
  vil1_memory_image_of<float> red_half =
    brip_vil1_float_ops::half_resolution(red, filter_coef);

  vil1_memory_image_of<float> grn_half =
    brip_vil1_float_ops::half_resolution(grn, filter_coef);

  vil1_memory_image_of<float> blu_half =
    brip_vil1_float_ops::half_resolution(blu, filter_coef);

  vil1_memory_image_of<unsigned char> red_half_char =
    brip_vil1_float_ops::convert_to_byte(red_half);

  vil1_memory_image_of<unsigned char> grn_half_char =
    brip_vil1_float_ops::convert_to_byte(grn_half);

  vil1_memory_image_of<unsigned char> blu_half_char =
    brip_vil1_float_ops::convert_to_byte(blu_half);

  int w2 = red_half.width(), h2 = red_half.height();
  vil1_memory_image_of<vil1_rgb<unsigned char> > out(w2,h2);
  for (int row = 0; row<h2; row++)
    for (int col = 0; col<w2; col++)
    {
      unsigned char rduc = red_half_char(col,row);
      unsigned char gruc = grn_half_char(col,row);
      unsigned char bluc = blu_half_char(col,row);
      vil1_rgb<unsigned char> v(rduc, gruc, bluc);
      out(col, row) = v;
    }
  return out;
}

vil1_memory_image_of<float>
brip_vil1_float_ops::gaussian(vil1_memory_image_of<float> const & input, float sigma)
{
  vil1_memory_image_of<float> output(vil1_smooth_gaussian(input, sigma));
  return output;
}

//-------------------------------------------------------------------
// Determine if the center of a (2n+1)x(2n+1) neighborhood is a local maximum
//
bool brip_vil1_float_ops::
local_maximum(vbl_array_2d<float> const & neighborhood,
              int n, float& value)
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
void brip_vil1_float_ops::
interpolate_center(vbl_array_2d<float> const & neighborhood,
                   float& dx, float& dy)
{
  dx = 0; dy=0;
  //extract the neighborhood
  float n_m1_m1 = neighborhood[0][0];
  float n_m1_0 = neighborhood[0][1];
  float n_m1_1 = neighborhood[0][2];
  float n_0_m1 = neighborhood[1][0];
  float n_0_0 = neighborhood[1][1];
  float n_0_1 = neighborhood[1][2];
  float n_1_m1 = neighborhood[2][0];
  float n_1_0 = neighborhood[2][1];
  float n_1_1 = neighborhood[2][2];

  //Compute the 2nd order quadratic coefficients
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
void brip_vil1_float_ops::
non_maximum_suppression(vil1_memory_image_of<float> const & input,
                        const int n,
                        const float thresh,
                        std::vector<float>& x_pos,
                        std::vector<float>& y_pos,
                        std::vector<float>& value)
{
  vul_timer t;
  const int N = 2*n+1;
  const int w = input.width(), h = input.height();
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
      if (brip_vil1_float_ops::local_maximum(neighborhood, n, max_v))
      {
        //if so sub-pixel interpolate (3x3) and output results
        brip_vil1_float_ops::interpolate_center(neighborhood, dx, dy);
        x_pos.push_back(x+dx);
        y_pos.push_back(y+dy);
        value.push_back(max_v);
      }
    }
  std::cout << "\nCompute non-maximum suppression on a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

// -----------------------------------------------------------------
// Subtract image_1 from image_2.
// Will not operate unless the two input images are the same dimensions
//
vil1_memory_image_of<float>
brip_vil1_float_ops::difference(vil1_memory_image_of<float> const & image_1,
                                vil1_memory_image_of<float> const & image_2)
{
  const int w1 = image_1.width(), h1 = image_1.height();
  const int w2 = image_2.width(), h2 = image_2.height();
  vil1_memory_image_of<float> temp(w1, h1);
  if (w1!=w2||h1!=h2)
  {
    std::cout << "In brip_vil1_float_ops::difference(..) - images are not the same dimensions\n";
    return temp;
  }
  vil1_memory_image_of<float> out;
  out.resize(w1, h1);
  for (int y = 0; y<h1; y++)
    for (int x = 0; x<w1; x++)
      out(x,y) = image_2(x,y)-image_1(x,y);
  return out;
}

vil1_memory_image_of<float>
brip_vil1_float_ops::abs_clip_to_level(vil1_memory_image_of<float> const & image,
                                       const float thresh, const float level)
{
  vil1_memory_image_of<float> out;
  const int w = image.width(), h = image.height();
  out.resize(w, h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      if (std::fabs(image(x,y))>thresh)
        out(x,y) = level;
      else
        out(x,y) = image(x,y);
    }
  return out;
}

//----------------------------------------------------------------
// Compute the gradient of the input, use a 3x3 mask
//
//         1  |-1  0  1|         1  |-1 -1 -1|
//   Ix = --- |-1  0  1|   Iy = --- | 0  0  0|
//         6  |-1  0  1|         6  | 1  1  1|
//
// Larger masks are computed by pre-convolving with a Gaussian
//
void brip_vil1_float_ops::gradient_3x3(vil1_memory_image_of<float> const & input,
                                       vil1_memory_image_of<float>& grad_x,
                                       vil1_memory_image_of<float>& grad_y)
{
  vul_timer t;
  const int w = input.width(), h = input.height();
  float scale = 1.0f/6.0f;
  for (int y = 1; y<h-1; y++)
    for (int x = 1; x<w-1; x++)
    {
      float gx = input(x+1,y-1)+input(x+1,y)+ input(x+1,y-1)
        -input(x-1,y-1) -input(x-1,y) -input(x-1,y-1);
      float gy = input(x+1,y+1)+input(x,y+1)+ input(x-1,y+1)
        -input(x+1,y-1) -input(x,y-1) -input(x-1,y-1);
      grad_x(x,y) = scale*gx;
      grad_y(x,y) = scale*gy;
    }
  brip_vil1_float_ops::fill_x_border(grad_x, 1, 0.0f);
  brip_vil1_float_ops::fill_y_border(grad_x, 1, 0.0f);
  brip_vil1_float_ops::fill_x_border(grad_y, 1, 0.0f);
  brip_vil1_float_ops::fill_y_border(grad_y, 1, 0.0f);
#ifdef DEBUG
  std::cout << "\nCompute Gradient in " << t.real() << " msecs.\n";
#endif
}

//----------------------------------------------------------------
// Compute the Hessian of the input, use a 3x3 mask
//
//          1 | 1  -2  1|          1 |  1  1  1|         1  | 1  0 -1|
//   Ixx = ---| 1  -2  1|   Iyy = ---| -2 -2 -2|  Ixy = --- | 0  0  0|
//          3 | 1  -2  1|          3 |  1  1  1|         4  |-1  0  1|
//
// Larger masks are computed by pre-convolving with a Gaussian
//
void brip_vil1_float_ops::hessian_3x3(vil1_memory_image_of<float> const & input,
                                      vil1_memory_image_of<float>& Ixx,
                                      vil1_memory_image_of<float>& Ixy,
                                      vil1_memory_image_of<float>& Iyy)
{
  vul_timer t;
  const int w = input.width(), h = input.height();
  for (int y = 1; y<h-1; y++)
    for (int x = 1; x<w-1; x++)
    {
      float xx = input(x-1,y-1)+input(x-1,y)+input(x+1,y)+
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
  brip_vil1_float_ops::fill_x_border(Ixx, 1, 0.0f);
  brip_vil1_float_ops::fill_y_border(Ixx, 1, 0.0f);
  brip_vil1_float_ops::fill_x_border(Ixy, 1, 0.0f);
  brip_vil1_float_ops::fill_y_border(Ixy, 1, 0.0f);
  brip_vil1_float_ops::fill_x_border(Iyy, 1, 0.0f);
  brip_vil1_float_ops::fill_y_border(Iyy, 1, 0.0f);
  std::cout << "\nCompute a hessian matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

vil1_memory_image_of<float>
brip_vil1_float_ops::beaudet(vil1_memory_image_of<float> const & Ixx,
                             vil1_memory_image_of<float> const & Ixy,
                             vil1_memory_image_of<float> const & Iyy)
{
  const int w = Ixx.width(), h = Ixx.height();
  vil1_memory_image_of<float> output;
  output.resize(w, h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      float xx = Ixx(x,y), xy = Ixy(x,y), yy = Iyy(x,y);

      //compute eigenvalues for experimentation
      float det = xx*yy-xy*xy;
      float tr = xx+yy;
      float arg = tr*tr-4.f*det, lambda0 = 0, lambda1=0;
      if (arg>0)
      {
        lambda0 = tr+std::sqrt(arg);
        lambda1 = tr-std::sqrt(arg);
      }
      output(x,y) = lambda0*lambda1; //just det for now
    }
  return output;
}

//----------------------------------------------------------------
//   t
//IxIx gradient matrix elements
// That is,
//                        _                           _
//                       | (dI/dx)^2    (dI/dx)(dI/dy) |
//                       |                             |
//  A = Sum(neighborhood)|                             |
//                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
//                       |_                           _|
//
// over a 2n+1 x 2n+1 neighborhood
//
void
brip_vil1_float_ops::grad_matrix_NxN(vil1_memory_image_of<float> const & input,
                                     const int n,
                                     vil1_memory_image_of<float>& IxIx,
                                     vil1_memory_image_of<float>& IxIy,
                                     vil1_memory_image_of<float>& IyIy)
{
  const int w = input.width(), h = input.height();
  const int N = (2*n+1)*(2*n+1);
  vil1_memory_image_of<float> grad_x, grad_y, output;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  output.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(input, grad_x, grad_y);
  vul_timer t;
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      float xx=0, xy=0, yy=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          float gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          xx += gx*gx;
          xy += gx*gy;
          yy += gy*gy;
        }
      IxIx(x,y) = xx/N;
      IxIy(x,y) = xy/N;
      IyIy(x,y) = yy/N;
    }
  brip_vil1_float_ops::fill_x_border(IxIx, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(IxIx, n, 0.0f);
  brip_vil1_float_ops::fill_x_border(IxIy, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(IxIy, n, 0.0f);
  brip_vil1_float_ops::fill_x_border(IyIy, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(IyIy, n, 0.0f);
  std::cout << "\nCompute a gradient matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

vil1_memory_image_of<float>
brip_vil1_float_ops::harris(vil1_memory_image_of<float> const & IxIx,
                            vil1_memory_image_of<float> const & IxIy,
                            vil1_memory_image_of<float> const & IyIy,
                            const double scale)
{
  const int w = IxIx.width(), h = IxIx.height();
  float norm = 1e-3f; // Scale the output to values in the 10->1000 range
  vil1_memory_image_of<float> output;
  output.resize(w, h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
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
//                        _                           _
//                       | (dI/dx)^2    (dI/dx)(dI/dy) |
//                       |                             |
//  A = Sum(neighborhood)|                             |
//                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
//                       |_                           _|
//
//  The output image is sqrt(lamba_1*lambda_2) where lambda_i are the
//  eigenvalues
//
vil1_memory_image_of<float>
brip_vil1_float_ops::sqrt_grad_singular_values(vil1_memory_image_of<float> & input,
                                               int n)
{
  const int N = (2*n+1)*(2*n+1);
  const int w = input.width(), h = input.height();
  vil1_memory_image_of<float> grad_x, grad_y, output;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  output.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(input, grad_x, grad_y);
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
      float det = (IxIx*IyIy-IxIy*IxIy)/N;
      output(x,y)=std::sqrt(std::fabs(det));
    }
  brip_vil1_float_ops::fill_x_border(output, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(output, n, 0.0f);
  std::cout << "\nCompute sqrt(sigma0*sigma1) in" << t.real() << " msecs.\n";
  return output;
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
brip_vil1_float_ops::Lucas_KanadeMotion(vil1_memory_image_of<float> & current_frame,
                                        vil1_memory_image_of<float> & previous_frame,
                                        int n, double thresh,
                                        vil1_memory_image_of<float>& vx,
                                        vil1_memory_image_of<float>& vy)
{
  const int N = (2*n+1)*(2*n+1);
  const int w = current_frame.width(), h = current_frame.height();
  vil1_memory_image_of<float> grad_x, grad_y, diff;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  //compute the gradient vector and the time derivative
  brip_vil1_float_ops::gradient_3x3(current_frame, grad_x, grad_y);
  diff = brip_vil1_float_ops::difference(previous_frame, current_frame);
  vul_timer t;
  //sum the motion terms over the (2n+1)x(2n+1) neighborhood.
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
      float det = float(IxIx*IyIy-IxIy*IxIy);
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
  brip_vil1_float_ops::fill_x_border(vx, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(vx, n, 0.0f);
  brip_vil1_float_ops::fill_x_border(vy, n, 0.0f);
  brip_vil1_float_ops::fill_y_border(vy, n, 0.0f);
  std::cout << "\nCompute Lucas-Kanade in " << t.real() << " msecs.\n";
}

void brip_vil1_float_ops::fill_x_border(vil1_memory_image_of<float> & image,
                                        int w, float value)
{
  const int width = image.width(), height = image.height();
  if (2*w>width)
  {
    std::cout << "In brip_vil1_float_ops::fill_x_border(..) - 2xborder exceeds image width\n";
    return;
  }
  for (int y = 0; y<height; y++)
    for (int x = 0; x<w; x++)
      image(x, y) = value;

  for (int y = 0; y<height; y++)
    for (int x = width-w; x<width; x++)
      image(x, y) = value;
}

void brip_vil1_float_ops::fill_y_border(vil1_memory_image_of<float> & image,
                                        int h, float value)
{
  const int width = image.width(), height = image.height();
  if (2*h>height)
  {
    std::cout << "In brip_vil1_float_ops::fill_y_border(..) - 2xborder exceeds image height\n";
    return;
  }
  for (int y = 0; y<h; y++)
    for (int x = 0; x<width; x++)
      image(x, y) = value;

  for (int y = height-h; y<height; y++)
    for (int x = 0; x<width; x++)
      image(x, y) = value;
}

vil1_memory_image_of<unsigned char>
brip_vil1_float_ops::convert_to_byte(vil1_memory_image_of<float> const & image)
{
  //determine the max min values
  float min_val = vnl_numeric_traits<float>::maxval;
  float max_val = -min_val;
  const int w = image.width(), h = image.height();
  vil1_memory_image_of<unsigned char> output;
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      min_val = std::min(min_val, image(x,y));
      max_val = std::max(max_val, image(x,y));
    }
  float range = max_val-min_val;
  if (range == 0.f)
    range = 1.f;
  else
    range = 255.f/range;
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (unsigned char)((image(x,y)-min_val)*range);
  return output;
}

//------------------------------------------------------------
// Convert the range between min_val and max_val to 255
vil1_memory_image_of<unsigned char>
brip_vil1_float_ops::convert_to_byte(vil1_memory_image_of<float> const & image,
                                     const float min_val, const float max_val)
{
  const int w = image.width(), h = image.height();
  vil1_memory_image_of<unsigned char> output;
  output.resize(w,h);
  float range = max_val-min_val;
  if (range == 0.f)
    range = 1.f;
  else
    range = 255.f/range;
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      float v = (image(x,y)-min_val)*range;
      if (v>255)
        v=255;
      if (v<0)
        v=0;
      output(x,y) = (unsigned char)v;
    }
  return output;
}

vil1_memory_image_of<unsigned short>
brip_vil1_float_ops::convert_to_short(vil1_memory_image_of<float> const & image,
                                      const float min_val, const float max_val)
{
  const int w = image.width(), h = image.height();
  float max_short = 65355.f;
  vil1_memory_image_of<unsigned short> output;
  output.resize(w,h);
  float range = max_val-min_val;
  if (!range)
    range = 1;
  else
    range = max_short/range;
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
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

vil1_memory_image_of<vil1_rgb<unsigned char> >
brip_vil1_float_ops::convert_to_rgb(vil1_memory_image_of<float> const & image)
{
  vil1_memory_image_of<unsigned char> temp = brip_vil1_float_ops::convert_to_byte(image);
  const int w = temp.width(), h = temp.height();
  vil1_memory_image_of<vil1_rgb<unsigned char> > out(w, h);
  for (int r = 0; r<h; r++)
    for (int c = 0; c<w; c++)
      out(c,r).r = out(c,r).g = out(c,r).b = temp(c,r);
  return out;
}

vil1_memory_image_of<vil1_rgb<unsigned char> >
brip_vil1_float_ops::convert_to_rgb(vil1_memory_image_of<float> const & image,
                                    const float min_val, const float max_val)
{
  vil1_memory_image_of<unsigned char> temp = brip_vil1_float_ops::convert_to_byte(image,min_val,max_val);
  const int w = temp.width(), h = temp.height();
  vil1_memory_image_of<vil1_rgb<unsigned char> > out(w, h);
  for (int r = 0; r<h; r++)
    for (int c = 0; c<w; c++)
      out(c,r).r = out(c,r).g = out(c,r).b = temp(c,r);
  return out;
}


vil1_memory_image_of<float>
brip_vil1_float_ops::convert_to_float(vil1_memory_image_of<unsigned char> const & image)
{
  vil1_memory_image_of<float> output;
  const int w = image.width(), h = image.height();
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (float)image(x,y);
  return output;
}

vil1_memory_image_of<float>
brip_vil1_float_ops::convert_to_float(vil1_memory_image_of<vil1_rgb<unsigned char> > const & image)
{
  vil1_memory_image_of<float> output;
  const int w = image.width(), h = image.height();
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (float)image(x,y).grey();
  return output;
}

vil1_memory_image_of<float>
brip_vil1_float_ops::convert_to_float(vnl_matrix<float> const & matrix)
{
  unsigned int nr = matrix.rows(), nc = matrix.cols();
  vil1_memory_image_of<float> out(nc, nr);
  for (unsigned int r = 0; r<nr; ++r)
    for (unsigned int c = 0; c<nc; ++c)
      out(c,r)=matrix[r][c];
  return out;
}

static void rgb_to_ihs(vil1_rgb<unsigned char> const & rgb,
                       float& i, float& h, float& s)
{
  // Reference: figure 13.36, page 595 of Foley & van Dam
  float r = rgb.R(), g = rgb.G(), b = rgb.B();
  i = rgb.grey();

  float maxval = std::max(r, std::max(g, b));
  float minval = std::min(r, std::min(g, b));

  //lightness
  float la = (maxval + minval) / 2.f;
  // Achromatic case, intensity is grey or near black or white
  if (maxval == minval||i<20||i>235)
  {
    s = -1.f;
    h = 0.f;
  }
  else//the chromatic case
  {
    // Calculate the saturation.
    if (la <= 127)
    {
      s = (255.f*(maxval - minval))/(maxval + minval);
    }
    else
    {
      s = (255.f*(maxval - minval)) / (512.f - maxval - minval);
      if (s<0)
        s = 0.f;
      if (s>255)
        s = 255.f;
    }

    // Calculate the hue.
    float delta = maxval - minval;
    if (r == maxval)
    {
      // The resulting color is between yellow and magenta.
      h = (60*(g - b))/ delta;
    }
    else if (g == maxval)
    {
      // The resulting color is between cyan and yellow.
      h = 120.f + (60*(b - r))/delta;
    }
    else
    {
      // The resulting color is between magenta and cyan.
      h = 240.f + (60*(r - g))/delta;
    }
    // Be sure 0 <= hue <= 360
    if (h < 0)
      h += 360.f;
    if (h > 360)
      h -= 360.f;
  }
}


void brip_vil1_float_ops::
convert_to_IHS(vil1_memory_image_of<vil1_rgb<unsigned char> >const& image,
               vil1_memory_image_of<float>& I,
               vil1_memory_image_of<float>& H,
               vil1_memory_image_of<float>& S)
{
  const int w = image.width(), h = image.height();
  I.resize(w,h);
  H.resize(w,h);
  S.resize(w,h);
  for (int r = 0; r < h; r++)
    for (int c = 0; c < w; c++)
    {
      float in, hue, sat;
      rgb_to_ihs(image(c,r), in, hue, sat);
      I(c,r) = in;
      H(c,r) = hue;
      S(c,r) = sat;
    }
}

#if 0 // this method commented out
void brip_vil1_float_ops::
display_IHS_as_RGB(vil1_memory_image_of<float> const& I,
                   vil1_memory_image_of<float> const& H,
                   vil1_memory_image_of<float> const& S,
                   vil1_memory_image_of<vil1_rgb<unsigned char> >& image)
{
  const int w = I.width(), h = I.height();
  image.resize(w,h);
  float s = 255.0f/360.0f;
  for (int r = 0; r < h; r++)
    for (int c = 0; c < w; c++)
    {
      float in, hue, sat;
      in = I(c,r);
      hue = H(c,r);
      sat = S(c,r);
      if (in<0)
        in = 0;
      if (sat<0)
        sat = 0;
      if (hue<0)
        hue = 0;
      if (in>255)
        in = 255;
      hue *=s;
      if (hue>255)
        hue = 255;
      if (sat>255)
        sat = 255;
      unsigned char vi = (unsigned char)in, vh = (unsigned char)hue, vs = (unsigned char)sat;
      image(c,r) = vil1_rgb<unsigned char>(vi, vh, vs);
    }
}
#endif // 0

//: map so that intensity is proportional to saturation and hue is color
void brip_vil1_float_ops::
display_IHS_as_RGB(vil1_memory_image_of<float> const& I,
                   vil1_memory_image_of<float> const& H,
                   vil1_memory_image_of<float> const& S,
                   vil1_memory_image_of<vil1_rgb<unsigned char> >& image)
{
  const int w = I.width(), h = I.height();
  image.resize(w,h);

  const float deg_to_rad = float(vnl_math::pi_over_180);
  for (int r = 0; r < h; r++)
    for (int c = 0; c < w; c++)
    {
      float hue = H(c,r);
      float sat = 2.f*S(c,r);
      if (sat<0)
        sat = 0.f;
      if (sat>255)
        sat = 255.f;
      float ang = deg_to_rad*hue;
      float cs = std::cos(ang), si = std::fabs(std::sin(ang));
      float red,green,blue;
      green = si*sat;
      if (cs>=0)
      {
        red = cs*sat;
        blue = 0;
      }
      else
      {
        red = 0;
        blue = sat*(-cs);
      }
      unsigned char rc = (unsigned char)red,
        gc = (unsigned char)green, bc = (unsigned char)blue;
      image(c,r)= vil1_rgb<unsigned char>(rc, gc, bc);
    }
}

vil1_memory_image_of<float>
brip_vil1_float_ops::convert_to_float(vil1_image const & image)
{
  vil1_memory_image_of<float> fimg;
  if (image.components()==1)
  {
    if (image.component_format()==VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
    //already a float image
    {
      fimg = vil1_memory_image_of<float>(image);
      return fimg;
    }
    vil1_memory_image_of<unsigned char> temp(image);
    fimg = brip_vil1_float_ops::convert_to_float(temp);
  }
  else if (image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    fimg = brip_vil1_float_ops::convert_to_float(temp);
  }
  else
  {
    std::cout << "In brip_vil1_float_ops::convert_to_float - input not color or grey\n";
    return vil1_memory_image_of<float>();
  }
  return fimg;
}

//-----------------------------------------------------------------
// : convert a vil1_rgb<unsigned char> image to an unsigned_char image.
vil1_memory_image_of<unsigned char>
brip_vil1_float_ops::convert_to_grey(vil1_image const& image)
{
  if (!image)
    return vil1_memory_image_of<unsigned char>();

  //Check if the image is a float
  if (image.components()==1 &&
      image.component_format()==VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
    return brip_vil1_float_ops::convert_to_byte(vil1_memory_image_of<float>(image));

  //Here we assume that the image is an unsigned char
  //In this case we should just return it.
  if (image.components()!=3)
    return vil1_memory_image_of<unsigned char>(image);

  // the image is color so we should convert it to greyscale
  // Here we assume the color elements are unsigned char.
  vil1_memory_image_of<vil1_rgb<unsigned char> > color_image(image);
  const int width = color_image.width(), height = color_image.height();
  // the output image
  vil1_memory_image_of<unsigned char> grey_image;
  grey_image.resize(width, height);
  for (int y = 0; y<height; y++)
    for (int x = 0; x<width; x++)
      grey_image(x,y) = color_image(x,y).grey();
  return grey_image;
}

//--------------------------------------------------------------
// Read a convolution kernel from file
// Assumes a square kernel with odd dimensions, i.e., w,h = 2n+1
// format:
//     n
//     scale
//     k00  k01  ... k02n
//           ...
//     k2n0 k2n1 ... k2n2n
//
vbl_array_2d<float> brip_vil1_float_ops::load_kernel(std::string const & file)
{
  std::ifstream instr(file.c_str(), std::ios::in);
  if (!instr)
  {
    std::cout << "In brip_vil1_float_ops::load_kernel - failed to load kernel\n";
    return vbl_array_2d<float>(0,0);
  }
  int n;
  float scale;
  float v =0;
  instr >> n;
  instr >> scale;
  int N = 2*n+1;
  vbl_array_2d<float> output(N, N);
  for (int y = 0; y<N; y++)
    for (int x = 0; x<N; x++)
    {
      instr >> v;
      output.put(x, y, v/scale);
    }
  std::cout << "The Kernel\n";
  for (int y = 0; y<N; y++)
  {
    for (int x = 0; x<N; x++)
      std::cout << ' ' <<  output[x][y];
    std::cout << '\n';
  }
  return output;
}

static void insert_image(vil1_memory_image_of<float> const& image, int col,
                         vnl_matrix<float> & I)
{
  const int width = image.width(), height = image.height();
  for (int y = 0, row = 0; y<height; ++y)
    for (int x = 0; x<width; x++, ++row) // row runs from 0 to width*height-1
      I.put(row, col, image(x,y));
}

void brip_vil1_float_ops::
basis_images(std::vector<vil1_memory_image_of<float> > const & input_images,
             std::vector<vil1_memory_image_of<float> > & basis)
{
  basis.clear();
  int n_images = input_images.size();
  if (!n_images)
  {
    std::cout << "In brip_vil1_float_ops::basis_images(.) - no input images\n";
    return;
  }
  const int width = input_images[0].width(), height = input_images[0].height();
  const int npix = width*height;

  //Insert the images into matrix I
  vnl_matrix<float> I(npix, n_images, 0.f);
  for (int i = 0; i<n_images; i++)
    insert_image(input_images[i], i, I);

  //Compute the SVD of matrix I
  std::cout << "Computing Singular values of a " <<  npix << " by "
           << n_images << " matrix\n";
  vul_timer t;
  vnl_svd<float> svd(I);
  std::cout << "SVD Took " << t.real() << " msecs\n"
           << "Eigenvalues:\n";
  for (int i = 0; i<n_images; i++)
    std::cout << svd.W(i) << '\n';

  //Extract the Basis images
  int rank = svd.rank();
  if (!rank)
  {
    std::cout << "In brip_vil1_float_ops::basis_images(.) - I has zero rank\n";
    return;
  }
  vnl_matrix<float> U = svd.U();
  //Output the basis images
  int rows = U.rows();
  for (int k = 0; k<rank; k++)
  {
    vil1_memory_image_of<float> out(width, height);
    int x =0, y = 0;
    for (int r = 0; r<rows; r++)
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
        std::cout << "In brip_vil1_float_ops::basis_images(.) - shouldn't happen\n";
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
//                N-1
//                ---
//            1   \          - j k 2 pi n / N
//    X(n) = ---   >   x(k) e                    = forward transform
//            N   /                                n=0..N-1
//                ---
//                k=0
//
//    Formula: reverse
//                N-1
//                ---
//                \          j k 2 pi n / N
//    X(n) =       >   x(k) e                    = forward transform
//                /                                n=0..N-1
//                ---
//                k=0
//
bool brip_vil1_float_ops::fft_1d(int dir, int m, double* x, double* y)
{
  long nn,i,i1,j,k,i2,l,l1,l2;
  double c1,c2,tx,ty,t1,t2,u1,u2,z;

  /* Calculate the number of points */
  nn = 1;
  for (i=0;i<m;i++)
    nn *= 2;

  /* Do the bit reversal */
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
//  Perform a 2D FFT inplace given a complex 2D array
//  The direction dir, 1 for forward, -1 for reverse
//  The size of the array (nx,ny)
//  Return false if there are memory problems or
//  the dimensions are not powers of 2
//
bool brip_vil1_float_ops::fft_2d(vnl_matrix<std::complex<double> >& c,int nx,int ny,int dir)
{
  int i,j;
  int mx, my;
  double *real,*imag;
  vnl_fft_prime_factors<double> pfx (nx);
  vnl_fft_prime_factors<double> pfy (ny);
  mx = (int)pfx.pqr()[0];
  my = (int)pfy.pqr()[0];
  /* Transform the rows */
  real = new double[nx];
  imag = new double[nx];
  if (real == nullptr || imag == nullptr)
    return false;
  for (j=0;j<ny;j++) {
    for (i=0;i<nx;i++) {
      real[i] = c[j][i].real();
      imag[i] = c[j][i].imag();
    }
    brip_vil1_float_ops::fft_1d(dir,mx,real,imag);
    for (i=0;i<nx;i++) {
      std::complex<double> v(real[i], imag[i]);
      c[j][i] = v;
    }
  }
  delete [] real;
  delete [] imag;
  /* Transform the columns */
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
void brip_vil1_float_ops::
ftt_fourier_2d_reorder(vnl_matrix<std::complex<double> > const& F1,
                       vnl_matrix<std::complex<double> > & F2)
{
  int rows = F1.rows(), cols = F1.cols();
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

//:  Compute the fourier transform.
//   If the image dimensions are not a power of 2 then the operation fails.
bool brip_vil1_float_ops::
fourier_transform(vil1_memory_image_of<float> const & input,
                  vil1_memory_image_of<float>& mag,
                  vil1_memory_image_of<float>& phase)
{
  const int w = input.width(), h = input.height();
  vnl_fft_prime_factors<float> pfx (w);
  vnl_fft_prime_factors<float> pfy (h);
  if (!pfx.pqr()[0]||!pfy.pqr()[0])
    return false;
  //fill the fft matrix
  vnl_matrix<std::complex<double> > fft_matrix(h, w), fourier_matrix(h,w);
  for (int y = 0; y<h; y++)
    for (int x =0; x<w; x++)
    {
      std::complex<double> cv(input(x,y), 0.0);
      fft_matrix.put(y, x, cv);
    }
#ifdef DEBUG
  for (int r = 0; r<h; r++)
    for (int c =0; c<w; c++)
    {
      std::complex<double> res = fft_matrix[r][c];
      std::cout << res << '\n';
    }
#endif

  brip_vil1_float_ops::fft_2d(fft_matrix, w, h, 1);
  brip_vil1_float_ops::ftt_fourier_2d_reorder(fft_matrix, fourier_matrix);
  mag.resize(w,h);
  phase.resize(w,h);

  //extract magnitude and phase
  for (int r = 0; r<h; r++)
    for (int c = 0; c<w; c++)
    {
      float re = (float)fourier_matrix[r][c].real(), im = (float)fourier_matrix[r][c].imag();
      mag(c,r) = std::sqrt(re*re + im*im);
      phase(c,r) = std::atan2(im, re);
    }

  return true;
}

bool brip_vil1_float_ops::
inverse_fourier_transform(vil1_memory_image_of<float> const& mag,
                          vil1_memory_image_of<float> const& phase,
                          vil1_memory_image_of<float>& output)
{
  const int w = mag.width(), h = mag.height();
  vnl_matrix<std::complex<double> > fft_matrix(h, w), fourier_matrix(h, w);
  for (int y = 0; y<h; y++)
    for (int x =0; x<w; x++)
    {
      float m = mag(x,y);
      float p = phase(x,y);
      std::complex<double> cv(m*std::cos(p), m*std::sin(p));
      fourier_matrix.put(y, x, cv);
    }

  brip_vil1_float_ops::ftt_fourier_2d_reorder(fourier_matrix, fft_matrix);
  brip_vil1_float_ops::fft_2d(fft_matrix, w, h, -1);

  output.resize(w,h);

  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (float)fft_matrix[y][x].real();
  return true;
}

void brip_vil1_float_ops::resize(vil1_memory_image_of<float> const & input,
                                 const int width, const int height,
                                 vil1_memory_image_of<float>& output)
{
  const int w = input.width(), h = input.height();
  output.resize(width, height);
  for (int y = 0; y<height; y++)
    for (int x = 0; x<width; x++)
      if (x<w && y<h)
        output(x,y) = input(x,y);
      else
        output(x,y) = 0;//pad with zeroes
}

//: resize the input to the closest power of two image dimensions
bool brip_vil1_float_ops::
resize_to_power_of_two(vil1_memory_image_of<float> const & input,
                       vil1_memory_image_of<float>& output)
{
  const int max_exp = 13; //we wouldn't want to have such large images in memory
  const int w = input.width(), h = input.height();
  int prodw = 1, prodh = 1;
  //Find power of two width
  int nw, nh;
  for (nw = 1; nw<=max_exp; nw++)
    if (prodw>w)
      break;
    else
      prodw *= 2;
  if (nw==max_exp)
    return false;
  //Find power of two height
  for (nh = 1; nh<=max_exp; nh++)
    if (prodh>h)
      break;
    else
      prodh *= 2;
  if (nh==max_exp)
    return false;
  brip_vil1_float_ops::resize(input, prodw, prodh, output);

  return true;
}

//
//: block a periodic signal by suppressing two Gaussian lobes in the frequency domain.
//  The lobes are on the line defined by dir_fx and dir_fy through the
//  dc origin, assumed (0, 0).  The center frequency, f0, is the distance along
//  the line to the center of each blocking lobe (+- f0). radius is the
//  standard deviation of each lobe. Later we can define a "filter" class.
//
float brip_vil1_float_ops::gaussian_blocking_filter(const float dir_fx,
                                                    const float dir_fy,
                                                    const float f0,
                                                    const float radius,
                                                    const float fx,
                                                    const float fy)
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

bool brip_vil1_float_ops::
spatial_frequency_filter(vil1_memory_image_of<float> const & input,
                         const float dir_fx, const float dir_fy,
                         const float f0, const float radius,
                         const bool output_fourier_mag,
                         vil1_memory_image_of<float> & output)
{
  //Compute the fourier transform of the image.
  vil1_memory_image_of<float> pow_two, mag, bmag, phase, pow_two_filt;
  brip_vil1_float_ops::resize_to_power_of_two(input, pow_two);
  const int Nfx = pow_two.width(), Nfy = pow_two.height();

  if (!brip_vil1_float_ops::fourier_transform(pow_two, mag, phase))
    return false;
  bmag.resize(Nfx, Nfy);

  //filter the magnitude function
  float Ofx = Nfx*0.5f, Ofy = Nfy*0.5f;
  for (int fy =0; fy<Nfy; fy++)
    for (int fx =0; fx<Nfx; fx++)
    {
      float gb = gaussian_blocking_filter(dir_fx, dir_fy, f0,
                                          radius,
                                          fx-Ofx, fy-Ofy);
      bmag(fx,fy) = mag(fx,fy)*gb;
    }
  if (output_fourier_mag)
  {
    output = bmag;
    return true;
  }
  //Transform back
  pow_two_filt.resize(Nfx, Nfy);
  brip_vil1_float_ops::inverse_fourier_transform(bmag, phase, pow_two_filt);

  //Resize to original input size
  brip_vil1_float_ops::resize(pow_two_filt, input.width(), input.height(), output);
  return true;
}

//----------------------------------------------------------------------
//: Bi-linear interpolation on the neighborhood below.
//      xr
//   yr 0  x
//      x  x
//
float brip_vil1_float_ops::
bilinear_interpolation(vil1_memory_image_of<float> const & input,
                       const double x, const double y)
{
  //check bounds
  const int w = input.width(), h = input.height();
  //the pixel containing the interpolated point
  int xr = (int)x, yr = (int)y;
  double fx = x-xr, fy = y-yr;
  if (xr<0||xr>w-2)
    return 0.f;
  if (yr<0||yr>h-2)
    return 0.f;
  double int00 = input(xr, yr), int10 = input(xr+1,yr);
  double int01 = input(xr, yr+1), int11 = input(xr+1,yr+1);
  double int0 = int00 + fy * (int01 - int00);
  double int1 = int10 + fy * (int11 - int10);
  float val = (float) (int0 + fx * (int1 - int0));
  return val;
}

//: Transform the input to the output by a homography.
//  if the output size is fixed then only the corresponding
//  region of input image space is transformed.
bool brip_vil1_float_ops::homography(vil1_memory_image_of<float> const & input,
                                     vgl_h_matrix_2d<double>const& H,
                                     vil1_memory_image_of<float>& output,
                                     bool output_size_fixed,
                                     float output_fill_value)
{
  if (!input)
    return false;

  //First, there is some rather complex bookeeping to insure that
  //the input and output image rois are consistent with the homography.

  // the bounding boxes corresponding to input and output rois
  // We also construct polygons since homographies turn boxes into arbitrary
  // quadrilaterals.
  vsol_box_2d_sptr input_roi, output_roi;
  vsol_polygon_2d_sptr input_poly, output_poly;
  vgl_h_matrix_2d<double> Hinv;
  // set up the roi and poly for the input image
  const int win = input.width(), hin = input.height();
  input_roi = new vsol_box_2d();
  input_roi->add_point(0, 0);
  input_roi->add_point(win, hin);
  input_poly = bsol_algs::poly_from_box(input_roi);
  //Case I
  // the output image size and input transform can be adjusted
  // to map the transformed image onto the full range
  if (!output_size_fixed)
  {
    if (!bsol_algs::homography(input_poly, H, output_poly))
      return false;
    vsol_box_2d_sptr temp = output_poly->get_bounding_box();
    output.resize((int)temp->width(), (int)temp->height());
    output.fill(output_fill_value);
    //offset the transform and transformed roi so that lower left is (0,0)
    output_roi = new vsol_box_2d();
    output_roi->add_point(0, 0);
    output_roi->add_point(temp->width(), temp->height());
    vnl_matrix_fixed<double,3, 3> Mt = H.get_matrix();
    Mt[0][2] -= temp->get_min_x();  Mt[1][2] -= temp->get_min_y();
    vnl_matrix_fixed<double,3, 3> Mtinv = vnl_inverse(Mt);
    Hinv = vgl_h_matrix_2d<double> (Mtinv);
  }
  else // Case II, the output image size is fixed so we have to find the
  {  // inverse mapping of the output roi and intersect with the input roi
    //  to determine the domain of the mapping
    if (!output)
      return false;
    //The output roi and poly
    int wout = output.width(), hout = output.height();
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
  //At this point we have the correct bounds for the input and
  //the output image

  //Iterate over the output image space and map the location of each
  //pixel into the input image space. Then carry out interpolation to
  //get the value of each output pixel

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

  /* The inverse transform is used to map backwards from the output */
  const vnl_matrix_fixed<double,3,3>& Minv = Hinv.get_matrix();

  /* Now use Hinv to transform the image */
  for (int i = bilow; i<bihigh; i++)
    for (int j = bjlow; j<bjhigh; j++)
    {
      /* Transform the pixel */
      float val;
      double u = Minv[0][0] * i + Minv[0][1] * j + Minv[0][2];
      double v = Minv[1][0] * i + Minv[1][1] * j + Minv[1][2];
      double w = Minv[2][0] * i + Minv[2][1] * j + Minv[2][2];
      u /= w;
      v /= w;

      /* Now do linear interpolation */
      {
        int iu = (int) u;
        int iv = (int) v;
        double fu = u - iu;
        double fv = v - iv;

        if ((iu < ailow || iu >= aihigh-1) ||
            (iv < ajlow || iv >= ajhigh-1))
          continue;
        else
        {
          /* Get the neighbouring pixels */
          /*      (u  v)    (u+1  v)     */
          /*      (u v+1)   (u+1 v+1)    */
          /*                             */
          double v00 = input(iu, iv);
          double v01 = input(iu, iv+1);
          double v10 = input(iu+1,iv);
          double v11 = input(iu+1, iv+1);

          double v0 = v00 + fv * (v01 - v00);
          double v1 = v10 + fv * (v11 - v10);
          val = (float) (v0 + fu * (v1 - v0));
        }
        /* Set the value */
        output(i,j) = val;
      }
    }
  return true;
}

//: rotate the input image counter-clockwise about the image origin.
// demonstrates the use of image homography
vil1_memory_image_of<float>
brip_vil1_float_ops::rotate(vil1_memory_image_of<float> const & input,
                            const double theta_deg)
{
  vil1_memory_image_of<float> out;
  if (!input)
    return out;
  double ang = theta_deg;
  //map theta_deg to [0 360]
  while (ang>360)
    ang-=360;
  while (ang<0)
    ang+=360;
  //convert to radians
  double deg_to_rad = vnl_math::pi_over_180;
  double rang = deg_to_rad*ang;
  double c = std::cos(rang), s = std::sin(rang);
  vnl_matrix_fixed<double,3, 3> M;
  //counter clockwise rotation about the image origin (0, 0)
  M[0][0]= c;   M[0][1]= -s;  M[0][2]= 0;
  M[1][0]= s;   M[1][1]= c;   M[1][2]= 0;
  M[2][0]= 0;   M[2][1]= 0;   M[2][2]= 1;
  vgl_h_matrix_2d<double> H(M);
  vil1_memory_image_of<float> temp;
  //The transform is adjusted to map the full input domain onto
  //the output image.
  if (!brip_vil1_float_ops::homography(input, H, temp))
    return out;
  return temp;
}

bool brip_vil1_float_ops::chip(vil1_memory_image_of<float> const & input,
                               vsol_box_2d_sptr const& roi,
                               vil1_memory_image_of<float>& chp)
{
  if (!input||!roi)
    return false;

  const int w = input.width(), h = input.height();
  int x_min = (int)roi->get_min_x(), y_min = (int)roi->get_min_y();
  int x_max = (int)roi->get_max_x(), y_max = (int)roi->get_max_y();
  if (x_min<0)
    x_min = 0;
  if (y_min<0)
    y_min = 0;
  if (x_max>w-1)
    x_max=w-1;
  if (y_max>h-1)
    y_max=w-1;
  int rw = x_max-x_min, rh = y_max-y_min;
  if (rw<=0||rh<=0)
    return false;
  chp.resize(rw, rh);
  for (int y = y_min; y<y_max; y++)
    for (int x =x_min; x<x_max; x++)
      chp(x-x_min, y-y_min) = input(x, y);
  return true;
}

//: Chipping for a general image type
bool brip_vil1_float_ops::chip(vil1_image const & input,
                               brip_roi_sptr const& roi,
                               vil1_image& chip)
{
  if (!input||!roi)
    return false;
  const int Nc = input.width(), Nr = input.height();
  int c_min = roi->cmin(0), r_min = roi->rmin(0);
  int c_max = roi->cmax(0), r_max = roi->rmax(0);
  if (c_min<0)
    c_min = 0;
  if (r_min<0)
    r_min = 0;
  if (c_max>Nc-1)
    c_max=Nc-1;
  if (r_max>Nr-1)
    r_max=Nc-1;
  int CNc = c_max-c_min, CNr = r_max-r_min;
  if (CNc<=0||CNr<=0)
    return false;

  if (input.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > timage(input);
    vil1_memory_image_of<vil1_rgb<unsigned char> > tchip(CNc, CNr);
    for (int r = r_min; r<r_max; r++)
      for (int c =c_min; c<c_max; c++)
        tchip(c-c_min, r-r_min) = timage(c, r);
    chip = tchip;
    return true;
  }

  if (input.component_format()==VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
  {
    vil1_memory_image_of<float> timage(input);
    vil1_memory_image_of<float> tchip(CNc, CNr);
    for (int r = r_min; r<r_max; r++)
      for (int c =c_min; c<c_max; c++)
        tchip(c-c_min, r-r_min) = timage(c, r);
    chip = tchip;
    return true;
  }

  if (input.get_size_bytes() ==2)
  {
    vil1_memory_image_of<unsigned short> timage(input);
    vil1_memory_image_of<unsigned short> tchip(CNc, CNr);
    for (int r = r_min; r<r_max; r++)
      for (int c =c_min; c<c_max; c++)
        tchip(c-c_min, r-r_min) = timage(c, r);
    chip = tchip;
    return true;
  }
  //BAD but for now force to byte output regardless FIX FIX FIX!
  //if (input.get_size_bytes() ==1)
  if (true)
  {
    vil1_memory_image_of<unsigned char> timage(input);
    vil1_memory_image_of<unsigned char> tchip(CNc, CNr);
    for (int r = r_min; r<r_max; r++)
      for (int c =c_min; c<c_max; c++)
        tchip(c-c_min, r-r_min) = timage(c, r);
    chip = tchip;
    return true;
  }

  return false;
}

//:assumes that the chip and image have the same pixel types.  Only works for
// color at present.
vil1_image brip_vil1_float_ops::insert_chip_in_image(vil1_image const & image,
                                                     vil1_image const & chip,
                                                     brip_roi_sptr const& roi)
{
  if (!chip||!roi)
    return image;
  //copy the input
  vil1_image temp(image);
  const int chip_cols = chip.width(), chip_rows = chip.height();
  //need to do cases
  //but just color now
  if (image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > timage(image);
    vil1_memory_image_of<vil1_rgb<unsigned char> > tchip(chip);
    for (int cr = 0; cr<chip_rows; cr++)
      for (int cc = 0; cc<chip_cols; cc++)
      {
        int imgc = roi->ic(cc), imgr = roi->ir(cr);
        timage(imgc, imgr) = tchip(cc, cr);
      }
  return timage;
  }
  return image;//no op
}

//:compute normalized cross correlation from the intensity moment sums.
static float cross_corr(const double area, const double si1, const double si2,
                        const double si1i1,
                        const double si2i2, const double si1i2,
                        const float intensity_thresh)
{
  if (!area)
    return 0.f;
  //the mean values
  double u1 = si1/area, u2 = si2/area;
  if (u1<intensity_thresh||u2<intensity_thresh)
    return -1.f;
  double neu = si1i2 - area*u1*u2;
  double sd1 = std::sqrt(si1i1-area*u1*u1), sd2 = std::sqrt(si2i2-area*u2*u2);
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

//:perform normalized cross-correlation at a sub-pixel location
// thus all the pixel values are interpolated.
float brip_vil1_float_ops::
cross_correlate(vil1_memory_image_of<float> const & image1,
                vil1_memory_image_of<float> const & image2,
                const float x, const float y,
                const int radius,
                const float intensity_thresh)
{
  const int w1 = image1.width(), h1 = image1.height();
  const int w2 = image1.width(), h2 = image1.height();
  //bounds checks
  if (w1!=w2||h1!=h2)
    return -1;
  if (x<radius||x>w1-radius-1||y<radius||y>h1-radius-1)
    return -1;

  //accumulate correlation sums,
  //bi-linear interpolate the values
  double sI1=0, sI2=0, sI1I1=0, sI2I2=0, sI1I2=0;
  for (int y0 = -10*radius; y0<=10*radius; ++y0)
    for (int x0 = -10*radius; x0<=10*radius; ++x0)
    {
      float xp = x+0.1f*x0, yp = y+0.1f*y0;
      double v1 = brip_vil1_float_ops::bilinear_interpolation(image1, xp, yp);
      double v2 = brip_vil1_float_ops::bilinear_interpolation(image2, xp, yp);
      sI1 += v1;
      sI2 += v2;
      sI1I1 += v1*v1;
      sI2I2 += v2*v2;
      sI1I2 += v1*v2;
    }
  //:compute correlation.
  int s = 2*radius+1;
  double area = s*s;
  return cross_corr(area, sI1, sI2, sI1I1, sI2I2, sI1I2, intensity_thresh);
}

//: r0 is the image from which to read the new intensity values
//  r is the summing array row in which the values are to be accumulated
static bool update_row(vil1_memory_image_of<float> const& image1,
                       vil1_memory_image_of<float> const& image2,
                       const int r0,
                       const int r,
                       vbl_array_2d<double>& SI1,
                       vbl_array_2d<double>& SI2,
                       vbl_array_2d<double>& SI1I1,
                       vbl_array_2d<double>& SI2I2,
                       vbl_array_2d<double>& SI1I2)
{
  const int w1 = image1.width(), h1 = image1.height();
  const int w2 = image2.width(), h2 = image2.height();
  if (w1!=w2||h1!=h2||r<0||r>=h1)
    return false;
  double i10 = image1(0,r0), i20 = image2(0,r0);
  SI1[r][0] = i10; SI2[r][0] = i20; SI1I1[r][0]=i10*i10;
  SI2I2[r][0]=i20*i20; SI1I2[r][0]=i10*i20;
  for (int c = 1; c<w1; c++)
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

static bool initialize_slice(vil1_memory_image_of<float> const& image1,
                             vil1_memory_image_of<float> const& image2,
                             const int radius,
                             vbl_array_2d<double>& SI1,
                             vbl_array_2d<double>& SI2,
                             vbl_array_2d<double>& SI1I1,
                             vbl_array_2d<double>& SI2I2,
                             vbl_array_2d<double>& SI1I2)
{
  for (int r = 0; r<=2*radius; r++)
    if (!update_row(image1, image2, r, r, SI1, SI2, SI1I1, SI2I2, SI1I2))
      return false;
  return true;
}

static bool collapse_slice( vbl_array_2d<double> const& SI1,
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
  //sanity check
  int w = SI1.cols(), h = SI1.rows();
  int dw = SI1.cols();
  if (dw!=w)
    return false;

  for (int c = 0; c<w; c++)
  {
    dSI1[c]=0; dSI2[c]=0; dSI1I1[c]=0;
    dSI2I2[c]=0; dSI1I2[c]=0;
    for (int r = 0; r<h; r++)
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
                                vbl_array_1d<float>& cc
                               )
{
  //sanity check
  int w = dSI1.size(), wc = cc.size();
  if (!w||!wc||w!=wc)
    return false;
  int s = 2*radius+1;
  double area = s*s;
  //the general case
  double si1=dSI1[s-1], si2=dSI2[s-1], si1i1=dSI1I1[s-1],
    si2i2=dSI2I2[s-1], si1i2=dSI1I2[s-1];
  cc[radius]= cross_corr(area, si1, si2, si1i1, si2i2, si1i2, intensity_thresh);
  //the remaining columns
  for (int c = radius+1; c+radius<w; c++)
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
  int nr = S.rows(), nc = S.cols();
  for (int r = 0; r<nr-1; r++)
    for (int c =0; c<nc; c++)
      S[r][c]=S[r+1][c];
}


static bool output_cc_row(const int r0,  vbl_array_1d<float> const& cc,
                          vil1_memory_image_of<float>& out)
{
  const int n = cc.size(), w = out.width();
  if (n!=w)
    return false;
  for (int c = 0; c<w; c++)
    out(c, r0) = cc[c];
  return true;
}


bool brip_vil1_float_ops::
cross_correlate(vil1_memory_image_of<float> const & image1,
                vil1_memory_image_of<float> const & image2,
                vil1_memory_image_of<float>& out,
                const int radius,
                const float intensity_thresh)
{
  vul_timer t;
  const int w = image1.width(), h = image1.height();
  const int w2 = image2.width(), h2 = image2.height();
  //sizes must match
  if (w!=w2||h!=h2)
  {
    std::cout << "In brip_vil1_float_ops::fast_cross_correlate(..) -"
             << " image sizes don't match\n";
    return false;
  }
  out.resize(w, h);
  out.fill(0.f);
  int s = 2*radius+1;
  //Create the running sum slices
  vbl_array_2d<double> SI1(s,w), SI2(s,w),
    SI1I1(s,w), SI2I2(s,w), SI1I2(s,w);
  vbl_array_1d<float> cc(w, 0.f);
  vbl_array_1d<double> dSI1(w, 0.0), dSI2(w, 0.0),
    dSI1I1(w, 0.0), dSI2I2(w, 0.0), dSI1I2(w, 0.0);
  initialize_slice(image1, image2, radius, SI1, SI2, SI1I1, SI2I2, SI1I2);
  if (!collapse_slice(SI1,  SI2,  SI1I1,  SI2I2,  SI1I2,
                      dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2))
    return false;
  int r0 = radius;
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
      return out;
    if (!collapse_slice(SI1,  SI2,  SI1I1,  SI2I2,  SI1I2,
                        dSI1, dSI2, dSI1I1, dSI2I2, dSI1I2))
      return false;
    if (!output_cc_row(r0, cc, out))
      return false;
  }
  //handle the last row
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
  std::cout << "RunningSumCrossCorrelation for " << w*h/1000.0f << " k pixels in "
           << t.real() << " msecs\n"<< std::flush;
  return true;
}
