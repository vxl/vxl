//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_iosfwd_h_
#define vcl_iosfwd_h_

// ANSI standard iostream forward decls.
// You can't write "class ostream" and expect it to work

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# if defined(VCL_WIN32)
#  define vcl_iosfwd_STD std::
#  define vcl_iosfwd_USE 
# else
class istream;
class ostream;
class ifstream;
class ofstream;
#  define vcl_iosfwd_STD ::
# endif

#elif defined(VCL_EGCS) || (defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3))
# include <iosfwd>
# define vcl_iosfwd_STD 

#elif defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# include <iosfwd>
# define vcl_iosfwd_STD std::
# define vcl_iosfwd_USE 

#elif defined(VCL_SUNPRO_CC_50)
# include <iosfwd>
# define vcl_iosfwd_STD std::
# define vcl_iosfwd_USE 

#elif defined(VCL_WIN32)
#  include <iosfwd>
# define vcl_iosfwd_STD std::
# define vcl_iosfwd_USE 

#elif defined(VCL_SGI_CC_720)
# include <iostream.h>
# define vcl_iosfwd_STD ::

#elif defined(VCL_SGI_CC_730)
# include <iosfwd>
# define vcl_iosfwd_STD std::
# define vcl_iosfwd_USE 

#else // -------------------- iso
# include <vcl/iso/vcl_iosfwd.h>
#endif

#ifdef vcl_iosfwd_STD
# define vcl_ostream  vcl_iosfwd_STD ostream
# define vcl_istream  vcl_iosfwd_STD istream
# define vcl_ofstream vcl_iosfwd_STD ofstream
# define vcl_ifstream vcl_iosfwd_STD ifstream
#endif

#ifdef vcl_iosfwd_USE
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
#endif

#endif
