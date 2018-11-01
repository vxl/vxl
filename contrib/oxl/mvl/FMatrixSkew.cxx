// This is oxl/mvl/FMatrixSkew.cxx
//:
//  \file

#include <iostream>
#include <cmath>
#include "FMatrixSkew.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PMatrix.h>

//--------------------------------------------------------------
//
//: Default constructor.

FMatrixSkew::FMatrixSkew()
{
  rank2_flag_ = true;
}

//--------------------------------------------------------------
//
//: Constructor.

FMatrixSkew::FMatrixSkew(const double* f_matrix)
{
  rank2_flag_ = true;
  set(f_matrix);
}

//--------------------------------------------------------------
//
//: Constructor.

FMatrixSkew::FMatrixSkew(const vnl_matrix<double>& f_matrix)
{
  rank2_flag_ = true;
  set(f_matrix.data_block());
}


//--------------------------------------------------------------
//
//: Destructor.

FMatrixSkew::~FMatrixSkew() = default;


//--------------------------------------------------------------
//
//: Compute the epipole which is the same in each image.
//  Returns true as FMatrixSkew is always Rank 2.

bool
FMatrixSkew::get_epipoles(vgl_homg_point_2d<double>& epipole1,
                          vgl_homg_point_2d<double>& epipole2) const
{
  // fm_compute_epipoles
  epipole1.set( f_matrix_.get(1,2),
               -f_matrix_.get(0,2),
                f_matrix_.get(0,1));
  epipole2.set( f_matrix_.get(1,2),
               -f_matrix_.get(0,2),
                f_matrix_.get(0,1));
  return true;
}


//--------------------------------------------------------------
//
//: Compute the epipole which is the same in each image.
//  Returns true as FMatrixSkew is always Rank 2.

bool
FMatrixSkew::get_epipoles(HomgPoint2D *epipole1_ptr, HomgPoint2D *epipole2_ptr) const
{
  // fm_compute_epipoles
  epipole1_ptr->set( f_matrix_.get(1,2),
                    -f_matrix_.get(0,2),
                     f_matrix_.get(0,1));
  epipole2_ptr->set( f_matrix_.get(1,2),
                    -f_matrix_.get(0,2),
                     f_matrix_.get(0,1));
  return true;
}


//-----------------------------------------------------------------------------
//
//: Decompose F to the product of a skew-symmetric matrix and a Rank 3 matrix.
//    Actually returns current matrix and identity matrix.

void
FMatrixSkew::decompose_to_skew_rank3(vnl_matrix<double> *skew_matrix_ptr,
                                     vnl_matrix<double> *rank3_matrix_ptr) const
{
  *skew_matrix_ptr = this->get_matrix().as_ref();
  rank3_matrix_ptr->fill(0.0);
  rank3_matrix_ptr->fill_diagonal(1.0);
}

//-------------------------------------------------------------------
//
//: Null function as already Rank 2.

inline void
FMatrixSkew::set_rank2_using_svd (void)
{
}

//-----------------------------------------------------------------------------
//
//: Returns current matrix which is already Rank 2.

inline FMatrixSkew
FMatrixSkew::get_rank2_truncated()
{
  return *this;
}


//-----------------------------------------------------------------------------
//
//: Find nearest match which is in agreement with F.
// For a specified pair of matching points, find the nearest (minimum sum
// of squared image distances) match which is in perfect agreement with
// the epipolar geometry of the F matrix.
// For skew symmetric matrix a reduced form with only a quadratic equation
// can be used (see Armstrong Zisserman Beardsley, BMVC 94 ).

