// This is oxl/mvl/SimilarityMetric.h
#ifndef SimilarityMetric_h_
#define SimilarityMetric_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <mvl/ImageMetric.h>
#include <vcl_iosfwd.h>
class Image;

class SimilarityMetric : public ImageMetric
{
  // Data Members--------------------------------------------------------------
  double centre_x_;
  double centre_y_;
  double inv_scale_;
  double scale_;

  vnl_matrix<double> cond_matrix;
  vnl_matrix<double> inv_cond_matrix;

 public:
  // Constructors/Destructors--------------------------------------------------

  SimilarityMetric();
  SimilarityMetric(int xsize, int ysize);
  SimilarityMetric(double cx, double cy, double scale);

  //  SimilarityMetric(const Image* ); -- don't want a dependency on ImageClasses
  // SimilarityMetric(const SimilarityMetric& that); - use default
  ~SimilarityMetric();
  // SimilarityMetric& operator=(const SimilarityMetric& that); - use default

  void set_from_rectangle(int xsize, int ysize);
  void set_center_and_scale(double cx, double cy, double scale);
  void scale_matrices(double s);

  // Operations----------------------------------------------------------------
  virtual HomgPoint2D image_to_homg(const vnl_double_2&) const;
  virtual HomgPoint2D image_to_homg(double x, double y) const;
  virtual vnl_double_2 homg_to_image(const HomgPoint2D&) const;
  virtual HomgPoint2D imagehomg_to_homg(const HomgPoint2D&) const;
  virtual HomgPoint2D homg_to_imagehomg(const HomgPoint2D&) const;

  virtual double perp_dist_squared(HomgPoint2D const& p, HomgLine2D const& l) const;
  virtual double distance_squared(HomgPoint2D const&, HomgPoint2D const&) const;
  virtual double distance_squared(HomgLineSeg2D const& segment, HomgLine2D const& line) const;

  virtual bool is_linear() const { return true; }
  virtual vnl_matrix<double> get_C() const { return cond_matrix; }
  virtual vnl_matrix<double> get_C_inverse() const { return inv_cond_matrix; }

  virtual bool can_invert_distance() const;
  virtual double image_to_homg_distance(double image_distance) const;
  virtual double homg_to_image_distance(double image_distance) const;

  // virtual bool can_invert_distance() const { return true; }

  // Data Control--------------------------------------------------------------
  vcl_ostream& print(vcl_ostream&) const;
  void print() const;
  void print(char* msg) const;

 private:
  // Helpers-------------------------------------------------------------------
  void make_matrices();
};

#endif // SimilarityMetric_h_
