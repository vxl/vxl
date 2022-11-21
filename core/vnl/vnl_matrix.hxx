// This is core/vnl/vnl_matrix.hxx
#ifndef vnl_matrix_hxx_
#define vnl_matrix_hxx_
//:
// \file
//
// Copyright (C) 1991 Texas Instruments Incorporated.
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated, General Electric Company,
// provides this software "as is" without express or implied warranty.
//
// Created: MBN Apr 21, 1989 Initial design and implementation
// Updated: MBN Jun 22, 1989 Removed non-destructive methods
// Updated: LGO Aug 09, 1989 Inherit from Generic
// Updated: MBN Aug 20, 1989 Changed template usage to reflect new syntax
// Updated: MBN Sep 11, 1989 Added conditional exception handling and base class
// Updated: LGO Oct 05, 1989 Don't re-allocate data in operator= when same size
// Updated: LGO Oct 19, 1989 Add extra parameter to varargs constructor
// Updated: MBN Oct 19, 1989 Added optional argument to set_compare method
// Updated: LGO Dec 08, 1989 Allocate column data in one chunk
// Updated: LGO Dec 08, 1989 Clean-up get and put, add const everywhere.
// Updated: LGO Dec 19, 1989 Remove the map and reduce methods
// Updated: MBN Feb 22, 1990 Changed size arguments from int to unsigned int
// Updated: MJF Jun 30, 1990 Added base class name to constructor initializer
// Updated: VDN Feb 21, 1992 New lite version
// Updated: VDN May 05, 1992 Use envelope to avoid unnecessary copying
// Updated: VDN Sep 30, 1992 Matrix inversion with singular value decomposition
// Updated: AWF Aug 21, 1996 set_identity, normalize_rows, scale_row.
// Updated: AWF Sep 30, 1996 set_row/column methods. Const-correct data_block().
// Updated: AWF 14 Feb 1997  get_n_rows, get_n_columns.
// Updated: PVR 20 Mar 1997  get_row, get_column.
//
// The parameterized vnl_matrix<T> class implements two dimensional arithmetic
// matrices of a user specified type. The only constraint placed on the type is
// that it must overload the following operators: +, -,  *,  and /. Thus, it
// will be possible to have a vnl_matrix over std::complex<T>. The vnl_matrix<T>
// class is static in size, that is once a vnl_matrix<T> of a particular size
// has been created, there is no dynamic growth method available. You can
// resize the matrix, with the loss of any existing data using set_size().
//
// Each matrix contains  a protected  data section  that has a T** slot that
// points to the  physical memory allocated  for the two  dimensional array. In
// addition, two integers  specify   the number  of  rows  and columns  for the
// matrix.  These values  are provided in the  constructors. A single protected
// slot  contains a pointer  to a compare  function  to   be used  in  equality
// operations. The default function used is the built-in == operator.
//
// Four  different constructors are provided.  The  first constructor takes two
// integer arguments  specifying the  row  and column  size.   Enough memory is
// allocated to hold row*column elements  of type Type.  The second constructor
// takes the  same two  first arguments, but  also accepts  an additional third
// argument that is  a reference to  an  object of  the appropriate  type whose
// value is used as an initial fill value.  The third constructor is similar to
// the third, except that it accepts a variable number of initialization values
// for the Matrix.  If there are  fewer values than elements,  the rest are set
// to zero. Finally, the last constructor takes a single argument consisting of
// a reference to a Matrix and duplicates its size and element values.
//
// Methods   are  provided   for destructive   scalar   and Matrix    addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for formatted input and output of matrix elements.
//
// Good matrix inversion is needed. We choose singular value decomposition,
// since it is general and works great for nearly singular cases. Singular
// value decomposition is preferred to LU decomposition, since the accuracy
// of the pivots is independent from the left->right top->down elimination.
// LU decomposition also does not give eigenvectors and eigenvalues when
// the matrix is symmetric.
//
// Several different constructors are provided. See .h file for brief descriptions.

//--------------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <vector>
#include <algorithm>
#include "vnl_matrix.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "vnl_math.h"
#include "vnl_vector.h"
#include "vnl_c_vector.h"
#include "vnl_numeric_traits.h"
//--------------------------------------------------------------------------------


//: r rows, c cols, special type.  Currently implements "identity" and "null".
template <class T>
vnl_matrix<T>::vnl_matrix(unsigned r, unsigned c, vnl_matrix_type t)
: Superclass(r,c)
{
  switch (t) {
   case vnl_matrix_identity:
    this->set_identity();
    break;
   case vnl_matrix_null:
    this->setConstant(T(0));
    break;
   default:
    assert(false);
    break;
  }
}

#if 1 // fsm: who uses this?
//: Creates a matrix with given dimension (rows, cols) and initialize first n elements, row-wise, to values. O(m*n).
template <class T>
vnl_matrix<T>::vnl_matrix (unsigned rowz, unsigned colz, unsigned n, T const values[])
: Superclass(rowz,colz)
{
  if (n > rowz*colz)
    n = rowz*colz;
  std::copy( values, values + n, this->data() );
}
#endif

