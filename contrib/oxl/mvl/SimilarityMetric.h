// This is oxl/mvl/SimilarityMetric.h
#ifndef SimilarityMetric_h_
#define SimilarityMetric_h_
//:
// \file
// \brief Scale + translate ImageMetric
//
//    An ImageMetric that simply scales and translates.  Most often used
//    to condition points by transforming the image centre to the origin,
//    and scaling so that the diagonal has length 2.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Feb 97
//
// \verbatim
//  Modifications
//   22 Jun 2003 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_fwd.h>
#include <mvl/ImageMetric.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class Image;

class SimilarityMetric : public ImageMetric
{
  // Data Members--------------------------------------------------------------
  double centre_x_;
  double centre_y_;
  double inv_scale_;
  double scale_;

  vnl_double_3x3 cond_matrix;
  vnl_double_3x3 inv_cond_matrix;

 public:
  // Constructors/Destructors--------------------------------------------------

  SimilarityMetric();
  SimilarityMetric(int xsize, int ysize);
  SimilarityMetric(double cx, double cy, double scale);

  //  SimilarityMetric(const Image* ); -- don't want a dependency on ImageClasses
  // SimilarityMetric(const SimilarityMetric& that); - use default
  ~SimilarityMetric() override;
  // SimilarityMetric& operator=(const SimilarityMetric& that); - use default

  void set_from_rectangle(int xsize, int ysize);
  void set_center_and_scale(double cx, double cy, double scale);
  void scale_matrices(double s);

  // Operations----------------------------------------------------------------
  vgl_homg_point_2d<double> image_to_homg(vgl_point_2d<double> const&) const override;
  HomgPoint2D image_to_homg(const vnl_double_2&) const override;
  HomgPoint2D image_to_homg(double x, double y) const override;

  vgl_point_2d<double> homg_to_image(vgl_homg_point_2d<double> const&) const override;
  vnl_double_2 homg_to_image(const HomgPoint2D&) const override;

  HomgPoint2D imagehomg_to_homg(const HomgPoint2D&) const override;
  HomgPoint2D homg_to_imagehomg(const HomgPoint2D&) const override;
  vgl_homg_point_2d<double> imagehomg_to_homg(vgl_homg_point_2d<double> const&) const override;
  vgl_homg_point_2d<double> homg_to_imagehomg(vgl_homg_point_2d<double> const&) const override;

  double perp_dist_squared(HomgPoint2D const& p, HomgLine2D const& l) const override;
  double perp_dist_squared(vgl_homg_point_2d<double> const&,
                                   vgl_homg_line_2d<double> const&) const override;
  double distance_squared(const vgl_homg_point_2d<double>&, const vgl_homg_point_2d<double>&) const override;
  double distance_squared(vgl_line_segment_2d<double> const& segment,
                                  vgl_homg_line_2d<double> const& line) const override;
  double distance_squared(HomgPoint2D const&, HomgPoint2D const&) const override;
  double distance_squared(HomgLineSeg2D const& segment, HomgLine2D const& line) const override;

  bool is_linear() const override { return true; }
  vnl_double_3x3 get_C() const override { return cond_matrix; }
  vnl_double_3x3 get_C_inverse() const override { return inv_cond_matrix; }

  bool can_invert_distance() const override;
  double image_to_homg_distance(double image_distance) const override;
  double homg_to_image_distance(double image_distance) const override;

  // virtual bool can_invert_distance() const { return true; }

  // Data Control--------------------------------------------------------------
  std::ostream& print(std::ostream&) const override;
  void print() const;
  void print(char* msg) const;

 private:
  // Helpers-------------------------------------------------------------------
  void make_matrices();
};

#endif // SimilarityMetric_h_
