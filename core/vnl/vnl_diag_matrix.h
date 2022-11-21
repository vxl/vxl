// This is core/vnl/vnl_diag_matrix.h
#ifndef vnl_diag_matrix_h_
#define vnl_diag_matrix_h_
//:
// \file
// \brief Contains class for diagonal matrices
// \author Andrew W. Fitzgibbon (Oxford RRG)
// \date   5 Aug 1996
//
// \verbatim
//  Modifications
//   IMS (Manchester) 16 Mar 2001: Tidied up the documentation + added binary_io
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Sep.2002 - Peter Vanroose - Added operator+, operator-, operator*
//   Mar.2004 - Peter Vanroose - removed deprecated resize()
//   Oct.2010 - Peter Vanroose - mutators and setters now return *this
//   Jan.2011 - Peter Vanroose - added methods set_diagonal() & get_diagonal()
// \endverbatim

#include <cassert>
#include <iosfwd>
#include <utility>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vnl_vector.h"
#include "vnl_matrix.h"
#include "vnl_matrix_ref.h"
#include "vnl_vector_ref.h"
#include "vnl/vnl_export.h"

// forward declarations
template <class T> class vnl_diag_matrix;
template <class T> VNL_EXPORT vnl_vector<T> operator*(vnl_diag_matrix<T> const&, vnl_vector<T> const&);

//: stores a diagonal matrix as a single vector.
//  vnl_diag_matrix stores a diagonal matrix for time and space efficiency.
//  Specifically, only the diagonal elements are stored, and some matrix
//  operations (currently *, + and -) are overloaded to use more efficient
//  algorithms.

template <class T>
class VNL_EXPORT vnl_diag_matrix: public Eigen::DiagonalMatrix<T,Eigen::Dynamic, Eigen::Dynamic>
{
protected:
  //vnl_vector<T> this->data();
  using Superclass = Eigen::DiagonalMatrix<T,Eigen::Dynamic, Eigen::Dynamic>;
 public:
  vnl_diag_matrix() = default;
  vnl_diag_matrix(const vnl_diag_matrix<T> &)  = default;
  vnl_diag_matrix(vnl_diag_matrix<T> &&)   noexcept = default;
  vnl_diag_matrix& operator=(const vnl_diag_matrix<T> &)  = default;
  vnl_diag_matrix& operator=(vnl_diag_matrix<T> &&)   noexcept = default;
  ~vnl_diag_matrix() = default;

  //: Construct an empty diagonal matrix.
  explicit vnl_diag_matrix(unsigned nn) : Superclass(nn) {}
  explicit vnl_diag_matrix(Superclass & rhs): Superclass(rhs) {}
  //: Construct a diagonal matrix with diagonal elements equal to value.
  vnl_diag_matrix(unsigned nn, T const& value) : Superclass(nn) { this->setIdentity(); (*this)*=value;}

  //: Construct a diagonal matrix from a vnl_vector.
  //  The vector elements become the diagonal elements.
  explicit vnl_diag_matrix(vnl_vector<T> that) : Superclass(std::move(that)) {}

  // Operations----------------------------------------------------------------

  //: In-place arithmetic operation
  inline vnl_diag_matrix<T>& operator*=(T v) {
    for(int i = 0; i < this->rows(); ++i)
    {
      this->data()[i] *= v;
    }
    return *this;
  }
  //: In-place arithmetic operation
  inline vnl_diag_matrix<T>& operator/=(T v)
  {
    for (int i = 0; i < this->rows(); ++i)
    {
      this->data()[i] /= v;
    }
    return *this;
  }
  // Computations--------------------------------------------------------------

  vnl_diag_matrix& invert_in_place();
  //baseclass T determinant() const;
  vnl_vector<T> solve(vnl_vector<T> const& b) const;
  void solve(vnl_vector<T> const& b, vnl_vector<T>* out) const;

  // Data Access---------------------------------------------------------------

  inline T operator () (unsigned i, unsigned j) const {
    return (i != j) ? T(0) : this->data()[i];
  }

  inline T& operator () (unsigned i, unsigned j) {
    assert(i == j); (void)j;
    return this->data()[i];
  }
  inline T& operator() (unsigned i) { return this->data()[i]; }
  inline T const& operator() (unsigned i) const { return this->data()[i]; }

