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
// Created: MBN 04/21/89  Initial design and implementation
// Updated: MBN 06/22/89  Removed non-destructive methods
// Updated: LGO 08/09/89  Inherit from Generic
// Updated: MBN 08/20/89  Changed template usage to reflect new syntax
// Updated: MBN 09/11/89  Added conditional exception handling and base class
// Updated: LGO 10/05/89  Don't re-allocate data in operator= when same size
// Updated: LGO 10/19/89  Add extra parameter to varargs constructor
// Updated: MBN 10/19/89  Added optional argument to set_compare method
// Updated: LGO 12/08/89  Allocate column data in one chunk
// Updated: LGO 12/08/89  Clean-up get and put, add const everywhere.
// Updated: LGO 12/19/89  Remove the map and reduce methods
// Updated: MBN 02/22/90  Changed size arguments from int to unsigned int
// Updated: MJF 06/30/90  Added base class name to constructor initializer
// Updated: VDN 02/21/92  New lite version
// Updated: VDN 05/05/92  Use envelope to avoid unecessary copying
// Updated: VDN 09/30/92  Matrix inversion with singular value decomposition
// Updated: AWF 08/21/96  set_identity, normalize_rows, scale_row.
// Updated: AWF 09/30/96  set_row/set_column methods.  Const-correct data_block().
// Updated: AWF 14/02/97  get_n_rows, get_n_columns.
// Updated: PVR 20/03/97  get_row, get_column.
//
// The parameterized vnl_matrix<T> class implements two dimensional arithmetic
// matrices of a user specified type. The only constraint placed on the type is
// that it must overload the following operators: +, -,  *,  and /. Thus, it 
// will be possible to have a vnl_matrix over vnl_complex<T>. The vnl_matrix<T> 
// class is static in size, that is once a vnl_matrix<T> of a particular size
// has been created, there is no dynamic growth or resize method available.
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
// the third, except that it accpets a variable number of initialization values
// for the Matrix.  If there are  fewer values than elements,  the rest are set
// to zero. Finally, the last constructor takes a single argument consisting of
// a reference to a Matrix and duplicates its size and element values.
//
// Methods   are  provided   for destructive   scalar   and Matrix    addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for fomatted input and output of matrix elements.
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

#include "vnl_matrix.h"

#include <vcl/vcl_cstdio.h>   // sprintf()
#include <vcl/vcl_cctype.h>   // isspace()
#include <vcl/vcl_cstring.h>  // strcpy()
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_c_vector.h>
#include <vnl/vnl_numeric_traits.h>

//--------------------------------------------------------------------------------

// Matrix -- Creates an empty matrix which has no array for the elements. O(1).
// 
template<class T> 
vnl_matrix<T>::vnl_matrix () :
  num_rows(0),
  num_cols(0),
  data(0)
{
}

// Matrix -- Creates a matrix with given number of rows and columns.
// Elements are not initialized. O(m*n).

template<class T> 
vnl_matrix<T>::vnl_matrix (unsigned rowz, unsigned colz):
  num_rows(rowz),
  num_cols(colz)
{
  if (colz && rowz) {
    this->data = new T*[rowz];                 // Allocate the row memory
    T* clmns = new T[colz*rowz];               // Allocate the array of elmts
    for (unsigned i = 0; i < rowz; i ++)  // For each row in the Matrix
      this->data[i] = &clmns[i*colz];          // Fill in address of row
  } else {
#define AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK this->data = 0;
#undef AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK
#define AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK (this->data = new T*[1])[0] = 0
    AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK;
  }
}

// Matrix -- Creates a matrix with given number of rows and columns,
// and initialize all elements to value. O(m*n).

template<class T> 
vnl_matrix<T>::vnl_matrix (unsigned rowz, unsigned colz, T const& value) :
  num_rows(rowz), num_cols(colz)
{
  if (colz && rowz) {
    this->data = new T*[rowz];                  // Allocate the row memory
    T* clmns = new T[colz*rowz];                // Allocate the array of elmts
    for (unsigned i = 0; i < rowz; i ++) { // For each row in the Matrix
      this->data[i] = &clmns[i*colz];           // Fill in address of row
      for (unsigned j = 0; j < colz; j++)  // For each element in column
	this->data[i][j] = value;               // Assign initial value
    }
  } else {
    AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK;
  }
}



// Matrix -- Creates a matrix with given dimension (rows, cols) and initialize
// first n elements, row-wise, to values. O(m*n).

template<class T> 
vnl_matrix<T>::vnl_matrix (unsigned rowz, unsigned colz, int n, T const values[])
: num_rows(rowz), num_cols( colz) {
  this->data = new T*[rowz];                 // Allocate the row memory
  T* clmns = new T[colz*rowz];               // Allocate the array of elmts
  unsigned i = 0, j = 0, k = 0;
  for (i = 0; i < rowz; i ++)                // For each row in the Matrix
    this->data[i] = &clmns[i*colz];          // Fill in address of row
  for (i = 0; i < rowz && n; i++)            // Initialize elements of matrix
    for (j = 0; j < colz && n; j++, n--)     // row-wise with values.
      this->data[i][j] = values[k++];
}

// Matrix -- Creates a matrix from a block array of data, stored row-wise.
// O(m*n).

template<class T>
vnl_matrix<T>::vnl_matrix (T const* datablck, unsigned rowz, unsigned colz)
: num_rows(rowz), num_cols(colz) {
  if (colz && rowz) {
    this->data = new T*[num_rows];             // Allocate the row memory
    unsigned n = num_rows * num_cols;        // Number of elements 
    T* clmns = new T[n];                  // Allocate the array of elmts
    for (unsigned i = 0; i < num_rows; i ++) // For each row in the Matrix
      this->data[i] = &clmns[i*num_cols];       // Fill in address of row
    for (unsigned d = 0; d < n; d++)         // Copy all the data elements
      clmns[d] = datablck[d];
  } else 
    AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK;
}


