// Example: format conversion.

#include <vcl/vcl_iostream.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>

int main(int argc, char ** argv)
{
  if (argc != 4) {
    vcl_cerr << "usage: vil_convert in out format\n";
    return -1;
  }
  char const* input_filename = argv[1];
  char const* output_filename = argv[2];
  char const* output_format = argv[3];

  vil_image in = vil_load(input_filename);
  if (!in) return -1;

  vil_save(in, output_filename, output_format);
}
