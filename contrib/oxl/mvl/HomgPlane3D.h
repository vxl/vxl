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
#ifndef _HomgPlane3D_h
#define _HomgPlane3D_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME HomgPlane3D - Homogeneous 4-vector for a 3D plane.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPlane3D.h
// .FILE HomgPlane3D.C
//
// .SECTION Description:
//
// A class to hold a homogeneous 4-vector for a 3D plane.
//

#include <mvl/Homg3D.h>
#include <vcl/vcl_iosfwd.h>

class HomgPoint3D;

class HomgPlane3D : public Homg3D {

  // PUBLIC INTERFACE----------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-------------------------------------

  HomgPlane3D ();
  HomgPlane3D (double x, double y, double z, double w);
  HomgPlane3D (const vnl_vector<double>& n, double d);
  HomgPlane3D (const vnl_vector<double>& v): Homg3D(v) {}
  ~HomgPlane3D ();

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

  // Utility Methods-----------------------------------------------------------
  HomgPoint3D closest_point(const HomgPoint3D& x) const;
  double distance(const HomgPoint3D& x) const;

  vnl_vector<double> n() const { return _homg_vector.extract(3).normalize(); }

  // Derived operator== is now working so this is not needed:
  //bool operator==(HomgPlane3D const& p) const { return Homg3D::operator==(p); }

  // INTERNALS-----------------------------------------------------------------

protected:

private:

  // Data Members--------------------------------------------------------------

private:

};

ostream& operator<<(ostream&, const HomgPlane3D&);

#endif // _HomgPlane3D_h
