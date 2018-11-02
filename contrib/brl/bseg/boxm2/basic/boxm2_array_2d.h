#ifndef boxm2_array_2d_h_
#define boxm2_array_2d_h_
//:
// \file
#include <iosfwd>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: simple wrapper for a 1D buffer (re interpreted as 2D)
template <class T>
class boxm2_array_2d
{
 public:
  typedef std::size_t size_type;
 public:

  //: Default constructor
  boxm2_array_2d() { construct(); }

  //: Construct m-by-n array.
  boxm2_array_2d(size_type m, size_type n, T* buff) { construct(m, n, buff); }

  //: Construct and fill an m-by-n array.
  boxm2_array_2d(size_type m, size_type n, T* buff, const T &v) { construct(m, n, buff); fill(v);}

  //: Destructor
  ~boxm2_array_2d() { destruct(); }

  //: Assignment
  boxm2_array_2d<T>& operator=(boxm2_array_2d<T> const &that) {
    destruct();
    construct(that.rows(), that.cols(), that.buffer_);
    return *this;
  }

  //: Comparison
  bool operator==(boxm2_array_2d<T> const &that) const {
    if (num_rows_ != that.num_rows_ || num_cols_ != that.num_cols_)
      return false;
    for (size_type i=0; i<num_rows_; ++i)
      for (size_type j=0; j<num_cols_; ++j)
        if (!( rows_[i][j] == that.rows_[i][j] )) // do not assume we have operator!=
          return false;
    return true;
  }

  //:
  bool operator!=(boxm2_array_2d<T> const &that) const {
    return ! operator==(that);
  }

  //: fill with `value'
  void fill(T value) {
    for (size_type i=0; i<num_rows_; ++i)
      for (size_type j=0; j<num_cols_; ++j)
        rows_[i][j] = value;
  }

  //: change size.
  void resize(size_type m, size_type n) {
    if (m != num_rows_ || n != num_cols_) {
      destruct();
      construct(m, n, buffer_);
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
  T const& operator() (size_type i, size_type j) const { return rows_[i][j]; }
  T      & operator() (size_type i, size_type j) { return rows_[i][j]; }

  void put(size_type i, size_type j, T const &x) { rows_[i][j] = x; }
  T    get(size_type i, size_type j) const { return rows_[i][j]; }

  T const* operator[] (size_type i) const { return rows_[i]; }
  T      * operator[] (size_type i) { return rows_[i]; }


  //: Return number of rows
  size_type rows() const { return num_rows_; }

  //: Return number of columns
  size_type cols() const { return num_cols_; }

  //: Return number of columns
  size_type columns() const { return num_cols_; }

  //: Return size = (number of rows) * (number of columns)
  size_type size() const { return num_rows_ * num_cols_; }

  T      *      * get_rows() { return rows_; }
  T const* const* get_rows() const { return rows_; }

  typedef T       *iterator;
  iterator begin() { return rows_[0]; }
  iterator end  () { return rows_[0] + num_cols_ * num_rows_; }

  typedef T const *const_iterator;
  const_iterator begin() const { return rows_[0]; }
  const_iterator end  () const { return rows_[0] + num_cols_ * num_rows_; }

 private:
  T*  buffer_;
  T** rows_;
  size_type num_rows_;
  size_type num_cols_;

  void construct() {
    rows_ = nullptr;
    num_rows_ = 0;
    num_cols_ = 0;
  }

  void construct(size_type m, size_type n, T* buffer) {
    num_rows_ = m;
    num_cols_ = n;
    buffer_   = buffer;
    if (m && n) {
      rows_ = new T * [m];
      for (size_type i = 0; i < m; ++i)
        rows_[i] = buffer + i * n;
    }
    else {
      rows_ = nullptr;
    }
  }

  //: Destruct just deletes the row array of pointers that was allocated for 2d indexing
  void destruct() {
    if (rows_) {
      delete [] rows_;
    }
  }
};

template <class T>
std::ostream& operator<<(std::ostream &, boxm2_array_2d<T> const &);

#define BOXM2_ARRAY_2D_INSTANTIATE \
extern "please include vbl/boxm2_array_2d.txx instead"

#endif // boxm2_array_2d_h_
