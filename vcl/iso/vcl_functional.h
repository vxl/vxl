#ifndef vcl_iso_functional_h_
#define vcl_iso_functional_h_

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
// unary_negate
#ifndef vcl_unary_negate
#define vcl_unary_negate std::unary_negate
#endif
// not1
#ifndef vcl_not1
#define vcl_not1 std::not1
#endif
// binary_negate
#ifndef vcl_binary_negate
#define vcl_binary_negate std::binary_negate
#endif
// not2
#ifndef vcl_not2
#define vcl_not2 std::not2
#endif
// binder1st
#ifndef vcl_binder1st
#define vcl_binder1st std::binder1st
#endif
// bind1st
#ifndef vcl_bind1st
#define vcl_bind1st std::bind1st
#endif
// binder2nd
#ifndef vcl_binder2nd
#define vcl_binder2nd std::binder2nd
#endif
// bind2nd
#ifndef vcl_bind2nd
#define vcl_bind2nd std::bind2nd
#endif
// pointer_to_unary_function
#ifndef vcl_pointer_to_unary_function
#define vcl_pointer_to_unary_function std::pointer_to_unary_function
#endif
// ptr_fun
#ifndef vcl_ptr_fun
#define vcl_ptr_fun std::ptr_fun
#endif
// pointer_to_binary_function
#ifndef vcl_pointer_to_binary_function
#define vcl_pointer_to_binary_function std::pointer_to_binary_function
#endif
// mem_fun_t
#ifndef vcl_mem_fun_t
#define vcl_mem_fun_t std::mem_fun_t
#endif
// mem_fun1_t
#ifndef vcl_mem_fun1_t
#define vcl_mem_fun1_t std::mem_fun1_t
#endif
// mem_fun
#ifndef vcl_mem_fun
#define vcl_mem_fun std::mem_fun
#endif
// mem_fun_ref_t
#ifndef vcl_mem_fun_ref_t
#define vcl_mem_fun_ref_t std::mem_fun_ref_t
#endif
// mem_fun1_ref_t
#ifndef vcl_mem_fun1_ref_t
#define vcl_mem_fun1_ref_t std::mem_fun1_ref_t
#endif
// mem_fun_ref
#ifndef vcl_mem_fun_ref
#define vcl_mem_fun_ref std::mem_fun_ref
#endif
// const_mem_fun_t
#ifndef vcl_const_mem_fun_t
#define vcl_const_mem_fun_t std::const_mem_fun_t
#endif
// const_mem_fun1_t
#ifndef vcl_const_mem_fun1_t
#define vcl_const_mem_fun1_t std::const_mem_fun1_t
#endif
// const_mem_fun_ref_t
#ifndef vcl_const_mem_fun_ref_t
#define vcl_const_mem_fun_ref_t std::const_mem_fun_ref_t
#endif
// const_mem_fun1_ref_t
#ifndef vcl_const_mem_fun1_ref_t
#define vcl_const_mem_fun1_ref_t std::const_mem_fun1_ref_t
#endif
// transform
#ifndef vcl_transform
#define vcl_transform std::transform
#endif

//not in the standard:
//@identity            
//@select1st           
//@select2nd           
//@project1st          
//@project2nd          
//@constant_void_fun   
//@constant_unary_fun  
//@constant_binary_fun 

#endif // vcl_iso_functional_h_
