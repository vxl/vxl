//:
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <vcl_iostream.h>
#include <testlib/testlib_root_dir.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>


int main(int argc, char** argv)
{
  vcl_string filename = testlib_root_dir() + "/vxl/vil/tests/file_read_data/ff_grey8bit_raw.pgm";
  vcl_cout<<"Load " << filename << " into an image data object." << vcl_endl;


  // This is how we quickly load an image view.

  vil2_image_view_base* im= vil2_load(filename.c_str());
  vil2_image_view<vil2_byte> b_im = static_cast<vil2_image_view<vil2_byte> >(*im);

  vil2_print_all(vcl_cout, b_im);

  // We can apply some operation to it.

  // And the we save it.

  vil2_save(b_im, "test.pbm");

  //don't forget to delete created view,

  delete im;

  return 0;
}
