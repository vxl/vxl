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

//: qNaN to indicate value Not Available.
// Don't assume that any VXL functions will do something sensible in the face of NA, unless
// explicitly documented.
double   vnl_na();


//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7ff00000000007a2, as used by Octave and R
bool vnl_na_isna(double);


//: Read a floating point number or "NA" from a stream.
// Should beahve exactly like a>>x, if the extraction operator was aware of the character
// sequence \code NA.
void vnl_na_double_parse(vcl_istream &is, double& x);

#endif // vnl_na_h_
