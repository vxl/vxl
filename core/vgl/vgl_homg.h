// This is core/vgl/vgl_homg.h
#ifndef vgl_homg_h_
#define vgl_homg_h_
//:
// \file
// \brief General purpose support class for vgl_homg_ classes
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_export.h>
#include <limits>
#include <cmath>

//: General purpose support class for vgl_homg_ classes
template <class T>
class vgl_homg
{
public:
  //: Standard placeholder for methods that wish to return infinity.
  static constexpr const T infinity{ std::numeric_limits<T>::infinity() };

  //: Standard way to test whether a number is indeed infinite.
  static bool
  is_infinity(T v)
  {
    return std::isinf(v);
  }

#ifndef VXL_LEGACY_REMOVE
  // Never used or properly implemented
  //: The tolerance used in "near zero" tests in the vgl_homg subclasses.
  static T infinitesimal_tol;

  //: Static method to set the default tolerance used for infinitesimal checks.
  // The default is 1e-12.
  static void
  set_infinitesimal_tol(T tol);
#endif
};

#endif // vgl_homg_h_
