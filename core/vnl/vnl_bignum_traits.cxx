//:
// \file
// \author Peter Vanroose
// \date   6 September 2002
//
//-----------------------------------------------------------------------------

#include "vnl_bignum_traits.h"
#include <vcl_cmath.h>

const vnl_bignum vnl_numeric_traits<vnl_bignum>::zero = vnl_bignum(0L);
const vnl_bignum vnl_numeric_traits<vnl_bignum>::one = vnl_bignum(1L);
const vnl_bignum vnl_numeric_traits<vnl_bignum>::maxval = vnl_bignum("+Inf");
