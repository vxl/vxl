#include <vcl_iostream.h>
#include <vil/vil_load.h>

int test_assign_main(int argc, char **argv)
{
  vil_image image;
  for (int i=1; i<argc; ++i) {
    vil_image tmp( vil_load(argv[i]) );
    vcl_cerr << "tmp = " << tmp << vcl_endl;
    image = tmp;
  }

  return 0;
}