//: Creates a matrix from a block array of data, stored row-wise.
// O(m*n).
template <class T>
vnl_matrix<T>::vnl_matrix (T const* datablck, unsigned rowz, unsigned colz)
: Superclass(rowz,colz)
{
  std::copy( datablck, datablck + rowz * colz, this->data() );
}

//: Frees up the dynamic storage used by matrix.
// O(m*n).

template <class T>
void
vnl_matrix<T>::destroy()
{
  this->resize(0,0);
}

template <class T>
void vnl_matrix<T>::clear()
{
  this->destroy();
}

// Resizes the data arrays of THIS matrix to (rows x cols). O(m*n).
// Elements are not initialized, existing data is not preserved.
// Returns true if size is changed.

template <class T>
bool vnl_matrix<T>::set_size (unsigned rowz, unsigned colz)
{
  this->resize(rowz,colz);
  return true;
}

//------------------------------------------------------------

//: Sets all diagonal elements of matrix to specified value. O(n).

template <class T>
vnl_matrix<T>& vnl_matrix<T>::fill_diagonal (T const& value)
{
  for (unsigned int i = 0; i < this->rows() && i < this->cols(); ++i)
    this->Superclass::operator()(i,i) = value;
  return *this;
}

//: Sets the diagonal elements of this matrix to the specified list of values.

template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_diagonal(vnl_vector<T> const& diag)
{
  assert(diag.size() >= this->rows() ||
         diag.size() >= this->cols());
  // The length of the diagonal of a non-square matrix is the minimum of
  // the matrix's width & height; that explains the "||" in the assert,
  // and the "&&" in the upper bound for the "for".
  for (unsigned int i = 0; i < this->rows() && i < this->cols(); ++i)
    this->Superclass::operator()(i,i) = diag(i);
  return *this;
}



//: Copies all elements of rhs matrix into lhs matrix. O(m*n).
// If needed, the arrays in lhs matrix are freed up, and new arrays are
// allocated to match the dimensions of the rhs matrix.

template <class T>
void vnl_matrix<T>::print(std::ostream& os) const
{
  for (unsigned int i = 0; i < this->rows(); i++) {
    for (unsigned int j = 0; j < this->columns(); j++)
      os << this->Superclass::operator()(i,j) << ' ';
    os << '\n';
  }
}

//: Prints the 2D array of elements of a matrix out to a stream.
// O(m*n).

template <class T>
std::ostream& operator<< (std::ostream& os, vnl_matrix<T> const& m)
{
  for (unsigned int i = 0; i < m.rows(); ++i) {
    for (unsigned int j = 0; j < m.columns(); ++j)
      os << m(i, j) << ' ';
    os << '\n';
  }
  return os;
}

//: Read a vnl_matrix from an ascii std::istream.
// Automatically determines file size if the input matrix has zero size.
template <class T>
std::istream& operator>>(std::istream& s, vnl_matrix<T>& M)
{
  M.read_ascii(s);
  return s;
}

//: Returns new matrix which is the negation of THIS matrix.
// O(m*n).

//: Return the matrix made by applying "f" to each element.
template <class T>
vnl_matrix<T> vnl_matrix<T>::apply(T (*f)(T const&)) const
{
  vnl_matrix<T> ret(this->rows(),this->cols());
  vnl_c_vector<T>::apply(this->data(), this->rows() * this->cols(), f, ret.data_block());
  return ret;
}

//: Return the matrix made by applying "f" to each element.
template <class T>
vnl_matrix<T> vnl_matrix<T>::apply(T (*f)(T)) const
{
  vnl_matrix<T> ret(this->rows(),this->cols());
  vnl_c_vector<T>::apply(this->data(), this->rows() * this->cols(), f, ret.data_block());
  return ret;
}

//: Make a vector by applying a function across rows.
template <class T>
vnl_vector<T>
vnl_matrix<T>
::apply_rowwise(T (*f)(vnl_vector<T> const&)) const
{
  vnl_vector<T> v(this->rows());
  for (unsigned int i = 0; i < this->rows(); ++i)
    v.put(i,f(this->get_row(i)));
  return v;
}

//: Make a vector by applying a function across columns.
template <class T>
vnl_vector<T>
vnl_matrix<T>
::apply_columnwise(T (*f)(vnl_vector<T> const&)) const
{
  vnl_vector<T> v(this->cols());
  for (unsigned int i = 0; i < this->cols(); ++i)
    v.put(i,f(this->get_column(i)));
  return v;
}


////--------------------------- Additions------------------------------------


// adjoint/hermitian transpose

template <class T>
vnl_matrix<T> vnl_matrix<T>::conjugate_transpose() const
{
  vnl_matrix<T> result(*this);
  result.transposeInPlace();
  vnl_c_vector<T>::conjugate(result.begin(),  // src
                             result.begin(),  // dst
                             result.size());  // size of block
  return result;
}

//: Replaces the submatrix of THIS matrix, starting at top left corner, by the elements of matrix m. O(m*n).
// This is the reverse of extract().

