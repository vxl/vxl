# This is contrib/brl/bseg/bmrf/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

ADD_EXECUTABLE ( bmrf_test_driver
  test_driver.cxx
  test_epi_point.cxx
  test_epi_seg.cxx
  test_node.cxx
  test_network.cxx
  test_network_builder.cxx
)
TARGET_LINK_LIBRARIES( bmrf_test_driver bmrf brip vil testlib vpl )

ADD_TEST( bmrf_epi_point                 bmrf_test_driver test_epi_point)
ADD_TEST( bmrf_epi_seg                   bmrf_test_driver test_epi_seg)
ADD_TEST( bmrf_node                      bmrf_test_driver test_node)
ADD_TEST( bmrf_network                   bmrf_test_driver test_network)
ADD_TEST( bmrf_network_builder           bmrf_test_driver test_network_builder)

#this is for test_includes
ADD_EXECUTABLE( bmrf_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bmrf_test_include bmrf)