void
FMatrixSkew::find_nearest_perfect_match(vgl_homg_point_2d<double> const& point1,
                                        vgl_homg_point_2d<double> const& point2,
                                        vgl_homg_point_2d<double>& perfect_point1,
                                        vgl_homg_point_2d<double>& perfect_point2) const
{
  // get the epipole
  vgl_homg_point_2d<double> e1,e2;
  get_epipoles(e1,e2);

  // scale points if not already done and transform such that  e1->(0,0)
  double u1 = point1.x()/point1.w() - e1.x()/e1.w();
  double v1 = point1.y()/point1.w() - e1.y()/e1.w();
  double u2 = point2.x()/point2.w() - e2.x()/e2.w();
  double v2 = point2.y()/point2.w() - e2.y()/e2.w();

  // form quadratic equation
  double a_qd = (u1*v1 + u2*v2);
  double b_qd = u1*u1 + u2*u2 - v1*v1 - v2*v2;
  double c_qd = -(u1*v1 + u2*v2);

  // solve quadratic for two solutions
  double temp = b_qd * b_qd - 4 * a_qd * c_qd;

  if (temp < 0)
  {
    std::cerr << "Error in FMatrixSkew::find_nearest_perfect_match\n"
             << "Imaginary solution obtained\n"
             << "No solution returned\n";
    return;
  }

  double ttheta1 = (-b_qd + std::sqrt(temp))/(2*a_qd);
  double ttheta2 = (-b_qd - std::sqrt(temp))/(2*a_qd);

  double theta1 = std::atan(ttheta1);
  double theta2 = std::atan(ttheta2);

  double ctheta1 = std::cos(theta1), stheta1 = std::sin(theta1);
  double ctheta2 = std::cos(theta2), stheta2 = std::sin(theta2);

  double dist11 = stheta1*u1 - ctheta1*v1;
  double dist12 = stheta1*u2 - ctheta1*v2;
  double dist21 = stheta2*u1 - ctheta2*v1;
  double dist22 = stheta2*u2 - ctheta2*v2;

  // find correct solution with minimum distance - set to theta1
  if (std::fabs(dist11) + std::fabs(dist12) > std::fabs(dist21) + std::fabs(dist22))
  {
    stheta1 = stheta2;
    ctheta1 = ctheta2;
    dist11   = dist21;
    dist12   = dist22;
  }

  // do with per_proj... in HomgOp2D::
  perfect_point1.set(u1-dist11*stheta1+e1.x()/e1.w(),
                     v1+dist11*ctheta1+e1.y()/e1.w(),1);
  perfect_point2.set(u2-dist12*stheta1+e2.x()/e2.w(),
                     v2+dist12*ctheta1+e2.y()/e2.w(),1);
}

//-----------------------------------------------------------------------------
//
//: Find nearest match which is in agreement with F.
// For a specified pair of matching points, find the nearest (minimum sum
// of squared image distances) match which is in perfect agreement with
// the epipolar geometry of the F matrix.
// For skew symmetric matrix a reduced form with only a quadratic equation
// can be used (see Armstrong Zisserman Beardsley, BMVC 94 ).

