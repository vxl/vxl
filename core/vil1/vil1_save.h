//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_save_h_
#define vil_save_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vil/vil_fwd.h>

//: Send vil_generic_image to disk, given filename
bool vil_save(vil_generic_image const*, char const* filename, char const* file_format = "pnm");

//: Send vil_generic_image to outstream
bool vil_save(vil_generic_image const*, vil_stream* outstream, char const* file_format = "pnm");

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_save.