  inline T& operator[] (unsigned i) { return this->data()[i]; }
  inline T const& operator[] (unsigned i) const { return this->data()[i]; }

  //: Return a vector (copy) with the content of the (main) diagonal
  inline vnl_vector<T> get_diagonal() const { return vnl_vector<T>(this->diagonal()); }

  //: Return diagonal elements as a vector
  inline vnl_vector<T> const diagonal() const { return this->Superclass::diagonal(); }

  //: Set all diagonal elements of matrix to specified value.
  inline vnl_diag_matrix& fill_diagonal (T const& v) { this->fill(v); return *this; }

  //: Sets the diagonal elements of this matrix to the specified list of values.
  inline vnl_diag_matrix& set_diagonal(vnl_vector<T> const& v)
  {
    for(int d=0; d< this->rows(); ++d)
    {
      this->operator()(d) = v[d];
    }
    return *this;
  }

  // iterators

  typedef typename vnl_vector<T>::iterator iterator;
  inline iterator begin() { return this->data(); }
  inline iterator end() { return this->begin() + this->rows(); }
  typedef typename vnl_vector<T>::const_iterator const_iterator;
  inline const_iterator begin() const { return this->data(); }
  inline const_iterator end() const { return this->begin() + this->rows(); }
  inline const_iterator cbegin() const { return this->data(); }
  inline const_iterator cend() const { return this->cbegin() + this->rows(); }

  //: Return the total number of elements stored by the matrix.
  // Since vnl_diag_matrix only stores values on the diagonal
  // and must be square, size() == rows() == cols().
  // Size is the size of the diagonal for vnl, rather than the implied storage size from Eigen
  inline unsigned int size() const { return this->Superclass::rows(); }

  //: Return the number of rows.
  // baseclass inline unsigned int rows() const { return diagonal_.size(); }

  //: Return the number of columns.
  // A synonym for columns().
  // baseclass inline unsigned int cols() const { return diagonal_.size(); }

  //: Return the number of columns.
  // A synonym for cols().
  inline unsigned int columns() const { return this->size(); }

  //: set element with boundary checks.
  inline void put (unsigned r, unsigned c, T const& v) {
    assert(r == c);
    (void)c;
#if VNL_CONFIG_CHECK_BOUNDS
    if (r >= this->size())                  // If invalid size specified
      {
      vnl_error_matrix_row_index("get", r); // Raise exception
      }
#endif
      this->operator()(r) = v;
  }

  //: get element with boundary checks.
  inline T get (unsigned r, unsigned c) const {
    assert(r == c);
    (void)c;
#if VNL_CONFIG_CHECK_BOUNDS
    if (r >= this->size())                  // If invalid size specified
      {
      vnl_error_matrix_row_index("get", r); // Raise exception
      }
#endif
    return this->operator()(r);
  }

  // Need this until we add a vnl_diag_matrix ctor to vnl_matrix;
  vnl_matrix<T> as_matrix( ) const;

  // This is as good as a vnl_diag_matrix ctor for vnl_matrix:
  explicit operator vnl_matrix<T> () const { return this->as_matrix(); }

  inline void set_size(int n) { this->resize(n); }

  inline void clear() { this->clear(); }
  inline vnl_diag_matrix& fill(T const &x) { this->fill(x); return *this; }


  //: Return pointer to the diagonal elements as a contiguous 1D C array;
  inline T*       data_block()       { return this->Superclass::diagonal().data(); }
  inline T const* data_block() const { return this->Superclass::diagonal().data(); }

  inline T*       data()       { return this->Superclass::diagonal().data(); }
  inline T const* data() const { return this->Superclass::diagonal().data(); }


  //: Set diagonal elements using vector
  inline vnl_diag_matrix& set(vnl_vector<T> const& v)  {
    for(int d = 0; d < this->rows(); ++d)
    {
      this->operator()(d)=v[d];
    }
    return *this;
  }

 private:
};

//:
// \relatesalso vnl_diag_matrix
template <class T> VNL_EXPORT
std::ostream& operator<< (std::ostream&, vnl_diag_matrix<T> const&);

