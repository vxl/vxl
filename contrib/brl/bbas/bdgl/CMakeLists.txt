# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
SET(bdgl_sources
    bdgl_curve_algs.cxx        bdgl_curve_algs.h
    bdgl_curve_description.cxx bdgl_curve_description.h

    bdgl_curve_tracker_primitive.cxx bdgl_curve_tracker_primitive.h
    bdgl_tracker_curve.cxx bdgl_tracker_curve.h
	bdgl_tracker_curve_sptr.h 
	bdgl_curve_tracking.cxx bdgl_curve_tracking.h
	bdgl_curve_matching.cxx bdgl_curve_matching.h
	bdgl_curve_clustering.cxx bdgl_curve_clustering.h
	curve.h		curve.cxx 
	DPMatch.h	DPMatch.cxx 
	curvematch.h
	utils.h		utils.cxx
)
AUX_SOURCE_DIRECTORY(Templates bdgl_sources)

ADD_LIBRARY(bdgl ${bdgl_sources})


TARGET_LINK_LIBRARIES(bdgl vbl vdgl vtol vsol vil vnl btol mvl mbl)


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
