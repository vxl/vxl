#ifdef __GNUG__
#pragma implementation
#endif

//:
//  \file

#include "HMatrix2D.h"

#include <vcl_fstream.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <mvl/HomgLine2D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/HomgPoint2D.h>
// #include <Basics/BoundingBox.h>

//--------------------------------------------------------------
//
//: Default constructor
HMatrix2D::HMatrix2D()
{
}

//: Copy constructor
HMatrix2D::HMatrix2D(const HMatrix2D& M)
{
  _t12_matrix = M._t12_matrix;
}


//: Constructor from vcl_istream
HMatrix2D::HMatrix2D(vcl_istream& s)
{
  _t12_matrix.read_ascii(s);
}

//: Constructor from file
HMatrix2D::HMatrix2D(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "HMatrix2D::read: Error opening " << filename << vcl_endl;
  else
    _t12_matrix.read_ascii(f);
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix2D::HMatrix2D(const vnl_matrix<double>& M):
  _t12_matrix (M)
{
}

//--------------------------------------------------------------
//
//: Constructor
HMatrix2D::HMatrix2D (const double* H)
  : _t12_matrix (H)
{
}

//: Destructor
HMatrix2D::~HMatrix2D()
{
}

// @{ OPERATIONS @}

//-----------------------------------------------------------------------------
//
//: Return the transformed point given by $ x_2 = {\tt H} x_1 $.

HomgPoint2D HMatrix2D::transform_to_plane2(const HomgPoint2D& x1) const
{
    return HomgPoint2D (_t12_matrix * x1.get_vector());
}


//-----------------------------------------------------------------------------
//
//: Return the transformed line given by $ l_1 = {\tt H}^\top l_2 $.

HomgLine2D HMatrix2D::transform_to_plane1(const HomgLine2D& l2) const
{
  return HomgLine2D(_t12_matrix.transpose() * l2.get_vector());
}

//-----------------------------------------------------------------------------
//: Return the transformed point @{$x_1$@} given by @{$x_2 = {\tt H} x_1$@}.
// Note that this calculates the inverse of @{\tt H@} on every call.

HomgPoint2D HMatrix2D::transform_to_plane1(const HomgPoint2D& x2) const
{
  static bool warned = false;
  if (! warned) {
    vcl_cerr << "HMatrix2D::transform_to_plane1(HomgPoint2D): Warning: calculating inverse matrix\n";
    warned = true;
  }
  vnl_double_3x3 _t21_matrix = this->get_inverse().get_matrix();
  return HomgPoint2D(_t21_matrix * x2.get_vector());
}

//-----------------------------------------------------------------------------
//: Return the transformed line @{$l_2$@} given by @{$l_1={\tt H}^\top l_2$@}.
// Note that this calculates the inverse of @{\tt H@} on every call.

HomgLine2D HMatrix2D::transform_to_plane2(const HomgLine2D& l1) const
{
  vcl_cerr << "HMatrix2D::transform_to_plane2(HomgLine2D): Warning: calculating inverse matrix\n";
  vnl_double_3x3 _t21_matrix = this->get_inverse().get_matrix().transpose();
  return HomgLine2D(_t21_matrix * l1.get_vector());
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const HMatrix2D& h)
{
  return s << h.get_matrix();
}

//: Read H from vcl_istream
vcl_istream& operator >> (vcl_istream& s, HMatrix2D& H)
{
  H = HMatrix2D(s);
  return s;
}

//: Read H from vcl_istream
bool HMatrix2D::read(vcl_istream& s)
{
  return _t12_matrix.read_ascii(s);
}

//: Read H from file
bool HMatrix2D::read(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "HMatrix2D::read: Error opening " << filename << vcl_endl;
  return read(f);
}

// @{ DATA ACCESS @}

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
double HMatrix2D::get (unsigned int row_index, unsigned int col_index) const
{
  return _t12_matrix. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix2D::get (double *H) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      *H++ = _t12_matrix. get (row_index, col_index);
}

//: Fill H with contents of this
void HMatrix2D::get (vnl_matrix<double>* H) const
{
  *H = _t12_matrix;
}

//: Set to identity
void HMatrix2D::set_identity ()
{
  _t12_matrix.set_identity();
}

//: Set to 3x3 row-stored matrix
void HMatrix2D::set (const double *H)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      _t12_matrix. put (row_index, col_index, *H++);
}

//: Set to given vnl_matrix
void HMatrix2D::set (const vnl_matrix<double>& H)
{
  _t12_matrix = H;
}

//: Return inverse of this homography
HMatrix2D HMatrix2D::get_inverse() const
{
  vnl_svd<double> svd(_t12_matrix);
  return svd.inverse();
}

//: Return new axis-aligned bounding box after (x0,y0) -> (x1,y1) have been premultiplied by H.
vnl_double_4 HMatrix2D::transform_bounding_box(double /*x0*/, double /*y0*/, double /*x1*/, double /*y1*/)
{
  vcl_cerr << "FIXME: HMatrix2D::transform_bounding_box() is not yet implemented\n";
  return vnl_double_4();

//  // Find bbox of transformed image
//  BoundingBox<double, 2> dest_bbox;
//  double logo_bbox[][2] = {
//    {x0, y0},
//    {x1, y0},
//    {x0, y1},
//    {x1, y1}
//  };
//  for(int k = 0; k < 4; ++k) {
//    HomgPoint2D corner(logo_bbox[k][0], logo_bbox[k][1], 1);
//    corner = (*this) * corner;
//    double s = 1.0/corner[2];
//    dest_bbox.update(corner[0]*s, corner[1]*s);
//  }
//
//  double* min = dest_bbox.get_min();
//  double* max = dest_bbox.get_max();
//  return vnl_double_4(min[0], min[1], max[0], max[1]);
}

