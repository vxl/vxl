// This is oxl/mvl/ImageMetric.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "ImageMetric.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_line_segment_2d.h>

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgOperator2D.h>

static vcl_ostream& warning(char const * fn)
{
  return vcl_cerr << fn << " WARNING: ";
}

// TRANSFORMATIONS

//: Condition the 2D point p.
//  Default implementation is simply to return p in homogeneous coordinates
vgl_homg_point_2d<double> ImageMetric::image_to_homg(vgl_point_2d<double> const& p) const
{
  assert(!"ImageMetric::image_to_homg should be implemented for efficiency");
  return vgl_homg_point_2d<double>(p.x(), p.y(), 1.0);
}

//: Condition the 2D point p.
//  Default implementation is simply to return p in homogeneous coordinates
HomgPoint2D ImageMetric::image_to_homg(const vnl_double_2& p) const
{
  assert(!"ImageMetric::image_to_homg should be implemented for efficiency");
  return HomgPoint2D(p[0], p[1], 1.0);
}

//: Condition 2D point (x,y)
HomgPoint2D ImageMetric::image_to_homg(double x, double y) const
{
  assert(!"ImageMetric::image_to_homg should be implemented for efficiency");
  return HomgPoint2D(x, y, 1.0);
}

//: Convert conditioned point p to image coordinates
vgl_point_2d<double> ImageMetric::homg_to_image(vgl_homg_point_2d<double> const& p) const
{
  assert(!"ImageMetric::homg_to_image should be implemented for efficiency");
  return p;
}
vnl_double_2 ImageMetric::homg_to_image(const HomgPoint2D& p) const
{
  assert(!"ImageMetric::homg_to_image should be implemented for efficiency");
  double x,y;
  p.get_nonhomogeneous(x, y);
  return vnl_double_2(x, y);
}

//: Convert homogeneous point in image coordinates to one in conditioned coordinates
vgl_homg_point_2d<double> ImageMetric::imagehomg_to_homg(vgl_homg_point_2d<double> const& x) const
{
  return x;
}
HomgPoint2D ImageMetric::imagehomg_to_homg(const HomgPoint2D& x) const
{
  return x;
}

//: Convert homogeneous point in conditioned coordinates to one in image coordinates
vgl_homg_point_2d<double> ImageMetric::homg_to_imagehomg(vgl_homg_point_2d<double> const& x) const
{
  return x;
}
HomgPoint2D ImageMetric::homg_to_imagehomg(const HomgPoint2D& x) const
{
  return x;
}

//: Convert homogeneous line in conditioned coordinates to one in image coordinates
vgl_homg_line_2d<double> ImageMetric::homg_to_image_line(vgl_homg_line_2d<double> const& l) const
{
  if (is_linear())
    return get_C_inverse().transpose() * l;

  // get points, decondition, and rejoin
  vgl_homg_point_2d<double> p1, p2;
  l.get_two_points(p1,p2);
  vgl_homg_point_2d<double> i1 = homg_to_imagehomg(p1);
  vgl_homg_point_2d<double> i2 = homg_to_imagehomg(p2);
  return vgl_homg_line_2d<double>(i1,i2);
}
HomgLine2D ImageMetric::homg_to_image_line(const HomgLine2D& l) const
{
  if (is_linear())
    return HomgLine2D(get_C_inverse().transpose() * l.get_vector().as_ref());

  // get points, decondition, and rejoin
  HomgPoint2D p1, p2;
  l.get_2_points_on_line(&p1, &p2);
  HomgPoint2D i1 = homg_to_imagehomg(p1);
  HomgPoint2D i2 = homg_to_imagehomg(p2);
  return HomgOperator2D::join(i1, i2);
}

