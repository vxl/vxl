// This is core/vil1/vil1_crop_image_impl.h
#ifndef vil1_crop_image_h_
#define vil1_crop_image_h_
//:
// \file
// \brief A generic_image adaptor that behaves like a cropped version of its input
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <string>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A generic_image adaptor that behaves like a cropped version of its input
class vil1_crop_image_impl : public vil1_image_impl
{
 public:
  vil1_crop_image_impl(vil1_image const&, int x0, int y0, int w, int h);
  ~vil1_crop_image_impl() override;

  int planes() const override { return gi_.planes(); }
  int width() const override { return width_; }
  int height() const override { return height_; }
  int components() const override { return gi_.components(); }

  int bits_per_component() const override { return gi_.bits_per_component(); }
  enum vil1_component_format component_format() const override { return gi_.component_format(); }

  bool get_section(void* buf, int x0, int y0, int width, int height) const override {
    return gi_.get_section(buf, x0 + x0_, y0 + y0_, width, height);
  }
  bool put_section(void const* buf, int x0, int y0, int width, int height) override {
    return gi_.put_section(buf, x0 + x0_, y0 + y0_, width, height);
  }
  //  vil1_image get_plane(unsigned int p) const;

  //: Return the name of the class;
  std::string is_a() const override;

  //: Return true if the name of the class matches the argument
  bool is_class(std::string const&) const override;

 protected:
  vil1_image gi_;
  int x0_;
  int y0_;
  int width_;
  int height_;
};

#endif // vil1_crop_image_h_
