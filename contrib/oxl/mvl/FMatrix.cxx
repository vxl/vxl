// This is oxl/mvl/FMatrix.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "FMatrix.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vul/vul_printf.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_transpose.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_rpoly_roots.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <mvl/HomgOperator2D.h>
#include <mvl/PMatrix.h>

//--------------------------------------------------------------
//
//: Default constructor.
// Sets matrices to size 3x3, zero-filled.

FMatrix::FMatrix()
{
  rank2_flag_ = false;
}

//--------------------------------------------------------------
//
//: Constructor.  Load from vcl_istream.

FMatrix::FMatrix(vcl_istream& f)
{
  rank2_flag_ = false;
  read_ascii(f);
}

//--------------------------------------------------------------
//
//: Constructor.

FMatrix::FMatrix(const double *f_matrix)
{
  rank2_flag_ = false;
  set(f_matrix);
}

//--------------------------------------------------------------
//
//: Constructor

FMatrix::FMatrix(const vnl_matrix<double>& f_matrix)
{
  rank2_flag_ = false;
  set(f_matrix);
}


//--------------------------------------------------------------
//
//: Construct from two P matrices

FMatrix::FMatrix(const PMatrix& P1, const PMatrix& P2)
{
  rank2_flag_ = false;
  set(P1, P2);
}

//--------------------------------------------------------------
//
//: Construct from one P matrix, the other is assumed to be [I 0].

FMatrix::FMatrix(const PMatrix& P2)
{
  rank2_flag_ = false;
  set(P2);
}

//--------------------------------------------------------------
//
//: Destructor
FMatrix::~FMatrix()
{
}

//---------------------------------------------------------------
//: Read from ASCII vcl_istream
bool FMatrix::read_ascii(vcl_istream& s)
{
  s >> f_matrix_;
  if (!(s.good() || s.eof()))
    return false;

  ft_matrix_ = f_matrix_.transpose();
  rank2_flag_ = false;
  set_rank2_using_svd();

  return true;
}

FMatrix FMatrix::read(char const* filename)
{
  vcl_ifstream f(filename);
  FMatrix F;
  if (!F.read_ascii(f))
    vcl_cerr << "FMatrix: Error reading from [" << filename << "]\n";
  return F;
}

//---------------------------------------------------------------
//: Read from ASCII vcl_istream
vcl_istream& operator>>(vcl_istream& s, FMatrix& F)
{
  F.read_ascii(s);
  return s;
}

//---------------------------------------------------------------
//: Read from ASCII vcl_istream
FMatrix FMatrix::read(vcl_istream& s)
{
  return FMatrix(s);
}

// == OPERATORS ==

//--------------------------------------------------------------
//
//: Return the epipolar line  $l_1$ in image 1: $l_1 = F^\top x_2$
vgl_homg_line_2d<double> FMatrix::image1_epipolar_line(const vgl_homg_point_2d<double>& x2) const
{
  vgl_homg_point_2d<double> p = ft_matrix_ * x2;
  return vgl_homg_line_2d<double>(p.x(),p.y(),p.w()); // dual point
}

//--------------------------------------------------------------
//
//: Return the epipolar line  $l_1$ in image 1: $l_1 = F^\top x_2$
HomgLine2D FMatrix::image1_epipolar_line(const HomgPoint2D& x2) const
{
  return HomgLine2D(ft_matrix_ * x2.get_vector());
}

//----------------------------------------------------------------
//
//: Return the epipolar line $l_2$ in image 2: $l_2 = F x_1$

vgl_homg_line_2d<double> FMatrix::image2_epipolar_line(const vgl_homg_point_2d<double>& x1) const
{
  vgl_homg_point_2d<double> p = f_matrix_ * x1;
  return vgl_homg_line_2d<double>(p.x(),p.y(),p.w()); // dual point
}

//----------------------------------------------------------------
//
//: Return the epipolar line $l_2$ in image 2: $l_2 = F x_1$

HomgLine2D FMatrix::image2_epipolar_line(const HomgPoint2D& x1) const
{
  return HomgLine2D(f_matrix_ * x1.get_vector());
}

//-----------------------------------------------------------------
//
// For the specified match, return the perpendicular distance squared
// between the epipolar line and the point, in image 1.

double
FMatrix::image1_epipolar_distance_squared(vgl_homg_point_2d<double> const& p1,
                                          vgl_homg_point_2d<double> const& p2) const
{
  vgl_homg_line_2d<double> epipolar_line = image1_epipolar_line (p2);
  return vgl_homg_operators_2d<double>::perp_dist_squared (p1, epipolar_line);
}

