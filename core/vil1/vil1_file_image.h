// This is core/vil1/vil1_file_image.h
#ifndef vil1_file_image_h_
#define vil1_file_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Class to load an image from disk
// \author awf@robots.ox.ac.uk
// \date 15 Mar 00

#include <vcl_string.h>
#include <vil1/vil1_image.h>

//: Load an image from disk
class vil1_file_image : public vil1_image
{
 public:
  enum verbosity {
    silent,
    laconic,
    verbose
  };

  // Default constructor
  vil1_file_image() {}

  //: Attempt to load named file
  vil1_file_image(char const* filename, verbosity v=verbose) {load(filename,v);}
  //: Attempt to load named file
  vil1_file_image(vcl_string const& f, verbosity v=verbose) { load(f, v); }

  //: Attempt to load named file
  bool load(char const* filename, verbosity = verbose);
  //: Attempt to load named file
  bool load(vcl_string const&, verbosity = verbose);
};

#endif // vil1_file_image_h_
