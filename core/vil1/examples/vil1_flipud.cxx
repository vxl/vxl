/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_flipud.h>
#include <vil/vil_save.h>

#include <vil/vil_memory_image.h>

int main(int argc, char **argv) {
  for (int i=1; i<argc; ++i) {
    vil_image I = vil_load(argv[i]);
    cerr << "I : " << I << endl;
    
    if (!I) {
      cerr << "ignore " << argv[i] << endl;
      continue;
    }

    I = vil_memory_image(I /*, "bah"*/); // close file
    cerr << "I : " << I << endl;
    
    I = vil_flipud(I);
    cerr << "I : " << I << endl;
    
    vil_save(I, argv[i], "jpeg");
    cerr << "I : " << I << endl;
  }
  return 0;
}
