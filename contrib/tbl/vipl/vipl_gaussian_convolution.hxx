#ifndef vipl_gaussian_convolution_hxx_
#define vipl_gaussian_convolution_hxx_

#include <iostream>
#include <cmath>
#include "vipl_gaussian_convolution.h"
#include <vcl_compiler.h>

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = *this->out_data_ptr();
  int size = masksize();

  // Make temporary buffer to hold result of first (horizontal) convolution
  int width  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis())
             - vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis());
  int height = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->Y_Axis())
             - vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->Y_Axis());
  double* buf = new double[width*height];
  if (!buf) return false; // memory allocation failed

  // 1-D mask was generated in preop(), we just use it here:

  // horizontal convolution:
  int starty = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->Y_Axis());
  int stopy = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
  {
    int buf_j = j - starty;
    int startx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis(),j);
    int stopx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis(),j);
    for (int i = startx; i < stopx; ++i) {
      int buf_i = i - startx;
      double result = mask()[0] * fgetpixel(in, i, j, DataIn(0));
      for (int x=1; x<size; ++x)
        result += mask()[x] * (getpixel(in, i+x, j, DataIn(0)) + getpixel(in, i-x, j, DataIn(0)));
      buf[buf_i+width*buf_j] = result;
    }
  }
  // vertical convolution:
  for (int j = starty; j < stopy; ++j)
  {
    int buf_j = j - starty;
    int startx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis(),j);
    int stopx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis(),j);
    for (int i = startx; i < stopx; ++i) {
      int buf_i = i - startx;
      double result = mask()[0] * buf[buf_i+width*buf_j];
      for (int y=1; y<size; ++y) {
        if (buf_j+y < height) result += mask()[y] * buf[buf_i+width*(buf_j+y)];
        if (buf_j >= y) result += mask()[y] * buf[buf_i+width*(buf_j-y)];
      }
      fsetpixel(out, i, j, DataOut(result));
    }
  }
  delete[] buf;
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop()
{
  // create 1-D mask:
  double lc = -2 * std::log(cutoff()); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(std::sqrt(lc)*sigma()); // sigma guaranteed >= 0
  int size = radius + 1; // only need half mask, because it is symmetric
  ref_masksize() = size;
  delete[] ref_mask(); ref_mask() = new double[size];
  double s = -0.5/sigma()/sigma();
  double halfnorm = std::exp(0.25*s) + 1.0;
  for (int y=1; y<8; ++y) halfnorm += 2*std::exp(y*y*0.0625*0.0625*s);
  ref_mask()[0] = 2*halfnorm;
  for (int x=1; x<size; ++x)
  { // trapezoid approximation (16 pieces) of integral between x-0.5 and x+0.5
    ref_mask()[x] = std::exp((x-0.5)*(x-0.5)*s) + std::exp((x+0.5)*(x+0.5)*s);
    for (int y=-7; y<8; ++y) ref_mask()[x] += 2*std::exp((x+y*0.0625)*(x+y*0.0625)*s);
    halfnorm += mask()[x];
  }
  for (int x=0; x<size; ++x) ref_mask()[x] /= 2*halfnorm; // normalise mask
  return true;
}

// We destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop()
{
  delete[] ref_mask(); ref_mask()=nullptr;
  return true;
}

#endif // vipl_gaussian_convolution_hxx_
