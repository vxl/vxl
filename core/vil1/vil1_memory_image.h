// This is vxl/vil/vil_memory_image.h
#ifndef vil_memory_image_h_
#define vil_memory_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief vil_image, stored entirely in RAM
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
//  Modifications
//     010126 BJM (mccane@cs.otago.ac.nz) added constructor from
//            previously allocated memory. This memory is not deallocated on
//            destruction.
//\endverbatim

#include <vil/vil_pixel.h>
#include <vil/vil_image.h>

//: Description of image format in memory
struct vil_memory_image_format
{
  int components;
  int bits_per_component;
  vil_component_format component_format;
};

//: vil_image, stored entirely in memory
class vil_memory_image : public vil_image
{
 public:
  vil_memory_image();
  vil_memory_image(int planes, int w, int h, vil_memory_image_format const&);
  vil_memory_image(int planes, int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(int planes, int w, int h, vil_pixel_format_t);
  vil_memory_image(int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(int w, int h, vil_pixel_format_t );
  // this constructor *should* be 'explicit'
  vil_memory_image(vil_image const &);
  vil_memory_image(vil_memory_image const&);

  vil_memory_image& operator=(vil_memory_image const&);

  void resize(int width, int height);
  void resize(int planes, int width, int height);
  inline void* get_buffer() { return rows0_[0]; }

  vil_memory_image(void *buf, int planes, int w, int h, vil_memory_image_format const&);
  vil_memory_image(void *buf, int planes, int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(void *buf, int planes, int w, int h, vil_pixel_format_t);
  vil_memory_image(void *buf, int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(void *buf, int w, int h, vil_pixel_format_t );

  // these duplicate the signatures in the base class, but are inlined, so
  // much faster. the base class methods incur a virtual function call.
  int width () const { return width_ ; }
  int height() const { return height_; }

  void assert_size(int width, int height) const;

  //: Reset this class's member variables from the image implementation
  // This is is useful if the impl object has been modified outside the control of this object.
  void recache_from_impl();
 protected:
  // The following informations are cached from the vil_memory_image_impl :
  int width_;
  int height_;
  void ** rows0_;
};

#endif // vil_memory_image_h_
