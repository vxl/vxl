// This is mul/vil2/vil2_crop.h
#ifndef vil2_crop_h_
#define vil2_crop_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil2/vil2_image_data.h>

//: Crop to a region of src.
vil2_image_data_sptr vil2_crop(const vil2_image_data_sptr &src, unsigned x0,
                               unsigned y0, unsigned nx, unsigned ny);


//: A generic_image adaptor that behaves like a cropped version of its input
class vil2_crop_image_data : public vil2_image_data
{
 public:
  vil2_crop_image_data(vil2_image_data_sptr const&, int x0, int y0, int nx, int ny);
  ~vil2_crop_image_data();

  virtual unsigned nplanes() const { return gi_->nplanes(); }
  virtual unsigned nx() const { return nx_; }
  virtual unsigned ny() const { return ny_; }

  virtual unsigned bits_per_component() const { return gi_->bits_per_component(); }
  virtual enum vil_component_format component_format() const { return gi_->component_format(); }


  virtual vil2_image_view_base* get_copy_view(unsigned x0, unsigned y0,
                                              unsigned plane0, unsigned nx, unsigned ny,
                                              unsigned nplanes) const {
    return gi_->get_copy_view(x0 + x0_, y0+y0_, plane0, nx, ny, nplanes);
  }

  virtual vil2_image_view_base* get_view(unsigned x0, unsigned y0,
                                         unsigned plane0, unsigned nx, unsigned ny,
                                         unsigned nplanes) const {
    return gi_->get_view(x0 + x0_, y0+y0_, plane0, nx, ny, nplanes);
  }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned x0,
                        unsigned y0, unsigned plane0 = 0) {
    return gi_->put_view(im, x0 + x0_, y0 + y0_, plane0);
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
    return gi_->get_property(tag, property_value);
  }

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 protected:
  vil2_image_data_sptr gi_;
  int x0_;
  int y0_;
  int nx_;
  int ny_;
};

#endif // vil22_crop_h_
