// This is core/vbl/vbl_bounding_box.hxx
#ifndef vbl_bounding_box_hxx_
#define vbl_bounding_box_hxx_

//-*- c++ -*-------------------------------------------------------------------
//

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 07 Aug 97
//
//-----------------------------------------------------------------------------

#include <iostream>
#include "vbl_bounding_box.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T, class DIM_>
std::ostream& vbl_bounding_box_base<T, DIM_>::print(std::ostream& s) const
{
  if (empty()) {
    s << "<bbox(empty, " << int(DIM_::value) << " dimensions)>";
  }
  else {
    s << "<bbox(";
    for (int i = 0; i < int(DIM_::value); ++i)
      s << min()[i] << ' ';
    s << ") to (";
    for (int i = 0; i < int(DIM_::value); ++i)
      s << max()[i] << ' ';
    s << ")>";
  }
  return s;
}

template <class T, class DIM_>
std::ostream& operator << (std::ostream& s, const vbl_bounding_box_base<T,DIM_>& bbox)
{
  return bbox.print(s);
}

#define VBL_BOUNDING_BOX_INSTANTIATE(T, DIM) \
template class vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > >; \
template class vbl_bounding_box<T , DIM >; \
template std::ostream& operator << (std::ostream&, vbl_bounding_box_base<T, vbl_bounding_box_DIM< DIM > > const&); \
/* template bool nested  (vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&); */ \
/* template bool disjoint(vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&); */ \
/* template bool meet    (vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&) */

#endif // vbl_bounding_box_hxx_
