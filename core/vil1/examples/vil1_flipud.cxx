/*
  fsm
*/
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_flipud.h>
#include <vil1/vil1_save.h>

#include <vil1/vil1_memory_image.h>

int main(int argc, char **argv) {
  for (int i=1; i<argc; ++i) {
    vil1_image I = vil1_load(argv[i]);
    std::cerr << "I : " << I << std::endl;

    if (!I) {
      std::cerr << "ignore " << argv[i] << std::endl;
      continue;
    }

    I = vil1_memory_image(I /*, "bah"*/); // close file
    std::cerr << "I : " << I << std::endl;

    I = vil1_flipud(I);
    std::cerr << "I : " << I << std::endl;

    vil1_save(I, argv[i], "jpeg");
    std::cerr << "I : " << I << std::endl;
  }
  return 0;
}
