// This is core/vil/vil_file_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma implementation
#endif
// :
// \file
#include "vil_file_format.h"

vil_file_format::~vil_file_format()
{
}

#include <vcl_cstdlib.h>
#include <vil/vil_config.h> // for list of configured file formats
#include <vil/vil_exception.h>

#if HAS_PNM
#  include <vil/file_formats/vil_pnm.h>
#endif

#if HAS_IRIS
#  include <vil/file_formats/vil_iris.h>
#endif

#if HAS_MIT
#  include <vil/file_formats/vil_mit.h>
#endif

#if HAS_VIFF
#  include <vil/file_formats/vil_viff.h>
#endif

#if HAS_PNG
#  include <vil/file_formats/vil_png.h>
#endif

#if HAS_JPEG
#  include <vil/file_formats/vil_jpeg.h>
#endif

#if HAS_TIFF
#  include <vil/file_formats/vil_tiff.h>
#  include <vil/file_formats/vil_pyramid_image_list.h>
#endif

#if HAS_BMP
#  include <vil/file_formats/vil_bmp.h>
#endif

#if HAS_GIF
#  include <vil/file_formats/vil_gif.h>
#endif

#if HAS_RAS
#  include <vil/file_formats/vil_ras.h>
#endif

#if HAS_GEN
#  include <vil/file_formats/vil_gen.h>
#endif

#if HAS_DCMTK
#  include <vil/file_formats/vil_dicom.h>
#endif

#if HAS_NITF
#  include <vil/file_formats/vil_nitf2_image.h>
#endif

#if HAS_J2K
#  include <vil/file_formats/vil_j2k_image.h>
#endif

#if HAS_OPENJPEG2
#  include <vil/file_formats/vil_openjpeg.h>
#endif

const unsigned MAX_FILE_FORMATS = 256;
// : Local class to hold file format list
// Clears list on deletion.
struct vil_file_format_storage
  {
  vil_file_format* * l;
  vil_file_format_storage() : l(new vil_file_format *[MAX_FILE_FORMATS])
  {
    unsigned c = 0;

#if HAS_JPEG
    l[c++] = new vil_jpeg_file_format;
#endif
#if HAS_PNG
    l[c++] = new vil_png_file_format;
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
// the DCMTK based reader is more complete, so use try that
// before the vil implementation
#if HAS_DCMTK
    l[c++] = new vil_dicom_file_format;
#endif

#if HAS_NITF
    l[c++] = new vil_nitf2_file_format;
#endif

#if HAS_J2K
    l[c++] = new vil_j2k_file_format;
#endif

#if HAS_OPENJPEG2
    l[c++] = new vil_openjpeg_jp2_file_format;
    // l[c++] = new vil_openjpeg_jpt_file_format;
    l[c++] = new vil_openjpeg_j2k_file_format;
#endif

#if HAS_TIFF
    l[c++] = new vil_tiff_file_format;
    l[c++] = new vil_pyramid_image_list_format;
#endif
    l[c++] = 0;
  }

  ~vil_file_format_storage()
  {
    unsigned c = 0;

    while( l[c] != 0 )
      {
      delete l[c++];
      }

    delete [] l;
    l = 0;
  }

  };

// : The function will take ownership of ff;
void vil_file_format::add_file_format(vil_file_format* ff)
{
  vil_file_format* * l = all();
  unsigned           c = 0;

  while( c < MAX_FILE_FORMATS - 1u && l[c] != 0 )
    {
    ++c;
    }

  if( l[c] != 0 )
    {
    vcl_cerr << "ERROR vil_file_format::add_file_format Unable to add any more file formats\n";
    vcl_abort();
    }
  l[c] = ff;
  l[c + 1] = 0;
}

vil_file_format * * vil_file_format::all()
{
  static vil_file_format_storage storage;

  return storage.l;
}
