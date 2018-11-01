// This is core/doc/vxl_doc_rules.cxx
//:
// \file

#include "vxl_doc_rules.h"

//: By all means replicate the documentation from the .h file here.
// But it seems that doxygen does not reliably pick up documentation in
// the .cxx file whereas we haven't had a problem with it picking it up
// from the .h file.
// The .h file documentation, if present, has precedence in any case.
my_class::my_class()
{
  // This constructor does absolutely nothing
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class some_weird_class_I_do_not_want_documented
{
};
#endif // DOXYGEN_SHOULD_SKIP_THIS
