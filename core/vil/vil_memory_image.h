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
  vil2_image_view_base * view_;
 public:

  vil2_memory_image();

  vil2_memory_image(unsigned nx,
                 unsigned ny,
                 unsigned nplanes,
                 vil2_pixel_format format);
  ~vil2_memory_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return view_->nplanes(); }
  virtual unsigned nx() const { return view_->nx(); }
  virtual unsigned ny() const { return view_->ny(); }

  virtual enum vil2_pixel_format pixel_format() const { return view_->pixel_format(); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base* get_copy_view(unsigned x0, unsigned y0, unsigned plane0,
                                              unsigned nx, unsigned ny, unsigned nplanes) const;

  //: Create a view of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base* get_view(unsigned x0, unsigned y0, unsigned plane0,
                                         unsigned nx, unsigned ny, unsigned nplanes) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned x0, unsigned y0, unsigned plane0 = 0);

  bool get_property(char const *tag, void *prop = 0) const {return false;}
};

#endif // vil2_memory_image_h_
