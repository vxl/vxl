// This is mul/vil3d/vil3d_memory_image.h
#ifndef vil3d_memory_image_h_
#define vil3d_memory_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott

#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view_base.h>

//: Generic image implementation for PNM files
// You can't create one of these yourself - use vil3d_new() instead.
class vil3d_memory_image : public vil3d_image_resource
{
  //: Management of the memory image is devolved to an internal image_view.
  vil3d_image_view_base* view_;

  //: Create an empty memory image.
  vil3d_memory_image();

  //: Create an in-memory image of given size and pixel type.
  // The pixel type must be scalar.
  vil3d_memory_image(unsigned ni, unsigned nj,
                     unsigned nk, unsigned nplanes,
                     vil2_pixel_format format);

  friend vil3d_image_resource_sptr vil3d_new_image_resource(
    unsigned ni, unsigned nj, unsigned nk, unsigned nplanes,
    vil2_pixel_format format);

 public:


  ~vil3d_memory_image() {delete view_;}

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return view_->nplanes(); }
  virtual unsigned ni() const { return view_->ni(); }
  virtual unsigned nj() const { return view_->nj(); }
  virtual unsigned nk() const { return view_->nk(); }

  virtual enum vil2_pixel_format pixel_format() const { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil3d_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                              unsigned j0, unsigned nj,
                                              unsigned k0, unsigned nk) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil3d_image_view_base& im,
                        unsigned i0, unsigned j0, unsigned k0);

  bool get_property(char const * /*tag*/, void * /*prop*/ = 0) const {return false;}
};

#endif // vil3d_memory_image_h_
