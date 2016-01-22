# This is mul/fhs/tests/CMakeLists.txt

ADD_EXECUTABLE( fhs_test_all
  test_driver.cxx

  test_arc.cxx
  test_searcher.cxx
)
TARGET_LINK_LIBRARIES( fhs_test_all fhs testlib )

ADD_TEST( NAME fhs_test_arc COMMAND $<TARGET_FILE:fhs_test_all> test_arc )
ADD_TEST( NAME fhs_test_searcher COMMAND $<TARGET_FILE:fhs_test_all> test_searcher )

ADD_EXECUTABLE( fhs_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( fhs_test_include fhs )