// Matrix -- Creates a new matrix and copies all the elements of from matrix.
// O(m*n).

template<class T> 
vnl_matrix<T>::vnl_matrix (vnl_matrix<T> const& from)
{
  this->num_rows = from.num_rows;
  this->num_cols = from.num_cols;
  if (this->num_rows && this->num_cols) {
    this->data = new T*[this->num_rows];       // Allocate the row memory
    T* clmns = new T[num_cols*num_rows];  // Allocate the array of elmts
    for (unsigned i = 0; i < num_rows; i ++) {       // For each row in the Matrix
      this->data[i] = &clmns[i*num_cols];       // Fill in address of row
      for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
	this->data[i][j] = from.data[i][j];               // Copy value
    }
  } else {
    AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK;
  }
}


// ~Matrix -- Deletes matrix and frees up the arrays and the elements in them.
// O(m*n).

template<class T> 
vnl_matrix<T>::~vnl_matrix() {
  if (this->data) {
    delete [] this->data[0];                    // Free up the array of elmts
    delete [] this->data;                       // Free up the row memory
  }
}

// fill -- Sets all elements of matrix to specified value. O(m*n).

template<class T> 
void vnl_matrix<T>::fill (T const& value) {
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row in the Matrix
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] = value;                 // Assign fill value
}

// fill_diagonal -- Sets all diagonal elements of matrix to specified value. O(n).

template<class T> 
void vnl_matrix<T>::fill_diagonal (T const& value) {
  for (unsigned i = 0; i < this->num_rows && i < this->num_cols; i++)
    this->data[i][i] = value;                   // Assign fill value
}

// operator= -- Assigns value to all elements of a matrix. O(m*n).

//template<class T> 
//vnl_matrix<T>& vnl_matrix<T>::operator= (T const& value) {
//  for (unsigned i = 0; i < this->num_rows; i++)    // For each row in Matrix
//    for (unsigned j = 0; j < this->num_cols; j++)  // For each column in Matrix
//      this->data[i][j] = value;                 // Assign value
//  return *this;                                 // Return Matrix reference
//}


// operator= -- Copies all elements of rhs matrix into lhs matrix. O(m*n).
// If needed, the arrays in lhs matrix are freed up, and new arrays are
// allocated to match the dimensions of the rhs matrix.

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator= (vnl_matrix<T> const& rhs) {
  if (this != &rhs) {                           // make sure *this != m
    this->resize(rhs.num_rows, rhs.num_cols);
    for (unsigned i = 0; i < this->num_rows; i++)  // For each row in the Matrix
      for (unsigned j = 0; j < this->num_cols; j++) // For each element in column
	this->data[i][j] = rhs.data[i][j];      // Copy value
  }
  return *this;                                 // Return Matrix reference
}

template<class T> 
void vnl_matrix<T>::print(ostream& os) const {
  for (unsigned i = 0; i < this->rows(); i++) {    // For each row in matrix
    for (unsigned j = 0; j < this->columns(); j++) // For each column in matrix
      os << this->data[i][j] << " ";            // Output data element
    os << "\n";                                 // Output newline
  }
}
  
// operator<< -- Prints the 2D array of elements of a matrix out to a stream.
// O(m*n).

template<class T> 
ostream& operator<< (ostream& os, vnl_matrix<T> const& m) {
  for (unsigned i = 0; i < m.rows(); i++) {        // For each row in m
    for (unsigned j = 0; j < m.columns(); j++) {   // For each column
#if 0 // This does not work when T is not a built-in type, e.g. T is complex:
      if (vnl_matrix<T>::print_format_set()) {
	char buf[1024];
	sprintf(buf, vnl_matrix<T>::get_print_format(), m.get(i,j));
	os << buf << " ";
      }
      else
#endif
	os << m.get(i,j) << " ";          // Output data element
    }
    os << "\n";                         // Output newline
  }
  return os;                            // Return ostream reference
}

// -- Read an vnl_matrix from an ascii istream, automatically
// determining file size if the input matrix has zero size.
template<class T>
istream& operator>>(istream& s, vnl_matrix<T>& M) {
  M.read_ascii(s); 
  return s;
}


  
// operator+= -- Mutates lhs matrix to add all elements with value. O(m*n).

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator+= (T const& value) {
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] += value;                // Add scalar
  return *this;
}

// operator*= -- Mutates lhs matrix to multiply all elements by value. O(m*n).

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator*= (T const& value) {
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] *= value;                // Multiply by scalar
  return *this;
}

// operator/= --  Mutates lhs matrix to divide all elements by value. O(m*n).

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator/= (T const& value) {
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] /= value;                // division by scalar
  return *this;
}

template<class T>
vnl_matrix<T> operator- (T const& value, vnl_matrix<T> const& m) {
  vnl_matrix<T> result(m.rows(),m.columns());
  for (unsigned i = 0; i < m.rows(); i++)  // For each row
    for (unsigned j = 0; j < m.columns(); j++) // For each elmt. in column
      result.put(i,j, value - m.get(i,j) );     // subtract from value elmt.
  return result;
}



// operator+= -- Adds lhs matrix with rhs matrix, and stores in place in
// lhs matrix. O(m*n). The dimensions of the two matrices must be identical.

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator+= (vnl_matrix<T> const& rhs) {
  if (this->num_rows != rhs.num_rows ||
      this->num_cols != rhs.num_cols)           // Size match?
    vnl_error_matrix_dimension ("operator+=", 
			   this->num_rows, this->num_cols, 
			   rhs.num_rows, rhs.num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] += rhs.data[i][j];       // Add elements
  return *this;
}


// operator-= -- Substract lhs matrix with rhs matrix and store in place
// in lhs matrix. O(m*n).
// The dimensions of the two matrices must be identical.

