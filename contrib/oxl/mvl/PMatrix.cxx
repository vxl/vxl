#ifdef __GNUG__
#pragma implementation
#endif

//:
//  \file

#include "PMatrix.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>

#include <mvl/HomgPrettyPrint.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgLineSeg3D.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgPlane3D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/PMatrixDecompAa.h>
#include <mvl/HMatrix3D.h>
#include <mvl/HMatrix2D.h>

//--------------------------------------------------------------
//
//: Constructor. Set up a canonical P matrix.
//

PMatrix::PMatrix ():
  _svd(0)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      if (row_index == col_index)
        _p_matrix. put (row_index, col_index, 1);
      else
        _p_matrix. put (row_index, col_index, 0);
}

//--------------------------------------------------------------
//
//: Construct by loading from vcl_istream.
// <pre>
//   PMatrix P(cin);
// </pre>
PMatrix::PMatrix (vcl_istream& i) :
  _svd(0)
{
  read_ascii(i);
}

//--------------------------------------------------------------
//
//: Construct from 3x4 matrix

PMatrix::PMatrix (const vnl_matrix<double>& pmatrix) :
  _p_matrix (pmatrix),
  _svd(0)
{
}

//--------------------------------------------------------------
//
//: Construct from 3x3 matrix A and vector a. P = [A a].

PMatrix::PMatrix (const vnl_matrix<double>& A, const vnl_vector<double>& a) :
  _svd(0)
{
  set(A,a);
}

//--------------------------------------------------------------
//
//: Construct from row-stored array of 12 doubles

PMatrix::PMatrix (const double *c_matrix) :
  _p_matrix (c_matrix),
  _svd(0)
{
}

//--------------------------------------------------------------
//
// - Copy ctor

PMatrix::PMatrix (const PMatrix& that) :
  _p_matrix (that._p_matrix),
  _svd(0)
{
}

// - Assignment
PMatrix& PMatrix::operator=(const PMatrix& that)
{
  _p_matrix = that._p_matrix;
  _svd = 0;
  return *this;
}

//--------------------------------------------------------------
//
// - Destructor
PMatrix::~PMatrix()
{
  delete _svd; _svd = 0;
}

// OPERATIONS

//-----------------------------------------------------------------------------
//
//: Return the image point which is the projection of the specified 3D point X
HomgPoint2D PMatrix::project (const HomgPoint3D& X) const
{
  vnl_double_3 x = _p_matrix * X.get_vector();
  return HomgPoint2D(x);
}


//-----------------------------------------------------------------------------
//
//: Return the image line which is the projection of the specified 3D line L
HomgLine2D PMatrix::project (const HomgLine3D& L) const
{
  return HomgOperator2D::join(project(L.get_point_finite()), project(L.get_point_infinite()));
}

//-----------------------------------------------------------------------------
//
//: Return the image linesegment which is the projection of the specified 3D linesegment L
HomgLineSeg2D PMatrix::project (const HomgLineSeg3D& L) const
{
  return HomgLineSeg2D(project(L.get_point1()), project(L.get_point2()));
}

//-----------------------------------------------------------------------------
//
//: Return the 3D point $\vec X$ which is $\vec X = P^+ \vec x$.
// Equivalently, the 3D point of smallest norm such that $P \vec X = \vec x$.
// Uses svd().

HomgPoint3D PMatrix::backproject_pseudoinverse (const HomgPoint2D& x) const
{
  return svd()->solve(x.get_vector());
}

//-----------------------------------------------------------------------------
//
//: Return the 3D line which is the backprojection of the specified image point, x.
// Uses svd().

HomgLine3D PMatrix::backproject (const HomgPoint2D& x) const
{
  return HomgLine3D(get_focal_point(), backproject_pseudoinverse(x));
}

//-----------------------------------------------------------------------------
//
//: Return the 3D plane which is the backprojection of the specified line l in the image
HomgPlane3D PMatrix::backproject (const HomgLine2D& l) const
{
  return HomgPlane3D(_p_matrix.transpose() * l.get_vector());
}

//-----------------------------------------------------------------------------
//: Print p on vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const PMatrix& p)
{
  if (HomgPrettyPrint::pretty)
    return vnl_matlab_print(s, p.get_matrix(), "");
  else
    return s << p.get_matrix();
}

//-----------------------------------------------------------------------------
//: Load p from ascii vcl_istream
vcl_istream& operator>>(vcl_istream& i, PMatrix& p)
{
  p.read_ascii(i);
  return i;
}

static bool ok(vcl_istream& f) { return f.good() || f.eof(); }

//: Load from file.
// <pre>
// P.read_ascii("file.P");
// </pre>
bool PMatrix::read_ascii(vcl_istream& f)
{
  f >> (vnl_matrix<double>&)this->_p_matrix;
  clear_svd();

  if (!ok(f)) {
    //    vcl_cerr << "PMatrix::read_ascii: Failed to load P matrix" << vcl_endl;
    return false;
  }

  return true;
}

