#include "brip_float_ops.h"
//:
// \file

#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vil1/vil1_smooth.h>

//------------------------------------------------------------
//:  Convolve with a kernel
//   It's assumed that the kernel is square with odd dimensions
vil1_memory_image_of<float>
brip_float_ops::convolve(vil1_memory_image_of<float> const & input,
                         vbl_array_2d<float> const & kernel)
{
  int w = input.width(), h = input.height();
  int kw = kernel.cols(); // kh = kernel.rows();
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
  brip_float_ops::fill_x_border(output, n, 0.0f);
  brip_float_ops::fill_y_border(output, n, 0.0f);
  return output;
}

static void fill_1d_array(vil1_memory_image_of<float> const & input,
                          const int y, float* output)
{
  int w = input.width();
  for (int x = 0; x<w; x++)
    output[x] = input(x,y);
}

//: Downsamples the 1-d array by 2 using the Burt-Adelson reduction algorithm.
void brip_float_ops::half_resolution_1d(const float* input, int width,
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
    if (x<width-2)
      {w[3] = input[n++]; w[4]= input[n++];}
    else
      {w[3] =w[1]; w[4]= w[0];}
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
brip_float_ops::half_resolution(vil1_memory_image_of<float> const & input,
                                float filter_coef)
{
  vul_timer t;
  float k0 = filter_coef, k1 = 0.25f*filter_coef, k2 = 0.5f*(0.5f-filter_coef);
  int w = input.width(), h = input.height();
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
  brip_float_ops::half_resolution_1d(in0, half_w, k0, k1, k2, out0);
  brip_float_ops::half_resolution_1d(in1, half_w, k0, k1, k2, out1);
  brip_float_ops::half_resolution_1d(in2, half_w, k0, k1, k2, out2);
  brip_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
  brip_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
  int x=0, y=0;
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
      brip_float_ops::half_resolution_1d(in3, half_w, k0, k1, k2, out3);
      brip_float_ops::half_resolution_1d(in4, half_w, k0, k1, k2, out4);
    }
  }
  delete [] in0;  delete [] in1; delete [] in2;
  delete [] in3;  delete [] in4;
  delete [] out0;  delete [] out1; delete [] out2;
  delete [] out3;  delete [] out4;
  vcl_cout << "\nDownsample a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
  return output;
}

vil1_memory_image_of<float>
brip_float_ops::gaussian(vil1_memory_image_of<float> const & input, float sigma)
{
  vil1_memory_image_of<float> output(vil1_smooth_gaussian(input, sigma));
  return output;
}
//-------------------------------------------------------------------
// Determine if the center of a (2n+1)x(2n+1) neighborhood is a local maximum
//
bool brip_float_ops::
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
void brip_float_ops::
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
    if (vcl_fabs(dx) > 1.0 || vcl_fabs(dy) > 1.0)
    {
      dx = 0; dy = 0;
    }
  }
}

//---------------------------------------------------------------
// Compute the local maxima of the input on a (2n+1)x(2n+2)
// neighborhood above the given threshold. At each local maximum,
// compute the sub-pixel location, (x_pos, y_pos).
void brip_float_ops::
non_maximum_suppression(vil1_memory_image_of<float> const & input,
                        const int n,
                        const float thresh,
                        vcl_vector<float>& x_pos,
                        vcl_vector<float>& y_pos,
                        vcl_vector<float>& value)
{
  vul_timer t;
  int N = 2*n+1;
  int w = input.width(), h = input.height();
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
      if (brip_float_ops::local_maximum(neighborhood, n, max_v))
      {
        //if so sub-pixel interpolate (3x3) and output results
        brip_float_ops::interpolate_center(neighborhood, dx, dy);
        x_pos.push_back(x+dx);
        y_pos.push_back(y+dy);
        value.push_back(max_v);
      }
    }
  vcl_cout << "\nCompute non-maximum suppression on a "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

