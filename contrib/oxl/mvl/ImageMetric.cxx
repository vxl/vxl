#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include "ImageMetric.h"

#include <vcl_iostream.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_math.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgOperator2D.h>

static vcl_ostream& warning(char const * fn) {
  return vcl_cerr << fn << " WARNING: ";
}

// - Default ctor
ImageMetric::ImageMetric()
{
}

// - Copy ctor
//ImageMetric::ImageMetric(const ImageMetric& that)
//{
//  operator=(that);
//}

// - Assignment
//ImageMetric& ImageMetric::operator=(const ImageMetric& )
//{
//  return *this;
//}

// - Virtual destructor
ImageMetric::~ImageMetric()
{
}

// TRANSFORMATIONS

//: Condition the 2D point p.
//  Default implementation is simply to return p in homogeneous coordinates
HomgPoint2D ImageMetric::image_to_homg(const vnl_double_2& p) const
{
  return HomgPoint2D(p.x(), p.y(), 1.0);
}

//: Condition 2D point (x,y)
HomgPoint2D ImageMetric::image_to_homg(double x, double y) const
{
  assert(!"ImageMetric::image_to_homg should be implemented for efficiency");
  return HomgPoint2D(x, y, 1.0);
}

//: Convert conditioned point p to image coordinates
vnl_double_2 ImageMetric::homg_to_image(const HomgPoint2D& p) const
{
  assert(!"ImageMetric::image_to_homg should be implemented for efficiency");
  double x,y;
  p.get_nonhomogeneous(x, y);
  return vnl_double_2(x, y);
}

//: Convert homogeneous point in image coordinates to one in conditioned coordinates
HomgPoint2D ImageMetric::imagehomg_to_homg(const HomgPoint2D& x) const
{
  return x;
}

//: Convert homogeneous point in conditioned coordinates to one in image coordinates
HomgPoint2D ImageMetric::homg_to_imagehomg(const HomgPoint2D& x) const
{
  return x;
}

//: Convert homogeneous line in conditioned coordinates to one in image coordinates
HomgLine2D ImageMetric::homg_to_image_line(const HomgLine2D& l) const
{
  if (is_linear())
    return HomgLine2D(get_C_inverse().transpose() * l.get_vector());

  // get points, decondition, and rejoin
  HomgPoint2D p1, p2;
  l.get_2_points_on_line(&p1, &p2);
  HomgPoint2D i1 = homg_to_imagehomg(p1);
  HomgPoint2D i2 = homg_to_imagehomg(p2);
  return HomgOperator2D::join(i1, i2);
}

HomgLine2D ImageMetric::image_to_homg_line(const HomgLine2D& l) const
{
  if (is_linear())
    return HomgLine2D(get_C().transpose() * l.get_vector());

  // get points, condition, and rejoin
  HomgPoint2D p1, p2;
  l.get_2_points_on_line(&p1, &p2);
  HomgPoint2D i1 = imagehomg_to_homg(p1);
  HomgPoint2D i2 = imagehomg_to_homg(p2);
  return HomgOperator2D::join(i1, i2);
}


//: Convert homogeneous line segment in conditioned coordinates to one in image coordinates
HomgLineSeg2D ImageMetric::homg_line_to_image(const HomgLineSeg2D& l) const
{
  // get points, decondition, and rejoin
  HomgPoint2D i1 = homg_to_imagehomg(l.get_point1());
  HomgPoint2D i2 = homg_to_imagehomg(l.get_point2());
  return HomgLineSeg2D(i1, i2);
}

HomgLineSeg2D ImageMetric::image_to_homg_line(const HomgLineSeg2D& l) const
{
  HomgPoint2D i1 = imagehomg_to_homg(l.get_point1());
  HomgPoint2D i2 = imagehomg_to_homg(l.get_point2());
  return HomgLineSeg2D(i1, i2);
}

// @{ MEASUREMENTS @}

//: Compute perpendicular distance in image coordinates between point p and line l, expressed in conditioned coordinates.
double ImageMetric::perp_dist_squared(const HomgPoint2D & p, const HomgLine2D & l) const
{
  return HomgOperator2D::perp_dist_squared(homg_to_imagehomg(p), homg_to_image_line(l));
}

//: Project point onto line.
HomgPoint2D ImageMetric::perp_projection(const HomgLine2D & l, const HomgPoint2D & p) const
{
  if (p.check_infinity()) {
    vcl_cerr << "ImageMetric::perp_projection -- point at infinity\n";
  }

  if (l.check_infinity()) {
    vcl_cerr << "ImageMetric::perp_projection -- line at infinity\n";
  }

  return HomgOperator2D::perp_projection(homg_to_image_line(l), homg_to_imagehomg(p));
}

//: Get perpendicular distance in image.
double ImageMetric::distance_squared(const HomgPoint2D & p1, const HomgPoint2D & p2) const
{
  return HomgOperator2D::distance_squared(homg_to_imagehomg(p1), homg_to_imagehomg(p2));
}

//: Get distance between a line segment and an infinite line.
//  The metric used is the maximum of the two endpoint perp distances.
double ImageMetric::distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const
{
  // ca_distance_squared_lineseg_to_line
  return vnl_math_max(this->perp_dist_squared(segment.get_point1(), line),
                       this->perp_dist_squared(segment.get_point2(), line));
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

// @{ MATRIX REPRESENTATION @}

//: Return true if the action of the conditioner can be represented as a planar homography.
bool ImageMetric::is_linear() const
{
  return false;
}

#include <vnl/vnl_identity_3x3.h>

static vnl_identity_3x3 I;

//: Return conditioning matrix C that converts homogeneous image points to homogeneous conditioned points.
//  If the ImageMetric used is nonlinear, then we'll have to make other arrangements...
const vnl_matrix<double>& ImageMetric::get_C() const
{
  warning("ImageMetric::get_C()") << "returning identity\n";
  return I;
}

//: Return conditioning matrix C that converts homogeneous conditioned points to image coords.
const vnl_matrix<double>& ImageMetric::get_C_inverse() const
{
  warning("ImageMetric::get_C_inverse()") << "returning identity\n";
  return I;
}

#include <mvl/FMatrix.h>

//: Convert FMatrix F expressed in conditioned frame to one that is valid in the image frame.
FMatrix ImageMetric::homg_to_image_deprecated(FMatrix const & F) const
{
  warning("ImageMetric::homg_to_image(FMatrix)") << "obsolete function, use HomgMetric::homg_to_image_F\n;";
  return F;
}

//: Convert FMatrix F expressed in image frame to one that is valid in the conditioned frame.
FMatrix ImageMetric::image_to_homg_deprecated(FMatrix const & F) const
{
  warning("ImageMetric::image_to_homg(FMatrix)") << "obsolete function, use HomgMetric::image_to_homg_F\n";
  return F;
}

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
