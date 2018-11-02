// This is core/vil/vil_rgba.h
#ifndef vil_rgba_h_
#define vil_rgba_h_
//:
// \file
// \brief Templated four-value colour cell
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date 12 Nov 99

//: This is the appropriate pixel type for RGBA colour images.
// The purpose of rgba<T> is to provide an object which consists of four Ts arranged
// in order and which can be referred to as 'R', 'G', 'B' and 'A'. Thus, if win32
// does something funny when blitting an rgba bitmap to screen, that's up to the
// renderer to sort out.
//
//    Currently also includes the following `utilities':
//    -#  conversion to ubyte (luminance of vil_rgba: weights (0.299, 0.587, 0.114, 0)).
//    -#  min and max of vil_rgba<byte>  values, useful for morphological operations.
//    -#  arithmetic operations

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
struct vil_rgba
{
 public:
  typedef T value_type;

  // The values.
  T  r; T g; T b; T a;
  inline T R() const { return r; }
  inline T G() const { return g; }
  inline T B() const { return b; }
  inline T A() const { return a; }

  vil_rgba() = default;

  //: Create grey (v,v,v,1) vil_rgba cell from value v.
  // This provides a conversion from T to vil_rgba<T>, needed by e.g. two constructors in IUE_filter.h.
  vil_rgba(T v):
    r(v), g(v), b(v), a(1) {}

  //: Construct from four values.
  vil_rgba(T red, T green, T blue, T alpha = 1):
    r(red), g(green), b(blue), a(alpha) {}

  //: equality
  inline bool operator==(vil_rgba<T> const& o) const
  {
    return r==o.r && g==o.g && b==o.b && a==o.a;
  }

  template <class S>
  vil_rgba(const vil_rgba<S>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }

  template <class S>
  vil_rgba<T>& operator=(const vil_rgba<S>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }

  //: Convert vil_rgba to gray using standard (.299, .587, .114) RGB weighting.
  T grey() const { return T(0.5+r*0.299+0.587*g+0.114*b); }

#if 0 // deprecated -- use .grey() instead
  operator T() const { return T(0.5+r*0.299+0.587*g+0.114*b); }
#endif
};

#define VIL_RGBA_INSTANTIATE \
extern "please include vil/vil_rgba.hxx instead"

#endif // vil_rgba_h_
