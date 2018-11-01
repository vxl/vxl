// This is oxl/mvl/HomgMetric.h
#ifndef HomgMetric_h_
#define HomgMetric_h_
//:
// \file
// \brief Measurements on homogeneous coordinates
//
//    HomgMetric is a class that allows measurements to be made between
//    homogeneous primitives.  If attached to an ImageMetric (q.v.), uses
//    that, otherwise uses HomgOperator2D.
//
//    HomgMetric is an ImageMetric pointer, it behaves just like one, and
//    {\em does not} have responsibility for memory management, no more
//    than any other pointer.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Jan 97
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class ImageMetric;
class HomgPoint2D;
class HomgLine2D;
class HomgLineSeg2D;
class FMatrix;
class TriTensor;
class HMatrix2D;
class PMatrix;

class HomgMetric
{
  // Data Members--------------------------------------------------------------
  const ImageMetric* metric_;
 public:
  // Constructors/Destructors--------------------------------------------------

  HomgMetric() : metric_(nullptr) {}
  HomgMetric(const ImageMetric* metric);

  // HomgMetric(const HomgMetric& that); - use default
  ~HomgMetric();
  // HomgMetric& operator=(const HomgMetric& that); - use default

  // Operations----------------------------------------------------------------

  // ** Conversion to/from homogeneous coordinates
  vgl_point_2d<double> homg_to_image(vgl_homg_point_2d<double> const&) const;
  vnl_double_2 homg_to_image(const HomgPoint2D&) const;
  void homg_to_image(const HomgPoint2D&, double* ix, double* iy) const;
  void homg_to_image(vgl_homg_point_2d<double> const&, double& ix, double& iy) const;

  vgl_homg_point_2d<double> image_to_homg(vgl_point_2d<double> const&) const;
  HomgPoint2D image_to_homg(const vnl_double_2&) const;
  HomgPoint2D image_to_homg(double x, double y) const;

  HomgPoint2D homg_to_imagehomg(const HomgPoint2D&) const;
  HomgPoint2D imagehomg_to_homg(const HomgPoint2D&) const;

  vgl_homg_point_2d<double> homg_to_imagehomg(vgl_homg_point_2d<double> const&) const;
  vgl_homg_point_2d<double> imagehomg_to_homg(vgl_homg_point_2d<double> const&) const;

  HomgLine2D homg_to_image_line(const HomgLine2D&) const;
  HomgLine2D image_to_homg_line(const HomgLine2D&) const;

  vgl_homg_line_2d<double> homg_to_image_line(vgl_homg_line_2d<double> const&) const;
  vgl_homg_line_2d<double> image_to_homg_line(vgl_homg_line_2d<double> const&) const;

  HomgLineSeg2D image_to_homg_line(const HomgLineSeg2D&) const;
  HomgLineSeg2D homg_line_to_image(const HomgLineSeg2D&) const;

  // ** Measurements
  double perp_dist_squared(const HomgPoint2D&, const HomgLine2D&) const;
  double perp_dist_squared(vgl_homg_point_2d<double> const& p,
                           vgl_homg_line_2d<double> const& l) const;
  HomgPoint2D perp_projection(const HomgLine2D& l, const HomgPoint2D& p) const;
  vgl_homg_point_2d<double> perp_projection(vgl_homg_line_2d<double> const& l,
                                            vgl_homg_point_2d<double> const& p) const;

  double distance_squared(double x1, double y1, double x2, double y2) const;
  double distance_squared(const HomgPoint2D&, const HomgPoint2D&) const;
  double distance_squared(vgl_homg_point_2d<double> const&,
                          vgl_homg_point_2d<double> const&) const;
  double distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const;
  double distance_squared(vgl_line_segment_2d<double> const& segment,
                          vgl_homg_line_2d<double> const& line) const;

  bool is_within_distance(const HomgPoint2D&, const HomgPoint2D&, double distance) const;
  bool is_within_distance(vgl_homg_point_2d<double> const&,
                          vgl_homg_point_2d<double> const&,
                          double distance) const;

  // ** Speedups available for certain systems.
  bool is_linear() const;
  vnl_double_3x3 get_C() const;
  vnl_double_3x3 get_C_inverse() const;

  bool can_invert_distance() const;
  double image_to_homg_distance(double image_distance) const;
  double homg_to_image_distance(double homg_distance) const;

  double image_to_homg_distance_sqr(double image_distance) const;
  double homg_to_image_distance_sqr(double homg_distance) const;

  std::ostream& print(std::ostream&) const;

  operator const ImageMetric* () const { return metric_; }

  // Static functions to condition/decondition image relations-----------------
  static PMatrix homg_to_image_P(const PMatrix&, const HomgMetric& c);
  static PMatrix image_to_homg_P(const PMatrix&, const HomgMetric& c);

  static FMatrix homg_to_image_F(const FMatrix&, const HomgMetric& c1, const HomgMetric& c2);
  static FMatrix image_to_homg_F(const FMatrix&, const HomgMetric& c1, const HomgMetric& c2);

  static TriTensor homg_to_image_T(const TriTensor&, const HomgMetric& c1, const HomgMetric& c2, const HomgMetric& c3);
  static TriTensor image_to_homg_T(const TriTensor&, const HomgMetric& c1, const HomgMetric& c2, const HomgMetric& c3);

  static HMatrix2D homg_to_image_H(const HMatrix2D&, const HomgMetric& c1, const HomgMetric& c2);
  static HMatrix2D image_to_homg_H(const HMatrix2D&, const HomgMetric& c1, const HomgMetric& c2);
};

inline std::ostream& operator<<(std::ostream& s, const HomgMetric& h) { return h.print(s); }

#endif // HomgMetric_h_
