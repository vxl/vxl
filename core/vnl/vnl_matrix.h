#ifndef vnl_matrix_h_
#define vnl_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif

// .SECTION Description
// The vnl_matrix<T> class implements two-dimensional arithmetic
// matrices  for  a user-specified numeric data type. Using the
// parameterized types facility of C++,  it  is  possible,  for
// example, for the user to create a matrix of rational numbers
// by parameterizing the vnl_matrix class over the Rational  class.
// The  only  requirement  for the type is that it supports the
// basic arithmetic operators.
//
// Note  that  unlike   the   other   sequence   classes,   the
// vnl_matrix<T>  class is fixed-size. It will not grow once the
// size has been specified to the constructor or changed by the
// assignment  or  multiplication  operators.  The vnl_matrix<T>
// class is row-based with addresses of rows being cached,  and
// elements accessed as m[row][col].
//
// Indexing of the matrix is zero-based, so the top-left element is M(0,0).
// 
// Inversion of matrix M, and other operations such as solving systems of linear
// equations are handled by the matrix decomposition classes in vnl/algo, such
// as matrix_inverse, svd, qr etc.
//
// Use a vnl_vector<T> with these matrices.
//  
// * Gcc 2.7.2 can't handle many traits, so norms are returned as the 
//   parameterizing type.  They *are* computed using abs_t, just converted to 
//   T on return.  This imposes the constraint that T must be convertible
//   to and from the numeric_traits<T>::abs_t.
//
// * Should have a constructor from DiagMatrix<T>, but
//   very weird things happen. G++ 2.7.2 compiles it and instantiates it fine,
//   but barfs when an vnl_vector<int> is declared nearby.

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_error.h>
#include <vnl/vnl_c_vector.h>

template <class T> class vnl_vector;
template <class T> class vnl_matrix;

// // define _vnl_matrix_h_INCLUDED for IUE compatibility
// #define _vnl_matrix_h_INCLUDED

// // define which makes life easy for the IUE
// #define IUE_diagonal_matrix DiagMatrix

// // forward declare ObjectStore support class
// template <class T> class vnl_matrix_HelperObjectStore;

//--------------------------------------------------------------------------------

template <class T> vnl_matrix<T> operator+ (T const&, vnl_matrix<T> const&);
template <class T> vnl_matrix<T> operator- (T const&, vnl_matrix<T> const&);
template <class T> vnl_matrix<T> operator* (T const&, vnl_matrix<T> const&);
template <class T> vnl_matrix<T> element_product(vnl_matrix<T> const&,vnl_matrix<T> const&);
template <class T> vnl_matrix<T> element_quotient(vnl_matrix<T> const&,vnl_matrix<T> const&);
template <class T> T             dot_product (vnl_matrix<T> const&, vnl_matrix<T> const&); 
template <class T> T             inner_product (vnl_matrix<T> const&, vnl_matrix<T> const&); 
template <class T> T             cos_angle(vnl_matrix<T> const&, vnl_matrix<T> const& );
template <class T> ostream& operator<< (ostream& os, vnl_matrix<T> const& m);
template <class T> istream& operator>> (istream& os, vnl_matrix<T>& m);

//--------------------------------------------------------------------------------

//
template<class T>
class vnl_matrix {
public:
  vnl_matrix();                                                         // empty matrix.
  vnl_matrix(unsigned r, unsigned c);                           // r rows, c cols.
  vnl_matrix(unsigned r, unsigned c, T const& v0);              // r rows, c cols, value v0.
  vnl_matrix(unsigned r, unsigned c, int n, T const values[]);	// use automatic arrays.
  vnl_matrix(T const* data_block, unsigned r, unsigned c);      // fill row-wise.
  vnl_matrix(vnl_matrix<T> const&);                                     // from another matrix.
  //vnl_matrix(const DiagMatrix<T>&); this confuses g++ 2.7.2 When an vnl_vector<int> is declared nearby...
  ~vnl_matrix();

