// This is vxl/vil/vil_skip_image_impl.h
#ifndef vil_skip_image_h_
#define vil_skip_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Adaptor which produces a new image by skipping rows and columns
// \author fsm

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vcl_string.h>

//: Adaptor which produces a new image by skipping rows and columns
class vil_skip_image_impl : public vil_image_impl
{
 public:
  vil_skip_image_impl(vil_image const &underlying, unsigned sx, unsigned sy);
  ~vil_skip_image_impl();

  //: these inlines partly document the semantics of vil_skip_image.
  int planes() const { return base.planes(); }
  int width() const { return base.width() / skipx; }
  int height() const { return base.height() / skipy; }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil_component_format component_format() const { return base.component_format(); }

  vil_image get_plane(int ) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h); // <- will fail

  bool get_property(char const *tag, void *property_value_out = 0) const;


/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */

 private:
  vil_image base;
  unsigned skipx, skipy;
};

#endif // vil_skip_image_h_
