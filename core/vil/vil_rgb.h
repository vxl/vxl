// This is core/vil/vil_rgb.h
#ifndef vil_rgb_h_
#define vil_rgb_h_
//:
// \file
// \brief Pixel type for 24 bit images
//
//  Currently also includes the following `utilities':
//   - conversion to ubyte (luminance of vil_rgb: weights (0.299,0.587,0.114)).
//   - min and max of vil_rgbcell values, useful for morphological operations.
//   - arithmetic operations
//
// \author Peter Vanroose, K.U.Leuven, ESAT/VISICS
// \date   15 nov. 1997
//
//\verbatim
//  Modifications:
//    250198 AWF Templated.
//    250198 AWF Modified to make POD struct until gcc inlines when debugging.
//    160298 PCP Removed underscore from public members.
//    290798 AWF Member templates for fancy compilers
//    220598 PVr moved instantiations files to Templates subdirectory.
//    050598 PVr added several operators ( + += - -= (T) ).
//    140898 David Capel added clamping functions to ensure 0-255 range on bytes and vil_rgb<byte>
//    090600 David Capel made clamping functions inline and removed all that partial specialization nonsense from the
//    .hxx file.
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//\endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: This is the appropriate pixel type for 24-bit colour images.
//
//    Currently also includes the following `utilities':
//    -  conversion to ubyte (luminance of vil_rgb: weights (0.299,0.587,0.114)).
//    -  min and max of vil_rgbcell values, useful for morphological operations.
//    -  arithmetic operations
template <class T>
struct vil_rgb
{
  typedef T value_type;

  inline vil_rgb() = default;

  //: Create grey (v,v,v) vil_rgb cell from value v.
  // This provides a conversion from T to vil_rgb<T>

  inline vil_rgb(T v)
    : r(v)
    , g(v)
    , b(v)
  {}

  //: Construct a vil_rgb value.
  inline vil_rgb(T red, T green, T blue)
    : r(red)
    , g(green)
    , b(blue)
  {}

  // The rgb values
  T r, g, b;
  inline T
  R() const
  {
    return r;
  }
  inline T
  G() const
  {
    return g;
  }
  inline T
  B() const
  {
    return b;
  }

  //: Convert vil_rgb to gray using standard (.299, .587, .114) weighting.
  inline T
  grey() const
  {
    return T(r * 0.299 + 0.587 * g + 0.114 * b);
  }

#if 0 // deprecated -- use .grey() instead
  inline operator T() const { return T(0.5+r*0.299+0.587*g+0.114*b); }
#endif

  //: equality
  inline bool
  operator==(const vil_rgb<T> & o) const
  {
    return r == o.r && g == o.g && b == o.b;
  }

  // operators
  inline vil_rgb<T>
  operator+(const vil_rgb<T> & A) const
  {
    return vil_rgb<T>(r + A.r, g + A.g, b + A.b);
  }
  inline vil_rgb<T>
  operator-(const vil_rgb<T> & A) const
  {
    return vil_rgb<T>(r - A.r, g - A.g, b - A.b);
  }
  inline vil_rgb<T>
  operator/(const vil_rgb<T> & A) const
  {
    return vil_rgb<T>(r / A.r, g / A.g, b / A.b);
  }
  inline vil_rgb<T> &
  operator+=(const vil_rgb<T> & A)
  {
    r += A.r, g += A.g, b += A.b;
    return *this;
  }
  inline vil_rgb<T> &
  operator-=(const vil_rgb<T> & A)
  {
    r -= A.r, g -= A.g, b -= A.b;
    return *this;
  }
  inline vil_rgb<T>
  operator*(T A) const
  {
    return vil_rgb<T>(r * A, g * A, b * A);
  }
  inline vil_rgb<T>
  operator/(T A) const
  {
    return vil_rgb<T>(r / A, g / A, b / A);
  }
  inline vil_rgb<T> &
  operator*=(T A)
  {
    r *= A, g *= A, b *= A;
    return *this;
  }
  inline vil_rgb<T> &
  operator/=(T A)
  {
    r /= A, g /= A, b /= A;
    return *this;
  }

  template <class S>
  inline vil_rgb(const vil_rgb<S> & that)
    : r(T(that.r))
    , g(T(that.g))
    , b(T(that.b))
  {}
  template <class S>
  inline vil_rgb<T> &
  operator=(const vil_rgb<S> & that)
  {
    r = T(that.r);
    g = T(that.g);
    b = T(that.b);
    return *this;
  }
};

