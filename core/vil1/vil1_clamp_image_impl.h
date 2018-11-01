// This is core/vil1/vil1_clamp_image_impl.h
#ifndef vil1_clamp_image_impl_h_
#define vil1_clamp_image_impl_h_
//:
// \file

#include <string>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: View of image clamped to a given range
class vil1_clamp_image_impl : public vil1_image_impl
{
 public:
  vil1_clamp_image_impl(vil1_image const& src, double low, double high)
    : base(src), low_(low), high_(high) {}

  int planes() const override { return base.planes(); }
  int width() const override { return base.width(); }
  int height() const override { return base.height(); }
  int components() const override { return base.components(); }
  int bits_per_component() const override { return base.bits_per_component(); }
  vil1_component_format component_format() const override { return base.component_format(); }

  vil1_image get_plane(unsigned int p) const override;

  bool get_section(void *buf, int x0, int y0, int w, int h) const override;
  bool put_section(void const *buf, int x0, int y0, int w, int h) override;

  //: Return the name of the class;
  std::string is_a() const override { return "vil1_clamp_image_impl"; }

  //: Return true if the name of the class matches the argument
  bool is_class(std::string const& s) const override
  { return s==is_a() || vil1_image_impl::is_class(s); }

 private:
  vil1_image base;
  double low_;
  double high_;
};

#endif // vil1_clamp_image_impl_h_