  // -- Return number of rows/columns/elements
  unsigned rows ()    const { return num_rows; }
  unsigned columns () const { return num_cols; }
  unsigned cols ()    const { return num_cols; }
  unsigned size ()    const { return rows()*cols(); }

  // get/set with boundary checks if error checking is on.
  void put (unsigned r, unsigned c, T const&);        // Assign value.
  T    get (unsigned r, unsigned c) const;            // Get value.

  // return pointer to given row
  inline T       * operator[] (unsigned r) { return data[r]; }
  inline T const * operator[] (unsigned r) const { return data[r]; }

  // no boundary checks here. meant to be fast.
protected: // fsm: who uses these?
  T       & operator() (unsigned r) { return this->data[r][0]; }
  T const & operator() (unsigned r) const { return this->data[r][0]; }
public:
  T       & operator() (unsigned r, unsigned c) { return this->data[r][c]; }
  T const & operator() (unsigned r, unsigned c) const { return this->data[r][c]; }

  // filling and copying
  void fill (T const&);          // fill with value
  void fill_diagonal (T const&);
  void copy_in(T const *);       // laminate matrix rowwise from an array.
  void copy_out(T *) const;      // copy matrix rowwise into an array.
  inline void set(T const *d) { copy_in(d); } 
  
  // assignment from scalars and matrices :
  inline vnl_matrix<T>& operator= (T const&v) { fill(v); return *this; } 
  vnl_matrix<T>& operator= (vnl_matrix<T> const&);
  
  // arithmetic  
  vnl_matrix<T>& operator+= (T const&);
  vnl_matrix<T>& operator*= (T const&);
  vnl_matrix<T>& operator/= (T const&);
  vnl_matrix<T>& operator-= (T const&);
  
  vnl_matrix<T>& operator+= (vnl_matrix<T> const&);
  vnl_matrix<T>& operator-= (vnl_matrix<T> const&);
  vnl_matrix<T>& operator*= (vnl_matrix<T> const&);

  vnl_matrix<T> operator- () const;	    // negation and 
  vnl_matrix<T> operator+ (T const&) const; // all binary operations 
  vnl_matrix<T> operator- (T const&) const; // return by values.
  vnl_matrix<T> operator* (T const&) const; //
  vnl_matrix<T> operator/ (T const&) const;

  vnl_matrix<T> operator+ (vnl_matrix<T> const& rhs) const;
  vnl_matrix<T> operator- (vnl_matrix<T> const& rhs) const;
  vnl_matrix<T> operator* (vnl_matrix<T> const& rhs) const;

  // binary friend operators
  friend vnl_matrix<T> operator-        VCL_STL_NULL_TMPL_ARGS (T const&, vnl_matrix<T> const&);
  friend vnl_matrix<T> operator+        VCL_STL_NULL_TMPL_ARGS (T const&, vnl_matrix<T> const&);
  friend vnl_matrix<T> operator*        VCL_STL_NULL_TMPL_ARGS (T const&, vnl_matrix<T> const&);

  friend vnl_matrix<T> element_product  VCL_STL_NULL_TMPL_ARGS (vnl_matrix<T> const&,
								vnl_matrix<T> const&);
  friend vnl_matrix<T> element_quotient VCL_STL_NULL_TMPL_ARGS (vnl_matrix<T> const&,
								vnl_matrix<T> const&);
  friend T             inner_product    VCL_STL_NULL_TMPL_ARGS (vnl_matrix<T> const&, // conjugates
								vnl_matrix<T> const&);// 2nd arg
  friend T             dot_product      VCL_STL_NULL_TMPL_ARGS (vnl_matrix<T> const&, // no conjugate
								vnl_matrix<T> const&); 

  ////--------------------------- Additions ----------------------------
  
  //
  vnl_matrix<T> apply(T (*f)(T)) const;
  vnl_matrix<T> apply(T (*f)(T const&)) const;

