# Copyright 2006-2009 Brad King, Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rgtl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

# List tests by name.
SET(test_names
  compact_tree_data_fixed
  octree_data_fixed
  octree_dual_visit
  octree_objects
  sqt_base
  )

# Construct list of test sources.
SET(test_srcs test_driver.cxx)
FOREACH(name ${test_names})
  SET(test_srcs ${test_srcs} test_${name}.cxx)
ENDFOREACH(name)

# Create the test executable.
ADD_EXECUTABLE(rgtl_test_all ${test_srcs})
TARGET_LINK_LIBRARIES(rgtl_test_all rgtl vnl vul testlib)

# Add the tests.
FOREACH(name ${test_names})
  ADD_TEST(rgtl_${name} ${EXECUTABLE_OUTPUT_PATH}/rgtl_test_all test_${name})
ENDFOREACH(name)

ADD_EXECUTABLE( rgtl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rgtl_test_include rgtl )
ADD_EXECUTABLE( rgtl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( rgtl_test_template_include rgtl )
