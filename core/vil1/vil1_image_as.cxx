// This is core/vil1/vil1_image_as.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <vector>
#include <string>
#include "vil1_image_as.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vil1/vil1_pixel.h"
#include "vil1/vil1_memory_image_of_format.hxx"

#include "vxl_config.h"


template <class T>
struct vil1_image_as_impl
  : public vil1_image_impl
  , public vil1_memory_image_of_format<T>
{
  using format = vil1_memory_image_of_format<T>;
  vil1_image image;
  vil1_image_as_impl(vil1_image const & underlying)
    : image(underlying)
  {}
  int
  planes() const override
  {
    return 1;
  }
  int
  width() const override
  {
    return image.width();
  }
  int
  height() const override
  {
    return image.height();
  }
  int
  components() const override
  {
    return format::components;
  }
  int
  bits_per_component() const override
  {
    return format::bits_per_component;
  }
  vil1_component_format
  component_format() const override
  {
    return format::component_format;
  }
  vil1_image get_plane(unsigned int p) const override {
    assert(p == 0);
    return const_cast<vil1_image_as_impl *>(this);
  }

#if 0
  char const* file_format() const { return 0; }
  bool get_property(char const* tag, void* property_value_out = 0) const { return false; }
#endif

  // There is no default implementation of this method. It must be
  // specialized by hand for each T.
  bool
  get_section(void * buf, int x0, int y0, int width, int height) const override;

  // This always fails, even if the underlying image is an image-of-T
  bool
  put_section(void const *, int, int, int, int) override
  {
    return false;
  }

  //: Return the name of the class;
  std::string
  is_a() const override;

  //: Return true if the name of the class matches the argument
  bool
  is_class(std::string const &) const override;
};

//--------------------------------------------------------------------------------

template <class Inp, class Out>
bool
convert_grey_to_grey(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
      static_cast<Out *>(buf)[i + width * j] = Out(scan[i]);
  }
  return true;
}


template <class Inp, class Out>
bool
convert_rgb_to_grey(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(3 * width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
    {
      Inp r(scan[3 * i + 0]);
      Inp g(scan[3 * i + 1]);
      Inp b(scan[3 * i + 2]);
      // Weights convert from linear RGB to CIE luminance assuming a
      // modern monitor.  See Charles Poynton's Colour FAQ
      // http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
      static_cast<Out *>(buf)[i + width * j] = Out(0.2125 * r + 0.7154 * g + 0.072 * b);
      // This are the old NTSC weights.
      // static_cast<Out*>(buf)[i + width*j] = Out(0.5+r*0.299+0.587*g+0.114*b);
    }
  }
  return true;
}


template <class Inp, class Out>
bool
convert_grey_to_rgb(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
    {
      static_cast<Out *>(buf)[3 * (i + width * j) + 0] = Out(scan[i]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 1] = Out(scan[i]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 2] = Out(scan[i]);
    }
  }
  return true;
}


template <class Inp, class Out>
bool
convert_rgb_to_rgb(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(3 * width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
    {
      static_cast<Out *>(buf)[3 * (i + width * j) + 0] = Out(scan[3 * i + 0]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 1] = Out(scan[3 * i + 1]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 2] = Out(scan[3 * i + 2]);
    }
  }
  return true;
}

template <class Inp, class Out>
bool
convert_rgba_to_rgb(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(4 * width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
    {
      static_cast<Out *>(buf)[3 * (i + width * j) + 0] = Out(scan[4 * i + 0]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 1] = Out(scan[4 * i + 1]);
      static_cast<Out *>(buf)[3 * (i + width * j) + 2] = Out(scan[4 * i + 2]);
    }
  }
  return true;
}


template <class Inp, class Out>
bool
convert_rgba_to_grey(const vil1_image & image, void * buf, int x0, int y0, int width, int height, Inp *, Out *)
{
  std::vector<Inp> scan(4 * width);
  for (int j = 0; j < height; ++j)
  {
    if (!image.get_section(/* xxx */ &scan[0], x0, y0 + j, width, 1))
      return false;
    for (int i = 0; i < width; ++i)
    {
      Inp r(scan[4 * i + 0]);
      Inp g(scan[4 * i + 1]);
      Inp b(scan[4 * i + 2]);
      // Weights convert from linear RGB to CIE luminance assuming a
      // modern monitor.  See Charles Poynton's Colour FAQ
      // http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
      static_cast<Out *>(buf)[i + width * j] = Out(0.2125 * r + 0.7154 * g + 0.072 * b);
      // This are the old NTSC weights.
      // static_cast<Out*>(buf)[i + width*j] = Out(0.5+r*0.299+0.587*g+0.114*b);
    }
  }
  return true;
}


