//:
// \file
// \brief Example of using rgb views and automatic conversion.
// \author Ian Scott

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vxl_config.h> // for vxl_byte
#include <testlib/testlib_root_dir.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  vcl_string filename = testlib_root_dir() + "/vxl/vil/tests/file_read_data/ff_rgb8bit_ascii.ppm";
  vcl_cout<<"Load " << filename << " into an rgb image object.\n";

  // This is how we quickly load an rgb image view.

  vil2_image_view<vil_rgb<vxl_byte> > b_im = vil2_load(filename.c_str());

  vil2_print_all(vcl_cout, b_im);

  // Note that if we try assign an image to a non compatible view type,
  // then the view will be set to empty.

  vcl_string filename2 = testlib_root_dir() + "/vxl/vil/tests/file_read_data/ff_grey8bit_raw.pgm";
  vcl_cout<<"\n\n\nTry to load " << filename2 << " into an rgb image object.\n";

  b_im = vil2_load(filename2.c_str());

  assert (!b_im);

  vil2_print_all(vcl_cout, b_im);

  return 0;
}