template <class T>
vnl_matrix<T>& vnl_matrix<T>::update (vnl_matrix<T> const& m,
                                      unsigned top, unsigned left)
{
  unsigned int bottom = top + m.rows();
  unsigned int right = left + m.cols();
#ifndef NDEBUG
  if (this->rows() < bottom || this->cols() < right)
    vnl_error_matrix_dimension ("update",
                                bottom, right, m.rows(), m.cols());
#endif
  for (unsigned int i = top; i < bottom; i++)
    for (unsigned int j = left; j < right; j++)
      this->Superclass::operator()(i,j) = m(i-top,j-left);
  return *this;
}


//: Returns a copy of submatrix of THIS matrix, specified by the top-left corner and size in rows, cols. O(m*n).
// Use update() to copy new values of this submatrix back into THIS matrix.

template <class T>
vnl_matrix<T> vnl_matrix<T>::extract (unsigned rowz, unsigned colz,
                                      unsigned top, unsigned left) const {
  vnl_matrix<T> result(rowz, colz);
  this->extract( result, top, left );
  return result;
}

template <class T>
void vnl_matrix<T>::extract( vnl_matrix<T>& submatrix,
                             unsigned top, unsigned left) const {
  unsigned const rowz = submatrix.rows();
  unsigned const colz = submatrix.cols();
#ifndef NDEBUG
  unsigned int bottom = top + rowz;
  unsigned int right = left + colz;
  if ((this->rows() < bottom) || (this->cols() < right))
    vnl_error_matrix_dimension ("extract",
                                this->rows(), this->cols(), bottom, right);
#endif
  for (unsigned int i = 0; i < rowz; i++)      // actual copy of all elements
    for (unsigned int j = 0; j < colz; j++)    // in submatrix
      submatrix(i,j) = this->Superclass::operator()(top+i , left+j);
}

//: Returns the dot product of the two matrices. O(m*n).
// This is the sum of all pairwise products of the elements m1[i,j]*m2[i,j].

template <class T>
T dot_product (vnl_matrix<T> const& m1, vnl_matrix<T> const& m2)
{
#ifndef NDEBUG
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("dot_product",
                                m1.rows(), m1.columns(),
                                m2.rows(), m2.columns());
#endif
  return vnl_c_vector<T>::dot_product(m1.cbegin(), m2.cbegin(), m1.rows()*m1.cols());
}

//: Hermitian inner product.
// O(mn).

template <class T>
T inner_product (vnl_matrix<T> const& m1, vnl_matrix<T> const& m2)
{
#ifndef NDEBUG
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("inner_product",
                                m1.rows(), m1.columns(),
                                m2.rows(), m2.columns());
#endif
  return vnl_c_vector<T>::inner_product(m1.cbegin(), m2.cbegin(), m1.rows()*m1.cols());
}

// cos_angle. O(mn).

template <class T>
T cos_angle (vnl_matrix<T> const& a, vnl_matrix<T> const& b)
{
  typedef typename vnl_numeric_traits<T>::abs_t Abs_t;
  typedef typename vnl_numeric_traits<Abs_t>::real_t abs_r;

  T ab = inner_product(a,b);
  Abs_t a_b = (Abs_t)std::sqrt( (abs_r)vnl_math::abs(inner_product(a,a) * inner_product(b,b)) );

  return T( ab / a_b);
}

//: Returns new matrix whose elements are the products m1[ij]*m2[ij].
// O(m*n).

template <class T>
vnl_matrix<T> element_product (vnl_matrix<T> const& m1,
                               vnl_matrix<T> const& m2)
{
#ifndef NDEBUG
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("element_product",
                                m1.rows(), m1.columns(), m2.rows(), m2.columns());
#endif
  vnl_matrix<T> result(m1.rows(), m1.columns());
  for (unsigned int i = 0; i < m1.rows(); i++)
    for (unsigned int j = 0; j < m1.columns(); j++)
      result.put(i,j, T(m1.get(i,j) * m2.get(i,j)) );
  return result;
}

//: Returns new matrix whose elements are the quotients m1[ij]/m2[ij].
// O(m*n).

template <class T>
vnl_matrix<T> element_quotient (vnl_matrix<T> const& m1,
                                vnl_matrix<T> const& m2)
{
#ifndef NDEBUG
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension("element_quotient",
                               m1.rows(), m1.columns(), m2.rows(), m2.columns());
#endif
  vnl_matrix<T> result(m1.rows(), m1.columns());
  for (unsigned int i = 0; i < m1.rows(); i++)
    for (unsigned int j = 0; j < m1.columns(); j++)
      result.put(i,j, T(m1.get(i,j) / m2.get(i,j)) );
  return result;
}

//: Fill this matrix with the given data.
//  We assume that p points to a contiguous rows*cols array, stored rowwise.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::copy_in(T const *p)
{
  std::copy( p, p + this->rows() * this->cols(), this->data() );
  return *this;
}

//: Fill the given array with this matrix.
//  We assume that p points to a contiguous rows*cols array, stored rowwise.
template <class T>
void vnl_matrix<T>::copy_out(T *p) const
{
  std::copy( this->data(), this->data() + this->rows() * this->cols(), p );
}

