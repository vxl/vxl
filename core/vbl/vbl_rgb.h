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
  T R, G, B;
  
  // -- Create (0,0,0) vbl_rgb cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vbl_rgb():
    R(0), G(0), B(0) {}
  
  // -- Create grey (v,v,v) vbl_rgb cell from value v.  This provides a conversion
  // from T to vbl_rgb<T>, needed by e.g. two constructors in vbl_filter.h.

  vbl_rgb(T v):
    R(v), G(v), B(v) {}
  
  // -- Construct an vbl_rgb value.
  vbl_rgb(T red, T green, T blue):
    R(red), G(green), B(blue) {}


// VC50 bombs with INTERNAL COMPILER ERROR on template member functions
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S>
  vbl_rgb(const vbl_rgb<S>& that) :
    R((T)that.R),
    G((T)that.G),
    B((T)that.B) {}

  template <class S>
  vbl_rgb<T>& operator=(const vbl_rgb<S>& that) {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    return *this;
  }
#else
  // Special-case for dumb compilers.
  InLine vbl_rgb(const vbl_rgb<double>& r);
  InLine vbl_rgb(const vbl_rgb<unsigned char>& r);

  vbl_rgb<T>& operator=(const vbl_rgb<double>& that)
  {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    return *this;
  }

  vbl_rgb<T>& operator=(const vbl_rgb<unsigned char>& that)
  {
    R=((T)that.R);
    G=((T)that.G);
    B=((T)that.B);
    return *this;
  }
#endif

 
  // -- Convert vbl_rgb to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+R*0.299+0.587*G+0.114*B); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  operator T() const { return int(0.5+R*0.299+0.587*G+0.114*B); }
  
  // -- equality
  inline bool operator== (vbl_rgb<T> const&) const;

  // -- operators
  vbl_rgb<T> operator+ (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R+A.R,G+A.G,B+A.B); }
  vbl_rgb<T> operator- (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R-A.R,G-A.G,B-A.B); }
  vbl_rgb<T> operator/ (vbl_rgb<T> const& A) const { return vbl_rgb<T>(R/A.R,G/A.G,B/A.B);}
  vbl_rgb<T>& operator+= (vbl_rgb<T> const& A) { R+=A.R,G+=A.G,B+=A.B; return *this; }
  vbl_rgb<T>& operator-= (vbl_rgb<T> const& A) { R-=A.R,G-=A.G,B-=A.B; return *this; }
  vbl_rgb<T> operator* (T A) const { return vbl_rgb<T>(R*A,G*A,B*A); }
  vbl_rgb<T> operator/ (T A) const { return vbl_rgb<T>(R/A,G/A,B/A); }
  vbl_rgb<T>& operator*= (T A) { R*=A,G*=A,B*=A; return *this; }
  vbl_rgb<T>& operator/= (T A) { R/=A,G/=A,B/=A; return *this; }
};

// Assorted hackery for busted compilers
typedef vbl_rgb<double> vbl_rgb_double;

#ifdef __GNUC__
extern vbl_rgb<double> tickle_mi_fancy;
#endif


#if !VCL_HAS_MEMBER_TEMPLATES
template <class T>
InLine vbl_rgb<T>::vbl_rgb(const vbl_rgb_double& r) :
  R((T)r.R),
  G((T)r.G),
  B((T)r.B) {}

template <class T>
InLine vbl_rgb<T>::vbl_rgb(const vbl_rgb<unsigned char>& r) :
  R((T)r.R),
  G((T)r.G),
  B((T)r.B) {}
#endif

template <class T>
inline
ostream& operator<<(ostream& s, const vbl_rgb<T>& rgb)
{
  return s << '[' << rgb.R << ' ' << rgb.G << ' ' << rgb.B << ']';
}

// Specialization to get vbl_rgb<byte> right.
VCL_DECLARE_SPECIALIZATION(ostream& operator<<(ostream& s, const vbl_rgb<unsigned char>& rgb))

// ** Arithmetic operators

template <class T>
inline
bool vbl_rgb<T>::operator== (vbl_rgb<T> const& o) const
{
  return R==o.R && G==o.G && B==o.B;
}

template <class T> 
inline
vbl_rgb<T> max(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R>b.R)?a.R:b.R,
		(a.G>b.G)?a.G:b.G,
		(a.B>b.B)?a.B:b.B);
}

template <class T> 
inline
vbl_rgb<T> min(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R<b.R)?a.R:b.R,
		(a.G<b.G)?a.G:b.G,
		(a.B<b.B)?a.B:b.B);
}

template <class T> 
inline
vbl_rgb<T> average(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>((a.R + b.R)/2, (a.G + b.G)/2, (a.B + b.B)/2);
}

template <class T> 
inline
vbl_rgb<T> operator+(const vbl_rgb<T>& a, const vbl_rgb<T>& b)
{
  return vbl_rgb<T>(a.R + b.R, a.G + b.G, a.B + b.B);
}

template <class T> 
inline
vbl_rgb<double> operator*(double b, vbl_rgb<T> const& a)
{
  return vbl_rgb<double>(a.R * b, a.G * b, a.B * b);
}

template <class T> 
inline
vbl_rgb<double> operator*(const vbl_rgb<T>& a, double b)
{
  return vbl_rgb<double>(a.R * b, a.G * b, a.B * b);
}

template <class T> 
inline
vbl_rgb<double> operator/(const vbl_rgb<T>& a, double b)
{
  return vbl_rgb<double>(a.R / b, a.G / b, a.B / b);
}

#define VBL_RGB_INSTANTIATE(T)  "error, see .txx file"
#define VBL_RGB_INSTANTIATE_LS(T) "error, see .txx file"

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_rgb.
