//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "ByteConvertImage.h"
#endif
//
// Class: ByteConvertImage
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 22 Dec 98
// Modifications:
//   981222 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "ByteConvertImage.h"

#include <vnl/vnl_math.h>
#include <vbl/vbl_printf.h>
#include <vil/vil_clamp.h>

ByteConvertImage::ByteConvertImage(vil_memory_image_of<float> const& in, bool ignore_zero):
  base(in.width(), in.height())
{
  ignore_zero_ = ignore_zero;
  min_ = (float)HUGE_VAL;
  max_ = -min_;
  for(int y = 0; y < in.height(); ++y)
    for(int x = 0; x < in.width(); ++x) {
      float v = in(x,y);
      if (ignore_zero && v == 0.0F)
	continue;

      if (v < min_) min_ = v;
      if (v > max_) max_ = v;
    }

  filter(in);
}

ByteConvertImage::ByteConvertImage(vil_memory_image_of<float> const& in, float min, float max):
  base(in.width(), in.height())
{
  //vil_memory_image_of<vil_byte>& outbuf = *this;

  ignore_zero_ = false;
  min_ = min;
  max_ = max;
  
  filter(in);
}

ByteConvertImage::ByteConvertImage(vil_memory_image_of<double> const& in, bool ignore_zero):
  base(in.width(), in.height())
{
  ignore_zero_ = ignore_zero;
  min_ = (float)HUGE_VAL;
  max_ = -min_;
  for(int y = 0; y < in.height(); ++y)
    for(int x = 0; x < in.width(); ++x) {
      double v = in(x,y);
      if (ignore_zero && v == 0.0F)
	continue;

      if ((float)v < min_) min_ = (float)v;
      if ((float)v > max_) max_ = (float)v;
    }

  filter(in);
}

ByteConvertImage::ByteConvertImage(vil_memory_image_of<double> const& in, double min, double max):
  base(in.width(), in.height())
{
  //vil_memory_image_of<vil_byte>& outbuf = *this;

  ignore_zero_ = false;
  min_ = min;
  max_ = max;
  
  filter(in);
}

void ByteConvertImage::filter(vil_memory_image_of<float> const& in)
{
  float scale = 255 / (max_ - min_);
  for(int y = 0; y < in.height(); ++y)
    for(int x = 0; x < in.width(); ++x) {
      float v = in(x,y);
      if (ignore_zero_ && v == 0.0F)
	(*this)(x,y) = 0;
      else
	(*this)(x,y) = vil_clamp((v - min_) * scale, (vil_byte *)0);
    }
}

void ByteConvertImage::filter(vil_memory_image_of<double> const& in)
{
  double dmin = (double)min_;
  double scale = 255 / (max_ - min_);
  for(int y = 0; y < in.height(); ++y)
    for(int x = 0; x < in.width(); ++x) {
      double v = in(x,y);
      if (ignore_zero_ && v == 0.0F)
	(*this)(x,y) = 0;
      else
	(*this)(x,y) = vil_clamp((v - dmin) * scale, (vil_byte *)0);
    }
}
 
void ByteConvertImage::print(ostream& s) const
{
  vbl_printf(s, "ByteConvertImage: range %g..%g\n", min_, max_);
}