template<class T> 
vnl_matrix<T>& vnl_matrix<T>::operator-= (vnl_matrix<T> const& rhs) {
  if (this->num_rows != rhs.num_rows ||
      this->num_cols != rhs.num_cols) // Size?
    vnl_error_matrix_dimension ("operator-=", 
			   this->num_rows, this->num_cols, 
			   rhs.num_rows, rhs.num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)
    for (unsigned j = 0; j < this->num_cols; j++)
      this->data[i][j] -= rhs.data[i][j];
  return *this;
}

// operator* -- Returns new matrix which is the product of m1 with m2, m1 * m2.
// O(n^3). Number of columns of first matrix must match number of rows
// of second matrix.

template<class T> 
vnl_matrix<T> vnl_matrix<T>::operator* (vnl_matrix<T> const& rhs) const {
  if (this->num_cols != rhs.num_rows)           // dimensions do not match?
    vnl_error_matrix_dimension("operator*", 
			  this->num_rows, this->num_cols, 
			  rhs.num_rows, rhs.num_cols);
  vnl_matrix<T> result(this->num_rows, rhs.num_cols); // Temp to store product
  for (unsigned i = 0; i < this->num_rows; i++) {  // For each row
    for (unsigned j = 0; j < rhs.num_cols; j++) {  // For each element in column
      T sum = 0;
      for (unsigned k = 0; k < this->num_cols; k++) // Loop over column values
	sum += (this->data[i][k] * rhs.data[k][j]);     // Multiply
      result(i,j) = sum;
    }
  }
  return result;
}

// operator- -- Returns new matrix which is the negation of THIS matrix. O(m*n).

template<class T> 
vnl_matrix<T> vnl_matrix<T>::operator- () const {
  vnl_matrix<T> result(this->num_rows, this->num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)
    for (unsigned j = 0; j < this->num_cols; j++)
      result.data[i][j] = - this->data[i][j];
  return result;
}

// operator+ -- Returns new matrix with elements of lhs matrix added
// with value. O(m*n).

template<class T> 
vnl_matrix<T> vnl_matrix<T>::operator+ (T const& value) const {
  vnl_matrix<T> result(this->num_rows, this->num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      result.data[i][j] = (this->data[i][j] + value);   // Add scalar
  return result;
}


// operator+ -- Returns new matrix with elements of lhs matrix multiplied
// with value. O(m*n).

template<class T> 
vnl_matrix<T> vnl_matrix<T>::operator* (T const& value) const {
  vnl_matrix<T> result(this->num_rows, this->num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      result.data[i][j] = (this->data[i][j] * value);   // Multiply
  return result;
}


// operator+ -- Returns new matrix with elements of lhs matrix divided by
// value. O(m*n).

template<class T> 
vnl_matrix<T> vnl_matrix<T>::operator/ (T const& value) const {
  vnl_matrix<T> result(this->num_rows, this->num_cols);
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      result.data[i][j] = (this->data[i][j] / value);   // Divide
  return result;
}

// -- Return the matrix made by applying "f" to each element.
template <class T>
vnl_matrix<T> vnl_matrix<T>::apply(T (*f)(T const&)) const {
  vnl_matrix<T> ret(num_rows, num_cols);
  vnl_c_vector<T>::apply(this->data[0], num_rows * num_cols, f, ret.data_block());
  return ret;
}

// --
template <class T>
vnl_matrix<T> vnl_matrix<T>::apply(T (*f)(T)) const {
  vnl_matrix<T> ret(num_rows, num_cols);
  vnl_c_vector<T>::apply(this->data[0], num_rows * num_cols, f, ret.data_block());
  return ret;
}

////--------------------------- Additions------------------------------------

// transpose -- Returns new matrix with rows and columns transposed. O(m*n).

template<class T> 
vnl_matrix<T> vnl_matrix<T>::transpose() const {
  vnl_matrix<T> result(this->num_cols, this->num_rows);
  for (unsigned i = 0; i < this->num_cols; i++)
    for (unsigned j = 0; j < this->num_rows; j++)
      result.data[i][j] = this->data[j][i];
  return result;
}

// adjoint/hermitian transpose

template<class T> 
vnl_matrix<T> vnl_matrix<T>::conjugate_transpose() const  {
  vnl_matrix<T> result(transpose());
  vnl_c_vector<T>::conjugate(result.begin(),  // src
			     result.begin(),  // dst
			     result.size());  // size of block
  return result;
}

// update -- Replaces the submatrix of THIS matrix, starting at top left corner,
// by the elements of matrix m. O(m*n). This is the reverse of extract().


template<class T> 
vnl_matrix<T>& vnl_matrix<T>::update (vnl_matrix<T> const& m, 
					    unsigned top, unsigned left) {
  unsigned bottom = top + m.num_rows;
  unsigned right = left + m.num_cols;
  if (this->num_rows < bottom || this->num_cols < right)
    vnl_error_matrix_dimension ("update", 
			   bottom, right, m.num_rows, m.num_cols);
  for (unsigned i = top; i < bottom; i++)
    for (unsigned j = left; j < right; j++)
      this->data[i][j] = m.data[i-top][j-left];
  return *this;
}


// extract -- Returns a copy of submatrix of THIS matrix, 
// specified by the top-left corner and size in rows, cols. O(m*n).
// Use update() to copy new values of this submatrix back into THIS matrix.

template<class T> 
vnl_matrix<T> vnl_matrix<T>::extract (unsigned rowz, unsigned colz, 
					    unsigned top, unsigned left) const{
  unsigned bottom = top + rowz;
  unsigned right = left + colz;
  if ((this->num_rows < bottom) || (this->num_cols < right))
    vnl_error_matrix_dimension ("extract", 
			   this->num_rows, this->num_cols, bottom, right);
  vnl_matrix<T> result(rowz, colz);
  for (unsigned i = 0; i < rowz; i++)      // actual copy of all elements
    for (unsigned j = 0; j < colz ; j++)   // in submatrix
      result.data[i][j] = data[top+i][left+j];
  return result;
}

// dot_product -- Returns the dot product of the two matrices, which is 
// the sum of all pairwise product of the elements m1[i,j]*m2[i,j]. O(m*n).

template<class T> 
T dot_product (vnl_matrix<T> const& m1, vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("dot_product", 
				m1.rows(), m1.columns(), 
				m2.rows(), m2.columns());
  return vnl_c_vector<T>::dot_product(m1.begin(), m2.begin(), m1.rows()*m1.cols());
}

// inner_product -- Hermitian inner product. O(mn).

template<class T> 
T inner_product (vnl_matrix<T> const& m1, vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("inner_product", 
				m1.rows(), m1.columns(), 
				m2.rows(), m2.columns());
  return vnl_c_vector<T>::inner_product(m1.begin(), m2.begin(), m1.rows()*m1.cols());
}

// cos_angle. O(mn).

template<class T> 
T cos_angle (vnl_matrix<T> const& a, vnl_matrix<T> const& b) {
  typedef vnl_numeric_traits<T>::real_t real_t;
  real_t ab = inner_product(a,b);
  real_t a_b = sqrt( vnl_math_abs(inner_product(a,a) * inner_product(b,b)) );
  return T( ab / a_b );
}

#if 0
// cross_2d -- Returns the 2X1 cross-product of 2 2d-vectors. O(n).

template<class T> 
T cross_2d (vnl_matrix<T> const& m1, vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns())
    vnl_error_matrix_dimension ("cross_2d", 
			m1.rows(), m1.columns(), m2.rows(), m2.columns());
  return m1.x() * m2.y() - m1.y() * m2.x();
}

// cross_3d -- Returns the 3X1 cross-product of 2 3d-vectors. 

template<class T> 
vnl_matrix<T> cross_3d (vnl_matrix<T> const& m1,
			    vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns())
    vnl_error_matrix_dimension ("cross_3d", 
			m1.rows(), m1.columns(), m2.rows(), m2.columns());
  vnl_matrix<T> result(m1.rows(), m1.columns()); 
  result.x() = m1.y() * m2.z() - m1.z() * m2.y(); // work for both col/row
  result.y() = m1.z() * m2.x() - m1.x() * m2.z(); // representation
  result.z() = m1.x() * m2.y() - m1.y() * m2.x();
  return result;                                 // copy of envelope
}
#endif

