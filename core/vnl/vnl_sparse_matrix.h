// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1998 TargetJr Consortium
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
#ifndef vnl_sparse_matrix_h_
#define vnl_sparse_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_sparse_matrix<T> - Simple sparse matrix
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_sparse_matrix.h
// .FILE        vnl/vnl_sparse_matrix.cxx
//
// .SECTION Description
//    vnl_sparse_matrix<T> - Simple sparse matrix.  Only those values which
//    are non-zero are stored. The sparse matrix currently supports
//    only getting/putting elements, and multiply by vector or another
//    sparse matrix.
//
//    Each row is stored as a vector of pair<unsigned int,T>, where the first
//    of the pair indicates the column index, and the second the
//    value.  All rows are stored, as vcl_vector<row>;
//
//
// .SECTION Author
//     Rupert W. Curwen, GE CR&D, 20 Oct 98
//
// .SECTION Modifications: 
//
//     Robin Flatland 5/31/99 Added pre_mult(lhs,result), where
//                            lhs is a vector.
//     Robin Flatland 6/08/99 Added iterator that allows sequential
//                            access to non-zero values in matrix.
//                            Iterator is controlled using reset, next,
//                            getrow, getcolumn, and value.
//
//-----------------------------------------------------------------------------

#include <assert.h>
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

  vnl_sparse_matrix_pair<T>& operator=(vnl_sparse_matrix_pair const &o)
  {
    if (&o != this)
      {
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
  typedef vcl_vector<VCL_SUNPRO_ALLOCATOR_HACK(vnl_sparse_matrix_pair<T>)> row;

  // Constructors/Destructors--------------------------------------------------

  // -- Construct an empty matrix
  vnl_sparse_matrix();

  // -- Construct an empty m*n matrix
  vnl_sparse_matrix(unsigned int m, unsigned int n);

  // -- Construct an m*n Matrix and copy rhs into it.
  vnl_sparse_matrix(const vnl_sparse_matrix<T>& rhs);

  // -- Copy another vnl_sparse_matrix<T> into this.
  vnl_sparse_matrix<T>& operator=(const vnl_sparse_matrix<T>& rhs);

  // -- Multiply this*rhs, another sparse matrix.
  void mult(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  // -- Multiply this*rhs, where rhs is a vector.
  //    Currently not implemented.
  void mult(const vnl_vector<T>& rhs, vnl_vector<T>& result);

  // -- Multiply this*p, a fortran order matrix.
  void mult(unsigned int n, unsigned int m, const T* p, T* q);

  // -- Multiplies lhs*this, where lhs is a vector
  void pre_mult(const vnl_vector<T>& lhs, vnl_vector<T>& result);

  // -- Add rhs to this.
  void add(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  // -- Subtract rhs from this.
  void subtract(const vnl_sparse_matrix<T>& rhs, vnl_sparse_matrix<T>& result);

  // -- Get a reference to an entry in the matrix.
  T& operator()(unsigned int row, unsigned int column);

  // -- Get the number of rows in the matrix.
  unsigned int rows() const { return rs; }

  // -- Get the number of columns in the matrix.
  unsigned int columns() const { return cs; }

  // -- Return whether a given row is empty
  bool empty_row(unsigned int r) const { return elements[r].empty(); }

  // -- This is occasionally useful.
  double sum_row(unsigned int r);

  // -- Resizes the array to have r rows and c cols
  //    Currently not implemented.
  void resize( int r, int c );

  // -- Resets the internal iterator 
  void reset( void );

  // -- Moves the internal iterator to next non-zero entry in matrix.
  // Returns true if there is another value, false otherwise. Use
  // in combination with methods reset, getrow, getcolumn, and value.
  bool next( void );
 
  // -- Returns the row of the entry pointed to by internal iterator.
  int getrow( void );

  // -- Returns the column of the entry pointed to by internal iterator.
  int getcolumn( void );

  // -- Returns the value pointed to by the internal iterator.
  T value( void );  



// Data members.
protected:
  vcl_vector<VCL_SUNPRO_ALLOCATOR_HACK(row)> elements;
  unsigned int rs, cs;

  // internal iterator
  unsigned int itr_row;
  row::iterator itr_cur;  
  bool itr_isreset;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_sparse_matrix.
