// This is vxl/vgl/vgl_homg.h
#ifndef _vgl_homg_h
#define _vgl_homg_h
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Private base class for homogeneous vectors
//
// This is the private base class for homogeneous vectors.  It provides the
// get/set interface, and also a static variable vgl_homg::infinity which is used
// throughout when returning infinite nonhomogeneous values.
//
// \author
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// \verbatim
//  Modifications
//   210297 AWF Switched to fixed-length vectors for speed.
//   23 Oct 2001 - Peter Vanroose - made templated and ported to vgl
// \endverbatim
//
//-------------------------------------------------------------------------------

#include <vcl_cassert.h>

template <class T>
class vgl_homg {
public:

//: Standard placeholder for methods that wish to return infinity.
  static T infinity;

//: Standard way to test whether a number is indeed infinite.
  static bool is_infinity(T v) { return v == infinity; }

//: The tolerance used in "near zero" tests in the vgl_homg subclasses.
  static T infinitesimal_tol;

//: Static method to set the default tolerance used for infinitesimal checks.
// The default is 1e-12.
  static void set_infinitesimal_tol(T tol);
};

#endif // _vgl_homg_h
