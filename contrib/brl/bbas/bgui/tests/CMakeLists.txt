# brl/bbas/bgui/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

#LINK_LIBRARIES(bgui vil testlib vcl)

# not quite ready for this yet and need to kill test failures
#ADD_EXECUTABLE(bgui_test_all
#   test_driver.cxx
#   test_histogram.cxx
#)
#  ADD_TEST( bgui_test_histogram  bgui_test_all test_histogram )	


