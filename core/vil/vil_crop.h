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
vil2_image_data_sptr vil2_crop(const vil2_image_data_sptr &src, unsigned i0,
                               unsigned j0, unsigned ni, unsigned nj);


//: A generic_image adaptor that behaves like a cropped version of its input
class vil2_crop_image_data : public vil2_image_data
{
 public:
  vil2_crop_image_data(vil2_image_data_sptr const&, int i0, int j0, int ni, int nj);
  ~vil2_crop_image_data();

  virtual unsigned nplanes() const { return gi_->nplanes(); }
  virtual unsigned ni() const { return ni_; }
  virtual unsigned nj() const { return nj_; }

  virtual enum vil2_pixel_format pixel_format() const { return gi_->pixel_format(); }


  virtual vil2_image_view_base* get_copy_view(unsigned i0, unsigned j0,
                                              unsigned ni, unsigned nj) const {
    return gi_->get_copy_view(i0+i0_, j0+j0_, ni, nj);
  }

  virtual vil2_image_view_base* get_view(unsigned i0, unsigned j0,
                                         unsigned ni, unsigned nj) const {
    return gi_->get_view(j0+j0_, j0+j0_, ni, nj);
  }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0,
                        unsigned j0) {
    return gi_->put_view(im, i0+i0_, j0+j0_);
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
  int i0_;
  int j0_;
  int ni_;
  int nj_;
};

#endif // vil2_crop_h_
