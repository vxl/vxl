// This is vxl/doc/vxl_doc_rules.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vxl_doc_rules.h"

//: By all means replicate the documentation from the .h file here
// But it seems the doxygen does not reliably pick up documentation in
// the .cxx file whereas we haven't had a problem with it picking it up
// from the .h file.
my_class::my_class()
{
  // This class does absolutely nothing
}
