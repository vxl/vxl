// This is core/vil/examples/vil_info.cxx
// \author fsm

#include <vcl_iostream.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>

int main(int argc, char **argv)
{
  for (int i=1; i<argc; ++i)
  {
    vcl_cerr << argv[i] << " :\n";
    vil_image I = vil_load(argv[i]);
    vcl_cerr << "  planes             : " << I.planes() << vcl_endl
             << "  width              : " << I.width() << vcl_endl
             << "  height             : " << I.height() << vcl_endl
             << "  components         : " << I.components() << vcl_endl
             << "  bits per component : " << I.bits_per_component() << vcl_endl
             << "  component format   : ";
    switch (I.component_format()) {
    case VIL_COMPONENT_FORMAT_UNKNOWN: vcl_cerr << "unknown\n"; break;
    case VIL_COMPONENT_FORMAT_UNSIGNED_INT: vcl_cerr << "unsigned\n"; break;
    case VIL_COMPONENT_FORMAT_SIGNED_INT: vcl_cerr << "signed\n"; break;
    case VIL_COMPONENT_FORMAT_IEEE_FLOAT: vcl_cerr << "float\n"; break;
    case VIL_COMPONENT_FORMAT_COMPLEX: vcl_cerr << "complex\n"; break;
    default: return 1;
    }
  }
  return 0;
}
