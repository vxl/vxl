// This is core/vil/vil_pixel_traits.h
#ifndef vil_pixel_traits_h_
#define vil_pixel_traits_h_
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
// In most cases it is probably better to use vil_pixel_format.
// \author Ian Scott
// \date   27 Nov 2002
//
//-----------------------------------------------------------------------------

//: Pixel properties for templates.
template <class T>
class vil_pixel_traits;


VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<bool>
{
 public:
  //: Type of individual components
  typedef bool component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<bool const> : public vil_pixel_traits<bool> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<char>
{
 public:
  //: Type of individual components
  typedef char component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<char const> : public vil_pixel_traits<char> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned char>
{
 public:
  //: Type of individual components
  typedef unsigned char component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned char const> : public vil_pixel_traits<unsigned char> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<signed char>
{
 public:
  //: Type of individual components
  typedef signed char component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<signed char const> : public vil_pixel_traits<signed char> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<short>
{
 public:
  //: Type of individual components
  typedef short component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<short const> : public vil_pixel_traits<short> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned short>
{
 public:
  //: Type of individual components
  typedef unsigned short component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned short const> : public vil_pixel_traits<unsigned short> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<int>
{
 public:
  //: Type of individual components
  typedef int component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<int const> : public vil_pixel_traits<int> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned int>
{
 public:
  //: Type of individual components
  typedef unsigned int component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned int const> : public vil_pixel_traits<unsigned int> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<long>
{
 public:
  //: Type of individual components
  typedef long component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<long const> : public vil_pixel_traits<long > {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned long>
{
 public:
  //: Type of individual components
  typedef unsigned long component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<unsigned long const> : public vil_pixel_traits<unsigned long> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<float>
{
 public:
  //: Type of individual components
  typedef float component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<float const> : public vil_pixel_traits<float> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<double>
{
 public:
  //: Type of individual components
  typedef double component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<double const> : public vil_pixel_traits<double> {};
#endif

VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<long double>
{
 public:
  //: Type of individual components
  typedef long double component_type;
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class vil_pixel_traits<long double const> : public vil_pixel_traits<long double> {};
#endif

// Define default implementation which assumes that T is compound type which
// declares the STL-like value_type typedef.
template <class T>
class vil_pixel_traits
{
 public:
  //: Type of individual components
#if defined VCL_VC60 || !VCL_HAS_TYPENAME
  typedef vil_pixel_traits<T::value_type>::component_type component_type;
#else
  typedef typename vil_pixel_traits<typename T::value_type>::component_type component_type;
#endif
};

#endif // vil_pixel_traits_h_
