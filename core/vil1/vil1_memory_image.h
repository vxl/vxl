//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_memory_image_h_
#define vil_memory_image_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
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
  vil_memory_image(int planes, int w, int h, vil_memory_image_format const& format);
  vil_memory_image(int planes, int w, int h, int components, int bits_per_component, vil_component_format component_format);
  vil_memory_image(int planes, int w, int h, vil_pixel_format pixel_format);
  vil_memory_image(int w, int h, int components, int bits_per_component, vil_component_format component_format);
  vil_memory_image(int w, int h, vil_pixel_format pixel_format);
  vil_memory_image(vil_image const &, char const *silly_hack);
  vil_memory_image(vil_memory_image const&);

  vil_memory_image& operator=(vil_memory_image const&);

  void resize(int width, int height);
  void resize(int planes, int width, int height);
  void* get_buffer() { return rows0_[0]; }

protected:
  // vil_memory_image_impl* impl; stored in vil_image

  // Caches these informations from the memory image
  int width_;
  int height_;
  void ** rows0_;

  vil_memory_image(): vil_image(0) { }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_memory_image.
