//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_iosfwd_h_
#define vcl_iosfwd_h_

// ANSI standard iostream forward decls.
// You can't write "class ostream" and expect it to work

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
class istream;
class ostream;
class ifstream;
class ofstream;

#elif defined(VCL_GCC_EGCS)
# include <iosfwd>

#elif defined(VCL_SUNPRO_CC_50)
# include <iosfwd>
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

#elif defined(VCL_WIN32)
#  include <iosfwd>
 using std::istream;
 using std::ostream;
 using std::ifstream;
 using std::ofstream;
#endif

// This works for all cases, since we have "using std::xx" 
// statements for compilers with namespace std.
#define vcl_ostream ostream
#define vcl_istream istream
#define vcl_ofstream ofstream
#define vcl_ifstream ifstream

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS VCL_iosfwd.
