// This is core/vil1/vil1_file_format.h
#ifndef vil1_file_format_h_
#define vil1_file_format_h_
//:
// \file
// \brief Base class for image formats
// \author awf

#include <vil1/vil1_fwd.h>
#include <vil1/vil1_image_impl.h>

//: Base class for image formats.
//  There is one derived class for each handled file format in the
// directory file_formats. E.g. vil1/file_formats/vil1_pnm.h etc
class vil1_file_format
{
 public:
  virtual ~vil1_file_format();

  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  virtual char const* tag() const = 0;

  //: Attempt to make a generic_image which will read from vil1_stream vs.
  // Reads enough of vs to determine if it's this format, and if not, returns 0.
  // If it is, returns a subclass of vil1_image_impl on which get_section may
  // be applied.
  virtual vil1_image_impl* make_input_image(vil1_stream* vs) = 0;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, as an image header may be
  // written to it immediately.
  // The width/height etc are explicitly specified, so that file_format implementors
  // know what they need to do...
  virtual vil1_image_impl* make_output_image(vil1_stream* vs,
                                             int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             enum vil1_component_format) = 0;

 public:
  static vil1_file_format** all();
};

#endif // vil1_file_format_h_
