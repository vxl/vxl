#ifndef vbl_array_2d_h_
#define vbl_array_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_array_2d
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_array_2d.h
// .FILE	vbl_array_2d.txx
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
  vbl_array_2d();
  vbl_array_2d(int m, int n);
  vbl_array_2d(vbl_array_2d<T> const &);
 ~vbl_array_2d();

  vbl_array_2d& operator=(const vbl_array_2d<T>&);

  // Operations----------------------------------------------------------------
  void fill(T value);
  void resize(int m, int n);

  // Data Access---------------------------------------------------------------
  T const& operator() (int i, int j) const { return rows_[i][j]; }
  T      & operator() (int i, int j) { return rows_[i][j]; }

  void put(int i, int j, T const &x) { rows_[i][j] = x; }
  T get(int i, int j) const { return rows_[i][j]; }

  T const* operator[] (int i) const { return rows_[i]; }
  T      * operator[] (int i) { return rows_[i]; }

  int rows() const { return num_rows_; }
  int cols() const { return num_cols_; }
  int columns() const { return num_cols_; }

  T      *      * get_rows() { return rows_; }
  T const* const* get_rows() const { return rows_; }

private:
  //protected:
  T** rows_;
  int num_rows_;
  int num_cols_;

  //protected:
  void destroy();
  void create(int m, int n);
};

template<class Type>
ostream& operator<< (ostream& os, const vbl_array_2d<Type> &v);

#define VBL_ARRAY_2D_INSTANTIATE \
extern "please include vbl/vbl_array_2d.txx instead"

#endif // vbl_array_2d_h_