//: Fill this matrix with a matrix having 1s on the main diagonal and 0s elsewhere.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_identity()
{
  this->setZero();
  for(int i = 0; i < this->rows() && i < this->cols(); ++i)
  {
    this->Superclass::operator()(i,i) = 1;
  }
  return *this;
}

//: Make each row of the matrix have unit norm.
// All-zero rows are ignored.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::normalize_rows()
{
  typedef typename vnl_numeric_traits<T>::abs_t Abs_t;
  typedef typename vnl_numeric_traits<T>::real_t Real_t;
  typedef typename vnl_numeric_traits<Real_t>::abs_t abs_real_t;
  for (unsigned int i = 0; i < this->rows(); ++i) {  // For each row in the Matrix
    Abs_t norm(0); // double will not do for all types.
    for (unsigned int j = 0; j < this->cols(); ++j)  // For each element in row
      norm += vnl_math::squared_magnitude(this->Superclass::operator()(i,j));

    if (norm != 0) {
      abs_real_t scale = abs_real_t(1)/(std::sqrt((abs_real_t)norm));
      for (unsigned int j = 0; j < this->cols(); ++j)
        this->Superclass::operator()(i,j) = T(Real_t(this->Superclass::operator()(i,j)) * scale);
    }
  }
  return *this;
}

//: Make each column of the matrix have unit norm.
// All-zero columns are ignored.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::normalize_columns()
{
  typedef typename vnl_numeric_traits<T>::abs_t Abs_t;
  typedef typename vnl_numeric_traits<T>::real_t Real_t;
  typedef typename vnl_numeric_traits<Real_t>::abs_t abs_real_t;
  for (unsigned int j = 0; j < this->cols(); j++) {  // For each column in the Matrix
    Abs_t norm(0); // double will not do for all types.
    for (unsigned int i = 0; i < this->rows(); i++)
      norm += vnl_math::squared_magnitude(this->Superclass::operator()(i,j));

    if (norm != 0) {
      abs_real_t scale = abs_real_t(1)/(std::sqrt((abs_real_t)norm));
      for (unsigned int i = 0; i < this->rows(); i++)
        this->Superclass::operator()(i,j) = T(Real_t(this->Superclass::operator()(i,j)) * scale);
    }
  }
  return *this;
}

//: Multiply row[row_index] by value
template <class T>
vnl_matrix<T>& vnl_matrix<T>::scale_row(unsigned row_index, T value)
{
#ifndef NDEBUG
  if (row_index >= this->rows())
    vnl_error_matrix_row_index("scale_row", row_index);
#endif
  for (unsigned int j = 0; j < this->cols(); j++)    // For each element in row
    this->Superclass::operator()(row_index,j) *= value;
  return *this;
}

//: Multiply column[column_index] by value
template <class T>
vnl_matrix<T>& vnl_matrix<T>::scale_column(unsigned column_index, T value)
{
#ifndef NDEBUG
  if (column_index >= this->cols())
    vnl_error_matrix_col_index("scale_column", column_index);
#endif
  for (unsigned int j = 0; j < this->rows(); j++)    // For each element in column
    this->Superclass::operator()(j,column_index) *= value;
  return *this;
}

//: Returns a copy of n rows, starting from "row"
template <class T>
vnl_matrix<T> vnl_matrix<T>::get_n_rows (unsigned row, unsigned n) const
{
  Superclass temp= this->block(row,0,row+n,this->cols());
  return temp;
}

//: Returns a copy of n columns, starting from "column".
template <class T>
vnl_matrix<T> vnl_matrix<T>::get_n_columns (unsigned column, unsigned n) const
{
#ifndef NDEBUG
  if (column + n > this->cols())
    vnl_error_matrix_col_index ("get_n_columns", column);
#endif

  vnl_matrix<T> result(this->rows(), n);
  for (unsigned int c = 0; c < n; ++c)
    for (unsigned int r = 0; r < this->rows(); ++r)
      result(r, c) = this->Superclass::operator()(r,column + c);
  return result;
}

//: Create a vector out of row[row_index].
template <class T>
vnl_vector<T> vnl_matrix<T>::get_row(unsigned row_index) const
{
#ifdef ERROR_CHECKING
  if (row_index >= this->rows())
    vnl_error_matrix_row_index ("get_row", row_index);
#endif

  vnl_vector<T> v(this->cols());
  for (unsigned int j = 0; j < this->cols(); j++)    // For each element in row
    v[j] = this->Superclass::operator()(row_index,j);
  return v;
}

//: Create a vector out of column[column_index].
template <class T>
vnl_vector<T> vnl_matrix<T>::get_column(unsigned column_index) const
{
#ifdef ERROR_CHECKING
  if (column_index >= this->cols())
    vnl_error_matrix_col_index ("get_column", column_index);
#endif

  vnl_vector<T> v(this->rows());
  for (unsigned int j = 0; j < this->rows(); j++)    // For each element in row
    v[j] = this->Superclass::operator()(j,column_index);
  return v;
}

