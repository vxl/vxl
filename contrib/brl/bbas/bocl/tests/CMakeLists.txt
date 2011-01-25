ADD_EXECUTABLE( bocl_test_all
  test_driver.cxx
  test_global_io_bandwidth.cxx
  test_command_queue.cxx
  )

TARGET_LINK_LIBRARIES( bocl_test_all bocl testlib vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)

ADD_TEST( bocl_test_global_io_bandwidth    ${EXECUTABLE_OUTPUT_PATH}/bocl_test_all test_global_io_bandwidth  )
ADD_TEST( bocl_test_command_queue          ${EXECUTABLE_OUTPUT_PATH}/bocl_test_all test_command_queue  )

ADD_EXECUTABLE( bocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bocl_test_include bocl)
ADD_EXECUTABLE( bocl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bocl_test_template_include bocl)