//-----------------------------------------------------------------
//
// For the specified match, return the perpendicular distance squared
// between the epipolar line and the point, in image 1.

double
FMatrix::image1_epipolar_distance_squared(HomgPoint2D *point1_ptr,
                                          HomgPoint2D *point2_ptr) const
{
  HomgLine2D epipolar_line = image1_epipolar_line (*point2_ptr);
  return HomgOperator2D::perp_dist_squared (*point1_ptr, epipolar_line);
}

//-------------------------------------------------------------------
//
// For the specified match, return the perpendicular distance squared
// between the epipolar line and the point, in image 2.

double
FMatrix::image2_epipolar_distance_squared(vgl_homg_point_2d<double> const& p1,
                                          vgl_homg_point_2d<double> const& p2) const
{
  vgl_homg_line_2d<double> epipolar_line = image2_epipolar_line(p1);
  return vgl_homg_operators_2d<double>::perp_dist_squared(p2, epipolar_line);
}

//-------------------------------------------------------------------
//
// For the specified match, return the perpendicular distance squared
// between the epipolar line and the point, in image 2.

double
FMatrix::image2_epipolar_distance_squared(HomgPoint2D *point1_ptr,
                                          HomgPoint2D *point2_ptr) const
{
  HomgLine2D epipolar_line = image2_epipolar_line (*point1_ptr);
  return HomgOperator2D::perp_dist_squared (*point2_ptr, epipolar_line);
}

//---------------------------------------------------------------
//: Print to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& os, const FMatrix& F)
{
  const vnl_double_3x3& m = F.get_matrix();
  for (unsigned long i = 0; i < m.rows(); i++) {    // For each row in matrix
    for (unsigned long j = 0; j < m.columns(); j++) // For each column in matrix
      vul_printf(os, "%24.16e ", m(i,j));           // Output data element
    os << '\n';                                     // Output newline
  }
  return os;
}

// == COMPUTATIONS ==
//-------------------------------------------------------------------

//: Return an FMatrix which corresponds to the reverse of this one.
FMatrix FMatrix::transpose() const
{
  return FMatrix(ft_matrix_);
}

//: Compute the epipoles (left and right nullspaces of F) using vnl_svd<double>.
// Return false if the rank of F is not 2, and set approximate epipoles,
// (the left and right singular vectors corresponding to the smallest
// singular value of F).

bool
FMatrix::get_epipoles(vgl_homg_point_2d<double>& epipole1,
                      vgl_homg_point_2d<double>& epipole2) const
{
  // fm_compute_epipoles
  vnl_svd<double> svd(f_matrix_);
  vnl_double_3 v = svd.nullvector();
  epipole1.set(v[0],v[1],v[2]);
  v = svd.left_nullvector();
  epipole2.set(v[0],v[1],v[2]);
  return svd.W(2,2) == 0;
}

//: Compute the epipoles (left and right nullspaces of F) using vnl_svd<double>.
// Return false if the rank of F is not 2, and set approximate epipoles,
// (the left and right singular vectors corresponding to the smallest
// singular value of F).

bool
FMatrix::get_epipoles(HomgPoint2D*epipole1_ptr, HomgPoint2D*epipole2_ptr) const
{
  // fm_compute_epipoles
  vnl_svd<double> svd(f_matrix_);
  epipole1_ptr->set(svd.nullvector());
  epipole2_ptr->set(svd.left_nullvector());
  return svd.W(2,2) == 0;
}


//-----------------------------------------------------------------------------
//
//: Find nearest match which agrees with F.
// For a specified pair of matching points, find the nearest (minimum sum
// of squared image distances) match which is in perfect agreement with
// the epipolar geometry of the F matrix.
// (see R.I. Hartley and P. Sturm, ``Triangulation''. In
// {\em Proceedings, Computer Analysis of Images and Patterns},
// Prague, 1995).

void
FMatrix::find_nearest_perfect_match(vgl_homg_point_2d<double> const& point1,
                                    vgl_homg_point_2d<double> const& point2,
                                    vgl_homg_point_2d<double>& perfect_point1,
                                    vgl_homg_point_2d<double>& perfect_point2) const
{
  vgl_homg_point_2d<double> epipole1, epipole2;
  get_epipoles(epipole1, epipole2);

  find_nearest_perfect_match(point1, point2, epipole1, epipole2, perfect_point1, perfect_point2);
}

