# brl/bbas/bugl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

LINK_LIBRARIES(bugl vbl testlib)

ADD_EXECUTABLE(bugl_test_all test_bugl.cxx)
ADD_TEST(bugl_test bugl_test_all)

ADD_EXECUTABLE(bugl_test_include test_include.cxx)
