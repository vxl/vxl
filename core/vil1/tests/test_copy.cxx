#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>

#include <vil/vil_load.h>
#include <vil/vil_copy.h>

#include <vxl_config.h>
#include <vcl_iostream.h>

#include <testlib/testlib_test.h>

static char default_filename[] = "square.pgm";

MAIN( test_copy )
{
  START("vil_copy");

  char* filename = argv[1];
  if( argc<2 ) {
    filename = default_filename;
    vcl_cerr << "Using default input image " << filename << vcl_endl;
  }

  vil_memory_image_of<vxl_uint_8> a( vil_load( filename ) );
  if( !a ) {
    vcl_cerr << "Could not load image " << filename << vcl_endl;
    return 1;
  }
  vil_memory_image_of<vxl_uint_8> b( a.width(), a.height() );
  vil_copy( a, b );
  vil_memory_image_of<vxl_uint_8> c( vil_copy(a) );
  a(0,0) = 0;
  b(0,0) = 1;
  c(0,0) = 2;
  TEST("", a(0,0) != b(0,0) && a(0,0) != c(0,0) && b(0,0) != c(0,0) , true);

  SUMMARY();
}