//-----------------------------------------------------------------------------
//
//: Find nearest match which agrees with F.
// For a specified pair of matching points, find the nearest (minimum sum
// of squared image distances) match which is in perfect agreement with
// the epipolar geometry of the F matrix.
// (see R.I. Hartley and P. Sturm, ``Triangulation''. In
// {\em Proceedings, Computer Analysis of Images and Patterns},
// Prague, 1995).

void
FMatrix::find_nearest_perfect_match(const HomgPoint2D& point1,
                                    const HomgPoint2D& point2,
                                    HomgPoint2D *perfect_point1_ptr,
                                    HomgPoint2D *perfect_point2_ptr) const
{
  HomgPoint2D epipole1, epipole2;
  get_epipoles(&epipole1, &epipole2);

  find_nearest_perfect_match(point1, point2, epipole1, epipole2, perfect_point1_ptr, perfect_point2_ptr);
}

//: Faster Hartley-Sturm using precomputed epipoles
void
FMatrix::find_nearest_perfect_match(vgl_homg_point_2d<double> const& point1,
                                    vgl_homg_point_2d<double> const& point2,
                                    vgl_homg_point_2d<double> const& epipole1,
                                    vgl_homg_point_2d<double> const& epipole2,
                                    vgl_homg_point_2d<double>& perfect_point1,
                                    vgl_homg_point_2d<double>& perfect_point2) const
{
  vgl_homg_line_2d<double> line_horiz(0,1,0);

  vgl_homg_line_2d<double> line1 = vgl_homg_operators_2d<double>::join_oriented(point1, epipole1);
  double angle1 = vgl_homg_operators_2d<double>::angle_between_oriented_lines (line1, line_horiz);

  vgl_homg_line_2d<double> line2 = vgl_homg_operators_2d<double>::join_oriented(point2, epipole2);
  double angle2 = vgl_homg_operators_2d<double>::angle_between_oriented_lines (line2, line_horiz);

  // If the transformation from the transformed frame to the raw image frame is Pi, i=1,2, then
  // the transformed F matrix is P2^T F P1.

  double x1 = point1.x()/point1.w(), y1 = point1.y()/point1.w(),
         x2 = point2.x()/point2.w(), y2 = point2.y()/point2.w();

  // c s x
  //-s c y
  // 0 0 1
  vnl_double_3x3 p1_matrix;
  p1_matrix(0, 0) = vcl_cos (angle1);
  p1_matrix(0, 1) = vcl_sin (angle1);
  p1_matrix(0, 2) = x1;
  p1_matrix(1, 0)= -vcl_sin (angle1);
  p1_matrix(1, 1) = vcl_cos (angle1);
  p1_matrix(1, 2) = y1;
  p1_matrix(2, 0) = 0;
  p1_matrix(2, 1) = 0;
  p1_matrix(2, 2) = 1;

  vnl_double_3x3 p2_matrix;
  p2_matrix(0, 0) = vcl_cos (angle2);
  p2_matrix(0, 1) = vcl_sin (angle2);
  p2_matrix(0, 2) = x2;
  p2_matrix(1, 0)= -vcl_sin (angle2);
  p2_matrix(1, 1) = vcl_cos (angle2);
  p2_matrix(1, 2) = y2;
  p2_matrix(2, 0) = 0;
  p2_matrix(2, 1) = 0;
  p2_matrix(2, 2) = 1;

  vnl_double_3x3 special_f_matrix= vnl_transpose(p2_matrix) *f_matrix_ *p1_matrix;

  double f = -special_f_matrix(1, 0) / special_f_matrix(1, 2);
  double f2 = -special_f_matrix(2, 0) / special_f_matrix(2, 2);
  double g = -special_f_matrix(0, 1) / special_f_matrix(2, 1);
  double g2 = -special_f_matrix(0, 2) / special_f_matrix(2, 2);
  if (vcl_fabs ((f-f2) / f) > 0.05 || vcl_fabs ((g-g2) / g) > 0.05)
    vcl_cerr << "F matrix isn't rank 2.\n";

  // section 4.2 of the paper.

  double a = special_f_matrix(1, 1);
  double b = special_f_matrix(1, 2);
  double c = special_f_matrix(2, 1);
  double d = special_f_matrix(2, 2);

  // generated from equation (6) in the paper, using mathematica.
  vnl_vector<double> coeffs(7);
  coeffs[0] = b*b*c*d - a*b*d*d;
  coeffs[1] = b*b*b*b + b*b*c*c - a*a*d*d + 2.0*b*b*d*d*g*g + d*d*d*d*g*g*g*g;
  coeffs[2] = 4.0*a*b*b*b + a*b*c*c - a*a*c*d + 2.0*b*b*c*d*f*f - 2.0*a*b*d*d*f*f
              + 4.0*b*b*c*d*g*g + 4.0*a*b*d*d*g*g + 4.0*c*d*d*d*g*g*g*g;
  coeffs[3] = 6.0*a*a*b*b + 2.0*b*b*c*c*f*f - 2.0*a*a*d*d*f*f + 2.0*b*b*c*c*g*g
              + 8*a*b*c*d*g*g + 2.0*a*a*d*d*g*g + 6.0*c*c*d*d*g*g*g*g;
  coeffs[4] = 4.0*a*a*a*b + 2.0*a*b*c*c*f*f - 2.0*a*a*c*d*f*f + b*b*c*d*f*f*f*f
              - a*b*d*d*f*f*f*f + 4.0*a*b*c*c*g*g + 4.0*a*a*c*d*g*g + 4.0*c*c*c*d*g*g*g*g;
  coeffs[5] = a*a*a*a + b*b*c*c*f*f*f*f - a*a*d*d*f*f*f*f + 2.0*a*a*c*c*g*g + c*c*c*c*g*g*g*g;
  coeffs[6] = a*b*c*c*f*f*f*f - a*a*c*d*f*f*f*f;

  vnl_rpoly_roots roots(coeffs);

  // Hartley mentions the special case of t=infinity in his paper
  // i.e. the corrected point is at the epipole. not implemented here...

  bool real_root_flag = false;
  double s_min = 1e20;
  double t_min = 0;
  for (int root_index = 0; root_index < 6; root_index++)
    if (roots.imag(root_index) == 0) {
      // equation (4) in the paper.
      double t = roots.real(root_index);
      double s = t * t / vnl_math_sqr(1.0 + f * f * t * t) +
        vnl_math_sqr(c * t + d) /
        (vnl_math_sqr(a * t + b) + g * g * vnl_math_sqr(c * t + d));
      if (s < s_min) {
        real_root_flag = true;
        t_min = t;
        s_min = s;
      }
    }

  if (!real_root_flag) {
    vcl_cerr << "FMatrix::find_nearest_perfect_match -- no real root\n";
    return;
  }

  // if (residual_sum_squared_ptr) *residual_sum_squared_ptr = s_min;

  // the epipolar lines in the two images.
  vgl_homg_line_2d<double> epipolar_line1(t_min * f, 1, -t_min);
  vgl_homg_line_2d<double> epipolar_line2(-g * (c*t_min + d), a*t_min + b, c*t_min + d);
  vgl_homg_point_2d<double> origin(0,0,1);

  perfect_point1 = p1_matrix * vgl_homg_operators_2d<double>::perp_projection(epipolar_line1, origin);
  perfect_point2 = p2_matrix * vgl_homg_operators_2d<double>::perp_projection(epipolar_line2, origin);
}

