// This is vxl/vil/vil_file_image.h
#ifndef vil_file_image_h_
#define vil_file_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Class to load an image from disk
// \author awf@robots.ox.ac.uk
// \date 15 Mar 00

#include <vcl_string.h>
#include <vil/vil_image.h>

//: Load an image from disk
class vil_file_image : public vil_image
{
 public:
  enum verbosity {
    silent,
    laconic,
    verbose
  };

  //: Default constructor
  vil_file_image();

  //: Attempt to load named file
  vil_file_image(char const* filename, verbosity = verbose);
  //: Attempt to load named file
  vil_file_image(vcl_string const&, verbosity = verbose);

  //: Attempt to load named file
  bool load(char const* filename, verbosity = verbose);
  //: Attempt to load named file
  bool load(vcl_string const&, verbosity = verbose);
};

#endif // vil_file_image_h_
