#ifndef HomgMetric_h_
#define HomgMetric_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    HomgMetric - Measurments on homogeneous coordinates
// .LIBRARY MViewBasics
// .HEADER  MultiView Package
// .INCLUDE mvl/HomgMetric.h
// .FILE    HomgMetric.cxx
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
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Jan 97
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

//
#include <vnl/vnl_fwd.h>
#include <vcl_iosfwd.h>

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

  HomgLineSeg2D image_to_homg_line(const HomgLineSeg2D&) const;
  HomgLineSeg2D homg_line_to_image(const HomgLineSeg2D&) const;

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

  vcl_ostream& print(vcl_ostream&) const;

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

inline vcl_ostream& operator<<(vcl_ostream& s, const HomgMetric& h) { return h.print(s); }

#endif // HomgMetric_h_
