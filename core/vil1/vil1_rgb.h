// This is core/vil1/vil1_rgb.h
#ifndef vil1_rgb_h_
#define vil1_rgb_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Pixel type for 24 bit images
//
//  Currently also includes the following `utilities':
//   - conversion to ubyte (luminance of vil1_rgb: weights (0.299,0.587,0.114)).
//   - min and max of vil1_rgbcell values, useful for morphological operations.
//   - arithmetic operations
//
// \author Peter Vanroose, K.U.Leuven, ESAT/VISICS
// \date   15 nov. 1997
//
// \verbatim
//  Modifications
//   250198 AWF Templated.
//   250198 AWF Modified to make POD struct until gcc inlines when debugging.
//   160298 PCP Removed underscore from public members.
//   220598 PVr moved instantiations files to Templates subdirectory.
//   050598 PVr added several operators ( + += - -= (T) ).
//   290798 AWF Member templates for fancy compilers
//   140898 David Capel added clamping functions to ensure 0-255 range on bytes and vil1_rgb<byte>
//   090600 David Capel made clamping functions inline and removed all that partial specialization nonsense from the .txx file.
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//\endverbatim

#include <vcl_iostream.h>
#include <vil1/vil1_clamp.h>

#ifdef VCL_SUNPRO_CC
# define InLine inline
#else
# define InLine
#endif

//: This is the appropriate pixel type for 24-bit colour images.
//
//    Currently also includes the following `utilities':
//    -  conversion to ubyte (luminance of vil1_rgb: weights (0.299,0.587,0.114)).
//    -  min and max of vil1_rgbcell values, useful for morphological operations.
//    -  arithmetic operations
template <class T>
struct vil1_rgb
{
  typedef T value_type;

  inline vil1_rgb() { }

  //:Create grey (v,v,v) vil1_rgb cell from value v.
  // This provides a conversion from T to vil1_rgb<T>, needed by e.g. two constructors in vil1_filter.h.

  inline vil1_rgb(T v):
    r(v), g(v), b(v) {}

  //:Construct an vil1_rgb value.
  inline vil1_rgb(T red, T green, T blue):
    r(red), g(green), b(blue) {}

  // The rgb values
  T r, g, b;
  inline T R() const { return r; }
  inline T G() const { return g; }
  inline T B() const { return b; }

  //:Convert vil1_rgb to gray using standard (.299, .587, .114) weighting.
  inline T grey() const { return T(r*0.299+0.587*g+0.114*b); }

  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  // Why can we not use .gray()?  This adds ambiguities.
#if 0
  inline operator T() const { return T(0.5+r*0.299+0.587*g+0.114*b); }
#endif

  //: equality
  inline bool operator== (vil1_rgb<T> const&) const;

  // operators
  inline vil1_rgb<T>  operator+  (vil1_rgb<T> const& A) const { return vil1_rgb<T>(r+A.r,g+A.g,b+A.b); }
  inline vil1_rgb<T>  operator-  (vil1_rgb<T> const& A) const { return vil1_rgb<T>(r-A.r,g-A.g,b-A.b); }
  inline vil1_rgb<T>  operator/  (vil1_rgb<T> const& A) const { return vil1_rgb<T>(r/A.r,g/A.g,b/A.b);}
  inline vil1_rgb<T>& operator+= (vil1_rgb<T> const& A) { r+=A.r,g+=A.g,b+=A.b; return *this; }
  inline vil1_rgb<T>& operator-= (vil1_rgb<T> const& A) { r-=A.r,g-=A.g,b-=A.b; return *this; }
  inline vil1_rgb<T>  operator*  (T A) const { return vil1_rgb<T>(r*A,g*A,b*A); }
  inline vil1_rgb<T>  operator/  (T A) const { return vil1_rgb<T>(r/A,g/A,b/A); }
  inline vil1_rgb<T>& operator*= (T A) { r*=A,g*=A,b*=A; return *this; }
  inline vil1_rgb<T>& operator/= (T A) { r/=A,g/=A,b/=A; return *this; }

#define vil1_rgb_call(m) \
m(unsigned char) \
m(int) \
m(long) \
m(double)

// VC50 bombs with INTERNAL COMPILER ERROR on template member functions.
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S> inline
  vil1_rgb(vil1_rgb<S> const& that):
    r(T(that.r)),
    g(T(that.g)),
    b(T(that.b)) { }
  template <class S> inline
  vil1_rgb<T>& operator=(vil1_rgb<S> const& that) {
    r=T(that.r);
    g=T(that.g);
    b=T(that.b);
    return *this;
  }
#else
  // For dumb compilers, just special-case the commonly used types.
# define macro(S) \
  inline vil1_rgb(vil1_rgb<S > const& that) : \
  r(T(that.r)), \
  g(T(that.g)), \
  b(T(that.b)) {}
vil1_rgb_call(macro)
# undef macro

# define macro(S) \
  InLine vil1_rgb<T>& operator=(vil1_rgb<S > const& that);
vil1_rgb_call(macro)
# undef macro
#endif
};

