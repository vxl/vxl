// This is mul/vil2/vil2_memory_image.h
#ifndef vil2_memory_image_h_
#define vil2_memory_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott


#include <vil2/vil2_image_data.h>
#include <vil2/vil2_image_view_base.h>

//: Generic image implementation for PNM files
class vil2_memory_image : public vil2_image_data
{
  //: Management of the memory image is devolved to an internal image_view.
  vil2_image_view_base_sptr view_;
 public:

  vil2_memory_image();

  vil2_memory_image(unsigned ni,
                 unsigned nj,
                 unsigned nplanes,
                 vil2_pixel_format format);

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return view_->nplanes(); }
  virtual unsigned ni() const { return view_->ni(); }
  virtual unsigned nj() const { return view_->nj(); }

  virtual enum vil2_pixel_format pixel_format() const { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base* get_copy_view(unsigned x0, unsigned y0,
                                              unsigned nx, unsigned ny) const;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base* get_view(unsigned x0, unsigned y0,
                                         unsigned nx, unsigned ny) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned x0, unsigned y0);

  bool get_property(char const *tag, void *prop = 0) const {return false;}
};

#endif // vil2_memory_image_h_
