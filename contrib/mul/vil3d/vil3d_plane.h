// This is mul/vil3d/vil3d_plane.h
#ifndef vil3d_plane_h_
#define vil3d_plane_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes.

#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>


//: Return a view of im's plane p.
//  O(1).
// \relates vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_plane(const vil3d_image_view<T> &im, unsigned p)
{
  assert(p<im.nplanes());
  return vil3d_image_view<T>(im.memory_chunk(),
                             im.origin_ptr()+p*im.planestep(),
                             im.ni(),im.nj(),im.nk(),1,
                             im.istep(),im.jstep(),im.kstep(),im.planestep());
}


//: Return a view of a selection of im's planes.
// You can select any equally-spaced sequence of planes.
// \param first The index of the first plane you want to select.
// \param skip The spacing in your selection - will be 1 for adjacent planes.
// \param n The total number of planes in your selection.
//  O(1).
template<class T>
inline vil3d_image_view<T> vil3d_planes(const vil3d_image_view<T> &im,
                                        unsigned first, int skip,
                                        unsigned n)
{
  assert(first<im.nplanes());
  int end = first + n*skip;
  assert(end >= 0);
  assert((unsigned)end <= im.nplanes());
  return vil3d_image_view<T>(im.memory_chunk(),
    im.origin_ptr()+first*im.planestep(), im.ni(), im.nj(), im.nk(), n,
    im.istep(),im.jstep(),im.kstep(), skip*im.planestep());
}


#endif // vil3d_plane_h_
