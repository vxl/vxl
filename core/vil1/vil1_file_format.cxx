#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_file_format.h"

vil_file_format::~vil_file_format()
{
}

#include <vil/file_formats/vil_pnm.h>
#include <vil/file_formats/vil_iris.h>
#include <vil/file_formats/vil_mit.h>
#include <vil/file_formats/vil_viff.h>

#if HAS_PNG
#include <vil/file_formats/vil_png.h>
#endif

#if HAS_JPEG
#include <vil/file_formats/vil_jpeg.h>
#endif

#if HAS_TIFF
#include <vil/file_formats/vil_tiff.h>
#endif

#include <vil/file_formats/vil_bmp.h>



static vil_file_format** storage = 0;
vil_file_format** vil_file_format::all()
{
  if (storage == 0) {
    storage = new vil_file_format*[32];
    int c = 0;

#if HAS_JPEG
    storage[c++] = new vil_jpeg_file_format;
#endif
#if HAS_PNG
    storage[c++] = new vil_png_file_format;
#endif
#if HAS_TIFF
    storage[c++] = new vil_tiff_file_format;
#endif
    storage[c++] = new vil_pnm_file_format;
    storage[c++] = new vil_iris_file_format;
    storage[c++] = new vil_mit_file_format;
    storage[c++] = new vil_viff_file_format;
    storage[c++] = new vil_bmp_file_format;

    storage[c++] = 0;
  }
  return storage;
}
