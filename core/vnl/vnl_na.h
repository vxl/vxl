// This is core/vnl/vnl_na.h
#ifndef vnl_na_h_
#define vnl_na_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif


#include <vcl_iosfwd.h>

//:
// \file
// \brief NA (Not Available) is a particular double NaN to represent missing data.
// For example, where a vnl_vector<double> represents a series of samples from an image,
// NA could be used to represent places where the measurement was taken outside the image.
//
// NA is distinct for the two other standard meanings of NaN - Indeterminate and Error.
// It is entirely up to each algorithm to treat NA values meaningfully. Unless
// a function's interpretation of NA is explicitly documentated, you should assume that
// it will be treated similarly to every other NaN.
// The IEEE754 bit value used to represent NA in double-precision is 0x7ff00000000007a2, the
// same as used by Octave and R. Initial values of NA are stored as signalling NaNs, but
// many uses will covert this to the non-signalling variant 0x7ff80000000007a2. vnl_isna()
// will accept either variant.
//
// You can read and write floating point values from a stream using standard operators
// by using a conversion manipulator.
// \verbatim
// double x, y;
// is >> x >> y;
// os << x << ' ' << y;
// \endverbatim

 
//: qNaN to indicate value Not Available.
// Don't assume that any VXL functions will do something sensible in the face of NA, unless
// explicitly documented.
double   vnl_na();


//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7ff00000000007a2, as used by Octave and R
bool vnl_na_isna(double);


//: Read a floating point number or "NA" from a stream.
// Should beahve exactly like a>>x, if the extraction operator was aware of the
// character sequence \code NA.
void vnl_na_double_extract(vcl_istream &is, double& x);


//: Write a floating point number or "NA" to a stream.
// Should beahve exactly like a<<x, if the insertion operator was aware of the
// character sequence \code NA.
void vnl_na_double_insert(vcl_ostream &is, double x);



//: Wrapper around a double that handles streaming NA.
struct vnl_na_stream_t
{
  double& x_;
  vnl_na_stream_t(double& x): x_(x) {}
};

//: Wrapper around a double that handles streaming NA.
struct vnl_na_stream_const_t
{
  const double& x_;
  vnl_na_stream_const_t(const double& x): x_(x) {}
};

//: Wrap a double to handle streaming NA.
inline vnl_na_stream_t vnl_na_stream(double& x)
{
  return vnl_na_stream_t(x);
}

//: Wrap a double to handle streaming NA.
inline vnl_na_stream_const_t vnl_na_stream(const double& x)
{
  return vnl_na_stream_const_t(x);
}

//: Insert wrapped double into stream, whilst handling NA.
inline vcl_ostream& operator <<(vcl_ostream &os, const vnl_na_stream_t& ns)
{
  vnl_na_double_insert(os, ns.x_);
  return os;
}

//: Insert wrapped double into stream, whilst handling NA.
inline vcl_ostream& operator <<(vcl_ostream &os, const vnl_na_stream_const_t& ns)
{
  vnl_na_double_insert(os, ns.x_);
  return os;
}

//: Extract wrapped double from stream, whilst handling NA.
inline vcl_istream& operator >>(vcl_istream &is, const vnl_na_stream_t& ns)
{
  vnl_na_double_extract(is, ns.x_);
  return is;
}


#endif // vnl_na_h_
