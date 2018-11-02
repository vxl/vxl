// This is conversions/image/Image_to_vil1.h
#ifndef Image_to_vil1_h_
#define Image_to_vil1_h_

#include <iostream>
#include <ImageClasses/Image.h>
#include <vil1/vil1_image.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_memory_image.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

inline vil1_image Image_to_vil1(Image const* im)
{
  // First try to load directly from file.  This will give wrong results
  // when im was copied into memory from a file image and then modified.
  const char* name = im->GetName();
  if (name && *name)
  {
    vil1_image imo = vil1_load(name);
    if (imo) return imo;
  }
  // was not able to open original file - probably this is an in-memory image:
  int cmpts = (im->GetImageClass() == 'C') ? 3 : 1; // very rough ! FIXME
  int width = im->GetSizeX();
  int height = im->GetSizeY();
  int bpc = im->GetBitsPixel() / cmpts;
  vil1_component_format format =
    (im->GetFormat() == 'A') ? VIL1_COMPONENT_FORMAT_IEEE_FLOAT :
    (im->GetFormat() == 'Y') ? VIL1_COMPONENT_FORMAT_COMPLEX :
                               VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
  if (im->GetBitsPixel() != bpc * cmpts)
    { std::cerr << "Image_to_vil1: Error: pixel size\n"; return 0; }

  vil1_memory_image imo(width, height, cmpts, bpc, format);

  void* buf = im->GetSection((void*)0, 0, 0, width, height);
  imo.put_section(buf, 0, 0, width, height);
  delete[] (char*)buf;
  return imo;
}

#endif // Image_to_vil1_h_
