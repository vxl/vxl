# This is core/vnl/acmalgo/tests/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/FindNetlib.cmake )

# message( NETLIB_FOUND " is " ${NETLIB_FOUND} )
if(NETLIB_FOUND)

  include_directories( ${NETLIB_INCLUDE_DIR} )

  add_executable( vnl_acmalgo_test_all
          test_include.cxx
          test_rpoly_roots.cxx  # Removed due to ACM Copyright
  )

  target_link_libraries( vnl_acmalgo_test_all ${VXL_LIB_PREFIX}vnl_acmalgo ${VXL_LIB_PREFIX}testlib ${CMAKE_THREAD_LIBS} )
  add_test( NAME vnl_acmalgo_test_rpoly_roots COMMAND vnl_acmalgo_test_all test_rpoly_roots             )  # Removed due to ACM Copyright

endif()
