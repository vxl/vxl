// This is oxl/mvl/SimilarityMetric.cxx
//:
// \file

#include <iostream>
#include "SimilarityMetric.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

#include <vnl/vnl_double_2.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgOperator2D.h>

//: Default constructor sets parameters for an identity transformation.
SimilarityMetric::SimilarityMetric()
{
  centre_x_ = 0;
  centre_y_ = 0;
  inv_scale_ = 1;
  scale_ = 1;

  make_matrices();
}

//: Create a SimilarityMetric that transforms according to (x,y) -> (x - cx, y - cy) * scale
SimilarityMetric::SimilarityMetric(double cx, double cy, double scale)
{
  centre_x_ = cx;
  centre_y_ = cy;
  inv_scale_ = 1 / scale;
  scale_ = scale;

  make_matrices();
}

//: Create a SimilarityMetric that transforms coordinates in the range (0..xsize, 0..ysize) to the square (-1..1, -1..1)
SimilarityMetric::SimilarityMetric(int xsize, int ysize)
{
  set_from_rectangle(xsize, ysize);
}

//: Set transform to "(x,y) -> (x - xsize/2, y - ysize/2) / ((xsize+ysize)/2)"
void SimilarityMetric::set_from_rectangle(int xsize, int ysize)
{
  centre_x_ = xsize * 0.5;
  centre_y_ = ysize * 0.5;
  inv_scale_ = (xsize + ysize) * 0.5;
  scale_ = 1.0 / inv_scale_;

  make_matrices();
}

//: Set transform to "(x,y) -> (x - cx, y - cy) * scale"
// For example, (640 / 2, 480 / 2, 2.0 / (640 + 480))
void SimilarityMetric::set_center_and_scale(double cx, double cy, double scale)
{
  centre_x_ = cx;
  centre_y_ = cy;
  inv_scale_ = 1 / scale;
  scale_ = scale;

  make_matrices();
}

void SimilarityMetric::make_matrices()
{
  cond_matrix (0, 0) = inv_scale_;
  cond_matrix (0, 1) = 0;
  cond_matrix (0, 2) = centre_x_;

  cond_matrix (1, 0) = 0;
  cond_matrix (1, 1) = inv_scale_;
  cond_matrix (1, 2) = centre_y_;

  cond_matrix (2, 0) = 0;
  cond_matrix (2, 1) = 0;
  cond_matrix (2, 2) = 1.0;

  inv_cond_matrix (0, 0) = scale_;
  inv_cond_matrix (0, 1) = 0;
  inv_cond_matrix (0, 2) = -centre_x_ * scale_;

  inv_cond_matrix (1, 0) = 0;
  inv_cond_matrix (1, 1) = scale_;
  inv_cond_matrix (1, 2) = -centre_y_ * scale_;

  inv_cond_matrix (2, 0) = 0;
  inv_cond_matrix (2, 1) = 0;
  inv_cond_matrix (2, 2) = 1.0;
}

//: Destructor
SimilarityMetric::~SimilarityMetric() = default;

//: One line printout
void SimilarityMetric::print(char* msg) const
{
  std::cerr<<msg<<": SimilarityMetric ("<<centre_x_<<','<<centre_y_<<", "<<inv_scale_<<")\n";
}

//: One line printout
std::ostream& SimilarityMetric::print(std::ostream& s) const
{
  return s<<"[SimilarityMetric ("<<centre_x_<<','<<centre_y_<<"), "<<inv_scale_ << ']';
}

// IMPLEMENTATION OF SimilarityMetric

//: Convert 2D image point $(x,y)$ to homogeneous coordinates.
// The precise transformation is $(x,y) \rightarrow (x - cx, y - cy, f)$
vgl_homg_point_2d<double> SimilarityMetric::image_to_homg(vgl_point_2d<double> const& p) const
{
  double nx = p.x();
  double ny = p.y();

  // homogenize point
  return {nx - centre_x_, ny - centre_y_, inv_scale_};
}

//: Convert 2D point $(x,y)$ to homogeneous coordinates.
// The precise transformation is
// $(x,y) \rightarrow (x - cx, y - cy, f)$
HomgPoint2D SimilarityMetric::image_to_homg(double x, double y) const
{
  double nx = x;
  double ny = y;

  // homogenize point
  return HomgPoint2D(nx - centre_x_, ny - centre_y_, inv_scale_);
}

//: Convert conditioned point p to image coordinates
vgl_point_2d<double> SimilarityMetric::homg_to_image(vgl_homg_point_2d<double> const& p) const
{
  return {p.x()/p.w()*inv_scale_+centre_x_, p.y()/p.w()*inv_scale_+centre_y_};
}

//: Decondition homogeneous point.
vnl_double_2 SimilarityMetric::homg_to_image(const HomgPoint2D& p) const
{
  double x,y;
  p.get_nonhomogeneous(x, y);
  x = x * inv_scale_;
  y = y * inv_scale_;

  return {x + centre_x_, y + centre_y_};
}

//: Condition the 2D point p
HomgPoint2D SimilarityMetric::image_to_homg(const vnl_double_2& p) const
{
  return image_to_homg(p[0], p[1]);
}

