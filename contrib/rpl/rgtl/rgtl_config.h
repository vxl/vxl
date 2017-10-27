// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rgtl_config_h
#define rgtl_config_h

//:
// \file
// \brief Configure cross-platform support macros.
// \author Brad King
// \date March 2007

// Macro to define a compile-time constant.
# define RGTL_STATIC_CONST(type, assignment) static type const assignment

// Join two preprocessor symbols into a single symbol.
#define RGTL_PP_JOIN(x,y)  RGTL_PP_JOIN0(x,y)
#define RGTL_PP_JOIN0(x,y) RGTL_PP_JOIN1(x,y)
#define RGTL_PP_JOIN1(x,y) x##y

#endif
