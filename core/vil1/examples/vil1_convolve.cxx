// Example: convolution

#include <vcl/vcl_cstring.h>
#include <vcl/vcl_cmath.h>   // fabs()
#include <vcl/vcl_iostream.h>

#include <vbl/vbl_arg.h>

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convolve.h>

typedef unsigned char byte;

short mask_diff[1][3] = {
  {-1, 0, 1},
};

short mask_sobel_x[3][3] = {
  {-1, 0, 1},
  {-2, 0, 2},
  {-1, 0, 1}
};

short mask_sobel_y[3][3] = {
  {-1, -2, -1},
  { 0,  0,  0},
  { 1,  2,  1}
};

byte mask_bar_5[1][5] = {
  {1, 1, 1, 1, 1},
};

double mask_gauss_15[1][15] = {
  {
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
    0.0001
  }
};


struct vil_kernel_info {
  char const* name;
  int w;
  int h;
  void* mask;
};

vil_kernel_info float_kernels[] = {
  {"gauss_x_15", 15, 1, &mask_gauss_15[0][0]},
  {"gauss_y_15", 1, 15, &mask_gauss_15[0][0]},
  {"gauss_15x15", 15, 15, 0},
  {0, 0}
};

vil_kernel_info short_kernels[] = {
  {"diff_x",      3, 1, &mask_diff[0][0]},
  {"diff_y",      1, 3, &mask_diff[0][0]},
  {"grad",        3, 3, 0},
  {"sobel_x",     3, 3, &mask_sobel_x[0][0]},
  {"sobel_y",     3, 3, &mask_sobel_y[0][0]},
  {"sobel_xy",    3, 3, 0},
  {0, 0}
};

vil_kernel_info byte_kernels[] = {
  {"bar_x_5",     5, 1, &mask_bar_5[0][0]},
  {"bar_y_5",     1, 5, &mask_bar_5[0][0]},
  {"mean_5",      5, 5, 0},
  {0, 0}
};

