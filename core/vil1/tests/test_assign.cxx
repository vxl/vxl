#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <testlib/testlib_test.h>

MAIN( test_assign )
{
  START( "assign" );
  vil_image image;
  for (int i=1; i<argc; ++i) {
    vil_image tmp( vil_load(argv[i]) );
    vcl_cerr << "tmp = " << tmp << vcl_endl;
    image = tmp;
  }

  SUMMARY();
}
