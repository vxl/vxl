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
#ifdef __GNUG__
#pragma implementation
#endif
//--------------------------------------------------------------
//
// Class : HomgPlane3D
//
// Modifications : see HomgPlane3D.h
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
//#include <vcl/vcl_memory.h>
#include <vcl/vcl_cstdlib.h>
#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <mvl/HomgPlane3D.h>
#include <mvl/HomgPoint3D.h>


//--------------------------------------------------------------
// 
// -- Constructor
HomgPlane3D::HomgPlane3D ()
{
}

//--------------------------------------------------------------
// 
// -- Constructor
HomgPlane3D::HomgPlane3D (double x, double y, double z, double w)
    : Homg3D (x, y, z, w)
{
}

//--------------------------------------------------------------
// 
// -- Constructor
HomgPlane3D::HomgPlane3D (const vnl_vector<double>& n, double d)
  : Homg3D (n[0], n[1], n[2], -d)
{
}

//--------------------------------------------------------------
// 
// -- Destructor
HomgPlane3D::~HomgPlane3D ()
{
}

// -- closest point
HomgPoint3D HomgPlane3D::closest_point(const HomgPoint3D& x) const
{
  vnl_double_3 n(_homg_vector[0], _homg_vector[1], _homg_vector[2]);

  double s = 1.0/n.magnitude();
  double d = -_homg_vector[3];

  n *= s;
  d *= s;

  vnl_double_3 x3 = x.get_double3();

  double dp = dot_product(x3, n) - d;
  
  vnl_double_3 px = x3 - dp * n;

  return HomgPoint3D(px[0], px[1], px[2], 1.0);
}

// -- Distance point to plane
double HomgPlane3D::distance(const HomgPoint3D& x) const
{
  vnl_double_3 n(_homg_vector[0], _homg_vector[1], _homg_vector[2]);
  
  double s = 1.0/n.magnitude();
  double d = -_homg_vector[3];

  vnl_double_3 x3 = x.get_double3();

  return (dot_product(x3, n) - d)*s;
}

// -- print
ostream& operator<<(ostream& s, const HomgPlane3D& P)
{
  return s << P.get_vector();
}
