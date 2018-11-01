// This is mul/vil3d/vil3d_memory_image.h
#ifndef vil3d_memory_image_h_
#define vil3d_memory_image_h_
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
                     vil_pixel_format format);

  friend vil3d_image_resource_sptr vil3d_new_image_resource(
    unsigned ni, unsigned nj, unsigned nk, unsigned nplanes,
    vil_pixel_format format);

 public:


  ~vil3d_memory_image() override {delete view_;}

  //: Dimensions:  planes x width x height x components
  unsigned nplanes() const override { return view_->nplanes(); }
  unsigned ni() const override { return view_->ni(); }
  unsigned nj() const override { return view_->nj(); }
  unsigned nk() const override { return view_->nk(); }

  enum vil_pixel_format pixel_format() const override { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const override;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  vil3d_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                              unsigned j0, unsigned nj,
                                              unsigned k0, unsigned nk) const override;

  //: Put the data in this view back into the image source.
  bool put_view(const vil3d_image_view_base& im,
                        unsigned i0, unsigned j0, unsigned k0) override;

  bool get_property(char const * /*tag*/, void * /*prop*/ = nullptr) const override {return false;}
};

#endif // vil3d_memory_image_h_
