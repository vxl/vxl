#ifndef vil_memory_image_h_
#define vil_memory_image_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_memory_image
// .INCLUDE vil/vil_memory_image.h
// .FILE vil_memory_image.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vil/vil_pixel.h>
#include <vil/vil_image.h>

struct vil_memory_image_format {
  int components;
  int bits_per_component;
  vil_component_format component_format;
};

//: vil_image, stored entirely in RAM
class vil_memory_image : public vil_image {
public:
  vil_memory_image();
  vil_memory_image(int planes, int w, int h, vil_memory_image_format const&);
  vil_memory_image(int planes, int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(int planes, int w, int h, vil_pixel_format);
  vil_memory_image(int w, int h, int components, int bits_per_component, vil_component_format);
  vil_memory_image(int w, int h, vil_pixel_format );
  // this constructor *should* be 'explicit'
  vil_memory_image(vil_image const &);
  vil_memory_image(vil_memory_image const&);

  vil_memory_image& operator=(vil_memory_image const&);

  void resize(int width, int height);
  void resize(int planes, int width, int height);
  void* get_buffer() { return rows0_[0]; }

  void assert_size(int width, int height) const;

protected:
  // The following informations are cached from the vil_memory_image_impl :
  int width_;
  int height_;
  void ** rows0_;
};

#endif // vil_memory_image_h_