//: Convert a vnl_diag_matrix to a Matrix.
template <class T>
vnl_matrix<T> vnl_diag_matrix<T>::as_matrix() const
{
  const unsigned len = this->size();
  vnl_matrix<T> ret(len, len);
  for (unsigned i = 0; i < len; ++i)
  {
    unsigned j;
    for (j = 0; j < i; ++j)
      ret(i,j) = T(0);
    for (j = i+1; j < len; ++j)
      ret(i,j) = T(0);
    ret(i,i) = this->operator()(i);
  }
  return ret;
}

//: Invert a vnl_diag_matrix in-situ.
// Just replaces each element with its reciprocal.
template <class T>
inline vnl_diag_matrix<T>& vnl_diag_matrix<T>::invert_in_place()
{
  const unsigned len = this->size();
  T* d = data_block();
  T one = T(1);
  for (unsigned i = 0; i < len; ++i)
    d[i] = one / d[i];
  return *this;
}

////: Return determinant as product of diagonal values.
//template <class T>
//inline T vnl_diag_matrix<T>::determinant() const
//{
//  T det = T(1);
//  T const* d = data_block();
//  const unsigned len = this->size();
//  for (unsigned i = 0; i < len; ++i)
//    det *= d[i];
//  return det;
//}

//: Multiply two vnl_diag_matrices.  Just multiply the diag elements - n flops
// \relatesalso vnl_diag_matrix
template <class T>
inline vnl_diag_matrix<T> operator* (vnl_diag_matrix<T> const& A, vnl_diag_matrix<T> const& B)
{
  assert(A.size() == B.size());
  vnl_diag_matrix<T> ret = A;
  for (unsigned i = 0; i < A.size(); ++i)
    ret(i,i) *= B(i,i);
  return ret;
}

//: Multiply a vnl_matrix by a vnl_diag_matrix.  Just scales the columns - mn flops
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator* (vnl_matrix<T> const& A, vnl_diag_matrix<T> const& D)
{
  assert(A.columns() == D.rows());
  vnl_matrix<T> ret(A.rows(), A.columns());
  for (unsigned i = 0; i < A.rows(); ++i)
    for (unsigned j = 0; j < A.columns(); ++j)
      ret(i,j) = A(i,j) * D(j,j);
  return ret;
}

//: Multiply a vnl_diag_matrix by a vnl_matrix.  Just scales the rows - mn flops
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator* (vnl_diag_matrix<T> const& D, vnl_matrix<T> const& A)
{
  assert(A.rows() == D.cols());
  vnl_matrix<T> ret(A.rows(), A.columns());
  T const* d = D.data_block();
  for (unsigned i = 0; i < A.rows(); ++i)
    for (unsigned j = 0; j < A.columns(); ++j)
      ret(i,j) = A(i,j) * d[i];
  return ret;
}

//: Add two vnl_diag_matrices.  Just add the diag elements - n flops
// \relatesalso vnl_diag_matrix
template <class T>
inline vnl_diag_matrix<T> operator+ (vnl_diag_matrix<T> const& A, vnl_diag_matrix<T> const& B)
{
  assert(A.size() == B.size());
  vnl_diag_matrix<T> ret = A;
  for (unsigned i = 0; i < A.size(); ++i)
    ret(i,i) += B(i,i);
  return ret;
}

template <class T>
inline vnl_matrix<T> operator+ (vnl_diag_matrix<T> const& A, vnl_matrix_ref<T> const& B)
{
  assert(A.rows() == B.rows());
  assert(A.cols() == B.cols());
  vnl_matrix<T> ret{B};
  for (unsigned i = 0; i < A.size(); ++i)
    ret(i,i) += A(i);
  return ret;
}

template <class T>
inline vnl_matrix<T> operator+ (vnl_matrix_ref<T> const& Aref, vnl_diag_matrix<T> const& Bdiag)
{
  assert(Aref.rows() == Bdiag.rows());
  assert(Aref.cols() == Bdiag.cols());
  vnl_matrix<T> ret { Aref };
  for (unsigned i = 0; i < Bdiag.size(); ++i)
    ret(i,i) += Bdiag(i);
  return ret;
}

