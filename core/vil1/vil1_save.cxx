//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vil_save.h"
#endif
//
// Class: vil_save
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
// Modifications:
//   000216 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vil_save.h"

#include <vcl/vcl_cstring.h>

#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_copy.h>

//: Send vil_generic_image to disk.
bool vil_save(vil_generic_image const* i, char const* filename, char const* file_format)
{
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
  return vil_save(i, os, file_format);
}

//: Send vil_generic_image to output stream.
// The possible file_formats are defined by the subclasses of vil_file_format
// in vil_file_format.cxx
bool vil_save(vil_generic_image const* i, vil_stream* os, char const* file_format)
{
  vil_generic_image* outimage = 0;
  for(vil_file_format** p = vil_file_format::all(); *p; ++p) {
    vil_file_format* fmt = *p;
    if (strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_output_image(os, i);
      if (outimage == 0) {
	cerr << "vil_save: Unknown cannot save to type [" << file_format << "]\n";
	break;
      }
    }
  }

  if (outimage == 0) {
    cerr << "vil_save: Unknown file type [" << file_format << "]\n";
    return false;
  }

  vil_copy(i, outimage);

  return true;
}
