// This is core/vil1/examples/vil1_convolve.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation "vul_arg.h"//otherwise "unresolved typeinfo vul_arg_base"
#endif

#include <vcl_cmath.h>   // vcl_fabs()
#include <vcl_iostream.h>

#include <vul/vul_arg.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_convolve_simple.h>

double mask_diff[1][3] = {
  {-1, 0, 1},
};

double mask_sobel_x[3][3] = {
  {-1, 0, 1},
  {-2, 0, 2},
  {-1, 0, 1}
};

double mask_bar_5[1][5] = {
  {1, 1, 1, 1, 1},
};

double mask_gauss_17[1][17] = {
  { 0.0000,
    0.0001,
    0.0018,
    0.0151,
    0.0876,
    0.3439,
    0.9132,
    1.6408,
    1.9947,
    1.6408,
    0.9132,
    0.3439,
    0.0876,
    0.0151,
    0.0018,
    0.0001,
    0.0000
  }
};


struct vil1_kernel_info {
  char const* name;
  int w;
  int h;
  double* mask;
};

vil1_kernel_info kernels[] = {
  {"diff_x",      3, 1, &mask_diff[0][0]},
  {"diff_y",      1, 3, &mask_diff[0][0]},
  {"sobel_x",     3, 3, &mask_sobel_x[0][0]},
  {"bar_x_5",     5, 1, &mask_bar_5[0][0]},
  {"bar_y_5",     1, 5, &mask_bar_5[0][0]},
  {"gauss_x_17", 17, 1, &mask_gauss_17[0][0]},
  {"gauss_y_17", 1, 17, &mask_gauss_17[0][0]},
  {0, 0, 0, 0}
};

int main(int argc, char ** argv)
{
  vul_arg<vcl_string> a_input_filename(0, "input");
  vul_arg<vcl_string> a_output_filename(0, "output");
  vul_arg<vcl_string> a_kernel(0, "kernel (choose from: sobel_x)", "sobel_x");
  vul_arg_parse(argc, argv);

  // Load from disk into memory "inimg"
  vil1_image in = vil1_load(a_input_filename().c_str());
  vil1_memory_image_of<unsigned char> inimg(in);

  // Build kernel in "kernelimg"
  vil1_memory_image_of<float> kernelimg(0,0);
  vcl_string kernel(a_kernel());
  for (vil1_kernel_info* kp = kernels; kp->name; ++kp)
    if (kernel == kp->name) {
      kernelimg.resize(kp->w, kp->h);
      double* v = kp->mask;
      double power = 0;
      for (int y = 0; y < kp->h; ++y)
        for (int x = 0; x < kp->w; ++x) {
          power += vcl_fabs(*v);
          kernelimg[y][x] = float(*v);
          ++v;
        }
      // Scale to unit power
      power = 1/power;
      for (int y = 0; y < kp->h; ++y)
        for (int x = 0; x < kp->w; ++x)
          kernelimg[y][x] *= float(power);
    }
  if (kernelimg.width() == 0) {
    vcl_cerr << "vil1_convolve: unknown kernel [" << kernel << "]\n";
    return -1;
  }

  // Make "outimg" -- needs to be big enough to hold the full valid convolution
  vil1_memory_image_of<unsigned char> outimg(in.width()  + kernelimg.width(),
                                             in.height() + kernelimg.height());
  outimg.fill(0);

  vil1_convolve_simple(inimg, kernelimg, (float*)0, outimg);

  vil1_save(outimg, a_output_filename().c_str(), in.file_format());
  return 0;
}

// save(crop, "t.pgm", "pnm") is implemented as:
//  (1) makes a file image, format "pnm" (i.e. a vil1_pnm_
//         dimensions,component type,size etc of "crop"
//  (2) vil1_copy(crop, fileimage)
