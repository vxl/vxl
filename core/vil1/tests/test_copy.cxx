#include <vil1/vil1_memory_image_of.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_copy.h>

#include <vxl_config.h>
#include <vcl_iostream.h>

#include <testlib/testlib_test.h>

static char default_filename[] = "square.pgm";

static void test_copy(int argc, char* argv[])
{
  const char* filename = argv[1];
  if ( argc<2 ) {
    filename = default_filename;
    vcl_cerr << "Using default input image " << filename << vcl_endl;
  }

  vil1_image img = vil1_load( filename );
  if ( !img ) {
    vcl_cerr << "Could not load image " << filename << vcl_endl;
    testlib_test_perform(false);
    return;
  }
  vil1_memory_image_of<vxl_uint_8> a( img );
  vil1_memory_image_of<vxl_uint_8> b( a.width(), a.height() );
  vil1_copy( a, b );
  vil1_memory_image_of<vxl_uint_8> c( vil1_copy(a) );
  a(0,0) = 0;
  b(0,0) = 1;
  c(0,0) = 2;
  TEST("", a(0,0) != b(0,0) && a(0,0) != c(0,0) && b(0,0) != c(0,0) , true);
}

TESTMAIN_ARGS(test_copy);