//: Faster Hartley-Sturm using precomputed epipoles
void
FMatrix::find_nearest_perfect_match(const HomgPoint2D& point1,
                                    const HomgPoint2D& point2,
                                    const HomgPoint2D& epipole1,
                                    const HomgPoint2D& epipole2,
                                    HomgPoint2D *perfect_point1_ptr,
                                    HomgPoint2D *perfect_point2_ptr) const
{
  HomgLine2D line_horiz(0,1,0);

  HomgLine2D line1 = HomgOperator2D::join_oriented(point1, epipole1);
  double angle1 = HomgOperator2D::angle_between_oriented_lines (line1, line_horiz);

  HomgLine2D line2 = HomgOperator2D::join_oriented(point2, epipole2);
  double angle2 = HomgOperator2D::angle_between_oriented_lines (line2, line_horiz);

  // If the transformation from the transformed frame to the raw image frame is Pi, i=1,2, then
  // the transformed F matrix is P2^T F P1.

  double x1, y1;
  point1.get_nonhomogeneous(x1, y1);

  double x2, y2;
  point2.get_nonhomogeneous(x2, y2);

  // c s x
  //-s c y
  // 0 0 1
  vnl_double_3x3 p1_matrix;
  p1_matrix(0, 0) = vcl_cos (angle1);
  p1_matrix(0, 1) = vcl_sin (angle1);
  p1_matrix(0, 2) = x1;
  p1_matrix(1, 0)= -vcl_sin (angle1);
  p1_matrix(1, 1) = vcl_cos (angle1);
  p1_matrix(1, 2) = y1;
  p1_matrix(2, 0) = 0;
  p1_matrix(2, 1) = 0;
  p1_matrix(2, 2) = 1;

  vnl_double_3x3 p2_matrix;
  p2_matrix(0, 0) = vcl_cos (angle2);
  p2_matrix(0, 1) = vcl_sin (angle2);
  p2_matrix(0, 2) = x2;
  p2_matrix(1, 0)= -vcl_sin (angle2);
  p2_matrix(1, 1) = vcl_cos (angle2);
  p2_matrix(1, 2) = y2;
  p2_matrix(2, 0) = 0;
  p2_matrix(2, 1) = 0;
  p2_matrix(2, 2) = 1;

  vnl_double_3x3 special_f_matrix= vnl_transpose(p2_matrix) *f_matrix_ *p1_matrix;

  double f = -special_f_matrix(1, 0) / special_f_matrix(1, 2);
  double f2 = -special_f_matrix(2, 0) / special_f_matrix(2, 2);
  double g = -special_f_matrix(0, 1) / special_f_matrix(2, 1);
  double g2 = -special_f_matrix(0, 2) / special_f_matrix(2, 2);
  if (vcl_fabs ((f-f2) / f) > 0.05 || vcl_fabs ((g-g2) / g) > 0.05)
    vcl_cerr << "F matrix isn't rank 2.\n";

  // section 4.2 of the paper.

  double a = special_f_matrix(1, 1);
  double b = special_f_matrix(1, 2);
  double c = special_f_matrix(2, 1);
  double d = special_f_matrix(2, 2);

  // generated from equation (6) in the paper, using mathematica.
  vnl_vector<double> coeffs(7);
  coeffs[0] = b*b*c*d - a*b*d*d;
  coeffs[1] = b*b*b*b + b*b*c*c - a*a*d*d + 2.0*b*b*d*d*g*g + d*d*d*d*g*g*g*g;
  coeffs[2] = 4.0*a*b*b*b + a*b*c*c - a*a*c*d + 2.0*b*b*c*d*f*f - 2.0*a*b*d*d*f*f
              + 4.0*b*b*c*d*g*g + 4.0*a*b*d*d*g*g + 4.0*c*d*d*d*g*g*g*g;
  coeffs[3] = 6.0*a*a*b*b + 2.0*b*b*c*c*f*f - 2.0*a*a*d*d*f*f + 2.0*b*b*c*c*g*g
              + 8*a*b*c*d*g*g + 2.0*a*a*d*d*g*g + 6.0*c*c*d*d*g*g*g*g;
  coeffs[4] = 4.0*a*a*a*b + 2.0*a*b*c*c*f*f - 2.0*a*a*c*d*f*f + b*b*c*d*f*f*f*f
              - a*b*d*d*f*f*f*f + 4.0*a*b*c*c*g*g + 4.0*a*a*c*d*g*g + 4.0*c*c*c*d*g*g*g*g;
  coeffs[5] = a*a*a*a + b*b*c*c*f*f*f*f - a*a*d*d*f*f*f*f + 2.0*a*a*c*c*g*g + c*c*c*c*g*g*g*g;
  coeffs[6] = a*b*c*c*f*f*f*f - a*a*c*d*f*f*f*f;

  vnl_rpoly_roots roots(coeffs);

  // Hartley mentions the special case of t=infinity in his paper
  // i.e. the corrected point is at the epipole. not implemented here...

  bool real_root_flag = false;
  double s_min = 1e20;
  double t_min = 0;
  for (int root_index = 0; root_index < 6; root_index++)
    if (roots.imag(root_index) == 0) {
      // equation (4) in the paper.
      double t = roots.real(root_index);
      double s = t * t / vnl_math_sqr(1.0 + f * f * t * t) +
        vnl_math_sqr(c * t + d) /
        (vnl_math_sqr(a * t + b) + g * g * vnl_math_sqr(c * t + d));
      if (s < s_min) {
        real_root_flag = true;
        t_min = t;
        s_min = s;
      }
    }

  if (!real_root_flag) {
    vcl_cerr << "FMatrix::find_nearest_perfect_match -- no real root\n";
    return;
  }

  // if (residual_sum_squared_ptr) *residual_sum_squared_ptr = s_min;

  if (perfect_point1_ptr) {
    // the epipolar lines in the two images.
    HomgLine2D epipolar_line1(t_min * f, 1, -t_min);
    HomgLine2D epipolar_line2(-g * (c*t_min + d), a*t_min + b, c*t_min + d);
    HomgPoint2D origin(0,0,1);

    *perfect_point1_ptr = p1_matrix * HomgOperator2D::perp_projection(epipolar_line1, origin).get_vector();
    *perfect_point2_ptr = p2_matrix * HomgOperator2D::perp_projection(epipolar_line2, origin).get_vector();
  }
}

