// Class: vil_memory_image_of
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 19 Aug 96
// Modifications:
//     960916 AWF Added save_pgm and more comments.
//     961209 Peter Vanroose added operator=() and copy constructor
//     980710 FSM Changed constructor to take a const Image *, not Image *
//     981105 AWF Made bilinear/bicubic return double.
//     990211 Peter Vanroose moved save_pgm() to Templates/ (EGCS complained)
//     990421 FSM Added constructor from a const fast_array<T> &
//
//-----------------------------------------------------------------------------

#include "vil_memory_image_of.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdlib.h> // abort()
#include <vil/vil_memory_image_of_format.txx>

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_image const& image):
  vil_memory_image(image.planes(), image.width(), image.height(), vil_memory_image_of_format<T>())
{
  assert(image.components() == components());
  assert(image.bits_per_component() == bits_per_component());
  assert(image.component_format() == component_format());

  set(image);
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(int sizex, int sizey):
  vil_memory_image(1, sizex, sizey, vil_memory_image_of_format<T>())
{
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(int sizex, int sizey, const T& value):
  vil_memory_image(1, sizex, sizey, vil_memory_image_of_format<T>())
{
  T *p = (T*)get_buffer();
  T *e = p + planes() * width() * height();
  while (p != e)
    *p++ = value;
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_memory_image_of<T> const & that):
  vil_memory_image(that)
{
}

template <class T>
void vil_memory_image_of<T>::set(vil_image const& image)
{
  resize(image.width(), image.height());
  image.get_section(get_buffer(), 0, 0, width(), height());
}

template <class T>
vil_memory_image_of<T>& vil_memory_image_of<T>::operator=(vil_memory_image_of<T> const & /*that*/)
{
  abort(); return *this;
//  planes_ = that.planes();
//  width_ = that.width();
//  height_ = that.height();
//  components_ = that.components();
//  bits_per_component_ = that.bits_per_component();
//  component_format_ = that.component_format();
//  bytes_per_pixel_ = that.bytes_per_pixel();
//
//  //assert(bytes_per_pixel_ * 8 == bits_per_component_ * components_); 
//  int size = planes_ * width_ * height_ * bytes_per_pixel_;
//  delete [] buf_;
//  buf_ = new unsigned char[size];
//
//  memcpy(buf_, that.buf_, size);
//  return *this;
}

template <class T>
void vil_memory_image_of<T>::fill(T const& v)
{
  int w = width();
  int h = height();
  for (int y=0; y<h; ++y) {
    T* raster = (*this)[y];
    for (int x=0; x<w; ++x)
      raster[x] = v;
  }
}


#define VIL_MEMORY_IMAGE_OF_INSTANTIATE(T)\
template class vil_memory_image_of<T >;
