#ifndef vil_clamp_image_impl_h_
#define vil_clamp_image_impl_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_clamp_image_impl
// .INCLUDE vil/vil_clamp_image_impl.h
// .FILE vil_clamp_image_impl.cxx

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

class vil_clamp_image_impl : public vil_image_impl {
public:
  vil_clamp_image_impl(vil_image const& src, double low, double high) :
    base(src), low_(low), high_(high) { }

  int planes() const { return base.planes(); }
  int width() const { return base.width(); }
  int height() const { return base.height(); }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil_component_format component_format() const { return base.component_format(); }
  
  vil_image get_plane(int ) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);

private:
  vil_image base;
  double low_;
  double high_;
};

#endif // vil_clamp_image_impl_h_
