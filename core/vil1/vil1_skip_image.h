#ifndef vil_skip_image_h_
#define vil_skip_image_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_generic_image.h>
#include <vil/vil_image_ref.h>

//: Adaptor which produces a new image by skipping rows and columns.
class vil_skip_image : public vil_generic_image {
public:
  vil_skip_image(vil_generic_image *underlying, unsigned sx, unsigned sy);
  ~vil_skip_image();

  //: these inlines partly document the semantics of vil_skip_image.
  int planes() const { return base->planes(); }
  int width() const { return base->width() / skipx; }
  int height() const { return base->height() / skipy; }
  int components() const { return base->components(); }
  int bits_per_component() const { return base->bits_per_component(); }
  vil_component_format component_format() const { return base->component_format(); }

  vil_generic_image *get_plane(int ) const;

  bool do_get_section(void *buf, int x0, int y0, int width, int height) const;
  bool do_put_section(void const *buf, int x0, int y0, int width, int height); // <- will fail

  bool get_property(char const *tag, void *property_value_out = 0) const;

private:
  vil_image_ref base;
  unsigned skipx, skipy;
};

#endif
