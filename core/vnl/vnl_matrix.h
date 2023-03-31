// This is core/vnl/vnl_matrix.h
#ifndef vnl_matrix_h_
#define vnl_matrix_h_
//:
// \file
// \brief An ordinary mathematical matrix
// \verbatim
//  Modifications
//   Apr 21, 1989 - MBN - Initial design and implementation
//   Jun 22, 1989 - MBN - Removed non-destructive methods
//   Aug 09, 1989 - LGO - Inherit from Generic
//   Aug 20, 1989 - MBN - Changed template usage to reflect new syntax
//   Sep 11, 1989 - MBN - Added conditional exception handling and base class
//   Oct 05, 1989 - LGO - Don't re-allocate data in operator= when same size
//   Oct 19, 1989 - LGO - Add extra parameter to varargs constructor
//   Oct 19, 1989 - MBN - Added optional argument to set_compare method
//   Dec 08, 1989 - LGO - Allocate column data in one chunk
//   Dec 08, 1989 - LGO - Clean-up get and put, add const everywhere.
//   Dec 19, 1989 - LGO - Remove the map and reduce methods
//   Feb 22, 1990 - MBN - Changed size arguments from int to unsigned int
//   Jun 30, 1990 - MJF - Added base class name to constructor initializer
//   Feb 21, 1992 - VDN - New lite version
//   May 05, 1992 - VDN - Use envelope to avoid unnecessary copying
//   Sep 30, 1992 - VDN - Matrix inversion with singular value decomposition
//   Aug 21, 1996 - AWF - set_identity, normalize_rows, scale_row.
//   Sep 30, 1996 - AWF - set_row/column methods. Const-correct data_block().
//   20 Mar 1997  - PVR - get_row, get_column.
//   24-Oct-2010 - Peter Vanroose - mutators and filling methods now return *this
//   18-Jan-2011 - Peter Vanroose - added methods set_diagonal() & get_diagonal()
// \endverbatim

#include <iosfwd>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vnl_c_vector.h"
#include <vnl/vnl_config.h>
#include "vnl_error.h"
#ifndef NDEBUG
# if VNL_CONFIG_CHECK_BOUNDS
#include <cassert>
# endif
#else
# undef VNL_CONFIG_CHECK_BOUNDS
# define VNL_CONFIG_CHECK_BOUNDS 0
# undef ERROR_CHECKING
#endif
#include "vnl/vnl_export.h"

#include <vnl/vnl_vector.h>
#include <numeric>

//--------------------------------------------------------------------------------

enum VNL_EXPORT vnl_matrix_type
{
  vnl_matrix_null,
  vnl_matrix_identity
};

//:  An ordinary mathematical matrix
// The vnl_matrix<T> class implements two-dimensional arithmetic
// matrices  for  a user-specified numeric data type. Using the
// parameterized types facility of C++,  it  is  possible,  for
// example, for the user to create a matrix of rational numbers
// by parameterizing the vnl_matrix class over the Rational  class.
// The  only  requirement  for the type is that it supports the
// basic arithmetic operators.
//
// Note: Unlike   the   other   sequence   classes,   the
// vnl_matrix<T>  class is fixed-size. It will not grow once the
// size has been specified to the constructor or changed by the
// assignment  or  multiplication  operators.  The vnl_matrix<T>
// class is row-based with addresses of rows being cached,  and
// elements accessed as m[row][col].
//
// Note: The matrix can, however, be resized using the set_size(nr,nc) function.
//
// Note: Indexing of the matrix is zero-based, so the top-left element is M(0,0).
//
// Note: Inversion of matrix M, and other operations such as solving systems of linear
// equations are handled by the matrix decomposition classes in vnl/algo, such
// as matrix_inverse, svd, qr etc.

#if 1
template <class T>
using eigen_vnl_matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajorBit>;


template <class T>
using vnl_matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajorBit>;

