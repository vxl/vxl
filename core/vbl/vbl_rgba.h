//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_rgba_h_
#define vbl_rgba_h_
#ifdef __GNUC__
#pragma interface
#endif
// class vbl_rgba<T>:
//    This is the appropriate pixel type for RGBA colour images.
// The purpose of rgba<T> is to provide an object which consists of four Ts arranged
// in order and which can be referred to as 'R', 'G', 'B' and 'A'. Thus, if win32
// does something funny when blitting an rgba bitmap to screen, that's up to the
// renderer to sort out.
//
//    Currently also includes the following `utilities':
//    (1) conversion to ubyte (luminance of vbl_rgba: weights (0.299, 0.587, 0.114, 0)).
//    (2) min and max of vbl_rgba<byte>  values, useful for morphological operations.
//    (3) arithmetic operations
//
// .SECTION Author
//              Philip C. Pritchett, 12 Nov 99
//              Robotics Research Group, University of Oxford
//
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iosfwd.h>

template <class T>
struct vbl_rgba {
public:
  typedef T value_type;
  
  // -- The values.
  // #ifdef WIN32
  //   T  A, B, G, R;
  // #else
  T  R; T G; T B; T A;
  // #endif
  
  // -- Create (0,0,0,0) vbl_rgba cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vbl_rgba():
    R(0), G(0), B(0), A(0) {}
  
  // -- Create grey (v,v,v,1) vbl_rgba cell from value v.  This provides a conversion
  // from T to vbl_rgba<T>, needed by e.g. two constructors in IUE_filter.h.
  vbl_rgba(T v):
    R(v), G(v), B(v), A(1) {}
  
  // -- Construct from four values.
  vbl_rgba(T red, T green, T blue, T alpha):
    R(red), G(green), B(blue), A(alpha) {}

  //#if defined(VCL_SGI_CC) // quell warning 3150 -- fsm
  //  vbl_rgba(vbl_rgba<T> const &that):
  //    R(that.R), B(that.B), G(that.G), A(that.A) {}
  //#endif

  // VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vbl_rgba(const vbl_rgba<S>& that) {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
  } 

  template <class S>
  vbl_rgba<T>& operator=(const vbl_rgba<S>& that) {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
    return *this;
  }
#else
  // Special-case for dumb compilers.
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vbl_rgba(const vbl_rgba<double>& that) {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
  }
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vbl_rgba(const vbl_rgba<unsigned char>& that) {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
  }

  vbl_rgba<T>& operator=(const vbl_rgba<double>& that)
  {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
    return *this;
  }

  vbl_rgba<T>& operator=(const vbl_rgba<float>& that)
  {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
    return *this;
  }

  vbl_rgba<T>& operator=(const vbl_rgba<unsigned char>& that)
  {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    A=((T)that.A);
    return *this;
  }
#endif

 
  // -- Convert vbl_rgba to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+R*0.299+0.587*G+0.114*B); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  operator T() const { return int(0.5+R*0.299+0.587*G+0.114*B); }
  
  // -- equality
  inline bool operator== (vbl_rgba<T> const&) const;
};

#define VBL_RGBA_INSTANTIATE(T) extern "please include vbl_rgba.txx"

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_rgba.
