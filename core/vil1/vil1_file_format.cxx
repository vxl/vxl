// This is core/vil1/vil1_file_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_file_format.h"

vil1_file_format::~vil1_file_format()
{
}

#define HAS_PNM  1
#define HAS_VIFF 1
#define HAS_IRIS 1
#define HAS_MIT  1
#define HAS_BMP  1
#define HAS_GIF  1
// These will be defined "outside" if there really is a JPEG (PNG, TIFF) library
// #define HAS_JPEG
// #define HAS_PNG
// #define HAS_TIFF
#define HAS_RAS  1
#define HAS_GEN  1

#if HAS_PNM
#include <vil1/file_formats/vil1_pnm.h>
#endif

#if HAS_IRIS
#include <vil1/file_formats/vil1_iris.h>
#endif

#if HAS_MIT
#include <vil1/file_formats/vil1_mit.h>
#endif

#if HAS_VIFF
#include <vil1/file_formats/vil1_viff.h>
#endif

#if HAS_PNG
#include <vil1/file_formats/vil1_png.h>
#endif

#if HAS_JPEG
#include <vil1/file_formats/vil1_jpeg.h>
#endif

#if HAS_TIFF
#include <vil1/file_formats/vil1_tiff.h>
#endif

#if HAS_BMP
#include <vil1/file_formats/vil1_bmp.h>
#endif

#if HAS_GIF
#include <vil1/file_formats/vil1_gif.h>
#endif

#if HAS_RAS
#include <vil1/file_formats/vil1_ras.h>
#endif

#if HAS_GEN
#include <vil1/file_formats/vil1_gen.h>
#endif


static vil1_file_format** storage = 0;
vil1_file_format** vil1_file_format::all()
{
  if (storage == 0) {
    storage = new vil1_file_format*[256];
    int c = 0;

#if HAS_JPEG
    storage[c++] = new vil1_jpeg_file_format;
#endif
#if HAS_PNG
    storage[c++] = new vil1_png_file_format;
#endif
#if HAS_TIFF
    storage[c++] = new vil1_tiff_file_format;
#endif
#if HAS_PNM
    storage[c++] = new vil1_pnm_file_format;
#endif
#if HAS_IRIS
    storage[c++] = new vil1_iris_file_format;
#endif
#if HAS_MIT
    storage[c++] = new vil1_mit_file_format;
#endif
#if HAS_VIFF
    storage[c++] = new vil1_viff_file_format;
#endif
#if HAS_BMP
    storage[c++] = new vil1_bmp_file_format;
#endif
#if HAS_GIF
    storage[c++] = new vil1_gif_file_format;
#endif
#if HAS_RAS
    storage[c++] = new vil1_ras_file_format;
#endif
#if HAS_GEN
    storage[c++] = new vil1_gen_file_format;
#endif

    storage[c] = 0;
  }
  return storage;
}

class vil1_file_format_list_destroyer
{
public:
  ~vil1_file_format_list_destroyer()
  {
    unsigned i=0;
    while (storage[i])
      delete storage[i++];
    delete [] storage;
  }
};
static vil1_file_format_list_destroyer destroyer_obj;

