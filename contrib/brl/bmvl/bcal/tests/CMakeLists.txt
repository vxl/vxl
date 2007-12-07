# brl/bmvl/bcal/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})

ADD_EXECUTABLE(bcal_test_all bcal_test.cxx)
TARGET_LINK_LIBRARIES(bcal_test_all bcal vgl testlib)

ADD_TEST(bcal_test ${EXECUTABLE_OUTPUT_PATH}/bcal_test_all ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(bcal_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bcal_test_include bcal)
