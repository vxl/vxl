/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_static_assert_hxx
#define rgtl_static_assert_hxx

//:
// \file
// \brief Macro to enforce compile-time conditions in a template.
// \author Brad King
// \date March 2007

#include "rgtl_config.hxx"

// Macro for use inside a template to enforce a certain condition at
// compile time.  This is adapted from boost.
#if defined(__BORLANDC__)
// Borland has problems with angle-brackets in conditions.
# define RGTL_STATIC_ASSERT(condition) enum { \
    RGTL_PP_JOIN(rgtl_static_assert_condition_, __LINE__) = (condition), \
    RGTL_PP_JOIN(rgtl_static_assert_, __LINE__) = \
      sizeof(::rgtl::STATIC_ASSERTION_FAILURE< \
             (bool)RGTL_PP_JOIN(rgtl_static_assert_condition_, __LINE__) \
             >) \
  }
#else
# define RGTL_STATIC_ASSERT(condition) \
  RGTL_STATIC_CONST(unsigned int, \
    RGTL_PP_JOIN(rgtl_static_assert_, __LINE__) = \
      sizeof(::rgtl::STATIC_ASSERTION_FAILURE<(bool)(condition)>))
#endif
#define RGTL_STATIC_ASSERT_USE() \
  static_cast<void>(RGTL_PP_JOIN(rgtl_static_assert_, __LINE__))
namespace rgtl
{
template <bool Condition> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> {};
template <> struct STATIC_ASSERTION_FAILURE<false>;
}

#endif
