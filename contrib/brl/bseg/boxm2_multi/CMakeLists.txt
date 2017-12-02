if(HAS_GEOTIFF)
# contrib/brl/bseg/boxm2/CMakeLists.txt
#set( boxm2_MULTI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )


#depends on OPENCL being found...
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${MUL_INCLUDE_DIR} )

set(boxm2_multi_sources
    boxm2_multi_cache.h         boxm2_multi_cache.cxx
    boxm2_multi_cache_group.h   boxm2_multi_cache_group.cxx
    boxm2_multi_util.h          boxm2_multi_util.cxx
   )

vxl_add_library(LIBRARY_NAME boxm2_multi LIBRARY_SOURCES  ${boxm2_multi_sources})
target_link_libraries(boxm2_multi boxm2 boxm2_ocl)

#multi gpu implementations
add_subdirectory(algo)

#executables
add_subdirectory(exe)

##executables
add_subdirectory(pro)
#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/boxm2_multi: requires geotiff")
endif()
