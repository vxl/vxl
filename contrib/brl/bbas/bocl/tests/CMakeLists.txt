set( bocl_test_sources
  test_driver.cxx
  test_global_io_bandwidth.cxx
  test_command_queue.cxx
  test_kernel.cxx
  bocl_global_memory_bandwidth_manager.h bocl_global_memory_bandwidth_manager.cxx
  bocl_command_queue_mgr.h  bocl_command_queue_mgr.cxx
  test_local_mem_access.cxx
  test_mem.cxx
  )

aux_source_directory(Templates bocl_test_sources)
add_executable(bocl_test_all ${bocl_test_sources})

target_link_libraries( bocl_test_all bocl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)

add_test( NAME bocl_test_global_io_bandwidth COMMAND $<TARGET_FILE:bocl_test_all> test_global_io_bandwidth  )
if( VXL_RUN_FAILING_TESTS ) ## This test is always segmentation faulting
  add_test( NAME bocl_test_command_queue COMMAND $<TARGET_FILE:bocl_test_all> test_command_queue  )
endif()
add_test( NAME bocl_test_kernel COMMAND $<TARGET_FILE:bocl_test_all> test_kernel )
add_test( NAME bocl_test_local_mem_access COMMAND $<TARGET_FILE:bocl_test_all> test_local_mem_access )
add_test( NAME bocl_test_mem COMMAND $<TARGET_FILE:bocl_test_all> test_mem )

add_executable( bocl_test_include test_include.cxx )
target_link_libraries( bocl_test_include bocl)
add_executable( bocl_test_template_include test_template_include.cxx )
target_link_libraries( bocl_test_template_include bocl)
