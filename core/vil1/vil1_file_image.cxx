// This is core/vil1/vil1_file_image.cxx

#include <iostream>
#include "vil1_file_image.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_load.h>

bool vil1_file_image::load(std::string const& filename, verbosity)
{
  return this->load(filename.c_str());
}

bool vil1_file_image::load(char const* filename, verbosity v)
{
  vil1_image i = vil1_load(filename);
  if (v == verbose) {
    std::cerr << "vil1_file_image: Loaded [" << filename << "]\n";
  }
  if (!i && v != silent) {
    std::cerr << "vil1_file_image: Could not load [" << filename << "]\n";
  }

  vil1_image::operator= (i);
  return (*this)?true:false;
}
