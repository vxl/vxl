// This is core/vsl/vsl_tuple_io.hxx
#ifndef vsl_tuple_io_hxx_
#define vsl_tuple_io_hxx_
//:
// \file
// \brief  binary IO functions for std::tuple<T1, T2, ...>
// \author Noah Johnson
//
// Implementation

#include "vsl_tuple_io.h"

#include <tuple>
#include <type_traits>

#include <vsl/vsl_binary_io.h>


// https://stackoverflow.com/a/26908596

// The first overload is called if I is equal to the size of the tuple.
// The function then simply does nothing and ends the recursion.
template<typename TupleType, typename FunctionType>
void _vsl_for_each(TupleType&&, FunctionType,
              std::integral_constant<size_t, std::tuple_size<typename std::remove_reference<TupleType>::type>::value>) {}


/* The second overload calls the function with the argument std::get<I>(t) and
 * increases the index by one. The class std::integral_constant is needed in
 * order to resolve the value of I at compile time. The std::enable_if SFINAE
 * stuff is used to help the compiler separate this overload from the previous
 * one, and call this overload only if I is smaller than the tuple size (on
 * Coliru this is needed, whereas in Visual Studio it works without).
 */
template<std::size_t I, typename TupleType, typename FunctionType,
         typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type>
void _vsl_for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
{
    f(std::get<I>(std::forward<TupleType>(t)));
    _vsl_for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
}


// The third overload starts the recursion with I=0. Users should call this.
template<typename TupleType, typename FunctionType>
void _vsl_for_each(TupleType&& t, FunctionType f)
{
    _vsl_for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
}


// https://stackoverflow.com/a/30071501
// Allow generic lambdas in C++11
struct _vsl_generic_write {
  _vsl_generic_write(vsl_b_ostream& os) : os_(os) {}

  template <class TupleElementType>
  void operator()(const TupleElementType& elem) const {
    vsl_b_write(this->os_, elem);
  }

  vsl_b_ostream& os_;
};

struct _vsl_generic_read {
  _vsl_generic_read(vsl_b_istream& is) : is_(is) {}

  template <class TupleElementType>
  void operator()(TupleElementType& elem) const {
    vsl_b_read(this->is_, elem);
  }

  vsl_b_istream& is_;
};

struct _vsl_generic_print {
  _vsl_generic_print(std::ostream& os) : os_(os) {}

  template <class TupleElementType>
  void operator()(const TupleElementType& elem) const {
    vsl_print_summary(this->os_, elem);
  }

  std::ostream& os_;
};


//: Write tuple to binary stream
template <typename ... Types>
void vsl_b_write(vsl_b_ostream& os, const std::tuple<Types ...>& v) {

  constexpr short version_no = 1;
  vsl_b_write(os, version_no);

  _vsl_for_each(v, _vsl_generic_write(os));
}


//: Read tuple from binary stream
template <typename ... Types>
void vsl_b_read(vsl_b_istream& is, std::tuple<Types ...>& v) {
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
    {
      _vsl_for_each(v, _vsl_generic_read(is));
      break;
    }
    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, Tuple&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit);  // Set an unrecoverable IO error on stream
      return;
    }
  }
}


//: Print human readable summary of object to a stream
template <typename ... Types>
void vsl_print_summary(std::ostream& os, const std::tuple<Types ...>& v) {

  os << "Tuple size: " << std::tuple_size<std::tuple<Types ...> >::value << std::endl;
  _vsl_for_each(v, _vsl_generic_print(os));
  os << std::endl;
}


#undef VSL_TUPLE_IO_INSTANTIATE
#define VSL_TUPLE_IO_INSTANTIATE(...) \
template void vsl_print_summary<__VA_ARGS__>(std::ostream& os, const std::tuple<__VA_ARGS__>& v); \
template void vsl_b_write<__VA_ARGS__>(vsl_b_ostream& os, const std::tuple<__VA_ARGS__>& v); \
template void vsl_b_read<__VA_ARGS__>(vsl_b_istream& is, std::tuple<__VA_ARGS__>& v)

#endif // vsl_tuple_io_hxx_
