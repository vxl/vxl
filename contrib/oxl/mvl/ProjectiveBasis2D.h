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
#ifndef ProjectiveBasis2D_h_
#define ProjectiveBasis2D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : ProjectiveBasis2D
//
// .SECTION Description
//    @{
//    Compute planar homography that maps 4 points into the canonical projective basis.
//    The returned H is such that
//    \[ H \mmatrix{4}{& & & \\ p_1 & p_2 & p_3 & p_4 \\  &  & & } \sim
//    \mmatrix{4}{1 & 0 & 0 & 1 \cr
//                0 & 1 & 0 & 1 \cr
//                0 & 0 & 1 & 1}
//    \] @}
//
// .NAME        ProjectiveBasis2D - Canonical basis of 4 points.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/ProjectiveBasis2D.h
// .FILE        ProjectiveBasis2D.h
// .FILE        ProjectiveBasis2D.C
// .EXAMPLE     ../Examples/exampleProjectiveBasis2D.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 26 Sep 96
//
// .SECTION Modifications:
//     Peter Vanroose - 3 dec 1998 - "collinear_" implemented
//
//-----------------------------------------------------------------------------

//////////////#include <cool/decls.h>
#include <mvl/HMatrix2D.h>

class HomgPoint2D;

class ProjectiveBasis2D {
public:
  // Constructors/Destructors--------------------------------------------------
  
  // tm_get_canonical_proj
  ProjectiveBasis2D(const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&);
  ProjectiveBasis2D(const vcl_vector<HomgPoint2D>&);
  ProjectiveBasis2D(const ProjectiveBasis2D& that);
 ~ProjectiveBasis2D();

  ProjectiveBasis2D& operator=(const ProjectiveBasis2D& that);

  // Operations----------------------------------------------------------------
  
  // Data Access---------------------------------------------------------------

// -- Return the planar homography that maps the points to the canonical frame.
  HMatrix2D& get_T() { return T_; }
  const vnl_double_3x3& get_T_matrix() const { return T_.get_matrix(); }

// -- Were three of the four given basis points collinear ?
  bool collinear() const { return collinear_; }
  
  // Data Control--------------------------------------------------------------

protected:
  // Computations--------------------------------------------------------------
  void compute(const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&);
  
  // Data Members--------------------------------------------------------------
  HMatrix2D T_;
  bool collinear_;
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS ProjectiveBasis2D.