// see above
#if VCL_HAS_MEMBER_TEMPLATES
#else
# define macro(S) \
vil1_rgb<S > vil1_rgb_gcc_272_pump_prime(S const *); \
template <class T> inline \
vil1_rgb<T>& vil1_rgb<T>::operator=(vil1_rgb<S > const& that) { \
  r=T(that.r); \
  g=T(that.g); \
  b=T(that.b); \
  return *this; \
}

vil1_rgb_call(macro)
# undef macro
#endif

#undef vil1_rgb_call


// Assorted hackery for busted compilers
typedef vil1_rgb<double> vil1_rgb_double;

#ifdef __GNUC__
extern vil1_rgb<double> tickle_mi_fancy;
#endif


template <class T>
inline
vcl_ostream& operator<<(vcl_ostream& s, vil1_rgb<T> const& rgb)
{
  return s << '[' << rgb.r << ' ' << rgb.g << ' ' << rgb.b << ']';
}


// ** Arithmetic operators

template <class T>
inline
bool vil1_rgb<T>::operator== (vil1_rgb<T> const& o) const
{
  return r==o.r && g==o.g && b==o.b;
}


// the following cause compilation errors under Microsoft Visual C++
// is there some conflict with min and max from the std library ?
#if 0
template <class T>
inline
vil1_rgb<T> max(vil1_rgb<T> const& a, vil1_rgb<T> const& b)
{
  return vil1_rgb<T>((a.r>b.r)?a.r:b.r,
                     (a.g>b.g)?a.g:b.g,
                     (a.b>b.b)?a.b:b.b);
}

template <class T>
inline
vil1_rgb<T> min(vil1_rgb<T> const& a, vil1_rgb<T> const& b)
{
  return vil1_rgb<T>((a.r<b.r)?a.r:b.r,
                     (a.g<b.g)?a.g:b.g,
                     (a.b<b.b)?a.b:b.b);
}
#endif

template <class T>
inline
vil1_rgb<T> average(vil1_rgb<T> const& a, vil1_rgb<T> const& b)
{
  return vil1_rgb<T>((a.r + b.r)/2, (a.g + b.g)/2, (a.b + b.b)/2);
}

template <class T>
inline
vil1_rgb<T> operator+(vil1_rgb<T> const& a, vil1_rgb<T> const& b)
{
  return vil1_rgb<T>(a.r + b.r, a.g + b.g, a.b + b.b);
}

template <class T>
inline
vil1_rgb<double> operator*(double b, vil1_rgb<T> const& a)
{
  return vil1_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline
vil1_rgb<double> operator*(vil1_rgb<T> const& a, double b)
{
  return vil1_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline
vil1_rgb<double> operator/(vil1_rgb<T> const& a, double b)
{
  return vil1_rgb<double>(a.r / b, a.g / b, a.b / b);
}

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
template <class T>
inline
vil1_rgb<T> vil1_clamp_pixel(vil1_rgb<T> const& b, double range_min , double range_max)
{
  return vil1_rgb<double>(vil1_clamp_pixel(b.r, range_min , range_max),
                          vil1_clamp_pixel(b.g, range_min , range_max),
                          vil1_clamp_pixel(b.b, range_min , range_max));
}
#endif

#if 0 // capes@robots : These vil1_clamp functions are deprecated. See vil1_clamp.h
inline
vil1_rgb<unsigned char> vil1_clamp(vil1_rgb<double> const & d, vil1_rgb<unsigned char>* dummy)
{
  return vil1_rgb<unsigned char>(vil1_clamp(d.r, &dummy->r),
                                 vil1_clamp(d.g, &dummy->g),
                                 vil1_clamp(d.b, &dummy->b));
}

inline
vil1_rgb<unsigned char> vil1_clamp(vil1_rgb<float> const& d, vil1_rgb<unsigned char>* dummy)
{
  return vil1_rgb<unsigned char>(vil1_clamp(d.r, &dummy->r),
                                 vil1_clamp(d.g, &dummy->g),
                                 vil1_clamp(d.b, &dummy->b));
}
#endif

#define VIL1_RGB_INSTANTIATE(T) \
extern "you must include vil1/vil1_rgb.txx first."
#define VIL1_RGB_INSTANTIATE_LS(T) \
extern "you must include vil1/vil1_rgb.txx first."

#endif // vil1_rgb_h_
