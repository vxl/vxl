// This is core/vil/vil_file_format.h
#ifndef vil_file_format_h_
#define vil_file_format_h_
//:
// \file
// \brief Base class for image formats
// \author awf

#include <list>
#include "vil_fwd.h" // for vil_stream
#include "vil_image_resource.h"
#include "vil_blocked_image_resource.h"
#include "vil_pyramid_image_resource.h"

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

  //: Read a pyramid resource from a list of image files in a directory
  //  ... or from an image file_format that supports multiple images per file.
  virtual vil_pyramid_image_resource_sptr
    make_input_pyramid_image(char const* /*directory_or_file*/)
    {return nullptr;}

  //: Construct a pyramid image resource from a base image.
  //  All levels are stored in the same resource file. Each level has the same
  //  scale ratio (0.5) to the preceding level. Level 0 is the original
  //  base image. The resource is returned open for reading.
  //  The temporary directory is for storing intermediate image
  //  resources during the construction of the pyramid. Files are
  //  be removed from the directory after completion.  If temp_dir is 0
  //  then the intermediate resources are created in memory.
  virtual vil_pyramid_image_resource_sptr
    make_pyramid_image_from_base(char const* /*filename*/,
                                 vil_image_resource_sptr const& /*base_image*/,
                                 unsigned /*nlevels*/,
                                 char const* /*temp_dir*/)
    {return nullptr;}

  virtual vil_pyramid_image_resource_sptr
  make_pyramid_image_from_base(char const* directory,
                               vil_image_resource_sptr const& base_image,
                               unsigned int nlevels,
                               bool copy_base,
                               char const* level_file_format,
                               char const* filename) { return nullptr; } ;

  //: Make a "generic_image" on which put_section may be applied.
  // The stream vs is assumed to be open for writing, as an image header may be
  // written to it immediately.
  // The width/height etc are explicitly specified, so that file_format implementors
  // know what they need to do...
  virtual vil_image_resource_sptr make_output_image(vil_stream* /*vs*/,
                                                    unsigned /*nx*/,
                                                    unsigned /*ny*/,
                                                    unsigned /*nplanes*/,
                                                    enum vil_pixel_format) = 0;
  //: Construct a blocked output image resource
  // Returns a null resource unless the format supports blocking
  virtual vil_blocked_image_resource_sptr
    make_blocked_output_image(vil_stream* /*vs*/,
                              unsigned /*nx*/,
                              unsigned /*ny*/,
                              unsigned /*nplanes*/,
                              unsigned /*size_block_i*/,
                              unsigned /* size_block_j*/,
                              enum vil_pixel_format)
    {return nullptr;}

  virtual vil_pyramid_image_resource_sptr
    make_pyramid_output_image(char const* /*file*/)
    {return nullptr;}

 public:
  typedef std::list<vil_file_format*>::iterator iterator;
  static std::list<vil_file_format*>& all();
  static void add_file_format(vil_file_format* ff);
};

#endif // vil_file_format_h_
