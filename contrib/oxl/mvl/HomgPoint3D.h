// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------
#ifndef _HomgPoint3D_h
#define _HomgPoint3D_h
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------
//
// .NAME HomgPoint3D - Homogeneous 4-vector representing a 3D point.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPoint3D.h
// .FILE HomgPoint3D.cxx
//
// .SECTION Description:
//
// A class to hold a homogeneous 4-vector for a 3D point.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <vcl/vcl_iosfwd.h>

class HomgPoint3D : public Homg3D {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  HomgPoint3D ();   
  HomgPoint3D (const HomgPoint3D& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector<double>& p): Homg3D(p) {}
  HomgPoint3D (double x, double y, double z);
  HomgPoint3D (double x, double y, double z, double w);
  ~HomgPoint3D ();
  
  // Data Access-------------------------------------------------------------

  bool check_infinity() const;
  bool check_infinity(double tol) const;
  bool get_nonhomogeneous(double& x, double& y, double& z) const;
  vnl_double_3 get_double3() const;
  double radius() const { return get_double3().magnitude(); }
  double squared_radius() const { return get_double3().squared_magnitude(); }
  
  // Data Control------------------------------------------------------------
  
  // Utility Methods---------------------------------------------------------

  // Derived operator== is now working so this is not needed:
  //bool operator==(HomgPoint3D const& p) const { return Homg3D::operator==(p); }
  
  // INTERNALS---------------------------------------------------------------
  
protected:

private:
  
  // Data Members------------------------------------------------------------
  
private:
  
};

ostream& operator<<(ostream& s, const HomgPoint3D& );

#endif
// _HomgPoint3D_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