//-------------------------------------------------------------------
//
//: Ensure the current Fundamental matrix is rank 2.
// Does this by taking its vnl_svd<double>, setting the smallest singular value
// to zero, and recomposing.  Sets the rank2_flag_ to true.

void FMatrix::set_rank2_using_svd(void)
{
  // ma2_static_set_rank
  vnl_svd<double> svd(f_matrix_);
  svd.W(2) = 0;
  f_matrix_ = svd.recompose();
  ft_matrix_ = f_matrix_.transpose();
  rank2_flag_ = true;
}

//-----------------------------------------------------------------------------
//
//: Decompose F to the product of a skew-symmetric matrix and a rank 3 matrix.

void
FMatrix::decompose_to_skew_rank3(vnl_matrix<double>*, vnl_matrix<double>*) const
{
  assert(!"Not implemented\n");
}

// == DATA ACCESS ==

//----------------------------------------------------------------
//
//: Return the element of the matrix at the specified indices (zero-based)
double FMatrix::get (unsigned int row_index, unsigned int col_index) const
{
  return f_matrix_(row_index, col_index);
}


//----------------------------------------------------------------
//
//: Copy the fundamental matrix into a 2D array of doubles for `C' compatibility.
void FMatrix::get (double *c) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      *c++ = f_matrix_(row_index, col_index);
}