template <class T>
inline std::ostream &
operator<<(std::ostream & s, const vil_rgb<T> & rgb)
{
  return s << '[' << rgb.r << ' ' << rgb.g << ' ' << rgb.b << ']';
}

template <>
std::ostream &
operator<<(std::ostream & s, const vil_rgb<unsigned char> & rgb);


// ** Arithmetic operators

template <class T>
inline bool
operator!=(const vil_rgb<T> & a, const vil_rgb<T> & b)
{
  return !(a == b);
}

template <class T>
inline vil_rgb<T>
average(const vil_rgb<T> & a, const vil_rgb<T> & b)
{
  return vil_rgb<T>((a.r + b.r) / 2, (a.g + b.g) / 2, (a.b + b.b) / 2);
}

template <class T>
inline vil_rgb<T>
operator+(const vil_rgb<T> & a, const vil_rgb<T> & b)
{
  return vil_rgb<T>(a.r + b.r, a.g + b.g, a.b + b.b);
}

template <class T>
inline vil_rgb<T>
operator*(const vil_rgb<T> & a, const vil_rgb<T> & b)
{
  return vil_rgb<T>(a.r * b.r, a.g * b.g, a.b * b.b);
}

template <class T>
inline vil_rgb<double>
operator*(double b, const vil_rgb<T> & a)
{
  return vil_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline vil_rgb<double>
operator*(const vil_rgb<T> & a, double b)
{
  return vil_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline vil_rgb<double>
operator/(const vil_rgb<T> & a, double b)
{
  return vil_rgb<double>(a.r / b, a.g / b, a.b / b);
}
#if 0
template<>
inline
vil_rgb<double> operator*(vil_rgb<bool> const&a, double b)
{
    return vil_rgb<double>((double)a.r * b, double(a.g) * b, double(a.b) * b);
}
#endif
template <>
inline vil_rgb<double>
operator/(vil_rgb<bool> const & a, double b)
{
  return { ((double)a.r) / b, ((double)a.g) / b, ((double)a.b) / b };
}

// Template Specialization Bool Operators
template <>
inline vil_rgb<bool>
vil_rgb<bool>::operator+(const vil_rgb<bool> & A) const
{
  return { r || A.r, g || A.g, b || A.b };
}
// vil_rgb <bool> vil_rgb<bool>::operator+ (vil_rgb<bool> const& A) const { return vil_rgb<bool>(r ^ A.r, g ^ A.g, b ^
// A.b); }
template <>
inline vil_rgb<bool>
vil_rgb<bool>::operator-(const vil_rgb<bool> & A) const
{
  return { r || !A.r, g || !A.g, b || !A.b };
}
template <>
inline vil_rgb<bool>
vil_rgb<bool>::operator/(const vil_rgb<bool> & A) const
{
  return { r && !A.r, g && !A.g, b && !A.b };
}
template <>
inline vil_rgb<bool> &
vil_rgb<bool>::operator+=(const vil_rgb<bool> & A)
{
  r = r || A.r, g = g || A.g, b = b || A.b;
  return *this;
}
template <>
inline vil_rgb<bool> &
vil_rgb<bool>::operator-=(const vil_rgb<bool> & A)
{
  r = r || !A.r, g = g || !A.g, b = b || !A.b;
  return *this;
}
template <>
inline vil_rgb<bool>
vil_rgb<bool>::operator*(bool A) const
{
  return { r && A, g && A, b && A };
}
template <>
inline vil_rgb<bool>
vil_rgb<bool>::operator/(bool A) const
{
  return { r && !A, g && !A, b && !A };
}
template <>
inline vil_rgb<bool> &
vil_rgb<bool>::operator*=(bool A)
{
  r = r && A, g = g && A, b = b && A;
  return *this;
}
#if 1
template <>
inline vil_rgb<bool> &
vil_rgb<bool>::operator/=(bool A)
{
  r = r && !A, g = g && !A, b = b && !A;
  return *this;
}
#endif

#define VIL_RGB_INSTANTIATE(T) extern "you must include vil/vil_rgb.hxx first."
#define VIL_RGB_INSTANTIATE_LS(T) extern "you must include vil/vil_rgb.hxx first."

#endif // vil_rgb_h_
