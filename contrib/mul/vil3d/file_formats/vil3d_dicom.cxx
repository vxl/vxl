//:
// \file
// \brief Reader/Writer for DICOM format images.
// \author Ian Scott, Chris Wolstenholme - Manchester

#include <iostream>
#include <cstdlib>
#include "vil3d_dicom.h"

#if 0

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>


vil3d_dicom_format::vil3d_dicom_format() {}

vil3d_dicom_format::~vil3d_dicom_format() {}


vil3d_image_resource_sptr vil3d_dicom_format::make_input_image(const char * filename) const
{
  vil2_smart_ptr<vil2_stream> is = vil2_open(filename);
  if (!is || !is->ok())
    return 0;

  vil_dicom_format dicom_reader;
  vil_image_resource_sptr im = dicom_reader.make_input_image(is.as_pointer());
  if (!im) return 0;

  // Now deduce filename numbering format.
  // start at the front and find all continuous groups of numeric characters.
  // For each group, search for all contiguously numbered files
  // Try and load each group of contiguously

  return 0;
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_dicom_format::make_output_image
                   (const char* filename, unsigned ni, unsigned nj,
                    unsigned nk, unsigned nplanes, enum vil_pixel_format) const
{
  std::cerr <<"vil3d_dicom_format::make_output_image() NYI\n";
  std::abort();
  return 0;
}

#endif // 0
