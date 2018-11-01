// This is oxl/mvl/HMatrix3D.cxx
//:
// \file
#include <iostream>
#include "HMatrix3D.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_matlab_print.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>

#include <mvl/HomgPrettyPrint.h>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#include <mvl/HomgPoint3D.h>

//--------------------------------------------------------------
//
//: Default constructor
HMatrix3D::HMatrix3D() = default;

//: Copy constructor
HMatrix3D::HMatrix3D(const HMatrix3D& M) = default;

//--------------------------------------------------------------
//
//: Constructor
HMatrix3D::HMatrix3D(vnl_double_4x4 const& M):
  Base(M)
{
}

//--------------------------------------------------------------
//
//: Load H from ASCII std::istream.
HMatrix3D::HMatrix3D(std::istream& s)
{
  load(s);
}


//--------------------------------------------------------------
//
//: Construct an affine HMatrix3D from 3x3 M and 3x1 m.
// \f[ H = \begin{array}{cc} M & m\\ 0 & 1 \end{array} \f]
HMatrix3D::HMatrix3D(vnl_double_3x3 const& M, vnl_double_3 const& m)
{
  assert(M.rows() == 3);
  assert(M.columns() == 3);
  assert(m.size() == 3);

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c)
      (*this)(r, c) = M(r,c);
    (*this)(r, 3) = m(r);
  }
  for (int c = 0; c < 3; ++c)
    (*this)(3,c) = 0;
  (*this)(3,3) = 1;
}

//--------------------------------------------------------------
//
//: Construct from a 16-element row-storage array of double.
HMatrix3D::HMatrix3D (const double* t_matrix) :
  Base(t_matrix)
{
}

//: Destructor
HMatrix3D::~HMatrix3D() = default;

// == OPERATIONS ==

//-----------------------------------------------------------------------------
//
// - Return the transformed point given by $ x_2 = T x_1 $

HomgPoint3D HMatrix3D::transform(const HomgPoint3D& x1) const
{
    return HomgPoint3D ((*this) * x1.get_vector());
}

//-----------------------------------------------------------------------------
//
//: Return the transformed line given by $ l_2 = T \ast l_1 $

HomgLine3D HMatrix3D::transform(const HomgLine3D& l1) const
{
  // transform the two points defining the line and then
  // create/trurn the transformed line
  HomgPoint3D p1((*this) * l1.get_point_finite().get_vector());
  HomgPoint3D p2((*this) * l1.get_point_infinite().get_vector());

  return HomgLine3D(p1,p2);
}

//-----------------------------------------------------------------------------
//: Print H on std::ostream
std::ostream& operator<<(std::ostream& s, const HMatrix3D& h)
{
  if (HomgPrettyPrint::pretty)
    return vnl_matlab_print(s, (vnl_matrix<double> const&)h, "");
  else
    return s << (vnl_matrix<double> const&)h;
}

//: Load H from ASCII file.
bool HMatrix3D::load(std::istream& s)
{
  this->read_ascii(s);
  return s.good() || s.eof();
}

//: Load H from ASCII file.
std::istream& operator>>(std::istream& s, HMatrix3D& H)
{
  H.load(s);
  return s;
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
double HMatrix3D::get (unsigned int row_index, unsigned int col_index) const
{
  return vnl_double_4x4::get(row_index, col_index);
}

//-----------------------------------------------------------------------------
//: Fill t_matrix with contents of H
void HMatrix3D::get (double *t_matrix) const
{
  for (int row_index = 0; row_index < 4; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      *t_matrix++ = vnl_double_4x4::get(row_index, col_index);
}

//-----------------------------------------------------------------------------
//: Fill t_matrix with contents of H
void HMatrix3D::get (vnl_matrix<double>* t_matrix) const
{
  *t_matrix = this->as_ref(); // size 4x4
}

//-----------------------------------------------------------------------------
//: Return the inverse of this HMatrix3D.  *Not* using vnl_svd.
HMatrix3D HMatrix3D::get_inverse() const
{
  return vnl_inverse(*this);
}
