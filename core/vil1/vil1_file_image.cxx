// This is vxl/vil/vil_file_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_file_image.h"
#include <vcl_iostream.h>

#include <vil/vil_load.h>

vil_file_image::vil_file_image(char const* filename, verbosity v)
{
  load(filename, v);
}

vil_file_image::vil_file_image(vcl_string const& filename, verbosity v)
{
  load(filename, v);
}

vil_file_image::vil_file_image()
{
}

bool vil_file_image::load(vcl_string const& filename, verbosity)
{
  return this->load(filename.c_str());
}

bool vil_file_image::load(char const* filename, verbosity v)
{
  vil_image i = vil_load(filename);
  if (v == verbose) {
    vcl_cerr << "vil_file_image: Loaded [" << filename << "]\n";
  }
  if (!i && v != silent) {
    vcl_cerr << "vil_file_image: Could not load [" << filename << "]\n";
  }

  vil_image::operator= (i);
  return operator bool ();
}
