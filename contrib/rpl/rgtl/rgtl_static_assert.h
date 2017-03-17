/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_static_assert_h
#define rgtl_static_assert_h

//:
// \file
// \brief Macro to enforce compile-time conditions in a template.
// \author Brad King
// \date March 2007

#include "rgtl_config.h"

// Macro for use inside a template to enforce a certain condition at
// compile time.  This is adapted from boost.
#define RGTL_STATIC_ASSERT(condition) \
  RGTL_STATIC_CONST(unsigned int, \
    RGTL_PP_JOIN(rgtl_static_assert_, __LINE__) = \
      sizeof(::rgtl::STATIC_ASSERTION_FAILURE<(bool)(condition)>))
#define RGTL_STATIC_ASSERT_USE() \
  static_cast<void>(RGTL_PP_JOIN(rgtl_static_assert_, __LINE__))
namespace rgtl
{
template <bool Condition> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> {};
template <> struct STATIC_ASSERTION_FAILURE<false>;
}

#endif
