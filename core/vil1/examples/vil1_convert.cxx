// Example: format conversion.

#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vil1/vil1_save.h"
#include "vil1/vil1_load.h"

int
main(int argc, char ** argv)
{
  if (argc != 4)
  {
    std::cerr << "usage: vil1_convert in out format\n";
    return -1;
  }
  const char * input_filename = argv[1];
  const char * output_filename = argv[2];
  const char * output_format = argv[3];

  vil1_image in = vil1_load(input_filename);
  if (!in)
    return -1;

  vil1_save(in, output_filename, output_format);
  return 0;
}
