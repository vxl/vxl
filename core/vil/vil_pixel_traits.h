// This is mul/vil2/vil2_pixel_traits.h
#ifndef vil2_pixel_traits_h_
#define vil2_pixel_traits_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Templated component number/type/size
//
// To allow templated image processing algorithms to determine appropriate types, and sizes
// The basic framework was copied from AWF's vnl_numeric_traits class.
//
// In most cases it is probably better to use vil2_pixel_format.
// \author Ian Scott
// \date   27 Nov 2002
//
//-----------------------------------------------------------------------------

//: Pixel properties for templates.
template <class T>
class vil2_pixel_traits;


VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<bool>
{
 public:
  //: Type of individual components
  typedef bool component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<bool const> : public vil2_pixel_traits<bool> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<char>
{
 public:
  //: Type of individual components
  typedef bool component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<char const> : public vil2_pixel_traits<char> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned char>
{
 public:
  //: Type of individual components
  typedef unsigned char component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned char const> : public vil2_pixel_traits<unsigned char> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<signed char>
{
 public:
  //: Type of individual components
  typedef signed char component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<signed char const> : public vil2_pixel_traits<signed char> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<short>
{
 public:
  //: Type of individual components
  typedef short component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<short const> : public vil2_pixel_traits<short> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned short>
{
 public:
  //: Type of individual components
  typedef unsigned short component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned short const> : public vil2_pixel_traits<unsigned short> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<int>
{
 public:
  //: Type of individual components
  typedef int component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<int const> : public vil2_pixel_traits<int> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned int>
{
 public:
  //: Type of individual components
  typedef unsigned int component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned int const> : public vil2_pixel_traits<unsigned int> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<long>
{
 public:
  //: Type of individual components
  typedef long component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<long const> : public vil2_pixel_traits<long > {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned long>
{
 public:
  //: Type of individual components
  typedef unsigned long component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<unsigned long const> : public vil2_pixel_traits<unsigned long> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<float>
{
 public:
  //: Type of individual components
  typedef float component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<float const> : public vil2_pixel_traits<float> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<double>
{
 public:
  //: Type of individual components
  typedef double component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<double const> : public vil2_pixel_traits<double> {};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<long double>
{
 public:
  //: Type of individual components
  typedef long double component_type;
};

VCL_DEFINE_SPECIALIZATION
class vil2_pixel_traits<long double const> : public vil2_pixel_traits<long double> {};

// Define default implementation which assumes that T is compound type which
// declares the STL-like value_type typedef.
template <class T>
class vil2_pixel_traits
{
 public:
  //: Type of individual components
#if defined VCL_VC60 || !VCL_HAS_TYPENAME
  typedef vil2_pixel_traits<T::value_type>::component_type component_type;
#else
  typedef typename vil2_pixel_traits<typename T::value_type>::component_type component_type;
#endif
};

#endif // vil2_pixel_traits_h_
