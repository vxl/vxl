#ifdef __GNUG__
#pragma implementation
#endif

//:
//  \file

#include "HMatrix1D.h"

#include <vcl_fstream.h>

#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgPoint1D.h>

//--------------------------------------------------------------
//
//: Default constructor
HMatrix1D::HMatrix1D()
{
}

//: Copy constructor
HMatrix1D::HMatrix1D(const HMatrix1D& M)
  : _t12_matrix(M._t12_matrix)
  , _t21_matrix(M._t21_matrix)
{
}

#if 0
  //: Copy constructor
  HMatrix1D::HMatrix1D(vcl_istream& s)
  {
  _t12_matrix.read_ascii(s);
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
  }
#endif

//--------------------------------------------------------------
//
//: Constructor
HMatrix1D::HMatrix1D(const vnl_matrix<double>& M)
  : _t12_matrix (M)
  , _t21_matrix(vnl_svd<double>(_t12_matrix).inverse())
{
}

//--------------------------------------------------------------------------------

HMatrix1D::HMatrix1D(const HMatrix1D&A,const HMatrix1D&B)
  : _t12_matrix(A._t12_matrix * B._t12_matrix)
  , _t21_matrix(B._t21_matrix * A._t21_matrix)
{
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix1D::HMatrix1D (const double* H)
  : _t12_matrix (H)
{
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

HMatrix1D::HMatrix1D (vcl_istream &is)
{
  _t12_matrix.read_ascii(is);
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

//: Destructor
HMatrix1D::~HMatrix1D()
{
}

// @{ OPERATIONS @}

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
  return HomgPoint1D(_t12_matrix * x1.get_vector() );
}

//
//: Return the transformed point given by $x_1 = {\tt H}^{-1} x_2$

HomgPoint1D HMatrix1D::preimage(const HomgPoint1D& x2) const
{
    return HomgPoint1D (_t21_matrix * x2.get_vector());
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

// @{ DATA ACCESS @}

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
double HMatrix1D::get (unsigned int row_index, unsigned int col_index) const
{
  return _t12_matrix. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix1D::get (double *H) const
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      *H++ = _t12_matrix. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix1D::get (vnl_matrix<double>* H) const
{
  *H = _t12_matrix;
}

//: Set to 2x2 row-stored matrix, and cache inverse.
void HMatrix1D::set (const double *H)
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      _t12_matrix. put (row_index, col_index, *H++);

  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

//: Set to given vnl_matrix, and cache inverse
void HMatrix1D::set (const vnl_matrix<double>& H)
{
  _t12_matrix = H;

  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

//: Set to inverse of given vnl_matrix, and cache inverse.
void HMatrix1D::set_inverse (const vnl_matrix<double>& H)
{
  _t21_matrix = H;
  vnl_svd<double> svd(H);
  _t12_matrix = svd.inverse();
}

//--------------------------------------------------------------------------------
