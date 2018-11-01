// This is core/vil/vil_memory_image.h
#ifndef vil_memory_image_h_
#define vil_memory_image_h_
//:
// \file
// \author Ian Scott
// \verbatim
//  Modifications
//   Peter Vanroose - 21 Aug.2003 - support added for _RGB_, _RGBA_ and _COMPLEX_ pixel_formats
// \endverbatim

#include <cstring>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_property.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Generic image implementation for PNM files
// You can't create one of these yourself - use vil_new_image_resource() instead.
class vil_memory_image : public vil_image_resource
{
  //: Management of the memory image is devolved to an internal image_view.
  vil_image_view_base* view_;

  //: Create an empty memory image.
  vil_memory_image();

  //: Create an in-memory image of given size and pixel type.
  // If not interleaved, pixel type must be scalar or nplanes must be 1.
  // If n_interleaved_planes is not 1, pixel type must be scalar,
  // and n_planes must be 1.
  vil_memory_image(unsigned ni,
                   unsigned nj,
                   unsigned nplanes,
                   vil_pixel_format format,
                   unsigned n_interleaved_planes = 1);

  //: Create a wrapper around the given image_view
  vil_memory_image(vil_image_view_base const &);


  friend vil_image_resource_sptr vil_new_image_resource(
    unsigned ni, unsigned nj, unsigned nplanes,
    vil_pixel_format format);

  friend vil_image_resource_sptr vil_new_image_resource_interleaved(
    unsigned ni, unsigned nj, unsigned nplanes,
    vil_pixel_format format);

  friend vil_image_resource_sptr vil_new_image_resource_of_view(
    vil_image_view_base const & view);

 public:

  ~vil_memory_image() override {delete view_;}

  //: Dimensions:  planes x width x height x components
  unsigned nplanes() const override { return view_->nplanes(); }
  unsigned ni() const override { return view_->ni(); }
  unsigned nj() const override { return view_->nj(); }

  enum vil_pixel_format pixel_format() const override { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const override;

  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override;

  //: Declare that this is an in-memory image which is not read-only
  bool get_property(char const * tag, void * prop = nullptr) const override
  {
    if (0==std::strcmp(tag, vil_property_memory))
      return prop ? (*static_cast<bool*>(prop)) = true : true;

    return false;
  }
};

#endif // vil_memory_image_h_
