// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifdef __GNUG__
#pragma implementation
#endif
//--------------------------------------------------------------
//
// Class : HMatrix1D
//
// Modifications : see HMatrix1D.h
//
//-----------------------------------------------------------------------------

#include "HMatrix1D.h"

#include <vcl/vcl_fstream.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <mvl/HomgPoint1D.h>

//--------------------------------------------------------------
//
// -- Default constructor
HMatrix1D::HMatrix1D()
{
}

// -- Copy constructor
HMatrix1D::HMatrix1D(const HMatrix1D& M)
  : _t12_matrix(M._t12_matrix)
  , _t21_matrix(M._t21_matrix)
{
}

/*
  // -- Copy constructor
  HMatrix1D::HMatrix1D(istream& s)
  {
  _t12_matrix.read_ascii(s);
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
  }
*/

//--------------------------------------------------------------
//
// -- Constructor
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
// -- Constructor
HMatrix1D::HMatrix1D (const double* H)
  : _t12_matrix (H)
{
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

HMatrix1D::HMatrix1D (istream &is)
{
  _t12_matrix.read_ascii(is);
  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

// -- Destructor
HMatrix1D::~HMatrix1D()
{
}

// @{ OPERATIONS @}

//-----------------------------------------------------------------------------
//
// -- Return the transformed point given by @{ $x_2 = {\tt H} x_1$ @}

HomgPoint1D HMatrix1D::transform_to_plane2(const HomgPoint1D& x1) const
{
  return (*this)(x1);
}

//
// -- Return the transformed point given by @{ $x_1 = {\tt H}^{-1} x_2$ @}

HomgPoint1D HMatrix1D::transform_to_plane1(const HomgPoint1D& x2) const
{
    return this->preimage(x2);
}

HomgPoint1D HMatrix1D::operator()(const HomgPoint1D& x1) const
{
  return HomgPoint1D(_t12_matrix * x1.get_vector() );
}

//
// -- Return the transformed point given by @{ $x_1 = {\tt H}^{-1} x_2$ @}

HomgPoint1D HMatrix1D::preimage(const HomgPoint1D& x2) const
{
    return HomgPoint1D (_t21_matrix * x2.get_vector());
}

//-----------------------------------------------------------------------------
// -- Print H on ostream
ostream& operator<<(ostream& s, const HMatrix1D& h)
{
  return s << h.get_matrix();
}

// -- Read H from istream
istream& operator >> (istream& s, HMatrix1D& H)
{
  H = HMatrix1D(s);
  return s;
}

// -- Read H from istream
HMatrix1D HMatrix1D::read(istream& s)
{
  return HMatrix1D(s);
}


// -- Read H from file
HMatrix1D HMatrix1D::read(char const* filename)
{
  ifstream f(filename);
  if (!f.good())
    cerr << "HMatrix1D::read: Error opening " << filename << endl;
  return HMatrix1D(f);
}

// @{ DATA ACCESS @}

//-----------------------------------------------------------------------------
// -- Get matrix element at (row_index, col_index)
double HMatrix1D::get (unsigned int row_index, unsigned int col_index) const
{
  return _t12_matrix. get (row_index, col_index);
}
  
// -- Fill H with contents of this
void HMatrix1D::get (double *H) const
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      *H++ = _t12_matrix. get (row_index, col_index);
}

// -- Fill H with contents of this
void HMatrix1D::get (vnl_matrix<double>* H) const
{
  *H = _t12_matrix;
}

// -- Set to 2x2 row-stored matrix, and cache inverse.
void HMatrix1D::set (const double *H)
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      _t12_matrix. put (row_index, col_index, *H++);

  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

// -- Set to given vnl_matrix, and cache inverse
void HMatrix1D::set (const vnl_matrix<double>& H)
{
  _t12_matrix = H;

  vnl_svd<double> svd(_t12_matrix);
  _t21_matrix = svd.inverse();
}

// -- Set to inverse of given vnl_matrix, and cache inverse.
void HMatrix1D::set_inverse (const vnl_matrix<double>& H)
{
  _t21_matrix = H;
  vnl_svd<double> svd(H);
  _t12_matrix = svd.inverse();
}

//--------------------------------------------------------------------------------

