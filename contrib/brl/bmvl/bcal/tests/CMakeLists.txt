# brl/bmvl/bcal/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bmvl)

LINK_LIBRARIES(bcal testlib)

ADD_EXECUTABLE(bcal_test_all bcal_test.cxx)
ADD_TEST(bcal_test bcal_test_all ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(bcal_test_include test_include.cxx)
