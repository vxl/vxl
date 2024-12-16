// This is core/vsl/vsl_utils.hxx
#ifndef vsl_utils_hxx_
#define vsl_utils_hxx_
//:
// \file
// \brief  Template metaprogramming functions for heterogeneous data structures
// \author Noah Johnson
//
// Implementation

#include <tuple>
#include <type_traits>

#include <vsl/vsl_binary_io.h>


/*
 * for_each support in C++11, since std::for_each isn't available yet.
 * This implementation is based on the following: https://stackoverflow.com/a/26908596
 */


// This first overload is called if I is equal to the size of the tuple.
// Nothing happens and the recursion ends.
template <typename TupleType, typename FunctionType>
void
_vsl_for_each(TupleType &&,
              FunctionType,
              std::integral_constant<size_t, std::tuple_size<typename std::remove_reference<TupleType>::type>::value>)
{}


/* This second overload calls the function with the argument std::get<I>(t) and
 * increases the index by one. The class std::integral_constant is needed in
 * order to resolve the value of I at compile time. The std::enable_if SFINAE
 * stuff is used to help the compiler separate this overload from the previous
 * one, and call this overload only if I is smaller than the tuple size.
 */
template <std::size_t I,
          typename TupleType,
          typename FunctionType,
          typename =
            typename std::enable_if<I != std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type>
void
_vsl_for_each(TupleType && t, FunctionType f, std::integral_constant<size_t, I>)
{
  f(std::get<I>(std::forward<TupleType>(t)));
  _vsl_for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
}


// This third overload starts the recursion with I=0. Users call this.
template <typename TupleType, typename FunctionType>
void
_vsl_for_each(TupleType && t, FunctionType f)
{
  _vsl_for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
}


/*
 * Write / read / print generic functions, templated for any type.
 * Use with above code on heterogeneous data structures, e.g. tuples.
 * Can be replaced with generic lambdas once VXL requires C++14.
 * This implementation is based on the following: https://stackoverflow.com/a/30071501
 */


struct _vsl_generic_write
{
  _vsl_generic_write(vsl_b_ostream & os)
    : os_(os)
  {}

  template <class ElementType>
  void
  operator()(const ElementType & elem) const
  {
    vsl_b_write(this->os_, elem);
  }

  vsl_b_ostream & os_;
};

struct _vsl_generic_read
{
  _vsl_generic_read(vsl_b_istream & is)
    : is_(is)
  {}

  template <class ElementType>
  void
  operator()(ElementType & elem) const
  {
    vsl_b_read(this->is_, elem);
  }

  vsl_b_istream & is_;
};

struct _vsl_generic_print
{
  _vsl_generic_print(std::ostream & os)
    : os_(os)
  {}

  template <class ElementType>
  void
  operator()(const ElementType & elem) const
  {
    vsl_print_summary(this->os_, elem);
  }

  std::ostream & os_;
};

#endif // vsl_utils_hxx_
