#ifndef vbl_array_2d_h_
#define vbl_array_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_array_2d.h

//:
// \file
// \brief Contains class for a templated 2d array
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   05 Aug 96
//
// \verbatim
// Modifications
// Peter Vanroose -13nov98- added copy constructor and assignment operator
// AWF 10/12/98 Added row/column store.  The old split was just too pedantic.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim


#include <vcl_iosfwd.h>

//: Simplest possible 2D array
export template <class T>
class vbl_array_2d {
public:

  //: Default constructor
  vbl_array_2d();

  //: Construct from two integers
  vbl_array_2d(int m, int n);

  // Construct and fill
  vbl_array_2d(int n1, int n2, T const& fill_value);

  //: Construct from a 2d array
  vbl_array_2d(vbl_array_2d<T> const &);


  //: Destructor
 ~vbl_array_2d();

  //: Assignment
  vbl_array_2d& operator=(vbl_array_2d<T> const&);
  bool operator==(vbl_array_2d<T> const&) const;

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


  //: Return number of rows
  int rows() const { return num_rows_; }

  //: Return number of columns
  int cols() const { return num_cols_; }

  //: Return number of columns
  int columns() const { return num_cols_; }

  //: Return size = (number of rows) * (number of columns)
  int size() const { return num_rows_ * num_cols_; }

  T      *      * get_rows() { return rows_; }
  T const* const* get_rows() const { return rows_; }

  typedef T       *iterator;
  iterator begin() { return rows_[0]; }
  iterator end  () { return rows_[0] + num_cols_ * num_rows_; }

  typedef T const *const_iterator;
  const_iterator begin() const { return rows_[0]; }
  const_iterator end  () const { return rows_[0] + num_cols_ * num_rows_; }

private:
  T** rows_;
  int num_rows_;
  int num_cols_;

  void destroy();
  void create(int m, int n);
};

export template<class Type>
vcl_ostream& operator<< (vcl_ostream& os, const vbl_array_2d<Type> &v);

#define VBL_ARRAY_2D_INSTANTIATE \
extern "please include vbl/vbl_array_2d.txx instead"

#endif // vbl_array_2d_h_
