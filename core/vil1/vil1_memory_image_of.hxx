// This is core/vil1/vil1_memory_image_of.hxx
#ifndef vil1_memory_image_of_hxx_
#define vil1_memory_image_of_hxx_

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 19 Aug 96
//
//-----------------------------------------------------------------------------

#include "vil1_memory_image_of.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_memory_image_of_format.hxx>

template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of():
  vil1_memory_image()
{
}

template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of(vil1_memory_image_of<T> const & that):
  vil1_memory_image(that)
{
}

template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of(vil1_image const& image):
  vil1_memory_image(image)
{
  assert(image.components() == components());
  assert(image.bits_per_component() == bits_per_component());
  assert(image.component_format() == component_format());
  // because of asserts, format is fine
  //  set(image);
}

template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of(int sizex, int sizey):
  vil1_memory_image(1, sizex, sizey, vil1_memory_image_of_format<T>())
{
}

// added by Brendan McCane for creating an image from already
// allocated memory.
template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of(T *buf, int sizex, int sizey):
  vil1_memory_image(buf, 1, sizex, sizey, vil1_memory_image_of_format<T>())
{
}

#if 0 // this method was removed
template <class T>
vil1_memory_image_of<T>::vil1_memory_image_of(int sizex, int sizey, T const& value):
  vil1_memory_image(1, sizex, sizey, vil1_memory_image_of_format<T>())
{
  T *p = (T*)get_buffer();
  T *e = p + planes() * width() * height();
  while (p != e)
    *p++ = value;
}
#endif

template <class T>
void vil1_memory_image_of<T>::set(vil1_image const& image)
{
  resize(image.width(), image.height());
  image.get_section(get_buffer(), 0, 0, width(), height());
}

template <class T>
vil1_memory_image_of<T>& vil1_memory_image_of<T>::operator=(vil1_memory_image_of<T> const &that)
{
  vil1_memory_image::operator=(that);
  return *this;
}

template <class T>
vil1_memory_image_of<T>& vil1_memory_image_of<T>::operator=(vil1_image const &that)
{
  assert((that.bits_per_component() * that.components()+7)/8 == sizeof (T));
  vil1_memory_image::operator=(that);
  return *this;
}

template <class  T>
void vil1_memory_image_of<T>::resize(int width, int height)
{
  if (ptr)
    vil1_memory_image::resize(width, height);
  else
    *this = vil1_memory_image_of<T>(width, height);
}

template <class  T>
void vil1_memory_image_of<T>::resize(int planes, int width, int height)
{
  assert(ptr);
  vil1_memory_image::resize(planes, width, height);
}

template <class T>
void vil1_memory_image_of<T>::fill(T const& v)
{
  int w = width();
  int h = height();
  for (int y=0; y<h; ++y) {
    T* raster = (*this)[y];
    for (int x=0; x<w; ++x)
      raster[x] = v;
  }
}

#define VIL1_MEMORY_IMAGE_OF_INSTANTIATE(T) \
template class vil1_memory_image_of<T >

#endif // vil1_memory_image_of_hxx_
