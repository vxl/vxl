// This is core/vpdl/vpdt/vpdt_enable_if.h
#ifndef vpdt_enable_if_h_
#define vpdt_enable_if_h_
//:
// \file
// \author Matthew Leotta
// \brief Use static booleans to control template instantiation
// \date March 16, 2009
//
// These functions are borrow from Boost
//
// \verbatim
//  Modifications
//   None
// \endverbatim



template <bool B, class T = void>
struct vpdt_enable_if_c {
  typedef T type;
};

template <class T>
struct vpdt_enable_if_c<false, T> {};

template <class Cond, class T = void>
struct vpdt_enable_if : public vpdt_enable_if_c<Cond::value, T> {};


template <bool B, class T = void>
struct vpdt_disable_if_c {
  typedef T type;
};

template <class T>
struct vpdt_disable_if_c<true, T> {};

template <class Cond, class T = void>
struct vpdt_disable_if : public vpdt_disable_if_c<Cond::value, T> {};


#endif // vpdt_enable_if_h_
