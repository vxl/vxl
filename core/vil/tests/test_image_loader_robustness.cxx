// This is core/vil/tests/test_image_loader_robustness.cxx
#include <vcl_string.h>
#include <vil/vil_load.h>

int
test_image_loader_robustness_main( int argc, char* argv[] )
{
  vcl_string image_base;
  if ( argc >= 2 ) {
    image_base = argv[1];
#ifdef VCL_WIN32
    image_base += "\\";
#else
    image_base += "/";
#endif
  }
  vil_image_resource_sptr ir = vil_load_image_resource((image_base + "rubbish_image.bin").c_str());
  if (ir)
    return 1;
  else
    return 0;
}
