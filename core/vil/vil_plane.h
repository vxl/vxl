// This is core/vil/vil_plane.h
#ifndef vil_plane_h_
#define vil_plane_h_
//:
// \file
// \author Ian Scott.

#include "vil_image_resource.h"
#include "vil_image_view.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Return a view of im's plane p.
//  O(1).
// \relatesalso vil_image_view
template <class T>
inline vil_image_view<T>
vil_plane(const vil_image_view<T> & im, unsigned p)
{
  assert(p < im.nplanes());
  return vil_image_view<T>(im.memory_chunk(),
                           im.top_left_ptr() + p * im.planestep(),
                           im.ni(),
                           im.nj(),
                           1,
                           im.istep(),
                           im.jstep(),
                           im.planestep());
}

//: Return a view of a selection of im's planes.
// You can select any equally-spaced sequence of planes.
// \param first The index of the first plane you want to select.
// \param skip The spacing in your selection - will be 1 for adjacent planes.
// \param n The total number of planes in your selection.
//
//  O(1).
template <class T>
inline vil_image_view<T>
vil_planes(const vil_image_view<T> & im, unsigned first, int skip, unsigned n)
{
  assert(first < im.nplanes());
  assert(int(first) + int(n) * skip >= 0);
  assert((unsigned)(first + n * skip) <= im.nplanes());
  return vil_image_view<T>(im.memory_chunk(),
                           im.top_left_ptr() + first * im.planestep(),
                           im.ni(),
                           im.nj(),
                           n,
                           im.istep(),
                           im.jstep(),
                           skip * im.planestep());
}


//: Return a specific plane of an image.
// \relatesalso vil_image_resource
vil_image_resource_sptr
vil_plane(const vil_image_resource_sptr & src, unsigned p);


//: A generic_image adaptor that behaves like a single plane version of its input
// For implementation use only - use vil_plane() to create one.
class vil_plane_image_resource : public vil_image_resource
{
  vil_plane_image_resource(const vil_image_resource_sptr &, unsigned p);
  friend vil_image_resource_sptr
  vil_plane(const vil_image_resource_sptr & src, unsigned p);

public:
  unsigned
  nplanes() const override
  {
    return 1;
  }
  unsigned
  ni() const override
  {
    return src_->ni();
  }
  unsigned
  nj() const override
  {
    return src_->nj();
  }

  enum vil_pixel_format
  pixel_format() const override
  {
    return src_->pixel_format();
  }

  vil_image_view_base_sptr
  get_copy_view(unsigned i0, unsigned ni, unsigned j0, unsigned nj) const override;

  vil_image_view_base_sptr
  get_view(unsigned i0, unsigned ni, unsigned j0, unsigned nj) const override;


  //: Put the data in this view back into the image source.
  bool
  put_view(const vil_image_view_base & im, unsigned i0, unsigned j0) override;

  //: Extra property information
  bool
  get_property(const char * tag, void * property_value = nullptr) const override
  {
    return src_->get_property(tag, property_value);
  }

protected:
  //: Reference to underlying image source
  vil_image_resource_sptr src_;
  //: which plane.
  unsigned plane_;
};

#endif // vil_plane_h_
