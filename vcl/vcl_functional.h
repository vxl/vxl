#ifndef vcl_functional_h_
#define vcl_functional_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_functional.h>
#else

#if defined(VCL_WIN32) 
#define vcl_function_h_STD std::
#else 
#define vcl_function_h_STD ::
#endif

# define  vcl_unary_function vcl_function_h_STD unary_function
# define  vcl_binary_function vcl_function_h_STD binary_function
# define  vcl_plus vcl_function_h_STD plus
# define  vcl_minus vcl_function_h_STD minus
# define  vcl_multiplies vcl_function_h_STD multiplies
# define  vcl_divides vcl_function_h_STD divides
# define  vcl_modulus vcl_function_h_STD modulus
# define  vcl_negate vcl_function_h_STD negate
# define  vcl_equal_to vcl_function_h_STD equal_to
# define  vcl_not_equal_to vcl_function_h_STD not_equal_to
# define  vcl_greater vcl_function_h_STD greater
# define  vcl_less vcl_function_h_STD less
# define  vcl_greater_equal vcl_function_h_STD greater_equal
# define  vcl_less_equal vcl_function_h_STD less_equal
# define  vcl_logical_and vcl_function_h_STD logical_and
# define  vcl_logical_or vcl_function_h_STD logical_or
# define  vcl_logical_not vcl_function_h_STD logical_not
# define  vcl___unary_fun_aux vcl_function_h_STD __unary_fun_aux
# define  vcl_identity vcl_function_h_STD identity
# define  vcl_select1st vcl_function_h_STD select1st
# define  vcl_select2nd vcl_function_h_STD select2nd
# define  vcl_project1st vcl_function_h_STD project1st
# define  vcl_project2nd vcl_function_h_STD project2nd
# define  vcl_constant_void_fun vcl_function_h_STD constant_void_fun
# define  vcl_constant_unary_fun vcl_function_h_STD constant_unary_fun
# define  vcl_constant_binary_fun vcl_function_h_STD constant_binary_fun

# ifdef __GNUC__
#  include_next <function.h>
# elif defined(VCL_SUNPRO_CC) || defined (WIN32)
// SunPro
#  include <functional>

// select1st and select2nd are extensions: they are not part of the standard.
template <class _Pair>
struct vcl_Select1st : public vcl_unary_function<_Pair, typename _Pair::first_type> {
  const typename _Pair::first_type& operator()(const _Pair& __x) const {
    return __x.first;
  }
};
 
template <class _Pair>
struct vcl_Select2nd : public vcl_unary_function<_Pair, typename _Pair::second_type>
{
  const typename _Pair::second_type& operator()(const _Pair& __x) const {
    return __x.second;
  }
};

// add select* to std
namespace std {
  template <class _Pair> struct select1st;
  template <class _Pair> struct select2nd;
};

template <class _Pair> 
struct vcl_select1st : public vcl_Select1st<_Pair> 
{
};

template <class _Pair> 
struct vcl_select2nd : public vcl_Select2nd<_Pair> 
{
};


template <class T>
inline bool operator!=(const T& x, const T& y) {
       return !(x == y);
   }
   
   template <class T>
   inline bool operator>(const T& x, const T& y) {
       return y < x;
   }
   
   template <class T>
   inline bool operator<=(const T& x, const T& y) {
       return !(y < x);
   }
   
   template <class T>
   inline bool operator>=(const T& x, const T& y) {
       return !(x < y);
   }
# else
  ** help **   
# endif
#endif

#endif


