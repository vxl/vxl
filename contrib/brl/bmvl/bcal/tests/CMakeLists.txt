# brl/bmvl/bcal/bcam/tests/CMakeLists.txt

LINK_LIBRARIES(bcal testlib)
ADD_EXECUTABLE(bcal_test bcal_test.cxx)
ADD_TEST(      bcal_test bcal_test ${CMAKE_CURRENT_SOURCE_DIR})
