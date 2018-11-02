#ifndef mbl_priority_bounded_queue_h_
#define mbl_priority_bounded_queue_h_
//:
// \file
// \brief Describes a bounded priority queue
// \author Ian Scott
// \date   Fri Oct  5  2001

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A bounded priority queue
// This is identical to a std::priority_queue, but
// as more elements are added past the queue's bound size
// the largest values are thrown out.
// So this queue keeps the n smallest values that
// are passed to it.
//
// To store the largest values, use vcl_more as the comparator O.
//
// top() returns the value that is closest to being thrown out,
// which is the largest value in the case of the default predicate.
template <class T, class C= std::vector<T>, class O= std::less<
#ifndef _MSC_VER
typename
#endif
  C::value_type> >
class mbl_priority_bounded_queue
{
public:
  typedef typename C::value_type value_type;
  typedef typename C::size_type size_type;
  typedef typename C::allocator_type allocator_type;

  explicit
  mbl_priority_bounded_queue(unsigned bound_size = 10, const O& comp = O()):
    b_size_(bound_size), comp_(comp) { }

  typedef const value_type *ITER;
  //: Construct a bounded priority queue from a controlled sequence.
  // The bounded size will be the length of the sequence.
  mbl_priority_bounded_queue(
    size_type bound_size, ITER first, ITER last, const O& comp = O(),
    const allocator_type& alloc = allocator_type()):
      b_size_(0), c_(alloc), comp_(comp)
    {for (; first != last; ++first) {++b_size_; push(*first);} }

  //: The largest size the queue can be before it starts throwing out data.
  size_type bound_size() const {return b_size_;}

  //: Set the largest size the queue can be before it starts throwing out data.
  // If the bound_size is smaller that the current size, then data will be thrown out.
  void set_bound_size(size_type bound_size) {
    while (bound_size > size()) pop();
    b_size_ = bound_size; }

  bool empty() const {return c_.empty(); }

  size_type size() const {return c_.size(); }

  value_type& top() {return c_.front(); }

  const value_type& top() const {return c_.front(); }

  void push(const value_type & x) {
    if (size() >= b_size_)
    {
      if ( comp_(x, top()) )
        pop();
      else return;
    }
    c_.push_back(x);
    std::push_heap(c_.begin(), c_.end(), comp_); } // ignore purify:UMR error here
  // It can be resolved by replacing a comparator object with a function pointer.
  // It seems that when using an object, some compilers put a small data marker in
  // to represent the object. But it contains no useful data.
  // Ignore purify:UMR error on next line as well - same cause.
  void pop() {std::pop_heap(c_.begin(), c_.end(), comp_); c_.pop_back(); }

protected:
  size_type b_size_;
  C c_;
  O comp_;
};

#endif // mbl_priority_bounded_queue_h_