//----------------------------------------------------------------
//
//: Copy the fundamental matrix into a vnl_matrix<double>
void FMatrix::get (vnl_matrix<double>* f_matrix) const
{
  *f_matrix = f_matrix_;
}


//----------------------------------------------------------------
//
//: Return the rank2_flag_
bool FMatrix::get_rank2_flag (void) const
{
  return rank2_flag_;
}

//----------------------------------------------------------------
//
//: Set the rank2_flag_
void FMatrix::set_rank2_flag (bool rank2_flag)
{
  rank2_flag_ = rank2_flag;
}

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the C-storage array c_matrix, and cache the transpose.
// Always returns true for the base class - showing the set was a success.
// When overridden by derived classes
// it may return false, to indicate that the matrix violates the
// constraints imposed by the derived classes.

bool FMatrix::set (const double *c_matrix)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++) {
      double v = *c_matrix++;
      f_matrix_(row_index, col_index) = v;
      ft_matrix_(col_index, row_index) = v;
    }

  return true;
}

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the vnl_matrix<double> f_matrix.
// Always returns true for the base class - showing the set was a success.
// When overridden by derived classes it may return false, to indicate
// that the matrix violates the constraints imposed by the derived classes.

bool FMatrix::set (const vnl_matrix<double>& f_matrix)
{
  f_matrix_ = f_matrix;
  ft_matrix_ = f_matrix.transpose();

  return true;
}

#include <vnl/vnl_cross_product_matrix.h>

//: Set from two P matrices
void FMatrix::set (const PMatrix& P1, const PMatrix& P2)
{
  vnl_svd<double>* svd = P1.svd();

  vnl_cross_product_matrix e2x(P2.get_matrix() * svd->nullvector());

  set(e2x * P2.get_matrix() * svd->inverse());
}

//: Set from one P matrix, the second.  The first is assumed to be [I O].
void FMatrix::set (const PMatrix& P2)
{
  vnl_double_3x3 A;
  vnl_double_3 a;
  P2.get(&A.as_ref().non_const(), &a.as_ref().non_const());

  vnl_cross_product_matrix e2x(a);

  set(e2x * A);
}

//: Set from one P matrix, the second.  The first is assumed to be [I O].
void FMatrix::set (const FMatrix& F)
{
  *this = F;
}
