#ifndef vcl_iterator_h_
#define vcl_iterator_h_

#include <vcl/vcl_compiler.h>

// ---------- emulation
#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_iterator.h>

// ---------- SunPro compiler
#elif defined(VCL_SUNPRO_CC)
# include <iterator>
// fsm: I get a funny error with "using std::?stream_iterator;",
//"vxl/vcl/vcl_iterator.h", line 19: Error: istream_iterator is not a member of std.
//"vxl/vcl/vcl_iterator.h", line 20: Error: ostream_iterator is not a member of std.
// but the following seems to work :
# define vcl_istream_iterator std::istream_iterator
# define vcl_ostream_iterator std::ostream_iterator

// ---------- all other compilers
#else
# include <iterator>

#ifdef WIN32
#define vcl_iterator_category_Iter_cat
#endif

using std::back_insert_iterator;
using std::front_insert_iterator;
using std::insert_iterator;
// SUNPRO 5 has no reverse_bidirectional_iterator
// using std::reverse_bidirectional_iterator;
using std::reverse_iterator;
// vc has no raw_storage_iterator
// using std::raw_storage_iterator;
using std::istream_iterator;
using std::ostream_iterator;

#define vcl_back_insert_iterator back_insert_iterator
#define vcl_front_insert_iterator front_insert_iterator
#define vcl_insert_iterator insert_iterator
// #define vcl_reverse_bidirectional_iterator reverse_bidirectional_iterator
#define vcl_reverse_iterator reverse_iterator
// #define vcl_raw_storage_iterator raw_storage_iterator
#define vcl_istream_iterator istream_iterator
#define vcl_ostream_iterator ostream_iterator

using std :: forward_iterator_tag;
#define vcl_forward_iterator_tag forward_iterator_tag

#endif

#endif
