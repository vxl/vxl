# This is contrib/brl/bseg/bvpl/kernels/CMakeLists.txt
# The kernel-related files for Brown Voxel Processing Library (BVPL)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(bvpl_kernels_sources
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

AUX_SOURCE_DIRECTORY(Templates bvpl_kernels_sources)

ADD_LIBRARY(bvpl_kernels ${bvpl_kernels_sources})
TARGET_LINK_LIBRARIES(bvpl_kernels bxml bsvg bsta vgl_algo vgl vnl vbl)

#Directory with python/c++ processes
SUBDIRS( io )
SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)


INSTALL_TARGETS(/lib bvpl_kernels)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bvpl/kernels/ ${bvpl_kernels_sources})
