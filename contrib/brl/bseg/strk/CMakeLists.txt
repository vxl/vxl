# segmentation tracking library
# brl/bseg/strk/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/strk
  DEPENDS contrib/brl/bseg/vpro
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

SET(strk_sources
    strk_tracker_params.cxx              strk_tracker_params.h
    strk_tracker.cxx                     strk_tracker.h
    strk_info_tracker_params.cxx         strk_info_tracker_params.h
    strk_info_tracker.cxx                strk_info_tracker.h
    strk_tracking_face_2d.cxx            strk_tracking_face_2d.h
                                         strk_tracking_face_2d_sptr.h
    strk_art_info_model.cxx              strk_art_info_model.h strk_art_info_model_sptr.h
    strk_info_model_tracker.cxx          strk_info_model_tracker.h
    strk_info_model_tracker_params.cxx   strk_info_model_tracker_params.h
    strk_info_model_tracker_process.cxx  strk_info_model_tracker_process.h
    strk_art_model_display_process.cxx   strk_art_model_display_process.h
    strk_corr_tracker_process.cxx        strk_corr_tracker_process.h
    strk_info_tracker_process.cxx        strk_info_tracker_process.h
    strk_track_display_process.cxx       strk_track_display_process.h
    strk_region_info_params.cxx          strk_region_info_params.h
    strk_region_info.cxx                 strk_region_info.h
    strk_io.cxx                          strk_io.h
    strk_feature_capture_process.cxx     strk_feature_capture_process.h
)
AUX_SOURCE_DIRECTORY(Templates strk_sources)

IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
SET(strk_sources ${strk_sources}
    strk_snippet_extractor_process.cxx   strk_snippet_extractor_process.h
)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)

ADD_LIBRARY(strk ${strk_sources})
INSTALL_TARGETS(/lib strk)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/strk ${strk_sources})

TARGET_LINK_LIBRARIES(strk bsta brip vpro bsol btol gevd vtol vsol vil1 vgl_algo vgl vnl vbl vul)
IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  # vidl_vil1 is only used by strk_snippet_extractor_process
  TARGET_LINK_LIBRARIES(strk vidl_vil1)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
