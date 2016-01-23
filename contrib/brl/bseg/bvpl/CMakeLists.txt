# This is contrib/brl/bseg/bvpl/CMakeLists.txt
project(bvpl)

include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(bvpl_sources
    bvpl_voxel_subgrid.h               bvpl_voxel_subgrid.txx
    bvpl_subgrid_iterator.h            bvpl_subgrid_iterator.txx
    bvpl_neighb_operator.h
    bvpl_combined_neighb_operator.h
    bvpl_vector_operator.h
    bvpl_discriminative_non_max_suppression.h
    bvpl_direction_to_color_map.h      bvpl_direction_to_color_map.cxx
    bvpl_susan_opinion_operator.h
   )

aux_source_directory(Templates bvpl_sources)

add_library(bvpl ${bvpl_sources})

target_link_libraries(bvpl bvpl_kernels bvpl_functors bvxm_grid bdgl bsta vnl vnl_io vil_algo vgl vpl vul vbl_io vsl vbl)

add_subdirectory(pro)
add_subdirectory(util)
add_subdirectory(bvpl_octree)
add_subdirectory(kernels)
add_subdirectory(functors)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
