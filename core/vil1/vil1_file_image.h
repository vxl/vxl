#ifndef vil_file_image_h_
#define vil_file_image_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_file_image
// .INCLUDE vil/vil_file_image.h
// .FILE vil_file_image.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 15 Mar 00

#include <vcl/vcl_string.h> // C++ specific includes first
#include <vil/vil_image.h>

//: Load an image from disk
class vil_file_image : public vil_image {
public:
  enum verbosity {
    silent,
    laconic,
    verbose
  };
  
  vil_file_image();
  vil_file_image(char const* filename, verbosity = verbose);
  vil_file_image(vcl_string const&, verbosity = verbose);

  bool load(char const* filename, verbosity = verbose);
  bool load(vcl_string const&, verbosity = verbose);
};

#endif // vil_file_image_h_
