// This is core/vbl/vbl_array_2d.h
#ifndef vbl_array_2d_h_
#define vbl_array_2d_h_
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

#include <iosfwd>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: simple 2D array
template <class T>
class vbl_array_2d
{
 public:
  typedef std::size_t size_type;
  typedef T element_type;

 private:
  element_type** rows_;
  size_type num_rows_;
  size_type num_cols_;

 public:
  typedef T       *iterator;
  typedef T const *const_iterator;

  typedef T       &reference;
  typedef T const &const_reference;
 public:

  //: Default constructor
  vbl_array_2d() { construct(); }

  //: Construct m-by-n array.
  vbl_array_2d(size_type m, size_type n) { construct(m, n); }

  //: Construct and fill an m-by-n array.
  vbl_array_2d(size_type m, size_type n, const T &v) { construct(m, n); fill(v);}

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
    for (size_type i=0; i<num_rows_; ++i)
      for (size_type j=0; j<num_cols_; ++j)
        rows_[i][j] = that.rows_[i][j];
    return *this;
  }

  //: Comparison
  bool operator==(vbl_array_2d<T> const &that) const {
    if (num_rows_ != that.num_rows_ || num_cols_ != that.num_cols_)
      return false;
    for (size_type i=0; i<num_rows_; ++i)
      for (size_type j=0; j<num_cols_; ++j)
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
    for (size_type i=0; i<num_rows_; ++i)
      for (size_type j=0; j<num_cols_; ++j)
        rows_[i][j] = value;
  }

  //: change size.
  void resize(size_type m, size_type n) {
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
  const_reference operator() (size_type i, size_type j) const { return rows_[i][j]; }
  reference       operator() (size_type i, size_type j) { return rows_[i][j]; }

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
  size_type capacity() const { return size(); }

  T      *      * get_rows() { return rows_; }
  T const* const* get_rows() const { return rows_; }

  // iterators
  iterator begin() { return rows_[0]; }
  iterator end  () { return rows_[0] + num_cols_ * num_rows_; }

  const_iterator begin() const { return rows_[0]; }
  const_iterator end  () const { return rows_[0] + num_cols_ * num_rows_; }

 private:
  void construct() {
    rows_ = nullptr;
    num_rows_ = 0;
    num_cols_ = 0;
  }

  void construct(size_type m, size_type n) {
    num_rows_ = m;
    num_cols_ = n;
    if (m && n) {
      rows_ = new T * [m];
      T* p = new T[m * n];
      for (size_type i = 0; i < m; ++i)
        rows_[i] = p + i * n;
    }
    else {
      rows_ = nullptr;
    }
  }

  void destruct() {
    if (rows_) {
      delete [] rows_[0];
      delete [] rows_;
    }
  }
};

template <class T>
std::ostream& operator<<(std::ostream &, vbl_array_2d<T> const &);

#define VBL_ARRAY_2D_INSTANTIATE \
extern "please include vbl/vbl_array_2d.hxx instead"

#endif // vbl_array_2d_h_
