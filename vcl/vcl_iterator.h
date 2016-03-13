#ifndef vcl_iterator_h_
#define vcl_iterator_h_

#include "vcl_compiler.h"
#include <iterator>
//vcl alias names to std names
#define vcl_iterator_traits std::iterator_traits
#define vcl_iterator std::iterator
#define vcl_reverse_iterator std::reverse_iterator
#define vcl_istream_iterator std::istream_iterator
#define vcl_ostream_iterator std::ostream_iterator
#define vcl_back_insert_iterator std::back_insert_iterator
#define vcl_front_insert_iterator std::front_insert_iterator
#define vcl_insert_iterator std::insert_iterator
#define vcl_inserter std::inserter
#define vcl_back_inserter std::back_inserter
#define vcl_front_inserter std::front_inserter
#define vcl_input_iterator_tag std::input_iterator_tag
#define vcl_output_iterator_tag std::output_iterator_tag
#define vcl_forward_iterator_tag std::forward_iterator_tag
#define vcl_bidirectional_iterator_tag std::bidirectional_iterator_tag
#define vcl_random_access_iterator_tag std::random_access_iterator_tag
#define vcl_advance std::advance
#define vcl_distance std::distance

#endif // vcl_iterator_h_