//: Load from file.
// Static method, so you can say
// <pre>
// PMatrix P = PMatrix::read("file.P");
// </pre>
PMatrix PMatrix::read(const char* filename)
{
  vcl_ifstream f(filename);
  if (!ok(f)) {
    vcl_cerr << "PMatrix::read: Failed to open P matrix file " << filename << vcl_endl;
    return PMatrix();
  }

  PMatrix P;
  if (!P.read_ascii(f))
    vcl_cerr << "PMatrix::read: Failed to read P matrix file " << filename << vcl_endl;

  return P;
}

//: Load from vcl_istream
PMatrix PMatrix::read(vcl_istream& s)
{
  PMatrix P;
  s >> P;
  return P;
}

// COMPUTATIONS

//-----------------------------------------------------------------------------
//
//: Compute the svd of this P and cache it, so that future operations that require it need not recompute it.
vnl_svd<double>* PMatrix::svd() const
{
  if (_svd == 0) {
    // Need to make _svd volatile for SGI g++ 2.7.2 optimizer bug.
    vnl_svd<double>* svd = new vnl_svd<double>(_p_matrix);
    ((PMatrix*)this)->_svd = svd; // const violation !
    return svd;
  }
  return _svd;
}

//: Discredit the cached svd.
//  This is necessary only in order to recover the space used by it if the PMatrix is not being deleted.
void PMatrix::clear_svd()
{
  if (_svd) {
    delete _svd;
    _svd = 0;
  }
}

//-----------------------------------------------------------------------------
//
//: Return the 3D point representing the focal point of the camera.
// Uses svd().
HomgPoint3D PMatrix::get_focal_point() const
{
  // From st_compute_focal_point
  if (svd()->singularities() > 1) {
    vcl_cerr << "PMatrix::get_focal_point: "
         << "  Nullspace dimension is " << svd()->singularities() << vcl_endl
         << "  Returning a vector of zeros" << vcl_endl;
    return HomgPoint3D(0,0,0,0);
  }

  vnl_matrix<double> nullspace = svd()->nullspace();

  if (nullspace(3,0) == 0)
    vcl_cerr << "PMatrix::get_focal_point: Focal point at infinity";

  return HomgPoint3D(nullspace(0,0),
                     nullspace(1,0),
                     nullspace(2,0),
                     nullspace(3,0));
}

//: Return the HMatrix3D s.t. P * H = [I 0].
// If P = [A a], then H = [inv(A) -inv(A)*a; 0 0 0 1];
HMatrix3D PMatrix::get_canonical_H() const
{
//
//M1 = P1(1:3,1:3);
//t1 = -inv(M1)*P1(:,4);
//Hinverse = [inv(M1) t1; 0 0 0 1];
//
  PMatrixDecompAa p(*this);
  vnl_svd<double> svd(p.A);
  return HMatrix3D(svd.inverse(), -svd.solve(p.a));
}

//: Return true iff P is [I 0].
// Equality is assumed if the max abs diff is less than tol.
bool PMatrix::is_canonical(double tol) const
{
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 4; ++c) {
      double d = (r == c) ? (_p_matrix(r,c) - 1) : _p_matrix(r,c);
      if (vcl_fabs(d) > tol)
        return false;
    }
  return true;
}

//: Postmultiply PMatrix by HMatrix3D
PMatrix operator*(const PMatrix& P, const HMatrix3D& H)
{
  return PMatrix(P.get_matrix() * H.get_matrix());
}

// DATA ACCESS

//-----------------------------------------------------------------------------
//
//: Return the element of the matrix at the specified indices
double
PMatrix::get (unsigned int row_index, unsigned int col_index) const
{
  return _p_matrix. get (row_index, col_index);
}

//-----------------------------------------------------------------------------
//
//: Return the 3x4 projection matrix in the array, p_matrix
void
PMatrix::get (double* c_matrix) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      *c_matrix++ = _p_matrix. get (row_index, col_index);
}

//----------------------------------------------------------------
//
//: Return the 3x4 projection matrix in the vnl_matrix<double>, p_matrix
void
PMatrix::get (vnl_matrix<double>* p_matrix) const
{
  *p_matrix = _p_matrix;
}

//----------------------------------------------------------------
//
//: Return the 3x3 matrix and 3x1 column vector of P = [A a].
void
PMatrix::get (vnl_matrix<double>* A, vnl_vector<double>* a) const
{
  A->put(0,0, _p_matrix(0,0));
  A->put(1,0, _p_matrix(1,0));
  A->put(2,0, _p_matrix(2,0));

  A->put(0,1, _p_matrix(0,1));
  A->put(1,1, _p_matrix(1,1));
  A->put(2,1, _p_matrix(2,1));

  A->put(0,2, _p_matrix(0,2));
  A->put(1,2, _p_matrix(1,2));
  A->put(2,2, _p_matrix(2,2));

  a->put(0, _p_matrix(0,3));
  a->put(1, _p_matrix(1,3));
  a->put(2, _p_matrix(2,3));
}

