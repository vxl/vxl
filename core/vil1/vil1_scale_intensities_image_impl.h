#ifndef vil_scale_intensities_image_impl_h_
#define vil_scale_intensities_image_impl_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_scale_intensities_image_impl
// .INCLUDE vil/vil_scale_intensities_image_impl.h
// .FILE vil_scale_intensities_image_impl.cxx

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

class vil_scale_intensities_image_impl : public vil_image_impl {
public:
  vil_scale_intensities_image_impl(vil_image const& src, double scale, double shift) :
    base(src), scale_(scale), shift_(shift) { }

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
  double scale_;
  double shift_;
};

#endif // vil_scale_intensities_image_impl_h_
