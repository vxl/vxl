# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(bdgl_sources
   
    bdgl_curve_algs.cxx              bdgl_curve_algs.h
    bdgl_curve_description.cxx       bdgl_curve_description.h
    bdgl_curve_tracker_primitive.cxx bdgl_curve_tracker_primitive.h
    bdgl_tracker_curve.cxx           bdgl_tracker_curve.h  bdgl_tracker_curve_sptr.h
    bdgl_curve_tracking.cxx          bdgl_curve_tracking.h
    bdgl_curve_matching.cxx          bdgl_curve_matching.h
    bdgl_curve_clustering.cxx        bdgl_curve_clustering.h
    bdgl_curve_matcher.cxx           bdgl_curve_matcher.h
    bdgl_curve_region.cxx            bdgl_curve_region.h
    bdgl_curve_tracker.cxx           bdgl_curve_tracker.h
    curve.cxx                        curve.h
    DPMatch.cxx                      DPMatch.h
                                     curveMatch.h
    utils.cxx                        utils.h
)
AUX_SOURCE_DIRECTORY(Templates bdgl_sources)

ADD_LIBRARY(bdgl ${bdgl_sources})

TARGET_LINK_LIBRARIES(bdgl vtol vdgl vsol vgl_algo vgl vnl_algo vnl mvl mbl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
