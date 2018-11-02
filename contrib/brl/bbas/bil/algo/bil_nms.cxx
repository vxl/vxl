// This is brl/bbas/bil/algo/bil_nms.cxx
#include <cstdio>
#include <iostream>
#include <cmath>
#include "bil_nms.h"
//:
// \file

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

static const vil_image_view<double> img_d; // dummy local variables, used to initialise
static const vil_image_view<float>  img_f; // the three "reference" data members -PVr

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

//: default constructor
bil_nms::bil_nms()
: thresh_(0.0),
  parabola_fit_type_(bil_nms_params::PFIT_3_POINTS),
  margin_(1),
  rel_thresh_(2.5),
  use_adaptive_thresh_(true),
  dir_x_(img_d),   // do not initialise to "vil_image_view<double>(0,0,1)"
  dir_y_(img_d),   // since these 3 members are "const&", i.e., they reference
  grad_mag_(img_f),// data stored somewhere outside this class. -PVr
  x_(0,0, 0.0),
  y_(0,0, 0.0),
  dir_(0,0, 0.0),
  mag_(0,0), // there's no vil_image_view constructor to fill the image with zeroes; adding a third arg sets planestep!
  deriv_(0,0, 0.0)
{
}

//: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
bil_nms::bil_nms(const bil_nms_params& nsp, const vil_image_view<double>& dir_x,
                 const vil_image_view<double>& dir_y, const vil_image_view<float>& grad_mag)
: thresh_(nsp.thresh_),
  parabola_fit_type_(nsp.pfit_type_),
  margin_(nsp.margin_),
  rel_thresh_(nsp.rel_thresh_),
  use_adaptive_thresh_(nsp.use_adaptive_thresh_),
  dir_x_(dir_x),
  dir_y_(dir_y),
  grad_mag_(grad_mag),
  x_(grad_mag.nj(), grad_mag.ni(), 0.0),
  y_(grad_mag.nj(), grad_mag.ni(), 0.0),
  dir_(grad_mag.nj(), grad_mag.ni(), 0.0),
  mag_(grad_mag.ni(), grad_mag.nj()),
  deriv_(grad_mag.nj(), grad_mag.ni(), 0.0)
{
    mag_.fill(0.0f);
}

//: Apply the algorithm
void bil_nms::apply()
{
  double f[3], s_list[3];
  std::vector<vgl_point_2d<double> > loc;
  std::vector<double> orientation;
  std::vector<double> d2f;

  std::vector<vgl_point_2d<int> > pix_loc;
  // run non-maximum suppression at every point inside the margins
  assert(&grad_mag_ != &img_f); // should no longer be what it was initialised to
  for (unsigned x = margin_; x < grad_mag_.ni()-margin_; ++x) {
    for (unsigned y = margin_; y < grad_mag_.nj()-margin_; ++y)
    {
      if (grad_mag_(x,y) < thresh_) // threshold by gradient magnitude
        continue;
      assert(&dir_x_ != &img_d); // should no longer be what it was initialised to
      assert(&dir_y_ != &img_d); // should no longer be what it was initialised to
      double gx = dir_x_(x,y);
      double gy = dir_y_(x,y);
      vgl_vector_2d<double> direction(gx,gy);
      normalize(direction);

      // The gradient has to be non-degenerate
      if (std::abs(direction.x()) < 10e-6 && std::abs(direction.y()) < 10e-6)
        continue;

      // now compute the values orthogonal to the edge and fit a parabola
      int face_num = intersected_face_number(direction); assert(face_num != -1);
      double s = intersection_parameter(direction, face_num); assert(s != -1000);
      f_values(x, y, direction, s, face_num, f);
      s_list[0] = -s; s_list[1] = 0.0; s_list[2] = s;

      // Amir: removed this maximum check because there can be a max between f- and f+ even when f<f- or f<f+

      // fit a parabola to the data
      double max_val = f[1]; // default (should be updated by parabola fit)
      double grad_val = 0.0; // should be updated by parabola fit

      // compute location of extrema
      double s_star = (parabola_fit_type_ == bil_nms_params::PFIT_3_POINTS) ?
                          subpixel_s(s_list, f, max_val, grad_val) : subpixel_s(x, y, direction, max_val);
      if (std::fabs(s_star)< 0.7)
      {
        // record this edgel
        x_(y,x) = x + s_star * direction.x();
        y_(y,x) = y + s_star * direction.y();
        dir_(y,x) = std::atan2(direction.x(), -direction.y());
        mag_(x,y) = float(max_val); // the mag at the max of the parabola
        deriv_(y,x) = grad_val;
      }
    }
  }
}