void
FMatrixSkew::find_nearest_perfect_match(const HomgPoint2D& point1,
                                        const HomgPoint2D& point2,
                                        HomgPoint2D *perfect_point1_ptr,
                                        HomgPoint2D *perfect_point2_ptr) const
{
  // get the epipole
  HomgPoint2D e1,e2;
  get_epipoles(&e1,&e2);

  // scale points if not already done and transform such that  e1->(0,0)
  double u1 = point1.x()/point1.w() - e1.x()/e1.w();
  double v1 = point1.y()/point1.w() - e1.y()/e1.w();
  double u2 = point2.x()/point2.w() - e2.x()/e2.w();
  double v2 = point2.y()/point2.w() - e2.y()/e2.w();

  // form quadratic equation
  double a_qd = (u1*v1 + u2*v2);
  double b_qd = u1*u1 + u2*u2 - v1*v1 - v2*v2;
  double c_qd = -(u1*v1 + u2*v2);

  // solve quadratic for two solutions
  double temp = b_qd * b_qd - 4 * a_qd * c_qd;

  if (temp < 0)
  {
    std::cerr << "Error in FMatrixSkew::find_nearest_perfect_match\n"
             << "Imaginary solution obtained\n"
             << "No solution returned\n";
    return;
  }

  double ttheta1 = (-b_qd + std::sqrt(temp))/(2*a_qd);
  double ttheta2 = (-b_qd - std::sqrt(temp))/(2*a_qd);

  double theta1 = std::atan(ttheta1);
  double theta2 = std::atan(ttheta2);

  double ctheta1 = std::cos(theta1), stheta1 = std::sin(theta1);
  double ctheta2 = std::cos(theta2), stheta2 = std::sin(theta2);

  double dist11 = stheta1*u1 - ctheta1*v1;
  double dist12 = stheta1*u2 - ctheta1*v2;
  double dist21 = stheta2*u1 - ctheta2*v1;
  double dist22 = stheta2*u2 - ctheta2*v2;

  // find correct solution with minimum distance - set to theta1
  if (std::fabs(dist11) + std::fabs(dist12) > std::fabs(dist21) + std::fabs(dist22))
  {
    stheta1 = stheta2;
    ctheta1 = ctheta2;
    dist11   = dist21;
    dist12   = dist22;
  }

  // do with per_proj... in HomgOp2D::
  perfect_point1_ptr->set(u1-dist11*stheta1+e1.x()/e1.w(),
                          v1+dist11*ctheta1+e1.y()/e1.w(),1);
  perfect_point2_ptr->set(u2-dist12*stheta1+e2.x()/e2.w(),
                          v2+dist12*ctheta1+e2.y()/e2.w(),1);
}


//--------------------------------------------------------------
//
//: Set the fundamental matrix using the two-dimensional array f_matrix.
// Only returns true if f_matrix contained a
// skew matrix, not an approximation to one.
// The test is against a 0.0 tolerance.
// Otherwise returns false and the matrix is not set.

bool FMatrixSkew::set (const double* f_matrix )
{
  int row_index, col_index;

  // should be set to 0.0
  constexpr double tolerance = 0.0;

  // CRUDE test for skewness with tolerance 0
  // test diagonal is zero and asymmetric
  if ((f_matrix[1] + f_matrix[3] > tolerance) |
      (f_matrix[2] + f_matrix[6] > tolerance) |
      (f_matrix[5] + f_matrix[7] > tolerance) |
      (f_matrix[0] > tolerance) |
      (f_matrix[4] > tolerance) |
      (f_matrix[8] > tolerance) )
  {
    std::cerr << "WARNING: F matrix not skew symmetric so cannot allocate to FMatrixSkew\n" ;
    return false;
  }

  for (row_index = 0; row_index < 3; row_index++)
    for (col_index = 0; col_index < 3; col_index++)
    {
      double v = *f_matrix++;
      f_matrix_. put (row_index, col_index,v);
      ft_matrix_. put (col_index, row_index,v);
    }

  // set rank flag true

  FMatrix::set_rank2_flag(true);

  return true;
}


//--------------------------------------------------------------
//
//: Set the fundamental matrix using the vnl_matrix<double> f_matrix.
// Only returns true if f_matrix contained a
// skew matrix, not an approximation to one.
// Otherwise returns false and the matrix is not set.
// Patch on FMatrixSkew::set (const double*).

inline bool
FMatrixSkew::set (const vnl_matrix<double>& f_matrix )
{
  return set(f_matrix.data_block());
}


//----------------------------------------------------------------
//
//: Returns the rank2_flag_ which is always true for FMatrixSkew.

inline bool
FMatrixSkew::get_rank2_flag (void) const
{
  return true;
}

//----------------------------------------------------------------
//
//: Set the rank2_flag_. Null function as always set true.

inline void
FMatrixSkew::set_rank2_flag (bool)
{
  FMatrix::set_rank2_flag(true);
}
