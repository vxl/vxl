# This is contrib/brl/bseg/bmrf/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

ADD_EXECUTABLE ( bmrf_test_driver
  test_driver.cxx
  test_node.cxx
  test_epi_point.cxx
)
TARGET_LINK_LIBRARIES( bmrf_test_driver bmrf testlib vpl )

ADD_TEST( bmrf_node                      bmrf_test_driver test_node)
ADD_TEST( bmrf_epi_point                 bmrf_test_driver test_epi_point)

#this is for test_includes
ADD_EXECUTABLE( bmrf_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bmrf_test_include bmrf)
