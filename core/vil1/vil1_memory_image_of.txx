// This is core/vil/vil_memory_image_of.txx
#ifndef vil_memory_image_of_txx_
#define vil_memory_image_of_txx_

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 19 Aug 96
//
//-----------------------------------------------------------------------------

#include "vil_memory_image_of.h"
#include <vcl_cassert.h>
#include <vil/vil_memory_image_of_format.txx>

template <class T>
vil_memory_image_of<T>::vil_memory_image_of():
  vil_memory_image()
{
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_memory_image_of<T> const & that):
  vil_memory_image(that)
{
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(vil_image const& image):
  vil_memory_image(image)
{
  assert(image.components() == components());
  assert(image.bits_per_component() == bits_per_component());
  assert(image.component_format() == component_format());
  // because of asserts, format is fine
  //  set(image);
}

template <class T>
vil_memory_image_of<T>::vil_memory_image_of(int sizex, int sizey):
  vil_memory_image(1, sizex, sizey, vil_memory_image_of_format<T>())
{
}

// added by Brendan McCane for creating an image from already
// allocated memory.
template <class T>
vil_memory_image_of<T>::vil_memory_image_of(T *buf, int sizex, int sizey):
  vil_memory_image(buf, 1, sizex, sizey, vil_memory_image_of_format<T>())
{
}

#if 0 // this method was removed
template <class T>
vil_memory_image_of<T>::vil_memory_image_of(int sizex, int sizey, T const& value):
  vil_memory_image(1, sizex, sizey, vil_memory_image_of_format<T>())
{
  T *p = (T*)get_buffer();
  T *e = p + planes() * width() * height();
  while (p != e)
    *p++ = value;
}
#endif

template <class T>
void vil_memory_image_of<T>::set(vil_image const& image)
{
  resize(image.width(), image.height());
  image.get_section(get_buffer(), 0, 0, width(), height());
}

template <class T>
vil_memory_image_of<T>& vil_memory_image_of<T>::operator=(vil_memory_image_of<T> const &that)
{
  vil_memory_image::operator=(that);
  return *this;
}

template <class T>
vil_memory_image_of<T>& vil_memory_image_of<T>::operator=(vil_image const &that)
{
  assert((that.bits_per_component() * that.components()+7)/8 == sizeof (T));
  vil_memory_image::operator=(that);
  return *this;
}

template <class  T>
void vil_memory_image_of<T>::resize(int width, int height)
{
  if (ptr)
    vil_memory_image::resize(width, height);
  else
    *this = vil_memory_image_of<T>(width, height);
}

template <class  T>
void vil_memory_image_of<T>::resize(int planes, int width, int height)
{
  assert(ptr);
  vil_memory_image::resize(planes, width, height);
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

#define VIL_MEMORY_IMAGE_OF_INSTANTIATE(T) \
template class vil_memory_image_of<T >

#endif // vil_memory_image_of_txx_
