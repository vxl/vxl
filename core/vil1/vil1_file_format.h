// This is vxl/vil/vil_file_format.h
#ifndef vil_file_format_h_
#define vil_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for image formats
// \author awf

#include <vil/vil_fwd.h>
#include <vil/vil_image_impl.h>

//: Base class for image formats.
//  There is one derived class for each handled file format in the
// directory file_formats. E.g. vil/file_formats/vil_pnm.h etc
class vil_file_format
{
 public:
  virtual ~vil_file_format();

  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  virtual char const* tag() const = 0;

  //: Attempt to make a generic_image which will read from vil_stream vs.
  // Reads enough of vs to determine if it's this format, and if not, returns 0.
  // If it is, returns a subclass of vil_image_impl on which get_section may
  // be applied.
  virtual vil_image_impl* make_input_image(vil_stream* vs) = 0;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, as an image header may be
  // written to it immediately.
  // The width/height etc are explicitly specified, so that file_format implementors
  // know what they need to do...
  virtual vil_image_impl* make_output_image(vil_stream* vs,
                                            int planes,
                                            int width,
                                            int height,
                                            int components,
                                            int bits_per_component,
                                            enum vil_component_format) = 0;

 public:
  static vil_file_format** all();
};

#endif // vil_file_format_h_
