#ifndef vbl_array_h_
#define vbl_array_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME	vbl_array
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_array.h
// .FILE	vbl_array.txx
// .SECTION Description
// a container which stores its elements in contiguous
// storage and whose iterator types are raw pointers. There is
// no requirement that the element type have a default constructor.
// .SECTION Author
//  fsm@robots.ox.ac.uk

#include <vcl/vcl_new.h>

template <class T>
struct vbl_array
{
  typedef T element_type;
  
  typedef T       *iterator;
  typedef T const *const_iterator;

  typedef T       &reference;
  typedef T const &const_reference;

  vbl_array() : begin_(0), end_(0), alloc_(0) { }

  vbl_array(const_iterator b, const_iterator e) {
    unsigned n = e - b;
    begin_ = (T*) new char [n * sizeof(T)]; // FIXME alignment
    end_   = begin_ + n;
    alloc_ = begin_ + n;
    for (unsigned i=0; i<n; ++i)
      new (begin_ + i) T(b[i]);
  }

  vbl_array(vbl_array<T> const &that) {
    new (this) vbl_array<T>(that.begin_, that.end_);
  }

  vbl_array<T> &operator=(vbl_array<T> const &that) {
    this->~vbl_array();
    new (this) vbl_array<T>(that.begin_, that.end_);
    return *this;
  }

  ~vbl_array() {
    if (begin_) {
      clear();
      delete [] (char*) begin_; 
    } 
  }
  
  void reserve(unsigned new_n) {
    unsigned n = end_ - begin_;
    if (new_n <= n) 
      return;
    
    T *new_begin_ = (T*) new char [new_n * sizeof(T)]; // FIXME alignment
    T *new_end_   = new_begin_ + n;
    T *new_alloc_ = new_begin_ + new_n;
    
    for (unsigned i=0; i<n; ++i) {
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
  unsigned size() const { return end_ - begin_; }
  unsigned capacity() const { return alloc_ - begin_; }

  reference       operator[](unsigned i) { return begin_[i]; }
  const_reference operator[](unsigned i) const { return begin_[i]; }
  
private:
  // begin_ <= end_ <= alloc_
  T *begin_, *end_, *alloc_;
};

#endif // vbl_array_h_
