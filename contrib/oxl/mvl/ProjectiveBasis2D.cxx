#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/algo/vnl_svd.h>
#include "ProjectiveBasis2D.h"

static const int warn_ = false;
static const int check_collinear = true;

//: Compute projective basis for 4 points.
ProjectiveBasis2D::ProjectiveBasis2D(const HomgPoint2D& p1, const HomgPoint2D& p2, const HomgPoint2D& p3, const HomgPoint2D& p4)
{
  compute(p1, p2, p3, p4);
}

//: Compute projective basis for first 4 points in given array.
ProjectiveBasis2D::ProjectiveBasis2D(const vcl_vector<HomgPoint2D>& points)
{
  assert(points.size() >= 4);
  compute(points[0], points[1], points[2], points[3]);
}

// Copy ctor
ProjectiveBasis2D::ProjectiveBasis2D(const ProjectiveBasis2D& that)
{
  operator=(that);
}

// Assignment
ProjectiveBasis2D& ProjectiveBasis2D::operator=(const ProjectiveBasis2D&)
{
  assert(!"ProjectiveBasis2D::operator=");
  return *this;
}

//: Destructor
ProjectiveBasis2D::~ProjectiveBasis2D()
{
}

//:
// Find the transformation which maps the 4 points to the canonical
// projective frame using the linear method of [Beardsley et. al., ECCV96].
// If three of the four points are nearly collinear, an error message is
// printed, but the computation is still carried out.
void ProjectiveBasis2D::compute(const HomgPoint2D& p1, const HomgPoint2D& p2, const HomgPoint2D& p3, const HomgPoint2D& p4)
{
  if (check_collinear) {
    vnl_double_3x4 full_matrix;
    full_matrix.set_column(0, p1.get_vector());
    full_matrix.set_column(1, p2.get_vector());
    full_matrix.set_column(2, p3.get_vector());
    full_matrix.set_column(3, p4.get_vector());

    if (! full_matrix.is_finite() || full_matrix.has_nans()) {
      vcl_cerr << "Error (ProjectiveBasis2D): given matrix has infinite or NaN values\n";
      T_.set_identity(); collinear_ = true; return;
    }

    vnl_svd<double> s(full_matrix, 1e-8);
    collinear_ = (s.rank() < 3);

    if (collinear_ && warn_)
      vcl_cerr << "Warning (ProjectiveBasis2D): Three out of the four points are nearly collinear\n";
  } else {
    collinear_ = false;
  }

  vnl_double_3x3 back_matrix;
  back_matrix.set_column(0, p1.get_vector());
  back_matrix.set_column(1, p2.get_vector());
  back_matrix.set_column(2, p3.get_vector());

  vnl_svd<double> svd(back_matrix);

  vnl_double_3 scales_vector = svd.solve(p4.get_vector());

  back_matrix.set_column(0, scales_vector[0] * p1.get_vector());
  back_matrix.set_column(1, scales_vector[1] * p2.get_vector());
  back_matrix.set_column(2, scales_vector[2] * p3.get_vector());

  if (! back_matrix.is_finite() || back_matrix.has_nans()) {
    vcl_cerr << "Error (ProjectiveBasis2D): back matrix has infinite or NaN values\n";
    T_.set_identity(); collinear_ = true; return;
  }

  vnl_svd<double> svd1(back_matrix);
  T_.set(svd1.inverse());
}