//: Transform homogeneous point to image coordinates, leaving it in homogeneous form.
vgl_homg_point_2d<double> SimilarityMetric::homg_to_imagehomg(vgl_homg_point_2d<double> const& x) const
{
  return cond_matrix * x;
}

//: Transform homogeneous point to image coordinates, leaving it in homogeneous form.
HomgPoint2D SimilarityMetric::homg_to_imagehomg(const HomgPoint2D& x) const
{
  // ho_cam2std_aspect_point
  return HomgPoint2D(cond_matrix * x.get_vector());
}

//: Transform homogeneous point in image coordinates to conditioned coordinates.
vgl_homg_point_2d<double> SimilarityMetric::imagehomg_to_homg(vgl_homg_point_2d<double> const& x) const
{
  // ho_std2cam_aspect_point
  return inv_cond_matrix * x;
}

HomgPoint2D SimilarityMetric::imagehomg_to_homg(const HomgPoint2D& x) const
{
  // ho_std2cam_aspect_point
  return HomgPoint2D(inv_cond_matrix * x.get_vector());
}

//: Compute distance (in image coordinates) between points supplied in conditioned coordinates.
double SimilarityMetric::distance_squared(vgl_homg_point_2d<double> const& p1,
                                          vgl_homg_point_2d<double> const& p2) const
{
  double x1 = p1.x() / p1.w();
  double y1 = p1.y() / p1.w();

  double x2 = p2.x() / p2.w();
  double y2 = p2.y() / p2.w();

  return vnl_math::sqr (inv_scale_) * (vnl_math::sqr (x1 - x2) + vnl_math::sqr (y1 - y2));
}

//: Compute distance (in image coordinates) between points supplied in conditioned coordinates.
double SimilarityMetric::distance_squared(HomgPoint2D const& p1, HomgPoint2D const& p2) const
{
  // ho_triveccam_noaspect_distance_squared
  double x1 = p1.x() / p1.w();
  double y1 = p1.y() / p1.w();

  double x2 = p2.x() / p2.w();
  double y2 = p2.y() / p2.w();

  return vnl_math::sqr (inv_scale_) * (vnl_math::sqr (x1 - x2) + vnl_math::sqr (y1 - y2));
}

//: Get distance between a line segment and an infinite line.
//  The metric used is the maximum of the two endpoint perp distances.
double SimilarityMetric::distance_squared(vgl_line_segment_2d<double> const& segment,
                                          vgl_homg_line_2d<double> const& line) const
{
  return std::max(this->perp_dist_squared(vgl_homg_point_2d<double>(segment.point1()), line),
                      this->perp_dist_squared(vgl_homg_point_2d<double>(segment.point2()), line));
}

//: Get distance between a line segment and an infinite line.
//  The metric used is the maximum of the two endpoint perp distances.
double SimilarityMetric::distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const
{
  return std::max(this->perp_dist_squared(segment.get_point1(), line),
                      this->perp_dist_squared(segment.get_point2(), line));
}

//: Compute perpendicular distance (in image coordinates) from point to line (supplied in conditioned coordinates).
double SimilarityMetric::perp_dist_squared(vgl_homg_point_2d<double> const& p,
                                           vgl_homg_line_2d<double> const& l) const
{
  if (p.ideal()) {
    std::cerr << "SimilarityMetric::perp_dist_squared -- point at infinity\n";
    return Homg::infinity;
  }

  if (l.ideal()) {
    std::cerr << "SimilarityMetric::perp_dist_squared -- line at infinity\n";
    return Homg::infinity;
  }

  double numerator = vnl_math::sqr(p.x()*l.a()+p.y()*l.b()+p.w()*l.c());
  double denominator = (vnl_math::sqr(l.a()) + vnl_math::sqr(l.b())) * vnl_math::sqr(p.w() * scale_);

  return numerator / denominator;
}

double SimilarityMetric::perp_dist_squared(HomgPoint2D const & p, HomgLine2D const & l) const
{
  // ho_triveccam_aspect_perpdistance_squared

  // pcp separated
  if (p.ideal()) {
    std::cerr << "SimilarityMetric::perp_dist_squared -- point at infinity\n";
    return Homg::infinity;
  }

  if (l.ideal()) {
    std::cerr << "SimilarityMetric::perp_dist_squared -- line at infinity\n";
    return Homg::infinity;
  }

  double numerator = vnl_math::sqr(HomgOperator2D::dot(l, p));
  double denominator = (vnl_math::sqr(l.x()) + vnl_math::sqr(l.y()))
    * vnl_math::sqr(p.w() * scale_);

  return numerator / denominator;
}

//: Multiply all components of the transform matrix by "s".
// This routine is likely to be used only by programs which need
// special control over the exact form of their output.
void SimilarityMetric::scale_matrices(double s)
{
  cond_matrix *= s;
  inv_cond_matrix *= (1/s);
}

bool SimilarityMetric::can_invert_distance() const
{
  return true;
}

double SimilarityMetric::image_to_homg_distance(double image_distance) const
{
  return image_distance * scale_;
}

double SimilarityMetric::homg_to_image_distance(double image_distance) const
{
  return image_distance * inv_scale_;
}
