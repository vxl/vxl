// This is oxl/oxp/ByteConvertImage.h
#ifndef ByteConvertImage_h_
#define ByteConvertImage_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author   Andrew W. Fitzgibbon, Oxford RRG, 22 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_byte.h>

class ByteConvertImage : public vil_memory_image_of<vil_byte>
{
  bool ignore_zero_;
  float min_;
  float max_;
  void filter(vil_memory_image_of<float> const&);
  void filter(vil_memory_image_of<double> const&);
 public:
  typedef vil_memory_image_of<vil_byte> base;

  ByteConvertImage(vil_memory_image_of<float> const& in, bool ignore_zero = false);
  ByteConvertImage(vil_memory_image_of<float> const& in, float min, float max);
  ByteConvertImage(vil_memory_image_of<double> const& in, bool ignore_zero = false);
  ByteConvertImage(vil_memory_image_of<double> const& in, float min, float max);

  void print(vcl_ostream&) const;
};

#endif // ByteConvertImage_h_
