// This is vxl/vil/vil_flipud_impl.h
#ifndef vil_flipud_impl_h_
#define vil_flipud_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vil/vil_image.h>
#include <vcl_string.h>

//: A view of an image which is flipped up/down (ie top row becomes bottom)
class vil_flipud_impl : public vil_image_impl
{
 public:
  vil_flipud_impl(vil_image const &base_) : base(base_) { }

  int planes() const { return base.planes(); }
  int width() const { return base.width(); }
  int height() const { return base.height(); }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil_component_format component_format() const { return base.component_format(); }

  vil_image get_plane(int ) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);


/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */

 private:
  vil_image base;
};

#endif // vil_flipud_impl_h_
