//-*- c++ -*-------------------------------------------------------------------
//
// Class: vbl_bounding_box
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 07 Aug 97
//
//-----------------------------------------------------------------------------

#include "vbl_bounding_box.h"

#include <vcl_iostream.h>

template <class T, int DIM>
vcl_ostream& vbl_bounding_box<T, DIM>::print(vcl_ostream& s) const
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

// VC can't do the <<, luckily it inlines it.
#ifdef WIN32
# define vbl_bbox_inst_inline(x) /* template x */
#else
# define vbl_bbox_inst_inline(x) template x
#endif

#if defined(VCL_SUNPRO_CC_50)
# define vbl_bbox_inst_functions(T,DIM) /* */
#else
# define vbl_bbox_inst_functions(T,DIM) \
VCL_INSTANTIATE_INLINE(bool nested  (vbl_bounding_box<T,DIM> const &, vbl_bounding_box<T,DIM> const &)); \
VCL_INSTANTIATE_INLINE(bool disjoint(vbl_bounding_box<T,DIM> const &, vbl_bounding_box<T,DIM> const &)); \
VCL_INSTANTIATE_INLINE(bool meet    (vbl_bounding_box<T,DIM> const &, vbl_bounding_box<T,DIM> const &));
#endif

#define VBL_BOUNDING_BOX_INSTANTIATE(T,DIM) \
template class vbl_bounding_box<T , DIM >; \
vbl_bbox_inst_inline(vcl_ostream& operator << (vcl_ostream&, vbl_bounding_box<T,DIM> const &)); \
vbl_bbox_inst_functions(T,DIM)
