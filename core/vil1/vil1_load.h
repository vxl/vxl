//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_load_h_
#define vil_load_h_
#ifdef __GNUC__
#pragma interface
#endif
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vcl/vcl_iosfwd.h>
#include <vil/vil_fwd.h>
#include <vil/vil_image.h>

//: Load an image
// Actually, return a pointer to a vil_image_impl object which will read the disk
// image when get_section is called.
vil_image vil_load(char const* filename);

//: Load from istream
// vil_image_impl* vil_load(istream&);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_load.
