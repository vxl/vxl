/*
  fsm
*/
#include <vcl_iostream.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_flipud.h>
#include <vil/vil_save.h>

#include <vil/vil_memory_image.h>

int main(int argc, char **argv) {
  for (int i=1; i<argc; ++i) {
    vil_image I = vil_load(argv[i]);
    vcl_cerr << "I : " << I << vcl_endl;

    if (!I) {
      vcl_cerr << "ignore " << argv[i] << vcl_endl;
      continue;
    }

    I = vil_memory_image(I /*, "bah"*/); // close file
    vcl_cerr << "I : " << I << vcl_endl;

    I = vil_flipud(I);
    vcl_cerr << "I : " << I << vcl_endl;

    vil_save(I, argv[i], "jpeg");
    vcl_cerr << "I : " << I << vcl_endl;
  }
  return 0;
}