//: Create a vector out of row[row_index].
template <class T>
vnl_matrix<T>
vnl_matrix<T>
::get_rows(vnl_vector<unsigned int> i) const
{
  vnl_matrix<T> m(i.size(), this->cols());
  for (unsigned int j = 0; j < i.size(); ++j)
    m.set_row(j, this->get_row(i.get(j)));
  return m;
}

//: Create a vector out of column[column_index].
template <class T>
vnl_matrix<T>
vnl_matrix<T>
::get_columns(vnl_vector<unsigned int> i) const
{
  vnl_matrix<T> m(this->rows(), i.size());
  for (unsigned int j = 0; j < i.size(); ++j)
    m.set_column(j, this->get_column(i.get(j)));
  return m;
}

//: Return a vector with the content of the (main) diagonal
template <class T>
vnl_vector<T> vnl_matrix<T>::get_diagonal() const
{
  vnl_vector<T> v(this->rows() < this->cols() ? this->rows() : this->cols());
  for (unsigned int j = 0; j < this->rows() && j < this->cols(); ++j)
    v[j] = this->Superclass::operator()(j,j);
  return v;
}

//: Flatten row-major (C-style)
template <class T>
vnl_vector<T> vnl_matrix<T>::flatten_row_major() const
{
  vnl_vector<T> v(this->rows() * this->cols());
  v.copy_in(this->data_block());
  return v;
}

//: Flatten column-major (Fortran-style)
template <class T>
vnl_vector<T> vnl_matrix<T>::flatten_column_major() const
{
  vnl_vector<T> v(this->rows() * this->cols());
  for (unsigned int c = 0; c < this->cols(); ++c)
    for (unsigned int r = 0; r < this->rows(); ++r)
      v(c*this->rows()+r) = this->Superclass::operator()(r,c);
  return v;
}

//--------------------------------------------------------------------------------

//: Set row[row_index] to data at given address. No bounds check.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_row(unsigned row_index, T const *v)
{
  for (unsigned int j = 0; j < this->cols(); j++)    // For each element in row
    this->Superclass::operator()(row_index,j) = v[j];
  return *this;
}

//: Set row[row_index] to given vector.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_row(unsigned row_index, vnl_vector<T> const &v)
{
#ifndef NDEBUG
  if (v.size() != this->cols())
    vnl_error_vector_dimension ("vnl_matrix::set_row", v.size(), this->cols());
#endif
  set_row(row_index,v.data());
  return *this;
}

//: Set row[row_index] to given value.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_row(unsigned row_index, T v)
{
  for (unsigned int j = 0; j < this->cols(); j++)    // For each element in row
    this->Superclass::operator()(row_index,j) = v;
  return *this;
}

//--------------------------------------------------------------------------------

//: Set column[column_index] to data at given address.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_column(unsigned column_index, T const *v)
{
  for (unsigned int i = 0; i < this->rows(); i++)    // For each element in row
    this->Superclass::operator()(i,column_index) = v[i];
  return *this;
}

//: Set column[column_index] to given vector.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_column(unsigned column_index, vnl_vector<T> const &v)
{
#ifndef NDEBUG
  if (v.size() != this->rows())
    vnl_error_vector_dimension ("vnl_matrix::set_column", v.size(), this->rows());
#endif
  set_column(column_index,v.data());
  return *this;
}

//: Set column[column_index] to given value.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_column(unsigned column_index, T v)
{
  for (unsigned int j = 0; j < this->rows(); j++)    // For each element in row
    this->Superclass::operator()(j,column_index) = v;
  return *this;
}


//: Set columns starting at starting_column to given matrix
template <class T>
vnl_matrix<T>& vnl_matrix<T>::set_columns(unsigned starting_column, vnl_matrix<T> const& m)
{
#ifndef NDEBUG
  if (this->rows() != m.rows() ||
      this->cols() < m.cols() + starting_column)           // Size match?
    vnl_error_matrix_dimension ("set_columns",
                                this->rows(), this->cols(),
                                m.rows(), m.cols());
#endif

  for (unsigned int j = 0; j < m.cols(); ++j)
    for (unsigned int i = 0; i < this->rows(); i++)    // For each element in row
      this->Superclass::operator()(i,starting_column + j) = m(i,j);
  return *this;
}

//--------------------------------------------------------------------------------

//: Two matrices are equal if and only if they have the same dimensions and the same values.
// O(m*n).
// Elements are compared with operator== as default.
// Change this default with set_compare() at run time or by specializing
// vnl_matrix_compare at compile time.

template <class T>
bool vnl_matrix<T>::operator_eq(vnl_matrix<T> const& rhs) const
{
  if (this == &rhs)                                      // same object => equal.
    return true;

  if (this->rows() != rhs.rows() || this->cols() != rhs.cols())
    return false;                                        // different sizes => not equal.

  for (unsigned int i = 0; i < this->rows(); i++)     // For each row
    for (unsigned int j = 0; j < this->cols(); j++)   // For each column
      if (this->Superclass::operator()(i,j) != rhs(i,j))            // different element ?
        return false;                                    // Then not equal.

  return true;                                           // Else same; return true
}

