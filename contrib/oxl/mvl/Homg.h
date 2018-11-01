// This is oxl/mvl/Homg.h
#ifndef Homg_h_
#define Homg_h_
//:
// \file
// \brief Private base class for homogeneous vectors
//
// This is the private base class for homogeneous vectors.  It provides the
// get/set interface, and also a static variable Homg::infinity which is used
// throughout when returning infinite nonhomogeneous values.
//
// \author
//   Paul Beardsley, 29.03.96
//   Oxford University, UK
//
// \verbatim
//  Modifications:
//    210297 AWF Switched to fixed-length vectors for speed.
// \endverbatim
//-------------------------------------------------------------------------------

class Homg
{
 public:

  //: Standard placeholder for methods that wish to return infinity.
  static double infinity;

  //: The tolerance used in "near zero" tests in the Homg subclasses.
  static double infinitesimal_tol;

  //: Static method to set the default tolerance used for infinitesimal checks.
  // The default is 1e-12.
  static void set_infinitesimal_tol(double tol);
};

#endif // Homg_h_
