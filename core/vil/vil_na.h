// This is core/vil/vil_na.h
#ifndef vil_na_h_
#define vil_na_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief NA (Not Available) is a particular double NaN to represent missing data.
// For example, where a vnl_vector<double> represents a series of samples from an image,
// NA could be used to represent places where the measurement was taken outside the image.
//
// NA is distinct for the two other standard meanings of NaN - Indeterminate and Error.
// It is entirely up to each algorithm to treat NA values meaningfully. Unless
// a function's interpretation of NA is explicitly documented, you should assume that
// it will be treated similarly to every other NaN.
// The IEEE754 bit value used to represent NA in double-precision is 0x7ff00000000007a2, the
// same as used by Octave and R. Initial values of NA are stored as signalling NaNs, but
// many uses will convert this to the non-signalling variant 0x7ff80000000007a2. vil_na_isna()
// will accept either variant.
//
// You can read and write floating point values from a stream using standard operators
// by using a conversion manipulator.
// \verbatim
// double x, y;
// is >> x >> y;
// os << x << ' ' << y;
// \endverbatim
//
// This file is a cut-and-paste of the essential sections of vnl_na, since vil is not
// allowed to depend upon vnl


//: qNaN to indicate value Not Available.
// Don't assume that any VXL functions will do something sensible in the face of NA, unless
// explicitly documented.
double   vil_na();


//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7ff00000000007a2, as used by Octave and R
bool vil_na_isna(double);



#endif // vil_na_h_
