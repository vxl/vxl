// This is oxl/mvl/HMatrix2D.cxx
//:
// \file

#include <iostream>
#include <fstream>
#include "HMatrix2D.h"
#include <mvl/HomgOperator2D.h>
#include <vnl/vnl_inverse.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//--------------------------------------------------------------
//
//: Default constructor
HMatrix2D::HMatrix2D() = default;

//: Copy constructor
HMatrix2D::HMatrix2D(const HMatrix2D& M)
{
  t12_matrix_ = M.t12_matrix_;
}


//: Constructor from std::istream
HMatrix2D::HMatrix2D(std::istream& s)
{
  t12_matrix_.read_ascii(s);
}

//: Constructor from file
HMatrix2D::HMatrix2D(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "HMatrix2D::read: Error opening " << filename << std::endl;
  else
    t12_matrix_.read_ascii(f);
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix2D::HMatrix2D(vnl_double_3x3 const& M):
  t12_matrix_ (M)
{
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix2D::HMatrix2D (const double* H)
  : t12_matrix_ (H)
{
}

//: Destructor
HMatrix2D::~HMatrix2D() = default;

// == OPERATIONS ==

//-----------------------------------------------------------------------------
//
//: Return the transformed point given by $ x_2 = {\tt H} x_1 $.

HomgPoint2D HMatrix2D::transform_to_plane2(const HomgPoint2D& x1) const
{
    return HomgPoint2D (t12_matrix_ * x1.get_vector());
}


//-----------------------------------------------------------------------------
//
//: Return the transformed line given by $ l_1 = {\tt H}^\top l_2 $.

HomgLine2D HMatrix2D::transform_to_plane1(const HomgLine2D& l2) const
{
  return HomgLine2D(t12_matrix_.transpose() * l2.get_vector());
}

//-----------------------------------------------------------------------------
//: Return the transformed point $x_1$ given by $x_2 = H \, x_1$.
// Note that this calculates the inverse of $H$ on every call.

HomgPoint2D HMatrix2D::transform_to_plane1(const HomgPoint2D& x2) const
{
  static bool warned = false;
  if (! warned) {
    std::cerr << "HMatrix2D::transform_to_plane1(HomgPoint2D): Warning: calculating inverse matrix\n";
    warned = true;
  }
  vnl_double_3x3 t21_matrix_ = this->get_inverse().get_matrix();
  return HomgPoint2D(t21_matrix_ * x2.get_vector());
}

//-----------------------------------------------------------------------------
//: Return the transformed line $l_2$ given by $l_1=H^\top l_2$.
// Note that this calculates the inverse of $H$ on every call.

HomgLine2D HMatrix2D::transform_to_plane2(const HomgLine2D& l1) const
{
  std::cerr << "HMatrix2D::transform_to_plane2(HomgLine2D): Warning: calculating inverse matrix\n";
  vnl_double_3x3 t21_matrix_ = this->get_inverse().get_matrix().transpose();
  return HomgLine2D(t21_matrix_ * l1.get_vector());
}

//-----------------------------------------------------------------------------
//: Print H on std::ostream
std::ostream& operator<<(std::ostream& s, const HMatrix2D& h)
{
  return s << h.get_matrix();
}

//: Read H from std::istream
std::istream& operator >> (std::istream& s, HMatrix2D& H)
{
  H = HMatrix2D(s);
  return s;
}

//: Read H from std::istream
bool HMatrix2D::read(std::istream& s)
{
  return t12_matrix_.read_ascii(s);
}

//: Read H from file
bool HMatrix2D::read(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "HMatrix2D::read: Error opening " << filename << std::endl;
  return read(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
double HMatrix2D::get (unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix2D::get (double *H) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      *H++ = t12_matrix_. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix2D::get (vnl_matrix<double>* H) const
{
  *H = t12_matrix_.as_ref(); // size 3x3
}

//: Set to identity
void HMatrix2D::set_identity ()
{
  t12_matrix_.set_identity();
}

//: Set to 3x3 row-stored matrix
void HMatrix2D::set (const double *H)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      t12_matrix_. put (row_index, col_index, *H++);
}

//: Set to given vnl_matrix
void HMatrix2D::set (vnl_double_3x3 const& H)
{
  t12_matrix_ = H;
}

//: Return inverse of this homography
HMatrix2D HMatrix2D::get_inverse() const
{
  return vnl_inverse(t12_matrix_);
}

//: Return new axis-aligned bounding box after (x0,y0) -> (x1,y1) have been pre-multiplied by H.
vnl_double_4 HMatrix2D::transform_bounding_box(double /*x0*/, double /*y0*/, double /*x1*/, double /*y1*/)
{
  std::cerr << "FIXME: HMatrix2D::transform_bounding_box() is not yet implemented\n";
  return {};

#if 0
  // Find bbox of transformed image
  BoundingBox<double, 2> dest_bbox;
  double logo_bbox[][2] = {
    {x0, y0},
    {x1, y0},
    {x0, y1},
    {x1, y1}
  };
  for (int k = 0; k < 4; ++k) {
    HomgPoint2D corner(logo_bbox[k][0], logo_bbox[k][1], 1);
    corner = (*this) * corner;
    double s = 1.0/corner[2];
    dest_bbox.update(corner[0]*s, corner[1]*s);
  }

  double* min = dest_bbox.get_min();
  double* max = dest_bbox.get_max();
  return vnl_double_4(min[0], min[1], max[0], max[1]);
#endif
}