int main(int argc, char ** argv)
{
  vbl_arg<char*> a_input_filename(0, "input file (8-bit grey)");
  vbl_arg<char*> a_output_filename(0, "output PGM file");
  vbl_arg<char*> a_kernel(0, "kernel (choose from: sobel_x sobel_y sobel_xy diff_x diff_y grad bar_x_5 bar_y_5 mean_5 gauss_x_15 gauss_y_15 gauss_15x15)", "sobel_x");
  vbl_arg_parse(argc, argv);

  // Load from disk into memory "inimg"
  vil_image in = vil_load(a_input_filename());
  vil_memory_image_of<byte> inimg(in);
  
  // Build kernel in "kernelimg_*"
  vil_memory_image_of<float> kernelimg_float(0,0);
  vil_memory_image_of<short> kernelimg_short(0,0);
  vil_memory_image_of<byte > kernelimg_byte(0,0);
  vcl_string kernel(a_kernel());
  for(vil_kernel_info* kp = float_kernels; kp->name; ++kp)
    if (kernel == kp->name) {
      kernelimg_float.resize(kp->w, kp->h);
      double* v = (double*)(kp->mask);
      double power = 0;
      if (v)
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x) {
	    power += fabs(*v);
	    kernelimg_float[y][x] = *v;
	    ++v;
	  }
      else if (kernel == "gauss_15x15") {
	v = &mask_gauss_15[0][0];
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x) {
	    power += fabs(v[x]*v[y]);
	    kernelimg_float[y][x] = v[x]*v[y];
	  }
      }
      // Scale to unit power
      power = 1.0/power;
      for(int y = 0; y < kp->h; ++y)
	for(int x = 0; x < kp->w; ++x)
	  kernelimg_float[y][x] *= power;
    }
  for(vil_kernel_info* kp = short_kernels; kp->name; ++kp)
    if (kernel == kp->name) {
      kernelimg_short.resize(kp->w, kp->h);
      short* v = (short*)(kp->mask);
      if (v)
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x) {
	    kernelimg_short[y][x] = *v;
	    ++v;
	  }
      else if (kernel == "grad") {
	v = &mask_diff[0][0];
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x)
	    kernelimg_short[y][x] = v[x]*v[y];
      }
      else if (kernel == "sobel_xy") {
	v = &mask_sobel_x[0][0];
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x,++v)
	    kernelimg_short[y][x] = *v;
	v = &mask_sobel_y[0][0];
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x,++v)
	    kernelimg_short[y][x] = *v;
      }
    }
  for(vil_kernel_info* kp = byte_kernels; kp->name; ++kp)
    if (kernel == kp->name) {
      kernelimg_byte.resize(kp->w, kp->h);
      byte* v = (byte*)(kp->mask);
      if (v)
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x) {
	    kernelimg_byte[y][x] = *v;
	    ++v;
	  }
      else if (kernel == "mean_5") {
	v = &mask_bar_5[0][0];
        for(int y = 0; y < kp->h; ++y)
	  for(int x = 0; x < kp->w; ++x)
	    kernelimg_byte[y][x] = v[x]*v[y];
      }
    }

  // Make "outimg" -- needs to be big enough to hold the full valid convolution
  if (kernelimg_float.width() > 0) {
    int w = in.width() + kernelimg_float.width() - 1,
        h = in.height()+ kernelimg_float.height()- 1;
    vil_memory_image_of<byte> outimg(w, h, 0);
    vil_convolve_simple(inimg, kernelimg_float, outimg, (float*)0);
    vil_save(outimg, a_output_filename(), in.file_format());
  }
  else if (kernelimg_short.width() > 0) {
    int w = in.width() + kernelimg_short.width() - 1,
        h = in.height()+ kernelimg_short.height()- 1;
    vil_memory_image_of<short> tmpimg(w, h, 0);
    vil_convolve_simple(inimg, kernelimg_short, tmpimg, (int*)0);
    int maxv = -(1<<17), minv = (1<<17);
    for(int y = 0; y < h; ++y)
      for(int x = 0; x < w; ++x) {
        if (maxv < tmpimg[y][x]) maxv = tmpimg[y][x];
        if (minv > tmpimg[y][x]) minv = tmpimg[y][x];
      }
    double sc = 256.0 / (maxv-minv+1);
    vil_memory_image_of<byte> outimg(w, h, 0);
    for(int y = 0; y < h; ++y)
      for(int x = 0; x < w; ++x)
        outimg[y][x] = (byte)(0.5 + sc*(tmpimg[y][x]-minv));
    vil_save(outimg, a_output_filename(), in.file_format());
  }
  else if (kernelimg_byte.width() > 0) {
    int w = in.width() + kernelimg_byte.width() - 1,
        h = in.height()+ kernelimg_byte.height()- 1;
    vil_memory_image_of<int> tmpimg(w, h, 0);
    vil_convolve_simple(inimg, kernelimg_byte, tmpimg, (int*)0);
    int maxv = -(1<<17), minv = (1<<17);
    for(int y = 0; y < h; ++y)
      for(int x = 0; x < w; ++x) {
        if (maxv < tmpimg[y][x]) maxv = tmpimg[y][x];
        if (minv > tmpimg[y][x]) minv = tmpimg[y][x];
      }
    double sc = 256.0 / (maxv-minv+1);
    vil_memory_image_of<byte> outimg(w, h, 0);
    for(int y = 0; y < h; ++y)
      for(int x = 0; x < w; ++x)
        outimg[y][x] = (byte)(0.5 + sc*(tmpimg[y][x]-minv));
    vil_save(outimg, a_output_filename(), in.file_format());
  }
  else {
    cerr << "vil_convolve: unknown kernel [" << kernel << "]\n";
    return -1;
  }

}

// save(crop, "t.pgm", "pnm") is implemented as:
//  (1) makes a file image, format "pnm" (i.e. a vil_pnm_
//         dimensions,component type,size etc of "crop"
//  (2) vil_copy(crop, fileimage)
