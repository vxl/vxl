#ifndef vcl_iso_functional_h_
#define vcl_iso_functional_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <functional>

// unary_function
#ifndef vcl_unary_function
#define vcl_unary_function std::unary_function
#endif
// binary_function
#ifndef vcl_binary_function
#define vcl_binary_function std::binary_function
#endif
// plus
#ifndef vcl_plus
#define vcl_plus std::plus
#endif
// minus
#ifndef vcl_minus
#define vcl_minus std::minus
#endif
// multiplies
#ifndef vcl_multiplies
#define vcl_multiplies std::multiplies
#endif
// divides
#ifndef vcl_divides
#define vcl_divides std::divides
#endif
// modulus
#ifndef vcl_modulus
#define vcl_modulus std::modulus
#endif
// negate
#ifndef vcl_negate
#define vcl_negate std::negate
#endif
// equal_to
#ifndef vcl_equal_to
#define vcl_equal_to std::equal_to
#endif
// not_equal_to
#ifndef vcl_not_equal_to
#define vcl_not_equal_to std::not_equal_to
#endif
// greater
#ifndef vcl_greater
#define vcl_greater std::greater
#endif
// less
#ifndef vcl_less
#define vcl_less std::less
#endif
// greater_equal
#ifndef vcl_greater_equal
#define vcl_greater_equal std::greater_equal
#endif
// less_equal
#ifndef vcl_less_equal
#define vcl_less_equal std::less_equal
#endif
// logical_and
#ifndef vcl_logical_and
#define vcl_logical_and std::logical_and
#endif
// logical_or
#ifndef vcl_logical_or
#define vcl_logical_or std::logical_or
#endif
// logical_not
#ifndef vcl_logical_not
#define vcl_logical_not std::logical_not
#endif
// identity
#ifndef vcl_identity
#define vcl_identity std::identity
#endif
// select1st
#ifndef vcl_select1st
#define vcl_select1st std::select1st
#endif
// select2nd
#ifndef vcl_select2nd
#define vcl_select2nd std::select2nd
#endif
// project1st
#ifndef vcl_project1st
#define vcl_project1st std::project1st
#endif
// project2nd
#ifndef vcl_project2nd
#define vcl_project2nd std::project2nd
#endif
// constant_void_fun
#ifndef vcl_constant_void_fun
#define vcl_constant_void_fun std::constant_void_fun
#endif
// constant_unary_fun
#ifndef vcl_constant_unary_fun
#define vcl_constant_unary_fun std::constant_unary_fun
#endif
// constant_binary_fun
#ifndef vcl_constant_binary_fun
#define vcl_constant_binary_fun std::constant_binary_fun
#endif

#endif
