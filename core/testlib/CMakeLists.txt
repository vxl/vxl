# ./vxl/testlib/CMakeLists.txt

ADD_LIBRARY(testlib testlib_sources)

FIND_FILE(TESTLIB_VCL_WHERE_ROOT_DIR_H
          vcl_where_root_dir.h
          ${allvxl_BINARY_DIR}/vcl )

IF (TESTLIB_VCL_WHERE_ROOT_DIR_H)
  ADD_DEFINITIONS(-DVCL_WHERE_ROOT_DIR_H_EXISTS)
ENDIF(TESTLIB_VCL_WHERE_ROOT_DIR_H)

SOURCE_FILES( testlib_sources

  # Useful utilities for the tests
  testlib_test.h                testlib_test.cxx

  # For use by the driver program
  testlib_register.h

  # The main function of the driver executable
  testlib_main.cxx

  # Used to locate test files in source tree
  testlib_root_dir.h            testlib_root_dir.cxx
)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