// element_product -- Returns new matrix whose elements are the products 
// m1[ij]*m2[ij]. O(m*n).
 
template<class T> 
vnl_matrix<T> element_product (vnl_matrix<T> const& m1, 
				   vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("element_product", 
			m1.rows(), m1.columns(), m2.rows(), m2.columns());
  vnl_matrix<T> result(m1.rows(), m1.columns());
  for (unsigned i = 0; i < m1.rows(); i++)
    for (unsigned j = 0; j < m1.columns(); j++)
      result.put(i,j, m1.get(i,j) * m2.get(i,j) );
  return result;
}
 
// element_quotient -- Returns new matrix whose elements are the quotients 
// m1[ij]/m2[ij]. O(m*n).
 
template<class T> 
vnl_matrix<T> element_quotient (vnl_matrix<T> const& m1, 
				    vnl_matrix<T> const& m2) {
  if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) // Size?
    vnl_error_matrix_dimension ("element_quotient", 
			m1.rows(), m1.columns(), m2.rows(), m2.columns());
  vnl_matrix<T> result(m1.rows(), m1.columns());
  for (unsigned i = 0; i < m1.rows(); i++)
    for (unsigned j = 0; j < m1.columns(); j++)
      result.put(i,j, m1.get(i,j) / m2.get(i,j) );
  return result;
}

// -- Fill this matrix with the given data.  We assume that p points to
// a contiguous rows*cols array, stored rowwise.
template<class T>
void vnl_matrix<T>::copy_in(T const *p)
{
  T* dp = this->data[0];
  int n = this->num_rows * this->num_cols;
  while (n--)
    *dp++ = *p++;
}

// -- Fill the given array with this matrix.  We assume that p points to
// a contiguous rows*cols array, stored rowwise.
template<class T>
void vnl_matrix<T>::copy_out(T *p) const
{
  T* dp = this->data[0];
  int n = this->num_rows * this->num_cols;
  while (n--)
    *p++ = *dp++;
}

// -- Fill this matrix with a row*row identity matrix.
template<class T>
void vnl_matrix<T>::set_identity()
{
  if (this->num_rows != this->num_cols) // Size?
    vnl_error_matrix_nonsquare ("set_identity");
  for (unsigned i = 0; i < this->num_rows; i++)    // For each row in the Matrix
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in column
      this->data[i][j] = (i == j) ? 1 : 0; // Assign fill value
}

// -- Make each row of the matrix have unit norm.
// All-zero rows are ignored.
template<class T>
void vnl_matrix<T>::normalize_rows()
{
  for (unsigned i = 0; i < this->num_rows; i++) {  // For each row in the Matrix
    double norm = 0; // Double should do all, but this should be specialized for complex.
    for (unsigned j = 0; j < this->num_cols; j++)  // For each element in row
      norm += vnl_math_squared_magnitude(this->data[i][j]);

    if (norm != 0) {
      vnl_numeric_traits<vnl_numeric_traits<T>::abs_t>::real_t scale = 1.0/sqrt(norm);
      for (unsigned j = 0; j < this->num_cols; j++) {
        // FIXME need correct rounding here
#ifdef VCL_WIN32
        this->data[i][j] *= scale;
#else
        this->data[i][j] = T(this->data[i][j] * scale);
#endif
      }
    }
  }
}

