#ifndef vbl_array_2d_h_
#define vbl_array_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .LIBRARY     vbl
// .HEADER	Basics Package
// .INCLUDE     vbl/vbl_array_2d.h
// .FILE        vbl/vbl_array_2d.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 05 Aug 96
//
// .SECTION Modifications:
//    Peter Vanroose -13nov98- added copy constructor and assignment operator
//   101298 AWF Added row/column store.  The old split was just too pedantic.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iosfwd.h>

//: Simplest possible 2D array
//
template <class T>
class vbl_array_2d {
public:
  // Constructors/Destructors--------------------------------------------------
  
  vbl_array_2d();
  vbl_array_2d(int m, int n);    
  vbl_array_2d(const vbl_array_2d<T>&);
 ~vbl_array_2d();

  vbl_array_2d& operator=(const vbl_array_2d<T>&);

  // Operations----------------------------------------------------------------
  void resize(int m, int n);
  
  // Computations--------------------------------------------------------------
  
  // Data Access---------------------------------------------------------------
  const T& operator() (int i, int j) const { return rows_[i][j]; }
  T& operator() (int i, int j) { return rows_[i][j]; }

  void fill(T value);
  int rows() const;
  int columns() const;

  // Data Control--------------------------------------------------------------
  T** GetRows() { return rows_; }
  T const* const* GetRows() const { return rows_; }

protected:
  // Data Members--------------------------------------------------------------
  T** rows_;
  int num_rows_;
  int num_cols_;
  
protected:
  // Helpers-------------------------------------------------------------------
  void destroy();
  void create(int m, int n);
};

template<class Type> 
ostream& operator<< (ostream& os, const vbl_array_2d<Type> &v);

#define VBL_ARRAY_2D_INSTANTIATE extern "please include vbl/vbl_array_2d.txx instead"

#endif // vbl_array_2d_h_
