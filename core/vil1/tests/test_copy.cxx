#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>

#include <vil/vil_load.h>
#include <vil/vil_copy.h>

#include <vxl_config.h>

#include <vcl_iostream.h>

int
main( int argc, char* argv[] )
{
  vcl_cout << "Testing vil_copy\n";
  if( argc==1 ) {
    vcl_cerr << "Need an input image" << vcl_endl;
    return 1;
  }

  vil_memory_image_of<vxl_uint_8> a( vil_load( argv[1] ) );
  if( !a ) {
    vcl_cerr << "Could not load image " << argv[1] << vcl_endl;
    return 1;
  }
  vil_memory_image_of<vxl_uint_8> b( a.width(), a.height() );
  vil_copy( a, b );
  vil_memory_image_of<vxl_uint_8> c( vil_copy(a) );
  a(0,0) = 0;
  b(0,0) = 1;
  c(0,0) = 2;
  if( a(0,0) != b(0,0) && a(0,0) != c(0,0) && b(0,0) != c(0,0) ) {
    vcl_cout << "  PASSED  \n";
    return 0;
  } else {
    vcl_cout << "**FAILED**\n";
    return 1;
  }
}
