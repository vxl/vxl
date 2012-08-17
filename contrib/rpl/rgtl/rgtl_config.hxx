// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_config_hxx
#define rgtl_config_hxx

//:
// \file
// \brief Configure cross-platform support macros.
// \author Brad King
// \date March 2007

// We need typename in a few places that old MSVC does not like.
#if defined(_MSC_VER) && _MSC_VER <= 1300
# define RGTL_TYPENAME
#else
# define RGTL_TYPENAME typename
#endif

// Macro to define a compile-time constant.
#if defined(_MSC_VER) && _MSC_VER <= 1300 || defined(__BORLANDC__)
# define RGTL_STATIC_CONST(type, assignment) enum { assignment }
#else
# define RGTL_STATIC_CONST(type, assignment) static type const assignment
#endif

// Join two preprocessor symbols into a single symbol.
#define RGTL_PP_JOIN(x,y)  RGTL_PP_JOIN0(x,y)
#define RGTL_PP_JOIN0(x,y) RGTL_PP_JOIN1(x,y)
#define RGTL_PP_JOIN1(x,y) x##y

#endif
