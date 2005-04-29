# This brl/bbas/bugl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE ( bugl_test_all
  test_driver.cxx
  test_bugl.cxx
  test_random_transform_set_2d.cxx
)
TARGET_LINK_LIBRARIES( bugl_test_all bugl testlib )

ADD_TEST( bugl_test_bugl   bugl_test_all test_bugl)
ADD_TEST( bugl_test_random_transform_2d bugl_test_all test_random_transform_2d)

#this is for test_includes
ADD_EXECUTABLE( bugl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bugl_test_include bugl)
