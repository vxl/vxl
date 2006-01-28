// This is core/vil/vil_file_format.h
#ifndef vil_file_format_h_
#define vil_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for image formats
// \author awf

#include <vil/vil_fwd.h> // for vil_stream
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource.h>
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
  // If it is, returns a subclass of vil_image_resource on which get_section may
  // be applied.
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs) = 0;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, as an image header may be
  // written to it immediately.
  // The width/height etc are explicitly specified, so that file_format implementors
  // know what they need to do...
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format) = 0;
  //: construct a blocked output image resource
  // returns a null resource unless the format supports blocking
  virtual vil_blocked_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    unsigned size_block_i,
                                                    unsigned size_block_j,
                                                    enum vil_pixel_format)
    {return 0;}

 public:
  static vil_file_format** all();
};

#endif // vil_file_format_h_
