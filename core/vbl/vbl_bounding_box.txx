// This is core/vbl/vbl_bounding_box.txx
#ifndef vbl_bounding_box_txx_
#define vbl_bounding_box_txx_

//-*- c++ -*-------------------------------------------------------------------
//

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 07 Aug 97
//
//-----------------------------------------------------------------------------

#include "vbl_bounding_box.h"

#include <vcl_iostream.h>

template <class T, class DIM_>
vcl_ostream& vbl_bounding_box_base<T, DIM_>::print(vcl_ostream& s) const
{
  if (empty()) {
    s << "<bbox(empty, " << DIM_::value << " dimensions)>";
  }
  else {
    s << "<bbox(";
    for (int i = 0; i < DIM_::value; ++i)
      s << min()[i] << " ";
    s << ") to (";
    for (int i = 0; i < DIM_::value; ++i)
      s << max()[i] << " ";
    s << ")>";
  }
  return s;
}

template <class T, class DIM_>
vcl_ostream& operator << (vcl_ostream& s, const vbl_bounding_box_base<T,DIM_>& bbox)
{
  return bbox.print(s);
}

#define VBL_BOUNDING_BOX_INSTANTIATE(T, DIM) \
template class vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > >; \
template class vbl_bounding_box<T , DIM >; \
template vcl_ostream& operator << (vcl_ostream&, vbl_bounding_box_base<T, vbl_bounding_box_DIM< DIM > > const&); \
VCL_INSTANTIATE_INLINE(bool nested  (vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&)); \
VCL_INSTANTIATE_INLINE(bool disjoint(vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&)); \
VCL_INSTANTIATE_INLINE(bool meet    (vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&, \
                                     vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > > const&))

#endif // vbl_bounding_box_txx_
