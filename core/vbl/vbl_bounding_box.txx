//-*- c++ -*-------------------------------------------------------------------
//
// Class: vbl_bounding_box
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 07 Aug 97
// Modifications:
//   970807 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vbl_bounding_box.h"

#include <vcl/vcl_iostream.h>

template <class T, int DIM>
ostream& vbl_bounding_box<T, DIM>::print(ostream& s) const
{
  s << "<bbox(";
  for(int i = 0; i < DIM; ++i)
    s << min_[i] << " ";
  s << ") to (";
  for(int i = 0; i < DIM; ++i)
    s << max_[i] << " ";
  s << ")>";
  return s;
}

#ifndef WIN32
#define VBL_BOUNDING_BOX_INSTANTIATE(T,DIM) \
template class vbl_bounding_box<T , DIM >; \
template ostream& operator << (ostream& s, const vbl_bounding_box<T,DIM>& bbox);
#else
// VC can't do the <<, luckily it inlines it.
#define VBL_BOUNDING_BOX_INSTANTIATE(T,DIM) \
template class vbl_bounding_box<T , DIM >; 
#endif
