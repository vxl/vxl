/*
  fsm
*/
#include <vcl_iostream.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_flipud.h>
#include <vil1/vil1_save.h>

#include <vil1/vil1_memory_image.h>

int main(int argc, char **argv) {
  for (int i=1; i<argc; ++i) {
    vil1_image I = vil1_load(argv[i]);
    vcl_cerr << "I : " << I << vcl_endl;

    if (!I) {
      vcl_cerr << "ignore " << argv[i] << vcl_endl;
      continue;
    }

    I = vil1_memory_image(I /*, "bah"*/); // close file
    vcl_cerr << "I : " << I << vcl_endl;

    I = vil1_flipud(I);
    vcl_cerr << "I : " << I << vcl_endl;

    vil1_save(I, argv[i], "jpeg");
    vcl_cerr << "I : " << I << vcl_endl;
  }
  return 0;
}
