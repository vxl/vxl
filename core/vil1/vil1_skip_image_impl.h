#ifndef vil_skip_image_h_
#define vil_skip_image_h_
#ifdef __GNUC__
#pragma interface "vil_skip_image_impl"
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

//: Adaptor which produces a new image by skipping rows and columns.
class vil_skip_image_impl : public vil_image_impl {
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

private:
  vil_image base;
  unsigned skipx, skipy;
};

#endif
