//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_save_h_
#define vil_save_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_save
// .INCLUDE vil/vil_save.h
// .FILE vil_save.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00
//
// .SECTION Modifications
//     000216 AWF Initial version.

class vil_stream;
#include <vil/vil_image.h>

//: Send vil_image to disk, deducing format from filename
bool vil_save(vil_image const&, char const* filename);

//: Send vil_image to disk, given filename
bool vil_save(vil_image const&, char const* filename, char const* file_format);

//: Send vil_image_impl to outstream
bool vil_save(vil_image const &, vil_stream* outstream, char const* file_format = "pnm");

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_save.