vgl_homg_line_2d<double> ImageMetric::image_to_homg_line(const vgl_homg_line_2d<double>& l) const
{
  if (is_linear())
    return get_C().transpose() * l;

  // get points, condition, and rejoin
  vgl_homg_point_2d<double> p1, p2;
  l.get_two_points(p1,p2);
  vgl_homg_point_2d<double> i1 = imagehomg_to_homg(p1);
  vgl_homg_point_2d<double> i2 = imagehomg_to_homg(p2);
  return vgl_homg_line_2d<double>(i1,i2);
}
HomgLine2D ImageMetric::image_to_homg_line(const HomgLine2D& l) const
{
  if (is_linear())
    return HomgLine2D(get_C().transpose() * l.get_vector().as_ref());

  // get points, condition, and rejoin
  HomgPoint2D p1, p2;
  l.get_2_points_on_line(&p1, &p2);
  HomgPoint2D i1 = imagehomg_to_homg(p1);
  HomgPoint2D i2 = imagehomg_to_homg(p2);
  return HomgOperator2D::join(i1, i2);
}


//: Convert homogeneous line segment in conditioned coordinates to one in image coordinates
vgl_line_segment_2d<double> ImageMetric::homg_line_to_image(vgl_line_segment_2d<double> const& l) const
{
  // get points, decondition, and rejoin
  vgl_homg_point_2d<double> i1 = homg_to_imagehomg(vgl_homg_point_2d<double>(l.point1()));
  vgl_homg_point_2d<double> i2 = homg_to_imagehomg(vgl_homg_point_2d<double>(l.point2()));
  return vgl_line_segment_2d<double>(i1,i2);
}
HomgLineSeg2D ImageMetric::homg_line_to_image(const HomgLineSeg2D& l) const
{
  // get points, decondition, and rejoin
  HomgPoint2D i1 = homg_to_imagehomg(l.get_point1());
  HomgPoint2D i2 = homg_to_imagehomg(l.get_point2());
  return HomgLineSeg2D(i1, i2);
}

vgl_line_segment_2d<double> ImageMetric::image_to_homg_line(vgl_line_segment_2d<double> const& l) const
{
  vgl_homg_point_2d<double> i1 = imagehomg_to_homg(vgl_homg_point_2d<double>(l.point1()));
  vgl_homg_point_2d<double> i2 = imagehomg_to_homg(vgl_homg_point_2d<double>(l.point2()));
  return vgl_line_segment_2d<double>(i1,i2);
}
HomgLineSeg2D ImageMetric::image_to_homg_line(const HomgLineSeg2D& l) const
{
  HomgPoint2D i1 = imagehomg_to_homg(l.get_point1());
  HomgPoint2D i2 = imagehomg_to_homg(l.get_point2());
  return HomgLineSeg2D(i1, i2);
}

// == MEASUREMENTS ==

//: Compute perpendicular distance in image coordinates between point p and line l, expressed in conditioned coordinates.
double ImageMetric::perp_dist_squared(vgl_homg_point_2d<double> const& p,
                                      vgl_homg_line_2d<double> const& l) const
{
  return vgl_homg_operators_2d<double>::perp_dist_squared(homg_to_imagehomg(p), homg_to_image_line(l));
}
double ImageMetric::perp_dist_squared(const HomgPoint2D& p, const HomgLine2D& l) const
{
  return HomgOperator2D::perp_dist_squared(homg_to_imagehomg(p), homg_to_image_line(l));
}

//: Project point onto line.
vgl_homg_point_2d<double> ImageMetric::perp_projection(vgl_homg_line_2d<double> const& l,
                                                       vgl_homg_point_2d<double> const& p) const
{
  if (l.ideal())
    vcl_cerr << "ImageMetric::perp_projection -- line at infinity\n";

  return vgl_homg_operators_2d<double>::perp_projection(homg_to_image_line(l), homg_to_imagehomg(p));
}
HomgPoint2D ImageMetric::perp_projection(const HomgLine2D & l, const HomgPoint2D & p) const
{
  if (p.ideal()) {
    vcl_cerr << "ImageMetric::perp_projection -- point at infinity\n";
  }

  if (l.ideal()) {
    vcl_cerr << "ImageMetric::perp_projection -- line at infinity\n";
  }

  return HomgOperator2D::perp_projection(homg_to_image_line(l), homg_to_imagehomg(p));
}

