// This is core/vil2/vil2_rgba.h
#ifndef vil2_rgba_h_
#define vil2_rgba_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
//    -#  conversion to ubyte (luminance of vil2_rgba: weights (0.299, 0.587, 0.114, 0)).
//    -#  min and max of vil2_rgba<byte>  values, useful for morphological operations.
//    -#  arithmetic operations

template <class T>
struct vil2_rgba
{
 public:
  typedef T value_type;

  // The values.
  T  r; T g; T b; T a;
  inline T R() const { return r; }
  inline T G() const { return g; }
  inline T B() const { return b; }
  inline T A() const { return a; }

  vil2_rgba() {}

  //: Create grey (v,v,v,1) vil2_rgba cell from value v.
  // This provides a conversion from T to vil2_rgba<T>, needed by e.g. two constructors in IUE_filter.h.
  vil2_rgba(T v):
    r(v), g(v), b(v), a(1) {}

  //: Construct from four values.
  vil2_rgba(T red, T green, T blue, T alpha = 1):
    r(red), g(green), b(blue), a(alpha) {}

  // VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vil2_rgba(const vil2_rgba<S>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }

  template <class S>
  vil2_rgba<T>& operator=(const vil2_rgba<S>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }
#else
  // Special-case for dumb compilers.
  inline vil2_rgba(const vil2_rgba<double>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }
  inline vil2_rgba(const vil2_rgba<unsigned char>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }

  inline vil2_rgba<T>& operator=(const vil2_rgba<double>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }

  inline vil2_rgba<T>& operator=(const vil2_rgba<float>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }

  inline vil2_rgba<T>& operator=(const vil2_rgba<unsigned char>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }
#endif

  //: Convert vil2_rgba to gray using standard (.299, .587, .114) RGB weighting.
  T grey() const { return T(0.5+r*0.299+0.587*g+0.114*b); }

  //: Convert vil2_rgba to gray using standard (.299, .587, .114) RGB weighting.
  operator T() const { return T(0.5+r*0.299+0.587*g+0.114*b); }
};

#define VIL2_RGBA_INSTANTIATE \
extern "please include vil2/vil2_rgba.txx instead"

#endif // vil2_rgba_h_
