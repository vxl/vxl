// This is core/vil1/vil1_resample_image_impl.h
#ifndef vil1_resample_image_impl_h_
#define vil1_resample_image_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// This class is best accessed through the external function vil1_resample().

#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vcl_string.h>

//: Adaptor which produces an image by resampling.
// Note that the actual subsampling only takes place at the moment when the
// get_section() method is called.
class vil1_resample_image_impl : public vil1_image_impl
{
 public:
  vil1_resample_image_impl(vil1_image const &underlying, unsigned nw, unsigned nh);
  ~vil1_resample_image_impl() {}

  //: these inlines partly document the semantics of vil1_resample_image.
  int planes() const { return base.planes(); }
  int width() const { return new_width; }
  int height() const { return new_height; }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil1_component_format component_format() const { return base.component_format(); }

  vil1_image get_plane(unsigned int p) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h); // <- will fail

  bool get_property(char const *tag, void *property_value_out = 0) const;

  //: Return the name of the class
  virtual vcl_string is_a() const { return "vil1_resample_image_impl"; }

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const& s) const
  { return s==is_a() || vil1_image_impl::is_class(s); }

 private:
  vil1_image base;
  unsigned new_width, new_height;
};

#endif // vil1_resample_image_impl_h_
