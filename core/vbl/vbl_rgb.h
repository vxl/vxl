//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_rgb_h_
#define vbl_rgb_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    vbl_rgb - Templated three-value colour cell
// .HEADER  Basics Package
// .LIBRARY vbl
// .INCLUDE vbl/vbl_rgb.h
// .FILE    vbl/vbl_rgb.cxx
//
// .SECTION Description
//    This is the appropriate pixel type for 24-bit colour images.
//    Currently also includes the following `utilities':
//    (1) conversion to ubyte (luminance of vbl_rgb: weights (0.299,0.587,0.114)).
//    (2) min and max of vbl_rgbcell values, useful for morphological operations.
//    (3) arithmetic operations
//
// .SECTION Author
//    Peter Vanroose, K.U.Leuven, ESAT/VISICS, 15 nov. 1997
//    250198 AWF Templated.
//    250198 AWF Modified to make POD struct until gcc inlines when debugging.
//    160298 PCP Removed underscore from public members.
//    290798 AWF Member templates for fancy compilers
//    220598 PVr moved instantiations files to Templates subdirectory.
//    050598 PVr added several operators ( + += - -= (T) ).
//    140898 David Capel added clamping functions to ensure 0-255 range on bytes and vbl_rgb<byte>

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iosfwd.h>
#include <vbl/vbl_clamp.h>

#ifdef VCL_SUNPRO_CC
# define InLine inline
#else
# define InLine
#endif

template <class T>
struct vbl_rgb {
public:
  typedef T value_type;
  
  // -- The rgb values
  T R_, G_, B_;
  inline T R() const { return R_; }
  inline T G() const { return G_; }
  inline T B() const { return B_; }
  
  // -- Create (0,0,0) vbl_rgb cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vbl_rgb():
    R_(0), G_(0), B_(0) {}
  
  // -- Create grey (v,v,v) vbl_rgb cell from value v.  This provides a conversion
  // from T to vbl_rgb<T>, needed by e.g. two constructors in vbl_filter.h.

  vbl_rgb(T v):
    R_(v), G_(v), B_(v) {}
  
  // -- Construct an vbl_rgb value.
  vbl_rgb(T red, T green, T blue):
    R_(red), G_(green), B_(blue) {}


// VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vbl_rgb(const vbl_rgb<S>& that) :
    R_((T)that.R_),
    G_((T)that.G_),
    B_((T)that.B_) {}

  template <class S>
  vbl_rgb<T>& operator=(const vbl_rgb<S>& that) {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    return *this;
  }
#else
  // Special-case for dumb compilers.
  InLine vbl_rgb(const vbl_rgb<double>& r);
  InLine vbl_rgb(const vbl_rgb<unsigned char>& r);

  vbl_rgb<T>& operator=(const vbl_rgb<double>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    return *this;
  }

  vbl_rgb<T>& operator=(const vbl_rgb<unsigned char>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    return *this;
  }

  vbl_rgb<T>& operator=(const vbl_rgb<int>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    return *this;
  }

  vbl_rgb<T>& operator=(const vbl_rgb<long>& that)
  {
    R_=((T)that.R_);
    G_=((T)that.G_);
    B_=((T)that.B_);
    return *this;
  }
#endif

 
  // -- Convert vbl_rgb to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  // Why can we not use .gray()?  This adds ambiguities.
  // operator T() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // -- equality
  inline bool operator== (vbl_rgb<T> const&) const;

  // -- operators
  vbl_rgb<T> operator+ (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R_+A.R_,G_+A.G_,B_+A.B_); }
  vbl_rgb<T> operator- (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R_-A.R_,G_-A.G_,B_-A.B_); }
  vbl_rgb<T> operator/ (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R_/A.R_,G_/A.G_,B_/A.B_);}
  vbl_rgb<T>& operator+= (vbl_rgb<T> const& A) { R_+=A.R_,G_+=A.G_,B_+=A.B_; return *this; }
  vbl_rgb<T>& operator-= (vbl_rgb<T> const& A) { R_-=A.R_,G_-=A.G_,B_-=A.B_; return *this; }
  vbl_rgb<T> operator* (T A) const { return vbl_rgb<T>(R_*A,G_*A,B_*A); }
  vbl_rgb<T> operator/ (T A) const { return vbl_rgb<T>(R_/A,G_/A,B_/A); }
  vbl_rgb<T>& operator*= (T A) { R_*=A,G_*=A,B_*=A; return *this; }
  vbl_rgb<T>& operator/= (T A) { R_/=A,G_/=A,B_/=A; return *this; }
};

// Assorted hackery for busted compilers
typedef vbl_rgb<double> vbl_rgb_double;

#ifdef __GNUC__
extern vbl_rgb<double> tickle_mi_fancy;
#endif


#if !VCL_HAS_MEMBER_TEMPLATES
template <class T>
InLine vbl_rgb<T>::vbl_rgb(const vbl_rgb_double& r) :
  R_((T)r.R_),
  G_((T)r.G_),
  B_((T)r.B_) {}

template <class T>
InLine vbl_rgb<T>::vbl_rgb(const vbl_rgb<unsigned char>& r) :
  R_((T)r.R_),
  G_((T)r.G_),
  B_((T)r.B_) {}
#endif

template <class T>
inline
ostream& operator<<(ostream& s, const vbl_rgb<T>& rgb)
{
  return s << '[' << rgb.R_ << ' ' << rgb.G_ << ' ' << rgb.B_ << ']';
}

// Specialization to get vbl_rgb<byte> right.
// VCL_DECxLARE_SPECIALIZATION(ostream& operator<<(ostream& s, const vbl_rgb<unsigned char>& rgb))

// ** Arithmetic operators

template <class T>
inline
bool vbl_rgb<T>::operator== (vbl_rgb<T> const& o) const
{
  return R_==o.R_ && G_==o.G_ && B_==o.B_;
}

template <class T> 
inline
vbl_rgb<T> max(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R_>b.R_)?a.R_:b.R_,
		(a.G_>b.G_)?a.G_:b.G_,
		(a.B_>b.B_)?a.B_:b.B_);
}

template <class T> 
inline
vbl_rgb<T> min(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R_<b.R_)?a.R_:b.R_,
		(a.G_<b.G_)?a.G_:b.G_,
		(a.B_<b.B_)?a.B_:b.B_);
}

template <class T> 
inline
vbl_rgb<T> average(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R_ + b.R_)/2, (a.G_ + b.G_)/2, (a.B_ + b.B_)/2);
}

template <class T> 
inline
vbl_rgb<T> operator+(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>(a.R_ + b.R_, a.G_ + b.G_, a.B_ + b.B_);
}

template <class T> 
inline
vbl_rgb<double> operator*(double b, vbl_rgb<T> const& a)
{
  return vbl_rgb<double>(a.R_ * b, a.G_ * b, a.B_ * b);
}

template <class T> 
inline
vbl_rgb<double> operator*(const vbl_rgb<T>& a, double b)
{
  return vbl_rgb<double>(a.R_ * b, a.G_ * b, a.B_ * b);
}

template <class T> 
inline
vbl_rgb<double> operator/(const vbl_rgb<T>& a, double b)
{
  return vbl_rgb<double>(a.R_ / b, a.G_ / b, a.B_ / b);
}

#define VBL_RGB_INSTANTIATE(T) \
extern "please include vbl/vbl_rgb.txx instead"
#define VBL_RGB_INSTANTIATE_LS(T) \
extern "please include vbl/vbl_rgb.txx instead"

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_rgb.
