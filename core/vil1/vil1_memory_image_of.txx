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
#include "vil_memory_image_of_format.txx"

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_generic_image const* image):
  vil_memory_image(image->planes(), image->width(), image->height(), vil_memory_image_of_format<T>())
{
  assert(image->components() == components_);
  assert(image->bits_per_component() == bits_per_component_);
  assert(image->component_format() == component_format_);

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
  T *p = (T*)buf_;
  T *e = p + planes_ * width_ * height_;
  while (p != e)
    *p++ = value;
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_memory_image_of<T> const & that):
  vil_memory_image(that)
{
}

template <class T>
void vil_memory_image_of<T>::set(vil_generic_image const * image)
{
  resize(image->width(), image->height());
  image->get_section(buf_, 0, 0, width_, height_);
}

template <class T>
vil_memory_image_of<T>& vil_memory_image_of<T>::operator=(vil_memory_image_of<T> const & that)
{
  planes_ = that.planes_;
  width_ = that.width_;
  height_ = that.height_;
  components_ = that.components_;
  bits_per_component_ = that.bits_per_component_;
  component_format_ = that.component_format_;
  bytes_per_pixel_ = that.bytes_per_pixel_;

  //assert(bytes_per_pixel_ * 8 == bits_per_component_ * components_); 
  int size = planes_ * width_ * height_ * bytes_per_pixel_;
  delete [] buf_;
  buf_ = new unsigned char[size];

  memcpy(buf_, that.buf_, size);
  return *this;
}

#define VIL_MEMORY_IMAGE_OF_INSTANTIATE(T)\
template class vil_memory_image_of<T >;
