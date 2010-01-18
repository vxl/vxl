/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_intpow_hxx
#define rtvl_intpow_hxx

#if defined(_MSC_VER) && _MSC_VER <= 1300
# define RTVL_INTPOW_HELP , rtvl_intpow_impl<T, n>* = 0
#else
# define RTVL_INTPOW_HELP
#endif

template <bool> struct rtvl_bool {};
template <typename T, unsigned int n> struct rtvl_intpow_impl;

template <unsigned int n, typename T>
inline T rtvl_intpow(T x RTVL_INTPOW_HELP)
{
  return rtvl_intpow_impl<T, n>::compute(x);
}

template <typename T, unsigned int n>
struct rtvl_intpow_impl
{
  typedef rtvl_bool<true> const& true_;
  typedef rtvl_bool<false> const& false_;
  typedef rtvl_intpow_impl<T, n> self_type;
  static inline T f(T x, true_, true_)  { return self_type::compute(x*x)*x; }
  static inline T f(T x, true_, false_) { return self_type::compute(x*x); }
  static inline T f(T x, false_, true_) { return x; }
  static inline T f(T, false_, false_)  { return 1; }
  static inline T compute(T x)
    {
    return (rtvl_intpow_impl<T, (n>>1)>::f(
              x, rtvl_bool<!!(n>>1)>(), rtvl_bool<!!(n&1)>()));
    }
};

#endif
