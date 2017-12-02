# gel/vtol/tests/CMakeLists.txt

add_executable( vtol_test_all
  test_driver.cxx
  test_vertex_2d.cxx
  test_zero_chain.cxx
  test_edge_2d.cxx
  test_one_chain.cxx
  test_face_2d.cxx
  test_two_chain.cxx
  test_block.cxx
  test_cycle_processor.cxx
  vtol_test_refcounting.cxx
  vtol_test_timing.cxx

  test_extract_topology.cxx
)
if(CMAKE_COMPILER_IS_GNUCXX)
 set_source_files_properties(test_cycle_processor.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()

target_link_libraries( vtol_test_all vtol_algo vtol vsol vdgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )

add_test( NAME vtol_test_vertex_2d COMMAND $<TARGET_FILE:vtol_test_all> test_vertex_2d)
add_test( NAME vtol_test_zero_chain COMMAND $<TARGET_FILE:vtol_test_all> test_zero_chain)
add_test( NAME vtol_test_edge_2d COMMAND $<TARGET_FILE:vtol_test_all> test_edge_2d)
add_test( NAME vtol_test_one_chain COMMAND $<TARGET_FILE:vtol_test_all> test_one_chain)
add_test( NAME vtol_test_face_2d COMMAND $<TARGET_FILE:vtol_test_all> test_face_2d)
add_test( NAME vtol_test_two_chain COMMAND $<TARGET_FILE:vtol_test_all> test_two_chain)
add_test( NAME vtol_test_block COMMAND $<TARGET_FILE:vtol_test_all> test_block)
add_test( NAME vtol_test_cycle_processor COMMAND $<TARGET_FILE:vtol_test_all> test_cycle_processor)
add_test( NAME vtol_test_refcounting COMMAND $<TARGET_FILE:vtol_test_all> vtol_test_refcounting)
add_test( NAME vtol_test_timing COMMAND $<TARGET_FILE:vtol_test_all> vtol_test_timing)
add_test( NAME vtol_test_extract_topology COMMAND $<TARGET_FILE:vtol_test_all> test_extract_topology)

add_executable( vtol_test_include test_include.cxx )
target_link_libraries( vtol_test_include vtol_algo vtol )
add_executable( vtol_test_template_include test_template_include.cxx )
target_link_libraries( vtol_test_template_include vtol_algo vtol )
