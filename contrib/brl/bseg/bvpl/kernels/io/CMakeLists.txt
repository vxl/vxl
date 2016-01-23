# contrib/brl/bseg/bvpl/io/CMakeLists.txt

set(bvpl_kerneles_io_sources
    bvpl_io_kernel.h       bvpl_io_kernel.cxx
)

aux_source_directory(Templates bvpl_kerneles_io_sources)

add_library(bvpl_kernels_io ${bvpl_kerneles_io_sources})
target_link_libraries(bvpl_kernels_io bvpl_kernels vbl_io vsl vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
