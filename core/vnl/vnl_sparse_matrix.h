#ifndef vnl_sparse_matrix_h_
#define vnl_sparse_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif
//    vnl_sparse_matrix<T> - Simple sparse matrix.  Only those values which
//    are non-zero are stored. The sparse matrix currently supports
//    only getting/putting elements, and multiply by vector or another
//    sparse matrix.
//
//    Each row is stored as a vector of vcl_pair<unsigned int,T>, where the first
//    of the pair indicates the column index, and the second the
//    value.  All rows are stored, as vcl_vector<row>;
//
//
// Author: Rupert W. Curwen, GE CR&D, 20 Oct 98
//
// Modifications: 
//
//     Robin Flatland 5/31/99 Added pre_mult(lhs,result), where
//                            lhs is a vector.
//
//     Robin Flatland 6/08/99 Added iterator that allows sequential
//                            access to non-zero values in matrix.
//                            Iterator is controlled using reset, next,
//                            getrow, getcolumn, and value.

#include <vcl/vcl_vector.h>
#include <vnl/vnl_vector.h>

template <class T>
class vnl_sparse_matrix_pair {
public:
  unsigned int first;
  T second;
  
  vnl_sparse_matrix_pair() : first(0), second(T(0)) {}
    
  vnl_sparse_matrix_pair(unsigned int const& a, T const& b) : first(a), second(b) {}
  vnl_sparse_matrix_pair(const vnl_sparse_matrix_pair<T>& o) : first(o.first), second(o.second) {}

  vnl_sparse_matrix_pair<T>& operator=(vnl_sparse_matrix_pair const &o) {
    if (&o != this) {
      first = o.first;
      second = o.second;
    }
    return *this;
  }
};

//: Simple sparse matrix

template <class T>
class vnl_sparse_matrix {
public:
  typedef vnl_sparse_matrix_pair<T> pair_t;
#if defined(VCL_GCC_295) || defined(VCL_EGCS) || defined(VCL_GCC_27) // it barfs -- fsm
  typedef vcl_vector < VCL_SUNPRO_ALLOCATOR_HACK(/*typename*/ pair_t ) > row ; 
  typedef vcl_vector < VCL_SUNPRO_ALLOCATOR_HACK(/*typename*/ row ) > vnl_sparse_matrix_elements;
#else
  typedef vcl_vector < VCL_SUNPRO_ALLOCATOR_HACK( typename pair_t ) > row ; 
  typedef vcl_vector < VCL_SUNPRO_ALLOCATOR_HACK( typename row ) > vnl_sparse_matrix_elements;
#endif

  // typedef vcl_vector<VCL_SUNPRO_ALLOCATOR_HACK(typename pair_t)> row;

  //: Construct an empty matrix
  vnl_sparse_matrix();

  //: Construct an empty m*n matrix
  vnl_sparse_matrix(unsigned int m, unsigned int n);

  //: Construct an m*n Matrix and copy rhs into it.
  vnl_sparse_matrix(const vnl_sparse_matrix<T>& rhs);

  //: Copy another vnl_sparse_matrix<T> into this.
  vnl_sparse_matrix<T>& operator=(const vnl_sparse_matrix<T>& rhs);

  //: Multiply this*rhs, another sparse matrix.
  void mult(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  //: Multiply this*rhs, where rhs is a vector.
  //    Currently not implemented.
  void mult(const vnl_vector<T>& rhs, vnl_vector<T>& result);

  //: Multiply this*p, a fortran order matrix.
  void mult(unsigned int n, unsigned int m, const T* p, T* q);

  //: Multiplies lhs*this, where lhs is a vector
  void pre_mult(const vnl_vector<T>& lhs, vnl_vector<T>& result);

  //: Add rhs to this.
  void add(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  //: Subtract rhs from this.
  void subtract(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  //: Get a reference to an entry in the matrix.
  T& operator()(unsigned int row, unsigned int column);

  //: Get the number of rows in the matrix.
  unsigned int rows() const { return rs_; }

  //: Get the number of columns in the matrix.
  unsigned int columns() const { return cs_; }

  //: Return whether a given row is empty
  bool empty_row(unsigned int r) const { return elements[r].empty(); }

  //: This is occasionally useful.
  double sum_row(unsigned int r);

  //: Resizes the array to have r rows and c cols
  //    Currently not implemented.
  void resize( int r, int c );

  //: Resets the internal iterator 
  void reset();

  //: Moves the internal iterator to next non-zero entry in matrix.
  // Returns true if there is another value, false otherwise. Use
  // in combination with methods reset, getrow, getcolumn, and value.
  bool next();
 
  //: Returns the row of the entry pointed to by internal iterator.
  int getrow();

  //: Returns the column of the entry pointed to by internal iterator.
  int getcolumn();

  //: Returns the value pointed to by the internal iterator.
  T value();  


protected:
  vnl_sparse_matrix_elements elements;
  unsigned int rs_, cs_;

  // internal iterator
  unsigned int itr_row;
  row::iterator itr_cur;  
  bool itr_isreset;
};

#endif
