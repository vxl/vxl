// This is mul/vil2/vil2_file_format.h
#ifndef vil2_file_format_h_
#define vil2_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for image formats
// \author awf

#include <vil/vil_fwd.h>
#include <vil2/vil2_image_data.h>

//: Base class for image formats.
//  There is one derived class for each handled file format in the
// directory file_formats. E.g. vil2/file_formats/vil2_pnm.h etc
class vil2_file_format
{
 public:
  virtual ~vil2_file_format();

  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  virtual char const* tag() const = 0;

  //: Attempt to make a generic_image which will read from vil2_stream vs.
  // Reads enough of vs to determine if it's this format, and if not, returns 0.
  // If it is, returns a subclass of vil2_image_data on which get_section may
  // be applied.
  virtual vil2_image_data* make_input_image(vil_stream* vs) = 0;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, as an image header may be
  // written to it immediately.
  // The width/height etc are explicitly specified, so that file_format implementors
  // know what they need to do...
  virtual vil2_image_data* make_output_image(vil_stream* vs,
                                               unsigned planes,
                                               unsigned width,
                                               unsigned height,
                                               unsigned components,
                                               unsigned bits_per_component,
                                               enum vil_component_format) = 0;

 public:
  static vil2_file_format** all();
};

#endif // vil2_file_format_h_
