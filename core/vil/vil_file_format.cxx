// This is mul/vil2/vil2_file_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil2_file_format.h"

vil2_file_format::~vil2_file_format()
{
}

#define HAS_PNM  1
#define HAS_VIFF 0
#define HAS_IRIS 0
#define HAS_MIT  0
#define HAS_BMP  0
#define HAS_GIF  0
// These will be defined "outside" if there really is a JPEG (PNG, TIFF) library
// #define HAS_JPEG
// #define HAS_PNG
// #define HAS_TIFF
#define HAS_RAS  0
#define HAS_GEN  0

#if HAS_PNM
#include <vil2/file_formats/vil2_pnm.h>
#endif

#if HAS_IRIS
#include <vil2/file_formats/vil2_iris.h>
#endif

#if HAS_MIT
#include <vil2/file_formats/vil2_mit.h>
#endif

#if HAS_VIFF
#include <vil2/file_formats/vil2_viff.h>
#endif

#if HAS_PNG
#include <vil2/file_formats/vil2_png.h>
#endif

#if HAS_JPEG
#include <vil2/file_formats/vil2_jpeg.h>
#endif

#if HAS_TIFF
#include <vil2/file_formats/vil2_tiff.h>
#endif

#if HAS_BMP
#include <vil2/file_formats/vil2_bmp.h>
#endif

#if HAS_GIF
#include <vil2/file_formats/vil2_gif.h>
#endif

#if HAS_RAS
#include <vil2/file_formats/vil2_ras.h>
#endif

#if HAS_GEN
#include <vil2/file_formats/vil2_gen.h>
#endif


static vil2_file_format** storage = 0;
vil2_file_format** vil2_file_format::all()
{
  if (storage == 0) {
    storage = new vil2_file_format*[256];
    int c = 0;

#if HAS_JPEG
    storage[c++] = new vil2_jpeg_file_format;
#endif
#if HAS_PNG
    storage[c++] = new vil2_png_file_format;
#endif
#if HAS_TIFF
    storage[c++] = new vil2_tiff_file_format;
#endif
#if HAS_PNM
    storage[c++] = new vil2_pnm_file_format;
    storage[c++] = new vil2_pbm_file_format;
    storage[c++] = new vil2_pgm_file_format;
    storage[c++] = new vil2_ppm_file_format;
#endif
#if HAS_IRIS
    storage[c++] = new vil2_iris_file_format;
#endif
#if HAS_MIT
    storage[c++] = new vil2_mit_file_format;
#endif
#if HAS_VIFF
    storage[c++] = new vil2_viff_file_format;
#endif
#if HAS_BMP
    storage[c++] = new vil2_bmp_file_format;
#endif
#if HAS_GIF
    storage[c++] = new vil2_gif_file_format;
#endif
#if HAS_RAS
    storage[c++] = new vil2_ras_file_format;
#endif
#if HAS_GEN
    storage[c++] = new vil2_gen_file_format;
#endif

    storage[c++] = 0;
  }
  return storage;
}
