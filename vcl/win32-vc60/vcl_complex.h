#ifndef vcl_win32_vc60_complex_h_
#define vcl_win32_vc60_complex_h_


// fsm: complex<T> is derived from _Complex_base<T>, which is not
// declared with __declspec(dllimport). So complex<T> shouldn't be either
// (else the compiler will emit an error). Whether or not it is depends on
// the value of _CRTIMP being set, e.g. in <math.h>

// These stream includes may not appear necessary, but lots of link
// errors may appear if they aren't here. IMS
// It seems that these includes must appear for the first time before
// _CRTIMP is modified below.  Otherwise, basic_filebuf (and others?)
// member functions get defined more than once. FWW
# include <vcl_fstream.h>       // don't remove
# include <vcl_sstream.h>       // don't remove

# include <vcl_cmath.h>
# pragma warning (push)
# pragma warning (disable: 4273)
# undef _CRTIMP
# define _CRTIMP
# include <ctype.h>
# include <complex>
# pragma warning (pop)

// It used to necessary to bring the complex math functions
// from the std namespace into the global namespace to avoid
// conflicts with the (incorrect) cmath and cstdlib headers. It
// is no longer necessary because now vcl versions of those
// headers bring the math functions into the std namespace
// instead. Thus std::complex can live completely in the std
// namespace.
//   -- Amitha Perera

#define vcl_generic_complex_STD std
#include "../generic/vcl_complex.h"

#endif // vcl_win32_vc60_complex_h_
