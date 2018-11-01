// This is core/vil/vil_file_format.cxx
//:
// \file
#include <cstdlib>
#include "vil_file_format.h"

vil_file_format::~vil_file_format() = default;

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_config.h> // for list of configured file formats
#include <vil/vil_exception.h>

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
#include <vil/file_formats/vil_pyramid_image_list.h>
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

#if HAS_DCMTK
#include <vil/file_formats/vil_dicom.h>
#endif

#if HAS_NITF
#include <vil/file_formats/vil_nitf2_image.h>
#endif

#if HAS_J2K
#include <vil/file_formats/vil_j2k_image.h>
#endif

#if HAS_OPENJPEG2
#include <vil/file_formats/vil_openjpeg.h>
#endif

#if HAS_SGI
#include <vil/file_formats/vil_sgi.h>
#endif

//constexpr unsigned MAX_FILE_FORMATS = 256;
//: Local class to hold file format list
// Clears list on deletion.
struct vil_file_format_storage
{
  std::list<vil_file_format*> l;
  vil_file_format_storage()
  {
#if HAS_JPEG
    l.push_back(new vil_jpeg_file_format);
#endif
#if HAS_PNG
    l.push_back(new vil_png_file_format);
#endif
#if HAS_PNM
    l.push_back(new vil_pnm_file_format);
    l.push_back(new vil_pbm_file_format);
    l.push_back(new vil_pgm_file_format);
    l.push_back(new vil_ppm_file_format);
#endif
#if HAS_IRIS
    l.push_back(new vil_iris_file_format);
#endif
#if HAS_MIT
    l.push_back(new vil_mit_file_format);
#endif
#if HAS_VIFF
    l.push_back(new vil_viff_file_format);
#endif
#if HAS_BMP
    l.push_back(new vil_bmp_file_format);
#endif
#if HAS_GIF
    l.push_back(new vil_gif_file_format);
#endif
#if HAS_RAS
    l.push_back(new vil_ras_file_format);
#endif
#if HAS_GEN
    l.push_back(new vil_gen_file_format);
#endif
// the DCMTK based reader is more complete, so use try that
// before the vil implementation
#if HAS_DCMTK
    l.push_back(new vil_dicom_file_format);
#endif

#if HAS_NITF
  l.push_back(new vil_nitf2_file_format);
#endif

#if HAS_J2K
  l.push_back(new vil_j2k_file_format);
#endif

#if HAS_OPENJPEG2
  l.push_back(new vil_openjpeg_jp2_file_format);
  l.push_back(new vil_openjpeg_j2k_file_format);
#endif

#if HAS_TIFF
    l.push_back(new vil_tiff_file_format);
    l.push_back(new vil_pyramid_image_list_format);
#endif

#if HAS_SGI
  l.push_back(new vil_sgi_file_format);
#endif
  }

  ~vil_file_format_storage()
  {
    for(auto & i : l)
    {
      if(i)
      {
        delete i;
      }
    }
  }
};

//: The function will take ownership of ff;
void vil_file_format::add_file_format(vil_file_format* ff)
{
  std::list<vil_file_format*>& l = all();

  // Always add runtime-loaded formats to the front to allow them to replace
  // built-in formats
  l.push_front(ff);
}


std::list<vil_file_format*>& vil_file_format::all()
{
  static vil_file_format_storage storage;
  return storage.l;
}
