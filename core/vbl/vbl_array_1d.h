// This is vxl/vbl/vbl_array_1d.h
#ifndef vbl_array_1d_h_
#define vbl_array_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A simple container.
// \author fsm
//
// \verbatim
// Modifications
//    Peter Vanroose 5apr2001 added operator==
// \endverbatim

#include <vcl_new.h>
#include <vcl_cassert.h>

//: A simple container.
// This container stores its elements in contiguous
// storage and whose iterator types are raw pointers. There is
// no requirement that the element type have a default constructor.
template <class T>
struct vbl_array_1d
{
  typedef T element_type;

  typedef T       *iterator;
  typedef T const *const_iterator;

  typedef T       &reference;
  typedef T const &const_reference;

  vbl_array_1d() : begin_(0), end_(0), alloc_(0) { }

  vbl_array_1d(const_iterator b, const_iterator e) {
    unsigned long n = e - b;
    begin_ = (T*) new char [n * sizeof(T)]; // FIXME alignment
    end_   = begin_ + n;
    alloc_ = begin_ + n;
    for (unsigned long i=0; i<n; ++i)
      new (begin_ + i) T(b[i]);
  }

  vbl_array_1d(vbl_array_1d<T> const &that) {
    new (this) vbl_array_1d<T>(that.begin_, that.end_);
  }

//: Construct an array with n elements, all equal to v
  vbl_array_1d(unsigned n, const T &v) {
    begin_ = (T*) new char [n * sizeof(T)]; // FIXME alignment
    end_   = begin_ + n;
    alloc_ = begin_ + n;
    for (unsigned i=0; i<n; ++i)
      new (begin_ + i) T(v);
  }


  vbl_array_1d<T> &operator=(vbl_array_1d<T> const &that) {
    this->~vbl_array_1d();
    new (this) vbl_array_1d<T>(that.begin_, that.end_);
    return *this;
  }

  bool operator==(vbl_array_1d<T> const& that) const {
    T* i = begin_;
    T* j = that.begin_;
    for ( ; i!=end_ && j!=that.end_; ++i, ++j)
      if (!(*i == *j)) return false;
    return i == end_ && j == that.end_;
  }

  ~vbl_array_1d() {
    if (begin_) {
      clear();
      delete [] (char*) begin_;
    }
  }

  void reserve(unsigned new_n) {
    unsigned long n = end_ - begin_;
    if (new_n <= n)
      return;

    T *new_begin_ = (T*) new char [new_n * sizeof(T)]; // FIXME alignment
    T *new_end_   = new_begin_ + n;
    T *new_alloc_ = new_begin_ + new_n;

    for (unsigned long i=0; i<n; ++i) {
      new (new_begin_ + i) T(begin_[i]);
      begin_[i].~T();
    }

    delete [] (char*) begin_;

    begin_ = new_begin_;
    end_   = new_end_;
    alloc_ = new_alloc_;
  }

  void push_back(T const &x) {
    if (end_ == alloc_)
      reserve(2*size() + 1);
    new (end_) T(x);
    ++end_;
  }

  void pop_back() {
    end_->~T();
    --end_;
  }

  reference back() { return end_[-1]; }
  const_reference back() const { return end_[-1]; }

  reference front() { return *begin_; }
  const_reference front() const { return *begin_; }

  void clear() {
    for (T *p = begin_; p!=end_; ++p)
      p->~T();
    end_ = begin_;
  }

  iterator begin() { return begin_; }
  iterator end() { return end_; }

  const_iterator begin() const { return begin_; }
  const_iterator end() const { return end_; }

  bool empty() const { return begin_ == end_; }
  unsigned long size() const { return end_ - begin_; }
  unsigned long capacity() const { return alloc_ - begin_; }

  //: Get the ith element.
  // #define NDEBUG to turn bounds checking off.
  reference       operator[](unsigned i)
  {
    assert (int(i) < end_ - begin_);
    return begin_[i];
  }

  //: Get the ith element.
  // #define NDEBUG to turn bounds checking off.
  const_reference operator[](unsigned i) const
  {
    assert (int(i) < end_ - begin_);
    return begin_[i];
  }

 private:
  // begin_ <= end_ <= alloc_
  T *begin_, *end_, *alloc_;
};

#endif // vbl_array_1d_h_
