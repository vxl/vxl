// This is oxl/mvl/HMatrix1D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "HMatrix1D.h"
#include <mvl/HomgPoint1D.h>
#include <vcl_fstream.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_inverse.h>

//--------------------------------------------------------------
//
//: Default constructor
HMatrix1D::HMatrix1D()
{
}

//: Copy constructor
HMatrix1D::HMatrix1D(const HMatrix1D& M)
  : t12_matrix_(M.t12_matrix_)
  , t21_matrix_(M.t21_matrix_)
{
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix1D::HMatrix1D(vnl_double_2x2 const& M)
  : t12_matrix_ (M)
  , t21_matrix_(vnl_inverse(t12_matrix_))
{
}

//--------------------------------------------------------------------------------

HMatrix1D::HMatrix1D(const HMatrix1D&A,const HMatrix1D&B)
  : t12_matrix_(A.t12_matrix_ * B.t12_matrix_)
  , t21_matrix_(B.t21_matrix_ * A.t21_matrix_)
{
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix1D::HMatrix1D (const double* H)
  : t12_matrix_ (H)
{
  t21_matrix_ = vnl_inverse(t12_matrix_);
}

HMatrix1D::HMatrix1D (vcl_istream &is)
{
  t12_matrix_.read_ascii(is);
  t21_matrix_ = vnl_inverse(t12_matrix_);
}

//: Destructor
HMatrix1D::~HMatrix1D()
{
}

// == OPERATIONS ==

//-----------------------------------------------------------------------------
//
//: Return the transformed point given by $x_2 = {\tt H} x_1$

HomgPoint1D HMatrix1D::transform_to_plane2(const HomgPoint1D& x1) const
{
  return (*this)(x1);
}

//
//: Return the transformed point given by $x_1 = {\tt H}^{-1} x_2$

HomgPoint1D HMatrix1D::transform_to_plane1(const HomgPoint1D& x2) const
{
  return this->preimage(x2);
}

HomgPoint1D HMatrix1D::operator()(const HomgPoint1D& x1) const
{
  vnl_double_2 v = t12_matrix_ * vnl_double_2(x1.x(),x1.w());
  return HomgPoint1D(v[0], v[1]);
}

//
//: Return the transformed point given by $x_1 = {\tt H}^{-1} x_2$

HomgPoint1D HMatrix1D::preimage(const HomgPoint1D& x2) const
{
  vnl_double_2 v = t21_matrix_ * vnl_double_2(x2.x(),x2.w());
  return HomgPoint1D(v[0], v[1]);
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const HMatrix1D& h)
{
  return s << h.get_matrix();
}

//: Read H from vcl_istream
vcl_istream& operator >> (vcl_istream& s, HMatrix1D& H)
{
  H = HMatrix1D(s);
  return s;
}

//: Read H from vcl_istream
HMatrix1D HMatrix1D::read(vcl_istream& s)
{
  return HMatrix1D(s);
}


//: Read H from file
HMatrix1D HMatrix1D::read(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "HMatrix1D::read: Error opening " << filename << vcl_endl;
  return HMatrix1D(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
double HMatrix1D::get (unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix1D::get (double *H) const
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      *H++ = t12_matrix_. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix1D::get (vnl_matrix<double>* H) const
{
  *H = t12_matrix_;
}

//: Set to 2x2 row-stored matrix, and cache inverse.
void HMatrix1D::set (const double *H)
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      t12_matrix_. put (row_index, col_index, *H++);

  t21_matrix_ = vnl_inverse(t12_matrix_);
}

//: Set to given vnl_matrix, and cache inverse
void HMatrix1D::set (vnl_double_2x2 const& H)
{
  t12_matrix_ = H;

  t21_matrix_ = vnl_inverse(t12_matrix_);
}

//: Set to inverse of given vnl_matrix, and cache inverse.
void HMatrix1D::set_inverse (vnl_double_2x2 const& H)
{
  t21_matrix_ = H;
  t21_matrix_ = vnl_inverse(t12_matrix_);
}

//--------------------------------------------------------------------------------
