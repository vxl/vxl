#ifndef vil_file_format_h_
#define vil_file_format_h_

// Author: awf

#include <vil/vil_fwd.h>

//: Base class for image formats.
//  There is one derived class for each handled file format in the
// directory file_formats. E.g. vil/file_formats/vil_pnm.h etc
class vil_file_format {
public:
  virtual ~vil_file_format();
  
  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  virtual char const* tag() const = 0;

  //: Attempt to make a generic_image which will read from vil_stream vs.
  // Reads enough of vs to determine if it's this format, and if not, returns 0.
  // If it is, returns a subclass of vil_generic_image on which get_section may
  // be applied.
  virtual vil_generic_image* make_input_image(vil_stream* vs) = 0;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, and an image header
  // will be written to it immediately.
  // The vil_generic_image prototype is used to determine width/height etc.
  virtual vil_generic_image* make_output_image(vil_stream* vs, vil_generic_image const* prototype) = 0;

public:
  static vil_file_format** all();
};

#endif
