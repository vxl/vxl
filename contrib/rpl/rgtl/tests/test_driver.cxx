//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <testlib/testlib_register.h>

 DECLARE( test_compact_tree_data_fixed );
 DECLARE( test_octree_data_fixed );
 DECLARE( test_octree_dual_visit );
 DECLARE( test_octree_objects );
 DECLARE( test_sqt_base );

void register_tests()
{
 REGISTER( test_compact_tree_data_fixed );
 REGISTER( test_octree_data_fixed );
 REGISTER( test_octree_dual_visit );
 REGISTER( test_octree_objects );
 REGISTER( test_sqt_base );
}

DEFINE_MAIN;