  // transpose 
  vnl_matrix<T> transpose () const;           // transpose row/column.
  vnl_matrix<T> conjugate_transpose () const; // transpose and conjugate.

  // submatrices
  vnl_matrix<T>& update (vnl_matrix<T> const&, unsigned top=0, unsigned left=0);
  void set_column(unsigned col_index, T const *);
  void set_column(unsigned col_index, T value );
  void set_column(unsigned j, vnl_vector<T> const&);
  void set_columns(unsigned starting_column, vnl_matrix<T> const&);
  void set_row   (unsigned col_index, T const *);
  void set_row   (unsigned col_index, T value );
  void set_row   (unsigned i, vnl_vector<T> const&);
  
  vnl_matrix<T> extract (unsigned rows,  unsigned cols, 
			 unsigned top=0, unsigned left=0) const;
  vnl_vector<T> get_row   (unsigned row) const;
  vnl_vector<T> get_column(unsigned col) const;
  vnl_matrix<T> get_n_rows   (unsigned rowstart, unsigned n) const;
  vnl_matrix<T> get_n_columns(unsigned colstart, unsigned n) const;
  
  // mutators
  void set_identity();
  void inplace_transpose();
  void flipud();
  void fliplr();
  void normalize_rows();
  void normalize_columns();
  void scale_row   (unsigned row, T value);
  void scale_column(unsigned col, T value);
  
  // norms etc. see vnl_c_vector<T> for the meaning of the norms.
  typedef vnl_c_vector<T>::abs_t abs_t;
  abs_t fro_norm() const { return vnl_c_vector<T>::two_norm(begin(), size()); }
  abs_t one_norm() const { return vnl_c_vector<T>::one_norm(begin(), size()); }
  abs_t inf_norm() const { return vnl_c_vector<T>::inf_norm(begin(), size()); }
  abs_t rms     () const { return vnl_c_vector<T>::rms_norm(begin(), size()); }
  T min_value () const { return vnl_c_vector<T>::min_value(begin(), size()); }
  T max_value () const { return vnl_c_vector<T>::max_value(begin(), size()); }
  T mean() const { return vnl_c_vector<T>::mean(begin(), size()); }

  // predicates
  bool is_identity(double tol = 0) const;
  bool is_zero(double tol = 0) const;
  bool is_finite() const;
  bool has_nans() const;
  
  //
  void assert_size(unsigned rows, unsigned cols) const;
  void assert_finite() const;

  ////----------------------- Input/Output ----------------------------

  static vnl_matrix<T> read(istream& s);
  bool read_ascii(istream& s);

  // (see also mat_ops)
  static void set_print_format(char const* c);
  static void reset_print_format();
  static const char* get_print_format();
  static bool print_format_set();

  //--------------------------------------------------------------------------------
  
  // -- access the contiguous block storing the elements in the matrix row-wise. O(1).
  // 1d array, row-major order.  
  inline T const* data_block () const { return data[0]; }
  inline T      * data_block () { return data[0]; }

  // -- access the 2D array, so that elements can be accessed with array[row][col] directly.
  // 2d array, [row][column].
  inline T const* const* data_array () const { return data; }
  inline T      *      * data_array () { return data; }

  // iterators
  typedef T element_type;
  typedef T       *iterator;
  iterator       begin() { return data[0]; }
  iterator       end() { return data[0]+num_rows*num_cols; }
  typedef T const *const_iterator;
  const_iterator begin() const { return data[0]; }
  const_iterator end() const { return data[0]+num_rows*num_cols; }

  ////--------------------------- Vector ---------------------------------------
  
  // using a 2d matrix to represent a 1d vector is less efficient in time and space.
  T      & x ()       { return data[0][0]; }
  T      & y ()       { return data[1][0]; }
  T      & z ()       { return data[2][0]; }
  T      & t ()       { return data[3][0]; }
  T const& x () const { return data[0][0]; }
  T const& y () const { return data[1][0]; }
  T const& z () const { return data[2][0]; }
  T const& t () const { return data[3][0]; }
  
