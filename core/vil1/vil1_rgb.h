//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_rgb_h_
#define vil_rgb_h_
#ifdef __GNUC__
#pragma interface "vil_rgb"
#endif
// .NAME    vil_rgb<> - Templated three-value colour cell
//
// .SECTION Description
//    This is the appropriate pixel type for 24-bit colour images.
//    Currently also includes the following `utilities':
//    (1) conversion to ubyte (luminance of vil_rgb: weights (0.299,0.587,0.114)).
//    (2) min and max of vil_rgbcell values, useful for morphological operations.
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
//    140898 David Capel added clamping functions to ensure 0-255 range on bytes and vil_rgb<byte>
//    090600 David Capel made clamping functions inline and removed all that partial specialization
//           nonsense from the .txx file.

#include <vcl/vcl_iostream.h>
#include <vil/vil_clamp.h>

#ifdef VCL_SUNPRO_CC
# define InLine inline
#else
# define InLine
#endif

template <class T>
struct vil_rgb 
{
  typedef T value_type;
  
  // -- Create (0,0,0) vil_rgb cell. We need the default ctor to do this as the STL
  // effectively mandates that T() produces a nil value.
  vil_rgb():
    R_(0), G_(0), B_(0) {}
  
  // -- Create grey (v,v,v) vil_rgb cell from value v.  This provides a conversion
  // from T to vil_rgb<T>, needed by e.g. two constructors in vil_filter.h.

  vil_rgb(T v):
    R_(v), G_(v), B_(v) {}
  
  // -- Construct an vil_rgb value.
  vil_rgb(T red, T green, T blue):
    R_(red), G_(green), B_(blue) {}

  // -- The rgb values
  T R_, G_, B_;
  inline T R() const { return R_; }
  inline T G() const { return G_; }
  inline T B() const { return B_; }
  
  // -- Convert vil_rgb to gray using standard (.299, .587, .114) weighting.
  T grey() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // Who wants this? It's a pain in the ass.
  // ImageProcessing/IIFOperators use this a lot!
  // Why can we not use .gray()?  This adds ambiguities.
  // operator T() const { return int(0.5+R_*0.299+0.587*G_+0.114*B_); }
  
  // -- equality
  inline bool operator== (vil_rgb<T> const&) const;

  // -- operators
  vil_rgb<T>  operator+  (vil_rgb<T> const& A) const { return vil_rgb<T>(R_+A.R_,G_+A.G_,B_+A.B_); }
  vil_rgb<T>  operator-  (vil_rgb<T> const& A) const { return vil_rgb<T>(R_-A.R_,G_-A.G_,B_-A.B_); }
  vil_rgb<T>  operator/  (vil_rgb<T> const& A) const { return vil_rgb<T>(R_/A.R_,G_/A.G_,B_/A.B_);}
  vil_rgb<T>& operator+= (vil_rgb<T> const& A) { R_+=A.R_,G_+=A.G_,B_+=A.B_; return *this; }
  vil_rgb<T>& operator-= (vil_rgb<T> const& A) { R_-=A.R_,G_-=A.G_,B_-=A.B_; return *this; }
  vil_rgb<T>  operator*  (T A) const { return vil_rgb<T>(R_*A,G_*A,B_*A); }
  vil_rgb<T>  operator/  (T A) const { return vil_rgb<T>(R_/A,G_/A,B_/A); }
  vil_rgb<T>& operator*= (T A) { R_*=A,G_*=A,B_*=A; return *this; }
  vil_rgb<T>& operator/= (T A) { R_/=A,G_/=A,B_/=A; return *this; }

#define vil_rgb_call(m) \
m(unsigned char) \
m(int) \
m(long) \
m(double)

// VC50 bombs with INTERNAL COMPILER ERROR on template member functions.
#if VCL_HAS_MEMBER_TEMPLATES
  template <class S> 
  vil_rgb(vil_rgb<S> const& that):
    R_(T(that.R_)),
    G_(T(that.G_)),
    B_(T(that.B_)) { }
  template <class S> 
  vil_rgb<T>& operator=(vil_rgb<S> const& that) {
    R_=T(that.R_);
    G_=T(that.G_);
    B_=T(that.B_);
    return *this;
  }
#else
  // For dumb compilers, just special-case the commonly used types.
# define macro(S) \
  vil_rgb(vil_rgb<S > const& that) : \
  R_(T(that.R_)), \
  G_(T(that.G_)), \
  B_(T(that.B_)) {}
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
  R_=T(that.R_); \
  G_=T(that.G_); \
  B_=T(that.B_); \
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
ostream& operator<<(ostream& s, vil_rgb<T> const& rgb)
{
  return s << '[' << rgb.R_ << ' ' << rgb.G_ << ' ' << rgb.B_ << ']';
}

// Specialization to get vil_rgb<byte> right.
VCL_DEFINE_SPECIALIZATION
ostream& operator<<(ostream&, vil_rgb<unsigned char> const&);

// ** Arithmetic operators

template <class T>
inline
bool vil_rgb<T>::operator== (vil_rgb<T> const& o) const
{
  return R_==o.R_ && G_==o.G_ && B_==o.B_;
}

template <class T> 
inline
vil_rgb<T> max(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.R_>b.R_)?a.R_:b.R_,
		(a.G_>b.G_)?a.G_:b.G_,
		(a.B_>b.B_)?a.B_:b.B_);
}

