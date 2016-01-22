SET( bocl_test_sources
  test_driver.cxx
  test_global_io_bandwidth.cxx
  test_command_queue.cxx
  test_kernel.cxx
  bocl_global_memory_bandwidth_manager.h bocl_global_memory_bandwidth_manager.cxx
  bocl_command_queue_mgr.h  bocl_command_queue_mgr.cxx
  test_local_mem_access.cxx
  test_mem.cxx
  )

AUX_SOURCE_DIRECTORY(Templates bocl_test_sources)
ADD_EXECUTABLE(bocl_test_all ${bocl_test_sources})

TARGET_LINK_LIBRARIES( bocl_test_all bocl testlib vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)

ADD_TEST( bocl_test_global_io_bandwidth    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bocl_test_all test_global_io_bandwidth  )
if( HACK_FORCE_BRL_FAILING_TESTS ) ## This test is always segmentation faulting
  ADD_TEST( bocl_test_command_queue          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bocl_test_all test_command_queue  )
endif()
ADD_TEST( bocl_test_kernel                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bocl_test_all test_kernel )
ADD_TEST( bocl_test_local_mem_access       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bocl_test_all test_local_mem_access )
ADD_TEST( bocl_test_mem                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bocl_test_all test_mem )

ADD_EXECUTABLE( bocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bocl_test_include bocl)
ADD_EXECUTABLE( bocl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bocl_test_template_include bocl)
