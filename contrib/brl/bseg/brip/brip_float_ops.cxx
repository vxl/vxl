#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vil/vil_smooth.h>
#include "brip_float_ops.h"
//------------------------------------------------------------
//  Convolve with a kernel
//   It's assumed that the kernel is square with odd dimensions
vil_memory_image_of<float>
brip_float_ops::convolve(vil_memory_image_of<float> const & input,
                         vbl_array_2d<float> const & kernel)
{
  int w = input.width(), h = input.height();
  int kw = kernel.cols(); // kh = kernel.rows();
  // add a check for kernels that are not equal dimensions of odd size JLM
  int n = (kw-1)/2;
  vil_memory_image_of<float> output;
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
  brip_float_ops::fill_x_border(output, n, 0.0);
  brip_float_ops::fill_y_border(output, n, 0.0);
  return output;
}

vil_memory_image_of<float>
brip_float_ops::gaussian(vil_memory_image_of<float> const & input, float sigma)
{
  vil_memory_image_of<float> output(vil_smooth_gaussian(input, sigma));
  return output;
}

// -----------------------------------------------------------------
// Subtract image_1 from image_2.
// Will not operate unless the two input images are the same dimensions
//
vil_memory_image_of<float>
brip_float_ops::difference(vil_memory_image_of<float> const & image_1,
                           vil_memory_image_of<float> const & image_2)
{
  int w1 = image_1.width(), h1 = image_1.height();
  int w2 = image_2.width(), h2 = image_2.height();
  vil_memory_image_of<float> temp(w1, h1);
  if (w1!=w2||h1!=h2)
    {
      vcl_cout << "In brip_float_ops::difference(..) - images are not"
               << " the same dimensions\n";
      return temp;
    }
  vil_memory_image_of<float> out;
  out.resize(w1, h1);
  for (int y = 0; y<h1; y++)
    for (int x = 0; x<w1; x++)
      out(x,y) = image_2(x,y)-image_1(x,y);
  return out;
}

//----------------------------------------------------------------
// Compute the gradient of the input, use a 3x3 mask
//
void brip_float_ops::gradient_3x3(vil_memory_image_of<float> const & input,
                                  vil_memory_image_of<float>& grad_x,
                                  vil_memory_image_of<float>& grad_y)
{
  vul_timer t;
  int w = input.width(), h = input.height();
  float scale = 1.0f/6.0f;
  for (int y = 1; y<h-1; y++)
    for (int x = 0; x<w-1; x++)
      {
        float gx = input(x+1,y-1)+input(x+1,y)+ input(x+1,y-1)
          -input(x-1,y-1) -input(x-1,y) -input(x-1,y-1);
        float gy = input(x+1,y+1)+input(x,y+1)+ input(x-1,y+1)
          -input(x+1,y-1) -input(x,y-1) -input(x-1,y-1);
        grad_x(x,y) = scale*gx;
        grad_y(x,y) = scale*gy;
      }
  brip_float_ops::fill_x_border(grad_x, 1, 0.0);
  brip_float_ops::fill_y_border(grad_x, 1, 0.0);
  brip_float_ops::fill_x_border(grad_y, 1, 0.0);
  brip_float_ops::fill_y_border(grad_y, 1, 0.0);
  vcl_cout << "\nCompute Gradient in " << t.real() << " msecs.\n";
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
vil_memory_image_of<float>
brip_float_ops::sqrt_grad_singular_values(vil_memory_image_of<float> & input,
                                          int n)
{
  int N = (2*n+1)*(2*n+1);
  int w = input.width(), h = input.height();
  vil_memory_image_of<float> grad_x, grad_y, output;
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
  brip_float_ops::fill_x_border(output, n, 0.0);
  brip_float_ops::fill_y_border(output, n, 0.0);
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
brip_float_ops::Lucas_KanadeMotion(vil_memory_image_of<float> & current_frame,
                                   vil_memory_image_of<float> & previous_frame,
                                   int n, double thresh,
                                   vil_memory_image_of<float>& vx,
                                   vil_memory_image_of<float>& vy)
{
  int N = (2*n+1)*(2*n+1);
  int w = current_frame.width(), h = current_frame.height();
  vil_memory_image_of<float> grad_x, grad_y, diff;
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
        double det = (IxIx*IyIy-IxIy*IxIy);
        //Eliminate small motion factors
        double dif = diff(x,y);
        double motion_factor = fabs(det*dif);
        if(motion_factor<thresh)
          {
            vx(x,y) = 0.0;
            vy(x,y) = 0.0;
            continue;
          }
        //solve for the motion vector
        vx(x,y) = (IyIy*IxIt-IxIy*IyIt)/det;
        vy(x,y) = (-IxIy*IxIt + IxIx*IyIt)/det;
      }
  brip_float_ops::fill_x_border(vx, n, 0.0);
  brip_float_ops::fill_y_border(vx, n, 0.0);
  brip_float_ops::fill_x_border(vy, n, 0.0);
  brip_float_ops::fill_y_border(vy, n, 0.0);
  vcl_cout << "\nCompute Lucas-Kanade in" << t.real() << " msecs.\n";
}

void brip_float_ops::fill_x_border(vil_memory_image_of<float> & image,
                                   int w, float value)
{
  int width = image.width(), height = image.height();
  if (2*w>width)
    {
      vcl_cout << "In brip_float_ops::fill_x_border(..) - 2xborder"
               << " exceeds image width\n";
      return;
    }
  for (int y = 0; y<height; y++)
    for (int x = 0; x<w; x++)
      image(x, y) = value;

  for (int y = 0; y<height; y++)
    for (int x = width-w; x<width; x++)
      image(x, y) = value;
}

void brip_float_ops::fill_y_border(vil_memory_image_of<float> & image,
                                   int h, float value)
{
  int width = image.width(), height = image.height();
  if (2*h>height)
    {
      vcl_cout << "In brip_float_ops::fill_y_border(..) - 2xborder"
               << " exceeds image height\n";
      return;
    }
  for (int y = 0; y<h; y++)
    for (int x = 0; x<width; x++)
      image(x, y) = value;

  for (int y = height-h; y<height; y++)
    for (int x = 0; x<width; x++)
      image(x, y) = value;
}

vil_memory_image_of<unsigned char>
brip_float_ops::convert_to_byte(vil_memory_image_of<float> const & image)
{
  //determine the max min values
  float min_val = vnl_numeric_traits<float>::maxval;
  float max_val = -min_val;
  int w = image.width(), h = image.height();
  vil_memory_image_of<unsigned char> output;
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      {
        min_val = vnl_math_min(min_val, image(x,y));
        max_val = vnl_math_max(max_val, image(x,y));
      }
  float range = max_val-min_val;
  if (!range)
    range = 1.0;
  else
    range = 255./range;
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
vil_memory_image_of<unsigned char>
brip_float_ops::convert_to_byte(vil_memory_image_of<float> const & image,
                                const float min_val, const float max_val)
{
  int w = image.width(), h = image.height();
  vil_memory_image_of<unsigned char> output;
  output.resize(w,h);
  float range = max_val-min_val;
  if (!range)
    range = 1;
  else
    range = 255.0/range;
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

vil_memory_image_of<float>
brip_float_ops::convert_to_float(vil_memory_image_of<unsigned char> const & image)
{
  vil_memory_image_of<float> output;
  int w = image.width(), h = image.height();
  output.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      output(x,y) = (float)image(x,y);
  return output;
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
           vcl_cout << t << " ";
         }
       vcl_cout << "\n";
     }
  return output;
}
