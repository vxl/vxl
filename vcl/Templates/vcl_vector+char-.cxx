// fsm@robots: my gcc2.95 seems to think that 'char', 'signed char' 
// and 'unsigned char' are three different types. so I need this file.
// on the other hand, sunpro 5.0 thinks that 'char' and 'signed char'
// are the same, so we mustn't compile the file for sunpro 5.0
#include <vcl/vcl_compiler.h>
#if defined(VCL_GCC_295)

#include <vcl/vcl_vector.h>
#include <vcl/vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(char);

#endif
