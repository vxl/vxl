// This is vxl/vil/vil_block_cache_image_impl.h
#ifndef vil_block_cache_image_impl_h_
#define vil_block_cache_image_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vcl_string.h>

//: vil_block_cache_image_impl - adaptor which caches the given image in blocks of given size.
class vil_block_cache_image_impl : public vil_image_impl
{
 public:

 //: Constructor with image and x and y bock-size
  vil_block_cache_image_impl(vil_image , unsigned blocksizex, unsigned blocksizey);

  //: Destructor
  ~vil_block_cache_image_impl();

  //: Number of image planes
  int planes() const { return base.planes(); }

  //: Image width
  int width() const { return base.width(); }

  //: Image height
  int height() const { return base.height(); }

  //: Number of measures per pixel
  int components() const { return base.components(); }

  //: Number of bits per component
  int bits_per_component() const { return base.bits_per_component(); }

  //: Component format
  vil_component_format component_format() const { return base.component_format(); }

  //: Access to image-plane
  vil_image get_plane(int ) const;

  //: Write nominated section of image to buf
  bool get_section(void *buf, int x0, int y0, int w, int h) const;

  //: Write buf to mininated section of image
  bool put_section(void const *buf, int x0, int y0, int w, int h); // write-through

  //: Get info about block-characteristics
  bool get_property(char const *tag, void *property_value_out = 0) const;


/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */

 private:
  // the underlying, uncached image.
  vil_image base;

  // size of blocks
  unsigned block_size_x;
  unsigned block_size_y;
};

// Helpers-------------------------------------------------------------------

#endif // vil_block_cache_image_impl_h_
