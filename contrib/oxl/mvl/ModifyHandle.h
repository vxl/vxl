#ifndef ModifyHandle_h_
#define ModifyHandle_h_
#ifdef __GNUC__
#pragma interface
#endif
//-----------------------------------------------------------------------------
//
// .NAME	ModifyHandle - Temporarily modify a value, restoring on end-of-block
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/ModifyHandle.h
// .FILE	ModifyHandle.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 03 Aug 96
//
// .SECTION Description
//    Take a pointer to some type, and when the ModifyHandle goes out
//    of scope, restore the old value.
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

template <class T>
class ModifyHandle {
  T  _oldvalue;
  T* _place;
public:
  ModifyHandle(T* place) { _place = place; _oldvalue = *place; }
  ~ModifyHandle() { *_place = _oldvalue; }
  operator T* () { return _place; }
  T& operator*() { return *_place; }
};

#endif // ModifyHandle_h_
