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
  T  R_; T G_; T B_; T A_;
  
  // -- Create (0,0,0,0) vbl_rgba cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vbl_rgba():
    R_(0), G_(0), B_(0), A_(0) {}
  
  // -- Create grey (v,v,v,1) vbl_rgba cell from value v.  This provides a conversion
  // from T to vbl_rgba<T>, needed by e.g. two constructors in IUE_filter.h.
  vbl_rgba(T v):
    R_(v), G_(v), B_(v), A_(1) {}
  
  // -- Construct from four values.
  vbl_rgba(T red, T green, T blue, T alpha):
    R_(red), G_(green), B_(blue), A_(alpha) {}

  //#if defined(VCL_SGI_CC) // quell warning 3150 -- fsm
  //  vbl_rgba(vbl_rgba<T> const &that):
  //    R_(that.R_), G_(that.G_), B_(that.B_), A_(that.A_) {}
  //#endif

  // VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vbl_rgba(const vbl_rgba<S>& that) {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
  } 

  template <class S>
  vbl_rgba<T>& operator=(const vbl_rgba<S>& that) {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
    return *this;
  }
#else
  // Special-case for dumb compilers.
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vbl_rgba(const vbl_rgba<double>& that) {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
  }
# ifdef VCL_SUNPRO_CC
  inline 
# endif
  vbl_rgba(const vbl_rgba<unsigned char>& that) {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
  }

  vbl_rgba<T>& operator=(const vbl_rgba<double>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
    return *this;
  }

  vbl_rgba<T>& operator=(const vbl_rgba<float>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
    return *this;
  }

  vbl_rgba<T>& operator=(const vbl_rgba<unsigned char>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    A_=((T)that.A_);
    return *this;
  }
#endif

 
  // -- Convert vbl_rgba to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  operator T() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // -- equality
  inline bool operator== (vbl_rgba<T> const&) const;
};

#define VBL_RGBA_INSTANTIATE \
extern "please include vbl/vbl_rgba.txx instead"

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_rgba.
