// This is conversions/image/Image_to_vil.h
#ifndef Image_to_vil_h_
#define Image_to_vil_h_

#include <ImageClasses/Image.h>
#include <vil/vil_image.h>

#include <vil/vil_load.h>
#include <vil/vil_memory_image.h>

#include <vcl_iostream.h>

inline vil_image Image_to_vil(Image const* im)
{
  // First try to load directly from file.  This will give wrong results
  // when im was copied into memory from a file image and then modified.
  const char* name = im->GetName();
  if (name && *name)
  {
    vil_image imo = vil_load(name);
    if (imo) return imo;
  }
  // was not able to open original file - probably this is an in-memory image:
  int cmpts = (im->GetImageClass() == 'C') ? 3 : 1; // very rough ! FIXME
  int width = im->GetSizeX();
  int height = im->GetSizeY();
  int bpc = im->GetBitsPixel() / cmpts;
  vil_component_format format =
    (im->GetFormat() == 'A') ? VIL_COMPONENT_FORMAT_IEEE_FLOAT :
    (im->GetFormat() == 'Y') ? VIL_COMPONENT_FORMAT_COMPLEX :
                               VIL_COMPONENT_FORMAT_UNSIGNED_INT;
  if (im->GetBitsPixel() != bpc * cmpts)
    { vcl_cerr << "Image_to_vil: Error: pixel size\n"; return 0; }

  vil_memory_image imo(width, height, cmpts, bpc, format);

  void* buf = im->GetSection((void*)0, 0, 0, width, height);
  imo.put_section(buf, 0, 0, width, height);
  delete[] (char*)buf;
  return imo;
}

#endif // Image_to_vil_h_