int bil_nms::intersected_face_number(const vgl_vector_2d<double>& direction)
{
  if (direction.x() >= 0 && direction.y() >= 0)
  {
    if (direction.x() >= direction.y())
      return 1;
    else
      return 2;
  }
  else if (direction.x() < 0 && direction.y() >= 0)
  {
    if (std::abs(direction.x()) < direction.y())
      return 3;
    else
      return 4;
  }
  else if (direction.x() < 0 && direction.y() < 0)
  {
    if (std::abs(direction.x()) >= std::abs(direction.y()))
      return 5;
    else
      return 6;
  }
  else if (direction.x() >= 0 && direction.y() < 0)
  {
    if (direction.x() < std::abs(direction.y()))
      return 7;
    else
      return 8;
  }
  return -1;
}

double bil_nms::intersection_parameter(const vgl_vector_2d<double>& direction, int face_num)
{
  if (face_num == 1 || face_num == 8)
    return 1.0/direction.x();
  else if (face_num == 2 || face_num == 3)
    return 1.0/direction.y();
  else if (face_num == 4 || face_num == 5)
    return -1.0/direction.x();
  else if (face_num == 6 || face_num == 7)
    return -1.0/direction.y();
  return -1000.0;
}

void bil_nms::f_values(int x, int y, const vgl_vector_2d<double>& direction, double s, int face_num, double *f)
{
  int corners[4];
  get_relative_corner_coordinates(face_num, corners);

  vgl_vector_2d<double> intersection_point = s * direction;
  vgl_vector_2d<double> corner1(corners[0], corners[1]); // have to convert to double for subtraction
  vgl_vector_2d<double> corner2(corners[2], corners[3]); // have to convert to double for subtraction
  double distance1 = length(intersection_point - corner1);
  double distance2 = length(intersection_point - corner2);
  double value1 = grad_mag_(x+corners[0], y+corners[1]);
  double value2 = grad_mag_(x+corners[2], y+corners[3]);
  double f_plus = value1 * distance2 + value2 * distance1;

  intersection_point = -s * direction;
  corner1.set(-corners[0], -corners[1]); // have to convert to double for subtraction
  corner2.set(-corners[2], -corners[3]); // have to convert to double for subtraction
  distance1 = length(intersection_point - corner1);
  distance2 = length(intersection_point - corner2);
  value1 = grad_mag_(x-corners[0], y-corners[1]);
  value2 = grad_mag_(x-corners[2], y-corners[3]);
  double f_minus = value1 * distance2 + value2 * distance1;

  f[0] = f_minus;
  f[1] = grad_mag_(x,y);
  f[2] = f_plus;
}

void bil_nms::get_relative_corner_coordinates(int face_num, int *corners)
{
  switch (face_num)
  {
   case 1:
    corners[0] = 1;
    corners[1] = 0;
    corners[2] = 1;
    corners[3] = 1;
    break;
   case 2:
    corners[0] = 1;
    corners[1] = 1;
    corners[2] = 0;
    corners[3] = 1;
    break;
   case 3:
    corners[0] = 0;
    corners[1] = 1;
    corners[2] = -1;
    corners[3] = 1;
    break;
   case 4:
    corners[0] = -1;
    corners[1] = 1;
    corners[2] = -1;
    corners[3] = 0;
    break;
   case 5:
    corners[0] = -1;
    corners[1] = 0;
    corners[2] = -1;
    corners[3] = -1;
    break;
   case 6:
    corners[0] = -1;
    corners[1] = -1;
    corners[2] = 0;
    corners[3] = -1;
    break;
   case 7:
    corners[0] = 0;
    corners[1] = -1;
    corners[2] = 1;
    corners[3] = -1;
    break;
   case 8:
    corners[0] = 1;
    corners[1] = -1;
    corners[2] = 1;
    corners[3] = 0;
    break;
   default:
    corners[0] = 0;
    corners[1] = 0;
    corners[2] = 0;
    corners[3] = 0;
    break;
  }
}

