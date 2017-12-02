# This is contrib/brl/bseg/bvpl/CMakeLists.txt
if (HAS_GEOTIFF)
project(bvpl)

include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(bvpl_sources
    bvpl_voxel_subgrid.h               bvpl_voxel_subgrid.hxx
    bvpl_subgrid_iterator.h            bvpl_subgrid_iterator.hxx
    bvpl_neighb_operator.h
    bvpl_combined_neighb_operator.h
    bvpl_vector_operator.h
    bvpl_discriminative_non_max_suppression.h
    bvpl_direction_to_color_map.h      bvpl_direction_to_color_map.cxx
    bvpl_susan_opinion_operator.h
   )

aux_source_directory(Templates bvpl_sources)

vxl_add_library(LIBRARY_NAME bvpl LIBRARY_SOURCES ${bvpl_sources})

target_link_libraries(bvpl bvpl_kernels bvpl_functors bvxm_grid bdgl bsta ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl)

add_subdirectory(pro)
add_subdirectory(util)
add_subdirectory(bvpl_octree)
add_subdirectory(kernels)
add_subdirectory(functors)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/bvpl: requires geotiff")
endif()
