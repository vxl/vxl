#include "vipl_gaussian_convolution.h"
#include <vcl_cmath.h> // for sqrt(), exp(), log()

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();
  int size = masksize();

  // Make temporary buffer to hold result of first (horizontal) convolution
  int width = stop(X_Axis())-start(X_Axis());
  int height = stop(Y_Axis())-start(Y_Axis());
  double* buf = new double[width*height];
  if (!buf) return false; // memory allocation failed

  // 1-D mask was generated in preop(), we just use it here:
  DataIn dummy;
  // horizontal convolution:
  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j) {
    int buf_j = j - start(Y_Axis());
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      int buf_i = i - start(X_Axis(),j);
      double result = mask()[0] * fgetpixel(in, i, j, dummy);
      for (int x=1; x<size; ++x)
	result += mask()[x] * (getpixel(in, i+x, j, dummy) + getpixel(in, i-x, j, dummy));
      buf[buf_i+width*buf_j] = result;
    }
  }
  // vertical convolution:
  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j) {
    int buf_j = j - start(Y_Axis());
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      int buf_i = i - start(X_Axis(),j);
      double result = mask()[0] * buf[buf_i+width*buf_j];
      for (int y=1; y<size; ++y) {
	if (buf_j+y < height) result += mask()[y] * buf[buf_i+width*(buf_j+y)];
	if (buf_j >= y) result += mask()[y] * buf[buf_i+width*(buf_j-y)];
      }
      fsetpixel(out, i, j, result);
    }
  }
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop() {
  // create 1-D mask:
  double lc = -2 * log(cutoff()); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(sqrt(lc)*sigma()); // sigma guaranteed >= 0
  int size = radius + 1; // only need half mask, because it is symmetric
  ref_masksize() = size;
  if(mask() == 0) ref_mask() = new double[size];
  else { delete ref_mask(); ref_mask() = new double[size]; }
  double halfnorm = 0.5;
  ref_mask()[0] = 1.0;
  for (int x=1; x<=size; ++x)
    { ref_mask()[x] = exp(-0.5*x*x/sigma()/sigma()); halfnorm += mask()[x]; }
  for (int x=0; x<=size; ++x) ref_mask()[x] /= 2*halfnorm; // normalise mask
  return true;
}

// We destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gaussian_convolution <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop(){
  delete ref_mask(); ref_mask()=0;
  return true;
}
