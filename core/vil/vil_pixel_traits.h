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
// \verbatim

//  Modifications J.L. Mundy Dec. 2004, Added traits to support range mapping.
//    is_signed   -  does the type have negative values
//    num_bits    -  number of bits required to encode the value range
//    real_number_field - is the type an approximation to the field of reals
//  (Functions used rather than static constants for minimal linking issues)
// \endverbatim
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

  //: Is signed
  static bool is_signed() {return false;}

  //: Size in bits
  static unsigned num_bits() {return 1;}

  //: Minimum value 
  static bool minval() {return false;}

  //: Maximum value 
  static bool maxval() {return true;}

  //: Real number field
  static bool real_number_field() {return false;}
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

  //: Is signed (char is not guaranteed to be unsigned)
  static bool is_signed() {return (char)255<0;}

  //: Size in bits
  static unsigned num_bits() {return 8;}

  //: Minimum value 
  static char minval() {return char(255)<0?-128:0;}

  //: Maximum value 
  static char maxval() {return char(255)<0?127:255;}

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return false;}

  //: Size in bits
  static unsigned num_bits() {return 8;}

  //: Minimum value 
  static unsigned char minval() {return 0;}

  //: Maximum value 
  static unsigned char maxval() {return 255;}

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8;}

  //: Minimum value 
  static signed char minval() {return -128;}

  //: Maximum value 
  static signed char maxval() {return 127;}

  //: Real number field
  static bool real_number_field() {return false;}
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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(short);}

  //: Minimum value 
  static short minval() {return -(1 << (num_bits()-1));}

  //: Maximum value 
  static short maxval() {return (1 << (num_bits()-1)) - 1;}

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return false;}

  //: Size in bits
  static unsigned num_bits() {return 16;}

  //: Minimum value 
  static unsigned short minval() {return 0;}

  //: Maximum value 
  static unsigned short maxval() {return (1 << num_bits())-1;}

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(int);}

  //: Minimum value 
  static int minval() {return -(1 << (num_bits()-1));}

  //: Maximum value 
  static int maxval() {return (1 << (num_bits()-1))- 1;}

  //: Real number field
  static bool real_number_field() {return false;}
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

  //: Is signed
  static bool is_signed() {return false;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(unsigned int);}

  //: Minimum value 
  static unsigned int minval() {return 0;}

  //: Maximum value 
  static unsigned int maxval() {return  4294967295;}//only for 32 bit machines

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(long);}

  //: Minimum value 
  static long minval() {return -(1 << (num_bits()-1));}

  //: Maximum value 
  static long maxval() {return (1 << (num_bits()-1))-1;}

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return false;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(unsigned long);}

  //: Minimum value 
  static unsigned long minval() {return 0;}

  //: Maximum value 
  static unsigned long maxval() {return 4294967295;}//only for 32 bit machines

  //: Real number field
  static bool real_number_field() {return false;}

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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(float);}

  //: Minimum value 
  static float minval() {return -3.40282346638528860e+38F;}

  //: Maximum value 
  static float maxval() {return 3.40282346638528860e+38F;}

  //: Real number field
  static bool real_number_field() {return true;}
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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(double);}

  //: Minmum value 
  static double minval() {return -1.7976931348623157E+308;}

  //: Maximum value 
  static double maxval() {return 1.7976931348623157E+308;}

  //: Real number field
  static bool real_number_field() {return true;}
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

  //: Is signed
  static bool is_signed() {return true;}

  //: Size in bits
  static unsigned num_bits() {return 8*sizeof(long double);}

  //: Minimum value 
  static long double minval() {return -1.7976931348623157E+308;}

  //: Maximum value 
  static long double maxval() {return 1.7976931348623157E+308;}

  //: Real number field
  static bool real_number_field() {return true;}
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
