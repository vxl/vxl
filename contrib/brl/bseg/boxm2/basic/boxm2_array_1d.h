#ifndef boxm2_array_1d_h_
#define boxm2_array_1d_h_
//:
// \file
#include <new>
#include <iosfwd>
#include <iostream>
#include <cstddef>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A simple wrapper for a buffer.
//  This class does not have ownership over data,
//  so this class does not delete or allocate any memory!
template <class T>
struct boxm2_array_1d
{
    typedef std::size_t size_type;
    typedef T element_type;

    typedef T       *iterator;
    typedef T const *const_iterator;

    typedef T       &reference;
    typedef T const &const_reference;

    //: empty constructor
    boxm2_array_1d() : begin_(nullptr), end_(nullptr), n_(0) {}

    //: create a wrapper for a buffer of size n
    boxm2_array_1d(size_type n, T* buffer) {
      begin_ = buffer;
      end_   = buffer + n;
      n_ = n;
    }

    //: Construct an array with n elements, all equal to v
    boxm2_array_1d(size_type n, T* buffer, const T &v) {
      // alignment guaranteed by 18.4.1.1
      begin_ = buffer;
      end_   = begin_ + n;
      n_     = n;
      for (size_type i=0; i<n; ++i)
        begin_[i] = v;
    }

    //: Assignment
    boxm2_array_1d<T>& operator=(boxm2_array_1d<T> const &that) {
      begin_ = that.begin_;
      end_   = that.begin_ + that.n_;
      n_     = that.n_;
      return *this;
    }

    //: pointer based iterators
    iterator begin()  { return begin_; }
    iterator end()    { return end_; }

    const_iterator begin() const  { return begin_; }
    const_iterator end() const    { return end_; }

    bool empty() const       { return begin_ == end_; }
    size_type size() const   { return end_ - begin_; }

    //: Get the ith element.
    // #define NDEBUG to turn bounds checking off.
    reference       operator[](std::ptrdiff_t i)
    {
      assert (i >= 0 && i < end_ - begin_);
      return begin_[i];
    }

    //: Get the ith element.
    // #define NDEBUG to turn bounds checking off.
    const_reference operator[](std::ptrdiff_t i) const
    {
      assert (i >= 0 && i < end_ - begin_);
      return begin_[i];
    }


  private:
    // begin_ <= end_ <= alloc_
    T *begin_, *end_;
    size_type n_;
};

template <class T>
std::ostream& operator<<(std::ostream &, boxm2_array_1d<T> const &);

#endif // boxm2_array_1d_h_
