// This is vxl/vbl/vbl_bounding_box.txx
#ifndef vbl_bounding_box_txx_
#define vbl_bounding_box_txx_

//-*- c++ -*-------------------------------------------------------------------
//

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 07 Aug 97
//
//-----------------------------------------------------------------------------

#include "vbl_bounding_box.h"

#include <vcl_compiler.h>
#include <vcl_iostream.h>

template <class T, class DIM_>
vcl_ostream& vbl_bounding_box_base<T, DIM_>::print(vcl_ostream& s) const
{
  s << "<bbox(";
  for(int i = 0; i < DIM_::value; ++i)
    s << min()[i] << " ";
  s << ") to (";
  for(int i = 0; i < DIM_::value; ++i)
    s << max()[i] << " ";
  s << ")>";
  return s;
}

// VC can't do the <<, luckily it inlines it.
#ifdef VCL_WIN32
# define vbl_bbox_inst_inline(x) /* template x */
#else
# define vbl_bbox_inst_inline(x) template x
#endif

#define vbl_bbox_inst_functions(T, DIM) \
VCL_INSTANTIATE_INLINE(bool nested  (vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &, \
                                     vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &)); \
VCL_INSTANTIATE_INLINE(bool disjoint(vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &, \
                                     vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &)); \
VCL_INSTANTIATE_INLINE(bool meet    (vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &, \
                                     vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> > const &))

#define VBL_BOUNDING_BOX_INSTANTIATE(T, DIM) \
template class vbl_bounding_box_base<T , vbl_bounding_box_DIM< DIM > >; \
template class vbl_bounding_box<T , DIM >; \
vbl_bbox_inst_inline(vcl_ostream& operator << (vcl_ostream&, vbl_bounding_box_base<T, vbl_bounding_box_DIM< DIM > > const &)); \
vbl_bbox_inst_functions(T,DIM)

#endif // vbl_bounding_box_txx_
