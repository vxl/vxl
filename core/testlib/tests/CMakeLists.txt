INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

ADD_EXECUTABLE( testlib_test_driver
   test_driver.cxx

   test_assert.cxx
   test_macros.cxx
   test_args.cxx
   test_root_dir.cxx
)

ADD_TEST( testlib_assert   testlib_test_driver test_assert )
ADD_TEST( testlib_macros   testlib_test_driver test_macros )
ADD_TEST( testlib_args     testlib_test_driver test_args one two )
ADD_TEST( testlib_root_dir testlib_test_driver test_root_dir )
