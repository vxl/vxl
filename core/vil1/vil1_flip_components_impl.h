// This is vxl/vil/vil_flip_components_impl.h
#ifndef vil_flip_components_impl_h_
#define vil_flip_components_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be

#include <vil/vil_image.h>
#include <vcl_string.h>

//: A view of a B,G,R image as if it were R,G,B (or the other way around)
class vil_flip_components_impl : public vil_image_impl
{
 public:
  vil_flip_components_impl(vil_image const &base_) : base(base_) { }

  int planes() const { return base.planes(); }
  int width() const { return base.width(); }
  int height() const { return base.height(); }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil_component_format component_format() const { return base.component_format(); }

  vil_image get_plane(int ) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);

  //: Return the name of the class
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 private:
  vil_image base;
};

#endif // vil_flip_components_impl_h_