//: Add a vnl_diag_matrix to a vnl_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator+ (vnl_matrix<T> const& A, vnl_diag_matrix<T> const& D)
{
  const unsigned n = D.cols();
  assert(A.rows() == n); assert(A.columns() == n);
  vnl_matrix<T> ret(A);
  T const* d = D.data_block();
  for (unsigned j = 0; j < n; ++j)
    ret(j,j) += d[j];
  return ret;
}

//: Add a vnl_matrix to a vnl_diag_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator+ (vnl_diag_matrix<T> const& D, vnl_matrix<T> const& A)
{
  return A + D;
}

//: Subtract two vnl_diag_matrices.  Just subtract the diag elements - n flops
// \relatesalso vnl_diag_matrix
template <class T>
inline vnl_diag_matrix<T> operator- (vnl_diag_matrix<T> const& A, vnl_diag_matrix<T> const& B)
{
  assert(A.size() == B.size());
  vnl_diag_matrix<T> ret = A;
  for (unsigned i = 0; i < A.size(); ++i)
    ret(i,i) -= B(i,i);
  return ret;
}

//: Subtract a vnl_diag_matrix from a vnl_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator- (vnl_matrix<T> const& A, vnl_diag_matrix<T> const& D)
{
  const unsigned n = D.rows();
  assert(A.rows() == n); assert(A.columns() == n);
  vnl_matrix<T> ret(A);
  T const* d = D.data_block();
  for (unsigned j = 0; j < n; ++j)
    ret(j,j) -= d[j];
  return ret;
}

//: Subtract a vnl_diag_matrix from a vnl_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator- (vnl_matrix_ref<T> const& A, vnl_diag_matrix<T> const& D)
{
  const unsigned n = D.rows();
  assert(A.rows() == n); assert(A.columns() == n);
  vnl_matrix<T> ret(A);
  T const* d = D.data_block();
  for (unsigned j = 0; j < n; ++j)
    ret(j,j) -= d[j];
  return ret;
}


//: Subtract a vnl_diag_matrix from a vnl_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator- (vnl_diag_matrix<T> const& Ddiag, vnl_matrix_ref<T> const& Aref)
{
  const unsigned n = Ddiag.rows();
  assert(Aref.rows() == n); assert(Aref.columns() == n);
  vnl_matrix<T> ret(-Aref); // Negate first!
  T const* d = Ddiag.data_block();
  for (unsigned j = 0; j < n; ++j)
    ret(j,j) += d[j]; // Now add negative
  return ret;
}


//: Subtract a vnl_matrix from a vnl_diag_matrix.  n adds, mn copies.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T> operator- (vnl_diag_matrix<T> const& D, vnl_matrix<T> const& A)
{
  const unsigned n = D.rows();
  assert(A.rows() == n); assert(A.columns() == n);
  vnl_matrix<T> ret(n, n);
  T const* d = D.data_block();
  for (unsigned i = 0; i < n; ++i)
  {
    for (unsigned j = 0; j < i; ++j)
      ret(i,j) = -A(i,j);
    for (unsigned j = i+1; j < n; ++j)
      ret(i,j) = -A(i,j);
    ret(i,i) = d[i] - A(i,i);
  }
  return ret;
}

//: Multiply a vnl_diag_matrix by a vnl_vector.  n flops.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_vector
template <class T>
inline vnl_vector<T> operator* (vnl_diag_matrix<T> const& D, vnl_vector<T> const& A)
{
  assert(A.size() == D.rows());
  return element_product(D.diagonal(), A);
}

template <class T>
inline vnl_vector<T> operator* (vnl_diag_matrix<T> const& D, vnl_vector_ref<T> const& A)
{
  assert(A.size() == D.rows());
  vnl_vector<T> tmp{A};
  return element_product(D.diagonal(), tmp);
}

//: Multiply a vnl_vector by a vnl_diag_matrix.  n flops.
// \relatesalso vnl_diag_matrix
// \relatesalso vnl_vector
template <class T>
inline vnl_vector<T> operator* (vnl_vector<T> const& A, vnl_diag_matrix<T> const& D)
{
  assert(A.size() == D.rows());
  return element_product(D.diagonal(), A);
}

#endif // vnl_diag_matrix_h_
