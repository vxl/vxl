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
//-*- c++ -*-------------------------------------------------------------------
#ifndef HomgPoint1D_h_
#define HomgPoint1D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        HomgPoint1D - Homogeneous 1D point
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgPoint1D.h
// .FILE        HomgPoint1D.h
// .FILE        HomgPoint1D.C
//
// .SECTION Description:
//
// HomgPoint1D represents a 1D point in homogeneous coordinates
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//
// .SECTION Modifications:
//     Peter Vanroose - 22 nov 98 - Constructor with 1/2 floats added
//
//-----------------------------------------------------------------------------

#include <mvl/Homg1D.h>

class HomgPoint1D : public Homg1D {
public:
  // Constructors/Destructors--------------------------------------------------

// -- constructors.
  HomgPoint1D() {}
  HomgPoint1D(const HomgPoint1D& that): Homg1D(that) {}
  HomgPoint1D(double px, double pw=1.0) : Homg1D(px,pw) {}
  HomgPoint1D(const vnl_vector<double>& vector_ptr): Homg1D(vector_ptr) {}

// -- Destructor
 ~HomgPoint1D() {}

// -- Assignment
  HomgPoint1D& operator=(const HomgPoint1D& that) { Homg1D::operator=(that); return *this; }

  // Operations----------------------------------------------------------------

// -- @{ Return nonhomogeneous form $x/w$.   If $w < infinitesimal\_tol$, return false @}
  bool get_nonhomogeneous(double& nonhomg) const;

// -- @{ Check $|w| < \mbox{tol} \times |x|$. @}
  bool check_infinity(double tol = infinitesimal_tol) const;
    
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgPoint1D.

