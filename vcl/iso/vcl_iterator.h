#ifndef vcl_iso_iterator_h_
#define vcl_iso_iterator_h_

#include <iterator>

// iterator_traits
#ifndef vcl_iterator_traits
#define vcl_iterator_traits std::iterator_traits
#endif
// iterator
#ifndef vcl_iterator
#define vcl_iterator std::iterator
#endif
// reverse_iterator
#ifndef vcl_reverse_iterator
#define vcl_reverse_iterator std::reverse_iterator
#endif
// istream_iterator
#ifndef vcl_istream_iterator
#define vcl_istream_iterator std::istream_iterator
#endif
// ostream_iterator
#ifndef vcl_ostream_iterator
#define vcl_ostream_iterator std::ostream_iterator
#endif
// istreambuf_iterator
#ifndef vcl_istreambuf_iterator
#define vcl_istreambuf_iterator std::istreambuf_iterator
#endif
// ostreambuf_iterator
#ifndef vcl_ostreambuf_iterator
#define vcl_ostreambuf_iterator std::ostreambuf_iterator
#endif
// back_insert_iterator
#ifndef vcl_back_insert_iterator
#define vcl_back_insert_iterator std::back_insert_iterator
#endif
// front_insert_iterator
#ifndef vcl_front_insert_iterator
#define vcl_front_insert_iterator std::front_insert_iterator
#endif
// insert_iterator
#ifndef vcl_insert_iterator
#define vcl_insert_iterator std::insert_iterator
#endif
// inserter
#ifndef vcl_inserter
#define vcl_inserter std::inserter
#endif
// input_iterator_tag
#ifndef vcl_input_iterator_tag
#define vcl_input_iterator_tag std::input_iterator_tag
#endif
// output_iterator_tag
#ifndef vcl_output_iterator_tag
#define vcl_output_iterator_tag std::output_iterator_tag
#endif
// forward_iterator_tag
#ifndef vcl_forward_iterator_tag
#define vcl_forward_iterator_tag std::forward_iterator_tag
#endif
// bidirectional_iterator_tag
#ifndef vcl_bidirectional_iterator_tag
#define vcl_bidirectional_iterator_tag std::bidirectional_iterator_tag
#endif
// random_access_iterator_tag
#ifndef vcl_random_access_iterator_tag
#define vcl_random_access_iterator_tag std::random_access_iterator_tag
#endif
// advance
#ifndef vcl_advance
#define vcl_advance std::advance
#endif
// distance
#ifndef vcl_distance
#define vcl_distance std::distance
#endif

#endif // vcl_iso_iterator_h_
