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
// Class : HomgPoint3D
//
// Modifications : see HomgPoint3D.h
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_memory.h>
#include <vcl/vcl_cstdlib.h>

////////////////#include <Geometry/IUPoint.h>
#include <mvl/Homg3D.h>
#include <mvl/HomgPoint3D.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

// - Constructor
HomgPoint3D::HomgPoint3D ()
{
}

// - Constructor
HomgPoint3D::HomgPoint3D (double x, double y, double z)
    : Homg3D (x, y, z, 1)
{
}

// -- Constructor. The homogeneous parameter w defaults to 1.
HomgPoint3D::HomgPoint3D (double x, double y, double z, double w)
    : Homg3D (x, y, z, w)
{
}

// - Destructor
HomgPoint3D::~HomgPoint3D ()
{
}


//--------------------------------------------------------------
//
// -- Return true if the point is at infinity
bool
HomgPoint3D::check_infinity(void) const
{
  return get_w() == 0;
}

//--------------------------------------------------------------
//
// -- Return true if the point is at infinity.
// Check that max(|x|, |y|, |z|) < tol * |w|
bool
HomgPoint3D::check_infinity(double tol) const
{
  double hx = fabs(get_x());
  double hy = fabs(get_y());
  double hz = fabs(get_z());
  double hw = fabs(get_w());
  
  double max = hx;
  if (hy > max) max = hy;
  if (hz > max) max = hz;
  
  return max < tol * hw;
}

//--------------------------------------------------------------
//
// -- Return the non-homogeneous coordinates of the point.
// If the point is at infinity, return false and set the
// output values to Homg::infinity.
bool
HomgPoint3D::get_nonhomogeneous(double& x, double& y, double& z) const
{
  double hx = get_x();
  double hy = get_y();
  double hz = get_z();
  double hw = get_w();
  if (hw == 0) {
    x = y = z = Homg::infinity;
    return false;
  }

  hw = 1.0/hw;
  x = hx * hw;
  y = hy * hw;
  z = hz * hw;

  return true;  
}

//--------------------------------------------------------------
//
// -- Return the non-homogeneous coordinates of the point as a vector of doubles
// If the point is at infinity, return a vector of Homg::infinity
vnl_double_3
HomgPoint3D::get_double3() const
{
  vnl_double_3 ret;
  get_nonhomogeneous(ret[0], ret[1], ret[2]);
  return ret;
}

//-----------------------------------------------------------------------------
//
// -- Print to ostream
ostream& operator<<(ostream& s, const HomgPoint3D& p)
{
  return s << "<HomgPoint3D " << p.get_vector() << ">";
}
