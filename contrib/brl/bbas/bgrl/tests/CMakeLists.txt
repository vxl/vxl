# This brl/bbas/bgrl/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR} )

add_executable( bgrl_test_all
  test_driver.cxx
  test_graph.cxx
  test_search.cxx
)
target_link_libraries( bgrl_test_all bgrl vpl vbl_io vsl testlib )

add_test( NAME bgrl_test_graph COMMAND $<TARGET_FILE:bgrl_test_all> test_graph )
add_test( NAME bgrl_test_search COMMAND $<TARGET_FILE:bgrl_test_all> test_search )

add_executable( bgrl_test_include test_include.cxx)
target_link_libraries( bgrl_test_include bgrl)
