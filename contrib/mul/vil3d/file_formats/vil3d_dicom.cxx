#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for DICOM format images.
// \author Ian Scott, Chris Wolstenholme - Manchester

#include "vil3d_dicom.h"
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vil2/vil2_stream_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil2/file_formats/vil2_dicom_header.h>

vil3d_dicom_format::vil3d_dicom_format() {}

vil3d_dicom_format::~vil3d_dicom_format() {}


vil3d_image_resource_sptr vil3d_dicom_format::make_input_image(const char * filename) const
{
  if (!vul_file::exists(filename))
    return 0;

  vil2_dicom_header_format dhf;  // Header format for reading
  vil2_dicom_header_info dhi;    // Info held in the header
  vil2_stream_fstream *is = new vil2_stream_fstream(filename, "r");
  is->ref();
  dhi = dhf.readHeader(*is);
  is->unref();
  if (dhf.headerValid())
    ; // return new vil3d_dicom_image(filename);

  return 0;
}


//: Make a "generic_image" on which put_section may be applied.
// The file may bo openned immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_dicom_format::make_output_image
                   (const char* filename, unsigned ni, unsigned nj,
                    unsigned nk, unsigned nplanes, enum vil2_pixel_format) const
{
  vcl_cerr <<"vil3d_dicom_format::make_output_image() NYI\n";
  vcl_abort();
  return 0;
}

