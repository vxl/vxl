// fsm@robots: gcc seems to think that 'char', 'signed char' 
// and 'unsigned char' are three different types, so this file
// is necessary.
// On the other hand, sunpro 5.0 thinks that 'char' and 'signed char'
// are the same, so we mustn't compile the file for sunpro 5.0
#include <vcl/vcl_compiler.h>
#if defined(VCL_GCC_295) || defined(VCL_EGCS) || defined(VCL_GCC_27)

#include <vcl/vcl_vector.txx>
VCL_VECTOR_INSTANTIATE(char);

#endif
