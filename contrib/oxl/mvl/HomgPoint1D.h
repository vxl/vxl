// This is oxl/mvl/HomgPoint1D.h
#ifndef HomgPoint1D_h_
#define HomgPoint1D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Homogeneous 1D point
//
// HomgPoint1D represents a 1D point in homogeneous coordinates
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//
// \verbatim
//  Modifications:
//     Peter Vanroose - 22 nov 98 - Constructor with 1/2 floats added
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/Homg1D.h>

class HomgPoint1D : public Homg1D
{
 public:
  // Constructors/Destructors--------------------------------------------------

//: constructors.
  HomgPoint1D() {}
  HomgPoint1D(const HomgPoint1D& that): Homg1D(that) {}
  HomgPoint1D(double px, double pw=1.0) : Homg1D(px,pw) {}
  HomgPoint1D(const vnl_vector<double>& vector_ptr): Homg1D(vector_ptr) {}

//: Destructor
 ~HomgPoint1D() {}

//: Assignment
  HomgPoint1D& operator=(const HomgPoint1D& that) { Homg1D::operator=(that); return *this; }

  // Operations----------------------------------------------------------------

//: @{ Return nonhomogeneous form $x/w$.   If $w < infinitesimal\_tol$, return false @}
  bool get_nonhomogeneous(double& nonhomg) const;

//: @{ Check $|w| < \mbox{tol} \times |x|$. @}
  bool check_infinity(double tol = infinitesimal_tol) const;
};

#endif // HomgPoint1D_h_
