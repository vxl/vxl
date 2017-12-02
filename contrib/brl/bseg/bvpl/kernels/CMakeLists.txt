# This is contrib/brl/bseg/bvpl/kernels/CMakeLists.txt
# The kernel-related files for Brown Voxel Processing Library (BVPL)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(bvpl_kernels_sources
    bvpl_kernel.h                         bvpl_kernel.cxx
    bvpl_kernel_iterator.h
    bvpl_create_directions.h              bvpl_create_directions.cxx
    bvpl_taylor_basis_factory.h           bvpl_taylor_basis_factory.cxx
    bvpl_kernel_factory.h                 bvpl_kernel_factory.cxx
    bvpl_edge2d_kernel_factory.h          bvpl_edge2d_kernel_factory.cxx
    bvpl_edge3d_kernel_factory.h          bvpl_edge3d_kernel_factory.cxx
    bvpl_gauss3d_kernel_factory.h         bvpl_gauss3d_kernel_factory.cxx
    bvpl_gauss3d_x_kernel_factory.h       bvpl_gauss3d_x_kernel_factory.cxx
    bvpl_gauss3d_xx_kernel_factory.h      bvpl_gauss3d_xx_kernel_factory.cxx
    bvpl_corner2d_kernel_factory.h        bvpl_corner2d_kernel_factory.cxx
    bvpl_weighted_cube_kernel_factory.h   bvpl_weighted_cube_kernel_factory.cxx
    bvpl_neighborhood_kernel_factory.h    bvpl_neighborhood_kernel_factory.cxx
   )

aux_source_directory(Templates bvpl_kernels_sources)

vxl_add_library(LIBRARY_NAME bvpl_kernels LIBRARY_SOURCES ${bvpl_kernels_sources})
target_link_libraries(bvpl_kernels bxml bsvg bsta ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

#Directory with python/c++ processes
add_subdirectory(io)
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
