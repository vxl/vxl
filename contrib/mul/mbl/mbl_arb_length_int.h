#ifndef mbl_arb_length_int_h_
#define mbl_arb_length_int_h_

//:
// \file
// \brief Infinite length integer numbers
// \author Ian Scott
// \deprecated in favour of vnl_big_num
// Note the vsl binary io for vnl_bignum IS NOT COMPATIBLE with mbl_arb_length_int.
// Convert via a vcl_stringstream.
//
// The  mbl_arb_length_int class  provides infinite length integer numbers and
// arithmetic, using a vcl_vector of unsigneds.

#include <vcl_deprecated_header.h>


#include <vnl/vnl_bignum.h>
typedef vnl_bignum mbl_arb_length_int;


#endif // mbl_arb_length_int_h_
