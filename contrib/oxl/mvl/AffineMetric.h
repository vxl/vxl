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
#ifndef AffineMetric_h_
#define AffineMetric_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : AffineMetric
//
// .SECTION Description
//    AffineMetric is an ImageMetric that is an affine transformation.
//
// .NAME        AffineMetric - Affine ImageMetric
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/AffineMetric.h
// .FILE        AffineMetric.h
// .FILE        AffineMetric.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 24 Feb 97
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <mvl/ImageMetric.h>

class AffineMetric : public ImageMetric {
public:
  // Constructors/Destructors--------------------------------------------------
  
  AffineMetric();
  AffineMetric(const vnl_matrix<double>& A);

  // Operations----------------------------------------------------------------
  virtual HomgPoint2D image_to_homg(double x, double y);
  virtual vnl_double_2 homg_to_image(const HomgPoint2D& p);

  virtual HomgPoint2D homg_to_imagehomg(const HomgPoint2D& p);
  virtual HomgPoint2D imagehomg_to_homg(const HomgPoint2D& p);

  void set(vnl_matrix<double> const& A);
  void set(double a11, double a13, double a22, double a23, double a33);
  
  // Data Access---------------------------------------------------------------
  virtual const vnl_matrix<double>& get_C() const { return A_; }
  virtual const vnl_matrix<double>& get_C_inverse() const { return A_inverse_; }

  virtual bool is_linear() const { return true; }
  virtual bool can_invert_distance() const { return false; }
  
  // Computations--------------------------------------------------------------
  ostream& print(ostream& s) const;

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  vnl_double_3x3 A_;
  vnl_double_3x3 A_inverse_;
  
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS AffineMetric.