double bil_nms::subpixel_s(const double *s, const double *f, double &max_f, double &max_d)
{
  // new version: assumes s[1]=0 and s[2]=-s[0]
  double A = -(f[1] - (f[0]+f[2])/2.0)/(s[2]*s[2]);
  double B = -(f[0]-f[2])/(2*s[2]);
  double C = f[1];

  double s_star = -B/(2*A);

  max_f = A*s_star*s_star + B*s_star + C;

  // second derivative
  double d2f = 2*A;
  max_d = d2f;

  if (A<0) { // make sure this is a maximum
    if (use_adaptive_thresh_) {
#if 0
      // derivatives at f+ and f-
      double d2fp = 2*A*s[2] + B;
      double d2fm = 2*A*s[0] + B;
      if (std::fabs(d2fp)>rel_thresh_ || std::fabs(d2fm)>rel_thresh_)
        return s_star;
#endif // 0
      if (d2f<-rel_thresh_) // d2f is always negative at a maximum
        return s_star;
      else
        return 5.0; // not reliable
    }
    else
      return s_star;
  }
  else
    return 5.0; // not a maximum
}

double bil_nms::subpixel_s(int x, int y, const vgl_vector_2d<double>& direction, double &max_f)
{
  double d;
  double s;
  double f;
  vgl_homg_point_2d<double> p1(0.0, 0.0);
  vgl_homg_point_2d<double> p2(direction.x(), direction.y());
  vgl_homg_line_2d<double> line1(p1,p2);
  // construct the matrices
  vnl_matrix<double> A(9, 3);
  vnl_matrix<double> B(9, 1);
  vnl_matrix<double> P(3, 1);
  int index = 0;
  for (int j = -1; j <= 1; ++j)
  {
    for (int i = -1; i <= 1; ++i)
    {
      find_distance_s_and_f_for_point(i, j, line1, d, s, direction);
      f = grad_mag_(x+i,y+j);
      A(index, 0) = std::pow(s,2.0);
      A(index, 1) = s;
      A(index, 2) = 1.0;
      B(index, 0) = f;
      ++index;
    }
  }
#if 0
  vnl_matrix<double> A_trans = A.transpose();
  vnl_matrix<double> temp = vnl_matrix_inverse<double> (A_trans*A);
  vnl_matrix<double> temp2 = temp * A_trans;
  P = temp2 * B;
#endif // 0
  vnl_svd<double> svd(A);
  P = svd.solve(B);
  double s_star = -P(1,0)/(2*P(0,0));

  max_f = P(0,0)*s_star*s_star + P(1,0)*s_star + P(2,0);

  if (P(0,0)<0)
    return s_star;
  else
    return 5.0; // not a maximum
}

void bil_nms::find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                              double &d, double &s, const vgl_vector_2d<double>& direction)
{
  vgl_homg_point_2d<double> point(x,y);
  vgl_homg_line_2d<double> perp_line = vgl_homg_operators_2d<double>::perp_line_through_point(line, point);
  vgl_homg_point_2d<double> intersection_point_homg = vgl_homg_operators_2d<double>::intersection(line, perp_line);
  vgl_point_2d<double> intersection_point(intersection_point_homg);
  vgl_vector_2d<double> d_helper(x-intersection_point.x(), y-intersection_point.y());
  d = length(d_helper);
  s = intersection_point.x() / direction.x();
}

//----------------------------------------------------------
//: Clear internal storage
//
void bil_nms::clear()
{
  mag_.clear();
}
