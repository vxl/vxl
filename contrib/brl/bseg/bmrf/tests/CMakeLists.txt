# This is contrib/brl/bseg/bmrf/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

ADD_EXECUTABLE ( bmrf_test_driver
  test_driver.cxx
  test_node.cxx
)
TARGET_LINK_LIBRARIES( bmrf_test_driver bmrf testlib vpl )

ADD_TEST( bmrf_node                      bmrf_test_driver test_node)

#this is for test_includes
ADD_EXECUTABLE( bmrf_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bmrf_test_include bmrf)
