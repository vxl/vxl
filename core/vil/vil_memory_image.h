// This is core/vil/vil_memory_image.h
#ifndef vil_memory_image_h_
#define vil_memory_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott
// \verbatim
//  Modifications
//   Peter Vanroose - 21 Aug.2003 - support added for _RGB_, _RGBA_ and _COMPLEX_ pixel_formats
// \endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_property.h>
#include <vcl_cstring.h>

//: Generic image implementation for PNM files
// You can't create one of these yourself - use vil_new_image_resource() instead.
class vil_memory_image : public vil_image_resource
{
  //: Management of the memory image is devolved to an internal image_view.
  vil_image_view_base* view_;

  //: Create an empty memory image.
  vil_memory_image();

  //: Create an in-memory image of given size and pixel type.
  // The pixel type must be scalar.
  vil_memory_image(unsigned ni,
                   unsigned nj,
                   unsigned nplanes,
                   vil_pixel_format format);

  //: Create a wrapper around the given image_view
  vil_memory_image(vil_image_view_base const &);


  friend vil_image_resource_sptr vil_new_image_resource(
    unsigned ni, unsigned nj, unsigned nplanes,
    vil_pixel_format format);

  friend vil_image_resource_sptr vil_new_image_resource_of_view(
    vil_image_view_base const & view);

 public:

  ~vil_memory_image() {delete view_;}

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return view_->nplanes(); }
  virtual unsigned ni() const { return view_->ni(); }
  virtual unsigned nj() const { return view_->nj(); }

  virtual enum vil_pixel_format pixel_format() const { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Declare that this is an in-memory image which is not read-only
  bool get_property(char const * tag, void * prop = 0) const
  {
    if (0==vcl_strcmp(tag, vil_property_memory))
      return prop ? (*static_cast<bool*>(prop)) = true : true;

    return false;
  }
};

#endif // vil_memory_image_h_
