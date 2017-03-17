/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_meta_h
#define rgtl_meta_h

//:
// \file
// \brief Template metaprogramming utilities.
// \author Brad King
// \date March 2007

#include "rgtl_config.h"

//: Template meta-programming assistance templates.
namespace rgtl
{
  namespace meta
  {
    // Meta-functor implementation details.
    namespace detail
    {
      template <bool Condition> struct if_impl
      {
        template <typename Then, typename Else>
        struct get
        {
          typedef Then type;
        };
      };
      template <> struct if_impl<false>
      {
        template <typename Then, typename Else>
        struct get
        {
          typedef Else type;
        };
      };
    } // namespace rgtl::meta::detail

    // Choose between two types based on a condition.
    template <bool Condition, typename Then, typename Else>
    struct if_
    {
      typedef typename detail::if_impl<Condition>::template get<Then,Else>::type type;
    };

    // The identity type meta-functor.
    template <typename T> struct identity
    {
      typedef T type;
    };

    // A value meta-functor to count the number of one-bits in an integer.
    template <unsigned int combo> struct bits_c
    {
      RGTL_STATIC_CONST(unsigned int,
                        value = (bits_c<(combo>>1)>::value + (combo&1)));
    };
    template <> struct bits_c<0>
    {
      RGTL_STATIC_CONST(unsigned int, value = 0);
    };

    // Helper template to call "T::invoke<F>(arg1)" across compilers.
    // Usage: call1<T,F>::invoke(arg1).
    template <typename T, typename F>
    struct call1
    {
      typedef typename F::arg1_type arg1_type;
      static void invoke(arg1_type arg1)
        {
        T::template invoke<F>(arg1);
        }
    };

    // Helper template to call "T::invoke<F>(arg1, arg2)" across compilers.
    // Usage: call2<T,F>::invoke(arg1, arg2).
    template <typename T, typename F>
    struct call2
    {
      typedef typename F::arg1_type arg1_type;
      typedef typename F::arg2_type arg2_type;
      static void invoke(arg1_type arg1, arg2_type arg2)
        {
        T::template invoke<F>(arg1, arg2);
        }
    };
  } // namespace rgtl::meta
} // namespace rgtl

#endif
