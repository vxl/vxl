# 3D reconstruction from multiple views library
# brl/bmvl/brct/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bmvl)
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${RPL_INCLUDE_DIR} )
SET(brct_sources
    brct_algos.h                  brct_algos.cpp
    kalman_filter.h               kalman_filter.cpp
    brct_structure_estimator.h    brct_structure_estimator.cxx
    brct_epi_reconstructor.h      brct_epi_reconstructor.cxx
    brct_plane_calibrator.h       brct_plane_calibrator.cxx
    brct_corr.h                   brct_corr.cxx          
    brct_plane_corr.h             brct_plane_corr.cxx    brct_plane_corr_sptr.h
    brct_plane_sweeper_params.h   brct_plane_sweeper_params.cxx      
    brct_plane_sweeper.h          brct_plane_sweeper.cxx      
    brct_volume_processor_params.h    brct_volume_processor_params.cxx
    brct_volume_processor.h       brct_volume_processor.cxx
#    brct_dense_reconstructor.h    brct_dense_reconstructor.cxx
)
AUX_SOURCE_DIRECTORY(Templates brct_sources)

ADD_LIBRARY(brct ${brct_sources})


TARGET_LINK_LIBRARIES(brct sdet brip rrel bsol gevd vdgl vtol vsol mvl vil1 vgl_algo vgl vnl vbl bdgl bugl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
