#ifndef vcl_iterator_h_
#define vcl_iterator_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_iterator.h>
#else
#  include <iterator>

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

#endif

#endif
