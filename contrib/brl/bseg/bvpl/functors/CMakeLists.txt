# This is contrib/brl/bseg/bvpl/kernels/CMakeLists.txt
# The kernel-related files for Brown Voxel Processing Library (BVPL)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(bvpl_functors_sources
    bvpl_edge_algebraic_mean_functor.h         bvpl_edge_algebraic_mean_functor.hxx
    bvpl_edge_geometric_mean_functor.h         bvpl_edge_geometric_mean_functor.hxx
    bvpl_gauss_convolution_functor.h           bvpl_gauss_convolution_functor.cxx
    bvpl_positive_gauss_conv_functor.h         bvpl_positive_gauss_conv_functor.cxx
    bvpl_negative_gauss_conv_functor.h         bvpl_negative_gauss_conv_functor.cxx
    bvpl_find_surface_functor.h                bvpl_find_surface_functor.cxx
    bvpl_local_max_functor.h                   bvpl_local_max_functor.cxx
    bvpl_edge2d_functor.h                      bvpl_edge2d_functor.hxx
    bvpl_opinion_functor.h                     bvpl_opinion_functor.cxx
    bvpl_algebraic_functor.h                   bvpl_algebraic_functor.cxx
   )

aux_source_directory(Templates bvpl_functors_sources)

vxl_add_library(LIBRARY_NAME bvpl_functors LIBRARY_SOURCES ${bvpl_functors_sources})

target_link_libraries(bvpl_functors bvpl_kernels)

#Directory with python/c++ processes
#add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
