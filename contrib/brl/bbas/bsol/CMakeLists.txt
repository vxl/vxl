# Additional functions to support vsol
# brl/bbas/bsol/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bsol_sources
    bsol_hough_line_index.cxx   bsol_hough_line_index.h  bsol_hough_line_index_sptr.h
    bsol_algs.cxx               bsol_algs.h
    bsol_distance_histogram.cxx bsol_distance_histogram.h
    bsol_intrinsic_curve_2d.cxx bsol_intrinsic_curve_2d.h bsol_intrinsic_curve_2d_sptr.h
    bsol_point_index_2d.cxx        bsol_point_index_2d.h
	bsol_point_index_3d.cxx        bsol_point_index_3d.h
)
AUX_SOURCE_DIRECTORY(Templates bsol_sources)

ADD_LIBRARY(bsol ${bsol_sources})

TARGET_LINK_LIBRARIES(bsol vsol vgl vnl vbl vul)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
