// This is vxl/vil/vil_memory_image_of_format.txx
#ifndef vil_memory_image_of_format_h_
#define vil_memory_image_of_format_h_
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date 21 Feb 2000
// fsm: moved from vil_memory_image_of.txx to allow clients to specialize
// the traits after the standard ones in here, but before first use in
// there.
//
//-----------------------------------------------------------------------------

#include <vcl_climits.h>  // for CHAR_BIT, number of bits in a char.
#include <vcl_complex_fwd.h>

#include <vil/vil_memory_image.h>
#include <vil/vil_rgb.h>

#include <vxl_config.h>

//: Map from Types to image formats
template <class T>
struct vil_memory_image_of_format : public vil_memory_image_format
{
 protected:
  // A protection error here means you haven't derived
  // the vil_memory_image_format<T> for a vil_memory_image_of<T>
  vil_memory_image_of_format();
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<float> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(float)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<double> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(double)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vcl_complex<float> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 2;
    bits_per_component = 32;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vcl_complex<double> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 2;
    bits_per_component = 64;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<signed char> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_SIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<bool> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(bool)*CHAR_BIT; // ugh! this is 32 on gcc -- fsm.
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<unsigned char> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<unsigned short> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(short)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<signed short> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(short)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_SIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<signed int> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(int)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_SIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<unsigned int> : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 1;
    bits_per_component = sizeof(int)*CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vil_rgb<unsigned char> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 3;
    bits_per_component = CHAR_BIT;
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vil_rgb<vxl_uint_16> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 3;
    bits_per_component = 16;
    component_format = VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vil_rgb<float> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 3;
    bits_per_component = 32;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

VCL_DEFINE_SPECIALIZATION
struct vil_memory_image_of_format<vil_rgb<double> > : public vil_memory_image_format
{
  vil_memory_image_of_format()
  {
    components = 3;
    bits_per_component = 64;
    component_format = VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }
};

#endif // vil_memory_image_of_format_h_
