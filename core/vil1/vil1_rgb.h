#ifndef vil_rgb_h_
#define vil_rgb_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_rgb.h

//:
// \file
// \author Peter Vanroose, K.U.Leuven
// \brief This is the appropriate pixel type for 24-bit colour images.
//
//\verbatim
//    Currently also includes the following `utilities':
//    (1) conversion to ubyte (luminance of vil_rgb: weights (0.299,0.587,0.114)).
//    (2) min and max of vil_rgbcell values, useful for morphological operations.
//    (3) arithmetic operations
//\endverbatim
//
//\verbatim
//  Modification:
//    Peter Vanroose, K.U.Leuven, ESAT/VISICS, 15 nov. 1997
//    250198 AWF Templated.
//    250198 AWF Modified to make POD struct until gcc inlines when debugging.
//    160298 PCP Removed underscore from public members.
//    290798 AWF Member templates for fancy compilers
//    220598 PVr moved instantiations files to Templates subdirectory.
//    050598 PVr added several operators ( + += - -= (T) ).
//    140898 David Capel added clamping functions to ensure 0-255 range on bytes and vil_rgb<byte>
//    090600 David Capel made clamping functions inline and removed all that partial specialization
//           nonsense from the .txx file.
//\endverbatim

#include <vcl_iostream.h>
#include <vil/vil_clamp.h>

#ifdef VCL_SUNPRO_CC
# define InLine inline
#else
# define InLine
#endif

//: Pixel type for 24 bit images
template <class T>
struct vil_rgb
{
  typedef T value_type;

#if 0
  //:Create (0,0,0) vil_rgb cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vil_rgb():
    r(0), g(0), b(0) {}
#else
  // No, it doesn't mandate that. It merely says there
  // must be a default constructor. When allocating big
  // buffers of vil_rgb<T>s we don't want to pay for
  // unnecessary initialization. -- fsm.
  vil_rgb() { }
#endif

  //:Create grey (v,v,v) vil_rgb cell from value v.  This provides a conversion
  // from T to vil_rgb<T>, needed by e.g. two constructors in vil_filter.h.

  vil_rgb(T v):
    r(v), g(v), b(v) {}

  //:Construct an vil_rgb value.
  vil_rgb(T red, T green, T blue):
    r(red), g(green), b(blue) {}

  // The rgb values
  T r, g, b;
  inline T R() const { return r; }
  inline T G() const { return g; }
  inline T B() const { return b; }

  //:Convert vil_rgb to gray using standard (.299, .587, .114) weighting.
  T grey() const { return T(r*0.299+0.587*g+0.114*b); }

  // This was wrong because it doesn't work with vil_rgb<double> where the
  // dynamic range is [0, 1]. Even if the dynamic range were [0, 255] it still
  // shouldn't cast the result to an int. fsm@robots.ox.ac.uk
  //  T grey() const { return int(0.5+r*0.299+0.587*g+0.114*b); }

  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  // Why can we not use .gray()?  This adds ambiguities.
  // operator T() const { return int(0.5+r*0.299+0.587*g+0.114*b); }

  //:equality
  inline bool operator== (vil_rgb<T> const&) const;

  // operators
  vil_rgb<T>  operator+  (vil_rgb<T> const& A) const { return vil_rgb<T>(r+A.r,g+A.g,b+A.b); }
  vil_rgb<T>  operator-  (vil_rgb<T> const& A) const { return vil_rgb<T>(r-A.r,g-A.g,b-A.b); }
  vil_rgb<T>  operator/  (vil_rgb<T> const& A) const { return vil_rgb<T>(r/A.r,g/A.g,b/A.b);}
  vil_rgb<T>& operator+= (vil_rgb<T> const& A) { r+=A.r,g+=A.g,b+=A.b; return *this; }
  vil_rgb<T>& operator-= (vil_rgb<T> const& A) { r-=A.r,g-=A.g,b-=A.b; return *this; }
  vil_rgb<T>  operator*  (T A) const { return vil_rgb<T>(r*A,g*A,b*A); }
  vil_rgb<T>  operator/  (T A) const { return vil_rgb<T>(r/A,g/A,b/A); }
  vil_rgb<T>& operator*= (T A) { r*=A,g*=A,b*=A; return *this; }
  vil_rgb<T>& operator/= (T A) { r/=A,g/=A,b/=A; return *this; }

#define vil_rgb_call(m) \
m(unsigned char) \
m(int) \
m(long) \
m(double)

// VC50 bombs with INTERNAL COMPILER ERROR on template member functions.
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vil_rgb(vil_rgb<S> const& that):
    r(T(that.r)),
    g(T(that.g)),
    b(T(that.b)) { }
  template <class S>
  vil_rgb<T>& operator=(vil_rgb<S> const& that) {
    r=T(that.r);
    g=T(that.g);
    b=T(that.b);
    return *this;
  }
#else
  // For dumb compilers, just special-case the commonly used types.
# define macro(S) \
  vil_rgb(vil_rgb<S > const& that) : \
  r(T(that.r)), \
  g(T(that.g)), \
  b(T(that.b)) {}
vil_rgb_call(macro)
# undef macro

# define macro(S) \
  InLine vil_rgb<T>& operator=(vil_rgb<S > const& that);
vil_rgb_call(macro)
# undef macro
#endif
};

