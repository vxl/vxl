#ifndef vil_rgba_h_
#define vil_rgba_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    vil_rgba - Templated four-value colour cell
// .LIBRARY vil
// .INCLUDE vil/vil_rgba.h
// .FILE    vil_rgba.txx
// .SECTION Description
//    This is the appropriate pixel type for RGBA colour images.
// The purpose of rgba<T> is to provide an object which consists of four Ts arranged
// in order and which can be referred to as 'R', 'G', 'B' and 'A'. Thus, if win32
// does something funny when blitting an rgba bitmap to screen, that's up to the
// renderer to sort out.
//
//    Currently also includes the following `utilities':
//    (1) conversion to ubyte (luminance of vil_rgba: weights (0.299, 0.587, 0.114, 0)).
//    (2) min and max of vil_rgba<byte>  values, useful for morphological operations.
//    (3) arithmetic operations
//
// .SECTION Author
//              Philip C. Pritchett, 12 Nov 99
//              Robotics Research Group, University of Oxford
//
#include <vcl/vcl_iosfwd.h>

template <class T>
struct vil_rgba {
public:
  typedef T value_type;
  
  // -- The values.
  T  r; T g; T b; T a;
  inline T R() const { return r; }
  inline T G() const { return g; }
  inline T B() const { return b; }
  inline T A() const { return a; }
  
  // -- Create (0,0,0,0) vil_rgba cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vil_rgba():
    r(0), g(0), b(0), a(0) {}
  
  // -- Create grey (v,v,v,1) vil_rgba cell from value v.  This provides a conversion
  // from T to vil_rgba<T>, needed by e.g. two constructors in IUE_filter.h.
  vil_rgba(T v):
    r(v), g(v), b(v), a(1) {}
  
  // -- Construct from four values.
  vil_rgba(T red, T green, T blue, T alpha):
    r(red), g(green), b(blue), a(alpha) {}

  //#if defined(VCL_SGI_CC) // quell warning 3150 -- fsm
  //  vil_rgba(vil_rgba<T> const &that):
  //    r(that.r), g(that.g), b(that.b), a(that.a) {}
  //#endif

  // VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBErTEMPLATES
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
#else
  // Special-case for dumb compilers.
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vil_rgba(const vil_rgba<double>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vil_rgba(const vil_rgba<unsigned char>& that) {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
  }

  vil_rgba<T>& operator=(const vil_rgba<double>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }

  vil_rgba<T>& operator=(const vil_rgba<float>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }

  vil_rgba<T>& operator=(const vil_rgba<unsigned char>& that)
  {
    r=((T)that.r);
    g=((T)that.g);
    b=((T)that.b);
    a=((T)that.a);
    return *this;
  }
#endif

 
  // -- Convert vil_rgba to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+r*0.299+0.587*g+0.114*b); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  operator T() const { return int(0.5+r*0.299+0.587*g+0.114*b); }
  
  // -u- equality
//unimp  inline bool operator== (vil_rgba<T> const&) const;
};

#define VIL_RGBA_INSTANTIATE \
extern "please include vil/vil_rgba.txx instead"

#endif // vil_rgba_h_
