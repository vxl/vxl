// This is vxl/vbl/vbl_array_2d.h
#ifndef vbl_array_2d_h_
#define vbl_array_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

//: simple 2D array
template <class T>
class vbl_array_2d
{
 public:

  //: Default constructor
  vbl_array_2d() { construct(); }

  //: Construct m-by-n array.
  vbl_array_2d(int m, int n) { construct(m, n); }

  //: Construct and fill an m-by-n array.
  vbl_array_2d(int m, int n, const T &v) { construct(m, n); fill(v);}

  //: Construct from a 2d array
  vbl_array_2d(vbl_array_2d<T> const &that) {
    construct(that.rows(), that.cols());
    operator=(that);
  }

  //: Destructor
  ~vbl_array_2d() { destruct(); }

  //: Assignment
  vbl_array_2d<T>& operator=(vbl_array_2d<T> const &that) {
    resize(that.rows(), that.cols());
    for (int i=0; i<num_rows_; ++i)
      for (int j=0; j<num_cols_; ++j)
        rows_[i][j] = that.rows_[i][j];
    return *this;
  }

  //: Comparison
  bool operator==(vbl_array_2d<T> const &that) const {
    if (num_rows_ != that.num_rows_ || num_cols_ != that.num_cols_)
      return false;
    for (int i=0; i<num_rows_; ++i)
      for (int j=0; j<num_cols_; ++j)
        if (!( rows_[i][j] == that.rows_[i][j] )) // do not assume we have operator!=
          return false;
    return true;
  }

  //: 
  bool operator!=(vbl_array_2d<T> const &that) const {
    return ! operator==(that);
  }

  //: fill with `value'
  void fill(T value) {
    for (int i=0; i<num_rows_; ++i)
      for (int j=0; j<num_cols_; ++j)
        rows_[i][j] = value;
  }

  //: change size.
  void resize(int m, int n) {
    if (m != num_rows_ || n != num_cols_) {
      destruct();
      construct(m, n);
    }
  }

  //: make as if default-constructed.
  void clear() {
    if (rows_) {
      destruct();
      construct();
    }
  }

  // Data Access---------------------------------------------------------------
  T const& operator() (int i, int j) const { return rows_[i][j]; }
  T      & operator() (int i, int j) { return rows_[i][j]; }

  void put(int i, int j, T const &x) { rows_[i][j] = x; }
  T    get(int i, int j) const { return rows_[i][j]; }

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

  void construct() {
    rows_ = 0;
    num_rows_ = 0;
    num_cols_ = 0;
  }

  void construct(int m, int n) {
    num_rows_ = m;
    num_cols_ = n;
    if (m && n) {
      rows_ = new T * [m];
      T* p = new T[m * n];
      for (int i = 0; i < m; ++i)
        rows_[i] = p + i * n;
    }
    else {
      rows_ = 0;
    }
  }

  void destruct() {
    if (rows_) {
      delete [] rows_[0];
      delete [] rows_;
    }
  }
};

export template <class T>
vcl_ostream& operator<<(vcl_ostream &, vbl_array_2d<T> const &);

#define VBL_ARRAY_2D_INSTANTIATE \
extern "please include vbl/vbl_array_2d.txx instead"

#endif // vbl_array_2d_h_