// -----------------------------------------------------------------
// Subtract image_1 from image_2.
// Will not operate unless the two input images are the same dimensions
//
vil1_memory_image_of<float>
brip_float_ops::difference(vil1_memory_image_of<float> const & image_1,
                           vil1_memory_image_of<float> const & image_2)
{
  int w1 = image_1.width(), h1 = image_1.height();
  int w2 = image_2.width(), h2 = image_2.height();
  vil1_memory_image_of<float> temp(w1, h1);
  if (w1!=w2||h1!=h2)
  {
    vcl_cout << "In brip_float_ops::difference(..) - images are not the same dimensions\n";
    return temp;
  }
  vil1_memory_image_of<float> out;
  out.resize(w1, h1);
  for (int y = 0; y<h1; y++)
    for (int x = 0; x<w1; x++)
      out(x,y) = image_2(x,y)-image_1(x,y);
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
void brip_float_ops::gradient_3x3(vil1_memory_image_of<float> const & input,
                                  vil1_memory_image_of<float>& grad_x,
                                  vil1_memory_image_of<float>& grad_y)
{
  vul_timer t;
  int w = input.width(), h = input.height();
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
  brip_float_ops::fill_x_border(grad_x, 1, 0.0f);
  brip_float_ops::fill_y_border(grad_x, 1, 0.0f);
  brip_float_ops::fill_x_border(grad_y, 1, 0.0f);
  brip_float_ops::fill_y_border(grad_y, 1, 0.0f);
  vcl_cout << "\nCompute Gradient in " << t.real() << " msecs.\n";
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
void brip_float_ops::hessian_3x3(vil1_memory_image_of<float> const & input,
                                 vil1_memory_image_of<float>& Ixx,
                                 vil1_memory_image_of<float>& Ixy,
                                 vil1_memory_image_of<float>& Iyy)
{
  vul_timer t;
  int w = input.width(), h = input.height();
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
  brip_float_ops::fill_x_border(Ixx, 1, 0.0f);
  brip_float_ops::fill_y_border(Ixx, 1, 0.0f);
  brip_float_ops::fill_x_border(Ixy, 1, 0.0f);
  brip_float_ops::fill_y_border(Ixy, 1, 0.0f);
  brip_float_ops::fill_x_border(Iyy, 1, 0.0f);
  brip_float_ops::fill_y_border(Iyy, 1, 0.0f);
  vcl_cout << "\nCompute a hessian matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

vil1_memory_image_of<float>
brip_float_ops::beaudet(vil1_memory_image_of<float> const & Ixx,
                        vil1_memory_image_of<float> const & Ixy,
                        vil1_memory_image_of<float> const & Iyy
                       )
{
  int w = Ixx.width(), h = Ixx.height();
  vil1_memory_image_of<float> output;
  output.resize(w, h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      double xx = Ixx(x,y), xy = Ixy(x,y), yy = Iyy(x,y);

     //compute eigenvalues for experimentation
      double det = xx*yy-xy*xy;
      double tr = xx+yy;
      double arg = tr*tr-4.0*det, lambda0 = 0, lambda1=0;
      if (arg>0)
      {
        lambda0 = tr+vcl_sqrt(arg);
        lambda1 = tr-vcl_sqrt(arg);
      }
      output(x,y)=float(lambda0*lambda1); //just det for now
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
// over a a 2n+1 x 2n+1 neigborhood
//
void
brip_float_ops::grad_matrix_NxN(vil1_memory_image_of<float> const & input,
                                const int n,
                                vil1_memory_image_of<float>& IxIx,
                                vil1_memory_image_of<float>& IxIy,
                                vil1_memory_image_of<float>& IyIy)
{
  int w = input.width(), h = input.height();
  int N = (2*n+1)*(2*n+1);
  vil1_memory_image_of<float> grad_x, grad_y, output;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  output.resize(w,h);
  brip_float_ops::gradient_3x3(input, grad_x, grad_y);
  vul_timer t;
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      float xx=0, xy=0, yy=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          double gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          xx += float(gx*gx);
          xy += float(gx*gy);
          yy += float(gy*gy);
        }
      IxIx(x,y) = xx/N;
      IxIy(x,y) = xy/N;
      IyIy(x,y) = yy/N;
    }
  brip_float_ops::fill_x_border(IxIx, n, 0.0f);
  brip_float_ops::fill_y_border(IxIx, n, 0.0f);
  brip_float_ops::fill_x_border(IxIy, n, 0.0f);
  brip_float_ops::fill_y_border(IxIy, n, 0.0f);
  brip_float_ops::fill_x_border(IyIy, n, 0.0f);
  brip_float_ops::fill_y_border(IyIy, n, 0.0f);
  vcl_cout << "\nCompute a gradient matrix "<< w <<" x " << h << " image in "<< t.real() << " msecs.\n";
}

vil1_memory_image_of<float>
brip_float_ops::harris(vil1_memory_image_of<float> const & IxIx,
                       vil1_memory_image_of<float> const & IxIy,
                       vil1_memory_image_of<float> const & IyIy,
                       const double scale)

{
  int w = IxIx.width(), h = IxIx.height();
  float norm = 1e-3f; // Scale the output to values in the 10->1000 range
  vil1_memory_image_of<float> output;
  output.resize(w, h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      double xx = IxIx(x,y), xy = IxIy(x,y), yy = IyIy(x,y);
      double det = xx*yy-xy*xy, trace = xx+yy;
      output(x,y) = (float)(det - scale*trace*trace)*norm;
    }
  return output;
}

//----------------------------------------------------------------
// Compute the sqrt of the product of the eigenvalues of the
// gradient matrix over a 2n+1 x 2n+1 neigborhood
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
brip_float_ops::sqrt_grad_singular_values(vil1_memory_image_of<float> & input,
                                          int n)
{
  int N = (2*n+1)*(2*n+1);
  int w = input.width(), h = input.height();
  vil1_memory_image_of<float> grad_x, grad_y, output;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  output.resize(w,h);
  brip_float_ops::gradient_3x3(input, grad_x, grad_y);
  vul_timer t;
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      double IxIx=0, IxIy=0, IyIy=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          double gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          IxIx += gx*gx;
          IxIy += gx*gy;
          IyIy += gy*gy;
        }
      double det = (IxIx*IyIy-IxIy*IxIy)/N;
      output(x,y)=(float)vcl_sqrt(vcl_fabs(det));
    }
  brip_float_ops::fill_x_border(output, n, 0.0f);
  brip_float_ops::fill_y_border(output, n, 0.0f);
  vcl_cout << "\nCompute sqrt(sigma0*sigma1) in" << t.real() << " msecs.\n";
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
brip_float_ops::Lucas_KanadeMotion(vil1_memory_image_of<float> & current_frame,
                                   vil1_memory_image_of<float> & previous_frame,
                                   int n, double thresh,
                                   vil1_memory_image_of<float>& vx,
                                   vil1_memory_image_of<float>& vy)
{
  int N = (2*n+1)*(2*n+1);
  int w = current_frame.width(), h = current_frame.height();
  vil1_memory_image_of<float> grad_x, grad_y, diff;
  grad_x.resize(w,h);
  grad_y.resize(w,h);
  //compute the gradient vector and the time derivative
  brip_float_ops::gradient_3x3(current_frame, grad_x, grad_y);
  diff = brip_float_ops::difference(previous_frame, current_frame);
  vul_timer t;
  //sum the motion terms over the (2n+1)x(2n+1) neighborhood.
  for (int y = n; y<h-n;y++)
    for (int x = n; x<w-n;x++)
    {
      double IxIx=0, IxIy=0, IyIy=0, IxIt=0, IyIt=0;
      for (int i = -n; i<=n; i++)
        for (int j = -n; j<=n; j++)
        {
          double gx = grad_x(x+i, y+j), gy = grad_y(x+i, y+j);
          double dt = diff(x+i, y+j);
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
      double dif = diff(x,y);
      double motion_factor = vcl_fabs(det*dif);
      if (motion_factor<thresh)
      {
        vx(x,y) = 0.0f;
        vy(x,y) = 0.0f;
        continue;
      }
      //solve for the motion vector
      vx(x,y) = float(IyIy*IxIt-IxIy*IyIt)/det;
      vy(x,y) = float(-IxIy*IxIt + IxIx*IyIt)/det;
    }
  brip_float_ops::fill_x_border(vx, n, 0.0f);
  brip_float_ops::fill_y_border(vx, n, 0.0f);
  brip_float_ops::fill_x_border(vy, n, 0.0f);
  brip_float_ops::fill_y_border(vy, n, 0.0f);
  vcl_cout << "\nCompute Lucas-Kanade in " << t.real() << " msecs.\n";
}

void brip_float_ops::fill_x_border(vil1_memory_image_of<float> & image,
                                   int w, float value)
{
  int width = image.width(), height = image.height();
  if (2*w>width)
  {
    vcl_cout << "In brip_float_ops::fill_x_border(..) - 2xborder exceeds image width\n";
    return;
  }
  for (int y = 0; y<height; y++)
    for (int x = 0; x<w; x++)
      image(x, y) = value;

  for (int y = 0; y<height; y++)
    for (int x = width-w; x<width; x++)
      image(x, y) = value;
}

void brip_float_ops::fill_y_border(vil1_memory_image_of<float> & image,
                                   int h, float value)
{
  int width = image.width(), height = image.height();
  if (2*h>height)
  {
    vcl_cout << "In brip_float_ops::fill_y_border(..) - 2xborder exceeds image height\n";
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
brip_float_ops::convert_to_byte(vil1_memory_image_of<float> const & image)
{
  //determine the max min values
  float min_val = vnl_numeric_traits<float>::maxval;
  float max_val = -min_val;
  int w = image.width(), h = image.height();
  vil1_memory_image_of<unsigned char> output;
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      min_val = vnl_math_min(min_val, image(x,y));
      max_val = vnl_math_max(max_val, image(x,y));
    }
  float range = max_val-min_val;
  if (range == 0.f)
    range = 1.f;
  else
    range = 255.f/range;
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      float v = (image(x,y)-min_val)*range;
      output(x,y) = (unsigned char)v;
    }
  return output;
}

//------------------------------------------------------------
// Convert the range between min_val and max_val to 255
vil1_memory_image_of<unsigned char>
brip_float_ops::convert_to_byte(vil1_memory_image_of<float> const & image,
                                const float min_val, const float max_val)
{
  int w = image.width(), h = image.height();
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
brip_float_ops::convert_to_short(vil1_memory_image_of<float> const & image,
                                 const float min_val, const float max_val)
{
  int w = image.width(), h = image.height();
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

vil1_memory_image_of<float>
brip_float_ops::convert_to_float(vil1_memory_image_of<unsigned char> const & image)
{
  vil1_memory_image_of<float> output;
  int w = image.width(), h = image.height();
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (float)image(x,y);
  return output;
}

vil1_memory_image_of<float>
brip_float_ops::convert_to_float(vil1_memory_image_of<vil1_rgb<unsigned char> > const & image)
{
  vil1_memory_image_of<float> output;
  int w = image.width(), h = image.height();
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
    {
      vil1_rgb<unsigned char> rgb = image(x,y);
      output(x,y) = (float)rgb.grey();
    }
  return output;
}

vil1_memory_image_of<float>
brip_float_ops::convert_to_float(vil1_image const & image)
{
  vil1_memory_image_of<float> fimg;
  if (image.components()==1)
  {
    vil1_memory_image_of<unsigned char> temp(image);
    fimg = brip_float_ops::convert_to_float(temp);
  }
  else if (image.components()==3)
  {
    vil1_memory_image_of<vil1_rgb<unsigned char> > temp(image);
    fimg = brip_float_ops::convert_to_float(temp);
  }
  else
  {
    vcl_cout << "In brip_float_ops::convert_to_float - input not color or grey\n";
    return vil1_memory_image_of<float>();
  }
  return fimg;
}
//-----------------------------------------------------------------
// : convert a vil1_rgb<unsigned char> image to an unsigned_char image.
vil1_memory_image_of<unsigned char>
 brip_float_ops::convert_to_grey(vil1_image const& image)
{
  if (!image)
    return vil1_memory_image_of<unsigned char>();

  //Check if the image is a float
  if (image.components()==1 &&
      image.component_format()==VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
    return brip_float_ops::convert_to_byte(vil1_memory_image_of<float>(image));

  //Here we assume that the image is an unsigned char
  //In this case we should just return it.
  if (image.components()!=3)
    return vil1_memory_image_of<unsigned char>(image);

  // the image is color so we should convert it to greyscale
  // Here we assume the color elements are unsigned char.
  vil1_memory_image_of<vil1_rgb<unsigned char> > color_image(image);
  int width = color_image.width(), height = color_image.height();
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
vbl_array_2d<float> brip_float_ops::load_kernel(vcl_string const & file)
{
  vcl_ifstream instr(file.c_str(), vcl_ios::in);
  if (!instr)
  {
    vcl_cout << "In brip_float_ops::load_kernel - failed to load kernel\n";
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
  vcl_cout << "The Kernel\n";
  for (int y = 0; y<N; y++)
  {
    for (int x = 0; x<N; x++)
    {
      float t = output[x][y];
      vcl_cout << ' ' << t;
    }
    vcl_cout << '\n';
  }
  return output;
}