template <class T>
bool vnl_matrix<T>::is_equal(vnl_matrix<T> const& rhs, double tol) const
{
  if (this == &rhs)                                      // same object => equal.
    return true;

  if (this->rows() != rhs.rows() || this->cols() != rhs.cols())
    return false;                                        // different sizes => not equal.

  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j)
      if (vnl_math::abs(this->Superclass::operator()(i,j) - rhs(i,j)) > tol)
        return false;                                    // difference greater than tol

  return true;
}


template <class T>
bool vnl_matrix<T>::is_identity() const
{
  T const zero(0);
  T const one(1);
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j) {
      T xm = this->Superclass::operator()(i,j);
      if ( !((i == j) ? (xm == one) : (xm == zero)) )
        return false;
    }
  return true;
}

//: Return true if maximum absolute deviation of M from identity is <= tol.
template <class T>
bool vnl_matrix<T>::is_identity(double tol) const
{
  T one(1);
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j) {
      T xm = this->Superclass::operator()(i,j);
      abs_t absdev = (i == j) ? vnl_math::abs(xm - one) : vnl_math::abs(xm);
      if (absdev > tol)
        return false;
    }
  return true;
}

template <class T>
bool vnl_matrix<T>::is_zero() const
{
  T const zero(0);
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j)
      if ( !( this->Superclass::operator()(i, j) == zero) )
        return false;

  return true;
}

//: Return true if max(abs((*this))) <= tol.
template <class T>
bool vnl_matrix<T>::is_zero(double tol) const
{
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j)
      if (vnl_math::abs(this->Superclass::operator()(i,j)) > tol)
        return false;

  return true;
}

//: Return true if any element of (*this) is nan
template <class T>
bool vnl_matrix<T>::has_nans() const
{
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j)
      if (vnl_math::isnan(this->Superclass::operator()(i,j)))
        return true;

  return false;
}

//: Return false if any element of (*this) is inf or nan
template <class T>
bool vnl_matrix<T>::is_finite() const
{
  for (unsigned int i = 0; i < this->rows(); ++i)
    for (unsigned int j = 0; j < this->columns(); ++j)
      if (!vnl_math::isfinite(this->Superclass::operator()(i,j)))
        return false;

  return true;
}

//: Abort if any element of M is inf or nan
template <class T>
void vnl_matrix<T>::assert_finite_internal() const
{
  if (is_finite())
    return;

  std::cerr << "\n\n" __FILE__ ": " << __LINE__ << ": matrix has non-finite elements\n";

  if (this->rows() <= 20 && this->cols() <= 20) {
    std::cerr << __FILE__ ": here it is:\n" << *this;
  }
  else {
    std::cerr << __FILE__ ": it is quite big (" << this->rows() << 'x' << this->cols() << ")\n"
             << __FILE__ ": in the following picture '-' means finite and '*' means non-finite:\n";

    for (unsigned int i=0; i<this->rows(); ++i) {
      for (unsigned int j=0; j<this->cols(); ++j)
        std::cerr << char(vnl_math::isfinite(this->Superclass::operator()(i, j)) ? '-' : '*');
      std::cerr << '\n';
    }
  }
  std::cerr << __FILE__ ": calling abort()\n";
  std::abort();
}

//: Abort unless M has the given size.
template <class T>
void vnl_matrix<T>::assert_size_internal(unsigned rs,unsigned cs) const
{
  if (this->rows()!=rs || this->cols()!=cs) {
    std::cerr << __FILE__ ": size is " << this->rows() << 'x' << this->cols()
             << ". should be " << rs << 'x' << cs << std::endl;
    std::abort();
  }
}

