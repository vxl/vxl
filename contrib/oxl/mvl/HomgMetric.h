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
#ifndef HomgMetric_h_
#define HomgMetric_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : HomgMetric
//
// .SECTION Description
//    HomgMetric is a class that allows measurements to be made between
//    homogeneous primitives.  If attached to an ImageMetric (q.v.), uses
//    that, otherwise uses HomgOperator2D.
//
//    HomgMetric is an ImageMetric pointer, it behaves just like one, and
//    {\em does not} have responsibility for memory management, no more
//    than any other pointer.
//
// .NAME        HomgMetric - Measurments on homogeneous coordinates
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgMetric.h
// .FILE        HomgMetric.h
// .FILE        HomgMetric.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Jan 97
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

//
#include <vnl/vnl_fwd.h>
#include <vcl/vcl_iosfwd.h>

class vnl_double_2;
class ImageMetric;
class HomgPoint2D;
class HomgLine2D;
class HomgLineSeg2D;
class FMatrix;
class TriTensor;
class HMatrix2D;
class PMatrix;

class HomgMetric {
public:
  // Constructors/Destructors--------------------------------------------------
  
  HomgMetric() { _metric = 0; }
  HomgMetric(const ImageMetric* metric);

  // HomgMetric(const HomgMetric& that); - use default
  ~HomgMetric();
  // HomgMetric& operator=(const HomgMetric& that); - use default

  // Operations----------------------------------------------------------------
  
  // ** Conversion to/from homogeneous coordinates
  vnl_double_2 homg_to_image(const HomgPoint2D&) const;
  void homg_to_image(const HomgPoint2D&, double* ix, double* iy) const;
  HomgPoint2D image_to_homg(const vnl_double_2&) const;
  HomgPoint2D image_to_homg(double x, double y) const;

  HomgPoint2D homg_to_imagehomg(const HomgPoint2D&) const;
  HomgPoint2D imagehomg_to_homg(const HomgPoint2D&) const;

  HomgLine2D homg_to_image_line(const HomgLine2D&) const;
  HomgLine2D image_to_homg_line(const HomgLine2D&) const;

  HomgLineSeg2D image_to_homg(const HomgLineSeg2D&) const;
  HomgLineSeg2D homg_to_image(const HomgLineSeg2D&) const;  

  // ** Measurements
  double perp_dist_squared(const HomgPoint2D&, const HomgLine2D&) const;
  HomgPoint2D perp_projection(const HomgLine2D& l, const HomgPoint2D& p) const;
  double distance_squared(const HomgPoint2D&, const HomgPoint2D&) const;
  double distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const;

  bool is_within_distance(const HomgPoint2D&, const HomgPoint2D&, double distance) const;

  // ** Speedups available for certain systems.
  bool is_linear() const;
  const vnl_matrix<double>& get_C() const;
  const vnl_matrix<double>& get_C_inverse() const;

  bool can_invert_distance() const;
  double image_to_homg_distance(double image_distance) const;
  double homg_to_image_distance(double homg_distance) const;

  double image_to_homg_distance_sqr(double image_distance) const;
  double homg_to_image_distance_sqr(double homg_distance) const;

  ostream& print(ostream&) const;

  operator const ImageMetric* () const { return _metric; }

  // Static functions to condition/decondition image relations-----------------
  static PMatrix homg_to_image_P(const PMatrix&, const HomgMetric& c);
  static PMatrix image_to_homg_P(const PMatrix&, const HomgMetric& c);

  static FMatrix homg_to_image_F(const FMatrix&, const HomgMetric& c1, const HomgMetric& c2);
  static FMatrix image_to_homg_F(const FMatrix&, const HomgMetric& c1, const HomgMetric& c2);

  static TriTensor homg_to_image_T(const TriTensor&, const HomgMetric& c1, const HomgMetric& c2, const HomgMetric& c3);
  static TriTensor image_to_homg_T(const TriTensor&, const HomgMetric& c1, const HomgMetric& c2, const HomgMetric& c3);

  static HMatrix2D homg_to_image_H(const HMatrix2D&, const HomgMetric& c1, const HomgMetric& c2);
  static HMatrix2D image_to_homg_H(const HMatrix2D&, const HomgMetric& c1, const HomgMetric& c2);

protected:
  // Data Members--------------------------------------------------------------
  ImageMetric* _metric;

private:
  // Helpers-------------------------------------------------------------------
};

inline ostream& operator<<(ostream& s, const HomgMetric& h) { return h.print(s); }

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgMetric.