// -- Make each column of the matrix have unit norm.
// All-zero columns are ignored.
template<class T>
void vnl_matrix<T>::normalize_columns()
{
  for (unsigned j = 0; j < this->num_cols; j++) {  // For each column in the Matrix
    double norm = 0; // Double should do all, but this should be specialized for complex.
    for (unsigned i = 0; i < this->num_rows; i++)
      norm += vnl_math_squared_magnitude(this->data[i][j]);

    if (norm != 0) {
      vnl_numeric_traits<vnl_numeric_traits<T>::abs_t>::real_t scale = 1.0/sqrt(norm);
      for (unsigned i = 0; i < this->num_rows; i++) {
        // FIXME need correct rounding here
#ifdef VCL_WIN32
        this->data[i][j] *= scale;
#else
        this->data[i][j] = T(this->data[i][j] * scale);
#endif
      }
    }
  }
}

// -- Multiply row[row_index] by value
template<class T>
void vnl_matrix<T>::scale_row(unsigned row_index, T value)
{
  if (row_index >= this->num_rows)
    vnl_error_matrix_row_index("scale_row", row_index);
  for (unsigned j = 0; j < this->num_cols; j++)    // For each element in row
    this->data[row_index][j] *= value;
}

// -- Multiply column[column_index] by value
template<class T>
void vnl_matrix<T>::scale_column(unsigned column_index, T value)
{
  if (column_index >= this->num_cols)
    vnl_error_matrix_col_index("scale_column", column_index);
  for (unsigned j = 0; j < this->num_rows; j++)    // For each element in column
    this->data[j][column_index] *= value;
}

// -- Returns a copy of n rows, starting from "row"
template<class T> 
vnl_matrix<T> vnl_matrix<T>::get_n_rows (unsigned row, unsigned n) const {
  if (row + n > this->num_rows)
    vnl_error_matrix_row_index ("get_n_rows", row);

  // Extract data rowwise.
  return vnl_matrix<T>(data[row], n, this->num_cols);
}

// -- Returns a copy of n columns, starting from "column".
template<class T> 
vnl_matrix<T> vnl_matrix<T>::get_n_columns (unsigned column, unsigned n) const {
  if (column + n > this->num_cols)
    vnl_error_matrix_col_index ("get_n_columns", column);
  
  vnl_matrix<T> result(this->num_rows, n);
  for(unsigned c = 0; c < n; ++c)
    for (unsigned r = 0; r < this->num_rows; ++r)
      result(r, c) = data[r][column + c];
  return result;
}

// -- Create a vector out of row[row_index].
template<class T>
vnl_vector<T> vnl_matrix<T>::get_row(unsigned row_index) const
{
#if ERROR_CHECKING
  if (row_index >= this->num_rows)
    vnl_error_matrix_row_index ("get_row", row_index);
#endif

  vnl_vector<T> v(this->num_cols);
  for (unsigned j = 0; j < this->num_cols; j++)    // For each element in row
    v[j] = this->data[row_index][j];
  return v;
}

// -- Create a vector out of column[column_index].
template<class T>
vnl_vector<T> vnl_matrix<T>::get_column(unsigned column_index) const
{
#if ERROR_CHECKING
  if (column_index >= this->num_cols)
    vnl_error_matrix_col_index ("get_column", column_index);
#endif

  vnl_vector<T> v(this->num_rows);
  for (unsigned j = 0; j < this->num_rows; j++)    // For each element in row
    v[j] = this->data[j][column_index];
  return v;
}

//--------------------------------------------------------------------------------

// -- Set row[row_index] to data at given address. No bounds check.
template<class T>
void vnl_matrix<T>::set_row(unsigned row_index, T const *v)
{
  for (unsigned j = 0; j < this->num_cols; j++)    // For each element in row
    this->data[row_index][j] = v[j];
}

// -- Set row[row_index] to given vector. No bounds check.
template<class T>
void vnl_matrix<T>::set_row(unsigned row_index, vnl_vector<T> const &v)
{
  set_row(row_index,v.data_block());
}

// -- Set row[row_index] to given value.
template<class T>
void vnl_matrix<T>::set_row(unsigned row_index, T v)
{
  for (unsigned j = 0; j < this->num_cols; j++)    // For each element in row
    this->data[row_index][j] = v;
}

//--------------------------------------------------------------------------------

// -- Set column[column_index] to data at given address.
template<class T>
void vnl_matrix<T>::set_column(unsigned column_index, T const *v)
{
  for (unsigned i = 0; i < this->num_rows; i++)    // For each element in row
    this->data[i][column_index] = v[i];
}

// -- Set column[column_index] to given vector. No bounds check.
template<class T>
void vnl_matrix<T>::set_column(unsigned column_index, vnl_vector<T> const &v)
{
  set_column(column_index,v.data_block());
}

// -- Set column[column_index] to given value.
template<class T>
void vnl_matrix<T>::set_column(unsigned column_index, T v)
{
  for (unsigned j = 0; j < this->num_rows; j++)    // For each element in row
    this->data[j][column_index] = v;
}


// -- Set columns starting at starting_column to given matrix
template<class T>
void vnl_matrix<T>::set_columns(unsigned starting_column, vnl_matrix<T> const& m)
{
  if (this->num_rows != m.num_rows ||
      this->num_cols < m.num_cols + starting_column)           // Size match?
    vnl_error_matrix_dimension ("set_columns", 
				this->num_rows, this->num_cols, 
				m.num_rows, m.num_cols);
  
  for (unsigned j = 0; j < m.num_cols; ++j)
    for (unsigned i = 0; i < this->num_rows; i++)    // For each element in row
      this->data[i][starting_column + j] = m.data[i][j];
}

//--------------------------------------------------------------------------------

// resize // Resizes the data arrays of THIS matrix to (rows x cols). O(m*n).
// Elements are not initialized. Returns true if size is changed.

