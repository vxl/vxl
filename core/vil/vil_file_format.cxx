// This is core/vil/vil_file_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
#include "vil_file_format.h"

vil_file_format::~vil_file_format()
{
}

#define HAS_PNM   1
#define HAS_VIFF  1
#define HAS_IRIS  1
#define HAS_MIT   1
#define HAS_BMP   1
#define HAS_GIF   0
// These will be defined "outside" if there really is a JPEG (PNG, TIFF) library
// #define HAS_JPEG
// #define HAS_PNG
// #define HAS_TIFF
#define HAS_RAS   1
#define HAS_GEN   0
#define HAS_DICOM 1
#define HAS_NITF 1

#if HAS_PNM
#include <vil/file_formats/vil_pnm.h>
#endif

#if HAS_IRIS
#include <vil/file_formats/vil_iris.h>
#endif

#if HAS_MIT
#include <vil/file_formats/vil_mit.h>
#endif

#if HAS_VIFF
#include <vil/file_formats/vil_viff.h>
#endif

#if HAS_PNG
#include <vil/file_formats/vil_png.h>
#endif

#if HAS_JPEG
#include <vil/file_formats/vil_jpeg.h>
#endif

#if HAS_TIFF
#include <vil/file_formats/vil_tiff.h>
#endif

#if HAS_BMP
#include <vil/file_formats/vil_bmp.h>
#endif

#if HAS_GIF
#include <vil/file_formats/vil_gif.h>
#endif

#if HAS_RAS
#include <vil/file_formats/vil_ras.h>
#endif

#if HAS_GEN
#include <vil/file_formats/vil_gen.h>
#endif

#if HAS_DICOM
#include <vil/file_formats/vil_dicom.h>
#endif

#if HAS_NITF
#include <vil/file_formats/vil_nitf.h>
#endif

//: Local class to hold file format list
// Clears list on deletion.
struct vil_file_format_storage
{
  vil_file_format** l;
  vil_file_format_storage(): l(new vil_file_format*[256])
  {
    unsigned c=0;
#if HAS_JPEG
    l[c++] = new vil_jpeg_file_format;
#endif
#if HAS_PNG
    l[c++] = new vil_png_file_format;
#endif
#if HAS_TIFF
    l[c++] = new vil_tiff_file_format;
#endif
#if HAS_PNM
    l[c++] = new vil_pnm_file_format;
    l[c++] = new vil_pbm_file_format;
    l[c++] = new vil_pgm_file_format;
    l[c++] = new vil_ppm_file_format;
#endif
#if HAS_IRIS
    l[c++] = new vil_iris_file_format;
#endif
#if HAS_MIT
    l[c++] = new vil_mit_file_format;
#endif
#if HAS_VIFF
    l[c++] = new vil_viff_file_format;
#endif
#if HAS_BMP
    l[c++] = new vil_bmp_file_format;
#endif
#if HAS_GIF
    l[c++] = new vil_gif_file_format;
#endif
#if HAS_RAS
    l[c++] = new vil_ras_file_format;
#endif
#if HAS_GEN
    l[c++] = new vil_gen_file_format;
#endif
#if HAS_DICOM
    l[c++] = new vil_dicom_file_format;
#endif
#if HAS_NITF
    l[c++] = new vil_nitf_file_format;
#endif

    l[c++] = 0;
  }

  ~vil_file_format_storage()
  {
    unsigned c=0;
    while (l[c]!=0)
      delete l[c++];
    delete [] l;
    l=0;
  }
};


vil_file_format** vil_file_format::all()
{
  static vil_file_format_storage storage;
  return storage.l;
}
