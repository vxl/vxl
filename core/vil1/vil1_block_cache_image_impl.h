#ifndef vil_block_cache_image_impl_h_
#define vil_block_cache_image_impl_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_block_cache_image_impl
// .INCLUDE vil/vil_block_cache_image_impl.h
// .FILE vil_block_cache_image_impl.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

//: adaptor which caches the given image in blocks of given size.
class vil_block_cache_image_impl : public vil_image_impl {
public:
  vil_block_cache_image_impl(vil_image , unsigned blocksizex, unsigned blocksizey);
  ~vil_block_cache_image_impl();

  //:
  int planes() const { return base.planes(); }
  int width() const { return base.width(); }
  int height() const { return base.height(); }
  int components() const { return base.components(); }
  int bits_per_component() const { return base.bits_per_component(); }
  vil_component_format component_format() const { return base.component_format(); }
  
  vil_image get_plane(int ) const;

  bool get_section(void *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h); // write-through
  
  bool get_property(char const *tag, void *property_value_out = 0) const;
  
private:
  // the underlying, uncached image.
  vil_image base;

  // size of blocks
  unsigned block_size_x;
  unsigned block_size_y;
};

#endif // vil_block_cache_image_impl_h_