// Explicitly instantiate the ones we use

// template bool convert_grey_to_grey( const vil1_image&, void*, int, int, int, int, vxl_byte*,vxl_byte*);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, vxl_byte *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, vxl_byte *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, float *, vxl_byte *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, double *, vxl_byte *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_byte *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, vxl_byte *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, float *, vxl_byte *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, double *, vxl_byte *);
template bool
convert_rgba_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_byte *);

template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_uint_16 *);
// template bool convert_grey_to_grey( const vil1_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, vxl_uint_16 *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, float *, vxl_uint_16 *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, double *, vxl_uint_16 *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_uint_16 *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, vxl_uint_16 *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, float *, vxl_uint_16 *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, double *, vxl_uint_16 *);

template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, int *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, int *);
// template bool convert_grey_to_grey( const vil1_image&, void*, int, int, int, int, vxl_uint_32*,int*);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, float *, int *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, double *, int *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, int *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, int *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, float *, int *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, double *, int *);

template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, float *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, float *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, float *);
// template bool convert_grey_to_grey( const vil1_image&, void*, int, int, int, int, float*,float*);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, double *, float *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, float *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, float *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, float *, float *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, double *, float *);

template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, double *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, double *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, double *);
template bool
convert_grey_to_grey(const vil1_image &, void *, int, int, int, int, float *, double *);
// template bool convert_grey_to_grey( const vil1_image&, void*, int, int, int, int, double*,double*);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_byte *, double *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, double *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, float *, double *);
template bool
convert_rgb_to_grey(const vil1_image &, void *, int, int, int, int, double *, double *);

template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, unsigned char *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, unsigned char *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, unsigned char *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, float *, unsigned char *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, double *, unsigned char *);
// template bool convert_rgb_to_rgb( const vil1_image&, void*, int, int, int, int, vxl_byte*,unsigned char* );
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, unsigned char *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, float *, unsigned char *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, double *, unsigned char *);
template bool
convert_rgba_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, unsigned char *);

template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_uint_16 *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, vxl_uint_16 *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, vxl_uint_16 *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, float *, vxl_uint_16 *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, double *, vxl_uint_16 *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_uint_16 *);
// template bool convert_rgb_to_rgb( const vil1_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, float *, vxl_uint_16 *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, double *, vxl_uint_16 *);
template bool
convert_rgba_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, vxl_uint_16 *);

template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, float *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, float *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_32 *, float *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, float *, float *);
template bool
convert_grey_to_rgb(const vil1_image &, void *, int, int, int, int, double *, float *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, float *);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_uint_16 *, float *);
// template bool convert_rgb_to_rgb( const vil1_image&, void*, int, int, int, int, float*,float*);
template bool
convert_rgb_to_rgb(const vil1_image &, void *, int, int, int, int, double *, float *);
template bool
convert_rgba_to_rgb(const vil1_image &, void *, int, int, int, int, vxl_byte *, float *);


//--------------------------------------------------------------------------------

template <> // specialize for byte.
bool
vil1_image_as_impl<vxl_byte>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = vxl_byte;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_UINT16:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGBA_BYTE:
      return convert_rgba_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<vxl_byte>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<vxl_byte>";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<vxl_byte>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<vxl_byte>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for byte.
template struct vil1_image_as_impl<vxl_byte>;

vil1_image
vil1_image_as_byte(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<vxl_byte>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, vxl_byte *)
{
  return vil1_image(new vil1_image_as_impl<vxl_byte>(image));
}

//--------------------------------------------------------------------------------

template <> // specialize for vxl_uint_16.
bool
vil1_image_as_impl<vxl_uint_16>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = vxl_uint_16;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_UINT32:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<vxl_uint_16>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<vxl_uint_16>";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<vxl_uint_16>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<vxl_uint_16>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for vxl_uint_16
template struct vil1_image_as_impl<vxl_uint_16>;

vil1_image
vil1_image_as_uint16(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<vxl_uint_16>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, vxl_uint_16 *)
{
  return vil1_image(new vil1_image_as_impl<vxl_uint_16>(image));
}


//--------------------------------------------------------------------------------

template <> // specialize for int.
bool
vil1_image_as_impl<int>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = int;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_FLOAT:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<int>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<int>";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<int>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<int>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for int.
template struct vil1_image_as_impl<int>;

vil1_image
vil1_image_as_int(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<int>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, int *)
{
  return vil1_image(new vil1_image_as_impl<int>(image));
}