//: Read a vnl_matrix from an ascii std::istream.
// Automatically determines file size if the input matrix has zero size.
template <class T>
bool vnl_matrix<T>::read_ascii(std::istream& s)
{
  if (!s.good()) {
    std::cerr << __FILE__ ": vnl_matrix<T>::read_ascii: Called with bad stream\n";
    return false;
  }

  bool size_known = (this->rows() != 0);

  if (size_known) {
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        s >> this->Superclass::operator()(i,j);

    return s.good() || s.eof();
  }

  bool debug = false;

  std::vector<T> first_row_vals;
  if (debug)
    std::cerr << __FILE__ ": vnl_matrix<T>::read_ascii: Determining file dimensions: ";

  for (;;) {
    // Clear whitespace, looking for a newline
    while (true)
    {
      int c = s.get();
      if (c == EOF)
        goto loademup;
      if (!std::isspace(c)) {
        if (!s.putback(char(c)).good())
          std::cerr << "vnl_matrix<T>::read_ascii: Could not push back '" << c << "'\n";

        goto readfloat;
      }
      // First newline after first number tells us the column dimension
      if (c == '\n' && first_row_vals.size() > 0) {
        goto loademup;
      }
    }
  readfloat:
    T val;
    s >> val;
    if (!s.fail())
      first_row_vals.push_back(val);
    if (s.eof())
      goto loademup;
  }
 loademup:
  std::size_t colz = first_row_vals.size();

  if (debug) std::cerr << colz << " cols, ";

  if (colz == 0)
    return false;

  // need to be careful with resizing here as will often be reading humungous files
  // So let's just build an array of row pointers
  std::vector<T*> row_vals;
  row_vals.reserve(1000);
  {
    // Copy first row.  Can't use first_row_vals, as may be a vector of bool...
    T* row = vnl_c_vector<T>::allocate_T(colz);
    for (unsigned int k = 0; k < colz; ++k)
      row[k] = first_row_vals[k];
    row_vals.push_back(row);
  }

  while (true)
  {
    T* row = vnl_c_vector<T>::allocate_T(colz);
    if (row == nullptr) {
      std::cerr << "vnl_matrix<T>::read_ascii: Error, Out of memory on row "
               << row_vals.size() << std::endl;
      return false;
    }
    s >> row[0];
    if (!s.good())
    {
      vnl_c_vector<T>::deallocate(row, colz);
      break;
    }
    for (unsigned int k = 1; k < colz; ++k) {
      if (s.eof()) {
        std::cerr << "vnl_matrix<T>::read_ascii: Error, EOF on row "
                 << row_vals.size() << ", column " << k << std::endl;

        return false;
      }
      s >> row[k];
      if (s.fail()) {
        std::cerr << "vnl_matrix<T>::read_ascii: Error, row "
                 << row_vals.size() << " failed on column " << k << std::endl;
        return false;
      }
    }
    row_vals.push_back(row);
  }

  std::size_t rowz = row_vals.size();

  if (debug)
    std::cerr << rowz << " rows.\n";

  set_size((unsigned int)rowz, (unsigned int)colz);

  T* p = this->data();
  for (unsigned int i = 0; i < rowz; ++i) {
    for (unsigned int j = 0; j < colz; ++j)
      *p++ = row_vals[i][j];
    /*if (i>0)*/ vnl_c_vector<T>::deallocate(row_vals[i], colz);
  }

  return true;
}

//: Read a vnl_matrix from an ascii std::istream.
// Automatically determines file size if the input matrix has zero size.
// This is a static method so you can type
// <verb>
// vnl_matrix<float> M = vnl_matrix<float>::read(cin);
// </verb>
// which many people prefer to the ">>" alternative.
template <class T>
vnl_matrix<T> vnl_matrix<T>::read(std::istream& s)
{
  vnl_matrix<T> M;
  s >> M;
  return M;
}

template <class T>
void vnl_matrix<T>::swap(vnl_matrix<T> &that) noexcept
{
  vnl_matrix<T> temp {*this};
  *this= that;
  that = temp;
}

//: Reverse order of rows.  Name is from Matlab, meaning "flip upside down".
template <class T>
vnl_matrix<T>& vnl_matrix<T>::flipud()
{
  const unsigned int n = this->rows();
  const unsigned int colz = this->columns();

  const unsigned int m = n / 2;
  for (unsigned int r = 0; r < m; ++r) {
    const unsigned int r1 = r;
    const unsigned int r2 = n - 1 - r;
    for (unsigned int c = 0; c < colz; ++c) {
      const T tmp = this->Superclass::operator()(r1, c);
      this->Superclass::operator()(r1, c) = this->Superclass::operator()(r2, c);
      this->Superclass::operator()(r2, c) = tmp;
    }
  }
  return *this;
}

//: Reverse order of columns.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::fliplr()
{
  const unsigned int n = this->cols();
  const unsigned int rowz = this->rows();

  const unsigned int m = n / 2;
  for (unsigned int c = 0; c < m; ++c) {
    const unsigned int c1 = c;
    const unsigned int c2 = n - 1 - c;
    for (unsigned int r = 0; r < rowz; ++r) {
      const T tmp = this->Superclass::operator()(r, c1);
      this->Superclass::operator()(r, c1) = this->Superclass::operator()(r, c2);
      this->Superclass::operator()(r, c2) = tmp;
    }
  }
  return *this;
}

// || M ||  = \max \sum | M   |
//        1     j    i     ij
template <class T>
typename vnl_matrix<T>::abs_t vnl_matrix<T>::operator_one_norm() const
{
  abs_t max = 0;
  for (unsigned int j=0; j<this->cols(); ++j) {
    abs_t tmp = 0;
    for (unsigned int i=0; i<this->rows(); ++i)
      tmp += vnl_math::abs(this->Superclass::operator()(i,j));
    if (tmp > max)
      max = tmp;
  }
  return max;
}

// || M ||   = \max \sum | M   |
//        oo     i    j     ij
template <class T>
typename vnl_matrix<T>::abs_t vnl_matrix<T>::operator_inf_norm() const
{
  abs_t max = 0;
  for (unsigned int i=0; i<this->rows(); ++i) {
    abs_t tmp = 0;
    for (unsigned int j=0; j<this->cols(); ++j)
      tmp += vnl_math::abs(this->Superclass::operator()(i,j));
    if (tmp > max)
      max = tmp;
  }
  return max;
}

