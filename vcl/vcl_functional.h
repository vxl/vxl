#ifndef vcl_functional_h_
#define vcl_functional_h_

#include "vcl_compiler.h"
#include <functional>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_algorithm.h"
//vcl alias names to std names
#define vcl_unary_function std::unary_function
#define vcl_binary_function std::binary_function
#define vcl_plus std::plus
#define vcl_minus std::minus
#define vcl_multiplies std::multiplies
#define vcl_divides std::divides
#define vcl_modulus std::modulus
#define vcl_negate std::negate
#define vcl_equal_to std::equal_to
#define vcl_not_equal_to std::not_equal_to
#define vcl_greater std::greater
#define vcl_less std::less
#define vcl_greater_equal std::greater_equal
#define vcl_less_equal std::less_equal
#define vcl_logical_and std::logical_and
#define vcl_logical_or std::logical_or
#define vcl_logical_not std::logical_not
#define vcl_unary_negate std::unary_negate
#define vcl_not1 std::not1
#define vcl_binary_negate std::binary_negate
#define vcl_not2 std::not2
#define vcl_binder1st std::binder1st
#define vcl_bind1st std::bind1st
#define vcl_binder2nd std::binder2nd
#define vcl_bind2nd std::bind2nd
#define vcl_pointer_to_unary_function std::pointer_to_unary_function
#define vcl_ptr_fun std::ptr_fun
#define vcl_pointer_to_binary_function std::pointer_to_binary_function
#define vcl_mem_fun_t std::mem_fun_t
#define vcl_mem_fun std::mem_fun
#define vcl_mem_fun1_t std::mem_fun1_t
#define vcl_mem_fun1 std::mem_fun1
#define vcl_mem_fun_ref_t std::mem_fun_ref_t
#define vcl_mem_fun_ref std::mem_fun_ref
#define vcl_mem_fun1_ref_t std::mem_fun1_ref_t
#define vcl_mem_fun1_ref std::mem_fun1_ref
#define vcl_const_mem_fun_t std::const_mem_fun_t
#define vcl_const_mem_fun std::const_mem_fun
#define vcl_const_mem_fun1_t std::const_mem_fun1_t
#define vcl_const_mem_fun1 std::const_mem_fun1
#define vcl_const_mem_fun_ref_t std::const_mem_fun_ref_t
#define vcl_const_mem_fun_ref std::const_mem_fun_ref
#define vcl_const_mem_fun1_ref_t std::const_mem_fun1_ref_t
#define vcl_const_mem_fun1_ref std::const_mem_fun1_ref

#endif // vcl_functional_h_
