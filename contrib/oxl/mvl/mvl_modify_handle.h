#ifndef mvl_modify_handle_h_
#define mvl_modify_handle_h_
#ifdef __GNUC__
#pragma interface
#endif
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
  T  _oldvalue;
  T* _place;
public:
  mvl_modify_handle(T* place) { _place = place; _oldvalue = *place; }
  ~mvl_modify_handle() { *_place = _oldvalue; }
  operator T* () { return _place; }
  T& operator*() { return *_place; }
};

#endif // mvl_modify_handle_h_
