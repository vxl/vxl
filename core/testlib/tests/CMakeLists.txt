ADD_EXECUTABLE( testlib_test_driver
   test_driver.cxx

   test_assert.cxx
   test_macros.cxx
   test_args.cxx
   test_root_dir.cxx
)
TARGET_LINK_LIBRARIES( testlib_test_driver testlib )

ADD_EXECUTABLE( testlib_test_link
   test_link.cxx
)
TARGET_LINK_LIBRARIES( testlib_test_link testlib )

ADD_TEST( testlib_assert     testlib_test_driver test_assert )
ADD_TEST( testlib_macros     testlib_test_driver test_macros )
ADD_TEST( testlib_args       testlib_test_driver test_args one two )
ADD_TEST( testlib_root_dir   testlib_test_driver test_root_dir )
ADD_TEST( testlib_test_link  testlib_test_link )