// see above
#if VCL_HAS_MEMBER_TEMPLATES
#else
# define macro(S) \
vil_rgb<S > vil_rgb_gcc_272_pump_prime(S const *); \
template <class T> \
vil_rgb<T>& vil_rgb<T>::operator=(vil_rgb<S > const& that) { \
  r=T(that.r); \
  g=T(that.g); \
  b=T(that.b); \
  return *this; \
}
vil_rgb_call(macro)
# undef macro
#endif

#undef vil_rgb_call


// Assorted hackery for busted compilers
typedef vil_rgb<double> vil_rgb_double;

#ifdef __GNUC__
extern vil_rgb<double> tickle_mi_fancy;
#endif


template <class T>
inline
vcl_ostream& operator<<(vcl_ostream& s, vil_rgb<T> const& rgb)
{
  return s << '[' << rgb.r << ' ' << rgb.g << ' ' << rgb.b << ']';
}


// ** Arithmetic operators

template <class T>
inline
bool vil_rgb<T>::operator== (vil_rgb<T> const& o) const
{
  return r==o.r && g==o.g && b==o.b;
}

template <class T>
inline
vil_rgb<T> max(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.r>b.r)?a.r:b.r,
                (a.g>b.g)?a.g:b.g,
                (a.b>b.b)?a.b:b.b);
}

template <class T>
inline
vil_rgb<T> min(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.r<b.r)?a.r:b.r,
                (a.g<b.g)?a.g:b.g,
                (a.b<b.b)?a.b:b.b);
}

template <class T>
inline
vil_rgb<T> average(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.r + b.r)/2, (a.g + b.g)/2, (a.b + b.b)/2);
}

template <class T>
inline
vil_rgb<T> operator+(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>(a.r + b.r, a.g + b.g, a.b + b.b);
}

template <class T>
inline
vil_rgb<double> operator*(double b, vil_rgb<T> const& a)
{
  return vil_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline
vil_rgb<double> operator*(vil_rgb<T> const& a, double b)
{
  return vil_rgb<double>(a.r * b, a.g * b, a.b * b);
}

template <class T>
inline
vil_rgb<double> operator/(vil_rgb<T> const& a, double b)
{
  return vil_rgb<double>(a.r / b, a.g / b, a.b / b);
}

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
template <class T>
inline
vil_rgb<T> vil_clamp_pixel(vil_rgb<T> const& b, double range_min , double range_max)
{
  return vil_rgb<double>(vil_clamp_pixel(b.r, range_min , range_max),
                         vil_clamp_pixel(b.g, range_min , range_max),
                         vil_clamp_pixel(b.b, range_min , range_max));
}
#endif

// capes@robots : These vil_clamp functions are deprecated. See vil_clamp.h
#if 0
inline
vil_rgb<unsigned char> vil_clamp(vil_rgb<double> const & d, vil_rgb<unsigned char>* dummy)
{
  return vil_rgb<unsigned char>(vil_clamp(d.r, &dummy->r),
                                vil_clamp(d.g, &dummy->g),
                                vil_clamp(d.b, &dummy->b));
}

inline
vil_rgb<unsigned char> vil_clamp(vil_rgb<float> const& d, vil_rgb<unsigned char>* dummy)
{
  return vil_rgb<unsigned char>(vil_clamp(d.r, &dummy->r),
                                vil_clamp(d.g, &dummy->g),
                                vil_clamp(d.b, &dummy->b));
}
#endif

#define VIL_RGB_INSTANTIATE(T) \
extern "you must include vil/vil_rgb.txx first."
#define VIL_RGB_INSTANTIATE_LS(T) \
extern "you must include vil/vil_rgb.txx first."

#endif // vil_rgb_h_