//--------------------------------------------------------------------------------

template <> // specialize for float.
bool
vil1_image_as_impl<float>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = float;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_DOUBLE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<float>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<float>";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<float>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<float>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for float.
template struct vil1_image_as_impl<float>;

vil1_image
vil1_image_as_float(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<float>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, float *)
{
  return vil1_image(new vil1_image_as_impl<float>(image));
}

//--------------------------------------------------------------------------------

template <> // specialize for double.
bool
vil1_image_as_impl<double>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = double;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_grey(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<double>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<double>";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<double>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<double>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for double.
template struct vil1_image_as_impl<double>;

vil1_image
vil1_image_as_double(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<double>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, double *)
{
  return vil1_image(new vil1_image_as_impl<double>(image));
}

//--------------------------------------------------------------------------------

template <> // specialize for rgb.
bool
vil1_image_as_impl<vil1_rgb<unsigned char>>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = unsigned char;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGBA_BYTE:
      return convert_rgba_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<vil1_rgb<unsigned char>>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<vil1_rgb<unsigned char> >";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<vil1_rgb<unsigned char>>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<vil1_rgb<unsigned char>>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for vil1_rgb_byte .
template struct vil1_image_as_impl<vil1_rgb<unsigned char>>;

vil1_image
vil1_image_as_rgb_byte(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<unsigned char>>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, vil1_rgb<vxl_byte> *)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<vxl_byte>>(image));
}

//--------------------------------------------------------------------------------

template <> // specialize for rgb float.
bool
vil1_image_as_impl<vil1_rgb<float>>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = float;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_FLOAT:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGBA_BYTE:
      return convert_rgba_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}

template <>
std::string
vil1_image_as_impl<vil1_rgb<float>>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<vil1_rgb<float> >";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<vil1_rgb<float>>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<vil1_rgb<float>>::is_a() || vil1_image_impl::is_class(s);
}

// instantiate for vil1_rgb_float .
template struct vil1_image_as_impl<vil1_rgb<float>>;

vil1_image
vil1_image_as_rgb_float(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<float>>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, vil1_rgb<float> *)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<float>>(image));
}


//--------------------------------------------------------------------------------

template <> // specialize for rgb 16-bit.
bool
vil1_image_as_impl<vil1_rgb<vxl_uint_16>>::get_section(void * buf, int x0, int y0, int width, int height) const
{
  using Outtype = vxl_uint_16;

  switch (vil1_pixel_format(image))
  {
    case VIL1_BYTE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT16:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_16 *)nullptr, (Outtype *)nullptr);
    case VIL1_UINT32:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (vxl_uint_32 *)nullptr, (Outtype *)nullptr);
    case VIL1_FLOAT:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_DOUBLE:
      return convert_grey_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_BYTE:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_UINT16:
      return image.get_section(buf, x0, y0, width, height);
    case VIL1_RGB_FLOAT:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (float *)nullptr, (Outtype *)nullptr);
    case VIL1_RGB_DOUBLE:
      return convert_rgb_to_rgb(image, buf, x0, y0, width, height, (double *)nullptr, (Outtype *)nullptr);
    case VIL1_RGBA_BYTE:
      return convert_rgba_to_rgb(image, buf, x0, y0, width, height, (vxl_byte *)nullptr, (Outtype *)nullptr);
    default:
      std::cerr << __FILE__ ": get_section() not implemented for " << image << std::endl;
      assert(false /* implement for your image type as needed */);
      return false;
  }
}


template <>
std::string
vil1_image_as_impl<vil1_rgb<vxl_uint_16>>::is_a() const
{
  static const std::string class_name_ = "vil1_image_as_impl<vil1_rgb<vxl_uint_16> >";
  return class_name_;
}

template <>
bool
vil1_image_as_impl<vil1_rgb<vxl_uint_16>>::is_class(std::string const & s) const
{
  return s == vil1_image_as_impl<vil1_rgb<vxl_uint_16>>::is_a() || vil1_image_impl::is_class(s);
}


// instantiate for vil1_rgb_uint16 .
template struct vil1_image_as_impl<vil1_rgb<vxl_uint_16>>;

vil1_image
vil1_image_as_rgb_uint16(vil1_image const & image)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<vxl_uint_16>>(image));
}

template <>
vil1_image
vil1_image_as(vil1_image const & image, vil1_rgb<vxl_uint_16> *)
{
  return vil1_image(new vil1_image_as_impl<vil1_rgb<vxl_uint_16>>(image));
}