template <class doublereal>              // ideally, char* should be bool* - PVr
int vnl_inplace_transpose(doublereal *a, unsigned m, unsigned n, char* move, unsigned iwrk)
{
  doublereal b, c;
  int k = m * n - 1;
  int iter, i1, i2, im, i1c, i2c, ncount, max_;

// *****
//  ALGORITHM 380 - REVISED
// *****
//  A IS A ONE-DIMENSIONAL ARRAY OF LENGTH MN=M*N, WHICH
//  CONTAINS THE MXN MATRIX TO BE TRANSPOSED (STORED
//  COLUMNWISE). MOVE IS A ONE-DIMENSIONAL ARRAY OF LENGTH IWRK
//  USED TO STORE INFORMATION TO SPEED UP THE PROCESS.  THE
//  VALUE IWRK=(M+N)/2 IS RECOMMENDED. IOK INDICATES THE
//  SUCCESS OR FAILURE OF THE ROUTINE.
//  NORMAL RETURN  IOK=0
//  ERRORS         IOK=-2 ,IWRK NEGATIVE OR ZERO
//                 IOK.GT.0, (SHOULD NEVER OCCUR),IN THIS CASE
//  WE SET IOK EQUAL TO THE FINAL VALUE OF ITER WHEN THE SEARCH
//  IS COMPLETED BUT SOME LOOPS HAVE NOT BEEN MOVED
//  NOTE * MOVE(I) WILL STAY ZERO FOR FIXED POINTS

  if (m < 2 || n < 2)
    return 0; // JUST RETURN IF MATRIX IS SINGLE ROW OR COLUMN
  if (iwrk < 1)
    return -2; // ERROR RETURN
  if (m == n) {
    // IF MATRIX IS SQUARE, EXCHANGE ELEMENTS A(I,J) AND A(J,I).
    for (unsigned i = 0; i < n; ++i)
    for (unsigned j = i+1; j < n; ++j) {
      i1 = i + j * n;
      i2 = j + i * m;
      b = a[i1];
      a[i1] = a[i2];
      a[i2] = b;
    }
    return 0; // NORMAL RETURN
  }
  ncount = 2;
  for (unsigned i = 0; i < iwrk; ++i)
    move[i] = char(0); // false;
  if (m > 2 && n > 2) {
    // CALCULATE THE NUMBER OF FIXED POINTS, EUCLIDS ALGORITHM FOR GCD(M-1,N-1).
    int ir2 = m - 1;
    int ir1 = n - 1;
    int ir0 = ir2 % ir1;
    while (ir0 != 0) {
      ir2 = ir1;
      ir1 = ir0;
      ir0 = ir2 % ir1;
    }
    ncount += ir1 - 1;
  }
// SET INITIAL VALUES FOR SEARCH
  iter = 1;
  im = m;
// AT LEAST ONE LOOP MUST BE RE-ARRANGED
  goto L80;
// SEARCH FOR LOOPS TO REARRANGE
L40:
  max_ = k - iter;
  ++iter;
  if (iter > max_)
    return iter; // error return
  im += m;
  if (im > k)
    im -= k;
  i2 = im;
  if (iter == i2)
    goto L40;
  if (iter <= (int)iwrk) {
    if (move[iter-1])
      goto L40;
    else
      goto L80;
  }
  while (i2 > iter && i2 < max_) {
    i1 = i2;
    i2 = m * i1 - k * (i1 / n);
  }
  if (i2 != iter)
    goto L40;
// REARRANGE THE ELEMENTS OF A LOOP AND ITS COMPANION LOOP
L80:
  i1 = iter;
  b = a[i1];
  i1c = k - iter;
  c = a[i1c];
  while (true) {
    i2 = m * i1 - k * (i1 / n);
    i2c = k - i2;
    if (i1 <= (int)iwrk)
      move[i1-1] = '1'; // true;
    if (i1c <= (int)iwrk)
      move[i1c-1] = '1'; // true;
    ncount += 2;
    if (i2 == iter)
      break;
    if (i2+iter == k) {
      doublereal d = b; b = c; c = d; // interchange b and c
      break;
    }
    a[i1] = a[i2];
    a[i1c] = a[i2c];
    i1 = i2;
    i1c = i2c;
  }
// FINAL STORE AND TEST FOR FINISHED
  a[i1] = b;
  a[i1c] = c;
  if (ncount > k)
    return 0; // NORMAL RETURN
  goto L40;
} /* dtrans_ */


//: Transpose matrix M in place.
//  Works for rectangular matrices using an enormously clever algorithm from ACM TOMS.
template <class T>
vnl_matrix<T>& vnl_matrix<T>::inplace_transpose()
{
  this->transposeInPlace();
  return *this;
}

//------------------------------------------------------------------------------

#define VNL_MATRIX_INSTANTIATE(T) \
template class VNL_EXPORT vnl_matrix<T >; \
template VNL_EXPORT T dot_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template VNL_EXPORT T inner_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template VNL_EXPORT T cos_angle(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template VNL_EXPORT vnl_matrix<T > element_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template VNL_EXPORT vnl_matrix<T > element_quotient(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template VNL_EXPORT int vnl_inplace_transpose(T*, unsigned, unsigned, char*, unsigned); \
template VNL_EXPORT std::ostream & operator<<(std::ostream &, vnl_matrix<T > const &); \
template VNL_EXPORT std::istream & operator>>(std::istream &, vnl_matrix<T >       &)

#endif // vnl_matrix_hxx_
