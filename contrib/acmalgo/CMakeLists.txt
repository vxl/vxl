# vxl/vnl/acmalgo/CMakeLists.txt

project(vnl_acmalgo)


include( ${VXL_CMAKE_DIR}/FindNetlib.cmake )

# most of vnl_acmalgo is simply a wrapper around netlib, so we must have netlib.
if(NETLIB_FOUND)

# Direct netlib dependencies are:
# vnl_rpoly_roots                  rpoly_

  include_directories(
    ${NETLIB_INCLUDE_DIR}
    )

  set( vnl_acmalgo_sources
    # equation solvers
          vnl_rpoly_roots.cxx vnl_rpoly_roots.h
          toms/rpoly.c toms/rpoly.h
  )

  # If VXL_INSTALL_INCLUDE_DIR is the default value
  if("${VXL_INSTALL_INCLUDE_DIR}" STREQUAL "include/vxl")
    set(_algo_install_dir ${VXL_INSTALL_INCLUDE_DIR}/core/vnl/acmalgo)
  else()
    set(_algo_install_dir ${VXL_INSTALL_INCLUDE_DIR}/vnl/acmalgo)
  endif()
  vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vnl_acmalgo
    LIBRARY_SOURCES ${vnl_acmalgo_sources}
    HEADER_INSTALL_DIR vnl/acmalgo)
  target_link_libraries( ${VXL_LIB_PREFIX}vnl_acmalgo ${NETLIB_LIBRARIES} ${VXL_LIB_PREFIX}vnl )
  set(CURR_LIB_NAME vnl_acmalgo)
  set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/vnl/acmalgo/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${_algo_install_dir}
     USE_HIDDEN_VISIBILITY
  )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

  if (VXL_BUILD_OBJECT_LIBRARIES)
    add_library(vnl_acmalgo-obj OBJECT ${vnl_acmalgo_sources})
  endif()
endif()
