//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_iosfwd_h_
#define vcl_iosfwd_h_

// ANSI standard iostream forward decls.
// You can't write "class ostream" and expect it to work

#include <vcl/vcl_compiler.h>

#if VCL_USE_NATIVE_STL
# if defined(VCL_GCC_EGCS)
#  include <iosfwd>
# elif defined(VCL_SUNPRO_CC_50)
#  include <iosfwd>
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
# endif
#else
class istream;
class ostream;
class ifstream;
class ofstream;
#endif

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS VCL_iosfwd.