//----------------------------------------------------------------
//
//: Return the rows of P = [a b c]'.
void
PMatrix::get (vnl_vector<double>* a, vnl_vector<double>* b, vnl_vector<double>* c) const
{
  a->put(0, _p_matrix(0, 0));
  a->put(1, _p_matrix(0, 1));
  a->put(2, _p_matrix(0, 2));
  a->put(3, _p_matrix(0, 3));

  b->put(0, _p_matrix(1, 0));
  b->put(1, _p_matrix(1, 1));
  b->put(2, _p_matrix(1, 2));
  b->put(3, _p_matrix(1, 3));

  c->put(0, _p_matrix(2, 0));
  c->put(1, _p_matrix(2, 1));
  c->put(2, _p_matrix(2, 2));
  c->put(3, _p_matrix(2, 3));
}

//-----------------------------------------------------------------------------
//
//: Set the 3x4 projective matrix with the matrix in the array, p_matrix
void
PMatrix::set (const double p_matrix [3][4])
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      _p_matrix. put (row_index, col_index, p_matrix [row_index][col_index]);
  clear_svd();
}

//-----------------------------------------------------------------------------
//
//: Set the 3x4 projective matrix with the matrix in the array, p_matrix
void
PMatrix::set (const double *p)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      _p_matrix. put (row_index, col_index, *p++);
  clear_svd();
}


//--------------------------------------------------------------
//
//: Set the fundamental matrix using the vnl_matrix<double> p_matrix.
void
PMatrix::set (const vnl_matrix<double>& p_matrix)
{
  _p_matrix = p_matrix;
  clear_svd();
}


//----------------------------------------------------------------
//
//: Set from 3x3 matrix and 3x1 column vector of P = [A a].
void
PMatrix::set (const vnl_matrix<double>& A, const vnl_vector<double>& a)
{
  _p_matrix(0,0) = A(0,0);
  _p_matrix(1,0) = A(1,0);
  _p_matrix(2,0) = A(2,0);

  _p_matrix(0,1) = A(0,1);
  _p_matrix(1,1) = A(1,1);
  _p_matrix(2,1) = A(2,1);

  _p_matrix(0,2) = A(0,2);
  _p_matrix(1,2) = A(1,2);
  _p_matrix(2,2) = A(2,2);

  _p_matrix(0,3) = a[0];
  _p_matrix(1,3) = a[1];
  _p_matrix(2,3) = a[2];
}

//: Scale P so determinant of first 3x3 is 1.
void
PMatrix::fix_cheirality()
{
  PMatrixDecompAa p(*this);
  double det = vnl_qr<double>(p.A).determinant();

  double scale = 1;
  // Used to scale by 1/det, but it's a bad idea if det is small
  if (0) {
    if (vcl_fabs(det - 1) > 1e-8) {
      vcl_cerr << "PMatrix::fix_cheirality: Flipping, determinant is " << det << vcl_endl;
    }

    scale = 1/det;
  } else {
    if (det < 0)
      scale = -1;
  }

  _p_matrix *= scale;
  if (_svd)
    _svd->W() *= scale;
}

//: Return true if the 3D point X is behind the camera represented by this P.
// This depends on the overall sign of the P matrix having been set correctly, a
// la Hartley cheirality paper.
bool
PMatrix::is_behind_camera(const HomgPoint3D& hX)
{
  vnl_double_4 X = hX.get_vector();

  int sign = +1;
  if (X[3] < 0)
    sign = -1;

  vnl_double_4 plane = _p_matrix.get_row(2);

  return sign * dot_product(plane, X) < 0;
}

//: Change the overall sign of the P matrix.
void
PMatrix::flip_sign()
{
  _p_matrix *= -1;
  if (_svd)
    _svd->W() *= -1;
}

//: Splendid hack that tries to detect if the P is an image-coords P or a normalized P.
bool
PMatrix::looks_conditioned()
{
  double cond = svd()->W(0) / svd()->W(2);
  // vcl_cerr << "PMatrix::looks_conditioned: cond = " << cond << vcl_endl;
  return cond < 100;
}

//: Apply 3-space homography to P.
PMatrix PMatrix::postmultiply(const HMatrix3D& H) const
{
  return postmultiply(H.get_matrix());
}

//: Postmultiply by 4x4 matrix.
PMatrix PMatrix::postmultiply(const vnl_matrix<double>& H) const
{
  return PMatrix(_p_matrix * H);
}

//: Apply 2-space homography to P.
PMatrix PMatrix::premultiply(const HMatrix2D& H) const
{
  return premultiply(H.get_matrix());
}

//: Premultiply by 3x3 matrix.
PMatrix PMatrix::premultiply(const vnl_matrix<double>& H) const
{
  return PMatrix(H * _p_matrix);
}

