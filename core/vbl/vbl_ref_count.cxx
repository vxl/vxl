// This is vxl/vbl/vbl_ref_count.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <vbl/vbl_ref_count.h>

//vbl_ref_count::vbl_ref_count() : ref_count(0) { }

// It is pointless to inline a virtual method. If the compiler
// doesn't know the type of an object at compile time it cannot
// inline the function call. fsm
vbl_ref_count::~vbl_ref_count() { }