template <class T> 
inline
vil_rgb<T> min(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.R_<b.R_)?a.R_:b.R_,
		(a.G_<b.G_)?a.G_:b.G_,
		(a.B_<b.B_)?a.B_:b.B_);
}

template <class T> 
inline
vil_rgb<T> average(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>((a.R_ + b.R_)/2, (a.G_ + b.G_)/2, (a.B_ + b.B_)/2);
}

template <class T> 
inline
vil_rgb<T> operator+(vil_rgb<T> const& a, vil_rgb<T> const& b)
{
  return vil_rgb<T>(a.R_ + b.R_, a.G_ + b.G_, a.B_ + b.B_);
}

template <class T> 
inline
vil_rgb<double> operator*(double b, vil_rgb<T> const& a)
{
  return vil_rgb<double>(a.R_ * b, a.G_ * b, a.B_ * b);
}

template <class T> 
inline
vil_rgb<double> operator*(vil_rgb<T> const& a, double b)
{
  return vil_rgb<double>(a.R_ * b, a.G_ * b, a.B_ * b);
}

template <class T> 
inline
vil_rgb<double> operator/(vil_rgb<T> const& a, double b)
{
  return vil_rgb<double>(a.R_ / b, a.G_ / b, a.B_ / b);
}

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
template <class T>
inline
vil_rgb<T> vil_clamp_pixel(vil_rgb<T> const& b, double range_min , double range_max)
{
  return vil_rgb<double>(vil_clamp_pixel(b.R_, range_min , range_max),
			 vil_clamp_pixel(b.G_, range_min , range_max),
			 vil_clamp_pixel(b.B_, range_min , range_max));
}
#endif

// capes@robots : These vil_clamp functions are deprecated. See vil_clamp.h
/*
inline
vil_rgb<unsigned char> vil_clamp(vil_rgb<double> const & d, vil_rgb<unsigned char>* dummy)
{
  return vil_rgb<unsigned char>(vil_clamp(d.R_, &dummy->R_),
				vil_clamp(d.G_, &dummy->G_),
				vil_clamp(d.B_, &dummy->B_));
}

inline
vil_rgb<unsigned char> vil_clamp(vil_rgb<float> const& d, vil_rgb<unsigned char>* dummy)
{
  return vil_rgb<unsigned char>(vil_clamp(d.R_, &dummy->R_),
				vil_clamp(d.G_, &dummy->G_),
				vil_clamp(d.B_, &dummy->B_));
}
*/
#define VIL_RGB_INSTANTIATE(T) \
extern "you must include vil/vil_rgb.txx first."
#define VIL_RGB_INSTANTIATE_LS(T) \
extern "you must include vil/vil_rgb.txx first."

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_rgb<>.
