/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#include <vil/vil_image_ref.h>
#include <vil/vil_load.h>

int main(int argc, char **argv) {
  for (unsigned i=1; i<argc; ++i) {
    cerr << argv[i] << " :" << endl;
    vil_image_ref I = vil_load(argv[i]);
    cerr << "  planes             : " << I->planes() << endl
	 << "  width              : " << I->width() << endl
	 << "  height             : " << I->height() << endl
	 << "  components         : " << I->components() << endl
	 << "  bits per component : " << I->bits_per_component() << endl
	 << "  component format   : ";
    switch (I->component_format()) {
    case VIL_COMPONENT_FORMAT_UNKNOWN: cerr << "unknown" << endl; break;
    case VIL_COMPONENT_FORMAT_UNSIGNED_INT: cerr << "unsigned" << endl; break;
    case VIL_COMPONENT_FORMAT_SIGNED_INT: cerr << "signed" << endl; break;
    case VIL_COMPONENT_FORMAT_IEEE_FLOAT: cerr << "float" << endl; break;
    case VIL_COMPONENT_FORMAT_COMPLEX: cerr << "complex" << endl; break;
    default: abort(); break; // MAINTAINME
    }
  }
  return 0;
}
