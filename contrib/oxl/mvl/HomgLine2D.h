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
//--------------------------------------------------------------
//
// .NAME HomgLine2D - Homogeneous 3-vector for a 2D line.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgLine2D.h
// .FILE HomgLine2D.cxx
//
// .SECTION Description:
//
// A class to hold a homogeneous 3-vector for a 2D line.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#ifndef _HomgLine2D_h
#define _HomgLine2D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_math.h>
#include <mvl/Homg2D.h>    

class HomgLineSeg2D;
class HomgPoint2D;

class HomgLine2D : public Homg2D {
  
  // PUBLIC INTERFACE----------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-------------------------------------
  
  HomgLine2D () {}
  HomgLine2D (const HomgLine2D& that): Homg2D(that) {}
  HomgLine2D (double px, double py, double pw): Homg2D (px, py, pw) {}
  explicit HomgLine2D (const vnl_vector<double>& vector_ptr): Homg2D (vector_ptr) {}
 ~HomgLine2D () {}

  HomgLine2D& operator=(const HomgLine2D& that)
  {
    Homg2D::operator=(that);
    return *this;
  }

  // Return true if exactly at infinity
  bool check_infinity() const;

  // Return true if @{min(|x|,|y|) < \mbox{tol} \times |z|@}
  bool check_infinity(double tol) const;

  // Clip the infinite line to the given bounding box and return
  HomgLineSeg2D clip(int x0, int y0, int x1, int y1) const;

  // Return 2 points on the line.
  void get_2_points_on_line(HomgPoint2D* p1, HomgPoint2D* p2) const;
  
  // INTERNALS-----------------------------------------------------------------

protected:

private:

  // Data Members-----------------------------------------------------------

private:

};

ostream& operator<<(ostream& s, const HomgLine2D& );

#endif
// _HomgLine2D_h

// Some emacs stuff, to insure c++-mode rather than c-mode:
// Local Variables:
// mode: c++
// End:
