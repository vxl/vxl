// This is core/vil1/vil1_resample_image_impl.h
#ifndef vil1_resample_image_impl_h_
#define vil1_resample_image_impl_h_
//:
// \file
// \author fsm
// This class is best accessed through the external function vil1_resample().

#include <string>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Adaptor which produces an image by resampling.
// Note that the actual subsampling only takes place at the moment when the
// get_section() method is called.
class vil1_resample_image_impl : public vil1_image_impl
{
 public:
  vil1_resample_image_impl(vil1_image const &underlying, unsigned nw, unsigned nh);
  ~vil1_resample_image_impl() override = default;

  //: these inlines partly document the semantics of vil1_resample_image.
  int planes() const override { return base.planes(); }
  int width() const override { return new_width; }
  int height() const override { return new_height; }
  int components() const override { return base.components(); }
  int bits_per_component() const override { return base.bits_per_component(); }
  vil1_component_format component_format() const override { return base.component_format(); }

  vil1_image get_plane(unsigned int p) const override;

  bool get_section(void *buf, int x0, int y0, int w, int h) const override;
  bool put_section(void const *buf, int x0, int y0, int w, int h) override; // <- will fail

  bool get_property(char const *tag, void *property_value_out = nullptr) const override;

  //: Return the name of the class
  std::string is_a() const override { return "vil1_resample_image_impl"; }

  //: Return true if the name of the class matches the argument
  bool is_class(std::string const& s) const override
  { return s==is_a() || vil1_image_impl::is_class(s); }

 private:
  vil1_image base;
  unsigned new_width, new_height;
};

#endif // vil1_resample_image_impl_h_
