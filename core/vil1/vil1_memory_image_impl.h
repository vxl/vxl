//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_memory_image_impl_h_
#define vil_memory_image_impl_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_memory_image_impl
// .INCLUDE vil/vil_memory_image_impl.h
// .FILE vil_memory_image_impl.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Mar 00

#include <vil/vil_image_impl.h>
#include <vil/vil_memory_image.h>

//: implementation class for vil_memory_image.
class vil_memory_image_impl : public vil_image_impl {
public:
  vil_memory_image_impl(int planes, int w, int h, vil_memory_image_format const& format);
  vil_memory_image_impl(int planes, int w, int h, int components, int bits_per_component, vil_component_format component_format);
  vil_memory_image_impl(int planes, int w, int h, vil_pixel_format pixel_format);
  vil_memory_image_impl(int w, int h, int components, int bits_per_component, vil_component_format component_format);
  vil_memory_image_impl(int w, int h, vil_pixel_format pixel_format);
  vil_memory_image_impl(vil_memory_image_impl const&);
  ~vil_memory_image_impl();

  virtual int planes() const { return planes_; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }
  virtual int bits_per_component() const { return bits_per_component_; }
  virtual vil_component_format component_format() const { return component_format_; }
  virtual vil_image get_plane(int plane) const;

  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  void resize(int planes, int width, int height);
  
protected:
  friend class vil_memory_image;

  void init(int planes, int w, int h, vil_pixel_format pixel_format);
  void init(int planes, int w, int h, int components, int bits_per_component, vil_component_format);

  int planes_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil_component_format component_format_;

  int bytes_per_pixel_;
  
  unsigned char* buf_;
  void*** rows_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_memory_image_impl.

