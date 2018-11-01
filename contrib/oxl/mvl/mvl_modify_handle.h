// This is oxl/mvl/mvl_modify_handle.h
#ifndef mvl_modify_handle_h_
#define mvl_modify_handle_h_
//:
// \file
// \brief Temporarily modify a value, restoring on end-of-block
//
//    Take a pointer to some type, and when the mvl_modify_handle goes out
//    of scope, restore the old value.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 03 Aug 96
//
//-----------------------------------------------------------------------------

template <class T>
class mvl_modify_handle
{
  T  oldvalue_;
  T* place_;
 public:
  mvl_modify_handle(T* place) { place_ = place; oldvalue_ = *place; }
  ~mvl_modify_handle() { *place_ = oldvalue_; }
  operator T* () { return place_; }
  T& operator*() { return *place_; }
};

#endif // mvl_modify_handle_h_
