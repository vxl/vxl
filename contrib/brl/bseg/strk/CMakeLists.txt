# segmentation tracking library
# brl/bseg/strk/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

SET(strk_sources
    strk_tracker_params.cxx      strk_tracker_params.h
    strk_tracker.cxx             strk_tracker.h
    strk_info_tracker_params.cxx strk_info_tracker_params.h
    strk_info_tracker.cxx        strk_info_tracker.h
    strk_tracking_face_2d.cxx    strk_tracking_face_2d.h
                                 strk_tracking_face_2d_sptr.h
    strk_art_info_model.cxx    strk_art_info_model.h strk_art_info_model_sptr.h
    strk_info_model_tracker.cxx   strk_info_model_tracker.h
    strk_info_model_tracker_params.cxx strk_info_model_tracker_params.h
    strk_info_model_tracker_process.cxx  strk_info_model_tracker_process.h
    strk_art_model_display_process.cxx  strk_art_model_display_process.h
    strk_corr_tracker_process.cxx    strk_corr_tracker_process.h
    strk_info_tracker_process.cxx    strk_info_tracker_process.h
    strk_track_display_process.cxx   strk_track_display_process.h
    strk_epi_point.cxx               strk_epi_point.h  strk_epi_point_sptr.h
    strk_epi_seg.cxx                 strk_epi_seg.h    strk_epi_seg_sptr.h
    strk_epipolar_grouper_params.cxx strk_epipolar_grouper_params.h
    strk_epipolar_grouper.cxx        strk_epipolar_grouper.h
    strk_epipolar_grouper_process.cxx  strk_epipolar_grouper_process.h
    strk_region_info_params.cxx      strk_region_info_params.h
    strk_region_info.cxx             strk_region_info.h
)
AUX_SOURCE_DIRECTORY(Templates strk_sources)

ADD_LIBRARY(strk ${strk_sources})

TARGET_LINK_LIBRARIES(strk sdet brip vpro btol gevd vdgl vtol vsol vil1 vgl_algo vnl vul vbl)
