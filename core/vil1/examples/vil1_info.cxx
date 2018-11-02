// This is core/vil1/examples/vil1_info.cxx
// \author fsm

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>

int main(int argc, char **argv)
{
  for (int i=1; i<argc; ++i)
  {
    std::cerr << argv[i] << " :\n";
    vil1_image I = vil1_load(argv[i]);
    std::cerr << "  planes             : " << I.planes() << std::endl
             << "  width              : " << I.width() << std::endl
             << "  height             : " << I.height() << std::endl
             << "  components         : " << I.components() << std::endl
             << "  bits per component : " << I.bits_per_component() << std::endl
             << "  component format   : ";
    switch (I.component_format()) {
    case VIL1_COMPONENT_FORMAT_UNKNOWN: std::cerr << "unknown\n"; break;
    case VIL1_COMPONENT_FORMAT_UNSIGNED_INT: std::cerr << "unsigned\n"; break;
    case VIL1_COMPONENT_FORMAT_SIGNED_INT: std::cerr << "signed\n"; break;
    case VIL1_COMPONENT_FORMAT_IEEE_FLOAT: std::cerr << "float\n"; break;
    case VIL1_COMPONENT_FORMAT_COMPLEX: std::cerr << "complex\n"; break;
    default: return 1;
    }
  }
  return 0;
}
