# This is contrib/brl/bseg/bvpl/kernels/CMakeLists.txt
# The kernel-related files for Brown Voxel Processing Library (BVPL)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(bvpl_functors_sources
    bvpl_edge_algebraic_mean_functor.h 	bvpl_edge_algebraic_mean_functor.txx
    bvpl_edge_geometric_mean_functor.h 	bvpl_edge_geometric_mean_functor.txx
    bvpl_gauss_convolution_functor.h   	bvpl_gauss_convolution_functor.cxx
    bvpl_positive_gauss_conv_functor.h 	bvpl_positive_gauss_conv_functor.cxx
    bvpl_negative_gauss_conv_functor.h 	bvpl_negative_gauss_conv_functor.cxx
    bvpl_find_surface_functor.h        	bvpl_find_surface_functor.cxx
    bvpl_local_max_functor.h           	bvpl_local_max_functor.cxx
    bvpl_edge2d_functor.h              	bvpl_edge2d_functor.txx
    bvpl_opinion_functor.h             	bvpl_opinion_functor.cxx
    bvpl_algebraic_functor.h           	bvpl_algebraic_functor.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_functors_sources)

ADD_LIBRARY(bvpl_functors ${bvpl_functors_sources})

TARGET_LINK_LIBRARIES(bvpl_functors bvpl_kernels)

#Directory with python/c++ processes
#SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
