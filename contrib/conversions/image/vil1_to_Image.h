// This is conversions/image/vil1_to_Image.h
#ifndef vil1_to_Image_h_
#define vil1_to_Image_h_

#include <iostream>
#include <ImageClasses/Image.h>
#include <vil1/vil1_image.h>

#include <ImageClasses/MemoryImage.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

inline Image* vil1_to_Image(vil1_image const& im)
{
  if (im.planes() > 1)
    { std::cerr << "vil1_to_Image: Error: cannot handle >1 plane\n"; return 0; }

  // Create a MemoryImage and return:
  int cmpts = im.components();
  int bpp = cmpts * im.bits_per_component();
  int width = im.width();
  int height = im.height();
  char format =
    (im.component_format() == VIL1_COMPONENT_FORMAT_IEEE_FLOAT) ? 'A' :
    (im.component_format() == VIL1_COMPONENT_FORMAT_COMPLEX)    ? 'Y' :
                                                                 'L' ;
  ImageTemplate imt(width, height, bpp, 256, 256);
  imt.SetFormat(format);
  if (cmpts == 3) imt.SetImageClass('C');

  MemoryImage* imo = new MemoryImage(&imt);
  if (imo->GetBitsPixel() != im.bits_per_component() * cmpts)
    { std::cerr << "vil1_to_Image: Error: pixel size\n"; return 0; }

  char* buf = new char[im.get_size_bytes()];
  im.get_section(buf, 0, 0, width, height);
  imo->PutSection(buf, 0, 0, width, height);
  delete[] buf;
  return imo;
}

#endif // vil1_to_Image_h_
