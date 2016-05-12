# contrib/brl/bseg/bvpl/io/CMakeLists.txt

set(bvpl_kerneles_io_sources
    bvpl_io_kernel.h       bvpl_io_kernel.cxx
)

aux_source_directory(Templates bvpl_kerneles_io_sources)

vxl_add_library(LIBRARY_NAME bvpl_kernels_io LIBRARY_SOURCES ${bvpl_kerneles_io_sources})
target_link_libraries(bvpl_kernels_io bvpl_kernels ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