//: Get perpendicular distance in image.
double ImageMetric::distance_squared(vgl_homg_point_2d<double> const& p1,
                                     vgl_homg_point_2d<double> const& p2) const
{
  return vgl_homg_operators_2d<double>::distance_squared(homg_to_imagehomg(p1), homg_to_imagehomg(p2));
}
double ImageMetric::distance_squared(const HomgPoint2D & p1, const HomgPoint2D & p2) const
{
  return HomgOperator2D::distance_squared(homg_to_imagehomg(p1), homg_to_imagehomg(p2));
}

//: Get distance between a line segment and an infinite line.
//  The metric used is the maximum of the two endpoint perp distances.
double ImageMetric::distance_squared(vgl_line_segment_2d<double> const& segment,
                                     vgl_homg_line_2d<double> const& line) const
{
  // ca_distance_squared_lineseg_to_line
  return vnl_math_max(this->perp_dist_squared(vgl_homg_point_2d<double>(segment.point1()), line),
                      this->perp_dist_squared(vgl_homg_point_2d<double>(segment.point2()), line));
}
double ImageMetric::distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const
{
  // ca_distance_squared_lineseg_to_line
  return vnl_math_max(this->perp_dist_squared(segment.get_point1(), line),
                      this->perp_dist_squared(segment.get_point2(), line));
}

bool ImageMetric::is_within_distance(vgl_homg_point_2d<double> const& p1,
                                     vgl_homg_point_2d<double> const& p2,
                                     double distance) const
{
  return distance_squared(p1, p2) < distance*distance;
}

bool ImageMetric::is_within_distance(const HomgPoint2D& p1, const HomgPoint2D& p2, double distance) const
{
  return distance_squared(p1, p2) < distance*distance;
}

//: Convert a distance in image coordinates to one in conditioned coordinates.
// This is only possible for similarity transformations, but where it does make
// sense it can mean significant increases in speed.
double ImageMetric::homg_to_image_distance(double image_distance) const
{
  warning("ImageMetric::invert_distance()") << "returning 0\n";
  return image_distance;
}

//: Convert a distance in image coordinates to one in conditioned coordinates.
// This is only possible for similarity transformations, but where it does make
// sense it can mean significant increases in speed.
double ImageMetric::image_to_homg_distance(double image_distance) const
{
  warning("ImageMetric::invert_distance()") << "returning 0\n";
  return image_distance;
}

//: Return true if the invert_distance function makes sense.
bool ImageMetric::can_invert_distance() const
{
  return false;
}

// == MATRIX REPRESENTATION ==

//: Return true if the action of the conditioner can be represented as a planar homography.
bool ImageMetric::is_linear() const
{
  return false;
}

#include <vnl/vnl_identity_3x3.h>

//: Return conditioning matrix C that converts homogeneous image points to homogeneous conditioned points.
//  If the ImageMetric used is nonlinear, then we'll have to make other arrangements...
vnl_double_3x3 ImageMetric::get_C() const
{
  static vnl_identity_3x3 I;
  warning("ImageMetric::get_C()") << "returning identity\n";
  return I;
}

//: Return conditioning matrix C that converts homogeneous conditioned points to image coordinates.
vnl_double_3x3 ImageMetric::get_C_inverse() const
{
  static vnl_identity_3x3 I;
  warning("ImageMetric::get_C_inverse()") << "returning identity\n";
  return I;
}

#include <mvl/FMatrix.h>

FMatrix ImageMetric::decondition(const FMatrix& F, const ImageMetric* c1, const ImageMetric* c2)
{
  if (!c1->is_linear())
    warning("ImageMetric::decondition(FMatrix...)") << "ImageMetric for image 1 is nonlinear\n";

  if (!c2->is_linear())
    warning("ImageMetric::decondition(FMatrix...)") << "ImageMetric for image 2 is nonlinear\n";

  vnl_double_3x3 C1inv = c1->get_C_inverse();
  vnl_double_3x3 C2inv = c2->get_C_inverse();
  return FMatrix( C1inv.transpose() * F.get_matrix() * C2inv );
}

vcl_ostream & ImageMetric::print(vcl_ostream& s) const {
  return s << "Empty ImageMetric";
}
