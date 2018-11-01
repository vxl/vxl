// This is core/vbl/vbl_array_1d.h
#ifndef vbl_array_1d_h_
#define vbl_array_1d_h_
//:
// \file
// \brief A simple container.
// \author fsm
//
// \verbatim
// Modifications
//    Peter Vanroose 5apr2001 added operator==
//    Amitha Perera  jan2003  fixed possible alignment issues.
// \endverbatim

#include <new>
#include <iosfwd>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: A simple container.
// This container stores its elements in contiguous
// storage and whose iterator types are raw pointers. There is
// no requirement that the element type have a default constructor.
template <class T>
class vbl_array_1d
{
 public:
  typedef std::size_t size_type;
  typedef T element_type;

 private:
  element_type *begin_, *end_, *alloc_; // begin_ <= end_ <= alloc_

 public:
  typedef T       *iterator;
  typedef T const *const_iterator;

  typedef T       &reference;
  typedef T const &const_reference;
 public:

  vbl_array_1d() : begin_(nullptr), end_(nullptr), alloc_(nullptr) { }

  vbl_array_1d(const_iterator b, const_iterator e) {
    std::ptrdiff_t n = e - b;
    assert(n>=0);
    // alignment guaranteed by 18.4.1.1
    begin_ = (T*) operator new( n * sizeof(T) );
    end_   = begin_ + n;
    alloc_ = begin_ + n;
    for (std::ptrdiff_t i=0; i<n; ++i)
      new (begin_ + i) T(b[i]);
  }

  vbl_array_1d(vbl_array_1d<T> const &that) {
    new (this) vbl_array_1d<T>(that.begin_, that.end_);
  }

//: Construct an array with n elements, all equal to v
  vbl_array_1d(size_type n, const T &v) {
    // alignment guaranteed by 18.4.1.1
    begin_ = (T*) operator new( n * sizeof(T) );
    end_   = begin_ + n;
    alloc_ = begin_ + n;
    for (size_type i=0; i<n; ++i)
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
      operator delete( begin_ );
    }
  }

  void reserve(std::ptrdiff_t new_n) {
    std::ptrdiff_t n = end_ - begin_;
    assert(n>=0);
    if (new_n <= n)
      return;

    // alignment guaranteed by 18.4.1.1
    T *new_begin_ = (T*) operator new( new_n * sizeof(T) );
    T *new_end_   = new_begin_ + n;
    T *new_alloc_ = new_begin_ + new_n;

    for (std::ptrdiff_t i=0; i<n; ++i) {
      new (new_begin_ + i) T(begin_[i]);
      begin_[i].~T();
    }

    operator delete( begin_ );

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
  size_type size() const { return end_ - begin_; }
  size_type capacity() const { return alloc_ - begin_; }

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
};

template <class T>
std::ostream& operator<<(std::ostream &, vbl_array_1d<T> const &);

#endif // vbl_array_1d_h_