template<class T> 
bool vnl_matrix<T>::resize (unsigned rowz, unsigned colz) {
  if (this->num_rows != rowz || this->num_cols != colz) {
    if (this->data) {
      delete [] this->data[0];                 // Free up the array of elmts
      delete [] this->data;                    // Free up the row memory
    }
    this->num_rows = rowz;                     // Copy rows
    this->num_cols = colz;                     // Copy columns 
    if (rowz && colz) {
      this->data = new T*[num_rows];           // Allocate the rows
      T* clmns = new T[num_cols*num_rows];     // Allocate the columns
      for (unsigned i = 0; i < this->num_rows; i++)   // For each row
	this->data[i] = &clmns[i*num_cols];    // Fill in address of row
    } else {
      AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK;
    }
    return true;
  } else
    return false;
}
#undef AWF_WANTS_ZERO_BY_N_MATRICES_TO_WORK

// operator== -- Two matrices are equal if and only if they have the same
// dimensions and the same values. O(m*n). 
// Elements are compared with operator== as default.
// Change this default with set_compare() at run time or by specializing
// vnl_matrix_compare at compile time.

template<class T> 
bool vnl_matrix<T>::operator_eq(vnl_matrix<T> const& rhs) const {
  if (this == &rhs)                                      // same object => equal.
    return true;

  if (this->num_rows != rhs.num_rows || this->num_cols != rhs.num_cols)
    return false;                                        // different sizes => not equal.
  
  for (unsigned i = 0; i < this->num_rows; i++)     // For each row
    for (unsigned j = 0; j < this->num_cols; j++)   // For each columne
      if (this->data[i][j] != rhs.data[i][j])            // different element ?
	return false;                                    // Then not equal.

  return true;                                           // Else same; return true
}

// -- Return true if maximum absolute deviation of M from identity is <= tol.
template <class T>
bool vnl_matrix<T>::is_identity(double tol) const
{
  T one = vnl_numeric_traits<T>::one;
  for(unsigned i = 0; i < this->rows(); ++i)
    for(unsigned j = 0; j < this->columns(); ++j) {
      T xm = (*this)(i,j);
      double absdev = (i == j) ? vnl_math_abs(xm - one) : vnl_math_abs(xm);
      if (absdev > tol)
	return false;
    }
  return true;
}

// -- Return true if max(abs((*this))) <= tol.  Tol defaults to zero.
template <class T>
bool vnl_matrix<T>::is_zero(double tol) const
{
  for(unsigned i = 0; i < this->rows(); ++i)
    for(unsigned j = 0; j < this->columns(); ++j)
      if (vnl_math_abs((*this)(i,j)) > tol)
	return false;

  return true;
}

// -- Return true if any element of (*this) is nan
template <class T>
bool vnl_matrix<T>::has_nans() const
{
  for(unsigned i = 0; i < this->rows(); ++i)
    for(unsigned j = 0; j < this->columns(); ++j)
      if (vnl_math_isnan((*this)(i,j)))
	return true;

  return false;
}

// -- Return false if any element of (*this) is inf or nan
template <class T>
bool vnl_matrix<T>::is_finite() const
{
  for(unsigned i = 0; i < this->rows(); ++i)
    for(unsigned j = 0; j < this->columns(); ++j)
      if (!vnl_math_isfinite((*this)(i,j)))
	return false;

  return true;
}

// -- Abort if any element of M is inf or nan
template <class T>
void vnl_matrix<T>::assert_finite() const
{
  if (is_finite()) 
    return;

  cerr << "*** NAN FEVER **\n";
  cerr << *this;
  abort();
}

// -- Abort unless M has the given size.
template <class T>
void vnl_matrix<T>::assert_size(unsigned rs,unsigned cs) const
{
  if (this->rows()!=rs || this->cols()!=cs) {
    cerr << "vnl_matrix : has size " << this->rows() << 'x' << this->cols() 
	 << ". Should be " << rs << 'x' << cs << endl;
    abort();
  }
}