template<class T, const Eigen::Index r, const Eigen::Index c>
vnl_matrix<T> make_initialized_matrix(const Eigen::Index max_size, const T * const values)
{
  if(r*c > max_size)
  {
    T full_size[r*c]={}; // Zero initialize
    std::copy_n(values, max_size, full_size);
    const Eigen::Map<Eigen::Matrix<T, r, c> > matrix_with_values( full_size, r, c);
    vnl_matrix<T> output = matrix_with_values;
    return output;
  }
  const Eigen::Map<Eigen::Matrix<T, r, c> > matrix_with_values( const_cast<T *>(values), r, c);
  vnl_matrix<T> output = matrix_with_values;
  return output;
}


// non-member arithmetical operators.
//
////:
//// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T>
operator*(T const & value, vnl_matrix<T> const & m)
{
  return m.operator*(value);
}

// template<class T>
// inline vnl_matrix<T> operator*( vnl_matrix<T> const& m,T const& value)
//{
//   vnl_matrix<T> out {m};
//   return out*=value;
// }
//
////:
//// \relatesalso vnl_matrix
template <class T>
inline vnl_matrix<T>
operator+(T const & value, vnl_matrix<T> const & m)
{
  return m.operator+(value);
}

template <class T>
inline vnl_matrix<T>
operator-(T const & value, vnl_matrix<T> const & m)
{
  return m.operator-().operator+(value);
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
  return vnl_c_vector<T>::dot_product(m1.data(), m2.data(), m1.rows()*m1.cols());
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
  return vnl_c_vector<T>::inner_product(m1.data(), m2.data(), m1.rows()*m1.cols());
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

//: Read a vnl_matrix from an ascii std::istream.
// Automatically determines file size if the input matrix has zero size.
template <class T>
std::istream& operator>>(std::istream& s, vnl_matrix<T>& M)
{
  M.read_ascii(s);
  return s;
}

#else

template <class T>
class VNL_EXPORT vnl_matrix : public eigen_vnl_matrix<T>
{
protected:
  using Superclass = eigen_vnl_matrix<T>;
public:
  //: Default constructor creates an empty matrix of size 0,0.
  vnl_matrix() = default;

    template<typename Derived>
    vnl_matrix(const Eigen::MatrixBase<Derived> & rhs)
      : Superclass(rhs)
    {}

  //: Construct a matrix of size r rows by c columns
  // Contents are unspecified.
  // Complexity $O(1)$
  vnl_matrix(unsigned r, unsigned c)
    : Superclass(r, c)
  {}

  //: Construct a matrix of size r rows by c columns, and all elements equal to v0
  // Complexity $O(r.c)$
  vnl_matrix(unsigned r, unsigned c, T const & v0)
    : Superclass(r, c)
  {
    this->setConstant(v0);
  }

  // Provide the baseclass object
  Superclass &
  asEigen()
  {
    return *this;
  }

  //: Construct a matrix of size r rows by c columns, with a special type
  // Contents are specified by t
  // Complexity $O(r.c)$
  vnl_matrix(unsigned r, unsigned c, vnl_matrix_type t); // r rows, c cols, special type

  //: Construct a matrix of size r rows by c columns, initialised by an automatic array
  // The first n elements, are initialised row-wise, to values.
  // Complexity $O(n)$
  vnl_matrix(unsigned r, unsigned c, unsigned n, T const values[]); // use automatic arrays.

  //: Construct a matrix of size r rows by c columns, initialised by a memory block
  // The values are initialise row wise from the data.
  // Complexity $O(r.c)$
  vnl_matrix(T const * data_block, unsigned r, unsigned c); // fill row-wise.

  //: Copy construct a matrix
  // Complexity $O(r.c)$
  // vnl_matrix(vnl_matrix<T> const &) = default; // from another matrix.
  // NOTE: move-assignment must be allowed to throw an exception, because we need to maintain
  //       backwards compatibility and the move-construction & move-aasignment
  //       operators fall back to the copy-assignment operator behavior in
  //       cases when the memory is externally managed.
  //: Move-constructor.
  // vnl_matrix(vnl_matrix<T> &&) = default;
  //: Move-assignment operator
  // vnl_matrix<T> & operator=(vnl_matrix<T> & rhs) = default;
  // vnl_matrix<T> & operator=(vnl_matrix<T> && rhs) = default;

  template<typename Derived>
  vnl_matrix<T> &
  operator=(const Eigen::MatrixBase<Derived> & rhs)
  {
      this->Superclass::operator=(rhs);
      return *this;
  }

  //: Matrix destructor
  virtual ~vnl_matrix() = default;

  // Basic 2D-Array functionality-------------------------------------------

  //: Return the total number of elements stored by the matrix.
  // This equals rows() * cols()
  // baseclass inline unsigned int size() const { return this->num_rows*this->num_cols; }

  //: Return the number of rows.
  // baseclassinline unsigned int rows() const { return this->num_rows; }

  //: Return the number of columns.
  // A synonym for columns().
  // baseclassinline unsigned int cols() const { return this->cols(); }

  //: Return the number of columns.
  // A synonym for cols().
  inline unsigned int
  columns() const
  {
    return this->cols();
  }

  //: set element with boundary checks if error checking is on.
  inline void
  put(unsigned r, unsigned c, T const &);

  //: get element with boundary checks if error checking is on.
  inline T
  get(unsigned r, unsigned c) const;

  //: return pointer to given row
  // No boundary checking here.
  T *
  operator[](unsigned r)
  {
    return &(this->data()[r * this->columns()]);
  }

  //: return pointer to given row
  // No boundary checking here.
  T const *
  operator[](unsigned r) const
  {
    return &(this->data()[r * this->columns()]);
  }

  //: Access an element for reading or writing
  // There are assert style boundary checks - #define NDEBUG to turn them off.
  T &
  operator()(unsigned r, unsigned c)
  {
#if VNL_CONFIG_CHECK_BOUNDS
    assert(r < rows());   // Check the row index is valid
    assert(c < cols());   // Check the column index is valid
#endif
    return this->Superclass::operator()(r,c);
  }

  //: Access an element for reading
  // There are assert style boundary checks - #define NDEBUG to turn them off.
  T const &
  operator()(unsigned r, unsigned c) const
  {
#if VNL_CONFIG_CHECK_BOUNDS
    assert(r < rows());   // Check the row index is valid
    assert(c < cols());   // Check the column index is valid
#endif
    return this->Superclass::operator()(r,c);
  }

    // ----------------------- Filling and copying -----------------------

  //: Sets all elements of matrix to specified value, and returns "*this".
  //  Complexity $O(r.c)$
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a matrix to a column-normalized all-elements-equal matrix, say
  //  \code
  //     M.fill(1).normalize_columns();
  //  \endcode
  //  Returning "*this" also allows passing such a matrix as argument
  //  to a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(5,5,1.0).normalize_columns());
  //  \endcode
  vnl_matrix &
  fill(T const & v)
  {
    this->Superclass::fill(v);
    return *this;
  }

  //: Sets all diagonal elements of matrix to specified value; returns "*this".
  //  Complexity $O(\min(r,c))$
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a 3x3 matrix to [5 0 0][0 10 0][0 0 15], just say
  //  \code
  //     M.fill_diagonal(5).scale_row(1,2).scale_column(2,3);
  //  \endcode
  //  Returning "*this" also allows passing a diagonal-filled matrix as argument
  //  to a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(3,3).fill_diagonal(5));
  //  \endcode
  vnl_matrix &
  fill_diagonal(T const &);

  //: Sets the diagonal elements of this matrix to the specified list of values.
  //  Returning "*this" allows "chaining" two or more operations: see the
  //  reasoning (and the examples) in the documentation for method
  //  fill_diagonal().
  vnl_matrix &
  set_diagonal(vnl_vector<T> const &);

  //: Fills (laminates) this matrix with the given data, then returns it.
  //  We assume that the argument points to a contiguous rows*cols array, stored rowwise.
  //  No bounds checking on the array.
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to fill a square matrix column-wise, fill it rowwise then transpose:
  //  \code
  //     M.copy_in(array).inplace_transpose();
  //  \endcode
  //  Returning "*this" also allows passing a filled-in matrix as argument
  //  to a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(3,3).copy_in(array));
  //  \endcode
  vnl_matrix &
  copy_in(T const *);

  //: Fills (laminates) this matrix with the given data, then returns it.
  // A synonym for copy_in()
  vnl_matrix &
  set(T const * d)
  {
    return copy_in(d);
  }

  //: Fills the given array with this matrix.
  //  We assume that the argument points to a contiguous rows*cols array, stored rowwise.
  // No bounds checking on the array.
  void
  copy_out(T *) const;

  //  //: Set all elements to value v
  //  // Complexity $O(r.c)$
  vnl_matrix<T> &
  operator=(T const & v)
  {
    this->setConstant(v);
    return *this;
  }

  //: Copies all elements of rhs matrix into lhs matrix.
  // Complexity $O(\min(r,c))$
  vnl_matrix<T> &
  operator=(vnl_matrix<T> const & rhs)
  {
    if (this != &rhs)
    {
      this->Superclass::operator=(rhs);
    }
    return *this;
  }


  // ----------------------- Arithmetic --------------------------------
  // note that these functions should not pass scalar as a const&.
  // Look what would happen to A /= A(0,0).

  // : Add rhs to each element of lhs matrix in situ
  vnl_matrix<T> &
  operator+=(const T & value)
  {
    this->array() += value;
    return *this;
  }
  vnl_matrix<T> &
  operator-=(const T & value)
  {
    this->array() -= value;
    return *this;
  }
  //: Scalar multiplication in situ of lhs matrix  by rhs
  vnl_matrix<T> &
  operator*=(const T & value)
  {
    this->Superclass::operator*=(value);
    return *this;
  }
  //: Scalar division of lhs matrix  in situ by rhs
  vnl_matrix<T> &
  operator/=(const T & value)
  {
    this->Superclass::operator/=(value);
    return *this;
  }

  //: Add rhs to lhs  matrix in situ
  vnl_matrix<T> &
  operator+=(vnl_matrix<T> const & rhs)
  {
    this->Superclass::operator+=(rhs);
    return *this;
  }
  //: Subtract rhs from lhs matrix in situ
  vnl_matrix<T> &
  operator-=(vnl_matrix<T> const & rhs)
  {
    this->Superclass::operator-=(rhs);
    return *this;
  }
  //: Multiply lhs matrix in situ by rhs
  vnl_matrix<T> &
  operator*=(vnl_matrix<T> const & rhs)
  {
    this->Superclass::operator*=(rhs);
    return *this;
  }

  //: Negate all elements of matrix
  vnl_matrix<T>
  operator-() const
  {
    return this->Superclass::operator-();
  }

  //: Subtract rhs from each element of lhs matrix and return result in new matrix
  vnl_matrix<T>
  operator-(T const & v) const
  {
    return vnl_matrix<T>{(this->array() - v).matrix()};
  }


  //: Matrix add rhs to lhs matrix and return result in new matrix
  vnl_matrix<T>
  operator+(const T & v) const
  {
    return (this->array() + v).matrix();
  }

  //: Matrix add rhs to lhs matrix and return result in new matrix
  vnl_matrix<T>
  operator*(const T & v) const
  {
    return this->Superclass::operator*(v);
  }
  //: Matrix add rhs to lhs matrix and return result in new matrix
  vnl_matrix<T>
  operator/(const T & v) const
  {
    return this->Superclass::operator/(v);
  }

  vnl_matrix<T>
  operator+(vnl_matrix<T> const & rhs) const
  {
    return this->Superclass::operator+(rhs);
  }

  //: Matrix subtract rhs from lhs and return result in new matrix
  vnl_matrix<T>
  operator-(vnl_matrix<T> const & rhs) const
  {
    return this->Superclass::operator-(rhs);
  }
  //: Matrix multiply lhs by rhs matrix and return result in new matrix
  vnl_matrix<T>
  operator*(vnl_matrix<T> const & rhs) const
  {
    return this->Superclass::operator*(rhs);
  }

  ////--------------------------- Additions ----------------------------

  //: Make a new matrix by applying function to each element.
  vnl_matrix<T> apply(T (*f)(T)) const;

  //: Make a new matrix by applying function to each element.
  vnl_matrix<T> apply(T (*f)(T const &)) const;

  //: Make a vector by applying a function across rows.
  vnl_vector<T> apply_rowwise(T (*f)(vnl_vector<T> const &)) const;

  //: Make a vector by applying a function across columns.
  vnl_vector<T> apply_columnwise(T (*f)(vnl_vector<T> const &)) const;

  //: Return transpose
  vnl_matrix<T>
  transpose() const
  {
    vnl_matrix<T> out (*this);
    out.Superclass::transposeInPlace();
    return out;
  }

  //: Return conjugate transpose
  vnl_matrix<T>
  conjugate_transpose() const;

  //: Set values of this matrix to those of M, starting at [top,left]
  vnl_matrix<T> &
  update(vnl_matrix<T> const &, unsigned top = 0, unsigned left = 0);

  //: Set the elements of the i'th column to v[i]  (No bounds checking)
  vnl_matrix &
  set_column(unsigned i, T const * v);

  //: Set the elements of the i'th column to value, then return *this.
  vnl_matrix &
  set_column(unsigned i, T value);

  //: Set j-th column to v, then return *this.
  vnl_matrix &
  set_column(unsigned j, vnl_vector<T> const & v);

  //: Set columns to those in M, starting at starting_column, then return *this.
  vnl_matrix &
  set_columns(unsigned starting_column, vnl_matrix<T> const & M);

  //: Set the elements of the i'th row to v[i]  (No bounds checking)
  vnl_matrix &
  set_row(unsigned i, T const * v);

  //: Set the elements of the i'th row to value, then return *this.
  vnl_matrix &
  set_row(unsigned i, T value);

  //: Set the i-th row
  vnl_matrix &
  set_row(unsigned i, vnl_vector<T> const &);

  //: Extract a sub-matrix of size r x c, starting at (top,left)
  //  Thus it contains elements  [top,top+r-1][left,left+c-1]
  vnl_matrix<T>
  extract(unsigned r, unsigned c, unsigned top = 0, unsigned left = 0) const;

  //: Extract a sub-matrix starting at (top,left)
  //
  //  The output is stored in \a sub_matrix, and it should have the
  //  required size on entry.  Thus the result will contain elements
  //  [top,top+sub_matrix.rows()-1][left,left+sub_matrix.cols()-1]
  void
  extract(vnl_matrix<T> & sub_matrix, unsigned top = 0, unsigned left = 0) const;


  //: Get a vector equal to the given row
  vnl_vector<T>
  get_row(unsigned r) const;

  //: Get a vector equal to the given column
  vnl_vector<T>
  get_column(unsigned c) const;

  //: Get a matrix composed of rows from the indices specified in the supplied vector.
  vnl_matrix<T>
  get_rows(vnl_vector<unsigned int> i) const;

  //: Get a matrix composed of columns from the indices specified in the supplied vector.
  vnl_matrix<T>
  get_columns(vnl_vector<unsigned int> i) const;

  //: Return a vector with the content of the (main) diagonal
  vnl_vector<T>
  get_diagonal() const;

  //: Flatten row-major (C-style)
  vnl_vector<T>
  flatten_row_major() const;

  //: Flatten column-major (Fortran-style)
  vnl_vector<T>
  flatten_column_major() const;

  // ==== mutators ====

  //: Sets this matrix to an identity matrix, then returns "*this".
  //  Returning "*this" allows e.g. passing an identity matrix as argument to
  //  a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(5,5).set_identity());
  //  \endcode
  //  Returning "*this" also allows "chaining" two or more operations:
  //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
  //  \code
  //     M.set_identity().scale_row(0,3).scale_column(1,2);
  //  \endcode
  //  If the matrix is not square, anyhow set main diagonal to 1, the rest to 0.
  vnl_matrix &
  set_identity();

  //: Transposes this matrix efficiently, and returns it.
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to fill a square matrix column-wise, fill it rowwise then transpose:
  //  \code
  //     M.copy_in(array).inplace_transpose();
  //  \endcode
  vnl_matrix &
  inplace_transpose();

  //: Reverses the order of rows, and returns "*this".
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to flip both up-down and left-right, one could just say
  //  \code
  //     M.flipud().fliplr();
  //  \endcode
  vnl_matrix &
  flipud();

  //: Reverses the order of columns, and returns "*this".
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to flip both up-down and left-right, one could just say
  //  \code
  //     M.flipud().fliplr();
  //  \endcode
  vnl_matrix &
  fliplr();

  //: Normalizes each row so it is a unit vector, and returns "*this".
  //  Zero rows are not modified
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a matrix to a row-normalized all-elements-equal matrix, say
  //  \code
  //     M.fill(1).normalize_rows();
  //  \endcode
  //  Returning "*this" also allows passing such a matrix as argument
  //  to a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(5,5,1.0).normalize_rows());
  //  \endcode
  vnl_matrix &
  normalize_rows();

  //: Normalizes each column so it is a unit vector, and returns "*this".
  //  Zero columns are not modified
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a matrix to a column-normalized all-elements-equal matrix, say
  //  \code
  //     M.fill(1).normalize_columns();
  //  \endcode
  //  Returning "*this" also allows passing such a matrix as argument
  //  to a function f, without having to name the constructed matrix:
  //  \code
  //     f(vnl_matrix<double>(5,5,1.0).normalize_columns());
  //  \endcode
  vnl_matrix &
  normalize_columns();

  //: Scales elements in given row by a factor T, and returns "*this".
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
  //  \code
  //     M.set_identity().scale_row(0,3).scale_column(1,2);
  //  \endcode
  vnl_matrix &
  scale_row(unsigned row, T value);

  //: Scales elements in given column by a factor T, and returns "*this".
  //  Returning "*this" allows "chaining" two or more operations:
  //  e.g., to set a 3x3 matrix to [3 0 0][0 2 0][0 0 1], one could say
  //  \code
  //     M.set_identity().scale_row(0,3).scale_column(1,2);
  //  \endcode
  vnl_matrix &
  scale_column(unsigned col, T value);

  //: Swap this matrix with that matrix
  void
  swap(vnl_matrix<T> & that) noexcept;

  //: Type def for norms.
  typedef typename vnl_c_vector<T>::abs_t abs_t;

  //: Return sum of absolute values of elements
  abs_t
  array_one_norm() const
  {
    return vnl_c_vector<T>::one_norm(this->cbegin(), this->size());
  }

  //: Return square root of sum of squared absolute element values
  abs_t
  array_two_norm() const
  {
    return vnl_c_vector<T>::two_norm(this->cbegin(), this->size());
  }

  //: Return largest absolute element value
  abs_t
  array_inf_norm() const
  {
    return vnl_c_vector<T>::inf_norm(this->cbegin(), this->size());
  }

  //: Return sum of absolute values of elements
  abs_t
  absolute_value_sum() const
  {
    return array_one_norm();
  }

  //: Return largest absolute value
  abs_t
  absolute_value_max() const
  {
    return array_inf_norm();
  }

  // $ || M ||_1 := \max_j \sum_i | M_{ij} | $
  abs_t
  operator_one_norm() const;

  // $ || M ||_\inf := \max_i \sum_j | M_{ij} | $
  abs_t
  operator_inf_norm() const;

  //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
  abs_t
  frobenius_norm() const
  {
    return array_two_norm();
  }

  //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
  abs_t
  fro_norm() const
  {
    return frobenius_norm();
  }


  //: Return RMS of all elements
  abs_t
  rms() const
  {
    return vnl_c_vector<T>::rms_norm(this->cbegin(), this->size());
  }

  //: Return minimum value of elements
  T
  min_value() const
  {
    return vnl_c_vector<T>::min_value(this->cbegin(), this->size());
  }

  //: Return maximum value of elements
  T
  max_value() const
  {
    return vnl_c_vector<T>::max_value(this->cbegin(), this->size());
  }

  //: Return location of minimum value of elements
  unsigned
  arg_min() const
  {
    return vnl_c_vector<T>::arg_min(this->cbegin(), this->size());
  }
  unsigned
  arg_max() const
  {
    return vnl_c_vector<T>::arg_max(this->cbegin(), this->size());
  }

  //: Mean of values in vector
  // baseclass T mean() const { return this->sum() / static_cast<T>(this->size()); }

  //: Sum of values in a vector
  // baseclass T sum() const { return std::accumulate(this->cbegin(), this->cend(), 0.0); }


  // predicates

  //: Return true iff the size is zero.
  bool
  empty() const
  {
    return this->size() == 0;
  }

  //:  Return true if all elements equal to identity.
  bool
  is_identity() const;

  //:  Return true if all elements equal to identity, within given tolerance
  bool
  is_identity(double tol) const;

  //: Return true if all elements equal to zero.
  bool
  is_zero() const;

  //: Return true if all elements equal to zero, within given tolerance
  bool
  is_zero(double tol) const;

  //:  Return true if all elements of both matrices are equal, within given tolerance
  bool
  is_equal(vnl_matrix<T> const & rhs, double tol) const;

  //: Return true if finite
  bool
  is_finite() const;

  //: Return true if matrix contains NaNs
  bool
  has_nans() const;

  //: abort if size is not as expected
  // This function does or tests nothing if NDEBUG is defined
  void
  assert_size(unsigned VXL_USED_IN_DEBUG(r), unsigned VXL_USED_IN_DEBUG(c)) const
  {
#ifndef NDEBUG
    assert_size_internal(r, c);
#endif
  }
  //: abort if matrix contains any INFs or NANs.
  // This function does or tests nothing if NDEBUG is defined
  void
  assert_finite() const
  {
#ifndef NDEBUG
    assert_finite_internal();
#endif
  }

  ////----------------------- Input/Output ----------------------------

  //: Read a vnl_matrix from an ascii std::istream, automatically determining file size if the input matrix has zero
  //: size.
  static vnl_matrix<T>
  read(std::istream & s);

  // : Read a vnl_matrix from an ascii std::istream, automatically determining file size if the input matrix has zero
  // size.
  bool
  read_ascii(std::istream & s);

  //--------------------------------------------------------------------------------

  //: Access the contiguous block storing the elements in the matrix row-wise. O(1).
  // 1d array, row-major order.
  T const *
  data_block() const
  {
    return this->data();
  }

  //: Access the contiguous block storing the elements in the matrix row-wise. O(1).
  // 1d array, row-major order.
  T *
  data_block()
  {
    return this->data();
  }


  typedef T element_type;

  //: Iterators
  typedef T * iterator;
  typedef T const * const_iterator;

  //  //: Iterator pointing to start of data
  iterator
  begin()
  {
    return this->data() ? this->data() : nullptr;
  }
  const_iterator
  begin() const
  {
    return this->data() ? this->data() : nullptr;
  }
  const_iterator
  cbegin() const
  {
    return this->data() ? this->data() : nullptr;
  }

  //  //: Iterator pointing to element beyond end of data
  iterator
  end()
  {
    return this->data() ? this->data() + this->size() : nullptr;
  }
  const_iterator
  end() const
  {
    return this->data() ? this->data() + this->size() : nullptr;
  }
  const_iterator
  cend() const
  {
    return this->data() ? this->data() + this->size() : nullptr;
  }

  //
  //  //: Const iterators
  //  typedef T const *const_iterator;
  //  //: Iterator pointing to start of data
  //  const_iterator begin() const { return data?data[0]:nullptr; }
  //  //: Iterator pointing to element beyond end of data
  //  const_iterator end() const { return data?data[0]+rows()*cols():nullptr; }

  //: Return a reference to this.
  // Useful in code which would prefer not to know if its argument
  // is a matrix, matrix_ref or a matrix_fixed.  Note that it doesn't
  // return a matrix_ref, so it's only useful in templates or macros.
  vnl_matrix<T> const &
  as_ref() const
  {
    return *this;
  }

  //: Return a reference to this.
  vnl_matrix<T> &
  as_ref()
  {
    return *this;
  }

  vnl_matrix<T> const &
  as_matrix() const
  {
    return *this;
  }

  //: Return a reference to this.
  vnl_matrix<T> &
  as_matrix()
  {
    return *this;
  }

  //--------------------------------------------------------------------------------

  //: Return true if *this == rhs
  bool
  operator_eq(vnl_matrix<T> const & rhs) const;

  //: Equality operator
  bool
  operator==(vnl_matrix<T> const & that) const
  {
    return this->operator_eq(that);
  }

  //: Inequality operator
  bool
  operator!=(vnl_matrix<T> const & that) const
  {
    return !this->operator_eq(that);
  }

  //: Print matrix to os in some hopefully sensible format
  void
  print(std::ostream & os) const;

  //: Make the matrix as if it had been default-constructed.
  void
  clear();

  //: Resize to r rows by c columns. Old data lost.
  // Returns true if size changed.
  bool
  set_size(unsigned r, unsigned c);

  //--------------------------------------------------------------------------------

protected:
  vnl_matrix(unsigned ext_num_rows, unsigned ext_num_cols, T * continuous_external_memory_block, bool manage_own_memory)
    : Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,Eigen::RowMajorBit>(ext_num_rows, ext_num_cols)
    , m_LetArrayManageMemory{ manage_own_memory }
  {
    std::copy_n(continuous_external_memory_block, this->size(), this->data());
  }

  void
  assert_size_internal(unsigned r, unsigned c) const;
  void
  assert_finite_internal() const;

  //: Delete data
  void
  destroy();

private:
  bool m_LetArrayManageMemory{ true };
  mutable T ** _data_ptr_ptr;
};

// Definitions of inline functions.

//: Returns the value of the element at specified row and column. O(1).
// Checks for valid range of indices.

template <class T>
inline T
vnl_matrix<T>::get(unsigned r, unsigned c) const
{
#if VNL_CONFIG_CHECK_BOUNDS
  if (r >= this->rows())                  // If invalid size specified
    vnl_error_matrix_row_index("get", r); // Raise exception
  if (c >= this->cols())                  // If invalid size specified
    vnl_error_matrix_col_index("get", c); // Raise exception
#endif
  return this->Superclass::operator()(r,c);
}

//: Puts value into element at specified row and column. O(1).
// Checks for valid range of indices.

template <class T>
inline void
vnl_matrix<T>::put(unsigned r, unsigned c, T const & v)
{
#if VNL_CONFIG_CHECK_BOUNDS
  if (r >= this->rows())                  // If invalid size specified
    vnl_error_matrix_row_index("put", r); // Raise exception
  if (c >= this->cols())                  // If invalid size specified
    vnl_error_matrix_col_index("put", c); // Raise exception
#endif
  this->Superclass::operator()(r, c) = v; // Assign data value
}


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



//: Returns new matrix which is the negation of THIS matrix.
// O(m*n).



////--------------------------- Additions------------------------------------





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

// template<class T>
// inline vnl_matrix<T> operator+(vnl_matrix<T> const& m, T const& value)
//{
//   return m+value;
// }

//: Swap two matrices
// \relatesalso vnl_matrix
template <class T>
inline void
swap(vnl_matrix<T> & A, vnl_matrix<T> & B) noexcept
{
  A.swap(B);
}
#endif
#endif // vnl_matrix_h_