  //--------------------------------------------------------------------------------

  // comparison
  inline bool operator ==(vnl_matrix<T> const &that) const { return this->operator_eq(that); }
  bool operator_eq (vnl_matrix<T> const &) const;
  void print(ostream& os) const;

  //--------------------------------------------------------------------------------
  bool resize (unsigned r, unsigned c); // True if size changes

protected:
  unsigned num_rows;   // Number of rows
  unsigned num_cols;   // Number of columns
  T** data;            // Pointer to the vnl_matrix 
  
  // -- Holds the format for printf-style output
  static char* print_format;

//   // give ObjectStore support class access to data
//   friend class vnl_matrix_HelperObjectStore<T>;
};
//

//--------------------------------------------------------------------------------
//
// Definitions of inline functions.
//

// get -- Returns the value of the element at specified row and column. O(1).
// Checks for valid range of indices.

template<class T> 
inline T vnl_matrix<T>::get (unsigned row, unsigned column) const {
#if ERROR_CHECKING
  if (row >= this->num_rows)                    // If invalid size specified
    vnl_error_matrix_row_index ("get", row);    // Raise exception
  if (column >= this->num_cols)                 // If invalid size specified
    vnl_error_matrix_col_index ("get", column); // Raise exception
#endif
  return this->data[row][column];
}

// put -- Puts value into element at specified row and column. O(1). 
// Checks for valid range of indices.

template<class T> 
inline void vnl_matrix<T>::put (unsigned row, unsigned column, T const& value) {
#if ERROR_CHECKING
  if (row >= this->num_rows)                    // If invalid size specified
    vnl_error_matrix_row_index ("put", row);  // Raise exception
  if (column >= this->num_cols)                 // If invalid size specified
    vnl_error_matrix_col_index ("put", column); // Raise exception
#endif
  this->data[row][column] = value;              // Assign data value
}


// operator-= -- Mutates lhs matrix to substracts in place,
// its elements with value. O(m*n).

template<class T> 
inline vnl_matrix<T>& vnl_matrix<T>::operator-= (T const& value) {
  return *this += (- value);
}

// operator- -- Returns new matrix with elements of lhs matrix substacted
// with value. O(m*n).

template<class T> 
inline vnl_matrix<T> vnl_matrix<T>::operator-(T const& value) const {
  return (*this) + (- value);
}

// operator*= -- Multiplies lhs matrix with rhs matrix, 
// then assigns the product to lhs matrix. O(n^3).

template<class T>
inline vnl_matrix<T>& vnl_matrix<T>::operator*= (vnl_matrix<T> const&rhs) {
  *this = (*this) * rhs;			// multiply then assign
  return *this;
}

// -- 

template<class T>
inline vnl_matrix<T> vnl_matrix<T>::operator+ (vnl_matrix<T> const& rhs) const {
  vnl_matrix<T> result(*this);
  result += rhs;
  return result;
}

// -- Returns a new matrix containing the addition/substraction 
// of two matrices m1 and m2.

template<class T>
inline vnl_matrix<T> vnl_matrix<T>::operator- (vnl_matrix<T> const& rhs) const {
  vnl_matrix<T> result(*this);
  result -= rhs;
  return result;
}

////--------------------------- Inline friends ---------------------------------------

// -- Returns new matrix with elements of matrix m added with
// value. O(m*n).

template<class T> 
inline vnl_matrix<T> operator+ (T const& value, vnl_matrix<T> const& m) {
  return m + value;
}


// -- Returns new matrix with elements of matrix m multiplied with
// value. O(m*n).

template<class T> 
inline vnl_matrix<T> operator* (T const& value, vnl_matrix<T> const& m) {
  return m * value;
}

//--------------------------------------------------------------------------------

// #ifdef IUE
// // Overloads of global IUEg_getTypeId, etc. (if using the full IUE)
// #include<MathDex/matrix_Helper.h>
// #endif

#endif