// -- Read a vnl_matrix from an ascii istream, automatically
// determining file size if the input matrix has zero size.
template <class T>
bool vnl_matrix<T>::read_ascii(istream& s)
{
  if (!s.good()) {
    cerr << "vnl_matrix<T>::read_ascii: Called with bad istream\n";
    return false;
  }

  bool size_known = (this->rows() != 0);

  if (size_known) {
    for(unsigned i = 0; i < this->rows(); ++i)
      for(unsigned j = 0; j < this->columns(); ++j)
	s >> this->data[i][j];

    return s.good() || s.eof();
  }

  bool debug = false;
  
  vcl_vector<T> first_row_vals;
  if (debug)
    cerr << "vnl_matrix<T>::read_ascii: Determining file dimensions: ";

  int c = ' ';
  for (;;) {
    // Clear whitespace, looking for a newline
    while (1) {
      c = s.get();
      if (c == EOF)
	goto loademup;
      if (!isspace(c)) {
	if (!s.putback(c).good())
	  cerr << "vnl_matrix<T>::read_ascii: Could not push back '" << c << "'\n";
	
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
    if (s.eof())
      goto loademup;
    first_row_vals.push_back(val);
  }
 loademup:
  int colz = first_row_vals.size();

  if (debug) cerr << colz << " cols, ";

  if (colz == 0)
    return false;

  // need to be careful with resizing here as will often be reading humungous files
  // So let's just build an array of row pointers
  vcl_vector<T*> row_vals;
  row_vals.reserve(1000);
  {
    // Copy first row.  Can't use first_row_vals, as may be a vector of bool...
    T* row = new T[colz];
    for(int k = 0; k < colz; ++k)
      row[k] = first_row_vals[k];
    row_vals.push_back(row);
  }

  while (1) {
    T* row = new T[colz];
    if (row == 0) {
      cerr << "vnl_matrix<T>::read_ascii: Error, Out of memory on row " << row_vals.size() << endl;
      return false;
    }
    s >> row[0];
    if (!s.good())
      break;
    for(int k = 1; k < colz; ++k) {
      if (s.eof()) {
	cerr << "vnl_matrix<T>::read_ascii: Error, EOF on row " << row_vals.size() << ", column " << k << endl;
	return false;
      }
      s >> row[k];
      if (!s.good()) {
	cerr << "vnl_matrix<T>::read_ascii: Error, row " << row_vals.size() << " failed on column " << k << endl;
	return false;
      }
    }
    row_vals.push_back(row);
  }
  
  int rowz = row_vals.size();

  if (debug)
    cerr << rowz << " rows.\n";

  resize(rowz, colz);

  T* p = this->data[0];
  for(int i = 0; i < rowz; ++i) {
    for(int j = 0; j < colz; ++j)
      *p++ = row_vals[i][j];
    if (i > 0) delete [] row_vals[i];
  }

  return true;
}

// -- Read a vnl_matrix from an ascii istream, automatically
// determining file size if the input matrix has zero size.
// This is a static method so you can type
// <verb>
// vnl_matrix<float> M = vnl_matrix<float>::read(cin);
// </verb>
// which many people prefer to the ">>" alternative.
template <class T>
vnl_matrix<T> vnl_matrix<T>::read(istream& s)
{
  vnl_matrix<T> M;
  s >> M;
  return M;
}

// Statics
// char* CoolBaseMatrix::print_format = 0;

template <class T>
void vnl_matrix<T>::set_print_format(const char* x)
{
  delete [] print_format;
  print_format = strcpy(new char[strlen(x)+1], x);
}

template <class T>
void vnl_matrix<T>::reset_print_format()
{
  delete [] print_format;
  print_format = 0;
}

template <class T>
const char* vnl_matrix<T>::get_print_format()
{
  if (print_format == 0)
    set_print_format("%g ");
  return print_format;
}

template <class T>
bool vnl_matrix<T>::print_format_set()
{
  return (print_format != 0);
}

// -- Reverse order of rows.  Name is from Matlab, meaning "flip upside down".
template <class T>
void vnl_matrix<T>::flipud()
{
  int n = this->rows();
  int colz = this->columns();

  int m = n / 2;
  for(int r = 0; r < m; ++r) {
    int r1 = r;
    int r2 = n - 1 - r;
    for(int c = 0; c < colz; ++c) {
      T tmp = (*this)(r1, c);
      (*this)(r1, c) = (*this)(r2, c);
      (*this)(r2, c) = tmp;
    }
  }
}
// -- Reverse order of columns.
template <class T>
void vnl_matrix<T>::fliplr()
{
  int n = this->cols();
  int rowz = this->rows();

  int m = n / 2;
  for(int c = 0; c < m; ++c) {
    int c1 = c;
    int c2 = n - 1 - c;
    for(int r = 0; r < rowz; ++r) {
      T tmp = (*this)(c1, r);
      (*this)(c1, r) = (*this)(c2, r);
      (*this)(c2, r) = tmp;
    }
  }
}

template <class doublereal>
int vnl_inplace_transpose(doublereal *a, int *m, int* n, int* mn, int* move, int* iwrk, int* iok)
{
    /* System generated locals */
    int i__1, i__2;

    /* Local variables */
    static doublereal b, c, d;
    static int i, j, k, i1, i2, j1, n1, im, i1c, i2c, ncount, ir0, ir1, 
	    ir2, kmi, max_;

/* ***** */
/*  ALGORITHM 380 - REVISED */
/* ***** */
/*  A IS A ONE-DIMENSIONAL ARRAY OF LENGTH MN=M*N, WHICH */
/*  CONTAINS THE MXN MATRIX TO BE TRANSPOSED (STORED */
/*  COLUMWISE). MOVE IS A ONE-DIMENSIONAL ARRAY OF LENGTH IWRK */
/*  USED TO STORE INFORMATION TO SPEED UP THE PROCESS.  THE */
/*  VALUE IWRK=(M+N)/2 IS RECOMMENDED. IOK INDICATES THE */
/*  SUCCESS OR FAILURE OF THE ROUTINE. */
/*  NORMAL RETURN  IOK=0 */
/*  ERRORS         IOK=-1 ,MN NOT EQUAL TO M*N */
/*                 IOK=-2 ,IWRK NEGATIVE OR ZERO */
/*                 IOK.GT.0, (SHOULD NEVER OCCUR),IN THIS CASE */
/*  WE SET IOK EQUAL TO THE FINAL VALUE OF I WHEN THE SEARCH */
/*  IS COMPLETED BUT SOME LOOPS HAVE NOT BEEN MOVED */
/*  NOTE * MOVE(I) WILL STAY ZERO FOR FIXED POINTS */
/* CHECK ARGUMENTS AND INITIALIZE. */
    /* Parameter adjustments */
    --move;
    --a;

    /* Function Body */
    if (*m < 2 || *n < 2) {
	goto L120;
    }
    if (*mn != *m * *n) {
	goto L180;
    }
    if (*iwrk < 1) {
	goto L190;
    }
    if (*m == *n) {
	goto L130;
    }
    ncount = 2;
    k = *mn - 1;
    i__1 = *iwrk;
    for (i = 1; i <= i__1; ++i) {
	move[i] = 0;
/* L10: */
    }
    if (*m < 3 || *n < 3) {
	goto L30;
    }
/* CALCULATE THE NUMBER OF FIXED POINTS, EUCLIDS ALGORITHM */
/* FOR GCD(M-1,N-1). */
    ir2 = *m - 1;
    ir1 = *n - 1;
L20:
    ir0 = ir2 % ir1;
    ir2 = ir1;
    ir1 = ir0;
    if (ir0 != 0) {
	goto L20;
    }
    ncount = ncount + ir2 - 1;
/* SET INITIAL VALUES FOR SEARCH */
L30:
    i = 1;
    im = *m;
/* AT LEAST ONE LOOP MUST BE RE-ARRANGED */
    goto L80;
/* SEARCH FOR LOOPS TO REARRANGE */
L40:
    max_ = k - i;
    ++i;
    if (i > max_) {
	goto L160;
    }
    im += *m;
    if (im > k) {
	im -= k;
    }
    i2 = im;
    if (i == i2) {
	goto L40;
    }
    if (i > *iwrk) {
	goto L60;
    }
    if (move[i] == 0) {
	goto L80;
    }
    goto L40;
L50:
    i2 = *m * i1 - k * (i1 / *n);
L60:
    if (i2 <= i || i2 >= max_) {
	goto L70;
    }
    i1 = i2;
    goto L50;
L70:
    if (i2 != i) {
	goto L40;
    }
/* REARRANGE THE ELEMENTS OF A LOOP AND ITS COMPANION LOOP */
L80:
    i1 = i;
    kmi = k - i;
    b = a[i1 + 1];
    i1c = kmi;
    c = a[i1c + 1];
L90:
    i2 = *m * i1 - k * (i1 / *n);
    i2c = k - i2;
    if (i1 <= *iwrk) {
	move[i1] = 2;
    }
    if (i1c <= *iwrk) {
	move[i1c] = 2;
    }
    ncount += 2;
    if (i2 == i) {
	goto L110;
    }
    if (i2 == kmi) {
	goto L100;
    }
    a[i1 + 1] = a[i2 + 1];
    a[i1c + 1] = a[i2c + 1];
    i1 = i2;
    i1c = i2c;
    goto L90;
/* FINAL STORE AND TEST FOR FINISHED */
L100:
    d = b;
    b = c;
    c = d;
L110:
    a[i1 + 1] = b;
    a[i1c + 1] = c;
    if (ncount < *mn) {
	goto L40;
    }
/* NORMAL RETURN */
L120:
    *iok = 0;
    return 0;
/* IF MATRIX IS SQUARE,EXCHANGE ELEMENTS A(I,J) AND A(J,I). */
L130:
    n1 = *n - 1;
    i__1 = n1;
    for (i = 1; i <= i__1; ++i) {
	j1 = i + 1;
	i__2 = *n;
	for (j = j1; j <= i__2; ++j) {
	    i1 = i + (j - 1) * *n;
	    i2 = j + (i - 1) * *m;
	    b = a[i1];
	    a[i1] = a[i2];
	    a[i2] = b;
/* L140: */
	}
/* L150: */
    }
    goto L120;
/* ERROR RETURNS. */
L160:
    *iok = i;
L170:
    return 0;
L180:
    *iok = -1;
    goto L170;
L190:
    *iok = -2;
    goto L170;
} /* dtrans_ */

// -- Transpose matrix M in place.  Works for rectangular matrices using an
// enormously clever algorithm from ACM TOMS.
template <class T>
void vnl_matrix<T>::inplace_transpose()
{
  int m = rows();
  int n = columns();
  int iwrk = (m+n/2);
  vnl_vector<int> move(iwrk);

  int iok;
  int mn = m*n;
  ::vnl_inplace_transpose(data_block(), &n, &m, &mn, move.data_block(), &iwrk, &iok);
  if (iok != 0)
    cerr << __FILE__ " : inplace_transpose() -- iok = " << iok << endl;

  this->num_rows = n;
  this->num_cols = m;
}

//--------------------------------------------------------------------------------

// instantiation macros.
#include <vcl/vcl_compiler.h>

// fsm: I'm not really sure this next piece of logic is necessary.
// Why not just use VCL_INSTANTIATE_INLINE ?
#if defined (VCL_SUNPRO_CC) || defined(VCL_GCC_27)
# define VCL_MATRIX_INSTANTIATE_INLINE(fn_decl)  template  fn_decl
#else
# define VCL_MATRIX_INSTANTIATE_INLINE(fn_decl)  
#endif

#define VNL_MATRIX_INSTANTIATE_internal(T) \
VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(char * vnl_matrix<T >::print_format = 0); \
template class vnl_matrix<T >; \
VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(vnl_matrix<T >::print_format); \
template vnl_matrix<T > operator-(T const &, vnl_matrix<T > const &); \
VCL_MATRIX_INSTANTIATE_INLINE(vnl_matrix<T > operator+(T const &, vnl_matrix<T > const &)); \
VCL_MATRIX_INSTANTIATE_INLINE(vnl_matrix<T > operator*(T const &, vnl_matrix<T > const &)); \
template T dot_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template T inner_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template T cos_angle(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template vnl_matrix<T > element_product(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template vnl_matrix<T > element_quotient(vnl_matrix<T > const &, vnl_matrix<T > const &); \
template int vnl_inplace_transpose(T*, int*, int*, int*, int*, int*, int*); \
template ostream & operator<<(ostream &, vnl_matrix<T > const &); \
template istream & operator>>(istream &, vnl_matrix<T >       &); \
VCL_MATRIX_INSTANTIATE_INLINE(bool operator!=(vnl_matrix<T > const &, vnl_matrix<T > const &))

// macro for types which have no operator<(), such as bool (on SGI CC).
#define VNL_MATRIX_INSTANTIATE_no_ordering(T) \
VNL_MATRIX_INSTANTIATE_internal(T)

// float, double
#define VNL_MATRIX_INSTANTIATE_floating_real(T) \
VNL_MATRIX_INSTANTIATE_internal(T)

// complex<float>, complex<double>
#define VNL_MATRIX_INSTANTIATE_floating_complex(T) \
VNL_MATRIX_INSTANTIATE_no_ordering(T)

// (signed|unsigned) (char|short|int|long)
#define VNL_MATRIX_INSTANTIATE_integral(T) \
VNL_MATRIX_INSTANTIATE_internal(T)
