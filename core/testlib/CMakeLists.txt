# ./vxl/testlib/CMakeLists.txt

doxygen_add_library(core/testlib
  PACKAGE core-test
  DESCRIPTION "Testing Library"
  )

find_file(TESTLIB_VCL_WHERE_ROOT_DIR_H
          vcl_where_root_dir.h
          ${vxl_BINARY_DIR}/vcl )

if(TESTLIB_VCL_WHERE_ROOT_DIR_H)
  add_definitions(-DVCL_WHERE_ROOT_DIR_H_EXISTS)
endif(TESTLIB_VCL_WHERE_ROOT_DIR_H)

set( testlib_sources

  # Useful utilities for the tests
  testlib_test.h                testlib_test.cxx

  # For use by the driver program
  testlib_register.h

  # The main function of the driver executable
  testlib_main.cxx

  # Used to locate test files in source tree
  testlib_root_dir.h            testlib_root_dir.cxx
)

add_library( testlib ${testlib_sources} )
target_link_libraries( testlib vcl )
install_targets( /lib testlib)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/testlib ${testlib_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
