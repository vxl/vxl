# Copyright 2006-2009 Brad King, Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rgtl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

# List tests by name.
set(test_names
  compact_tree_data_fixed
  octree_data_fixed
  octree_dual_visit
  octree_objects
  sqt_base
  )

# Construct list of test sources.
set(test_srcs test_driver.cxx)
foreach(name ${test_names})
  set(test_srcs ${test_srcs} test_${name}.cxx)
endforeach()

# Create the test executable.
add_executable(rgtl_test_all ${test_srcs})
target_link_libraries(rgtl_test_all rgtl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib)

# Add the tests.
foreach(name ${test_names})
  add_test( NAME rgtl_${name} COMMAND $<TARGET_FILE:rgtl_test_all> test_${name})
endforeach()

add_executable( rgtl_test_include test_include.cxx )
target_link_libraries( rgtl_test_include rgtl )
add_executable( rgtl_test_template_include test_template_include.cxx )
target_link_libraries( rgtl_test_template_include rgtl )
