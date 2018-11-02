// This is brl/bseg/boxm2/basic/boxm2_array_3d.h
#ifndef boxm2_array_3d_h_
#define boxm2_array_3d_h_
//:
// \file
#include <iostream>
#include <cstddef>
#include <iosfwd>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef __OPTIMIZE__
# define RANGECHECK(i,j,k) ((void)0)
#else
#include <cassert>
# define RANGECHECK(i,j,k) assert(((size_type)i < row1_count_) && \
                   ((size_type)j < row2_count_) && ((size_type)k < row3_count_))
#endif

//: Simple 3d wrapper over a 1D buffer
//
template <class T>
class boxm2_array_3d
{
 public:
  typedef std::size_t size_type;
 public:
  typedef T element_type;
  typedef T* iterator;
  typedef T const* const_iterator;

  boxm2_array_3d(): buffer_(nullptr), element_(nullptr), row1_count_(0), row2_count_(0), row3_count_(0)
  {
    construct(0,0,0,nullptr);
  }

  boxm2_array_3d(size_type n1, size_type n2, size_type n3, T* buffer):buffer_(nullptr), element_(nullptr), row1_count_(0), row2_count_(0), row3_count_(0)
  {
    construct(n1, n2, n3, buffer);
  }

  boxm2_array_3d(size_type n1, size_type n2, size_type n3, T* buffer, T const& fill_value):buffer_(nullptr), element_(nullptr), row1_count_(0), row2_count_(0), row3_count_(0)
  {
    construct(n1, n2, n3, buffer);
    fill(fill_value);
  }

  ~boxm2_array_3d () { destruct(); }

  //: need to rethink = operator with only 1 data buffer
  boxm2_array_3d<T>& operator=(boxm2_array_3d<T> const& that) {
    destruct();
    construct(that.row1_count_, that.row2_count_, that.row3_count_, that.buffer_);
    return *this;
  }

  //: Comparison
  bool operator==(boxm2_array_3d<T> const& that) const {
    if (row1_count_ != that.row1_count_ ||
        row2_count_ != that.row2_count_ ||
        row3_count_ != that.row3_count_)
      return false;
    const_iterator i = this->begin();
    const_iterator j = that.begin();
    while (i != this->end())
    {
      if (!(*i == *j)) // do not assume we have operator!=(T)
        return false;
      ++i; ++j;
    }
    return true;
  }

  // Data Access---------------------------------------------------------------

  T      & operator() (size_type i1, size_type i2, size_type i3)
  {
    RANGECHECK(i1,i2,i3);
    return element_ [i1][i2][i3];
  }

  T const& operator() (size_type i1, size_type i2, size_type i3) const
  {
    RANGECHECK(i1,i2,i3);
    return element_ [i1][i2][i3];
  }

  T      * const* operator[](size_type i1) { return element_[i1]; }
  T const* const* operator[](size_type i1) const { return element_[i1]; }

  // dimensions
  size_type get_row1_count () const { return row1_count_; }
  size_type get_row2_count () const { return row2_count_; }
  size_type get_row3_count () const { return row3_count_; }

  // iterators
  size_type size() const
  {
    return row1_count_ * row2_count_ * row3_count_;
  }

  iterator begin() { return element_[0][0]; }
  iterator end  () { return begin() + size(); }
  const_iterator begin() const { return element_[0][0]; }
  const_iterator end  () const { return begin() + size(); }

  //: convert from 3-d indices to the linear index
  size_type linear_index(size_type i1, size_type i2, size_type i3) const{
    return (i1*row2_count_+i2)*row3_count_ + i3;
  }

  //: convert from the linear index to 3-d indices. Indices are stored
  // in-place in arguments.
  void coords_from_index(size_type idx,
                         size_type &i1,
                         size_type &i2,
                         size_type &i3) const {
    ldiv_t div = std::ldiv(idx, (row2_count_ * row3_count_));
    i1 = div.quot;
    div = std::div(div.rem, row3_count_);
    i2 = div.quot;
    i3 = div.rem;
  }

  // data_block will return all elements of the array in sequential storage.
  T      * data_block()       { return element_[0][0]; }
  T const* data_block() const { return element_[0][0]; }

  void resize(size_type n1, size_type n2, size_type n3); // no malloc unless size changes.
  void set(T const* array);
  void get(T* array) const;
  void fill(T const& value);

 protected:
  void construct(size_type, size_type, size_type, T* buffer);
  void destruct ();

 private:
  T *  buffer_;
  T ***element_;
  size_type row1_count_;
  size_type row2_count_;
  size_type row3_count_;
};

#undef RANGECHECK

//
// formatted I/O
//
template <class T> std::ostream& operator<<(std::ostream&,
                                                  boxm2_array_3d<T >const&);

template <class T> std::istream& operator>>(std::istream&,
                                                  boxm2_array_3d<T >&);

#define BOXM2_ARRAY_3D_INSTANTIATE \
extern "please include vbl/boxm2_array_3d.txx instead"
#define BOXM2_ARRAY_3D_IO_INSTANTIATE \
extern "please include vbl/boxm2_array_3d.txx instead"

#endif // boxm2_array_3d_h_
