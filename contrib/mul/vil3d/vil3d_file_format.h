// This is mul/vil3d/vil3d_file_format.h
#ifndef vil3d_file_format_h_
#define vil3d_file_format_h_
//:
// \file
// \brief Manager for 3d image file loaders
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_image_view_base.h>
#include <vil3d/vil3d_image_resource.h>

class vil3d_file_format
{
 public:
  virtual ~vil3d_file_format() = default;

  //: Add a format reader to current list of those available
  // This function will take ownership of the passed object, and will
  // delete it when the program dies.
  static void add_format(vil3d_file_format* new_format);

  //: Number of formats available (number added by add_format()
  static unsigned n_formats();

  //: Access to available format readers supplied by add_format
  static const vil3d_file_format& format(unsigned i);

  //: Create an image_resource from an existing file.
  virtual vil3d_image_resource_sptr make_input_image(const char *filename)const =0;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format) const =0;

  //: default filename tag for this image.
  virtual const char *tag()const =0;
};

#endif
