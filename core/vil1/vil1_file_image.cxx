// This is core/vil1/vil1_file_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_file_image.h"
#include <vcl_iostream.h>

#include <vil1/vil1_load.h>

vil1_file_image::vil1_file_image(char const* filename, verbosity v)
{
  load(filename, v);
}

vil1_file_image::vil1_file_image(vcl_string const& filename, verbosity v)
{
  load(filename, v);
}

vil1_file_image::vil1_file_image()
{
}

bool vil1_file_image::load(vcl_string const& filename, verbosity)
{
  return this->load(filename.c_str());
}

bool vil1_file_image::load(char const* filename, verbosity v)
{
  vil1_image i = vil1_load(filename);
  if (v == verbose) {
    vcl_cerr << "vil1_file_image: Loaded [" << filename << "]\n";
  }
  if (!i && v != silent) {
    vcl_cerr << "vil1_file_image: Could not load [" << filename << "]\n";
  }

  vil1_image::operator= (i);
  return (*this)?true:false;
}
