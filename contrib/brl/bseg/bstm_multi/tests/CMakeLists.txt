add_executable( bstm_multi_test_all
    test_driver.cxx
    test_bstm_multi_tree_util.cxx
    test_bstm_multi_block_metadata.cxx
    test_space_time_scene_parser.cxx
    test_space_time_scene.cxx
 )

target_link_libraries( bstm_multi_test_all ${VXL_LIB_PREFIX}testlib boxm2 boxm2_basic bstm bstm_multi boxm2_io bstm_io bstm_basic bstm_cpp_algo)

add_test( NAME bstm_multi_test_bstm_multi_block_metadata COMMAND $<TARGET_FILE:bstm_multi_test_all>  test_bstm_multi_block_metadata )
add_test( NAME bstm_multi_test_bstm_multi_tree_util COMMAND $<TARGET_FILE:bstm_multi_test_all>  test_bstm_multi_tree_util )
add_test( NAME bstm_multi_test_space_time_scene_parser COMMAND $<TARGET_FILE:bstm_multi_test_all>  test_space_time_scene_parser )
add_test( NAME bstm_multi_test_space_time_scene COMMAND $<TARGET_FILE:bstm_multi_test_all>  test_space_time_scene )

add_executable( bstm_multi_test_include test_include.cxx )
target_link_libraries( bstm_multi_test_include bstm_multi)
