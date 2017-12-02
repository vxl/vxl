// This is core/vil1/vil1_flipud_impl.h
#ifndef vil1_flipud_impl_h_
#define vil1_flipud_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <string>
#include <vil1/vil1_image.h>
#include <vcl_compiler.h>

//: A view of an image which is flipped up/down (ie top row becomes bottom)
class vil1_flipud_impl : public vil1_image_impl
{
 public:
  vil1_flipud_impl(vil1_image const &base_) : base(base_) { }

  int planes() const { return base.planes(); }
  int width() const { return base.width(); }
  int height() const { return base.height(); }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil1_component_format component_format() const { return base.component_format(); }

  vil1_image get_plane(unsigned int p) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);

  //: Return the name of the class;
  virtual std::string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(std::string const&) const;

 private:
  vil1_image base;
};

#endif // vil1_flipud_impl_h_
